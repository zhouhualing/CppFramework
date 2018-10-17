#ifndef __ScriptWithXmlExporter_H__
#define __ScriptWithXmlExporter_H__

#include <ostream>
#include <string>

#include "ScriptExporter.h"
#include "StcExport.h"

namespace stc {
namespace framework {

/**
 * Generic script exporter with config serialized to XML.
 */
class ScriptWithXmlExporter : public ScriptExporter
{
  public:
    /// \brief Open the named file for a Tcl script that loads external XML
    STC_PUBLIC ScriptWithXmlExporter(string strFilename);
    void setOptions(const ExporterOptions& options);
    
  protected:
    // Methods to be overridden in subtypes
    virtual void writeHeader(Exportable& exp);
    virtual void startObject(Exportable& exp, Exportable& expParent, const string& strPrefix);
    virtual void finishObjectProperties(Exportable& exp, const string& strPrefix) {};
    virtual void finishObject(Exportable& exp, const string& strPrefix) {};
    virtual void writeSerializationBaseProperty() {};
    virtual void startProperty(const std::string& strName,
                               const std::string& strValue,
                               const string& strPrefix) {};
    virtual void finishProperty(const string& strPrefix) {};
    virtual void handleRelationship(Exportable& exp, 
                                    const string& strType,
                                    bool bSourceToTarget,
                                    bool last,
                                    const string& strPrefix) {};
    virtual void writeTrailer();

    /// \brief Save out the configuration as XML using XML SPecific Exporter
    void writeConfigAsXML(Exportable& exp);

    virtual void writeFilePathSubstitution(Exportable& exp);

  private:

    /// \brief Return the XML filename that matches this Tcl script filename
    string getXmlFilename();
    
};

}
}

#endif

