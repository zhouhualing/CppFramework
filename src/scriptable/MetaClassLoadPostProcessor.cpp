#include "StdAfx.h"
#include "MetaClassLoadPostProcessor.h"
#include "BLLLoggerFactory.h"
#include "ChartPoint.h"
#include "ChassisSlotPort.h"
#include "CowDataStore.h"
#include "CollectionProperty.h"
#include "CommandStatusManager.h"
#include "EnumProperty.h"
#include "MetaClassManager.h"
#include "MetaClassUtil.h"

#include <boost/foreach.hpp>

DEFINE_STATIC_LOGGER("fmwk.bll.base.dm", devLogger);

using namespace std;
using namespace stc::framework;

MetaClassLoadPostProcessor::MetaClassLoadPostProcessor(std::vector<CMetaClass*>& metaClasses,
                                                       std::vector< std::vector< ConstString> >& commandStatusVec) :
    m_metaClasses(metaClasses),
    m_commandStatusVec(commandStatusVec)
{
}

MetaClassLoadPostProcessor::~MetaClassLoadPostProcessor()
{
}

void MetaClassLoadPostProcessor::Process()
{
    DoProcess();
}

StakMetaClassLoadPostProcessor::StakMetaClassLoadPostProcessor(std::vector<CMetaClass*>& metaClasses,
                                                               PropInfoEnumRefMap_t& propEnumMap,
                                                               std::vector< std::vector< ConstString> >& commandStatusVec,
                                                               RelationInfoVec_t& relInfoVec) :
    MetaClassLoadPostProcessor(metaClasses, commandStatusVec),
    m_propEnumMap(propEnumMap),
    m_relInfoVec(relInfoVec)
{
}

StakMetaClassLoadPostProcessor::~StakMetaClassLoadPostProcessor()
{
}

///////////////////////////////////////////////////////////////////////////////
/// PROTECTED AREA
//////////////////////////////////////////////////////////////////////////////

void MetaClassLoadPostProcessor::DoProcess()
{
    CMetaClassManager& m = CMetaClassManager::Instance();

    BOOST_FOREACH(CMetaClass* cls, m_metaClasses)
    {
        CMetaClass* parent = m.GetClass(cls->GetMetaInfo().parentClassId);
        if (parent == NULL && cls->GetMetaInfo().parentClassId != NULL_CLASS_ID())
        {
            throw CStcRunTimeError("invalid parent class id");
        }
        cls->SetParent(parent);
    }

    BOOST_FOREACH(CMetaClass* cls, m_metaClasses)
    {
        cls->SetArchetypeDataStore(new CowDataStore(*cls));// do not delete
        DataStore* store = cls->GetArchetypeDataStore();

        for (CMetaPropertyIterator it = cls->PropertyBegin(); it != cls->PropertyEnd(); ++it)
        {
            const CMetaProperty& mprop = *it;
            if (mprop.GetMetaInfo().id.GetClassId() == cls->GetMetaInfo().id)
            {
                try
                {
                    Property* prop = MakeProperty(mprop);
                    store->AddProperty(std::auto_ptr<Property>(prop));
                }
                catch (const CStcRunTimeError& err)
                {
                    LOG_ERROR(devLogger(),
                              "Error processing property " << mprop.GetMetaInfo().id.ToString()
                              << ": " << err.GetUserMessage());
                }
            }
        }
    }

    std::set<const CMetaClass*> unprocessed;
    unprocessed.insert(m_metaClasses.begin(), m_metaClasses.end());
    BOOST_FOREACH(CMetaClass* cls, m_metaClasses)
    {
        MergeDataStore(cls, unprocessed);
    }

    BOOST_FOREACH(CMetaClass* cls, m_metaClasses)
    {
        cls->InitRelationInfo();
    }

    BOOST_FOREACH(const std::vector<ConstString>& status, m_commandStatusVec)
    {
        bool s;
        CCommandStatusManager::Instance().RegisterCommand(
            MetaClassUtil::GetClassId(status[0]),
            MetaClassUtil::GetClassId(status[1]),
            ConvertToType(s, status[2].c_str()));
    }

    m_commandStatusVec.clear();
}

void StakMetaClassLoadPostProcessor::DoProcess()
{
    vector<CMetaClass*> sortedClasses;
    try
    {
        // There could be STAK user errors. We want to continue processing
        // and load whatever commands are correct.
        MetaClassUtil::SortByInheritanceTree(m_metaClasses, sortedClasses);
    }
    catch(CStcRunTimeError)
    {
        ; // Already logged.
    }

    m_metaClasses.assign(sortedClasses.begin(), sortedClasses.end());

    BOOST_FOREACH(CMetaClass* mc, m_metaClasses)
    {
        const MetaClassInfo& mci = mc->GetMetaInfo();

        try
        {
            TagInfo::const_iterator it = mci.tagInfo.find("baseClass");
            assert(it != mci.tagInfo.end());
            if (it != mci.tagInfo.end())
            {
                const bool isBaseStak = (it->second == "stak.StakCommand" ||
                                         it->second == "stak.StakGroupCommand" ||
										 it->second == "stak.StakWhileCommand" ||
                                         it->second == "stak.DynamicScriptable");
                (const_cast<MetaClassInfo&>(mci)).parentClassId =
                    MetaClassUtil::GetClassId(it->second, !isBaseStak);
            }

            try
            {
                // Detect duplicate classes.
                MetaClassUtil::AddClassId(mci.name, mci.id, true);
            }
            catch(CStcRunTimeError)
            {
                // Already logged.
                continue;
            }

            LOG_INFO(devLogger(), "Registering STAK class: " << mci.name << " with classid " << mci.id <<
                                  " baseClass = " << mci.parentClassId.ToString());
            CMetaClassManager::Instance().Register(mc);
        }
        catch (const std::exception& ex)
        {
            LOG_ERROR(devLogger(), "Error processing class " << mci.name << ": " << ex.what());
        }
        catch (...)
        {
            LOG_ERROR(devLogger(), "Unknown error processing class " << mci.name);
        }
    }

    AssignEnumRefIds();
    CheckPropertyDefaults();

    try
    {
        MetaClassLoadPostProcessor::DoProcess();
    }
    catch (const std::exception& ex)
    {
        LOG_ERROR(devLogger(), "Error post processing STAK MetaClasses: " << ex.what());
    }
    catch (...)
    {
        LOG_ERROR(devLogger(), "Unknown error post processing STAK MetaClasses.");
    }

    ProcessRelations();
}

///////////////////////////////////////////////////////////////////////////////
/// PRIVATE AREA
///////////////////////////////////////////////////////////////////////////////

void StakMetaClassLoadPostProcessor::CheckPropertyDefaults()
{
    BOOST_FOREACH(const CMetaClass* mc, m_metaClasses)
    {
        for (CMetaPropertyIterator pit = mc->PropertyBegin(); pit != mc->PropertyEnd(); ++pit)
        {
            std::string errMsg;
            if(!MetaClassUtil::CheckPropertyDefault(pit->GetMetaInfo(), errMsg))
            {
                LOG_ERROR(devLogger(), "Invalid default for property " << pit->GetMetaInfo().id.ToString() <<
                          " - " << errMsg);
            }
        }
    }
}

void StakMetaClassLoadPostProcessor::AssignEnumRefIds()
{
    BOOST_FOREACH(const PropInfoEnumRefMap_t::value_type& propEnum, m_propEnumMap)
    {
        try
        {
            propEnum.first->enumerationRefClassId = MetaClassUtil::GetClassId(propEnum.second, true);
        }
        catch(const CStcRunTimeError& err)
        {
            LOG_ERROR(devLogger(), "Error processing enum ref " <<
                      propEnum.second << " for property " << propEnum.first->id.ToString() <<
                      " - " << err.GetUserMessage());
        }
    }
}

void StakMetaClassLoadPostProcessor::ProcessRelations()
{
    BOOST_FOREACH(const RelationInfo_t* relInfo, m_relInfoVec)
    {
        MetaRelationInfo* mr = relInfo->mri;

        // TODO Change to DEBUG.
        LOG_INFO(devLogger(), "Registering relation " << mr->name << " type=" << mr->typeName <<
                              " srcClassRef=" << relInfo->srcClass << " dstClassRef=" << relInfo->dstClass);

        try
        {
            CMetaClass* srcClass = CMetaClassManager::Instance().GetClass(relInfo->srcClass);
            if(srcClass == NULL)
            {
                ostringstream msg;
                msg << "Cannot find srcClassRef " << relInfo->srcClass;
                throw CStcRunTimeError(msg.str());
            }

            mr->class1 = srcClass->GetMetaInfo().id;

            CMetaClass* dstClass = CMetaClassManager::Instance().GetClass(relInfo->dstClass);
            if(dstClass == NULL)
            {
                ostringstream msg;
                msg << "Cannot find dstClassRef " << relInfo->dstClass;
                throw CStcRunTimeError(msg.str());
            }

            mr->class2 = dstClass->GetMetaInfo().id;

            CRelationManager::Instance().AddMetaInfo(*mr);
        }
        catch(const CStcRunTimeError& err)
        {
            LOG_ERROR(devLogger(), "Error processing relation "  << mr->name <<
                                   " relation type " << mr->typeName << ": " << err.GetUserMessage());
        }
    }
}

template <class T>
Property* MakeEnumPropertyT(const CMetaProperty& mprop)
{
    const MetaPropertyInfo& mpi = mprop.GetMetaInfo();
    T propDefault = EnumProperty<T>::ConvertToEnumType(mpi, mpi.defaultValue.str());

    if (mpi.IsCollection())
    {
        return new EnumVectorProperty<T>(propDefault, mprop);
    }
    return new EnumProperty<T>(propDefault, mprop);
}

template <class T>
Property* MakePropertyT(const CMetaProperty& mprop)
{
    const MetaPropertyInfo& mpi = mprop.GetMetaInfo();

    if (mpi.enumerationRefClassId != NULL_CLASS_ID())
    {
        return MakeEnumPropertyT<T>(mprop);
    }
    if (!mpi.IsCollection() && mpi.nullable && mpi.defaultValue == "null")
    {
        return new PropertyT<T>(mprop);
    }

    T propDefault;

    // TODO: hack for invalid defaults...
    //       e.g. stc:property name="SrcMacList" type="mac" collectionType="vector" category="config"
    //displayName="Source MAC List" default="0"
    try
    {
        if (mpi.minOccurs > 0)
        {
            ConvertToType(propDefault, mpi.defaultValue.c_str());
        }
    }
    catch (CStcException&)
    {
    }

    if (mpi.IsCollection())
    {
        return new VectorProperty<T>(propDefault, mprop);
    }
    return new PropertyT<T>(propDefault, mprop);
}

Property* MetaClassLoadPostProcessor::MakeProperty(const CMetaProperty& mprop)
{
    const MetaPropertyInfo& mpi = mprop.GetMetaInfo();
    switch (mpi.type)
    {
    case MetaPropertyInfo::BYTE:             return MakePropertyT<int8_t>(mprop);
    case MetaPropertyInfo::SHORT:            return MakePropertyT<int16_t>(mprop);
    case MetaPropertyInfo::LONG:             return MakePropertyT<int32_t>(mprop);
    case MetaPropertyInfo::S64:              return MakePropertyT<int64_t>(mprop);
    case MetaPropertyInfo::UBYTE:            return MakePropertyT<uint8_t>(mprop);
    case MetaPropertyInfo::USHORT:           return MakePropertyT<uint16_t>(mprop);
    case MetaPropertyInfo::ULONG:            return MakePropertyT<uint32_t>(mprop);
    case MetaPropertyInfo::U64:              return MakePropertyT<uint64_t>(mprop);
    case MetaPropertyInfo::BOOL:             return MakePropertyT<bool>(mprop);
    case MetaPropertyInfo::DOUBLE:           return MakePropertyT<double>(mprop);
    case MetaPropertyInfo::STRING:           return MakePropertyT<std::string>(mprop);
    case MetaPropertyInfo::CLASSID:          return MakePropertyT<ClassId>(mprop);
    case MetaPropertyInfo::PROPID:           return MakePropertyT<PropId>(mprop);
    case MetaPropertyInfo::RELATIONID:       return MakePropertyT<RelationId>(mprop);
    case MetaPropertyInfo::HANDLE:           return MakePropertyT<StcHandle>(mprop);
    case MetaPropertyInfo::CSP:              return MakePropertyT<CChassisSlotPort>(mprop);
    case MetaPropertyInfo::MAC:              return MakePropertyT<CMacAddress>(mprop);
    case MetaPropertyInfo::IP:               return MakePropertyT<CIpAddress>(mprop);
    case MetaPropertyInfo::IPV6:             return MakePropertyT<CIpv6Address>(mprop);
    case MetaPropertyInfo::CHARTPOINT:       return MakePropertyT<ChartPoint>(mprop);
    case MetaPropertyInfo::EVENTMESSAGE:     return MakePropertyT<EventMessage>(mprop);
    case MetaPropertyInfo::INPUT_FILE_PATH:  return MakePropertyT<InputFilePath>(mprop);
    case MetaPropertyInfo::OUTPUT_FILE_PATH: return MakePropertyT<OutputFilePath>(mprop);
    case MetaPropertyInfo::WWN:              return MakePropertyT<CWorldWideName>(mprop);
    default:
        throw CStcRunTimeError("unsupported property type: " + mpi.type);
    }
}

void MetaClassLoadPostProcessor::MergeDataStore(const CMetaClass* cls,
                                                    std::set<const CMetaClass*>& unprocessed)
{
    if (unprocessed.find(cls) == unprocessed.end())
    {
        return;
    }

    const CMetaClass* pntCls = cls->GetParent();
    if (pntCls)
    {
        MergeDataStore(pntCls, unprocessed);

        DataStore* store = cls->GetArchetypeDataStore();
        store->Merge(*(pntCls->GetArchetypeDataStore()));
        unprocessed.erase(cls);
    }
}

template <> Property* MakeEnumPropertyT<bool>(const CMetaProperty& mprop) { throw CStcLogicError(); }
template <> Property* MakeEnumPropertyT<double>(const CMetaProperty& mprop) { throw CStcLogicError(); }
template <> Property* MakeEnumPropertyT<std::string>(const CMetaProperty& mprop) { throw CStcLogicError(); }
template <> Property* MakeEnumPropertyT<CChassisSlotPort>(const CMetaProperty& mprop) { throw CStcLogicError(); }
template <> Property* MakeEnumPropertyT<CIpAddress>(const CMetaProperty& mprop) { throw CStcLogicError(); }
template <> Property* MakeEnumPropertyT<CIpv6Address>(const CMetaProperty& mprop) { throw CStcLogicError(); }
template <> Property* MakeEnumPropertyT<CMacAddress>(const CMetaProperty& mprop) { throw CStcLogicError(); }
template <> Property* MakeEnumPropertyT<ChartPoint>(const CMetaProperty& mprop) { throw CStcLogicError(); }
template <> Property* MakeEnumPropertyT<EventMessage>(const CMetaProperty& mprop) { throw CStcLogicError(); }
template <> Property* MakeEnumPropertyT<InputFilePath>(const CMetaProperty& mprop) { throw CStcLogicError(); }
template <> Property* MakeEnumPropertyT<OutputFilePath>(const CMetaProperty& mprop) { throw CStcLogicError(); }
template <> Property* MakeEnumPropertyT<CWorldWideName>(const CMetaProperty& mprop) { throw CStcLogicError(); }
template <> Property* MakeEnumPropertyT<ClassId>(const CMetaProperty& mprop) { throw CStcLogicError(); }
template <> Property* MakeEnumPropertyT<PropId>(const CMetaProperty& mprop) { throw CStcLogicError(); }

