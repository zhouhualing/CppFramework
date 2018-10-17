#include "StdAfx.h"

#include "PerlCodeGen.h"
#include "StringUtil.h"
#include <sstream>

using namespace std;
using namespace stc::framework;

#define INDENT _options.indent
#define INDENTx2 _options.indent << _options.indent
#define INDENTx3 _options.indent << _options.indent << _options.indent

PerlCodeGen::PerlCodeGen() :
    _options(),
    _commentProvider(ExporterOptions(), "", "", "")
{

}

PerlCodeGen::~PerlCodeGen()
{

}

void PerlCodeGen::SetOptions(const stc::framework::ExporterOptions &options)
{
    _options = options;
    if(_options.strNamespace.empty())
    {
        _options.strNamespace = "StcTest";
    }
}

void PerlCodeGen::SetScriptCommentProvider(const stc::framework::ScriptCommentProvider& commentProvider)
{
    _commentProvider = commentProvider;
}

std::string PerlCodeGen::IntroComment(const std::string &time, const std::string &filename)
{
    ostringstream code;
    code << _commentProvider.getLogicComment() << endl << endl;   
    return code.str();
}

std::string PerlCodeGen::Init(const std::string &logTo, const std::string &logLevel, const std::string &errorHandler)
{    
    ostringstream code;

   code << "package " << _options.strNamespace << ";"
<< endl
<< endl << "use strict;"
<< endl
<< endl << "# Load Spirent TestCenter"
<< endl << "use SpirentTestCenter;"
<< endl << "our $stc = new StcPerl;"
<< endl << endl;

// init proc
   code << _commentProvider.getInitProcComment()
<< endl << "sub " << _options.strPrefix << "init {"
<< endl << INDENT << "$stc->config('automationoptions', logTo=>'" << logTo << "', logLevel=>'" << logLevel << "');"
<< endl << "}"
<< endl;

// Create a string_map proc if needed which is equivalent to Tcl's "string map"
if(!_options.pathReplacements.empty())
{
code
<< endl << "sub string_map { "
<< endl << INDENT <<  "my ($str, %lookup) = @_;"
<< endl << INDENT <<  "my $re_str = join \"|\", map {quotemeta($_)} reverse sort keys %lookup;"
<< endl << INDENT <<  "my $re = qr/($re_str)/;"
<< endl << INDENT <<  "$str =~ s/$re/$lookup{$1}/g;"
<< endl << INDENT <<  "return $str;"
<< endl << "}"
<< endl;
}

// config proc begin
code
<< endl << _commentProvider.getConfigProcComment()
<< endl << "sub " << _options.strPrefix << "config {";
if(_options.withXML)
{
code 
<< endl << INDENT << "my($configFile, $resultsDir, @portLocations) = @_;"
<< endl;
}
else
{
code 
<< endl << INDENT << "my($resultsDir, @portLocations) = @_;"
<< endl;
}

   return code.str();
}

std::string PerlCodeGen::XmlLoad(const std::string &xmlFilename)
{
   ostringstream code;             
   code << INDENT << "$stc->perform('loadfromxml', filename=>$configFile);"
<< endl << endl;   

   if(!_options.pathReplacements.empty())
    {    
        code << INDENT << "my $seq = ($stc->get( 'system1', 'children-sequencer' ))[0];"
     << endl << INDENT << "my @basePathPairs = %::basePathMap;"
     << endl << INDENT << "$stc->perform( 'filepathreplacement', RootObject=>$seq, PathReplacements=>\"@basePathPairs\" );"
     << endl << endl;
    }

   return code.str();
}

std::string PerlCodeGen::ConnectReserveApply()
{    
    ostringstream code;

// body of the config sub ...
if(_options.withXML)
// Only needed for XML load since port creation will be configuring location as we go.   
{
   code << INDENT << "if(scalar(@portLocations) != 0) {"
<< endl << INDENT << "    my %cmdResult = $stc->perform('GetObjects', ClassName=>'Port', Condition=>'IsVirtual=false');"
<< endl << INDENT << "    my @ports = split(/ +/, $cmdResult{ObjectList});"
<< endl << INDENT << "    my $idx = 0;"
<< endl << INDENT << "    foreach my $port (@ports) {"
<< endl << INDENT << "        $stc->config($port, location=>$portLocations[$idx]);"
<< endl << INDENT << "        $idx+=1;"
<< endl << INDENT << "    }"
<< endl << INDENT << "}"
<< endl << endl;
}

   code << INDENT << "$stc->config('project1.testResultSetting', saveResultsRelativeTo=>'NONE', resultsDirectory=>$resultsDir);"
<< endl << INDENT << "$stc->config('system1.sequencer', errorHandler=>'STOP_ON_ERROR');"
<< endl << "}"
<< endl

// connect proc
<< endl << _commentProvider.getConnectProcComment()
<< endl << "sub " << _options.strPrefix << "connect {"
<< endl << INDENT  << "$stc->perform('attachPorts');"
<< endl << "}"
<< endl

// apply proc
<< endl << _commentProvider.getApplyProcComment()
<< endl << "sub " << _options.strPrefix << "apply {"
<< endl << INDENT << "$stc->apply();"
<< endl << "}"
<< endl

// beginning of run proc
<< endl << _commentProvider.getRunProcComment()
<< endl << "sub " << _options.strPrefix << "run {";
   
   return code.str();
}

std::string PerlCodeGen::HandleForChildObject(const std::string &codeForParent, const std::string &childType, int idx, ValueType_t type)
{    
    ostringstream code;
    string myCodeForParent = ( type == VARIABLE ) ? "$" + codeForParent : "'" + codeForParent + "'";
    code << "($stc->get( " << myCodeForParent << ", 'children-" << childType << "' ))[" << idx << "]";
    return code.str();
}

std::string PerlCodeGen::DanForObject(const std::string &code, const std::string &type, int idx)
{
    ostringstream dan;
    dan << "." << type << "(" << idx << ")" << code;
    return dan.str();
}

std::string PerlCodeGen::Subscribe(const std::string &filenamePrefix, 
                                   const std::string &parent, 
                                   const std::string &resultParent, 
                                   const std::string &configType,
                                   const std::string &resultType, 
                                   const std::string &filterList,
                                   const std::string &viewAttributeList)
{    
    string myFilterList = (filterList.empty()) ? "''" : filterList;
    ostringstream code;
    code 
<< endl << INDENT << "# Subscribe to results for result query " << filenamePrefix
<< endl << INDENT << "$stc->subscribe(parent=>'project1',"
<< endl << INDENT << INDENT << "resultParent=>'project1',"
<< endl << INDENT << INDENT << "configType=>'" << configType << "',"
<< endl << INDENT << INDENT << "resultType=>'" << resultType << "',"
<< endl << INDENT << INDENT << "filterList=>" << myFilterList << ","
<< endl << INDENT << INDENT << "viewAttributeList=>'" << viewAttributeList << "',"
<< endl << INDENT << INDENT << "interval=>'1', filenamePrefix=>'" << filenamePrefix << "');"
<< endl;
    return code.str();    
}

std::string PerlCodeGen::ChartSubscribe()
{    
    ostringstream code;
    code
<< endl << INDENT << "# Subscribe to the result query charts."
<< endl << INDENT << "foreach my $hResultDataSet ($stc->get('project1', 'children-resultchartqueryset')) {"
<< endl << INDENT << INDENT << "$stc->perform('ResultDataSetSubscribeCommand', ResultDataSet=>$hResultDataSet);"
<< endl << INDENT << "}"
<< endl;

    return code.str();
}

std::string PerlCodeGen::Disconnect()
{    
    ostringstream code;
    code
<< endl << INDENT << "# Start the sequencer"
<< endl << INDENT << "$stc->perform('sequencerStart');"
<< endl
<< endl << INDENT << "# Wait for sequencer to finish"
<< endl << INDENT << "my $testState = $stc->waituntilcomplete();"
<< endl << INDENT << "return $testState;"
<< endl << "}"
<< endl

// cleanup proc
<< endl << _commentProvider.getCleanupProcComment()
<< endl << "sub " << _options.strPrefix << "cleanup {"
<< endl << INDENT  << "$stc->perform('chassisDisconnectAll');"
<< endl << INDENT  << "$stc->perform('resetConfig');"
<< endl << "}";
    
    return code.str();
}

std::string PerlCodeGen::LauncherScript(const std::string &time, const std::string &filename, const std::string &logicFilename, const std::vector<std::string> &portLocations)
{    
    ostringstream code;
   code << _commentProvider.getLauncherComment()   
<< endl
<< endl << "use File::Spec;"
<< endl;

    string basePathMapDeclare = BasePathMapDeclaration();
    code << basePathMapDeclare

<< endl << "($logicFile = __FILE__) =~ s/\\.pl/_logic.pl/;"
<< endl << "require $logicFile;"
<< endl
<< endl << "package " << _options.strNamespace << ";"
<< endl << endl;

    if (_options.includeConfigCustom)
    {
        // configCustom proc
        code << _commentProvider.getConfigCustomComment()
        << endl << "sub " << "configCustom {"
        << endl << INDENT << "(my $scriptFile = __FILE__) =~ s/\\.pl$/_custom.pl/;"
        << endl << INDENT << "if (-e $scriptFile) {"
        << endl << INDENT << INDENT << "unless (my $result = do $scriptFile) {"
        << endl << INDENT << INDENT << INDENT << "die \"Failed running script $scriptFile: $@\" if $@;"
        << endl << INDENT << INDENT << "}"
        << endl << INDENT << "}"
        << endl << "}"
        << endl << endl;
    }

    code
<< endl << "sub runTest {"
<< endl << INDENT << _options.strNamespace << "::" << _options.strPrefix << "init();"
<< endl << INDENT << "($vol,$dir,$file) = File::Spec->splitpath( __FILE__ );"
<< endl;

   if(_options.withXML)
   {  
        string xmlFilename = StringUtil::Replace(filename, ".pl", ".xml");
        code << INDENT << _options.strNamespace << "::" << _options.strPrefix << "config( File::Spec->catpath( $vol, $dir, '" << xmlFilename << "' ),  $dir, ( ";
   }
   else
   {
       code << INDENT << _options.strNamespace << "::"  << _options.strPrefix << "config( $dir, ( ";
   }

    // port locations
    vector<string>::const_iterator it = portLocations.begin();
    for(; it != portLocations.end(); ++it)
    {        
        code << "'" << *it << "'";
        if(it != portLocations.end() - 1)
        {
            code << ", ";
        }
    }
    
   code << " ) );";

   if(_options.includeConfigCustom)
   {
        code << endl << INDENT << "configCustom();";
   }

   code
<< endl << INDENT << _options.strNamespace << "::"  << _options.strPrefix << "connect();"
<< endl << INDENT << _options.strNamespace << "::" << _options.strPrefix << "apply();"
<< endl << INDENT << "$testState = " << _options.strNamespace << "::"  << _options.strPrefix << "run();";

if (_options.strBeforeDisconnectScript != "") 
{
code
<< endl
<< endl << INDENT << "# Invoke this user-provided script before terminating"
<< endl << INDENT << "require \"" << _options.strBeforeDisconnectScript << "\";"
<< endl;
}

code
<< endl << INDENT << _options.strNamespace << "::" << _options.strPrefix << "cleanup();"
<< endl << INDENT << "return $testState;"
<< endl << "}"
<< endl
<< endl << "unless (caller) {"
<< endl << INDENT << "if(" << _options.strNamespace << "::runTest() eq 'FAILED') {"
<< endl << INDENT << INDENT << "exit(1);"
<< endl << INDENT << "}"
<< endl << INDENT << "else {"
<< endl << INDENT << INDENT << "exit(0);"
<< endl << INDENT << "}"
<< endl << "}"
<< endl;
    return code.str();
}

std::string PerlCodeGen::Assignment(const std::string &varName, const std::string &value, const std::string &varType, ValueType_t type)
{
    ostringstream code;

    string myValue;
    switch(type)
    {
    case stc::framework::VARIABLE:
        myValue = '$' + value;
        break;  
    case stc::framework::STRING_LITERAL:        
        myValue = "'" + value + "'";
        break;
    default:
        myValue = value;
    }
    
    code << _options.indent << "my $" << varName << " = " << myValue << ";" << endl;
    return code.str();
}

std::string PerlCodeGen::CreateVarName(const std::string& baseVarName, int idx)
{
    ostringstream code;
    code << baseVarName << '_' << idx;
    return code.str();
}

// Goes in the launcher script ...
std::string PerlCodeGen::BasePathMapDeclaration()
{       
    ostringstream code;
    if(!_options.pathReplacements.empty())
    {   
        const string SPACES = "                     ";
        code << endl << "our %basePathMap = ( ";
        map<string, string>::const_iterator it = _options.pathReplacements.begin();
        for(; it != _options.pathReplacements.end(); ++it)
        {
            string replace = StringUtil::Replace(it->first, "\\", "\\\\");
            string with = it->second.empty() ? "\"\"" : StringUtil::Replace(it->second, "\\", "\\\\");

            code << "\"" << replace << "\" => \"" << with << "\","
            << endl << SPACES;
        }

        // Remove last comma and new line and spacing ...
        code.seekp((long)code.tellp() - (SPACES.length() + 2));
    
        // assume this is going to be a full Unix path separator
        // replacement based on the first entry.       
        string with = _options.pathReplacements.begin()->second;
        if(with.find("/") != -1)
        {
            code << "," << endl
                 << SPACES << "\"\\\\\" => '/'";
        }

        code << " );" << endl;
    }

    return code.str();
}

// Goes in the logic script ...
std::string PerlCodeGen::FilePathSubstitution(const std::string &varName, const std::string &filename)
{    
    ostringstream code;
    code << _options.indent << "my $" << varName << " = string_map( '" << filename << "', %::basePathMap );"
    << endl;
    return code.str();
}

std::string PerlCodeGen::CloseCreate()
{
    return " );";
}

std::string PerlCodeGen::CloseConfig()
{
    return " );";
}

std::string PerlCodeGen::Comment(const std::string &comment)
{
    ostringstream code;
    code << "# " << comment << endl;
    return code.str();
}

std::string PerlCodeGen::Config(const std::string &objectName, const Properties &props, ValueType_t typeOfObject, ValueType_t typeOfValues, bool closeStmt)
{
    ostringstream code;
    string myObjectName = ( typeOfObject == VARIABLE ) ? "$" + objectName : "'" + objectName + "'";
    code << _options.indent << "$stc->config( " << myObjectName;

    // If we're closing the statement here 
    // ( as opposed to Property template getting called for each property type/value pair ) we need the comma here ...
    if(closeStmt)
    {
        code << ",";
    }

    Properties::const_iterator it = props.begin();
    for(; it != props.end(); ++it)
    {
        code << " " << it->first << "=>";
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

        code  << ", ";
    }

    // Remove the last comma ...
    if(props.size() != 0)
    {   
        code.seekp((long)code.tellp() - 2);
    }    

    if(closeStmt)
    {
        string close = CloseConfig();
        code << close;
    }

    return code.str();
}

std::string PerlCodeGen::ConfigRelationship(const std::string& objectName, const std::string& relName)
{
    ostringstream code;    
    code << _options.indent << "$stc->config( $" << objectName << ", '" << relName << "',";
    return code.str();
}

std::string PerlCodeGen::ConnectRelationshipTargets(const std::string &configStmt, const std::vector<std::string> &targets, ValueType_t type)
{
    ostringstream code;
    code << configStmt;
    vector<string>::const_iterator it = targets.begin();
    code << " \"";
    for(; it != targets.end(); ++it)
    {        
        code << " ";
        if(type == VARIABLE)
        {
            code << '$';
        }
        code << *it;
    }

    code << " \" );";    
    return code.str();
}

std::string PerlCodeGen::Create(const std::string &varName, const std::string &type, const std::string &parent, ValueType_t parentType)
{    
    ostringstream code;
    code << _options.indent << "my $" << varName << " = $stc->create( '" << type << "'";    
    if(!parent.empty())
    {
        code <<  "," << endl << INDENTx3 << "under=>";
        if(parentType == VARIABLE)
        {
            code << '$';
        }
        code << parent;
    }       

    return code.str();
}

std::string PerlCodeGen::DeleteChildren(const std::string &parent)
{    
    ostringstream code;
    code 
<< _options.indent << "foreach my $child ($stc->get( $" << parent << ", 'children' )) {"
        << endl << INDENTx2 << "$stc->delete($child);"
<< endl << _options.indent << "}"
<< endl << endl;
    return code.str();
}

std::string PerlCodeGen::GetVariableSigil()
{
    return "$";
}

std::string PerlCodeGen::Property(const std::string &propName, const std::string &propValue, ValueType_t type)
{    
    string myValue = ( type == STRING_LITERAL ) ? "'" + StringUtil::Replace(propValue, "'", "\\'") + "'" : "\"" + propValue + "\""; 
    ostringstream code;
    code << ","
<< endl << INDENTx3 << "" << propName << "=>" << myValue;
    return code.str();  
}

std::string PerlCodeGen::PropertyLocation(int portIdx)
{    
    ostringstream code;    
    code << ","
<< endl << INDENTx3 << "Location=>@portLocations[" << portIdx << "]";
    return code.str();
}

