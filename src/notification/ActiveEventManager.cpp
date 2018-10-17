#include "StdAfx.h"

#include "ActiveEventManager.h"
#include "ActiveEventManager_AutoGen.cpp"
#include "StcSystem.h"
#include "BLLLoggerFactory.h"

#include <cassert>

using namespace stc::framework;

// development and user category/logger
DEFINE_STATIC_LOGGER("user.event", userEventLogger);

CActiveEventManager& CActiveEventManager::Instance()
{
	static CActiveEventManager* m = 0;

	if (m == 0)
	{
		m = dynamic_cast<CActiveEventManager*>
			(CStcSystem::Instance().GetObject(FRAMEWORK_ActiveEventManager));
	}

	return *m;
}

CActiveEventManager::CActiveEventManager()
{
}

CActiveEventManager::~CActiveEventManager()
{
}

bool CActiveEventManager::AddActiveEvent(CActiveEvent* actEvent)
{
	assert(actEvent);
		
    if (AddObject(*actEvent))
    {
        // TODO: Skip logging configuration error/warning later.
        // For now, GUI wants to log even configuration error/warning
        // in the user log; since there is no active event viewer in the GUI now.
        //
        //if (actEvent->GetCategory() == "Validation")
        //{
        //}

        switch (actEvent->GetLevel())
        {
        case CActiveEvent::EnumLevel_DEBUG: 
            LOG_DEBUG(userEventLogger(), actEvent->GetMessage()); break;
        case CActiveEvent::EnumLevel_INFO: 
            LOG_INFO(userEventLogger(), actEvent->GetMessage()); break;

        // UI wants these two to be logged as INFO instead...
        case CActiveEvent::EnumLevel_WARN: 
            LOG_INFO(userEventLogger(), actEvent->GetMessage()); break;
        case CActiveEvent::EnumLevel_ERROR: 
            LOG_INFO(userEventLogger(), actEvent->GetMessage()); break;
        }

        return true;
    }

    return false;
}

