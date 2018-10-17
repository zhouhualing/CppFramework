#include "StdAfx.h"

#include "MockProvider.h"

#include <vector>
#include <string>

#include "../ImportableFactory.h"
#include "MockScriptable.h"

ImportableExportable* MockProvider::create(string strType, ImportableExportable* parent) {
	ImportableExportable* obj = new MockScriptable(strType);
	if (parent)
	{
		parent->addChild(obj);
	}

	return obj;
}

MockProvider::MockProvider() {
    registerProvider();
}

MockProvider::~MockProvider() {
    unregisterProvider();
}

void MockProvider::registerProvider()
{
    vector<string> vecTypes;
    vecTypes.push_back("Project");
    vecTypes.push_back("RFC2544ThroughputTest");
    vecTypes.push_back("Port");
    vecTypes.push_back("Traffic");
    ImportableFactory::getInstance().registerProvider(this, vecTypes);
}

void MockProvider::unregisterProvider()
{
    ImportableFactory::getInstance().unregisterProvider(this);
}

