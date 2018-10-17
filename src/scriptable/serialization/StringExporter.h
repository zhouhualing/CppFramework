#ifndef __STRINGEXPORTER_H__
#define __STRINGEXPORTER_H__

#include <ostream>
#include <string>
#include <sstream>

#include "AbstractRecursiveExporter.h"

namespace stc {
namespace framework {

/**
 * StringExporter is an exporter that generates a human-readable
 * string version of a configuration.
 *
 * @author <a href="mailto:Andrew.Oliver@spirentcom.com">Andy Oliver</a>
 */
class StringExporter : public AbstractRecursiveExporter
{
  public:
    StringExporter(string strFilename);
    string getString();
  protected:
    virtual void openOutput() {};
    virtual void closeOutput() {};
    virtual void writeHeader(Exportable& exp);
    virtual void startObject(Exportable& exp, Exportable& expParent, const string& strPrefix);
    virtual void finishObjectProperties(Exportable& exp, const string& strPrefix);
    virtual void finishObject(Exportable& exp, const string& strPrefix);
    virtual void writeSerializationBaseProperty();
    virtual void startProperty(const std::string& strName,
                               const std::string& strValue,
                               const string& strPrefix);
    virtual void finishProperty(const string& strPrefix);
    virtual void handleRelationship(const string& strId, 
                                    const string& strType,
                                    const bool bSourceToTarget,
                                    const string& strPrefix);
    virtual void writeTrailer();
  private:
    ostringstream out;
};

}
}

#endif

