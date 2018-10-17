#ifndef __ScriptExporter_H__
#define __ScriptExporter_H__

#include <list>
#include <map>
#include <ostream>
#include <string>
#include <deque>

#include "AbstractRecursiveFileExporter.h"
#include "StcExport.h"
#include "TaskManager.h"
#include "TclCodeGen.h"
#include "ScriptExporterOptions.h"

using namespace std;

namespace stc {
namespace framework {

/**
 * Generic script exporter.
 */
class ScriptExporter : public AbstractRecursiveFileExporter
{
  public:

    /// \brief Open the named file for Tcl export
    STC_PUBLIC ScriptExporter(string strFilename);

    /// \brief Use the provided stream for Tcl export
    ScriptExporter(ostream* posOutputDestination);

    STC_PUBLIC virtual ~ScriptExporter();
            
    /// \brief Return Tcl code to get the handle to the specified object using DAN and relations, not handles
    STC_PUBLIC void createCodeToGetHandleForObject(Exportable& exp, string& strCode);
        
    void setOptions(const ExporterOptions& options) 
    { 
        this->options = options;
        initScriptCodeGen();
    }

    ExporterOptions getOptions() const { return this->options; }   
    
    static bool STC_PUBLIC ConfigHasExposedProperties();

  protected:
    list<Exportable*> listResultQueryForRDS; ///< The ResultQuery objects seen while exporting
    list<Exportable*> listResultQueryForCharts;       

    /** List of locations of ports used in the script */
    list<string> listPortLocations;    

    /** Holds the objects for previously-seen IDs */
    map<string, Exportable*> mapIdToExportable;

    string strLogicFilename;    
    ExporterOptions options;
    ostream* poutLauncher; ///< The stream for the launcher script (pout has the logic/proc script)

    /// \brief Open the file to hold the logic script
    void openLogic();

    virtual void saveSpecialCaseObjects(Exportable& exp);

    /// \brief Adds the date/time, filename, and more to the top
    virtual void writeIntroComment();

    /// \brief Writes out the initialization tcl logic
    virtual void writeInitCode();

    /// \brief Writes out the connect and reserve tcl logic (not handled by apply)
    virtual void writeConnectReserveCode();

    /** Write out the initialization and logging messages */
    virtual void writeHeader(Exportable& exp);
    virtual void startObject(Exportable& exp, Exportable& expParent, const string& strPrefix);
    virtual void finishObjectProperties(Exportable& exp, const string& strPrefix);
    virtual void finishObject(Exportable& exp, const string& strPrefix);
    virtual void writeSerializationBaseProperty();
    virtual void startProperty(const std::string& strName,
                               const std::string& strValue,
                               const string& strPrefix);
    virtual void finishProperty(const string& strPrefix);
    virtual string getSpecialPropertyValue(Exportable& exp,int propId);    
    virtual void handleRelationship(Exportable& exp, 
                                    const string& strType,
                                    bool bSourceToTarget,
                                    bool last,
                                    const string& strPrefix);    
    virtual void writeTrailer();
    virtual void writeLauncherScript();

    virtual void closeOutput();

    /// \brief Writes out ports and all ancestors under the given Exportable, assuming P2 data model.
	void writePortsAndAncestors(Exportable& exp, Exportable& expParent, std::string& strPrefix, int depth);
        
    /// \brief Return the Tcl variable name to be used for an object
    virtual string getNameForObject(Exportable& exp);     

    /**
       \brief Lookup the numeric ID for a property.
       
       @return The property ID, or -1 if none is found
    */
    virtual int getPropertyIdForName(Exportable& exp, 
                                     const string strPropertyName);

    /// \brief Compute and return the index of the provided object among its siblings of the same type
    int getPositionInParent(Exportable& exp);
    
    /// \brief Return Is file path replacement enabled for inputFilePath/outputFilePath types.
    bool isFilePathReplacementEnabled()
    {
        return !options.pathReplacements.empty();
    }

    // \brief Write the file path substitution code for the Exportable if file path mapping is enabled.
    // called from startObject
    virtual void writeFilePathSubstitution(Exportable& exp); 

    /// brief Saves ResultQuery objects for later special-purpose handling, but
    //        only if the result set is subscribed.
    //         Called from startObject.
    virtual void saveResultQuery(Exportable& exp, Exportable& expParent);
        
    /// \brief Is the given Exportable (ResultDataSet or ResultChartQuerySet) subsribed?
    bool isResultSetSubscribed(Exportable& exp);

    /// \brief Returns whether the port is a virtual (e.g. aggregator) port.
    bool isVirtualPort(Exportable &exp);

    IScriptCodeGen* mScriptGen;

 private:
    /// \brief Return Tcl DAN code snippet to point to the specified object
    void createDanForObject(Exportable& exp, string& strCode);

    /// \brief Return cvs file name for subscribe
	string getCsvFileName(string resultTypeStr, int iterCount); 
            
    // \brief Determine if the exp object is manually/auto created
    bool isManuallyCreated(Exportable& exp);
            
    /// \brief Determine if the given Exportable can be removed from code generation.
    bool canRemove(Exportable& exp);

    /// \brief Returns whether or not the given Exportable is a child of a default host.
    bool isChildOfDefaultHost(Exportable& exp);
    
    /// \brief Was the given Exportable removed?
    bool wasRemoved(Exportable& exp);
    
    void writeHandleTranslation();

    // \brief Replace colons with safe characters in type names
    string sanitizeType(string strType);

    /// \brief Perform all escaping needed to make an attribute Tcl-safe
    string sanitizeString(string strUnclean);

    /** Holds the largest numeric ID used for each type in the key */
    map<string, int> mapTypeToLargestId;
    
    /** Holds the names for previously-seen objects */
    map<string, string> mapExportableToName;

    /** The current object being handled */
    Exportable* pexpCurrent;

    list<string> listRelationshipsOrdered;
   
    /** Map of relationship types to list of handles */
    map<string, list<string> > mapRelationships;

	/** Holds handle properties for each object */
	map<Exportable*, deque< pair<int, string> > > mapHandleProps;

    /** The zero-based index of the current port in the listPortLocations list.  Note that virtual ports are skipped. */
    int iiPortLocationCurrent;

    /** list of ids of objects that were removed from code
     *  generation */
    std::vector<std::string> removedIds;

    /** list of Exportable ids whose handles have already been
     *  created during the relationship/handle lookups. */
    std::vector<std::string> handlesCreatedIds;

    /** List of types that are potential candidates for removal
     *  during a compact ( lossy ) save. */
    std::vector<std::string> removalCandidates;

    /**
     * Cache of auto-created Exportable ids. Means we generate
     * stc::get instead of stc::create.
    */
    std::vector<std::string> autoCreatedIds;

    /// \brief Initializes the proper code generator for the language.
    void initScriptCodeGen();  

    /// \brief Checks to see if the given Exportable is one of the types in the removal candidates list.
    bool isCandidateForRemoval(Exportable& exp);

    /// \brief Returns true if the given Exportable is the source or target of a side relation.
    bool isSourceOrDestForRelationship(Exportable& exp);

    /// \brief Returns code for getting handles all the way up the tree for removed Exportables    
    string createHandleGetCodeForRemovedExportable(Exportable& exp);
};

}
}

#endif

