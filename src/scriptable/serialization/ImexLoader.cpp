#include "StdAfx.h"

#include "ImexLoader.h"

#include "XmlSpecificImporter.h"

using namespace stc::framework;

ImportableExportable* ImexLoader::loadFromXml(string strFilename) {
    XmlSpecificImporter importer(strFilename);
    return importer.load();
}

ImportableExportable* ImexLoader::loadFromXmlString(string config) {
    XmlSpecificImporter importer;
	importer.SetConfigString(config);
    return importer.load();
}


vector<ImportableExportable*> ImexLoader::loadPartialFromXml(string strFilename, ImportableExportable* parent) {
    XmlSpecificImporter importer(strFilename);
    return importer.loadPartial(parent);
}

vector<ImportableExportable*> ImexLoader::loadPartialFromXmlString(string config, ImportableExportable* parent) {
    XmlSpecificImporter importer;
	importer.SetConfigString(config);
    return importer.loadPartial(parent);
}

