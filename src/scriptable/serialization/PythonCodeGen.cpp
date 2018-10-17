#include "StdAfx.h"

#include "PythonCodeGen.h"
#include "StcException.h"
#include "StringUtil.h"
#include <sstream>

using namespace std;
using namespace stc::framework;

#define INDENT _options.indent
#define INDENTx2 _options.indent << _options.indent
#define INDENTx3 _options.indent << _options.indent << _options.indent

PythonCodeGen::PythonCodeGen() :
    _options(),
    _commentProvider(ExporterOptions(), "", "", "")
{

}

PythonCodeGen::~PythonCodeGen()
{

}

void PythonCodeGen::SetOptions(const stc::framework::ExporterOptions &options)
{
    _options = options;
    if(_options.strNamespace.empty())
    {
        _options.strNamespace = "StcTest";
    }

    if(_options.withXML == false)
    {
        throw CStcInvalidArgument("Only with XML option is currently supported.");
        // Need to implement the rest of the methods throwing CStcRunTimeError
        // for full export to work.
    }            
}

void PythonCodeGen::SetScriptCommentProvider(const stc::framework::ScriptCommentProvider& commentProvider)
{
    _commentProvider = commentProvider;
}

std::string PythonCodeGen::IntroComment(const std::string &time, const std::string &filename)
{
    ostringstream code;
    code << _commentProvider.getLogicComment() << endl << endl;   
    return code.str();
}

std::string PythonCodeGen::Init(const std::string &logTo, const std::string &logLevel, const std::string &errorHandler)
{    
    ostringstream code;

   code
<< endl
<< endl << "# Load Spirent TestCenter"
<< endl << "from StcPython import StcPython"
<< endl << "stc = StcPython()"
<< endl << endl;

// init proc
   code << _commentProvider.getInitProcComment()
<< endl << "def " << _options.strPrefix << "init():"
<< endl << INDENT << "stc.config('automationoptions', logTo='" << logTo << "', logLevel='" << logLevel << "')"
<< endl

// config proc begin
<< endl << _commentProvider.getConfigProcComment()
<< endl << "def " << _options.strPrefix << "config(configFile, resultsDir, portLocations):"
<< endl;

   return code.str();
}

std::string PythonCodeGen::XmlLoad(const std::string &xmlFilename)
{
   ostringstream code;             
   code << INDENT << "stc.perform('loadfromxml', filename=configFile)"
<< endl << endl;   
   
   return code.str();
}

std::string PythonCodeGen::ConnectReserveApply()
{    
    ostringstream code;

// body of the config sub ...
   code << INDENT << "if len(portLocations) > 0:"
<< endl << INDENT << "    cmdResult = stc.perform('GetObjects', ClassName='Port', Condition='IsVirtual=false')"
<< endl << INDENT << "    ports = cmdResult['ObjectList'].split()"
<< endl << INDENT << "    idx = 0"
<< endl << INDENT << "    for port in ports:"
<< endl << INDENT << "        stc.config(port, location=portLocations[idx])"
<< endl << INDENT << "        idx+=1"
<< endl

<< endl << INDENT << "stc.config('project1.testResultSetting', saveResultsRelativeTo='NONE', resultsDirectory=resultsDir)"
<< endl << INDENT << "stc.config('system1.sequencer', errorHandler='STOP_ON_ERROR')"
<< endl

// connect proc
<< endl << _commentProvider.getConnectProcComment()
<< endl << "def " << _options.strPrefix << "connect():"
<< endl << INDENT  << "stc.perform('attachPorts')"
<< endl

// apply proc
<< endl << _commentProvider.getApplyProcComment()
<< endl << "def " << _options.strPrefix << "apply():"
<< endl << INDENT << "stc.apply()"
<< endl

// beginning of run proc
<< endl << _commentProvider.getRunProcComment()
<< endl << "def " << _options.strPrefix << "run():";
   
   return code.str();
}

std::string PythonCodeGen::HandleForChildObject(const std::string &codeForParent, const std::string &childType, int idx, ValueType_t type)
{    
    ostringstream code;
    string myCodeForParent = ( type == VARIABLE ) ? codeForParent : "'" + codeForParent + "'";
    code << "(stc.get( " << myCodeForParent << ", 'children-" << childType << "' ))[" << idx << "]";
    return code.str();
}

std::string PythonCodeGen::DanForObject(const std::string &code, const std::string &type, int idx)
{
    throw CStcRunTimeError("Not implemented");
    return "";
}

std::string PythonCodeGen::Subscribe(const std::string &filenamePrefix, 
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
<< endl << INDENT << "stc.subscribe(parent='project1',"
<< endl << INDENT << INDENT << "          resultParent='project1',"
<< endl << INDENT << INDENT << "          configType='" << configType << "',"
<< endl << INDENT << INDENT << "          resultType='" << resultType << "',"
<< endl << INDENT << INDENT << "          filterList=" << myFilterList << ","
<< endl << INDENT << INDENT << "          viewAttributeList='" << viewAttributeList << "',"
<< endl << INDENT << INDENT << "          interval='1', filenamePrefix='" << filenamePrefix << "')"
<< endl;
    return code.str();    
}

std::string PythonCodeGen::ChartSubscribe()
{    
    ostringstream code;
    code
<< endl << INDENT << "# Subscribe to the result query charts."
<< endl << INDENT << "for resultDataSet in stc.get('project1', 'children-resultchartqueryset').split(' '):"
<< endl << INDENT << INDENT << "stc.perform('ResultDataSetSubscribeCommand', ResultDataSet=resultDataSet)"
<< endl;

    return code.str();
}

std::string PythonCodeGen::Disconnect()
{    
    ostringstream code;
    code
<< endl << INDENT << "# Start the sequencer"
<< endl << INDENT << "stc.perform('sequencerStart')"
<< endl
<< endl << INDENT << "# Wait for sequencer to finish"
<< endl << INDENT << "testState = stc.waitUntilComplete()"
<< endl << INDENT << "return testState"
<< endl

// cleanup proc
<< endl << _commentProvider.getCleanupProcComment()
<< endl << "def " << _options.strPrefix << "cleanup():"
<< endl << INDENT  << "stc.perform('chassisDisconnectAll')"
<< endl << INDENT  << "stc.perform('resetConfig')";
    
    return code.str();
}

std::string PythonCodeGen::LauncherScript(const std::string &time, const std::string &filename, const std::string &logicFilename, const std::vector<std::string> &portLocations)
{  
    const string xmlFilename = StringUtil::Replace(filename, ".py", ".xml");
    ostringstream code;
   code << _commentProvider.getLauncherComment()   
<< endl
<< endl << "import os" 
<< endl << "import sys" 
<< endl << "import " << StringUtil::Replace(logicFilename, ".py", "") << " as " << _options.strNamespace
<< endl   
<< endl << "def runTest():"
<< endl << INDENT << _options.strNamespace << "." << _options.strPrefix << "init()"
<< endl << INDENT << "configFile = os.path.join(sys.path[0], '" << xmlFilename << "')"
<< endl << INDENT << _options.strNamespace << "." << _options.strPrefix << "config( configFile,  sys.path[0], [ ";
   
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
    
   code << " ] )"   
   
<< endl << INDENT << _options.strNamespace << "."  << _options.strPrefix << "connect()"
<< endl << INDENT << _options.strNamespace << "." << _options.strPrefix << "apply()"
<< endl << INDENT << "testState = " << _options.strNamespace << "."  << _options.strPrefix << "run()"
<< endl << INDENT << _options.strNamespace << "." << _options.strPrefix << "cleanup()"
<< endl << INDENT << "return testState"
<< endl
<< endl << "if __name__ == '__main__':"
<< endl << INDENT << "if runTest() == 'FAILED':"
<< endl << INDENT << INDENT << "sys.exit(1)"
<< endl << INDENT << "else:"
<< endl << INDENT << INDENT << "sys.exit(0)"
<< endl;
    return code.str();
}

std::string PythonCodeGen::Assignment(const std::string &varName, const std::string &value, const std::string &varType, ValueType_t type)
{
    throw CStcRunTimeError("Not implemented");
    return "";
}

std::string PythonCodeGen::CreateVarName(const std::string& baseVarName, int idx)
{
    throw CStcRunTimeError("Not implemented");
    return "";
}

std::string PythonCodeGen::BasePathMapDeclaration()
{       
    throw CStcRunTimeError("Not implemented");
    return "";
}

std::string PythonCodeGen::FilePathSubstitution(const std::string &varName, const std::string &filename)
{    
    throw CStcRunTimeError("Not implemented");
    return "";
}

std::string PythonCodeGen::CloseCreate()
{
    throw CStcRunTimeError("Not implemented");
    return "";
}

std::string PythonCodeGen::CloseConfig()
{
    throw CStcRunTimeError("Not implemented");
    return "";
}

std::string PythonCodeGen::Comment(const std::string &comment)
{
    ostringstream code;
    code << "# " << comment << endl;
    return code.str();
}

std::string PythonCodeGen::Config(const std::string &objectName, const Properties &props, ValueType_t typeOfObject, ValueType_t typeOfValues, bool closeStmt)
{
    throw CStcRunTimeError("Not implemented");
    return "";
}

std::string PythonCodeGen::ConfigRelationship(const std::string& objectName, const std::string& relName)
{
    throw CStcRunTimeError("Not implemented");
    return "";
}

std::string PythonCodeGen::ConnectRelationshipTargets(const std::string &configStmt, const std::vector<std::string> &targets, ValueType_t type)
{
    throw CStcRunTimeError("Not implemented");
    return "";
}

std::string PythonCodeGen::Create(const std::string &varName, const std::string &type, const std::string &parent, ValueType_t parentType)
{    
    throw CStcRunTimeError("Not implemented");
    return "";
}

std::string PythonCodeGen::DeleteChildren(const std::string &parent)
{    
    throw CStcRunTimeError("Not implemented");
    return "";
}

std::string PythonCodeGen::GetVariableSigil()
{
    throw CStcRunTimeError("Not implemented");
    return "";
}

std::string PythonCodeGen::Property(const std::string &propName, const std::string &propValue, ValueType_t type)
{    
    throw CStcRunTimeError("Not implemented");
    return "";
}

std::string PythonCodeGen::PropertyLocation(int portIdx)
{    
    throw CStcRunTimeError("Not implemented");
    return "";
}

