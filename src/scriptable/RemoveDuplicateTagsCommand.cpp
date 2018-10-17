#include "StdAfx.h"

#include "RemoveDuplicateTagsCommand.h"
#include "RemoveDuplicateTagsCommand_AutoGen.cpp"

#include "Tags.h"

using namespace stc::framework;

CRemoveDuplicateTagsCommand::CRemoveDuplicateTagsCommand()
{
}

CRemoveDuplicateTagsCommand::~CRemoveDuplicateTagsCommand()
{
}

void CRemoveDuplicateTagsCommand::DoRun()
{
    bool result = CTags::Instance().RemoveDuplicateTags();
    SetFoundDuplicates(result);
}
