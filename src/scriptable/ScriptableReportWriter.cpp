#include "StdAfx.h"
#include "ScriptableReportWriter.h"
#include "Scriptable.h"
#include "Timer.h"
#include "StcSystem.h"

#include <fstream>

#include <boost/foreach.hpp>

using namespace stc::framework;
using namespace std;

CScriptableReportWriter::CScriptableReportWriter(const std::string& reportName,
                                                 const std::string& filename,
                                                 const std::vector<Page*>& pages,
                                                 const std::set<PropId>& excludedProperties,
                                                 std::auto_ptr<AbstractScriptableReportFormatter> formatter,
                                                 const std::set<stc::framework::PropId>& includedProperties) :
    _reportName(reportName),
    _filename(filename),
    _pages(pages),
    _excludedProperties(excludedProperties),
    _formatter(formatter),
    _includedProperties(includedProperties)
{

}

CScriptableReportWriter::~CScriptableReportWriter()
{

}
    
void CScriptableReportWriter::Write() const
{
    ofstream ostream(_filename.c_str(), ios_base::trunc | ios_base::out);
    if(!ostream.is_open())
    {        
        throw CStcRunTimeError(string("ScriptableReportWriter could not open file ") + _filename);
    }

    DoWrite(ostream);
    ostream.close();
}

///////////////////////////////////////////////////////////////////////////////
// PRIVATE AREA
///////////////////////////////////////////////////////////////////////////////

string GetPropertyDisplayName(const MetaPropertyInfo& mpi)
{
    string propName = mpi.name;
    TagInfo::const_iterator tit = mpi.tagInfo.find("displayName");
    if(tit != mpi.tagInfo.end() && tit->second.empty() == false)
    {
        propName = tit->second;
    }

    return propName;
}

void CScriptableReportWriter::DoWrite(std::ostream& os) const
{
    os << _formatter->FormatHeader(_reportName);

    BOOST_FOREACH(Page* p, _pages)
    {
        os << _formatter->FormatPageName(p->_pageName);

        if(p->_fkeys.empty() == false && p->_vec.empty() == false && 
           p->_fkeys.size() != p->_vec.size())
        {
            throw CStcInvalidArgument("vec and fkeys size must be the same.");
        }

        int i = 0;
        BOOST_FOREACH(CScriptable* s, p->_vec)
        {           
            AbstractScriptableReportFormatter::PropertyNameValPairs_t props;
            GetPropertyNameVals(*s, props);
            string fkeyHeader;
            string fkeyVal;
            if(!p->_fkeys.empty())
            {
                const CScriptableReportWriter::Page::ForeignKey_t& fkey = p->_fkeys[i];
                const CScriptable* sfkey = fkey.first;
                const PropId propId = fkey.second;
                const MetaPropertyInfo& mpi = propId.GetMetaProperty()->GetMetaInfo();
                fkeyHeader = GetPropertyDisplayName(mpi);
                sfkey->Get(mpi.id, fkeyVal);
            }

            // Process composites, if any.
            AbstractScriptableReportFormatter::CompositePropertyPairs_t compPairs;
            CScriptableReportWriter::Page::CompositeMap_t::iterator cit = p->_compMap.find(s);
            if(cit != p->_compMap.end())
            {
                ScriptableVec& compVec = cit->second;
                BOOST_FOREACH(CScriptable* comp, compVec)
                {
                    AbstractScriptableReportFormatter::PropertyNameValPairs_t compProps;
                    GetPropertyNameVals(*comp, compProps);
                    compPairs.push_back(std::make_pair(comp, compProps));
                }                                
            }

            os << _formatter->FormatScriptable(*s, props, compPairs, fkeyHeader, fkeyVal);
            i++;
        }
    }
}

void CScriptableReportWriter::GetPropertyNameVals(const stc::framework::CScriptable& s, 
                                                  AbstractScriptableReportFormatter::PropertyNameValPairs_t& props) const
{    
    const CMetaClass& mc = s.GetMetaClass();    
    for(CMetaPropertyIterator it = mc.PropertyBegin(); it != mc.PropertyEnd(); ++it)
    {
        const MetaPropertyInfo& mpi = it->GetMetaInfo();
        if(!mpi.isInternal && !IsFiltered(mpi.id))
        {
            const string propName = GetPropertyDisplayName(mpi);

            string val;
            s.Get(mpi.id, val);
            const bool isEnum = !mpi.enumerationRefName.empty();
        	if (isEnum)
        	{	                
                const MetaEnumerationInfo* mei = CMetaClassManager::Instance().GetEnumeration(mpi);
                uint64_t ival;
                if(mei->GetEnumerationValue(val, ival))
                {
                    val.clear();
                    mei->GetEnumerationDisplayString(ival, val);
                }                
        	}
            
            props.push_back(std::make_pair(propName, val));
        }
    }
}

bool CScriptableReportWriter::IsFiltered(stc::framework::PropId propId) const
{
    if(_includedProperties.empty() == false)
    {
        return ( _includedProperties.find(propId) == _includedProperties.end() );
    }

    return ( _excludedProperties.find(propId) != _excludedProperties.end() );
}


///////////////////////////////////////////////////////////////////////////////
// DefaultCsvScriptableReportFormatter
///////////////////////////////////////////////////////////////////////////////

DefaultCsvScriptableReportFormatter::DefaultCsvScriptableReportFormatter() :
    _lastClassId(NULL_CLASS_ID()) 
{


}

DefaultCsvScriptableReportFormatter::~DefaultCsvScriptableReportFormatter()
{

}

std::string DefaultCsvScriptableReportFormatter::FormatHeader(const std::string& reportName)
{
    ostringstream s;
    s << "# " << reportName << endl;
    s << "# Generated on " << CTimer::GetCurrentSystemDateTime() << endl;    
    s << "# Framework ver. " << CStcSystem::Instance().GetVersion() << endl << endl;
    return s.str();
}

std::string DefaultCsvScriptableReportFormatter::FormatPageName(const std::string& pageName)
{    
    ostringstream s;
    s << endl << "# " << pageName << endl;
    return s.str();
}

string EscapeChars(const string& s)
{
    if (s.find(" ") != string::npos ||
        s.find(",") != string::npos)
    {
        return "\"" + s + "\"";
    }

    return s;
}

std::string DefaultCsvScriptableReportFormatter::FormatScriptable(const stc::framework::CScriptable& s, 
                                                                  const PropertyNameValPairs_t& nameValPairs,
                                                                  const CompositePropertyPairs_t& compPairs,
                                                                  const std::string& foreignKeyHeaderName,
                                                                  const std::string& foreignKeyVal)
{
    const bool needsHeader = _lastClassId != s.GetClassId();
    _lastClassId = s.GetClassId();
    ostringstream header, data;

    // Foreign key if there is one.
    if(foreignKeyHeaderName.empty() == false && needsHeader)
    {
        header << EscapeChars(foreignKeyHeaderName) << ",";
    }

    if(foreignKeyVal.empty() == false)
    {
        data << EscapeChars(foreignKeyVal) << ",";
    }

    if(needsHeader)
    {
        // Main Scriptable headers.
        BOOST_FOREACH(const PropertyNameValPairs_t::value_type& pair, nameValPairs)
        {
            header << EscapeChars(pair.first) << ",";
        }
        
        // Now include each composite Scriptable headers.
        BOOST_FOREACH(const CompositePropertyPairs_t::value_type& compPair, compPairs)
        {
            BOOST_FOREACH(const PropertyNameValPairs_t::value_type& pair, compPair.second)
            {
                header << EscapeChars(pair.first) << ",";
            }
        }

        header << endl;       
    }

    // Main Scriptable values.
    BOOST_FOREACH(const PropertyNameValPairs_t::value_type& pair, nameValPairs)
    {              
        data << EscapeChars(pair.second)  << ",";
    }

    // Include each composite Scriptable values.
    BOOST_FOREACH(const CompositePropertyPairs_t::value_type& compPair, compPairs)
    {
        BOOST_FOREACH(const PropertyNameValPairs_t::value_type& pair, compPair.second)
        {
            data << EscapeChars(pair.second) << ",";
        }
    }
    
    ostringstream rows;
    rows << header.str() << data.str() << endl;
    return rows.str();
}

