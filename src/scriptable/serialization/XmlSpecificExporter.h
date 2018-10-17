#ifndef __XMLSPECIFICEXPORTER_H__
#define __XMLSPECIFICEXPORTER_H__

#include "XmlExporter.h"
#include "StcExport.h"

namespace stc {
namespace framework {

/**
 * XmlSpecificExporter is an exporter that generates an XML file
 * containing elements whose names are specific to the
 * ScriptableObject being stored.
 *
 * @author <a href="mailto:Andrew.Oliver@spirentcom.com">Andy Oliver</a>
 */
class XmlSpecificExporter : public XmlExporter
{
  public:

    /// \brief Open the named file for specific-style XML
    STC_PUBLIC XmlSpecificExporter(string strFilename);

    /// \brief Use the provided stream for specific-style XML
    STC_PUBLIC XmlSpecificExporter(ostream* posOutputDestination);

    virtual void writeHeader(Exportable& exp);
    virtual void startObject(Exportable& exp, Exportable& expParent, const string& prefix);
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

	// returns xml encoding appropriate for the current code page.
	// Implemented on Windows only, on non-Windows platforms the method always return NULL.
	static const char* GetEncoding();
};

}
}

#endif

