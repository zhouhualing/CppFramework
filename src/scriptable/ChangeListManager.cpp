#include "StdAfx.h"

#include "ChangeListManager.h"

using namespace stc::framework;

ChangeListManager& ChangeListManager::Instance()
{
	static ChangeListManager cm;
	return cm;
}

ChangeListManager::ChangeListManager():
	m_activeChangeList(0)
{
}

ChangeListManager::~ChangeListManager()
{
}


/***
	Returns a new ChangeList
 */
ChangeList* ChangeListManager::CreateChangeList()
{
	return 0;
}

/***
	Returns the current active ChangeList.
 */
void ChangeListManager::SetActiveChangeList(ChangeList* cl)
{
	m_activeChangeList = cl;
}

/***
	Returns the current active ChangeList.
 */
ChangeList* ChangeListManager::GetActiveChangeList() const
{
	return m_activeChangeList;
}

