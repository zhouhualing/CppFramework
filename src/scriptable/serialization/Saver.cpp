#include "StdAfx.h"

#include "Saver.h"

#include "ImexSaver.h"

#include "ScriptableImexAdapter.h"
#include "NoDefaultScriptableImexAdapter.h"

void Saver::setupStandardExclusions(vector<string>& vecClassesToExclude) {
	// TODO: Drop vecClassesToExclude from the interface since the meta class info already has these info
	//       and we are checking for them in ScriptableImexAdapter::isSuitableForExport
    //vecClassesToExclude.push_back("CommandStatusUpdate");
    //vecClassesToExclude.push_back("PhysicalChassis");
}

void Saver::saveAsXml(CScriptable& exp, string strFilename, bool isSaveDefault, bool skipNoSaveAttribute) {
    vector<string> vecClassesToExclude;
	if(isSaveDefault)
	{
		ScriptableImexAdapter adapter(&exp);
		adapter.SetSkipNoSaveAttributeFlag(skipNoSaveAttribute);
		setupStandardExclusions(vecClassesToExclude);
		ImexSaver::saveAsXml(adapter, vecClassesToExclude, strFilename);
	}
	else
	{
		NoDefaultScriptableImexAdapter adapter(&exp);
		adapter.SetSkipNoSaveAttributeFlag(skipNoSaveAttribute);
		setupStandardExclusions(vecClassesToExclude);
		ImexSaver::saveAsXml(adapter, vecClassesToExclude, strFilename);
	}
}

void Saver::saveAsXml(CScriptable& exp, vector<string> vecClassesToExclude, 
                      string strFilename, bool isSaveDefault, bool skipNoSaveAttribute) 
{
	if(isSaveDefault)
	{
		ScriptableImexAdapter adapter(&exp);
		adapter.SetSkipNoSaveAttributeFlag(skipNoSaveAttribute);
		ImexSaver::saveAsXml(adapter, vecClassesToExclude, strFilename);
	}
	else
	{
		NoDefaultScriptableImexAdapter adapter(&exp);
		adapter.SetSkipNoSaveAttributeFlag(skipNoSaveAttribute);
		ImexSaver::saveAsXml(adapter, vecClassesToExclude, strFilename);
	}
}


void Saver::saveAsXml(CScriptable& root, 
					const ScriptableVec& toSaveVec,
					string strFilename, bool isSaveDefault, bool skipNoSaveAttribute) 
{
    vector<string> vecClassesToExclude;
	std::set<Exportable*> toSaveSet;
	if(isSaveDefault)
	{
		ScriptableImexAdapter adapter(&root);
		setupStandardExclusions(vecClassesToExclude);
		for (ScriptableVec::const_iterator it = toSaveVec.begin();
			it != toSaveVec.end();
			++it)
		{
			toSaveSet.insert( new ScriptableImexAdapter(*it) );
		}
		adapter.SetSkipNoSaveAttributeFlag(skipNoSaveAttribute);
		ImexSaver::saveAsXml(adapter, vecClassesToExclude, toSaveSet, strFilename);

	}
	else
	{
		NoDefaultScriptableImexAdapter adapter(&root);
		setupStandardExclusions(vecClassesToExclude);
		for (ScriptableVec::const_iterator it = toSaveVec.begin();
			it != toSaveVec.end();
			++it)
		{
			toSaveSet.insert( new NoDefaultScriptableImexAdapter(*it) );
		}
		adapter.SetSkipNoSaveAttributeFlag(skipNoSaveAttribute);
		ImexSaver::saveAsXml(adapter, vecClassesToExclude, toSaveSet, strFilename);
	}
	for (std::set<Exportable*>::const_iterator sit = toSaveSet.begin();
		sit != toSaveSet.end();
		++sit)
	{
		delete *sit;
	}
}

void Saver::saveAsXml(CScriptable& root, 
					const ScriptableVec& toSaveVec,
					std::ostream& os, bool isSaveDefault, bool skipNoSaveAttribute) 
{
	vector<string> vecClassesToExclude;
	std::set<Exportable*> toSaveSet;
	if(isSaveDefault)
	{
		ScriptableImexAdapter adapter(&root);
		setupStandardExclusions(vecClassesToExclude);

		for (ScriptableVec::const_iterator it = toSaveVec.begin();
			it != toSaveVec.end();
			++it)
		{
			toSaveSet.insert( new ScriptableImexAdapter(*it) );
		}
		adapter.SetSkipNoSaveAttributeFlag(skipNoSaveAttribute);
		ImexSaver::saveAsXml(adapter, vecClassesToExclude, toSaveSet, os);
	}
	else
	{
		NoDefaultScriptableImexAdapter adapter(&root);
		setupStandardExclusions(vecClassesToExclude);

		for (ScriptableVec::const_iterator it = toSaveVec.begin();
			it != toSaveVec.end();
			++it)
		{
			toSaveSet.insert( new NoDefaultScriptableImexAdapter(*it) );
		}
		adapter.SetSkipNoSaveAttributeFlag(skipNoSaveAttribute);
		ImexSaver::saveAsXml(adapter, vecClassesToExclude, toSaveSet, os);
	}

	for (std::set<Exportable*>::const_iterator sit = toSaveSet.begin();
		sit != toSaveSet.end();
		++sit)
	{
		delete *sit;
	}
}


void Saver::saveAsScript(CScriptable& exp, string strFilename, bool isSaveDefault, string strPrefix, 
                         string strBeforeDisconnectScript, string strNamespace,
                         PathReplacements_t pathReplacements,
                         Language_t language, bool compact) 
{
    ExporterOptions options;
    options.strBeforeDisconnectScript = strBeforeDisconnectScript;    
    options.strPrefix = strPrefix;
    options.strNamespace = strNamespace;
    options.pathReplacements = pathReplacements;
    options.useDefaultsForRarelyUsedSettings = compact;
    options.language = language;
	vector<string> vecClassesToExclude;
	if(isSaveDefault)
	{
		ScriptableImexAdapter adapter(&exp);
		setupStandardExclusions(vecClassesToExclude);
		ImexSaver::saveAsScript(adapter, vecClassesToExclude, strFilename, options);
	}
	else
	{
		NoDefaultScriptableImexAdapter adapter(&exp);
		setupStandardExclusions(vecClassesToExclude);
		ImexSaver::saveAsScript(adapter, vecClassesToExclude, strFilename, options);
	}
}

void Saver::saveAsScript(CScriptable& exp, vector<string> vecClassesToExclude, 
                         string strFilename, bool isSaveDefault, string strPrefix, 
                         string strBeforeDisconnectScript,
                         string strNamespace, PathReplacements_t pathReplacements,
                         Language_t language, bool compact) 
{
    ExporterOptions options;
    options.strBeforeDisconnectScript = strBeforeDisconnectScript;    
    options.strPrefix = strPrefix;
    options.strNamespace = strNamespace;
    options.pathReplacements = pathReplacements;
    options.useDefaultsForRarelyUsedSettings = compact;
    options.language = language;
	if(isSaveDefault)
	{
		ScriptableImexAdapter adapter(&exp);
		ImexSaver::saveAsScript(adapter, vecClassesToExclude, strFilename, options);
	}
	else
	{
		NoDefaultScriptableImexAdapter adapter(&exp);
		ImexSaver::saveAsScript(adapter, vecClassesToExclude, strFilename, options);
	}
}

void Saver::saveAsScriptWithXml(CScriptable& exp, string strFilename, string strPrefix, 
                             string strBeforeDisconnectScript, bool isSaveDefault, 
                             string strNamespace,
                             PathReplacements_t pathReplacements,
                             Language_t language)
{
    ExporterOptions options;
    options.strBeforeDisconnectScript = strBeforeDisconnectScript;    
    options.strPrefix = strPrefix;
    options.strNamespace = strNamespace;
    options.pathReplacements = pathReplacements;
    options.language = language;    
    vector<string> vecClassesToExclude;
	if(isSaveDefault)
	{
		ScriptableImexAdapter adapter(&exp);
		setupStandardExclusions(vecClassesToExclude);
		ImexSaver::saveAsScriptWithXml(adapter, vecClassesToExclude, strFilename, options);
	}
	else
	{
		NoDefaultScriptableImexAdapter adapter(&exp);
		setupStandardExclusions(vecClassesToExclude);
		ImexSaver::saveAsScriptWithXml(adapter, vecClassesToExclude, strFilename, options);
	}
}

void Saver::saveAsScriptWithXml(CScriptable& exp, vector<string> vecClassesToExclude, 
                             string strFilename, string strPrefix, string strBeforeDisconnectScript, 
                             bool isSaveDefault,
                             string strNamespace, 
                             PathReplacements_t pathReplacements,
                             Language_t language) 
{
    ExporterOptions options;
    options.strBeforeDisconnectScript = strBeforeDisconnectScript;    
    options.strPrefix = strPrefix;
    options.strNamespace = strNamespace;
    options.pathReplacements = pathReplacements;
    options.language = language;    
	if(isSaveDefault)
	{
		ScriptableImexAdapter adapter(&exp);
		ImexSaver::saveAsScriptWithXml(adapter, vecClassesToExclude, strFilename, options);
	}
	else
	{
		NoDefaultScriptableImexAdapter adapter(&exp);
		ImexSaver::saveAsScriptWithXml(adapter, vecClassesToExclude, strFilename, options);
	}
}

static vector<string> s_savedFiles;

void Saver::setSavedFiles(const vector<string>& files)
{
	s_savedFiles = files;
}

vector<string> Saver::getSavedFiles()
{
	return s_savedFiles;
}


