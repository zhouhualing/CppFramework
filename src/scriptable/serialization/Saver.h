#ifndef __SAVER_H__
#define __SAVER_H__

#include <string>
#include <sstream>

#include "Scriptable.h"
#include "ScriptExporterOptions.h"

using namespace std;
using namespace stc::framework;

namespace stc {
namespace framework {

typedef map<string, string> PathReplacements_t;

/**
 * A Saver is a facade for the import/export classes that
 * provides simple methods to save Scriptable objects to XML
 * and/or script.
 *
 * @author <a href="mailto:Andrew.Oliver@spirentcom.com">Andy Oliver</a>
 */
class Saver {
  public:
    /// \brief Save the config to a file, omitting standard exclusions
    STC_PUBLIC static void saveAsXml(CScriptable& exp, string strFilename, bool isSaveDefault, bool skipNoSaveAttribute  = false);
    /// \brief Save the config to a file, omitting only specified classes
    static void saveAsXml(CScriptable& exp, vector<string> vecClassesToExclude, 
                          string strFilename, bool isSaveDefault, bool skipNoSaveAttribute  = false);

	/// \brief Save as xml to a file.
	static void saveAsXml(CScriptable& root, 
						const ScriptableVec& toSaveVec,
						string strFilename, bool isSaveDefault, bool skipNoSaveAttribute  = false);
	/// \brief Save as xml to a stream.
	static void saveAsXml(CScriptable& root, 
						const ScriptableVec& toSaveVec,
						std::ostream& os, bool isSaveDefault, bool skipNoSaveAttribute = false);

    /// \brief Save the config to a file, omitting standard exclusions
    STC_PUBLIC static void saveAsScript(CScriptable& exp, string strFilename, bool isSaveDefault, 
                                     string strPrefix = "", string strBeforeDisconnectScript = "",
                                     string strNamespace = "",
                                     PathReplacements_t pathReplacements = PathReplacements_t(),
                                     Language_t language = TCL, bool compact = false);
    /// \brief Save the config to a file, omitting only specified classes
    static void saveAsScript(CScriptable& exp, vector<string> vecClassesToExclude, 
                          string strFilename, bool isSaveDefault, string strPrefix, 
                          string strBeforeDisconnectScript,
                          string strNamespace = "", 
                          PathReplacements_t pathReplacements = PathReplacements_t(),
                          Language_t language = TCL, bool compact = false);
    /// \brief Save the config to a file, omitting standard exclusions
    static void saveAsScriptWithXml(CScriptable& exp, string strFilename, 
                                 string strPrefix, string strBeforeDisconnectScript, 
                                 bool isSaveDefault,
                                 string strNamespace = "", 
                                 PathReplacements_t pathReplacements = PathReplacements_t(),
                                 Language_t language = TCL);
    /// \brief Save the config to a file, omitting only specified classes
    static void saveAsScriptWithXml(CScriptable& exp, vector<string> vecClassesToExclude, 
                                 string strFilename, string strPrefix, 
                                 string strBeforeDisconnectScript, bool isSaveDefault, 
                                 string strNamespace = "",
                                 PathReplacements_t pathReplacements = PathReplacements_t(),
                                 Language_t language = TCL);

	static void setSavedFiles(const vector<string>& files);
	static vector<string> getSavedFiles();

  protected:
    static void setupStandardExclusions(vector<string>& vecClassesToExclude);

};

}
}

#endif

