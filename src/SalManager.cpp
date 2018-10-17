#include "StdAfx.h"
#include "SalManager.h"

using namespace stc::framework;

CSalManager::CSalManager() :
    _activeType(MSG),
    _activeImplChangedDelegate()
{
}

CSalManager::~CSalManager()
{
}

CSalManager& CSalManager::Instance()
{
    static CSalManager inst;
    return inst;
}

void CSalManager::SetActiveImpl(SalImplType_t implType)
{
    _activeType = implType;
    _activeImplChangedDelegate(implType);
}

void CSalManager::RegisterActiveImplChanged(ActiveImplChangedDelegate d)
{
    _activeImplChangedDelegate += d;
}

void CSalManager::UnRegisterActiveImplChanged(ActiveImplChangedDelegate d)
{
    _activeImplChangedDelegate -= d;
}

CScopedSalImplDirect::CScopedSalImplDirect()
{
    CSalManager::Instance().SetActiveImpl(CSalManager::DIRECT);
}

CScopedSalImplDirect::~CScopedSalImplDirect()
{
    CSalManager::Instance().SetActiveImpl(CSalManager::MSG);
}



