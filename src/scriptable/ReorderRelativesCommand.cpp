#include "StdAfx.h"

#include "ReorderRelativesCommand.h"
#include "ReorderRelativesCommand_AutoGen.cpp"
#include "RelativesCommandsHelpers.h"

using namespace stc::framework;

CReorderRelativesCommand::CReorderRelativesCommand()
{
}

CReorderRelativesCommand::~CReorderRelativesCommand()
{
}

void CReorderRelativesCommand::DoRun()
{
	CHandleRegistry & hr = CHandleRegistry::Instance();
	CScriptable * source = hr.Find(GetSource());
	if ( source == NULL )
		throw CStcInvalidArgument("Invalid Object Handle");

	RelationType relationType;
	relationType.dir = GetRelationTypeDirection(GetDirection());
	if ( relationType.dir == RelationType::DIR_INVALID )
		throw CStcInvalidArgument("Invalid Relation Direction");

	relationType.id = GetRelationTypeId(GetRelationType());
	if ( relationType.id == NULL_RELATION_TYPE_ID() )
		throw CStcInvalidArgument("Invalid Relation Type");

	HandleVec hvec;
	ScriptableVec svec;	
	GetTargets().Get(hvec);
	hr.Find(hvec, svec);

	source->SetObjects(svec, relationType, true);
}
