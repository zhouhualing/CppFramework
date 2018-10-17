#ifndef __ABSTRACTRECURSIVEFILEEXPORTER_H__
#define __ABSTRACTRECURSIVEFILEEXPORTER_H__

#include "AbstractRecursiveExporter.h"
#include "StcExport.h"

#include <ostream>
#include <string>

using namespace std;

namespace stc {
namespace framework {

/**
 * An AbstractRecursiveFileExporter is a small wrapper around
 * AbstractRecursiveExporter that provides support for writing to
 * files on the filesystem (or any other output destination with
 * ostream support).
 *
 * @author <a href="mailto:Andrew.Oliver@spirentcom.com">Andy Oliver</a>
 */
class AbstractRecursiveFileExporter : public AbstractRecursiveExporter {
  public:

    /// \brief Shortcut constructor that takes a filename
    AbstractRecursiveFileExporter(string strFilename);
    
    /// \brief Constructor that uses the provided ostream for output
    AbstractRecursiveFileExporter(ostream* posOutputDestination);

    /// \brief Destructor must close open output resources
    STC_PUBLIC virtual ~AbstractRecursiveFileExporter();

  protected:
    virtual void saveSpecialCaseObjects(Exportable& exp);
    virtual void openOutput();
    virtual void writeHeader(Exportable& exp) = 0;
    virtual void startObject(Exportable& exp, Exportable& expParent, const string& strPrefix) = 0;
    virtual void finishObjectProperties(Exportable& exp, const string& strPrefix) = 0;
    virtual void finishObject(Exportable& exp, const string& strPrefix) = 0;
    virtual void writeSerializationBaseProperty() = 0;
    virtual void startProperty(const string& strName,
                               const string& strValue,
                               const string& strPrefix) = 0;
    virtual void finishProperty(const string& strPrefix) = 0;
    virtual void handleRelationship(Exportable& exp, 
                                    const string& strType,
                                    bool bSourceToTarget,
                                    bool last,
                                    const string& strPrefix) = 0;
    virtual void writeTrailer() = 0;
    virtual void closeOutput();

    /// \brief Replaces occurrences of one string with another
    string fixEscapeInString(string strUnescaped, 
                             string strNeedsEscaping, string strEscapedString);

  protected:
    string strFilename; ///< The filename being used for output, if provided
    ostream* pout; ///< The stream that must be used for output
};

}
}

#endif

