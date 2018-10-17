#include "StdAfx.h"
#include <sstream>
#include <iostream>

#include "Path.h"
#include "StringUtil.h"
#include "TclCodeGen.h"

using namespace std;
using namespace stc::framework;

TclCodeGen::TclCodeGen() :
    _options(),
    _commentProvider(ExporterOptions(), "", "", "")
{

}

TclCodeGen::~TclCodeGen()
{

}

void TclCodeGen::SetOptions(const ExporterOptions &options)
{
    _options = options;   
}

void TclCodeGen::SetScriptCommentProvider(const ScriptCommentProvider& commentProvider)
{
    _commentProvider = commentProvider;
}

std::string TclCodeGen::IntroComment(const std::string &time, const std::string &filename)
{
    ostringstream code;
    code << _commentProvider.getLogicComment() << endl << endl;   
    return code.str();
}

std::string TclCodeGen::ConnectReserveApply()
{
    const string INDENT = _options.indent;
    ostringstream code;

// body of the config sub ...
if(_options.withXML)
// Only needed for XML load since port creation will be configuring location as we go.   
{
   code << INDENT << "if {[llength $portLocations] > 0} {"
<< endl << INDENT << "    array set cmdResult [ stc::perform GetObjects -ClassName Port -Condition IsVirtual=false ]"
<< endl << INDENT << "    set ports $cmdResult(-ObjectList)"
<< endl << INDENT << "    set idx 0"
<< endl << INDENT << "    foreach port $ports {"
<< endl << INDENT << "        stc::config $port -location [ lindex $portLocations $idx ]"
<< endl << INDENT << "        incr idx"
<< endl << INDENT << "    }"
<< endl << INDENT << "}"
<< endl << endl;    
}

// at the end of config proc
   code << INDENT << "# Save to an XML file, if desired"
<< endl << INDENT << "#stc::perform saveAsXml -config system1 -filename sampleSavedFilename.xml"
<< endl << INDENT << ""

<< endl << INDENT << "# Save to a Tcl file, if desired"
<< endl << INDENT << "#stc::perform saveAsTcl -config system1 -filename sampleSavedFilename.tcl"
<< endl << endl << "}"
<< endl

// connect proc
<< endl << _commentProvider.getConnectProcComment()
<< endl << "proc " << _options.strPrefix << "connect {} {"
<< endl << INDENT  << "stc::perform attachPorts -autoConnect true -portList [ stc::get project1 -children-Port ]"
<< endl << "}"
<< endl

// apply proc
<< endl << _commentProvider.getApplyProcComment()
<< endl << "proc " << _options.strPrefix << "apply {} {"
<< endl << INDENT << "stc::apply"
<< endl << "}"
<< endl

// beginning of run proc
<< endl << _commentProvider.getRunProcComment()
<< endl << "proc " << _options.strPrefix << "run {} {";

    return code.str();
}

std::string TclCodeGen::Subscribe(const std::string& filenamePrefix,
                                  const std::string& parent,
                                  const std::string& resultParent,
                                  const std::string& configType,
                                  const std::string& resultType,
                                  const std::string& filterList,
                                  const std::string& viewAttributeList)
{
    const string INDENT = _options.indent;
    ostringstream code;
    code
<< endl << INDENT << "# Subscribe to results for result query " << filenamePrefix
<< endl << INDENT << "stc::subscribe -parent " << parent << " \\"
<< endl << INDENT << INDENT << "-resultParent " << resultParent << " \\"
<< endl << INDENT << INDENT << "-configType " << configType << " \\"
<< endl << INDENT << INDENT << "-resultType " << resultType << " \\"
<< endl << INDENT << INDENT << "-filterList " << "\"" << filterList << "\" \\"
<< endl << INDENT << INDENT << "-viewAttributeList " << "\"" << viewAttributeList << "\" \\"
<< endl << INDENT << INDENT << "-interval 1 -filenamePrefix \"" << filenamePrefix << "\""
<< endl;
    return code.str();
}

std::string TclCodeGen::ChartSubscribe()
{
    const string INDENT = _options.indent;
    ostringstream code;
    code
<< endl << INDENT << "# Subscribe to the result query charts."
<< endl << INDENT << "foreach hResultDataSet [stc::get [stc::get system1 -children-Project] -children-resultchartqueryset] {"
<< endl << INDENT << INDENT << "stc::perform ResultDataSetSubscribeCommand -ResultDataSet $hResultDataSet "
<< endl << INDENT << "}"
<< endl << endl;

    return code.str();
}

std::string TclCodeGen::Disconnect()
{
    const string INDENT = _options.indent;
    ostringstream code;
    code
<< endl << INDENT << "# Start the sequencer"
<< endl << INDENT << "stc::perform sequencerStart"
<< endl
<< endl << INDENT << "# Wait for sequencer to finish"
<< endl << INDENT << "stc::waituntilcomplete"
<< endl
	//******************************************************
	// check the sequencer status and testState  
<< endl << INDENT << "# check the sequencer status and test state"
<< endl << INDENT << "set sqrHandle [stc::get System1 -Children-Sequencer]"
<< endl << INDENT << "set sqrStatus [stc::get $sqrHandle -Status]"
<< endl << INDENT << "set sqrTestState [stc::get $sqrHandle -TestState]"
<< endl << INDENT << "if { $sqrStatus eq \"\" }  {"
<< endl << INDENT << "    return $sqrTestState"
<< endl << INDENT << "} else {"
<< endl << INDENT << "    return $sqrStatus"
<< endl << INDENT << "}"
<< endl
	//******************************************************
<< endl << "}"
<< endl

// disconnect proc
<< endl << _commentProvider.getCleanupProcComment()
<< endl << "proc " << _options.strPrefix << "cleanup {} {"
<< endl << INDENT  << "stc::perform chassisDisconnectAll "
<< endl << INDENT  << "stc::perform resetConfig"
<< endl << "}";

    // end namespace
    if(_options.strNamespace.length() != 0)
    {
code << endl << "}" << endl;
    }

    return code.str();
}

std::string TclCodeGen::Init(const std::string &logTo, const std::string &logLevel, const std::string &errorHandler)
{
    const string INDENT = _options.indent;
    ostringstream code;

   code << "# Load Spirent TestCenter"
<< endl << "package require SpirentTestCenter"
<< endl << "#source SpirentTestCenter.tcl"
<< endl << endl;

    if(_options.strNamespace.length() != 0)
    {
   code << "namespace eval " << _options.strNamespace << " { "
<< endl << endl;
    }

// init proc
code << _commentProvider.getInitProcComment()
<< endl << "proc " << _options.strPrefix << "init {} {"
<< endl << INDENT << "stc::config automationoptions -logTo " << logTo << " -logLevel " << logLevel
<< endl << "}"
<< endl

// configResultLocation proc
<< endl << _commentProvider.getConfigResultLocationComment()
<< endl << "proc " << _options.strPrefix << "configResultLocation  { location } {"
<< endl << INDENT << "set TestResultSetting(1) [lindex [stc::get System1.Project -children-TestResultSetting] 0]"
<< endl << INDENT << "stc::config $TestResultSetting(1) -saveResultsRelativeTo NONE -resultsDirectory $location"
<< endl << "}"
<< endl

// configMiscOptions proc
<< endl << _commentProvider.getConfigMiscOptionsComment()
<< endl << "proc " << _options.strPrefix << "configMiscOptions  {} {"
<< endl << INDENT << "set Sequencer(1) [lindex [stc::get system1 -children-sequencer] 0]"
<< endl << INDENT << "stc::config $Sequencer(1) -ErrorHandler " << errorHandler
<< endl << "}"
<< endl

// config proc begin
<< endl << _commentProvider.getConfigProcComment()
<< endl << "proc " << _options.strPrefix << "config {portLocations} {"
<< endl;

    return code.str();
}

std::string TclCodeGen::LauncherScript(const std::string &time, const std::string &filename, const std::string &logicFilename, const std::vector<std::string>& portLocations)
{
    const string INDENT = _options.indent;    
    const string PREFIX = GetFullPrefix();
    
    ostringstream code;
    code << _commentProvider.getLauncherComment() << endl << endl;
    
	// check the ProcedurePrefix is namespace or not
	if (!_options.strPrefix.empty())
	{
		size_t pos = _options.strPrefix.find("::");
		if (pos != std::string::npos)
		{
				code << "# Declare the Namespace  "
				<< endl << "namespace eval " << PREFIX << " {}"
				<< endl << endl;
		}
	}

    string basePathMap = BasePathMapDeclaration();
    code << basePathMap;

   code << "source [ file normalize [ file join [ file dirname [ info script ] ] {" << logicFilename << "} ] ]"
<< endl << endl;

    if (_options.includeConfigCustom)
    {
        // configCustom proc
        code
        << endl << _commentProvider.getConfigCustomComment()
        << endl << "proc " << "configCustom  {} {"
        << endl << INDENT << "set scriptFile [ regsub \"\\.tcl$\" [info script] \"_custom.tcl\" ]"
        << endl << INDENT << "if [file exists $scriptFile] {"
        << endl << INDENT << INDENT << "uplevel #0 [list source $scriptFile]"
        << endl << INDENT << "}"
        << endl << "}"
        << endl;
    }

    code
<< endl << PREFIX << "init"
<< endl << PREFIX << "config" << " [list ";

    // port locations
    vector<string>::const_iterator it = portLocations.begin();
    for(; it != portLocations.end(); ++it)
    {
        code << "\"" << *it << "\" ";
    }

    code << "]"
<< endl << PREFIX << "configResultLocation" << " [ file normalize [ file dirname [ info script ] ] ]"
<< endl << PREFIX << "configMiscOptions";
    if (_options.includeConfigCustom)
    {
        code << endl << PREFIX << "configCustom";
    }
    code
<< endl << PREFIX << "connect"
<< endl << PREFIX << "apply"
<< endl << "set test_status [eval [concat " << PREFIX << "run" << " ]]"
<< endl;

    if (_options.strBeforeDisconnectScript != "") 
    {
        cout << endl;
        cout << "# Invoke this user-provided script before terminating" << endl;
        if (Path::IsRelativePath(_options.strBeforeDisconnectScript))
        {
            cout << "source [ file normalize [ file join [ file dirname [ info script ] ] {" << _options.strBeforeDisconnectScript << "} ]" << endl;
        }
        else
        {
            cout << "source {" << _options.strBeforeDisconnectScript << "}" << endl;
        }
        code << endl;
    }

   code << PREFIX << "cleanup"
<< endl << "return $test_status"
<< endl;
    return code.str();
}

std::string TclCodeGen::ConnectRelationshipTargets(const std::string& configStmt, 
                                                   const std::vector<std::string>& targets,
                                                   ValueType_t type)
{
    ostringstream code;
    code << configStmt;
    vector<string>::const_iterator it = targets.begin();
    for(; it != targets.end(); ++it)
    {
        code << " ";
        if(type == VARIABLE)
        {
            code << '$';
        }
        code << *it;
    }

    code << " \"";
    return code.str();
}

std::string TclCodeGen::DeleteChildren(const std::string& parent)
{
    const string INDENT = _options.indent;
    ostringstream code;
   code << "foreach child [stc::get $" << parent << " -children] {"
<< endl << INDENT << "stc::delete $child"
<< endl << "}"
<< endl;
    return code.str();
}

std::string TclCodeGen::Property(const std::string& propName, const std::string& propValue, ValueType_t type)
{
    const string INDENT = _options.indent + _options.indent;
    
    string myValue(propValue);    
    // Prevent escaping the close bracket when we have path names.
    if(type == STRING_LITERAL && StringUtil::EndsWith(myValue, "\\"))
    {
        const string tempPath = StringUtil::TrimFromEnd(myValue, "\\");
        if((tempPath.length() == 2 && tempPath[1] == ':') || 
            Path::DirectoryExists(tempPath))
        {
             myValue = tempPath;
        }
    }                 

    // propValue can be a variable name, but here we make no distinction.
    myValue = ( type == STRING_LITERAL ) ? "{" + myValue + "}" : "\"" + myValue + "\""; 
    ostringstream code;
   code << " \\"
<< endl << INDENT << "-" << propName << " " << myValue;
    return code.str();  
}

std::string TclCodeGen::PropertyLocation(int portIdx)
{
    const string INDENT = _options.indent + _options.indent;
    ostringstream code;    
   code << " \\"
<< endl << INDENT << "-Location [lindex $portLocations " << portIdx << "]";
    return code.str();
}

std::string TclCodeGen::DanForObject(const std::string &code, const std::string &type, int idx)
{
    ostringstream dan;
    dan << "." << type << "(" << idx << ")" << code;
    return dan.str();
}

std::string TclCodeGen::HandleForChildObject(const std::string& codeForParent, 
                                             const std::string& childType, int idx, ValueType_t type)
{
    ostringstream code;
    string myCodeForParent = ( type == VARIABLE ) ? "$" + codeForParent : codeForParent;
    code << "[lindex [stc::get " << myCodeForParent << " -children-" << childType << "] " << idx << "]";
    return code.str();
}

std::string TclCodeGen::Config(const std::string& objectName, const Properties& props, 
                               ValueType_t typeOfObject, ValueType_t typeOfValues, bool closeStmt)
{
    ostringstream code;
    string myObjectName = ( typeOfObject == VARIABLE ) ? "$" + objectName : objectName;
    code << "stc::config " << myObjectName;
    Properties::const_iterator it = props.begin();
    for(; it != props.end(); ++it)
    {
        code << " -" << it->first << " ";
        vector<string> values = it->second;
        if(!values.empty() && typeOfValues != LITERAL)
        {
            code << "\"";
        }
        vector<string>::const_iterator it_values = values.begin();
        for(; it_values != values.end(); ++it_values)
        {
            if(typeOfValues == VARIABLE)
            {
                code << '$';
            }

            code << *it_values;
            if(it_values != values.end() - 1)
            {
                code << " ";
            }
        }

        if(typeOfValues != LITERAL)
        {       
            code << "\"";
        }
    }

    // Ignoring the closeStmt flag in Tcl. Not needed.

    return code.str();
}

std::string TclCodeGen::ConfigRelationship(const std::string& objectName, const std::string& relName)
{
    // Just pass params directly to regular Config template; no difference in config statements in Tcl.
    Properties props;
    vector<string> values;
    props[relName] = values;
    return Config(objectName, props, VARIABLE, STRING_LITERAL, false);    
}

std::string TclCodeGen::Create(const std::string& varName, const std::string& type,
                               const std::string& parent, ValueType_t parentType)
{
    const string INDENT = _options.indent + _options.indent;
    ostringstream code;
    code << "set " << varName << " [stc::create \"" << type << "\"";
    if(!parent.empty())
    {
        code <<  " \\" << endl << INDENT << "-under ";
        if(parentType == VARIABLE)
        {
            code << '$';
        }
        code << parent;
    }       

    return code.str();
}

std::string TclCodeGen::CloseCreate()
{
    return " ]";
}

std::string TclCodeGen::CloseConfig()
{
    // Not needed in Tcl.
    return "";
}

std::string TclCodeGen::XmlLoad(const std::string& xmlFilename)
{
    const string INDENT = _options.indent;
    ostringstream code;
   code
<< endl << INDENT << "set " << "StcSystem(1) [ stc::perform loadfromxml -filename [ file normalize [ file join [ file dirname [ info script ] ] {" << xmlFilename << "} ] ] ]"
<< endl;
    if(!_options.pathReplacements.empty())
    {    
        code << INDENT << "set seq [lindex [stc::get system1 -children-sequencer] 0]"
     << endl << INDENT << "stc::perform filepathreplacement -RootObject $seq -PathReplacements [ array get ::basePathMap ]"
     << endl;
    }

    code << endl;
    return code.str();
}

std::string TclCodeGen::BasePathMapDeclaration()
{
    ostringstream code;
    if(!_options.pathReplacements.empty())
    {    
        map<string, string>::iterator it = _options.pathReplacements.begin();
        for(; it != _options.pathReplacements.end(); ++it)
        {
            string replace = it->first;        
            string with = it->second.empty() ? "\"\"" : it->second;
            
            code << "set { basePathMap(" << replace << ") } { " << with << " }" << endl;        
        }
    
        // assume this is going to be a full Unix path separator
        // replacement based on the first entry.       
        string with = _options.pathReplacements.begin()->second;
        if(with.find("/") != -1)
        {
            code << "set basePathMap(\\\\) /" << endl;
        }
    }

    return code.str();
}

std::string TclCodeGen::FilePathSubstitution(const std::string &varName, const std::string &filename)
{
    ostringstream code;
    code << "set " << varName << " [string map [ array get ::basePathMap ] {" << filename << "} ]" << endl;
    return code.str();
}

std::string TclCodeGen::Comment(const std::string& comment)
{
    ostringstream code;
    code << "# " << comment << endl;
    return code.str();
}

std::string TclCodeGen::Assignment(const std::string& varName, 
                                   const std::string& value,
                                   const std::string& varType,                                   
                                   ValueType_t type)
{
    ostringstream code;
    code << "set " << varName << " ";
    if(type == VARIABLE)
    {
        code << '$';
    }

    code << value << endl;

    return code.str();
}

std::string TclCodeGen::CreateVarName(const std::string& baseVarName, int idx)
{
    ostringstream code;
    code << baseVarName << '(' << idx << ')';
    return code.str();
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE AREA
///////////////////////////////////////////////////////////////////////////////

std::string TclCodeGen::GetFullPrefix()
{
    string prefix = _options.strNamespace;
    if(!prefix.empty())
    {
        prefix.append("::");
    }
    prefix.append(_options.strPrefix);
    return prefix;
}


