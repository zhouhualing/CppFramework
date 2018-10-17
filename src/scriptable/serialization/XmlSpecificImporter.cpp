#include "StdAfx.h"

#include "XmlSpecificImporter.h"
#include "StcException.h"
#include "ScriptableImexAdapter.h"

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include <iostream>
#include <set>

XERCES_CPP_NAMESPACE_USE

static bool                     doNamespaces        = false;
static bool                     doSchema            = false;
static bool                     schemaFullChecking  = false;
//static const char*              encodingName    = "LATIN1";

using namespace stc::framework;

XmlSpecificImporter::XmlSpecificImporter(string filename):
	strFilename(filename),
	errorCode(0),
	errorCount(0),
	usePartialLoad(false),
	useDefaultName(false),
	overwriteParentOnPartialLoad(false),
	partialLoadParent(0),
	parser(NULL),
	pimexLoaded(NULL) {

	strConfig = "";
}

XmlSpecificImporter::XmlSpecificImporter():
	errorCode(0),
	errorCount(0),
	usePartialLoad(false),
	useDefaultName(false),
	overwriteParentOnPartialLoad(false),
	partialLoadParent(0),
	parser(NULL),
	pimexLoaded(NULL) {

	strConfig = "";
	strFilename = "";
}


XmlSpecificImporter::~XmlSpecificImporter() {
	cleanupParser();
}

void XmlSpecificImporter::SetConfigString(const string& config) {
	STC_ASSERT(strFilename == "", "strFileName must be empty when loading config from a string");
	strConfig = config;
}

void XmlSpecificImporter::OverwriteParentOnPartialLoad(bool overwriteParent)
{
	overwriteParentOnPartialLoad = overwriteParent;
}


void XmlSpecificImporter::SetUseDefaultName(bool defaultName)
{
	useDefaultName = defaultName;
}


void XmlSpecificImporter::setupAndParse(void) {
    try {
        setupParser();
        parse();
        pimexLoaded = handler.getImportable();
        vecimexLoaded = handler.getImportableBranches();
		cleanupParser();
		// xerces doesn't always throw on errors
		if (errorCount > 0)
		{
			throw CStcRunTimeError("Failed to parse " + strFilename);
		}
    } catch (const XMLException& toCatch) {
        char* pszErrorMessage = XMLString::transcode(toCatch.getMessage());
        string strErrorMessage(pszErrorMessage);
        XMLString::release(&pszErrorMessage);

        XERCES_STD_QUALIFIER cerr << "\nAn error occurred while parsing the XML file" <<
            "\n  Error: "
             << strErrorMessage << "\n" << XERCES_STD_QUALIFIER endl;
        errorCode = -1;
		++errorCount;
		cleanupParser();
	} catch(CStcException&) {
		++errorCount;
		cleanupParser();
		throw;
	}
}

ImportableExportable* XmlSpecificImporter::doLoad(void) {
	usePartialLoad = false;

    setupAndParse();

    return pimexLoaded;
}

vector<ImportableExportable*> XmlSpecificImporter::doLoadPartial(ImportableExportable* parent) {
	usePartialLoad = true;
	partialLoadParent = parent;

    setupAndParse();

	return vecimexLoaded;
}

void XmlSpecificImporter::setupParser() {
    // Initialize the XML4C2 system
    try {
        XMLPlatformUtils::Initialize();
    } catch (const XMLException& toCatch) {
        char* pszErrorMessage = XMLString::transcode(toCatch.getMessage());
        string strErrorMessage(pszErrorMessage);
        XMLString::release(&pszErrorMessage);

        XERCES_STD_QUALIFIER cerr << "Error during initialization! :\n"
                                  << strErrorMessage << XERCES_STD_QUALIFIER endl;
        errorCount = 1;
    }

    //  Create a SAX parser object. Then, according to what we were told on
    //  the command line, set it to validate or not.
    parser = new SAXParser;
    parser->setValidationScheme(SAXParser::Val_Auto);
    parser->setDoNamespaces(doNamespaces);
    parser->setDoSchema(doSchema);
    parser->setValidationSchemaFullChecking(schemaFullChecking);
}

void XmlSpecificImporter::parse() {
	handler.SetUsePartialLoad(usePartialLoad);
	handler.SetPartialLoadParent(partialLoadParent, overwriteParentOnPartialLoad);
	handler.SetUseDefaultName(useDefaultName);
	handler.SetImporter(this);
    parser->setDocumentHandler(&handler);
    parser->setErrorHandler(&handler);
	if (strFilename == "")
	{
		MemBufInputSource buf((const XMLByte *)strConfig.c_str(),
            (const unsigned int)strConfig.length(), "fake");
		parser->parse(buf);
	}
	else
	{
		parser->parse(strFilename.c_str());
	}
    errorCount = parser->getErrorCount();
}

void XmlSpecificImporter::cleanupParser() {

	if (errorCount > 0 && parser != NULL) {

        pimexLoaded = handler.getImportable();
        vecimexLoaded = handler.getImportableBranches();

		std::set<ImportableExportable*> imexs;
		imexs.insert(pimexLoaded);
		imexs.insert(vecimexLoaded.begin(), vecimexLoaded.end());

		for (std::set<ImportableExportable*>::iterator it = imexs.begin(); it != imexs.end(); ++it)	{
			ScriptableImexAdapter* s = dynamic_cast<ScriptableImexAdapter*>(*it);
			if (s && s->getScriptable()) {
				s->getScriptable()->MarkDelete();
			}
		}

        pimexLoaded = NULL;
        vecimexLoaded.clear();
		ScriptableImexAdapter::ClearAllAdapter();
    }

    // Delete the parser we created in setup
    delete parser;
	parser = NULL;


    // Clean up the SAX module
    // Do not call terminate. It takes significant amount of time 
    // to reinitialize parser.
    // According to the doc. for this method, it is optional.
    // It helps to avoid leak reports from memory leak detectors.
    // -- Vladimir
    // XMLPlatformUtils::Terminate();
}

