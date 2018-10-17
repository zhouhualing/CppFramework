#ifndef __XMLEXPORTER_H__
#define __XMLEXPORTER_H__

#include <ostream>
#include <string>

#include "AbstractRecursiveFileExporter.h"

namespace stc {
namespace framework {

/**
 * XmlExporter is an exporter that generates an XML file.
 *
 * This is abstract and provides functions to derived classes for
 * element and attribute writing.
 *
 * @author <a href="mailto:Andrew.Oliver@spirentcom.com">Andy Oliver</a>
 */
class XmlExporter : public AbstractRecursiveFileExporter
{
  public:

    /// \brief Open the named file for XML export
    XmlExporter(string strFilename);

    /// \brief Use the provided stream for XML export
    XmlExporter(ostream* posOutputDestination);
    
  protected:
    // Methods to be overridden in XML subtypes
    virtual void writeHeader(Exportable& exp) = 0;
    virtual void startObject(Exportable& exp, Exportable& expParent, const string& strPrefix) = 0;
    virtual void finishObjectProperties(Exportable& exp, const string& strPrefix) = 0;
    virtual void finishObject(Exportable& exp, const string& strPrefix) = 0;
    virtual void startProperty(const std::string& strName,
                               const std::string& strValue,
                               const string& strPrefix) = 0;
    virtual void finishProperty(const string& strPrefix) = 0;
    virtual void handleRelationship(Exportable& exp, 
                                    const string& strType,
                                    bool bSourceToTarget,
                                    bool last,
                                    const string& strPrefix) = 0;
    virtual void writeTrailer() = 0;

    // Utility functions for subtypes

    /// \brief Add serializationBase="true" attribute to the element
    virtual void writeSerializationBaseProperty();

    /// \brief Perform all escaping needed to make an attribute XML-safe
    virtual string sanitizeString(string strUnclean);
};

}
}

#endif

