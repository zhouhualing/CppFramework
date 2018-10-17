#ifndef __PYTHON_CODEGEN_H__
#define __PYTHON_CODEGEN_H__

#include "IScriptCodeGen.h"
#include "StcExport.h"

namespace stc {
namespace framework {

class STC_PUBLIC PythonCodeGen : public IScriptCodeGen
{
public:
    PythonCodeGen();
    virtual ~PythonCodeGen();

    virtual std::string ChartSubscribe();
    virtual std::string ConnectReserveApply();
    virtual std::string Disconnect();
    virtual std::string HandleForChildObject(const std::string &codeForParent, const std::string &childType, int idx, ValueType_t type);
    virtual std::string DanForObject(const std::string &code, const std::string &type, int idx);
    virtual std::string IntroComment(const std::string &time, const std::string &filename);              
    virtual std::string Init(const std::string &logTo, const std::string &logLevel, const std::string &errorHandler);
    virtual std::string LauncherScript(const std::string &time, const std::string &filename, const std::string &logicFilename, const std::vector<std::string> &portLocations);
    virtual void SetOptions(const stc::framework::ExporterOptions &options);
    virtual void SetScriptCommentProvider(const stc::framework::ScriptCommentProvider& commentProvider);
    virtual std::string Subscribe(const std::string &filenamePrefix, const std::string &parent, const std::string &resultParent, const std::string &configType, const std::string &resultType, const std::string &filterList, const std::string& viewAttributeList);    
    virtual std::string XmlLoad(const std::string &xmlFilename);            
    virtual std::string Assignment(const std::string &varName, const std::string &value, const std::string &varType, ValueType_t type);
    virtual std::string CreateVarName(const std::string& baseVarName, int idx);
    virtual std::string BasePathMapDeclaration();
    virtual std::string CloseCreate();            
    virtual std::string CloseConfig();
    virtual std::string Comment(const std::string &comment);           
    virtual std::string Config(const std::string &objectName, const Properties &props, ValueType_t typeOfObject, ValueType_t typeOfValues, bool closeStmt);
    virtual std::string ConfigRelationship(const std::string& objectName, const std::string& relName);
    virtual std::string ConnectRelationshipTargets(const std::string &configStmt, const std::vector<std::string> &targets, ValueType_t type);                
    virtual std::string Create(const std::string &varName, const std::string &type, const std::string &parent, ValueType_t parentType);                
    virtual std::string DeleteChildren(const std::string &parent);                
    virtual std::string FilePathSubstitution(const std::string &varName, const std::string &filename);    
    virtual std::string GetVariableSigil();                                        
    virtual std::string Property(const std::string &propName, const std::string &propValue, ValueType_t type);        
    virtual std::string PropertyLocation(int portIdx);                   

private:

    stc::framework::ExporterOptions _options;
    stc::framework::ScriptCommentProvider _commentProvider;
};

}
}

#endif /* __PYTHON_CODEGEN_H__ */

