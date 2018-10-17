#include "StdAfx.h"

#include "ScriptWithXmlExporter.h"

#include "Saver.h"
#include "Path.h"
#include "XmlSpecificExporter.h"
#include "StcFileManager.h"

#include <ostream>
#include <fstream>

using namespace stc::framework;

ScriptWithXmlExporter::ScriptWithXmlExporter(string strFilename) : ScriptExporter(strFilename) 
{
    pvecClassesToExclude = NULL;    
}

void ScriptWithXmlExporter::setOptions(const ExporterOptions& options)
{
    ExporterOptions myOptions(options);
    myOptions.withXML = true;
    ScriptExporter::setOptions(myOptions);
}

void ScriptWithXmlExporter::writeConfigAsXML(Exportable& exp) 
{    
    XmlSpecificExporter exporter(getXmlFilename());
    exporter.save(exp, *pvecClassesToExclude);
}

string ScriptWithXmlExporter::getXmlFilename() 
{
    std::string strNameOfXmlFile = strFilename;
    string::size_type posPeriod = strNameOfXmlFile.rfind('.');
    if (posPeriod != string::npos) 
    {
        strNameOfXmlFile = strFilename.substr(0, posPeriod);
    }
	strNameOfXmlFile += ".xml";
    
    return strNameOfXmlFile;
}

void ScriptWithXmlExporter::writeHeader(Exportable& exp) 
{    
    // Save config as XML file to be loaded by script this class writes
    writeConfigAsXML(exp);

    writeIntroComment();
    writeInitCode();
    	
    string xmlFileName = Path::GetFileName(getXmlFilename());
    string code = mScriptGen->XmlLoad(xmlFileName);
    *pout << code;    
}

void ScriptWithXmlExporter::startObject(Exportable& exp, Exportable& expParent, const string& strPrefix)
{
    saveResultQuery(exp, expParent);

    if (exp.getType() == "Port" && !isVirtualPort(exp))
    {
        string strLocation = exp.getPropertyValueForId(getPropertyIdForName(exp, "Location"));
        listPortLocations.push_back(strLocation);
    }
}

void ScriptWithXmlExporter::writeTrailer() 
{
    // Connect, reserve
    writeConnectReserveCode();

    // Write Launcher Script into separate file
    writeLauncherScript();


	std::vector<std::string> files;
	files.push_back(strFilename);
	files.push_back(strLogicFilename);
	files.push_back(getXmlFilename());

	Saver::setSavedFiles(files);
}

void ScriptWithXmlExporter::writeFilePathSubstitution(Exportable& exp)
{
    ;
}

