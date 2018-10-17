#include "StdAfx.h"

#include "MetaClassManager.h"
#include "MetaClass.h"
#include "MetaClassUtil.h"
#include "MetaClassLoadPostProcessor.h"
#include "RelationManager.h"
#include "PluginManager.h"
#include "StcSystemConfiguration.h"
#include "CommandStatusManager.h"
#include "StcSystem.h"
#include "Path.h"
#include "ResourcePack.h"
#include "StringUtil.h"
#include "Command.h"
#include "SystemUtils.h"
#include "StakCommandMetaClass.h"
#include "BLLLoggerFactory.h"
#include "Delegate.h"
#include "PropValidatorImpl.h"
#include "Directory.h"

#include <cassert>
#include <algorithm>
#include <fstream>
#include <stack>
#include <iostream>

#include <boost/ref.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>

#define BOOST_FILESYSTEM_NO_LIB
#define BOOST_SYSTEM_NO_LIB
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/util/BinInputStream.hpp>

using namespace stc::framework;
XERCES_CPP_NAMESPACE_USE

// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.base.dm", devLogger);

DEFINE_STATIC_LOGGER("user.fmwk.bll.base.dm", userLogger);


///////////////////////////////////////////////////////////////////////////////

class StdBinInputStream : public xercesc::BinInputStream
{
public:
    StdBinInputStream(std::istream& stream) : BinInputStream(), m_stream(stream) {}

    virtual ~StdBinInputStream() {}
#if XERCES_VERSION_MAJOR == 2
    virtual unsigned int curPos() const { return ( unsigned int ) m_stream.tellg(); }

    virtual unsigned int readBytes(XMLByte* const buf, const unsigned int maxToRead)
#else
    virtual XMLSize_t curPos() const { return m_stream.tellg(); }

    virtual XMLSize_t readBytes(XMLByte* const buf, XMLSize_t maxToRead)
#endif
    {
        assert(sizeof(XMLByte) == sizeof(char));
        if (!m_stream) { return 0; }
        m_stream.read(reinterpret_cast<char*>(buf), maxToRead);
        return ( unsigned int ) m_stream.gcount();
    }

    virtual const XMLCh* getContentType() const { return NULL; }

private:
    std::istream& m_stream;
};

class StdBinInputSource : public xercesc::InputSource
{
public:
    StdBinInputSource(std::istream& stream) : m_stream(stream) {}

    virtual xercesc::BinInputStream* makeStream() const
    {
        return new StdBinInputStream(m_stream);
    }
private:
    std::istream& m_stream;
};

namespace std
{
    template<>
    void swap<MetaEnumerationInfo>(MetaEnumerationInfo& a, MetaEnumerationInfo& b)
    {
        a.Swap(b);
    }
}

class StcDataModelManager
{
public:

    static StcDataModelManager& Instance()
    {
        static StcDataModelManager m;
        return m;
    }

    StcDataModelManager():m_entryCnt(0)
    {
        Initialize();
    }

    ~StcDataModelManager()
    {
    }

    void Initialize()
    {
        // Note: For unittests, this is invoked before StcInit so we need to call this here
        // TODO: try to figure out a better way to deal with the unittest init
        XMLPlatformUtils::Initialize();
        InitPluginFilter();

        std::string plugins;
        std::vector<std::string> vec;

        StcSystemConfiguration::Instance().GetKey(
                "system.preloadPlugins", plugins, plugins);
        Tokenize(plugins, " ,", vec);
        m_preloadPluginFilter.insert(vec.begin(), vec.end());
        m_preloadPluginFilter.insert("framework");

        std::string lazyLoadPlugins;
        StcSystemConfiguration::Instance().GetKey(
                "system.lazyLoadPlugins", "false", lazyLoadPlugins);
        m_lazyLoading = (lazyLoadPlugins == "true");

        LoadPluginInfo();
    }

    bool HasDataModelClass(const ClassId clsId) const
    {
        return MetaClassUtil::HasDataModelClass(clsId);
    }

    bool HasDataModelClass(const std::string& clsName) const
    {
        return MetaClassUtil::HasDataModelClass(clsName);
    }

    void ActivatePluginDataModel(const std::string& pluginName)
    {
        BOOST_FOREACH(PluginMap::value_type& e, m_pluginMap)
        {
            PluginInfo& info = e.second;
            if (info.name == pluginName)
            {
                InitializePluginDataModel(info);
                return;
            }
        }
        throw CStcRunTimeError("Invalid plugin: " + pluginName);
    }

    void ActivatePluginDataModel(uint32_t pluginId)
    {
        PluginMap::iterator it = m_pluginMap.find(pluginId);
        if (it == m_pluginMap.end())
        {
            throw CStcRunTimeError("Invalid plugin");
        }
        PluginInfo& info = it->second;
        InitializePluginDataModel(info);
    }

    CMetaClass* ActivateDataModel(const std::string& clsName)
    {
        const ClassId classId = MetaClassUtil::GetClassId(clsName);
        return ActivateDataModel(classId);
    }

    CMetaClass* ActivateDataModel(const ClassId clsId)
    {
        EntryCount e(m_entryCnt);

        if (clsId == NULL_CLASS_ID())
        {
            return NULL;
        }
        const uint32_t pluginId = clsId.GetPluginId();

        PluginMap::iterator it = m_pluginMap.find(pluginId);
        if (it == m_pluginMap.end())
        {
            throw CStcRunTimeError("Invalid plugin");
        }
        PluginInfo& info = it->second;

        if (info.isInitialized)
        {
            return NULL;
        }
        // Work around unittests not specifying the proper plugin list
        // We need to reinit the filter for unittest.
        InitPluginFilter();
        if (m_entryCnt == 1 && m_pluginFilter.find(info.name) == m_pluginFilter.end())
        {
            return NULL;
        }
        CPlugin* plugin = PluginManager::Instance().FindPlugin(info.name);

        if(plugin && plugin->GetState() == CPlugin::UNAVAILABLE)
        {
            return NULL;
        }

        const bool triggerLoad =
            m_lazyLoading &&
            plugin && !plugin->IsLoaded() &&
            m_preloadPluginFilter.find(info.name) == m_preloadPluginFilter.end();

        if (triggerLoad)
        {
            char cwd[2048];
            ACE_OS::getcwd(cwd, sizeof(cwd));
            ACE_OS::chdir(CStcSystem::GetApplicationInstallPath().c_str());

            // This will trigger InitializePluginDataModel
            plugin->Load();

            assert(info.isInitialized);

            plugin->Activate();

            ACE_OS::chdir(cwd);
        }
        else
        {
            InitializePluginDataModel(info);
        }

        return CMetaClassManager::Instance().GetClass(clsId);
    }

    std::string GetPluginName(uint32_t pluginId)
    {
        PluginMap::iterator it = m_pluginMap.find(pluginId);
        if (it == m_pluginMap.end())
        {
            return std::string("unknown");
        }
        PluginInfo& info = it->second;
        return info.name;
    }

    void LoadStakCommands()
    {
        const std::string stakCommandsLocation = stc::framework::CStcSystem::GetApplicationCommonDataPath() + "STAKCommands/";
        std::vector<std::string> stakCommandFiles;
        StakCommandMetaClassBuilder mcp;

        GetStakCommandFiles(stakCommandsLocation, stakCommandFiles);

        std::string strLoadUnitTests;
        StcSystemConfiguration::Instance().GetKey("system.stakLoadUnitTests", "", strLoadUnitTests);
        const bool loadUnitTests = strLoadUnitTests == "true";

        for (std::vector<std::string>::const_iterator it = stakCommandFiles.begin(); it != stakCommandFiles.end(); ++it)
        {
            const std::string stakCommandFile = Path::JoinPath(stakCommandsLocation, *it);

            if (!loadUnitTests && StringUtil::StartsWith((*it), "unittest"))
            {
                LOG_INFO(devLogger(), "Skipping STAK file: " << stakCommandFile);
                continue;
            }

            LOG_INFO(devLogger(), "Parsing STAK file: " << stakCommandFile);
            try
            {
                mcp.Parse(stakCommandFile.c_str());
            }
            catch(const CStcRunTimeError& err)
            {
                LOG_ERROR(userLogger(), "Could not parse STAK file: " << stakCommandFile << " " << err.GetUserMessage());
            }
        }

        StakMetaClassLoadPostProcessor mpp(mcp.GetAllMetaClasses(),
                                           mcp.GetEnumRefMap(),
                                           mcp.GetCommandStatusVec(),
                                           mcp.GetRelationInfoVec());
        mpp.Process();
    }

private:

    class EntryCount {
        int& m_cnt;
    public:
        EntryCount(int& i):m_cnt(i) { ++m_cnt; }
        ~EntryCount() { --m_cnt; }
    };

    int m_entryCnt;

    struct PluginInfo
    {
        PluginInfo():
            id(0),
            isInitialized(false) {}

        uint32_t    id;
        std::string name;
        std::string modelFilePath;
        bool        isInitialized;
    };

    typedef std::map<uint32_t, PluginInfo> PluginMap;
    PluginMap m_pluginMap;

    typedef std::set<std::string> PluginFilter;
    PluginFilter m_pluginFilter;
    PluginFilter m_preloadPluginFilter;

    bool m_lazyLoading;

    ResourcePack& GetResourcePack()
    {
        static ResourcePack& pack = ResourcePack::Instance("Resource.zip");
        return pack;
    }

    void LoadPluginInfo()
    {
        ClassInfoParser p;
        p.SetPluginFilter(m_pluginFilter);

        ResourcePack::iterator it = GetResourcePack().begin("Model/*.xml");
        ResourcePack::iterator end;
        for (;it != end; ++it)
        {
            LOG_INFO(devLogger(), "LoadPluginInfo " << it.GetName());

            std::auto_ptr<std::istream> is(it.GetStream());
            p.Parse(*is.get());

            if (p.m_plugin != "")
            {
                PluginInfo info;
                info.name = p.m_plugin;
                info.id = p.m_pluginId;
                info.modelFilePath = it.GetName();

                m_pluginMap.insert(std::make_pair(info.id, info));
            }
            else
            {
            	LOG_ERROR(devLogger(), "Unable Load Plugin " << it.GetName());
            }
        }
    }

    void InitializePluginDataModel(PluginInfo& info)
    {
        if (info.isInitialized)
        {
            return;
        }

        LOG_INFO(devLogger(), "ActivatePluginDataModel " << info.name);

        info.isInitialized = true;
        MetaClassBuilder b;
        std::auto_ptr<std::istream> is(GetResourcePack().GetResourceFile(info.modelFilePath));
        b.Parse(*is.get());

        LOG_INFO(devLogger(), "ActivatedPluginDataModel " << info.name);
    }

    void InitPluginFilter()
    {
        m_pluginFilter.clear();
        std::string plugins;
        StcSystemConfiguration::Instance().GetKey(
                "system.plugins", plugins, plugins);
        std::vector<std::string> vec;
        Tokenize(plugins, " ,", vec);
        m_pluginFilter.insert(vec.begin(), vec.end());
        m_pluginFilter.insert("framework");
    }

    void GetStakCommandFiles(const std::string& stakCommandsDir, std::vector<std::string>& files)
    {
        CDirectory dir;
        int count = 0;
        if (dir.Open(stakCommandsDir.c_str()) == 0)
        {
            const char * nxtFile = dir.GetNextFileName();
            while (nxtFile != NULL)
            {
                const std::string nextFile = nxtFile;
                if (nextFile.size() > 4 &&
                    nextFile.find(".xml", nextFile.size() - 4) != std::string::npos)
                {
                    files.push_back( nextFile );
                    count++;
                }
                nxtFile = dir.GetNextFileName();
            }
        }
        if (count == 0)
        {
            LOG_INFO(devLogger(), "No STAK data model files found in: " << stakCommandsDir);
        }
    }

    class Parser : public HandlerBase
    {
    public:

        Parser()
        {
            m_parser.reset(new SAXParser);

            m_parser->setValidationScheme(SAXParser::Val_Auto);
            m_parser->setDoNamespaces(false);
            m_parser->setDoSchema(false);
            m_parser->setValidationSchemaFullChecking(false);
            m_parser->setDocumentHandler(this);
            m_parser->setErrorHandler(this);
        }

        virtual ~Parser() {}

        struct Abort {};

        void Parse(std::istream& is)
        {
            try
            {
                StdBinInputSource src(is);
                m_parser->parse(src);
            }
            catch (const Abort&)
            {
            }
        }

        void Parse(const std::string& fileName)
        {
            try
            {
                m_parser->parse(fileName.c_str());
            }
            catch (const Abort&)
            {
            }
        }

        void startElement(const XMLCh* const name, AttributeList& attributes)
        {
            const ConstString& eleName = MetaClassUtil::GetXmlString(name);
            startElement(eleName, attributes);
        }

        void endElement(const XMLCh* const name)
        {
            const ConstString& eleName = MetaClassUtil::GetXmlString(name);
            endElement(eleName);
        }

        virtual void startElement(const ConstString& eleName, AttributeList& attributes) {}
        virtual void endElement(const ConstString& eleName) {}

        // -----------------------------------------------------------------------
        //  Implementations of the SAX ErrorHandler interface
        // -----------------------------------------------------------------------
        void warning(const SAXParseException& exc) {}
        void error(const SAXParseException& exc) {}
        void fatalError(const SAXParseException& exc) {}

    protected:

        typedef TagInfo StrMap;

        std::auto_ptr<SAXParser> m_parser;

        void GetAttributeStrs(StrMap& strMap, const AttributeList& attributes)
        {
            unsigned int len = attributes.getLength();
            for (unsigned int index = 0; index < len; index++)
            {
                const ConstString& attrName = MetaClassUtil::GetXmlString(attributes.getName(index));
                const ConstString& attrValue = MetaClassUtil::GetXmlString(attributes.getValue(index));
                strMap[attrName] = attrValue;
            }
        }
    };

    class ClassInfoParser : public Parser
    {
    public:
        ConstString m_plugin;
        uint32_t m_pluginId;
        std::set<std::string> m_pluginFilter;

        ClassInfoParser():m_pluginId(0)
        {
        }

        void SetPluginFilter(const std::set<std::string>& pluginFilter)
        {
            m_pluginFilter = pluginFilter;
        }

        virtual void startElement(const ConstString& eleName, AttributeList& attributes)
        {
            if (eleName == "stc:meta")
            {
                StrMap strMap;
                GetAttributeStrs(strMap, attributes);

                //TODO: can't enable early abort since unittests don't specify the proper list
                //if (m_pluginFilter.find(strMap["plugin"].str()) == m_pluginFilter.end())
                //  throw Abort();

                ConvertToType(m_pluginId, strMap["id"].c_str());
                m_plugin = strMap["plugin"];
            }
            else if (eleName == "stc:class")
            {
                StrMap strMap;
                GetAttributeStrs(strMap, attributes);
                ClassId id;
                ConvertToType(id.m_id, strMap["id"].c_str());
                MetaClassUtil::AddClassId(strMap["name"], id);
            }
            else if (eleName == "stc:attributeType")
            {
                StrMap strMap;
                GetAttributeStrs(strMap, attributes);
                AttributeTypeId id;
                ConvertToType(id, strMap["id"].c_str());
                MetaClassUtil::AddAttributeTypeId(m_plugin, strMap["name"], id);
            }
        }
    };

    class MetaClassBuilder : public Parser
    {
        ClassId m_curPropEnumerationRefClassId;
        ClassId m_curPropEnumerationRefEnumName;

    protected:
        ConstString m_plugin;
        uint32_t m_pluginId;
        std::stack<StrMap> m_strMapStack;
        EnumInfoMap m_curEnumMap;
        std::vector<EnumEntryInfo> m_curEnumEntries;
        std::vector<CMetaClass*> m_curMetaClasses;
        std::vector<AttributeType*> m_curClsAttributes;
        std::vector<CMetaProperty*> m_curMetaProperties;
        std::vector<AttributeType*> m_curPropAttributes;
        bool inProperty;
        ConstString m_curPropType;
        std::vector< std::vector<ConstString> > m_commandStatusVec;
        std::string m_curPropEnumerationRef;
        PropValidatorImpl* m_curPropValidator;

    public:

        // build meta
        void startDocument()
        {
            while (!m_strMapStack.empty())
                m_strMapStack.pop();

            m_curMetaClasses.clear();
            m_curEnumEntries.clear();
            m_curEnumMap.clear();
            m_curMetaProperties.clear();
            m_curPropAttributes.clear();
            m_curClsAttributes.clear();
            inProperty = false;
        }

        void endDocument()
        {
            MetaClassLoadPostProcessor mcp(m_curMetaClasses, m_commandStatusVec);
            mcp.Process();
        }

        void startElement(const ConstString& eleName, AttributeList& attributes)
        {
            m_strMapStack.push(StrMap());
            StrMap& strMap = m_strMapStack.top();
            GetAttributeStrs(strMap, attributes);

            //<stc:meta id="0x01000000" package="base" plugin="core" version="0.0.0-0" xmlns:stc="urn:www.spirentcom.com:XMLSchema.xsd" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
            if (eleName == "stc:meta")
            {
                ConvertToType(m_pluginId, strMap["id"].c_str());
                m_plugin = strMap["plugin"];
            }

            //<stc:relationType allowDuplicate="false" dstName="ViewControlPlaneMplsBindingResult" id="0x01000020" name="ControlPlaneBindingResult" srcName="ProtocolConfig"/>
            else if (eleName == "stc:relationType")
            {
                MetaRelationTypeInfo mrt;
                ConvertToType(mrt.typeId.m_id, strMap["id"].c_str());
                mrt.name = strMap["name"];
                ConvertToType(mrt.saveAtSource, strMap["saveAtSource"].c_str());

                mrt.tagInfo["name"] = strMap["name"];
                mrt.tagInfo["srcName"] = strMap["srcName"];
                mrt.tagInfo["dstName"] = strMap["dstName"];
                mrt.tagInfo["saveAtSource"] = strMap["saveAtSource"];

                CRelationManager::Instance().AddMetaInfo(mrt);
            }

            //  <stc:relation allowDuplicate="false" dstClassRef="core.Node" dstName="none" embedDstInIlClass="false" embedSrcInIlClass="false" id="0x01001001" isInternal="false" maxOccurs="unbounded" minOccurs="0" name="ParentProject" saveAtSource="true" srcClassRef="framework.Project" srcName="none" type="framework.ParentChild"/>
            else if (eleName == "stc:relation")
            {
                MetaRelationInfo mr;
                mr.name = strMap["name"];

                mr.typeName = strMap["type"];
                size_t pos = mr.typeName.rfind('.', mr.typeName.size()); //2nd param is needed due to bug in rfind
                if (pos != ConstString::npos && pos != (mr.typeName.length() - 1))
                {
                    mr.typeName = MetaClassUtil::GetString(strMap["type"].substr(pos+1));

                    size_t ppos = strMap["type"].find('.');
                    if (ppos == ConstString::npos || ppos == pos)
                    {
                        ppos = 0;
                    }
                    else
                    {
                        ppos += 1;
                    }
                    assert(pos > ppos);
                    ConstString pluginName = MetaClassUtil::GetString(strMap["type"].substr(ppos, pos - ppos));
                    StcDataModelManager::Instance().ActivatePluginDataModel(pluginName);
                }

                ConvertToType(mr.id, strMap["id"].c_str());

                const MetaRelationTypeInfo* rinfo = CRelationManager::Instance().GetMetaRelationType(mr.typeName);
                assert(rinfo);
                mr.typeId = rinfo->typeId;

                mr.class1 = MetaClassUtil::GetClassId(strMap["srcClassRef"]);
                mr.class2 = MetaClassUtil::GetClassId(strMap["dstClassRef"]);

                mr.minOccurs = MapOccurrence(strMap, "minOccurs");
                mr.maxOccurs = MapOccurrence(strMap, "maxOccurs");

                mr.desc = "";
                ConvertToType(mr.isInternal, strMap["isInternal"].c_str());
                ConvertToType(mr.allowDuplicate, strMap["allowDuplicate"].c_str());

                if (strMap["order"] == "sequenced")
                {
                    mr.order = MetaRelationInfo::SEQUENCED;
                }
                else if (strMap["order"] == "none")
                {
                    mr.order = MetaRelationInfo::NONE;
                }
                else
                {
                    throw CStcRunTimeError("Unsupported relation order " + strMap["order"]);
                }

                std::swap(mr.tagInfo, strMap);
                CRelationManager::Instance().AddMetaInfo(mr);
            }

            else if (eleName == "stc:property")
            {
                inProperty = true;
                m_curPropType = strMap["type"];
                m_curPropEnumerationRef = "";
                m_curPropValidator = NULL;
            }

            //  <stc:commandTarget command="ApplyToILCommand" default="1" target="framework.Project"/>
            else if (eleName == "stc:commandTarget")
            {
                std::vector<ConstString> status;
                status.push_back(strMap["target"]);
                status.push_back(strMap["command"]);
                status.push_back(strMap["default"]);

                m_commandStatusVec.push_back(status);
            }

            // this is needed to handle out of order initialization
            else if (eleName == "stc:class")
            {
                const ClassId id = MetaClassUtil::GetClassId(strMap["baseClass"]);
                StcDataModelManager::Instance().ActivateDataModel(id);
            }
        }

        void endElement(const ConstString& eleName)
        {
            StrMap& strMap = m_strMapStack.top();

            //<stc:class baseClass="core.NetworkEndpoint" canCreate="false" id="0x0100A000" idlMapped="ifSetup" ilMapped="default" isInternal="false" isReadOnly="false" isSingleton="false" name="EmulatedIf" noSave="false" profile="none">
            if (eleName == "stc:class")
            {
                MetaClassInfo mc;

                mc.parentClassId = MetaClassUtil::GetClassId(strMap["baseClass"]);
                ConvertToType(mc.id.m_id, strMap["id"].c_str());
                mc.name = strMap["name"];
                mc.desc = "";
                ConvertToType(mc.isInternal, strMap["isInternal"].c_str());
                ConvertToType(mc.isReadOnly, strMap["isReadOnly"].c_str());
                ConvertToType(mc.noSave, strMap["noSave"].c_str());

                std::swap(mc.enumInfoMap, m_curEnumMap);
                m_curEnumMap.clear();

                std::swap(mc.tagInfo, strMap);

                CMetaClass* cls = new CMetaClass(mc); // do not delete
                CMetaClassManager::Instance().Register(cls);
                m_curMetaClasses.push_back(cls);

                BOOST_FOREACH(AttributeType* a, m_curClsAttributes)
                {
                    cls->AddAttribute(a);
                }
                m_curClsAttributes.clear();

                BOOST_FOREACH(CMetaProperty* mprop, m_curMetaProperties)
                {
                    cls->AddProperty(mprop);
                }
                m_curMetaProperties.clear();
            }

            //<stc:property category="config" collectionType="vector" databaseType="DEFAULT" default="null" displayName="Device Type" format="number" id="0x01004001" idlMapped="true" ilMapped="true" isDataStream="false" isDeprecated="false" isInternal="false" isReadOnly="false" maxOccurs="1" minOccurs="1" name="DeviceType" noSave="false" nullable="false" onModifyNotifyMode="periodic" precision="" replaces="" type="string">
            else if (eleName == "stc:property")
            {
                inProperty = false;

                if (strMap["name"].find("__") == 0)
                {
                    m_curPropAttributes.clear();
                    m_curPropEnumerationRef = "";
                    m_curPropValidator = NULL;
                }
                else
                {
                    CMetaProperty* mprop = new CMetaProperty(); // do not delete
                    MetaPropertyInfo& mp = mprop->GetMetaInfoRef();
                    mp.desc = "";
                    ConvertToType(mp.id.m_id, strMap["id"].c_str());
                    mp.name = strMap["name"];
                    mp.typeName = strMap["type"];
                    mp.defaultValue = strMap["default"];
                    ConvertToType(mp.isDeprecated, strMap["isDeprecated"].c_str());
                    ConvertToType(mp.isInternal, strMap["isInternal"].c_str());
                    ConvertToType(mp.isReadOnly, strMap["isReadOnly"].c_str());
                    ConvertToType(mp.isDataStream, strMap["isDataStream"].c_str());
                    ConvertToType(mp.noSave, strMap["noSave"].c_str());
                    ConvertToType(mp.nullable, strMap["nullable"].c_str());
                    ConvertToType(mp.isComposite, strMap["isComposite"].c_str());

                    typedef std::map<const ConstString, MetaPropertyInfo::Type> TypeMap;
                    static TypeMap typeMap = boost::assign::map_list_of
                        ("s8",             MetaPropertyInfo::BYTE)
                        ("s16",            MetaPropertyInfo::SHORT)
                        ("s32",            MetaPropertyInfo::LONG)
                        ("s64",            MetaPropertyInfo::S64)
                        ("u8",             MetaPropertyInfo::UBYTE)
                        ("u16",            MetaPropertyInfo::USHORT)
                        ("u32",            MetaPropertyInfo::ULONG)
                        ("u64",            MetaPropertyInfo::U64)
                        ("bool",           MetaPropertyInfo::BOOL)
                        ("double",         MetaPropertyInfo::DOUBLE)
                        ("string",         MetaPropertyInfo::STRING)
                        ("classId",        MetaPropertyInfo::CLASSID)
                        ("propertyId",     MetaPropertyInfo::PROPID)
                        ("relationId",     MetaPropertyInfo::RELATIONID)
                        ("handle",         MetaPropertyInfo::HANDLE)
                        ("csp",            MetaPropertyInfo::CSP)
                        ("mac",            MetaPropertyInfo::MAC)
                        ("ip",             MetaPropertyInfo::IP)
                        ("ipv6",           MetaPropertyInfo::IPV6)
                        ("chartPoint",     MetaPropertyInfo::CHARTPOINT)
                        ("eventMessage",   MetaPropertyInfo::EVENTMESSAGE)
                        ("inputFilePath",  MetaPropertyInfo::INPUT_FILE_PATH)
                        ("outputFilePath", MetaPropertyInfo::OUTPUT_FILE_PATH)
                        ("wwn",            MetaPropertyInfo::WWN);

                    TypeMap::const_iterator it = typeMap.find(mp.typeName);
                    if (it != typeMap.end())
                    {
                        mp.type = it->second;
                    }
                    else
                    {
                        throw CStcRunTimeError("Unsupported property type: " + mp.typeName);
                    }

                    if (strMap["onModifyNotifyMode"] == "instant")
                    {
                        mp.onModifyNotifyMode = MetaPropertyInfo::NOTIFY_INSTANT;
                    }
                    else if (strMap["onModifyNotifyMode"] == "queued")
                    {
                        mp.onModifyNotifyMode = MetaPropertyInfo::NOTIFY_QUEUED;
                    }
                    else if (strMap["onModifyNotifyMode"] == "periodic")
                    {
                        mp.onModifyNotifyMode = MetaPropertyInfo::NOTIFY_PERIODIC;
                    }
                    else if (strMap["onModifyNotifyMode"] == "none")
                    {
                        mp.onModifyNotifyMode = MetaPropertyInfo::NOTIFY_NONE;
                    }
                    else
                    {
                        throw CStcRunTimeError("Unsupported onModifyNotifyMode: " + strMap["onModifyNotifyMode"]);
                    }

                    mp.minOccurs = MapOccurrence(strMap, "minOccurs");
                    mp.maxOccurs = MapOccurrence(strMap, "maxOccurs");

                    if (!m_curPropEnumerationRef.empty())
                    {
                        std::string::size_type pos = m_curPropEnumerationRef.rfind('.');

                        if (pos == std::string::npos)
                        {
                            mp.enumerationRefClassId = mp.id.GetClassId();
                            mp.enumerationRefName = MetaClassUtil::GetString(m_curPropEnumerationRef);
                        }
                        else if (pos < (m_curPropEnumerationRef.size() - 1))
                        {
                            mp.enumerationRefClassId = MetaClassUtil::GetClassId(m_curPropEnumerationRef.substr(0, pos));
                            mp.enumerationRefName = MetaClassUtil::GetString(m_curPropEnumerationRef.substr(pos+1));
                        }
                        m_curPropEnumerationRef = "";
                    }

                    if (m_curPropValidator != NULL)
                    {
                        mp.validator.SetImpl(m_curPropValidator);
                        m_curPropValidator = NULL;
                    }

                    BOOST_FOREACH(AttributeType* a, m_curPropAttributes)
                    {
                        mprop->AddAttribute(a);
                    }
                    m_curPropAttributes.clear();

                    std::swap(mp.tagInfo, strMap);

                    m_curMetaProperties.push_back(mprop);
                }
            }

            //<stc:attributeType default="BITS" id="0x01000033" name="BaseUnit" type="u8">
            else if (eleName == "stc:attributeType")
            {
                if (!m_curEnumMap.empty())
                {
                    ConstString key = MetaClassUtil::GetString(m_plugin + "." + strMap["name"]);
                    std::swap(AttributeType::s_enumInfoMap[key], m_curEnumMap.begin()->second);
                }

                m_curEnumMap.clear();
            }

            //  <stc:enumeration idlMapped="true" ilMapped="true" isFlags="false" name="EnumBaseUnit">
            else if (eleName == "stc:enumeration")
            {
                MetaEnumerationInfo& info = m_curEnumMap[strMap["name"]];
                ConvertToType(info.isFlags, strMap["isFlags"].c_str());
                info.Initialize(m_curEnumEntries);
                m_curEnumEntries.clear();
            }

            //    <stc:enum displayName=" " idlMapped="true" ilMapped="true" name="NONE" value="0"/>
            else if (eleName == "stc:enum")
            {
                EnumEntryInfo info;
                info.name = strMap["name"];
                info.displayName = strMap["displayName"];

                // TODO: fix MetaInfo.h after 3.60
                //       take out the EnumEntryInfo pack...
                uint64_t value;

                ConvertToType(value, strMap["value"].c_str());

                info.value = value;

                m_curEnumEntries.push_back(info);
            }

            else if (eleName == "stc:enumerationRef")
            {
                if (inProperty)
                {
                    m_curPropEnumerationRef = strMap["ref"].str();
                }
            }

            //  <stc:attribute id="0x01064001" internalIsEnum="false" internalType="bool" name="none" type="RequireApplyToIL" value="true"/>
            else if (eleName == "stc:attribute")
            {
                ConstString name = strMap["type"];
                ConstString typeId = strMap["type"];
                size_t pos = typeId.rfind('.', typeId.size()); //2nd param is needed due to bug in rfind
                if (pos == ConstString::npos)
                {
                    typeId = MetaClassUtil::GetString(m_plugin + "." + typeId);
                }
                else if (pos != ConstString::npos && pos != (name.length() - 1))
                {
                    size_t opos = typeId.find('.');
                    if (opos != pos)
                    {
                        assert(opos < pos);
                        typeId = MetaClassUtil::GetString(name.substr(opos+1));
                    }

                    name = name.substr(pos+1);
                }
                // hack for data model bug
                if (typeId == "core.IsPropertyResettable")
                {
                    typeId = MetaClassUtil::GetString("framework.IsPropertyResettable");
                }
                if (typeId == "core.SaveOutputFilesRelativeToTRS")
                {
                    typeId = MetaClassUtil::GetString("framework.SaveOutputFilesRelativeToTRS");
                }
                std::string sval = strMap["value"].str();
                if (strMap["internalIsEnum"] == "true")
                {
                    uint64_t val;
                    AttributeType::s_enumInfoMap[typeId].GetEnumerationValue(sval, val);
                    ConvertToString(sval, val);
                }

                AttributeType* a = new AttributeType(MetaClassUtil::GetAttributeTypeId(typeId),
                                                     MetaClassUtil::GetString(name).c_str(),
                                                     MetaClassUtil::GetString(sval).c_str());
                if (inProperty)
                {
                    m_curPropAttributes.push_back(a);
                }
                else
                {
                    m_curClsAttributes.push_back(a);
                }
            }

            // validators
            if (inProperty)
            {
                if (eleName == "stc:stringValidation")
                {
                    uint16_t min, max;

                    m_curPropValidator = new StringValidator(
                                ConvertToType(min, strMap["minLength"].c_str()),
                                ConvertToType(max, strMap["maxLength"].c_str()),
                                strMap["excludePattern"].str());
                }

                else if (eleName == "stc:regExpValidation")
                {
                    m_curPropValidator = new RegExpValidator(
                                //StringUtil::Replace(strMap["pattern"].str(), "\\", "\\\\"),
                                strMap["pattern"].str(),
                                strMap["errorMsg"].str());
                }

                else if (eleName == "stc:numericValidation")
                {
                    m_curPropValidator = MakeValidator(m_curPropType, strMap);
                }

                else if (eleName == "stc:ipv4Validation")
                {
                    if (strMap["unicast"] == "true")
                    {
                        m_curPropValidator = new Ipv4Validator<CIpAddress>();
                    }
                }
            }

            m_strMapStack.pop();
        }

    protected:
        static int MapOccurrence(StrMap& strMap, const ConstString& name)
        {
            int occurrence = -1;
            if (strMap[name] != "unbounded")
            {
                ConvertToType(occurrence, strMap[name].c_str());
            }
            return occurrence;
        }

        PropValidatorImpl* MakeValidator(ConstString type, StrMap& strMap)
        {
            if (type == "s8")     { return MakeValidatorT<int8_t>(strMap); }
            if (type == "s16")    { return MakeValidatorT<int16_t>(strMap); }
            if (type == "s32")    { return MakeValidatorT<int32_t>(strMap); }
            if (type == "s64")    { return MakeValidatorT<int64_t>(strMap); }
            if (type == "u8")     { return MakeValidatorT<uint8_t>(strMap); }
            if (type == "u16")    { return MakeValidatorT<uint16_t>(strMap); }
            if (type == "u32")    { return MakeValidatorT<uint32_t>(strMap); }
            if (type == "u64")    { return MakeValidatorT<uint64_t>(strMap); }
            if (type == "double") { return MakeValidatorT<double>(strMap); }

            return NULL;
        }

    private:
        template <class T>
        PropValidatorImpl* MakeValidatorT(StrMap& strMap)
        {
            T min, max;
            return new NumericValidator<T>(
                ConvertToType(min, strMap["minInclusive"].c_str()),
                ConvertToType(max, strMap["maxInclusive"].c_str()));
        }
    };

    class StakCommandMetaClassBuilder : public MetaClassBuilder
    {
    public:

        StakCommandMetaClassBuilder()
        {
            configureParser();
        }

        static int s_stakCommandMetaId;
        static int s_stakCommandClassId;
        static int s_stakCommandPropId;
        static int s_stakRelationId;
        static int s_stakRelationTypeId;

    private:
        ConstString m_package;
        ConstString m_globalPackage;

    public:

        std::vector<CMetaClass*>& GetAllMetaClasses()
        {
            static std::vector<CMetaClass*> metaClasses;
            return metaClasses;
        }

        std::vector< std::vector<ConstString> >& GetCommandStatusVec()
        {
            static std::vector< std::vector<ConstString> > commandStatusVec;
            return commandStatusVec;
        }

        typedef std::map<MetaPropertyInfo*, std::string> PropInfoEnumRefMap_t;
        PropInfoEnumRefMap_t& GetEnumRefMap()
        {
            static PropInfoEnumRefMap_t enumRefMap;
            return enumRefMap;
        }

        StakMetaClassLoadPostProcessor::RelationInfoVec_t& GetRelationInfoVec()
        {
            static StakMetaClassLoadPostProcessor::RelationInfoVec_t riVec;
            return riVec;
        }

        void endDocument()
        {
            // Class registration and post processing are deferred until all
            // documents have been loaded and parsed so we can make sure
            // the registration order is correct for inheritance hierarchies.
            std::vector<CMetaClass*>& allMetaClasses = GetAllMetaClasses();
            allMetaClasses.insert(allMetaClasses.end(), m_curMetaClasses.begin(), m_curMetaClasses.end());

            std::vector< std::vector<ConstString> >& commandStatusVec = GetCommandStatusVec();
            commandStatusVec.insert(commandStatusVec.end(), m_commandStatusVec.begin(), m_commandStatusVec.end());
        }

        void startElement(const ConstString& eleName, AttributeList& attributes)
        {
            m_strMapStack.push(StrMap());
            StrMap& strMap = m_strMapStack.top();
            GetAttributeStrs(strMap, attributes);

            //<stc:meta id="0x01000000" package="base" plugin="core" version="0.0.0-0" xmlns:stc="urn:www.spirentcom.com:XMLSchema.xsd" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
            if (eleName == "stc:meta")
            {
                m_pluginId = s_stakCommandMetaId;
                m_plugin = strMap["plugin"];
                m_package = strMap["package"];
                m_globalPackage = m_package;
            }

            else if (eleName == "stc:attributeType")
            {
                AttributeTypeId id = s_stakCommandPropId;
                s_stakCommandPropId += 0x01;
                MetaClassUtil::AddAttributeTypeId(m_plugin, strMap["name"], id);
                LOG_INFO(devLogger(), "Adding STAK class atttribute " << strMap["name"] << " with property id " << id);
            }

            //<stc:relationType allowDuplicate="false" dstName="ViewControlPlaneMplsBindingResult" id="0x01000020" name="ControlPlaneBindingResult" srcName="ProtocolConfig"/>
            else if (eleName == "stc:relationType")
            {
                MetaRelationTypeInfo mrt;
                mrt.typeId.m_id = s_stakRelationTypeId;
                mrt.name = strMap["name"];
                if (strMap.find("saveAtSource") == strMap.end())
                {
                    mrt.saveAtSource = true;
                }
                else
                {
                    ConvertToType(mrt.saveAtSource, strMap["saveAtSource"].c_str());
                }

                mrt.tagInfo["name"] = strMap["name"];
                mrt.tagInfo["srcName"] = strMap["srcName"];
                mrt.tagInfo["dstName"] = strMap["dstName"];
                mrt.tagInfo["saveAtSource"] = mrt.saveAtSource;

                CRelationManager::Instance().AddMetaInfo(mrt);
                s_stakRelationTypeId += 0x00000001;
            }

            //  <stc:relation allowDuplicate="false" dstClassRef="core.Node" dstName="none" embedDstInIlClass="false" embedSrcInIlClass="false" id="0x01001001" isInternal="false" maxOccurs="unbounded" minOccurs="0" name="ParentProject" saveAtSource="true" srcClassRef="framework.Project" srcName="none" type="framework.ParentChild"/>
            else if (eleName == "stc:relation")
            {
                MetaRelationInfo* mr = new MetaRelationInfo();
                mr->name = strMap["name"];
                mr->typeName = strMap["type"];
                mr->id = s_stakRelationId;
                size_t pos = mr->typeName.rfind('.', mr->typeName.size()); //2nd param is needed due to bug in rfind
                if (pos != ConstString::npos && pos != (mr->typeName.length() - 1))
                {
                    mr->typeName = MetaClassUtil::GetString(strMap["type"].substr(pos+1));
                }

                const MetaRelationTypeInfo* rinfo = CRelationManager::Instance().GetMetaRelationType(mr->typeName);
                assert(rinfo);
                if(rinfo == NULL)
                {
                    std::ostringstream msg;
                    msg << "Relation type " << mr->typeName << " has not been defined";
                    throw CStcRunTimeError(msg.str());
                }

                mr->typeId = rinfo->typeId;

                if(strMap.find("minOccurs") == strMap.end())
                {
                    strMap["minOccurs"] = "0";
                }

                if(strMap.find("maxOccurs") == strMap.end())
                {
                    strMap["maxOccurs"] = "unbounded";
                }

                mr->minOccurs = MapOccurrence(strMap, "minOccurs");
                mr->maxOccurs = MapOccurrence(strMap, "maxOccurs");
                mr->desc = "";
                mr->isInternal = false;
                mr->allowDuplicate = false;
                mr->order = MetaRelationInfo::SEQUENCED;

                // The remainder of the setup is deferred until all classes have been registered.
                StakMetaClassLoadPostProcessor::RelationInfoVec_t& riVec = GetRelationInfoVec();
                StakMetaClassLoadPostProcessor::RelationInfo_t* relInfo =
                    new StakMetaClassLoadPostProcessor::RelationInfo_t(mr,
                                                                       strMap["srcClassRef"],
                                                                       strMap["dstClassRef"]);
                riVec.push_back(relInfo);
                std::swap(mr->tagInfo, strMap);
                s_stakRelationId += 0x00000001;
            }

            else if (eleName == "stc:property")
            {
                inProperty = true;
                m_curPropType = strMap["type"];
                m_curPropEnumerationRef = "";
                m_curPropValidator = NULL;
            }

            //  <stc:commandTarget command="ApplyToILCommand" default="1" target="framework.Project"/>
            else if (eleName == "stc:commandTarget")
            {
                std::vector<ConstString> status;
                status.push_back(strMap["target"]);
                status.push_back(strMap["command"]);
                status.push_back(strMap["default"]);

                m_commandStatusVec.push_back(status);
            }

            // this is needed to handle out of order initialization
            else if (eleName == "stc:class")
            {
                if(strMap["baseClass"].empty())
                {
                    strMap["baseClass"] = "stak.StakCommand";
                }

                if(strMap["canCreate"].empty())
                {
                    strMap["canCreate"] = "true";
                }

                // class:packageName overrides meta:package
                TagInfo::iterator it = strMap.find("packageName");
                if(it != strMap.end())
                {
                    m_package = it->second;
                }
                else
                {
                    strMap["packageName"] = m_globalPackage;
                }
            }
        }

        void endElement(const ConstString& eleName)
        {
            StrMap& strMap = m_strMapStack.top();

            //<stc:class baseClass="core.NetworkEndpoint" canCreate="false" id="0x0100A000" idlMapped="ifSetup" ilMapped="default" isInternal="false" isReadOnly="false" isSingleton="false" name="EmulatedIf" noSave="false" profile="none">
            if (eleName == "stc:class")
            {
                MetaClassInfo mc;
                mc.name = m_package + "." + strMap["name"];

                const std::string plugin = StringToLower(m_plugin);
                if (!plugin.empty() &&
                    plugin != "stakcommands" &&
                    plugin != "plcommands")
                {
                    if(PluginManager::Instance().FindPlugin(plugin) == NULL)
                    {
                        LOG_INFO(devLogger(),
                                 plugin << " plugin is not loaded. Cannot load STAK class " << mc.name);
                        return;
                    }
                }


                mc.id = s_stakCommandClassId;
                mc.desc = "";
                mc.noSave = false;
                mc.isInternal = ( strMap["isInternal"].empty() ) ? false :
                    ConvertToType(mc.isInternal, strMap["isInternal"].c_str());
                mc.isReadOnly = false;

                bool bllDirectInterface = false;
                StrMap::iterator it = strMap.find("bllDirectInterface");
                if ( it != strMap.end() && it->second != "false")
                {
                    bllDirectInterface = true;
                }

                std::string pythonVirtualenv;
                it = strMap.find("pythonVirtualenv");
                if (it != strMap.end() && !(it->second.empty()))
                {
                    pythonVirtualenv = it->second;
                }

                std::swap(mc.enumInfoMap, m_curEnumMap);
                m_curEnumMap.clear();

                std::swap(mc.tagInfo, strMap);

                CMetaClass* cls = new StakCommandMetaClass(mc, bllDirectInterface, pythonVirtualenv);
                m_curMetaClasses.push_back(cls);

                BOOST_FOREACH(AttributeType* a, m_curClsAttributes)
                {
                    cls->AddAttribute(a);
                }
                m_curClsAttributes.clear();

                BOOST_FOREACH(CMetaProperty* mprop, m_curMetaProperties)
                {
                    MetaPropertyInfo& mp = mprop->GetMetaInfoRef();
                    LOG_DEBUG(devLogger(), "Adding STAK property " << mp.name << " with property id " << mp.id);
                    cls->AddProperty(mprop);
                }
                m_curMetaProperties.clear();

                s_stakCommandClassId += 0x00001000;
                s_stakCommandPropId = s_stakCommandClassId + 0x00000001;

                // package may be overriden by class packageName
                m_package = m_globalPackage;
            }

            //<stc:attributeType default="BITS" id="0x01000033" name="BaseUnit" type="u8">
            else if (eleName == "stc:attributeType")
            {
                if (!m_curEnumMap.empty())
                {
                    ConstString key = MetaClassUtil::GetString(m_plugin + "." + strMap["name"]);
                    std::swap(AttributeType::s_enumInfoMap[key], m_curEnumMap.begin()->second);
                }

                m_curEnumMap.clear();
            }

            //<stc:property category="config" collectionType="vector" databaseType="DEFAULT" default="null" displayName="Device Type" format="number" id="0x01004001" idlMapped="true" ilMapped="true" isDataStream="false" isDeprecated="false" isInternal="false" isReadOnly="false" maxOccurs="1" minOccurs="1" name="DeviceType" noSave="false" nullable="false" onModifyNotifyMode="periodic" precision="" replaces="" type="string">
            else if (eleName == "stc:property")
            {
                inProperty = false;

                if (strMap["name"].find("__") == 0)
                {
                    m_curPropAttributes.clear();
                    m_curPropEnumerationRef = "";
                    m_curPropValidator = NULL;
                }
                else
                {
                    CMetaProperty* mprop = new CMetaProperty(); // do not delete
                    MetaPropertyInfo& mp = mprop->GetMetaInfoRef();
                    mp.desc = "";

                    mp.id = s_stakCommandPropId;
                    s_stakCommandPropId += 0x01;

                    mp.name = strMap["name"];
                    mp.typeName = strMap["type"];
                    mp.defaultValue = strMap["default"];
                    mp.isDeprecated = false;
                    mp.isInternal = ( strMap["isInternal"].empty() ) ? false : ConvertToType(mp.isInternal, strMap["isInternal"].c_str());
                    mp.nullable = ( strMap["nullable"].empty() ) ?  false : ConvertToType(mp.nullable, strMap["nullable"].c_str());
                    mp.isDataStream = false;
                    mp.isComposite = false;
                    mp.onModifyNotifyMode = MetaPropertyInfo::NOTIFY_NONE;

                    if (strMap["category"] == "input")
                    {
                        mp.isReadOnly = false;
                        mp.noSave = false;
                    }
                    else if (strMap["category"] == "output")
                    {
                        mp.isReadOnly = true;
                        mp.noSave = true;
                    }
                    else if (strMap["category"] == "state")
                    {
                        mp.isReadOnly = true;
                        mp.noSave = true;
                        mp.isInternal = true;
                    }
                    else if (strMap["category"] == "config")
                    {
                        mp.isReadOnly = false;
                        mp.noSave = false;
                        mp.isInternal = false;
                    }
                    else if (strMap["category"] == "result")
                    {
                        mp.isReadOnly = true;
                        mp.noSave = true;
                        mp.isInternal = false;
                    }
                    else
                    {
                        throw CStcRunTimeError("Unsupported category type: " + strMap["category"]);
                    }

                    if (strMap.find("minOccurs") != strMap.end())
                    {
                        mp.minOccurs = MapOccurrence(strMap, "minOccurs");
                    }
                    else
                    {
                        mp.minOccurs = 1;
                    }
                    if (strMap.find("maxOccurs") != strMap.end())
                    {
                        mp.maxOccurs = MapOccurrence(strMap, "maxOccurs");
                    }
                    else
                    {
                        mp.maxOccurs = 1;
                    }

                    typedef std::map<const ConstString, MetaPropertyInfo::Type> TypeMap;
                    static TypeMap typeMap = boost::assign::map_list_of
                        ("s8",             MetaPropertyInfo::BYTE)
                        ("s16",            MetaPropertyInfo::SHORT)
                        ("s32",            MetaPropertyInfo::LONG)
                        ("s64",            MetaPropertyInfo::S64)
                        ("u8",             MetaPropertyInfo::UBYTE)
                        ("u16",            MetaPropertyInfo::USHORT)
                        ("u32",            MetaPropertyInfo::ULONG)
                        ("u64",            MetaPropertyInfo::U64)
                        ("bool",           MetaPropertyInfo::BOOL)
                        ("double",         MetaPropertyInfo::DOUBLE)
                        ("string",         MetaPropertyInfo::STRING)
                        ("handle",         MetaPropertyInfo::HANDLE)
                        ("mac",            MetaPropertyInfo::MAC)
                        ("ip",             MetaPropertyInfo::IP)
                        ("ipv6",           MetaPropertyInfo::IPV6)
                        ("inputFilePath",  MetaPropertyInfo::INPUT_FILE_PATH)
                        ("outputFilePath", MetaPropertyInfo::OUTPUT_FILE_PATH)
                        ("wwn",            MetaPropertyInfo::WWN);

                    TypeMap::const_iterator it = typeMap.find(mp.typeName);
                    if (it != typeMap.end())
                    {
                        mp.type = it->second;
                    }
                    else
                    {
                        throw CStcRunTimeError("Unsupported property type: " + mp.typeName);
                    }

                    if (mp.nullable && (mp.type == MetaPropertyInfo::STRING ||
                                        mp.type == MetaPropertyInfo::OUTPUT_FILE_PATH ||
                                        mp.type == MetaPropertyInfo::INPUT_FILE_PATH))
                    {
                        std::ostringstream msg;
                        msg << mp.typeName << " types cannot be nullable.";
                        throw CStcRunTimeError(msg.str());
                    }

                    if (!mp.nullable && mp.defaultValue == "" &&
                        mp.type != MetaPropertyInfo::STRING &&
                        mp.type != MetaPropertyInfo::OUTPUT_FILE_PATH &&
                        mp.type != MetaPropertyInfo::INPUT_FILE_PATH)
                    {
                        mp.defaultValue = "0";
                    }

                    if (!m_curPropEnumerationRef.empty())
                    {
                        std::string::size_type pos = m_curPropEnumerationRef.rfind('.');

                        if (pos == std::string::npos)
                        {
                            mp.enumerationRefClassId = mp.id.GetClassId();
                            mp.enumerationRefName = MetaClassUtil::GetString(m_curPropEnumerationRef);
                        }
                        else if (pos < (m_curPropEnumerationRef.size() - 1))
                        {
                            const std::string refClassName = m_curPropEnumerationRef.substr(0, pos);
                            const CMetaClass* refMc = CMetaClassManager::Instance().GetClass(refClassName);
                            if (refMc)
                            {
                                // If the class is already registered, it's not a STAK command since
                                // STAK command registration is deferred. Go ahead and assign the ref id.
                                mp.enumerationRefClassId = refMc->GetClassId();
                            }
                            else
                            {
                                // Cache and wait for post processing to address registration order.
                                PropInfoEnumRefMap_t& enumRefMap = GetEnumRefMap();
                                enumRefMap[&mp] = refClassName;
                            }

                            mp.enumerationRefName = MetaClassUtil::GetString(m_curPropEnumerationRef.substr(pos+1));
                        }
                        m_curPropEnumerationRef = "";
                    }

                    if (m_curPropValidator != NULL)
                    {
                        mp.validator.SetImpl(m_curPropValidator);
                        m_curPropValidator = NULL;
                    }

                    BOOST_FOREACH(AttributeType* a, m_curPropAttributes)
                    {
                        mprop->AddAttribute(a);
                    }
                    m_curPropAttributes.clear();

                    std::swap(mp.tagInfo, strMap);

                    m_curMetaProperties.push_back(mprop);
                }
            }

            //  <stc:enumeration idlMapped="true" ilMapped="true" isFlags="false" name="EnumBaseUnit">
            else if (eleName == "stc:enumeration")
            {
                MetaEnumerationInfo& info = m_curEnumMap[strMap["name"]];
                const std::string isFlags = ( strMap.find("isFlags") == strMap.end() ) ?
                    "false" : strMap["isFlags"];
                ConvertToType(info.isFlags, isFlags.c_str());
                info.Initialize(m_curEnumEntries);
                m_curEnumEntries.clear();
            }

            //    <stc:enum displayName=" " idlMapped="true" ilMapped="true" name="NONE" value="0"/>
            else if (eleName == "stc:enum")
            {
                EnumEntryInfo info;
                info.name = strMap["name"];
                info.displayName = strMap["displayName"];

                // TODO: fix MetaInfo.h after 3.60
                //       take out the EnumEntryInfo pack...
                uint64_t value;

                ConvertToType(value, strMap["value"].c_str());

                info.value = value;

                m_curEnumEntries.push_back(info);
            }

            else if (eleName == "stc:enumerationRef")
            {
                if (inProperty)
                {
                    m_curPropEnumerationRef = strMap["ref"].str();
                }
            }

            //  <stc:attribute id="0x01064001" internalIsEnum="false" internalType="bool" name="none" type="RequireApplyToIL" value="true"/>
            else if (eleName == "stc:attribute")
            {
                ConstString name = strMap["type"];
                ConstString typeId = strMap["type"];
                size_t pos = typeId.rfind('.', typeId.size()); //2nd param is needed due to bug in rfind
                if (pos == ConstString::npos)
                {
                    typeId = MetaClassUtil::GetString(m_plugin + "." + typeId);
                }
                else if (pos != ConstString::npos && pos != (name.length() - 1))
                {
                    size_t opos = typeId.find('.');
                    if (opos != pos)
                    {
                        assert(opos < pos);
                        typeId = MetaClassUtil::GetString(name.substr(opos+1));
                    }

                    name = name.substr(pos+1);
                }
                // hack for data model bug
                if (typeId == "core.IsPropertyResettable")
                {
                    typeId = MetaClassUtil::GetString("framework.IsPropertyResettable");
                }
                if (typeId == "core.SaveOutputFilesRelativeToTRS")
                {
                    typeId = MetaClassUtil::GetString("framework.SaveOutputFilesRelativeToTRS");
                }
                std::string sval = strMap["value"].str();
                if (strMap["internalIsEnum"] == "true")
                {
                    uint64_t val;
                    AttributeType::s_enumInfoMap[typeId].GetEnumerationValue(sval, val);
                    ConvertToString(sval, val);
                }

                AttributeType* a = new AttributeType(MetaClassUtil::GetAttributeTypeId(typeId),
                                                     MetaClassUtil::GetString(name).c_str(),
                                                     MetaClassUtil::GetString(sval).c_str());
                if (inProperty)
                {
                    m_curPropAttributes.push_back(a);
                }
                else
                {
                    m_curClsAttributes.push_back(a);
                }
            }

            // validators
            if (inProperty)
            {
                if (eleName == "stc:stringValidation")
                {
                    uint16_t min, max;

                    m_curPropValidator = new StringValidator(
                                ConvertToType(min, strMap["minLength"].c_str()),
                                ConvertToType(max, strMap["maxLength"].c_str()),
                                strMap["excludePattern"].str());
                }

                else if (eleName == "stc:regExpValidation")
                {
                    m_curPropValidator = new RegExpValidator(
                                //StringUtil::Replace(strMap["pattern"].str(), "\\", "\\\\"),
                                strMap["pattern"].str(),
                                strMap["errorMsg"].str());
                }

                else if (eleName == "stc:numericValidation")
                {
                    m_curPropValidator = MakeValidator(m_curPropType, strMap);
                }

                else if (eleName == "stc:ipv4Validation")
                {
                    if (strMap["unicast"] == "true")
                    {
                        m_curPropValidator = new Ipv4Validator<CIpAddress>();
                    }
                }
            }

            m_strMapStack.pop();
        }

        std::string formatError(const SAXParseException& exc)
        {
            std::ostringstream msg;
            char* exMsg = XMLString::transcode(exc.getMessage());
            msg << std::string(exMsg) << " at line number: " << exc.getLineNumber() << " column: " << exc.getColumnNumber();
            XMLString::release(&exMsg);
            return msg.str();
        }

        virtual void error (const SAXParseException& exc)
        {
            std::string msg = formatError(exc);
            throw CStcRunTimeError(msg);
        }

        virtual void fatalError (const SAXParseException& exc)
        {
            std::string msg = formatError(exc);
            throw CStcRunTimeError(msg);
        }

        virtual void warning (const SAXParseException& exc)
        {
            std::string msg = formatError(exc);
            LOG_WARN(devLogger(), msg);
        }

    private:
        void configureParser()
        {
            std::string schemaEnabled;
            StcSystemConfiguration::Instance().GetKey("system.enableStakValidation", "false", schemaEnabled);
            if(StringToLower(schemaEnabled) == "true")
            {
                LOG_INFO(devLogger(), "STAK registration validation enabled.");
                std::string schemaPath("file:///");
                schemaPath += StringUtil::Replace(Path::JoinPath(
                    CStcSystem::GetApplicationInstallPath(), "Model/stc_STAK.xsd"), "\\", "/");
                schemaPath = "urn:www.spirentcom.com:XMLSchema.xsd " + schemaPath;
                XMLCh* schemaLoc = XMLString::transcode(schemaPath.c_str());
                m_parser->setExternalSchemaLocation(schemaLoc);
                XMLString::release(&schemaLoc);
                m_parser->setDoSchema(true);
                m_parser->setDoNamespaces(true);
            }
            else
            {
                LOG_INFO(devLogger(), "STAK registration validation disabled.");
            }
        }
    };
};

//Stak Command Based Meta Data Identifier - Base of 0x92 (decimal of 146 from stcConstRange.txt) is used
int StcDataModelManager::StakCommandMetaClassBuilder::s_stakCommandMetaId = 0x92000000;
int StcDataModelManager::StakCommandMetaClassBuilder::s_stakCommandClassId = s_stakCommandMetaId + 0x00005000;
int StcDataModelManager::StakCommandMetaClassBuilder::s_stakCommandPropId = s_stakCommandClassId + 0x00000001;
int StcDataModelManager::StakCommandMetaClassBuilder::s_stakRelationId = s_stakCommandMetaId + 0x00000001;
int StcDataModelManager::StakCommandMetaClassBuilder::s_stakRelationTypeId = s_stakCommandMetaId + 0x00000001;

CMetaClassManager&
CMetaClassManager::Instance()
{
    static CMetaClassManager mgr;
    return mgr;
}

CMetaClassManager::CMetaClassManager() :
    mClassMap(),
    mClassNameMap(),
    mAltMetaClassCreator(NULL)
{
}

CMetaClassManager::~CMetaClassManager()
{
}

void
CMetaClassManager::Register(CMetaClass* pCls)
{
    mClassMap.insert(std::make_pair(pCls->GetMetaInfo().id, pCls));

    // class name map will be kept as lower case for case-insensitive lookup
    const std::string className = pCls->GetMetaInfo().name;
    mClassNameMap.insert(std::make_pair(StringToLower(className), pCls));
}

void
CMetaClassManager::Register(CMetaClass* pCls, const std::string& fqClassName)
{
    mClassMap.insert(std::make_pair(pCls->GetMetaInfo().id, pCls));

    // class name map will be kept as lower case for case-insensitive lookup
    mClassNameMap.insert(std::make_pair(StringToLower(fqClassName), pCls));
}

CMetaClass*
CMetaClassManager::Unregister(const ClassId& id)
{
    ClassMap::iterator it = mClassMap.find(id);
    if (it != mClassMap.end())
    {
        CMetaClass* cls = it->second;
        mClassMap.erase(it);

        // class name map will be kept as lower case for case-insensitive lookup
        const std::string className = cls->GetMetaInfo().name;
        mClassNameMap.erase(StringToLower(className));
        return cls;
    }

    return 0;
}

void
CMetaClassManager::ActivateDataModel(uint32_t pluginId)
{
    StcDataModelManager::Instance().ActivatePluginDataModel(pluginId);
}

CMetaClass*
CMetaClassManager::GetClass(const ClassId& id) const
{
    ClassMap::const_iterator it = mClassMap.find(id);
    if (it != mClassMap.end())
    {
        return it->second;
    }
    else if (StcDataModelManager::Instance().HasDataModelClass(id))
    {
        return StcDataModelManager::Instance().ActivateDataModel(id);
    }

    return 0;
}

CMetaClass*
CMetaClassManager::GetClass(const std::string& name,
                            const CMetaClass* prntClass,
                            bool useAltCreator) const
{
    std::string qualifiedName = StringToLower((prntClass) ? prntClass->QualifyChildName(name) : name);

    // class name map will be kept as lower case for case-insensitive lookup
    ClassNameMap::const_iterator it =
        mClassNameMap.find(qualifiedName);
    if (it != mClassNameMap.end())
    {
        return it->second;
    }
    if (StcDataModelManager::Instance().HasDataModelClass(qualifiedName))
    {
        return StcDataModelManager::Instance().ActivateDataModel(qualifiedName);
    }

	if (useAltCreator && mAltMetaClassCreator)
    {
        return mAltMetaClassCreator->GetClass(name);
	}

    return 0;
}

std::string
CMetaClassManager::GetPluginName(const ClassId& id) const
{
    return StcDataModelManager::Instance().GetPluginName(id.GetPluginId());
}

const MetaEnumerationInfo *
CMetaClassManager::GetEnumeration(const MetaPropertyInfo& mpi) const
{

    ClassId enumRefClass = mpi.enumerationRefClassId;
    const CMetaClass * mc = GetClass(enumRefClass);

    const std::string & enumRefName = mpi.enumerationRefName;

    while (mc != NULL)
    {
        const EnumInfoMap & enumInfoMap = mc->GetMetaInfo().enumInfoMap;
        EnumInfoMap::const_iterator it = enumInfoMap.find(enumRefName);
        if (it != enumInfoMap.end())
        {
            return &(*it).second;
        }
        mc = mc->GetParent();
    }

    return NULL;
}

/**
    Returns the number of meta classes.
 */
size_t CMetaClassManager::GetMetaClassCount() const
{
    return mClassMap.size();
}

/**
    Returns a meta class iterator
 */
CMetaClassIterator CMetaClassManager::GetMetaClassIterator() const
{
    return CMetaClassIterator(mClassMap.begin(), mClassMap.end());
}

void
CMetaClassManager::LoadStakCommands()
{
    StcDataModelManager::Instance().LoadStakCommands();
}

bool CMetaClassManager::IsStakCommand(const MetaClassInfo& mci) const
{
    if(mci.name == "StakCommand" ||
       mci.name == "StakGroupCommand" ||
	   mci.name == "StakWhileCommand")
    {
        return true;
    }

    TagInfo::const_iterator it = mci.tagInfo.find("baseClass");
    if(it != mci.tagInfo.end() &&
      (it->second == "stak.StakCommand" ||
       it->second == "stak.StakGroupCommand" ||
	   mci.name == "stak.StakWhileCommand") )
    {
        return true;
    }

    CMetaClass* parentMc = GetClass(mci.parentClassId);
    if(parentMc)
    {
        return IsStakCommand(parentMc->GetMetaInfo());
    }

    return false;
}

bool CMetaClassManager::IsStakGroupCommand(const MetaClassInfo& mci) const
{
    if(mci.name == "StakGroupCommand")
    {
        return true;
    }

    TagInfo::const_iterator it = mci.tagInfo.find("baseClass");
    if(it != mci.tagInfo.end() && it->second == "stak.StakGroupCommand")
    {
        return true;
    }

    CMetaClass* parentMc = GetClass(mci.parentClassId);
    if(parentMc)
    {
        return IsStakGroupCommand(parentMc->GetMetaInfo());
    }

    return false;
}

bool CMetaClassManager::IsStakWhileCommand(const MetaClassInfo& mci) const
{
    if(mci.name == "StakWhileCommand")
    {
        return true;
    }

    TagInfo::const_iterator it = mci.tagInfo.find("baseClass");
    if(it != mci.tagInfo.end() && it->second == "stak.StakWhileCommand")
    {
        return true;
    }

    CMetaClass* parentMc = GetClass(mci.parentClassId);
    if(parentMc)
    {
        return IsStakWhileCommand(parentMc->GetMetaInfo());
    }

    return false;
}

bool CMetaClassManager::IsDynamicScriptable(const MetaClassInfo& mci) const
{
    if(mci.name == "DynamicScriptable")
    {
        return true;
    }

    TagInfo::const_iterator it = mci.tagInfo.find("baseClass");
    std::string base = it->second;
    if(it != mci.tagInfo.end() && it->second == "stak.DynamicScriptable")
    {
        return true;
    }

    CMetaClass* parentMc = GetClass(mci.parentClassId);
    if(parentMc)
    {
        return IsDynamicScriptable(parentMc->GetMetaInfo());
    }

    return false;
}

bool CMetaClassManager::IsDynamicScriptable(const std::string& className) const
{
    CMetaClass* mc = GetClass(className);
    if(!mc)
    {
        return false;
    }

    return IsDynamicScriptable(mc->GetMetaInfo());
}

static bool nameIsSimilar(const std::string& expected, const std::string& actual)
{
    if (actual.size() > 0 &&
        CaselessStartsWith(expected.c_str(), actual.c_str()))
    {
        // beginning is same
        return true;
    }

    // true is entire string is similar
    return (CaselessStringDiff(actual, expected, actual.size() * 2 / 3) != -1);
}

std::string CMetaClassManager::GetSuggestions(const std::string& clsName, bool prefixErrorMsg)
{
    CMetaClassManager& mcm = CMetaClassManager::Instance();
    CMetaClassIterator mIter = mcm.GetMetaClassIterator();
    std::vector<std::string> classNames;
    std::string thing("class"), things("classes"), cmdName;
    bool isCommand = false;

    if (CaselessEndsWith(clsName.c_str(), "Command"))
    {
        thing = "command";
        things = "commands";
        cmdName = clsName.substr(0, clsName.size() - 7);
        isCommand = true;
    }

    while (mIter.HasNext())
    {
        const MetaClassInfo& info(mIter.Next()->GetMetaInfo());
        TagInfo::const_iterator canCreateIter = info.tagInfo.find("canCreate");

        if (!info.isInternal &&
            canCreateIter != info.tagInfo.end() && canCreateIter->second != std::string("false"))
        {
            if (isCommand)
            {
                if (CaselessEndsWith(info.name.c_str(), "Command") &&
                    nameIsSimilar(info.name.substr(0, info.name.size() - 7), cmdName))
                    classNames.push_back(info.name);
            }
            else
            {
                if (nameIsSimilar(info.name, clsName))
                    classNames.push_back(info.name);
            }
        }
    }

    std::string suggestions("");
    if(prefixErrorMsg)
    {
        suggestions += "unable to create unknown " + thing + " \"" + clsName + "\"";
    }

    if (classNames.size() == 1)
    {
        suggestions += ": a similar " + thing + " is " + classNames[0];
    }
    else if (!classNames.empty())
    {
        suggestions += ": similar " + things + " include ";
        std::sort(classNames.begin(), classNames.end());
        AppendCommaSepList(suggestions, classNames, "and");
    }

    return suggestions;
}

/**
    Ctor
 */
CMetaClassIterator::CMetaClassIterator()
{
}

/**
    Dtor
 */
CMetaClassIterator::~CMetaClassIterator()
{
}

/**
    Ctor
 */
CMetaClassIterator::CMetaClassIterator
(
    CMetaClassManager::ClassMap::const_iterator begin,  ///< begin iter
    CMetaClassManager::ClassMap::const_iterator end     ///< end iter
):
    mIt(begin),
    mEndIt(end)
{
}

/**
    Copy Ctor
 */
CMetaClassIterator::CMetaClassIterator(const CMetaClassIterator& iter)
{
    mIt = iter.mIt;
    mEndIt = iter.mEndIt;
}

/**
    Assignment operator
 */
CMetaClassIterator CMetaClassIterator::operator = (const CMetaClassIterator& iter)
{
    if (&iter == this)
    {
        return (*this);
    }

    mIt = iter.mIt;
    mEndIt = iter.mEndIt;

    return (*this);
}

/**
    Returns the next CMetaClasses and advances the iterator.
 */
CMetaClass* CMetaClassIterator::Next()
{
    if (!HasNext())
    {
        return 0;
    }

    CMetaClass* cls = mIt->second;
    ++mIt;

    return cls;
}

/**
    Returns if there are more CMetaClasses.
 */
bool CMetaClassIterator::HasNext() const
{
    return (mIt != mEndIt);
}
