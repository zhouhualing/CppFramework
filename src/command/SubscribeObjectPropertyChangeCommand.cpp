#include "StdAfx.h"
#include "SubscribeObjectPropertyChangeCommand.h"
#include "SubscribeObjectPropertyChangeCommand_AutoGen.cpp"
#include "HttpNotificationService.h"

using namespace std;
using namespace stc::framework;

CSubscribeObjectPropertyChangeCommand::CSubscribeObjectPropertyChangeCommand()
{

}

CSubscribeObjectPropertyChangeCommand::~CSubscribeObjectPropertyChangeCommand()
{

}

void CSubscribeObjectPropertyChangeCommand::DoRun()
{
	CollectionProperty<StcHandle>& objects = GetObjectList();
    for (int i = 0; i < objects.Count(); ++i)
    {
        CHttpNotificationService::Instance().SubscribeObjectProperty(GetPublishUrl(),
																	 GetPropertyId(),
																	 objects.GetAt(i),
																	 GetContext());
    }
}



