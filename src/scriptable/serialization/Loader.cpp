#include "StdAfx.h"

#include "Loader.h"

#include "ImexLoader.h"
#include "ScriptableImexAdapter.h"
#include "ScriptableProvider.h"

CScriptable* Loader::loadFromXml(string strFilename) {
    ScriptableProvider provider = ScriptableProvider::getInstance();
	ScriptableImexAdapter::ClearAllAdapter();
    ImportableExportable* pimex = ImexLoader::loadFromXml(strFilename);
	if (pimex)
		return (dynamic_cast<ScriptableImexAdapter*>(pimex))->getScriptable();
	else
		return 0;
}

CScriptable* Loader::loadFromXmlString(string strConfig) {
    ScriptableProvider provider = ScriptableProvider::getInstance();
	ScriptableImexAdapter::ClearAllAdapter();
    ImportableExportable* pimex = ImexLoader::loadFromXmlString(strConfig);
	if (pimex)
		return (dynamic_cast<ScriptableImexAdapter*>(pimex))->getScriptable();
	else
		return 0;
}

static std::vector<CScriptable*> loadPartialFromXmlUtil(string str, CScriptable* baseConfig, bool isFile) {
	assert(baseConfig);

	ScriptableImexAdapter::ClearAllAdapter();
	ScriptableImexAdapter parent(baseConfig);

    ScriptableProvider provider = ScriptableProvider::getInstance();

	vector<ImportableExportable*> pimexies;
	
	if (isFile)
	{
		pimexies = ImexLoader::loadPartialFromXml(str, &parent);
	}
	else
	{
		pimexies = ImexLoader::loadPartialFromXmlString(str, &parent);
	}

	std::vector<CScriptable*> retVec;

	for (vector<ImportableExportable*>::iterator it = pimexies.begin();
		it != pimexies.end();
		++it)
	{
		ScriptableImexAdapter* imex = dynamic_cast<ScriptableImexAdapter*>(*it);
		if (imex && imex->getScriptable())
		{
			retVec.push_back(imex->getScriptable());
		}
	}

	return retVec;
}

std::vector<CScriptable*> Loader::loadPartialFromXml(string strFilename, CScriptable* baseConfig) {
	return loadPartialFromXmlUtil(strFilename, baseConfig, true);
}


std::vector<CScriptable*> Loader::loadPartialFromXmlString(string strConfig, CScriptable* baseConfig) {
	return loadPartialFromXmlUtil(strConfig, baseConfig, false);
}

