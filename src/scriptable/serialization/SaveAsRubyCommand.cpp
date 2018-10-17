#include "StdAfx.h"

#include "SaveAsRubyCommand.h"
#include "SaveAsRubyCommand_AutoGen.cpp"
#include "StcSystem.h"
#include "Project.h"
#include "StcFileManager.h"
#include "HandleRegistry.h"
#include "Path.h"
#include "StringUtil.h"
#include "Saver.h"
#include "ScriptCommentProvider.h"
#include "ScriptExporter.h"

#include "ace/ACE.h"

#include <iomanip>
#include <fstream>
#include <sstream>

using namespace stc::framework;
using std::endl;

/*
TODO: refactor in 3.30
class ScriptExporter {
public:
	class ExportOption
	{
	};

	ScriptExporter(const ExportOption& option):m_option(option)	{}

	virtual ~ScriptExporter() {}

protected:
	
private:

	ExportOption m_option;
};
*/


static void WriteLauncherScript(const std::string& filePath, const std::string& logicPath, const std::string& timestamp)
{
    ExporterOptions options;
    options.language = RUBY;
    options.includeConfigCustom |= ScriptExporter::ConfigHasExposedProperties();

    ScriptCommentProvider commentProvider(options, filePath, logicPath, timestamp);
	std::ofstream out(filePath.c_str());
   out << commentProvider.getLauncherComment()   
<< endl << ""
<< endl << "require File.expand_path( File.join( File.dirname(__FILE__), File.basename(__FILE__, '.rb') + '_logic.rb') )"
<< endl << ""
<< endl << "module StcTest";
    if (options.includeConfigCustom)
    {
        out
        << endl << commentProvider.getConfigCustomComment()
        << endl << "    def self.configCustom()"
        << endl << "        scriptFile = File.expand_path(File.join(File.dirname(__FILE__), File.basename(__FILE__, '.rb') + '_custom.rb'))"
        << endl << "        if (File.exists?(scriptFile))"
        << endl << "            load scriptFile"
        << endl << "        end"
        << endl << "    end"
        << endl << "";
    }
    out
<< endl << "    def self.runTest()"
<< endl << "        StcTest.init()"
<< endl << "        StcTest.config("
<< endl << "            'CONFIG_FILE'=>File.expand_path(File.join(File.dirname(__FILE__), File.basename(__FILE__, '.rb') + '.xml')), "
<< endl << "            'PORT_LOCATIONS'=>[], "
<< endl << "            'RESULTS_DIRECTORY'=>File.dirname(__FILE__))";
    if (options.includeConfigCustom)
    {
        out
        << endl << "        configCustom()";
    }
    out
<< endl << "        StcTest.connect()"
<< endl << "        testState = StcTest.run()"
<< endl << "        StcTest.cleanup()"
<< endl << "        return testState"
<< endl << "    end"
<< endl << "end"
<< endl << ""
<< endl << ""
<< endl << "if __FILE__ == $0"
<< endl << "    if StcTest.runTest() == 'FAILED'"
<< endl << "        exit(1)"
<< endl << "    else"
<< endl << "        exit(0)"
<< endl << "    end"
<< endl << "end"
<< endl << ""
<< endl;
}

static std::string GetCsvFileName(const std::string& baseName, const std::string& resultTypeStr, int iterCount)
{
	std::ostringstream os;
	os << baseName << "-" << std::setw(4) << std::setfill('0') << iterCount << "-" << resultTypeStr;
	return os.str();
}

static std::string GetDanString(const CScriptable& obj)
{
	std::string str;
	const CScriptable* s = &obj;
	while (s != NULL)
	{
		if (s == &CStcSystem::Instance())
		{
			str = "system1" + str;
			break;
		}

		if (s == &CProject::Instance())
		{
			str = "project1" + str;
			break;
		}

		const CScriptable* parent = s->GetParent();
		if (parent == NULL)
			throw CStcLogicError("GetDanString object has invalid parent");

		ScriptableVec svec;
		parent->GetObjects(svec, s->GetClassId());
		const int idx = std::distance(svec.begin(),
			std::find(svec.begin(), svec.end(), s)) + 1;

		std::ostringstream os;
		os << "." << s->GetClassId().ToString() << "." << idx;
		str = os.str() + str;
		s = parent;
	}

	return str;
}

static std::string GetCollectionHandlePropertyInDan(const CScriptable& obj, const std::string& collectionPropName)
{
	PropId propId;
	propId.CopyFromString(collectionPropName);
	HandleVec hnds;
	obj.GetCollection<StcHandle>(propId).Get(hnds);

	const CHandleRegistry& reg = CHandleRegistry::Instance();
	std::vector<std::string> strVec;
	for (HandleVec::const_iterator it = hnds.begin(); it != hnds.end(); ++it)
	{
		const CScriptable* s = reg.Get(*it);
		const std::string str = GetDanString(*s);

		// SAL should just handle dan handle param...
		if (str == "project1")
		{
			strVec.push_back("'project1'");
			continue;
		}

		if (!str.empty())
		{
			strVec.push_back("Stc.get('" + str + "', 'handle')");
		}
	}

	return "[" + StringUtil::Join(strVec, ", ") + "]";
}

static std::string WriteSubscribeLogic(const std::string& indent, const std::string& baseName)
{
	std::ostringstream os;
	ScriptableVec svec;
	CProject::Instance().GetObjects(svec, "ResultDataSet");
	bool hasChart = false;
	int idx = 0;
	os << endl << indent << "subscriptions = []";
	for (ScriptableVec::const_iterator it = svec.begin(); it != svec.end(); ++it)
	{
		const CScriptable& rds = (**it);
		std::string val;
		rds.Get("ResultState", val);
		if (val == "SUBSCRIBED")
		{
			if (!hasChart && rds.IsTypeOf("ResultChartQuerySet"))
			{
				hasChart = true;
				continue;
			}

			ScriptableVec qvec;
			rds.GetObjects(qvec, "ResultQuery");
			for (ScriptableVec::const_iterator qit = qvec.begin(); qit != qvec.end(); ++qit)
			{
				const CScriptable& rq = (**qit);
				std::string configType;
				std::string resultType;
                std::string propertyIdArray;
				rq.Get("ConfigClassId", configType);
				rq.Get("ResultClassId", resultType);
                rq.Get("PropertyIdArray", propertyIdArray);

                vector<string> ids;
                StringUtil::Tokenize(propertyIdArray, " ", ids);
                ostringstream viewAttributeList;
                vector<string>::iterator it = ids.begin();
                for(; it != ids.end(); ++it)
                {
                    vector<string> tokens;
                    StringUtil::Tokenize((*it), ".", tokens);
                    viewAttributeList << tokens[1] << " ";
                }

				const std::string filenamePrefix = GetCsvFileName(baseName, resultType, ++idx);
				const std::string resultParents = GetCollectionHandlePropertyInDan(rq, "ResultQuery.ResultRootList");
				const std::string filters = "Stc.get('" + GetDanString(rq) + "', 'resultFilters-targets')";

				os << endl << indent << "# Subscribe to results for result query " << filenamePrefix;
				os << endl << indent << "s = Stc.subscribe('parent'=>'project1', ";
				os << endl << indent << "    'resultParent'=>" << resultParents << ",";
				os << endl << indent << "    'configType'=>'" << configType << "',";
				os << endl << indent << "    'resultType'=>'" << resultType << "',";
				os << endl << indent << "    'filterList'=>" << filters << ",";
                os << endl << indent << "    'viewAttributeList'=>'" << viewAttributeList.str() << "',";
				os << endl << indent << "    'interval'=>1,"; 
				os << endl << indent << "    'filenamePrefix'=>'" << filenamePrefix << "')";
				os << endl << indent << "subscriptions.push(s)"; 
				os << endl;
			}
		}
	}

	if (hasChart)
	{    
		os << endl << indent << "# Subscribe to charts.";
		os << endl << indent << "for rds in Stc.get('project1', 'children-resultChartQuerySet').split() ";
		os << endl << indent << "    Stc.perform('resultDataSetSubscribeCommand', 'resultDataSet'=>rds)";
		os << endl << indent << "end";
		os << endl;
	}

	return os.str();
}

static std::string WriteUnsubscribeLogic(const std::string& indent)
{
	std::ostringstream os;
	os << endl << indent << "subscriptions.each { |s| Stc.unsubscribe(s) }";
	return os.str();
}

static void WriteLogicScript(const std::string& filePath, const std::string& launcherPath, const std::string& timestamp, const std::string& baseName)
{
    ExporterOptions options;
    options.language = RUBY;
    ScriptCommentProvider commentProvider(options, launcherPath, filePath, timestamp);

	std::vector<std::string> vers;
	StringUtil::Tokenize(CStcSystem::Instance().GetVersion(), ".", vers);
	assert(vers.size() == 4);
	// only check major.minor.0
	const std::string minVer = vers[0] + "." + vers[1] + ".0";
	
	std::ofstream out(filePath.c_str());
   out << commentProvider.getLogicComment()   
<< endl << ""
<< endl << "require 'rubygems'"
<< endl << "gem 'spirenttestcenter', '>= " << minVer << "'"
<< endl << "require 'spirenttestcenter'"
<< endl << ""
<< endl << "module StcTest"
<< endl << ""
<< endl << "    class << self"
<< endl << ""
<< endl << commentProvider.getInitProcComment()
<< endl << "    def init()"
<< endl << "        Stc.config('automationoptions', 'logTo'=>'stdout', 'logLevel'=>'WARN')"
<< endl << "    end"
<< endl << ""
<< endl << commentProvider.getConfigProcComment()
<< endl << "    def config(args={})"
<< endl << "        config = args.fetch('CONFIG_FILE')"
<< endl << "        Stc.perform('loadFromXml', 'fileName'=>config)"
<< endl << ""
<< endl << "        locs = args.fetch('PORT_LOCATIONS', [])"
<< endl << "        cmdResult = Stc.perform('GetObjects', 'ClassName'=>'Port', 'Condition'=>'IsVirtual=false')"
<< endl << "        ports = cmdResult['ObjectList'].split()"
<< endl << "        (0..([locs.size(), ports.size()].min() - 1)).each { |i| Stc.config(ports[i], 'location'=>locs[i]) }"
<< endl << ""
<< endl << "        resultDir = args.fetch('RESULTS_DIRECTORY')"
<< endl << "        Stc.config('project1.testResultSetting', 'saveResultsRelativeTo'=>'NONE', 'resultsDirectory'=>resultDir)"
<< endl << "        Stc.config('system1.sequencer', 'errorHandler'=>'STOP_ON_ERROR')"
<< endl << "    end"
<< endl << ""
<< endl << commentProvider.getConnectProcComment()
<< endl << "    def connect()"
<< endl << "        Stc.perform('attachPorts', "
<< endl << "                'autoConnect'=>true, "
<< endl << "                'portList'=> Stc.get('project1', 'children-Port').split())"
<< endl << "    end"
<< endl << ""
<< endl << commentProvider.getRunProcComment()
<< endl << "    def run()"
        <<          WriteSubscribeLogic("        ", baseName)
<< endl << "        Stc.perform('sequencerStart')"
<< endl << "        testState = Stc.waitUntilComplete()"
        <<          WriteUnsubscribeLogic("        ")
<< endl << "        return testState"
<< endl << "    end"
<< endl << ""
<< endl << commentProvider.getCleanupProcComment()
<< endl << "    def cleanup()"
<< endl << "        Stc.perform('chassisDisconnectAll')"
<< endl << "        Stc.perform('resetConfig')"
<< endl << "    end"
<< endl << ""
<< endl << "    end"
<< endl << "end"
<< endl << ""
<< endl;
}

static void WriteXmlConfig(const std::string& filePath)
{
	Saver::saveAsXml(CStcSystem::Instance(), filePath, false);
}

CSaveAsRubyCommand::CSaveAsRubyCommand() 
{
	SetUserCanInterrupt(false);
}

CSaveAsRubyCommand::~CSaveAsRubyCommand() {}

void CSaveAsRubyCommand::DoRun() 
{
	const std::string timestamp = CTimer::GetCurrentSystemDateTime();
	const std::string fileName = Path::GetFullOutputFilePath(GetFileName());
	const std::string basePathWithoutExt = 
		Path::JoinPath(Path::GetDirectoryName(fileName), Path::GetFileNameWithoutExtension(fileName));

	typedef std::vector<std::string> StrVec;
	StrVec files;

	const std::string launcherFilePath = fileName;
    const std::string logicFilePath = basePathWithoutExt + "_logic" + Path::GetExtension(fileName);
	WriteLauncherScript(launcherFilePath, logicFilePath, timestamp);
	files.push_back(launcherFilePath);
	
	WriteLogicScript(logicFilePath, launcherFilePath, timestamp, Path::GetFileNameWithoutExtension(fileName));
	files.push_back(logicFilePath);

	const std::string xmlFilePath = basePathWithoutExt + ".xml";
	WriteXmlConfig(xmlFilePath);
	files.push_back(xmlFilePath);


	FileManager& fm = FileManager::Instance();
	const std::string clientDir = 
		Path::GetDirectoryName( GetFileName().GetClientSideFilePath() );
	for (StrVec::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		FileManager::FileEntry entry;
		entry.category = CRemoteFileMappingEntry::EnumFileCategory_DOCUMENT;
		entry.clientUriBase = CRemoteFileMappingEntry::EnumUriBase_NONE;
		entry.clientPath = Path::JoinPath( clientDir, Path::GetFileName(*it) );
		entry.serverPath = *it;	

		fm.AddFile(entry);
	}
}

