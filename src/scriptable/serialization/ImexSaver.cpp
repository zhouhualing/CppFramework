#include "StdAfx.h"

#include "ImexSaver.h"

#include "XmlSpecificExporter.h"
#include "ScriptWithXmlExporter.h"
#ifndef USE_MOCK
#include "DbExporter.h"
#endif

using namespace stc::framework;

void ImexSaver::saveAsXml(Exportable& exp, string strFilename) {
    XmlSpecificExporter exporter(strFilename);
    exporter.save(exp);
}

void ImexSaver::saveAsXml(Exportable& exp, vector<string> vecClassesToExclude, 
                          string strFilename) {
    XmlSpecificExporter exporter(strFilename);
    exporter.save(exp, vecClassesToExclude);
}

void ImexSaver::saveAsXml(Exportable& exp, vector<string> vecClassesToExclude, 
						set<Exportable*> setExportablesToSave,
						string strFilename)
{
    XmlSpecificExporter exporter(strFilename);
	if (!setExportablesToSave.empty())
	{
		exporter.save(exp, setExportablesToSave);
	}
	else
	{
		exporter.save(exp, vecClassesToExclude);
	}
}

void ImexSaver::saveAsXml(Exportable& exp, vector<string> vecClassesToExclude, 
						set<Exportable*> setExportablesToSave,
						std::ostream& os)
{
    XmlSpecificExporter exporter(&os);
	if (!setExportablesToSave.empty())
	{
		exporter.save(exp, setExportablesToSave);
	}
	else
	{
		exporter.save(exp, vecClassesToExclude);
	}
}


void ImexSaver::saveAsScript(Exportable& exp, string strFilename, const ExporterOptions& options) {
    ScriptExporter exporter(strFilename);
    exporter.setOptions(options);
    exporter.save(exp);
}

void ImexSaver::saveAsScript(Exportable& exp, vector<string> vecClassesToExclude, 
                          string strFilename, const ExporterOptions& options) {
    ScriptExporter exporter(strFilename);
    exporter.setOptions(options);    
    exporter.save(exp, vecClassesToExclude);
}

void ImexSaver::saveAsScriptWithXml(Exportable& exp, string strFilename, const ExporterOptions& options) {
    ScriptWithXmlExporter exporter(strFilename);
    exporter.setOptions(options);        
    exporter.save(exp);
}

void ImexSaver::saveAsScriptWithXml(Exportable& exp, vector<string> vecClassesToExclude, 
                                 string strFilename, const ExporterOptions& options) {
    ScriptWithXmlExporter exporter(strFilename);
    exporter.setOptions(options);    
    exporter.save(exp, vecClassesToExclude);
}

