#include "StdAfx.h"

#include "EotResultNode.h"
#include "EotResultNode_AutoGen.cpp"

#include "EotResultNodeParam.h"

using namespace stc::framework;

CEotResultNode::CEotResultNode():
	m_canSave(true)
{
}

CEotResultNode::~CEotResultNode()
{
}


void CEotResultNode::SetResultNodeParameter(
	const std::string& name, 
	const std::string& value)
{
	ScriptableVec params;
	GetObjects(params, CEotResultNodeParam::CLASS_ID());
	for (ScriptableVec::const_iterator it = params.begin(); it != params.end(); ++it)
	{
		CEotResultNodeParam* param = dynamic_cast<CEotResultNodeParam*>(*it);
		assert(param);

		if (param->GetName() == name)
		{
			param->SetParamValue(value);
			return;
		}
	}

	// Not found, create a new one
	CScriptableCreator ctor;
	CScriptableAutoPtr<CEotResultNodeParam> 
		param(ctor.CreateAPT<CEotResultNodeParam>(this));

	param->SetName(name);
	param->SetParamValue(value);
	param.release();
}

