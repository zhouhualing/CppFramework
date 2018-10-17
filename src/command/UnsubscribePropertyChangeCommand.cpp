#include "StdAfx.h"
#include "UnsubscribePropertyChangeCommand.h"
#include "UnsubscribePropertyChangeCommand_AutoGen.cpp"
#include "HttpNotificationService.h"

using namespace std;
using namespace stc::framework;

CUnsubscribePropertyChangeCommand::CUnsubscribePropertyChangeCommand()
{

}

CUnsubscribePropertyChangeCommand::~CUnsubscribePropertyChangeCommand()
{

}

void CUnsubscribePropertyChangeCommand::DoRun()
{
    CollectionProperty<PropId>& propIds = GetPropertyIdList();
    for (int i = 0; i < propIds.Count(); ++i)
    {
        CHttpNotificationService::Instance().Unsubscribe(GetPublishUrl(),
                                                         GetPropertyClassId(),
                                                         propIds.GetAt(i));
    }
}

