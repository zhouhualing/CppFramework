#include "StdAfx.h"

#include "ScriptableProvider.h"

#include <vector>
#include <string>

#include "frameworkConst.h"
#include "ImportableFactory.h"
#include "MetaClassManager.h"
#include "MetaInfo.h"
#include "Scriptable.h"
#include "ScriptableImexAdapter.h"

using namespace stc::framework;


const ScriptableProvider& ScriptableProvider::getInstance() {
    static ScriptableProvider providerSingleton;

    return providerSingleton;
}

ImportableExportable* ScriptableProvider::create(string strType, ImportableExportable* parent) {

	CScriptableCreator ctor;

	CScriptable* pParentScriptable = NULL;
	if (parent)
	{
		pParentScriptable = 
			dynamic_cast<ScriptableImexAdapter*>(parent)->getScriptable();
	}

	CScriptable* pscriptable = ctor.Create(std::string(strType), 
		pParentScriptable, 
		true, false);

    // Return a new instance of the class
	return ScriptableImexAdapter::Create(pscriptable);
}

ScriptableProvider::ScriptableProvider() {
    vector<string> vecTypes;

    // Get types from MetaClassManager when it has such support
	CMetaClassManager& mgr = CMetaClassManager::Instance();
	CMetaClassIterator it = mgr.GetMetaClassIterator();

	while (it.HasNext()) {
		CMetaClass* cls = it.Next();
        vecTypes.push_back(cls->GetMetaInfo().name);
    }
	vecTypes.push_back("*");

    ImportableFactory::getInstance().registerProvider(this, vecTypes);
}

