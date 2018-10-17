#include "StdAfx.h"

#include "ImportableFactory.h"

#include <iostream>

using namespace stc::framework;

ImportableFactory& ImportableFactory::getInstance() {
    static ImportableFactory factorySingleton;
    return factorySingleton;
}

void ImportableFactory::registerProvider(ImportableProvider* pprovider,
                                         vector<string> vecProvidedTypes) {
    // Loop over strings in type vector, adding each to the internal map
    vector<string>::iterator iterTypes = vecProvidedTypes.begin();
    while (iterTypes != vecProvidedTypes.end()) {
        mapTypeToProvider[*iterTypes] = pprovider;
        iterTypes++;
    }
}

void ImportableFactory::unregisterProvider(ImportableProvider* pprovider) {
    
    map<string, ImportableProvider*>::iterator iter = mapTypeToProvider.begin();
    for(; iter!=mapTypeToProvider.end(); )
    {
        if(iter->second == pprovider)
        {
            mapTypeToProvider.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }
}

ImportableExportable* ImportableFactory::create(string strType, ImportableExportable* parent) {
    // Find provider to handle this type
    ImportableProvider* pprovider = getProviderForType(strType);

    ImportableExportable* pimexToReturn = NULL;
    // Check that a provider is available
    if (NULL != pprovider) {
        // Delegate to the provider
        pimexToReturn = pprovider->create(strType, parent);
    } else {
        // TODO: P2S3: Log error via standard logging: No provider for the requested type
        cout << "WARNING: No provider for type " << strType << endl;
    }

    // Return the importable
    return pimexToReturn;
}

ImportableFactory::ImportableFactory() {
}

ImportableProvider* ImportableFactory::getProviderForType(string strType) {
	ImportableProvider* pprovider = mapTypeToProvider[strType];
	if (NULL == pprovider) {
		pprovider = mapTypeToProvider["*"];
	}
    return pprovider;
}

