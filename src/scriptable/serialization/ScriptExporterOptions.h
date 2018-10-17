#ifndef __SCRIPT_EXPORTER_OPTIONS_H__
#define __SCRIPT_EXPORTER_OPTIONS_H__

#include <string>
#include <map>

namespace stc {
namespace framework {

enum Language_t { TCL, RUBY, PERL, PYTHON };

struct ExporterOptions
{
    ExporterOptions() : 
        language(TCL), withXML(false), includeConfigCustom(false), useDefaultsForRarelyUsedSettings(false)
    {

    }

    /// \brief Change the prefix to a new value
    std::string strPrefix;
    /// \brief Change the name of the script to be invoked before disconnecting to a new value
    std::string strBeforeDisconnectScript;    
    /// \brief Specify a namespace for procs and global variables.
    std::string strNamespace;
    /// \brief Specify a mapping of file path replacements. This is used to generate a basePathMap variable
    /// and code that does path substitutions at runtime.
    std::map<std::string, std::string> pathReplacements;
    /// \brief If enabled, some rarely configured objects will not be saved.
    bool useDefaultsForRarelyUsedSettings;
    /// \brief Specify the language to generate.
    Language_t language;
    /// \brief Specify whether or not to export the XML.
    bool withXML;
    /// \brief Specify a custom indentation.
    std::string indent;
    /// \brief Specify whether to include configuration customization support.
    bool includeConfigCustom;
};

}
}

#endif /*__SCRIPT_EXPORTER_OPTIONS_H__*/

