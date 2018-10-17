#include "StdAfx.h"

#include "Command.h"
#include "ScriptableCreator.h"
#include "Scriptable.h"
#include "NotificationService.h"
#include "TaskManager.h"
#include "StcSystem.h"

using namespace stc::framework;

/**
	Creator is an util class for defering/batching callbacks events.

	The nested level is used to batch up creators on the same stack,
	so events are only fired at the end of the last level.

 */

CScriptableCreator::ThreadCtx& CScriptableCreator::GetThreadCtx()
{
	typedef std::map<CThread::ID, ThreadCtx> ThreadCtxMap;
	static ThreadCtxMap s_ctxMap;

	const CThread::ID tid = CThread::GetCurrentThreadId();

	ThreadCtxMap::iterator it = s_ctxMap.find(tid);
	if (it == s_ctxMap.end())
	{
		ThreadCtx c;
		c.nestedLevel = 0;
		it = s_ctxMap.insert(std::make_pair(tid, c)).first;
	}

	return it->second;
}


CScriptableCreator::CScriptableCreator()
{
	ThreadCtx& c = GetThreadCtx();
	c.nestedLevel++;
	assert(c.nestedLevel > 0);
}

/**
	Generates create events for created objects upon deletion of creator.
 */
CScriptableCreator::~CScriptableCreator()
{
	static CHandleRegistry& hndReg = CHandleRegistry::Instance();
	static CNotificationService& ns = CNotificationService::Instance();

	ThreadCtx& c = GetThreadCtx();
	c.nestedLevel--;
	assert(c.nestedLevel >= 0);

	if ( c.nestedLevel == 0 )
	{
		CTaskManager::CtmYielder yielder(100);
		HandleList::const_iterator it;
		for (it = c.createdHndList.begin(); it != c.createdHndList.end(); ++it)
		{
			ns.UnblockEvents(*it);

			// Could be deleted before reaching the end of the creator, so
			// check for it.
			CScriptable* s = hndReg.Find(*it);
			if (s && !s->IsDeleted())
			{
				yielder.CtmYield();
				ns.WriteEvent(EVT_CREATE, s);
			}
		}
		c.createdHndList.clear();
	}
}

/**
	Returns 0 if the create failed.
 */
CScriptable* CScriptableCreator::Create
(
	ClassId id, 				///< the class we wish to create.
	CScriptable* prntObj,		///< the parent object to attach to.
	bool genEvent,				///< whether to generate a create event; default true
	bool createDefaultChlild,	///< whether to create default children; default true
	StcHandle handle			///< user managed handle; default 0 (BLL managed)
)
{
	CMetaClassManager& mcm = CMetaClassManager::Instance();
	return Create(mcm.GetClass(id), prntObj, genEvent, createDefaultChlild, handle);
}

/**
	Returns 0 if the create failed.
 */
CScriptable* CScriptableCreator::Create
(
	const std::string& clsName,		///< the name of the class we wish to create.
	CScriptable* prntObj,		///< the parent object to attach to.
	bool genEvent,				///< whether to generate a create event; default true
	bool createDefaultChlild,	///< whether to create default children; default true
	StcHandle handle			///< user managed handle; default 0 (BLL managed)
)
{
	CMetaClassManager& mcm = CMetaClassManager::Instance();

    const CMetaClass* pparentmc  = prntObj ? &(prntObj->GetMetaClass()) : 0;
	CMetaClass* pmetaclass = mcm.GetClass(clsName, pparentmc, true);

    if (NULL == pmetaclass)
    {
        std::string errorMsg = CMetaClassManager::GetSuggestions(clsName, true);
        throw CStcException(errorMsg);
    }
    else
    {
        if(CStcSystem::Instance().IsLoadingFromConfiguration() && (pmetaclass->GetClassId().GetPluginId() == 0x17000000))
        {
            std::string warningMsg = "skip to create: " + clsName;
            throw CStcException(warningMsg, false);
        }
    }


	return Create(pmetaclass, prntObj, genEvent, createDefaultChlild, handle);
}

void CScriptableCreator::Remove
(
	const CScriptable* s	///< object to be removed
)
{
	if (s)
	{
		ThreadCtx& c = GetThreadCtx();
		assert(c.nestedLevel > 0);
		c.createdHndList.remove(s->GetObjectHandle());
	}
}


CScriptable* CScriptableCreator::Create
(
	CMetaClass* cls,			///< the class's metaclass we wish to create.
	CScriptable* prntObj,		///< the parent object to attach to.
	bool genEvent,				///< whether to generate a create event; default true
	bool createDefaultChlild,	///< whether to create default children; default true
	StcHandle handle			///< user managed handle; default 0 (BLL managed)
)
{
	if (cls)
	{
		ThreadCtx& c = GetThreadCtx();
		assert(c.nestedLevel > 0);

		// Since Make might create new objects use ScriptableCreator,
		// we will record the handle list
		// so we can maintain the correct create order.
		HandleList hndList;
		hndList.swap(c.createdHndList);

		CScriptable* s = cls->Make(prntObj, handle, createDefaultChlild, genEvent);
		assert(s && "cls->Make should throw on error");

        hndList.push_back(s->GetObjectHandle());
		hndList.splice(hndList.end(), c.createdHndList);
		c.createdHndList.swap(hndList);

		return s;
	}

	throw CStcException("Trying to create unknown class");
}

stc::framework::CCommand* CScriptableCreator::CreateCommand(const std::string& clsName)
{
    std::string cmdName(StringToLower(clsName));
    if(StringUtil::EndsWith(cmdName, "command") == false)
    {
        cmdName += "command";
    }

    CCommand* cmd = dynamic_cast<CCommand*> ( Create(cmdName, &CStcSystem::Instance(), true, true, NULL_STCHANDLE) );
    if(cmd == NULL)
    {
        std::ostringstream msg;
        msg << clsName << " could not be converted to a CCommand";
        throw stc::framework::CStcRunTimeError(msg.str());
    }

    return cmd;
}

