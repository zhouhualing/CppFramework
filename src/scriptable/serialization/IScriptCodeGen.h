#ifndef __ISCRIPT_CODEGEN_H__
#define __ISCRIPT_CODEGEN_H__

#include <string>
#include <vector>
#include <map>

#include "ScriptExporterOptions.h"
#include "ScriptCommentProvider.h"

namespace stc {
namespace framework {

// Property name to value(s)
typedef std::map<std::string, std::vector<std::string> > Properties;

enum ValueType_t { STRING_LITERAL, LITERAL, VARIABLE };

/**
 * Templating interface for script code generation.
 * Implementations will provide the template code and fill in
 * the template variables with the given parameters for each
 * method. Instances of IScriptCodeGen will be plugged into
 * ScriptExporter.
 */
class IScriptCodeGen {

public:
	virtual ~IScriptCodeGen() {}// gets rid of Linux Compiler Warning. 
    /// \brief
    /// @param
    virtual void SetOptions(const stc::framework::ExporterOptions& options) = 0;    

    /// \brief
    /// @param 
    virtual void SetScriptCommentProvider(const stc::framework::ScriptCommentProvider& commentProvider) = 0;

    /// \brief Creates code for the logic script comment section.
    /// @param time
    /// @param filename
    virtual std::string IntroComment(const std::string& time, const std::string& filename) = 0;

    /// \brief Creates code for loading a config from an XML file.
    /// @param xmlFilename
    virtual std::string XmlLoad(const std::string& xmlFilename) = 0;

    /// \brief Creates the init function.    
    virtual std::string Init(const std::string& logTo, 
                             const std::string& logLevel, 
                             const std::string& errorHandler) = 0;

    /// \brief Creates the connect, reserve, and apply functions.    
    virtual std::string ConnectReserveApply() = 0;

    /// \brief Creates the subscribe code.    
    virtual std::string Subscribe(const std::string& filenamePrefix,
                                  const std::string& parent,
                                  const std::string& resultParent,
                                  const std::string& configType,
                                  const std::string& resultType,
                                  const std::string& filterList,
                                  const std::string& viewAttributeList) = 0;

    /// \brief Creates the chart subscription code.   
    virtual std::string ChartSubscribe() = 0;

    /// \brief Creates the disconnect function
    virtual std::string Disconnect() = 0;

    /// \brief Creates code to delete children from the given parent.
    /// @param parent
    virtual std::string DeleteChildren(const std::string& parent) = 0;    

    /// \brief Creates the property name/value code for an object.
    /// @param propName
    /// @param propValue
    /// @param type
    virtual std::string Property(const std::string& propName, const std::string& propValue, ValueType_t type) = 0;

    /// \brief Creates the property name/value code for a Port Location property.
    /// @param portIdx
    virtual std::string PropertyLocation(int portIdx) = 0;

    /// \brief Finishes the relationship connection statement ( config ) given
    ///        the first part of the config statement and a list of targets.
    /// @param configStmt - First part of the config statement.
    /// @param targets - a list of targets
    /// @param type - The type of the targets.
    virtual std::string ConnectRelationshipTargets(const std::string& configStmt, 
                                                   const std::vector<std::string>& targets,
                                                   ValueType_t type) = 0;

    /// \brief Creates code for the launcher script.
    /// @param time
    /// @param filename
    /// @param logicFilename
    /// @param portLocations
    virtual std::string LauncherScript(const std::string& time, 
                                       const std::string& filename, 
                                       const std::string& logicFilename,
                                       const std::vector<std::string>& portLocations) = 0;

    /// \brief Creates the basePathMap code for the launcher script.    
    virtual std::string BasePathMapDeclaration() = 0;

    /// \brief Creates the file path substitution code.
    /// @param varName
    /// @param filename
    virtual std::string FilePathSubstitution(const std::string& varName, const std::string& filename) = 0;

    /// \brief Creates code to get a handle for a child object at a certain index.
    /// @param codeForParent - The code to get the parent. Typically a DAN string, but could be anything.
    /// @param childType - The type of children
    /// @param idx - The index of the child to retrieve.
    /// @param type - The ValueType_t of codeForParent.
    virtual std::string HandleForChildObject(const std::string& codeForParent, 
                                             const std::string& childType, int idx, ValueType_t type) = 0;

    /// \brief Creates DAN code.
    /// @param code
    /// @param type
    /// @param idx
    virtual std::string DanForObject(const std::string& code, 
                                     const std::string& type, int idx) = 0;
    
    /// \brief Creates a config statement.
    /// @param objectName - The object name.
    /// @param props - property names/values to configure.
    /// @param typeOfObject - The value type of the object.
    /// @param typeOfValues - The value types of the values in props.
    /// @param closeStmt - Closes the config statement if true. Just call CloseConfig method if true.
    virtual std::string Config(const std::string& objectName, const Properties& props, 
                               ValueType_t typeOfObject, ValueType_t typeOfValues, bool closeStmt = true) = 0;

    /// \brief Creates the beginning part of a config statement for connecting relationships.
    /// @param objectName - The object name.
    /// @param relName - The relationship name, i.e. -targets/-sources    
    virtual std::string ConfigRelationship(const std::string& objectName, const std::string& relName) = 0;

    /// \brief Creates a create statement and assigns to a variable. 
    ///        ( Does NOT close the statement. Close it with CloseCreate method. )
    /// @param varName - The variable name.
    /// @param type - The object type to create.
    /// @param parent - The parent object. May be empty string.
    /// @param parentType - The value type of the parent.
    virtual std::string Create(const std::string& varName, const std::string& type, 
                               const std::string& parent, ValueType_t parentType) = 0;

    /// \brief Closes the create statement.    
    virtual std::string CloseCreate() = 0;

    /// \brief Closes the config statement.
    virtual std::string CloseConfig() = 0;

//////////////////////////////////////////////////////////////////////////////
// Generic language constructs and helpers
//////////////////////////////////////////////////////////////////////////////

    /// \brief Creates a comment statement.
    /// @param comment
    virtual std::string Comment(const std::string& comment) = 0;

    /// \brief Creates an assignment statement
    /// @param varName - The variable name.
    /// @param value - The variable's value.
    /// @param varType - The variable's type. May not be needed in some languages.
    /// @param type - The type of the value being assigned.
    virtual std::string Assignment(const std::string& varName, 
                                   const std::string& value,
                                   const std::string& varType,                                   
                                   ValueType_t type) = 0;

    /// \brief Get the sigil that can be attached to a variable name to denote a scalar value.
    ///        May not be applicable in some languages, in which case implementations can return an empty string.
    virtual std::string GetVariableSigil() = 0;
        
    /// \brief Creates a valid variable name for the language from a base name.
    /// @param baseVarName
    /// @param idx - The index of the object type.
    virtual std::string CreateVarName(const std::string& baseVarName, int idx) = 0;
};
}
}

#endif /* __ISCRIPT_CODEGEN_H__ */

