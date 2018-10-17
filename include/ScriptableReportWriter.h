#ifndef __SCRIPTABLE_REPORT_WRITER_H__
#define __SCRIPTABLE_REPORT_WRITER_H__

#include "StcCollections.h"
#include "MetaInfo.h"

#include <set>

class UnitTestScriptableReportWriter;

namespace stc {
namespace framework { 
    class CScriptable;

/// <summary>
/// Base class for formatting Scriptables for ScriptableReportWriter.
/// </summary>
class AbstractScriptableReportFormatter
{
public:
    AbstractScriptableReportFormatter(){}
    virtual ~AbstractScriptableReportFormatter(){}

    /// <summary>
    /// Returns the formatted header from the report name.
    /// </summary>
    /// <param name="reportName"></param>
    /// <returns>std::string</returns>
    virtual std::string FormatHeader(const std::string& reportName) = 0;

    /// <summary>
    /// Returns the formatted page (tab) name.
    /// </summary>
    /// <param name="pageName"></param>
    /// <returns>std::string</returns>
    virtual std::string FormatPageName(const std::string& pageName) = 0;

    typedef std::vector< std::pair< std::string, std::string > > PropertyNameValPairs_t;
    typedef std::vector< std::pair<CScriptable*, PropertyNameValPairs_t > > CompositePropertyPairs_t;

    /// <summary>
    /// Returns the formatted Scriptable and it's property names/values.
    /// </summary>
    /// <param name="s"></param>
    /// <param name="nameValPairs"></param>
    /// <param name="compPairs">May be empty.</param>
    /// <param name="foreignKeyHeaderName">May be empty.</param>
    /// <param name="foreignKeyVal">May be empty.</param>
    /// <returns>std::string</returns>
    virtual std::string FormatScriptable(const stc::framework::CScriptable& s, 
                                         const PropertyNameValPairs_t& nameValPairs,
                                         const CompositePropertyPairs_t& compPairs,
                                         const std::string& foreignKeyHeaderName,
                                         const std::string& foreignKeyVal) = 0;    
};

/// <summary>
/// The default CSV formatter which prints 2 rows for each Scriptable, 
/// a header row with property display name followed by the values row.
/// </summary>
class STC_PUBLIC DefaultCsvScriptableReportFormatter : public AbstractScriptableReportFormatter
{
public:
    DefaultCsvScriptableReportFormatter();
    virtual ~DefaultCsvScriptableReportFormatter();
    virtual std::string FormatHeader(const std::string& reportName);
    virtual std::string FormatPageName(const std::string& pageName);
    virtual std::string FormatScriptable(const stc::framework::CScriptable& s, 
                                         const PropertyNameValPairs_t& nameValPairs,
                                         const CompositePropertyPairs_t& compPairs,
                                         const std::string& foreignKeyHeaderName,
                                         const std::string& foreignKeyVal);    
private:
    stc::framework::ClassId _lastClassId;    
};

/// <summary>
/// A report writer for Scriptables with formatting provider by a AbstractScriptableReportFormatter instance.
/// It handles the concept of "pages", i.e. tabs or tables, multiple Scriptables of different or same types
/// on a page, Scriptables on the same row ( composites ), and also the concept of foreign key properties to refer
/// to Scriptables on a different page.
/// </summary>
class STC_PUBLIC CScriptableReportWriter
{   
public:

    struct Page
    {
        typedef std::pair<CScriptable*, stc::framework::PropId> ForeignKey_t;
        typedef std::map<CScriptable*, stc::framework::ScriptableVec> CompositeMap_t;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="pageName">The name of the page( tab or table )</param>
        /// <param name="vec">The Scriptable objects making up the page.</param>
        /// <param name="compositeMap">The (optional) composite Scriptables. These Scriptables
        ///                            are displayed on the same row as the main ones. The key is the Scriptable from vec.</param>
        /// <param name="fkeys">The (optional) foreign keys to another page.
        ///                    Think of it as foreign key to a database table. If specified, this
        ///                    value will be displayed first in the output.
        ///                    If specified, the size of vec and fkeys must be the same.</param>
        Page(const std::string& pageName, 
             const stc::framework::ScriptableVec& vec, 
             const CompositeMap_t compositeMap = CompositeMap_t() , 
             const std::vector<ForeignKey_t>& fkeys = std::vector<ForeignKey_t>()) :
            _pageName(pageName),
            _vec(vec),
            _compMap(compositeMap),
            _fkeys(fkeys)
        {
        }
        
        std::string _pageName;
        stc::framework::ScriptableVec _vec;
        std::vector<ForeignKey_t> _fkeys;
        CompositeMap_t _compMap;
    };

    /// <summary>
    /// 
    /// </summary>
    /// <param name="reportName">The name of the report.</param>
    /// <param name="filename">The filename to write the report to.</param>
    /// <param name="pages">The pages to format and write.</param>
    /// <param name="excludedProperties">Properties to exclude from the report. Internal properties are automatically filtered.</param>    
    /// <param name="formatter">The formatter.</param>    
    /// <param name="includedProperties">Properties to include in the report. Use in lieu of excludedProperties.
    ///                                  If empty, this parameter is not used.</param>
    CScriptableReportWriter(const std::string& reportName,
                            const std::string& filename,
                            const std::vector<Page*>& pages,
                            const std::set<stc::framework::PropId>& excludedProperties,                            
                            std::auto_ptr<AbstractScriptableReportFormatter> formatter = std::auto_ptr<AbstractScriptableReportFormatter>(
                                new DefaultCsvScriptableReportFormatter),
                            const std::set<stc::framework::PropId>& includedProperties = std::set<stc::framework::PropId>());
                            
    ~CScriptableReportWriter();

    /// <summary>
    /// Write the report.
    /// </summary>
    void Write() const;


private:

    void DoWrite(std::ostream& os) const;

    void GetPropertyNameVals(const stc::framework::CScriptable& s, AbstractScriptableReportFormatter::PropertyNameValPairs_t& props) const;

    bool IsFiltered(stc::framework::PropId propId) const;

    std::string _reportName;
    std::string _filename;
    std::vector<Page*> _pages;
    std::set<stc::framework::PropId> _excludedProperties;
    std::auto_ptr<AbstractScriptableReportFormatter> _formatter; 
    std::set<stc::framework::PropId> _includedProperties;

    friend class ::UnitTestScriptableReportWriter;

};

}
}

#endif /* __SCRIPTABLE_REPORT_WRITER_H__ */

