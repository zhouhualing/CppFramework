#ifndef __IMEXSAVER_H__
#define __IMEXSAVER_H__

#include <string>

#include "Exportable.h"
#include "ScriptExporter.h"
#include "ScriptExporterOptions.h"
#include "StcExport.h"

using namespace std;

namespace stc {
namespace framework {

/**
 * An ImexSaver is a facade for the import/export classes that
 * provides simple methods to save to XML and/or a script.
 *
 * @author <a href="mailto:Andrew.Oliver@spirentcom.com">Andy Oliver</a>
 */
class ImexSaver {
  public:
    STC_PUBLIC static void saveAsXml(Exportable& exp, string strFilename);
    static void saveAsXml(Exportable& exp, vector<string> vecClassesToExclude, 
                          string strFilename);

    static void saveAsXml(Exportable& exp, vector<string> vecClassesToExclude, 
							set<Exportable*> setExportablesToSave,
							string strFilename);

    static void saveAsXml(Exportable& exp, vector<string> vecClassesToExclude, 
							set<Exportable*> setExportablesToSave,
							std::ostream& os);

    STC_PUBLIC static void saveAsScript(Exportable& exp, string strFilename, const ExporterOptions& options);
    static void saveAsScript(Exportable& exp, vector<string> vecClassesToExclude, string strFilename, const ExporterOptions& options);
    static void saveAsScriptWithXml(Exportable& exp, string strFilename, const ExporterOptions& options);
    static void saveAsScriptWithXml(Exportable& exp, vector<string> vecClassesToExclude, 
                                 string strFilename, const ExporterOptions& options);
};

}
}

#endif

