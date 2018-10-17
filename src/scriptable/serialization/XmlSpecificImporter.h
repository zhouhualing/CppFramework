#ifndef __XMLSPECIFICIMPORTER_H__
#define __XMLSPECIFICIMPORTER_H__

#include "AbstractImporter.h"
#include "StcExport.h"

#include "XmlSpecificSaxHandlers.h"

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/parsers/SAXParser.hpp>

#include <string>
#include <vector>

XERCES_CPP_NAMESPACE_USE

using namespace std;

namespace stc {
namespace framework {

/**
 * XmlSpecificImporter is an importer that reads an XML file
 * containing elements whose names are specific to the
 * ScriptableObject being created.
 *
 * It produces one node which contains all nodes from the hierarchy,
 * and a collection containing all of the partial branches marked as
 * serialization bases in the XML file.
 *
 * @author <a href="mailto:Andrew.Oliver@spirentcom.com">Andy Oliver</a>
 */
class XmlSpecificImporter : public AbstractImporter
{
  public:

    /// \brief Open the named file for reading specific-style XML
    STC_PUBLIC XmlSpecificImporter(string strFilename);

	STC_PUBLIC XmlSpecificImporter();

    STC_PUBLIC virtual ~XmlSpecificImporter();

	STC_PUBLIC void SetConfigString(const string& config);

	STC_PUBLIC void OverwriteParentOnPartialLoad(bool overwriteParent);

	STC_PUBLIC void SetUseDefaultName(bool defaultName);

  protected:

    /** 
        \brief Read an object tree from the provided file

        Read an object tree from a stream and return the root of the
        hierarchy.  The caller is responsible for deleting the
        returned object.
     */
    STC_PUBLIC virtual ImportableExportable* doLoad(void);

    /** 
        \brief Read branches from an object tree in the provided file
		and attach them to the parent

        Read an object tree from a stream and return a collection of
        objects marked during serialization as 'serializationBase'
        objects.  The caller is responsible for deleting the returned
        object.
     */
    STC_PUBLIC virtual vector<ImportableExportable*> doLoadPartial(ImportableExportable* parent);

    /// \brief Prepare the parser for use
    virtual void setupParser();

    /// \brief Parse the provided document, storing the resulting objects
    virtual void parse();

    /// \brief Do all setup and parsing, then record results
    virtual void setupAndParse();

    virtual void cleanupParser();

  private:
    int errorCode;
    int errorCount;
    ImportableExportable* pimexLoaded; ///< Root of the loaded hierarchy
    vector<ImportableExportable*> vecimexLoaded; ///< Collection of loaded branches
    SAXParser* parser;
    string strFilename;
    XmlSpecificSaxHandlers handler;
	bool usePartialLoad;
	bool overwriteParentOnPartialLoad;
	bool useDefaultName;
	ImportableExportable* partialLoadParent;
	std::string strConfig;

	XmlSpecificImporter(const XmlSpecificImporter&);
	XmlSpecificImporter& operator = (const XmlSpecificImporter&);
};

}
}

#endif

