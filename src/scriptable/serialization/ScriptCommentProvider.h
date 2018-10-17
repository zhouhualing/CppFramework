#ifndef __SCRIPT_COMMENT_PROVIDER_H__
#define __SCRIPT_COMMENT_PROVIDER_H__

#include "ScriptExporterOptions.h"

namespace stc {
namespace framework {

/**
 * Provides script level and proc level comments for script
 * exporter.
 */
class ScriptCommentProvider
{
public:
    ScriptCommentProvider(const ExporterOptions& options, 
                          const std::string& launcherFilename, 
                          const std::string& logicFilename,
                          const std::string& timestamp);                          

    ~ScriptCommentProvider();

    std::string getLauncherComment() const;
    std::string getLogicComment() const;
    std::string getInitProcComment() const;    
    std::string getConfigResultLocationComment() const;
    std::string getConfigMiscOptionsComment() const;
    std::string getConfigCustomComment() const;
    std::string getConfigProcComment() const;
    std::string getConnectProcComment() const;
    std::string getApplyProcComment() const;
    std::string getRunProcComment() const;
    std::string getCleanupProcComment() const;    

private:
    ExporterOptions          _options;
    std::string              _launcherFilename;
    std::string              _logicFilename;
    std::string              _timestamp;    

    std::string buildTclLauncherComment() const; // Preserved for legacy code generation...
    std::string buildNewLauncherComment() const;
    std::string getScriptFilenameSuffix() const;

    static std::string getUserName();
    static std::string getUserNotes();    
};

}
}

#endif /* __SCRIPT_COMMENT_PROVIDER_H__ */

