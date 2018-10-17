#ifndef __XMLGENERICEXPORTER_H__
#define __XMLGENERICEXPORTER_H__

#include "XmlExporter.h"
#include "StcExport.h"

namespace stc {
namespace framework {

/**
 * XmlGenericExporter is an exporter that generates an XML file
 * containing generic "Object" elements with type as an attribute.
 *
 * @author <a href="mailto:Andrew.Oliver@spirentcom.com">Andy Oliver</a>
 */
class XmlGenericExporter : public XmlExporter
{
  public:

    /// \brief Open the named file for generic-style XML
    STC_PUBLIC XmlGenericExporter(string strFilename);

    /// \brief Use the provided stream for generic-style XML
    XmlGenericExporter(ostream* posOutputDestination);

    virtual void writeHeader(Exportable& exp);
    virtual void startObject(Exportable& exp, Exportable& expParent, const string& prefix);
    virtual void writeSerializationBaseProperty();
    virtual void finishObjectProperties(Exportable& exp, const string& strPrefix);
    virtual void finishObject(Exportable& exp, const string& prefix);
    virtual void startProperty(const string& strName,
                               const string& strValue,
                               const string& prefix);
    virtual void finishProperty(const string& prefix);
    virtual void handleRelationship(Exportable& exp, 
                                    const string& strType,
                                    bool bSourceToTarget,
                                    bool last,
                                    const string& strPrefix);
    virtual void writeTrailer();
};

}
}

#endif

