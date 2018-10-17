#ifndef __XMLSPECIFICSAXHANDLER_H__
#define __XMLSPECIFICSAXHANDLER_H__

#include <xercesc/sax/HandlerBase.hpp>

#include "ImportableExportable.h"
#include "AbstractImporter.h"

#include <deque>
#include <map>

XERCES_CPP_NAMESPACE_USE

namespace stc {
namespace framework {

class XmlSpecificSaxHandlers : public HandlerBase
{
  public:
	  
    XmlSpecificSaxHandlers();
    ~XmlSpecificSaxHandlers();

	void SetImporter(AbstractImporter* imp);
	void SetUsePartialLoad(bool partialLoad);
	void SetPartialLoadParent(ImportableExportable* parent, bool overwriteParent);
	void SetUseDefaultName(bool defaultName);

    // -----------------------------------------------------------------------
    //  Implementations of the SAX DocumentHandler interface
    // -----------------------------------------------------------------------
    void endDocument();

    void endElement(const XMLCh* const name);

    void startDocument();

    void startElement(const XMLCh* const name, AttributeList& attributes);

    // -----------------------------------------------------------------------
    //  Implementations of the SAX ErrorHandler interface
    // -----------------------------------------------------------------------
    void warning(const SAXParseException& exc);
    void error(const SAXParseException& exc);
    void fatalError(const SAXParseException& exc);

    // -----------------------------------------------------------------------
    //  Implementation of the SAX DTDHandler interface
    // -----------------------------------------------------------------------
    void notationDecl(const XMLCh* const name,
                      const XMLCh* const publicId,
                      const XMLCh* const systemId);

    void unparsedEntityDecl(const XMLCh* const name,
                            const XMLCh* const publicId, 
                            const XMLCh* const systemId, 
                            const XMLCh* const notationName);

    /// \brief Retrieve the root of the deserialized hierarchy
    ImportableExportable* getImportable();

    /// \brief Retrieve the serializationBase branches marked in the hierarchy
    vector<ImportableExportable*> getImportableBranches();

    void handleRelationship(AttributeList& attributes);

  private:

	ImportableExportable* GetCurrentChild(
		const std::string& name, 
		const AttributeList& attributes,
		ImportableExportable* parent);

    deque<ImportableExportable*> deqParents;
    ImportableExportable* pimexLoaded;
	vector<ImportableExportable*> vecimexLoaded;
    int icElementsSeen;
	bool useDefaultName;
	bool usePartialLoad;
	bool overwriteParentOnPartialLoad;
	bool isInSerializationBaseBranch;
	std::string serializationBaseName;
	ImportableExportable* partialLoadParent;
	AbstractImporter* importer;

	XmlSpecificSaxHandlers(const XmlSpecificSaxHandlers&);
	XmlSpecificSaxHandlers& operator = (const XmlSpecificSaxHandlers&);
};

}
}

#endif

