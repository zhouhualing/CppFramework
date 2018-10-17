#include "StdAfx.h"
#include <sstream>

#include "ScriptCommentProvider.h"
#include "SystemUtils.h"
#include "Path.h"
#include "Project.h"
#include "UserNote.h"
#include "MessageService.h"
#include "ClientInfo.h"
#include "StcSystem.h"

using namespace stc::framework;
using namespace std;

#define COMMENT_DELIM '#'
#define BEGIN_COM COMMENT_DELIM <<
#define COM_LINE << endl << COMMENT_DELIM <<

ScriptCommentProvider::ScriptCommentProvider(const ExporterOptions& options,
                                             const std::string& launcherFilename,
                                             const std::string& logicFilename,
                                             const std::string& timestamp) :
    _options(options),
    _launcherFilename(Path::GetFileName(launcherFilename)),
    _logicFilename(Path::GetFileName(logicFilename)),
    _timestamp(timestamp)
{

}

ScriptCommentProvider::~ScriptCommentProvider(){}

std::string ScriptCommentProvider::getLauncherComment() const
{
    if(_options.language == TCL)
    {
        return buildTclLauncherComment();
    }
    else
    {
        return buildNewLauncherComment();
    }
}

std::string ScriptCommentProvider::getLogicComment() const
{
    ostringstream comment;
comment <<
BEGIN_COM " Spirent TestCenter Logic Script"
COM_LINE  " Generated on " << _timestamp << " by " << ScriptCommentProvider::getUserName()
COM_LINE  " Framework ver. " << CStcSystem::Instance().GetVersion()
COM_LINE  ""
COM_LINE  " Comments: "
COM_LINE  " " << getUserNotes()
COM_LINE  ""
COM_LINE  " This logic script contains the following routines invoked from the"
COM_LINE  " " << _launcherFilename << " script.";
    return comment.str();
}

std::string ScriptCommentProvider::getInitProcComment() const
{
    ostringstream comment;
comment <<
BEGIN_COM "    init - set the logging level and logging location (stdout)."
COM_LINE  "           Possible logLevel values are: "
COM_LINE  "             DEBUG - Display DEBUG, INFO, WARN, and ERROR messages"
COM_LINE  "             INFO  - Display INFO, WARN, and ERROR messages"
COM_LINE  "             WARN  - Display WARN and ERROR messages"
COM_LINE  "             ERROR - Display only ERROR messages"
COM_LINE  ""
COM_LINE  "           Possible values for logTo are \"stdout\" or a file name (can include"
COM_LINE  "           the path). Use forward slashes between directory names.";

    return comment.str();
}

std::string ScriptCommentProvider::getConfigResultLocationComment() const
{
    assert(_options.language == TCL);
    ostringstream comment;
comment <<
BEGIN_COM "    configResultLocation -  set the location for results files."
COM_LINE  "            Possible values are: "
COM_LINE  "              INSTALL_DIR - Spirent TestCenter installation directory."
COM_LINE  "              CURRENT_WORKING_DIR - Current working directory. This "
COM_LINE  "                  is the directory that Spirent TestCenter currently"
COM_LINE  "                  has open."
COM_LINE  "              USER_WORKING_DIR - User working directory."
COM_LINE  "              CURRENT_CONFIG_DIR - Current configuration directory. "
COM_LINE  "                  This is the directory where the saved or loaded"
COM_LINE  "                  .xml or .tcc file is located. If no .xml or .tcc "
COM_LINE  "                  file has been saved or loaded, files are saved"
COM_LINE  "                  to the user working directory."
COM_LINE  ""
COM_LINE  "            The location of the results files can be modified in the"
COM_LINE  "            launcher file. The saveResultsRelativeTo parameter sets a path "
COM_LINE  "            that is prepended to the value of the ResultsDirectory "
COM_LINE  "            parameter. To set an fully qualified (absolute) path for "
COM_LINE  "            results, set the ResultsDirectory parameter and set "
COM_LINE  "            SaveResultsRelativeTo to NONE.";
    return comment.str();
}

std::string ScriptCommentProvider::getConfigMiscOptionsComment() const
{
    assert(_options.language == TCL);
    ostringstream comment;
comment <<
BEGIN_COM "    configMiscOptions - set up the sequencer. Currently sets the sequencer"
COM_LINE  "                        to stop on any error.  Other options are IGNORE_ERROR and "
COM_LINE  "                        PAUSE_ON_ERROR.";
    return comment.str();
}

std::string ScriptCommentProvider::getScriptFilenameSuffix() const
{
    size_t found = _launcherFilename.find_last_of('.');
    if (found == std::string::npos)
    {
        STC_ASSERT(0, "Unable to determine script filename suffix");
        return "";
    }
    return _launcherFilename.substr(found+1);
}

std::string ScriptCommentProvider::getConfigCustomComment() const
{
    std::string suffix = getScriptFilenameSuffix();

    ostringstream comment;
comment <<
BEGIN_COM "    configCustom - customize the configuration."
COM_LINE  "                   Currently sources a script file <filename>_custom." << suffix
COM_LINE  "                   where <filename> is the name of the launcher script "
COM_LINE  "                   without the suffix.";
    return comment.str();
}

std::string ScriptCommentProvider::getConfigProcComment() const
{
    ostringstream comment;

    if(_options.language == TCL && _options.withXML)
    {   
        // This is the old style Tcl generation which we could not change
        // for some reason. 
comment <<
BEGIN_COM "    config - load the configuration into memory. The port locations"
COM_LINE  "             are taken from the XML file but may be passed in from the"
COM_LINE  "             launcher script.";
    }
    else if((_options.language == TCL ) || (_options.language == PERL && !_options.withXML))
    {
comment <<
BEGIN_COM "    config - load the configuration into memory and set the STC port locations."
COM_LINE  "             The port locations can be modified in the launcher file. This "
COM_LINE  "             routine contains the majority of the object creation and attribute"
COM_LINE  "             setting code taken from the GUI configuration. Near the end of "
COM_LINE  "             this routine is a set of lines that specifies the protocol stack "
COM_LINE  "             linkages for both the stream blocks and any configured protocols. "
COM_LINE  "             Following that code is the command sequence construction code.";
    }
    else // Ruby and Perl with XML.
    {
comment <<
BEGIN_COM "    config - load the configuration into memory. The port locations"
COM_LINE  "             are taken from the XML file but may be passed in from the"
COM_LINE  "             launcher script. The XML config file may be passed in from"
COM_LINE  "             the launcher script as well."
COM_LINE  ""
COM_LINE  "           - set the location for results files."
COM_LINE  "             Possible values are: "
COM_LINE  "               INSTALL_DIR - Spirent TestCenter installation directory."
COM_LINE  "               CURRENT_WORKING_DIR - Current working directory. This "
COM_LINE  "                   is the directory that Spirent TestCenter currently"
COM_LINE  "                   has open."
COM_LINE  "               USER_WORKING_DIR - User working directory."
COM_LINE  "               CURRENT_CONFIG_DIR - Current configuration directory. "
COM_LINE  "                   This is the directory where the saved or loaded"
COM_LINE  "                   .xml or .tcc file is located. If no .xml or .tcc "
COM_LINE  "                   file has been saved or loaded, files are saved"
COM_LINE  "                   to the user working directory."
COM_LINE  ""
COM_LINE  "             The location of the results files can be modified in the"
COM_LINE  "             launcher file. The saveResultsRelativeTo parameter sets a path "
COM_LINE  "             that is prepended to the value of the ResultsDirectory "
COM_LINE  "             parameter. To set an fully qualified (absolute) path for "
COM_LINE  "             results, set the ResultsDirectory parameter and set "
COM_LINE  "             SaveResultsRelativeTo to NONE."
COM_LINE  ""
COM_LINE  "           - set up the sequencer. Currently sets the sequencer"
COM_LINE  "             to stop on any error.  Other options are IGNORE_ERROR and "
COM_LINE  "             PAUSE_ON_ERROR.";
    }
    return comment.str();
}

std::string ScriptCommentProvider::getConnectProcComment() const
{
    ostringstream comment;
comment <<
BEGIN_COM "    connect - perform the logical to physical port mapping, connect to the "
COM_LINE  "              chassis' and reserve the ports. This routine performs the connect,"
COM_LINE  "              reserve, and logical to physical port mappings directly."
COM_LINE  "              The port list is retrieved from the in-memory configuration.";
    return comment.str();
}

std::string ScriptCommentProvider::getApplyProcComment() const
{
    ostringstream comment;
comment <<
BEGIN_COM "    apply - apply writes the logical information held in memory on the "
COM_LINE  "            workstation to the ports in the STC chassis'.";
    return comment.str();
}

std::string ScriptCommentProvider::getRunProcComment() const
{
    ostringstream comment;
comment <<
BEGIN_COM "    run - subscribe to any results views located in the in-memory configuration"
COM_LINE  "          and execute the sequencer and return the test status from the "
COM_LINE  "          command sequence, if any. Test status is set by the Stopped Reason"
COM_LINE  "          in the Stop Command Sequence command. This is a string value and "
COM_LINE  "          can be anything. If there is no sequence defined or no Stop "
COM_LINE  "          Command Sequence command is executed, then the test state is "
COM_LINE  "          returned. Test state can take the values: NONE, PASSED or FAILED.";
    return comment.str();
}

std::string ScriptCommentProvider::getCleanupProcComment() const
{
    ostringstream comment;
comment <<
BEGIN_COM "    cleanup - release the ports, disconnect from the chassis' and reset "
COM_LINE  "              the in-memory configuration.";
    return comment.str();
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE AREA
///////////////////////////////////////////////////////////////////////////////
std::string ScriptCommentProvider::buildTclLauncherComment() const
{
    ostringstream comment;
comment <<
BEGIN_COM " Spirent TestCenter Launcher Script"
COM_LINE  " Generated on " << _timestamp << " by " << ScriptCommentProvider::getUserName()
COM_LINE  " Framework ver. " << CStcSystem::Instance().GetVersion()
COM_LINE  ""
COM_LINE  " Comments: "
COM_LINE  " " << getUserNotes()
COM_LINE  ""
COM_LINE  " This launcher script invokes the following routines contained in the"
COM_LINE  " " << _logicFilename << " script. After sourcing the logic script,"
COM_LINE  " the logic flow is as follows:"
COM_LINE  ""
COM_LINE  " init - set the logging level and logging location (stdout)."
COM_LINE  ""
COM_LINE  " config - load the configuration into memory and set the STC port locations."
COM_LINE  "          The port locations can be modified in this file."
COM_LINE  ""
COM_LINE  " configResultLocation -  set the location for results files."
COM_LINE  "                         The location of the results files can be modified in this file."
COM_LINE  "                         This does not set the location of the log files. Set "
COM_LINE  "                         the STC_LOG_OUTPUT_DIRECTORY environment variable to choose "
COM_LINE  "                         a log file location."
COM_LINE  ""
COM_LINE  " configMiscOptions - set up the sequencer."
COM_LINE  ""
COM_LINE  " connect - perform the logical to physical port mapping, connect to the "
COM_LINE  "           chassis' and reserve the ports."
COM_LINE  ""
COM_LINE  " apply - write the configuration to the ports."
COM_LINE  ""
COM_LINE  " run - execute the sequencer and obtain the test status from the "
COM_LINE  "       Basic: Stop Command Sequence \"Stopped Reason\" value. If there"
COM_LINE  "       is no sequence defined, then the test state is returned: NONE,"
COM_LINE  "       PASSED or FAILED."
COM_LINE  ""
COM_LINE  " cleanup - disconnect from the chassis (releases the ports) and reset "
COM_LINE  "           the in memory configuration."
COM_LINE  ""
COM_LINE  " return - return the test status obtained from the sequencer execution"
COM_LINE  "          to the caller.";

    return comment.str();
}

std::string ScriptCommentProvider::buildNewLauncherComment() const
{
    ostringstream comment;
comment <<
BEGIN_COM " Spirent TestCenter Launcher Script"
COM_LINE  " Generated on " << _timestamp << " by " << ScriptCommentProvider::getUserName()
COM_LINE  " Framework ver. " << CStcSystem::Instance().GetVersion()
COM_LINE  ""
COM_LINE  " Comments: "
COM_LINE  " " << getUserNotes()
COM_LINE  ""
COM_LINE  " This launcher script invokes the following routines contained in the"
COM_LINE  " " << _logicFilename << " script. After sourcing the logic script,"
COM_LINE  " the logic flow is as follows:"
COM_LINE  ""
COM_LINE  " init - set the logging level and logging location (stdout)."
COM_LINE  ""
COM_LINE  " config - set up various test configuration parameters including:"
COM_LINE  "          - load the configuration into memory and set the STC port locations."
COM_LINE  "            The port locations and the XML config file can be modified in this file."
COM_LINE  "          - set the location for results files."
COM_LINE  "            The location of the results files can be modified in this file."
COM_LINE  "            This does not set the location of the log files. Set "
COM_LINE  "            the STC_LOG_OUTPUT_DIRECTORY environment variable to choose "
COM_LINE  "            a log file location."
COM_LINE  "          - set up the sequencer."
COM_LINE  ""
COM_LINE  " connect - perform the logical to physical port mapping, connect to the "
COM_LINE  "           chassis' and reserve the ports."
COM_LINE  ""
COM_LINE  " apply - write the configuration to the ports."
COM_LINE  ""
COM_LINE  " run - execute the sequencer and obtain the test status from the "
COM_LINE  "       Basic: Stop Command Sequence \"Stopped Reason\" value. If there"
COM_LINE  "       is no sequence defined, then the test state is returned: NONE,"
COM_LINE  "       PASSED or FAILED."
COM_LINE  ""
COM_LINE  " cleanup - disconnect from the chassis (releases the ports) and reset "
COM_LINE  "           the in memory configuration."
COM_LINE  ""
COM_LINE  " return - return the test status obtained from the sequencer execution"
COM_LINE  "          to the caller.";

    return comment.str();
}

std::string ScriptCommentProvider::getUserName()
{
    string user;
    const CClientInfo* clientInfo = CMessageService::Instance().GetCurrentClientInfo();

    // Default to local user.
    if(clientInfo == NULL)
    {
        EnvGet("USERNAME", user);
        if(user.empty())
        {
            EnvGet("USER", user);
        }
    }
    else
    {
        user = clientInfo->GetClientStringId();
    }

    if(user.empty())
    {
        user = "unknown"; // Must have something for unit tests to pass.
    }

    return user;
}

std::string ScriptCommentProvider::getUserNotes()
{
    ostringstream notes;
    ScriptableVec vec;
    CProject::Instance().GetObjects(vec, CUserNote::CLASS_ID());
    ScriptableVec::iterator it = vec.begin();
    for(; it != vec.end(); ++it)
    {
        CUserNote* note = static_cast<CUserNote*> (*it);
        notes << note->GetNoteKey() << ": " << note->GetNoteValue() << endl << COMMENT_DELIM << ' ';
    }

    return notes.str();
}

