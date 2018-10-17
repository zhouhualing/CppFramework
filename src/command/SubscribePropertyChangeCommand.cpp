#include "StdAfx.h"
#include "SubscribePropertyChangeCommand.h"
#include "SubscribePropertyChangeCommand_AutoGen.cpp"
#include "HttpNotificationService.h"

using namespace std;
using namespace stc::framework;

CSubscribePropertyChangeCommand::CSubscribePropertyChangeCommand()
{

}

CSubscribePropertyChangeCommand::~CSubscribePropertyChangeCommand()
{

}

void CSubscribePropertyChangeCommand::DoRun()
{
    CollectionProperty<PropId>& propIds = GetPropertyIdList();
    for (int i = 0; i < propIds.Count(); ++i)
    {
        CHttpNotificationService::Instance().Subscribe(GetPublishUrl(),
                                                       GetPropertyClassId(),
                                                       propIds.GetAt(i),
                                                       GetEnableSubClasses(),
                                                       GetContext(),
                                                       GetIncludeResultParent());
    }
}

