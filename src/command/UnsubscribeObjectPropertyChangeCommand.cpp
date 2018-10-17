#include "StdAfx.h"
#include "UnsubscribeObjectPropertyChangeCommand.h"
#include "UnsubscribeObjectPropertyChangeCommand_AutoGen.cpp"
#include "HttpNotificationService.h"

using namespace std;
using namespace stc::framework;

CUnsubscribeObjectPropertyChangeCommand::CUnsubscribeObjectPropertyChangeCommand()
{

}

CUnsubscribeObjectPropertyChangeCommand::~CUnsubscribeObjectPropertyChangeCommand()
{

}

void CUnsubscribeObjectPropertyChangeCommand::DoRun()
{
	CollectionProperty<StcHandle>& objects = GetObjectList();
    for (int i = 0; i < objects.Count(); ++i)
    {
        CHttpNotificationService::Instance().UnsubscribeObjectProperty(GetPublishUrl(),
																	   GetPropertyId(),
													                   objects.GetAt(i));
    }
}

