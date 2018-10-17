#include "StdAfx.h"

#include "FilePathReplacementCommand.h"
#include "FilePathReplacementCommand_AutoGen.cpp"

#include "MetaInfo.h"
#include "BLLLoggerFactory.h"
#include "StringUtil.h"

DEFINE_STATIC_LOGGER("fmwk.bll.base.pathReplacement", devLogger);

using namespace stc::framework;
using namespace std;

CFilePathReplacementCommand::CFilePathReplacementCommand(){}
CFilePathReplacementCommand::~CFilePathReplacementCommand(){}

void CFilePathReplacementCommand::DoRun()
{
    StcHandle root = GetRootObject();
    CollectionProperty< std::string >& filePathMap  = GetPathReplacements();

    if(filePathMap.Count() == 0)
        return;

    if(root == NULL_STCHANDLE)
    {        
        throw CStcInvalidArgument("no root handle.");
    }  

    if((filePathMap.Count() % 2) != 0)
    {
        throw CStcInvalidArgument("odd number of path replacements.");
    }

    ConvertToMap();
    CScriptable* pRoot = CHandleRegistry::Instance().Get(root);
    MakeReplacements(pRoot);
}

void CFilePathReplacementCommand::MakeReplacements(CScriptable* const pRoot)
{    
    string name;
    pRoot->Get(FRAMEWORK_Scriptable_szName,name);    
    
    PropertyMap props;
    pRoot->GetProperties(props);    
    for(PropertyMap::iterator it = props.begin(); it != props.end(); ++it)
    {
        Property* pProp = it->second;    
		const MetaPropertyInfo& minfo = pProp->GetMetaProperty().GetMetaInfo();
        MetaPropertyInfo::Type type = minfo.type;
        if(type == MetaPropertyInfo::INPUT_FILE_PATH || 
           type == MetaPropertyInfo::OUTPUT_FILE_PATH)
        {
			if (minfo.IsCollection())
			{
				if (type == MetaPropertyInfo::INPUT_FILE_PATH)
				{
					CollectionProperty<InputFilePath>* p = dynamic_cast<CollectionProperty<InputFilePath>*>(pProp);
					for (int i = 0; i < p->Count(); ++i)
					{
						InputFilePath path = p->GetAt(i);
						const string value = path.GetClientSideFilePath();
						const string newValue = GetNewPath(value);
						path.CopyFromString(newValue);
						p->SetAt(i, path);
						LOG_DEBUG(devLogger(), "Changed property " << minfo.name << ":" << i <<
							" from " << value << " to " << newValue);
					}
				}
				else
				{
					CollectionProperty<OutputFilePath>* p = dynamic_cast<CollectionProperty<OutputFilePath>*>(pProp);
					for (int i = 0; i < p->Count(); ++i)
					{
						OutputFilePath path = p->GetAt(i);
						const string value = path.GetClientSideFilePath();
						const string newValue = GetNewPath(value);
						path.CopyFromString(newValue);
						p->SetAt(i, path);
						LOG_DEBUG(devLogger(), "Changed property " << minfo.name << ":" << i <<
							" from " << value << " to " << newValue);
					}
				}
			}
			else
			{
				string value;
				pProp->Get(pRoot,value);

				const string newPath = GetNewPath(value);
				pProp->Set(pRoot, newPath);
				LOG_DEBUG(devLogger(), "Changed property " << minfo.name <<
					" from " << value << " to " << newPath);
			}
        }        
    }

    ScriptableVec scriptables;
    pRoot->GetObjects(scriptables);
    for(ScriptableVec::iterator it = scriptables.begin(); it != scriptables.end(); ++it)
    {
        MakeReplacements(*it);
    }
}

string CFilePathReplacementCommand::GetNewPath(const string& path)
{         
    string newPath = path;    
    CollectionProperty< std::string >& filePathMap = GetPathReplacements();
    bool forwardPathSep = false;
    for(int i = 0; i < filePathMap.Count(); i+=2)
    {
        string replace = FixUpFileSep(filePathMap.GetAt(i));
        string with = m_pathMap[replace];
        if(replace == "\\" && with == "/")
        {
            forwardPathSep = true;
        }
        else
        {        
            newPath = StringUtil::Replace(newPath,replace,with);
        }
    }    

    // This must always happen at the end.
    if(forwardPathSep)
    {
        newPath = StringUtil::Replace(newPath,"\\","/");
    }

    return newPath;    
}

void CFilePathReplacementCommand::ConvertToMap()
{
    // The map is simply to prevent duplicates so that the behavior is like Tcl string map.
    m_pathMap.clear();
    CollectionProperty< std::string >& filePathMap = GetPathReplacements();
    for(int i = 0; i < filePathMap.Count(); i+=2)
    {
        string replace = FixUpFileSep(filePathMap.GetAt(i));
        string with = filePathMap.GetAt(i+1);
        m_pathMap.insert(make_pair(replace,with));
    }
}

std::string CFilePathReplacementCommand::FixUpFileSep(const std::string& srcPath)
{
    bool unc = ( srcPath.find("\\\\") == 0 && srcPath.length() > 2);
    string fixedPath = StringUtil::Replace(srcPath,"\\\\","\\");
    if(unc)
    {
        fixedPath.insert(0,"\\");
    }

    return fixedPath;
}

