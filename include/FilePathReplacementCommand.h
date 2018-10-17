#ifndef INCLUDED_FRAMEWORK_FILEPATHREPLACEMENTCOMMAND_H
#define INCLUDED_FRAMEWORK_FILEPATHREPLACEMENTCOMMAND_H
#pragma once

#include "Scriptable.h"
#include "Command.h"

namespace stc {
namespace framework {

class STC_PUBLIC CFilePathReplacementCommand : public CCommand {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//

protected:
	// Construct through ScriptableCreator instead.
	CFilePathReplacementCommand();
	CFilePathReplacementCommand(const CFilePathReplacementCommand&);
	CFilePathReplacementCommand& operator = (const CFilePathReplacementCommand&);
	virtual ~CFilePathReplacementCommand();

    virtual void DoRun();

    virtual void MakeReplacements(CScriptable* const pRoot);

private:
	friend class CFilePathReplacementCommandCtorDtorHelper;    

    std::map<std::string,std::string> m_pathMap;

    std::string GetNewPath(const std::string& path);
    void ConvertToMap();
    
    std::string FixUpFileSep(const std::string& srcPath);
    
// Keep this include as the last line of the class!!!
#include "FilePathReplacementCommand_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_FILEPATHREPLACEMENTCOMMAND_H

