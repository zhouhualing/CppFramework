#ifndef __ABSTRACTRECURSIVEEXPORTER_H__
#define __ABSTRACTRECURSIVEEXPORTER_H__

#include <map>
#include <set>

#include "Exporter.h"
#include "StcExport.h"

namespace stc {
namespace framework {

/**
 * An AbstractRecursiveExporter is a class than can write a ScriptableObject into some
 * format on permanent storage, like XML, HTML, Tcl, an SQL database,
 * etc.
 *
 * Subtypes are expected to have constructors that specify the
 * format-specific destination specification (filename, database
 * connection string, etc.)
 *
 * @author <a href="mailto:Andrew.Oliver@spirentcom.com">Andy Oliver</a>
 */
class AbstractRecursiveExporter : public Exporter {
  public:
    STC_PUBLIC AbstractRecursiveExporter();
    STC_PUBLIC virtual ~AbstractRecursiveExporter();
    
    /**
       Saves the single exportable which should be the root of a
       configuration hierarchy.  This method calls other methods which
       can be overridden and should not be overridden itself
     */
    STC_PUBLIC virtual int save(Exportable& exportableToStore);

    /**
       Saves the collection of nodes that are all descendants of the
       provided root.  This method calls other methods which can be
       overridden and should not be overridden itself
     */
    STC_PUBLIC virtual int save(Exportable& expRoot,
                     set<Exportable*> setExportablesToSave);

    /**
       Saves the provided exportable, skipping trees rooted in objects
       that belong to one of the classes in the provided list.  This
       method calls other methods which can be overridden and should
       not be overridden itself
     */
    STC_PUBLIC virtual int save(Exportable& expRoot,
                     vector<string>& vecClassesToExclude);
  protected:

    /// \brief Returns a set of ancestor ids (parents, grandparents, etc.)
	void getAncestorIdsForObject(Exportable* exp, set<std::string>& setAncestors);

    /// \brief Returns a set of all ancestor ids for a set of objects
	set<std::string> getAncestorIdsForObjects(set<Exportable*> setExportablesToSave);

    /// \brief Configure nested or flat mode (set by subtypes)
    void setNested(bool bNested);

    /// \brief Configure unistable object exclusion mode (set by subtypes)
    void setExcludeUnsuitable(bool bExcludeUnsuitable);

    /// \brief Allows subtypes to save objects early in serialized form
    STC_PUBLIC virtual void saveSpecialCaseObjects(Exportable& exp);

    /// \brief Writes out attributes for one object, no children
    int writeOneObject(Exportable& exp,
                       Exportable& expParent,
                       string& strPrefix,
                       bool bIsSerializationBase);

    /// \brief Allows subtypes to prepare their output destination
    virtual void openOutput() = 0;

    /// \brief Allows subtypes to write out information before the first object
    virtual void writeHeader(Exportable& exp) = 0;

    /// \brief Subtypes should write out the beginning of an object
    virtual void startObject(Exportable& exp, Exportable& expParent, const string& strPrefix) = 0;

    /// \brief Subtypes should write any termination required after properties
    virtual void finishObjectProperties(Exportable& exp, const string& strPrefix) = 0;

    /// \brief Subtypes should write any termination required after an object
    virtual void finishObject(Exportable& exp, const string& strPrefix) = 0;

    /// \brief Subtypes must annotate the current object as a serialization base
    virtual void writeSerializationBaseProperty() = 0;

    /// \brief Subtypes must output the name/value pair
    virtual void startProperty(const string& strName,
                               const string& strValue,
                               const string& strPrefix) = 0;

    /// \brief Subtypes must finish the previously-begun name/value pair
    virtual void finishProperty(const string& strPrefix) = 0;

    /// \brief The default implementation returns the property value from the given exp.
    /// Subtypes may override to override the value and give their own custom value.
    virtual string getSpecialPropertyValue(Exportable& exp, int propId);

    /**
       \brief Subtypes must write out the relationship.

       Export the specified relationship in the current object.  Will
       be called for each relationship where the object is on the
       default side of the relationship.
     
    *  @param exp The object at the other end of the relationship
     
    *  @param strType A string specifying the type fo the relationship
    * 
    *  @param last Indicates the last relationship for the current
    *              object of the specified type.
     
    *  @param bSourceToTarget True indicates that the current object
       is the source in this relationship.  False indicates that the
       current object is the target.
     */
    virtual void handleRelationship(Exportable& exp, 
                                    const string& strType,
                                    bool bSourceToTarget,
                                    bool last, 
                                    const string& strPrefix) = 0;

    /**
       \brief Subtypes must write the closing material to the export target.

       Write the closing material to the export target.  This may
       include any final entries cached from earlier calls or simply
       termination commands or constructs, depending on the export
       format.
     */
    virtual void writeTrailer() = 0;

    /// \brief Subtypes must close/flush the serialized destination
    virtual void closeOutput() = 0;

    /// \brief Adds another level of nesting to the indentation (prefix)
    STC_PUBLIC virtual string incrementPrefix(const string& strPrefix);

    /// \brief Subtypes use it to prevent an object from being re-exported later
    STC_PUBLIC virtual void rememberAlreadyExported(Exportable& exp);

  protected:

    vector<string>* pvecClassesToExclude;

  private:
    int icElementsSeen;

    /// \brief If true, serialized objects are nested, if false, they are flat
    bool bNestedSerialization;
    /// \brief If true, unsuitable objects are left out of serialized files
    bool bExcludeUnsuitable;
    /// \brief If true, class list filter is used to exclude branches
    bool bExcludeByClass;

	map<std::string, std::string> mapAlreadySaved;

    /// \brief Determines whether the branch rooted at the provided node should be saved
    bool includeThisBranch(Exportable& exp);

    /// \brief Saves the object and all of its descendants
    int writeRecursively(Exportable& exp);

    /// \brief Saves the object and all of its descendants
    int writeRecursively(Exportable& exp, Exportable& expParent, std::string& strPrefix,
                         bool bIsSerializationBase);

    /// \brief Saves the objects, their descendants, and their ancestors
    int writeSelectedObjectsRecursively(Exportable& exp,
                                        Exportable& expParent,
										set<std::string> setAncestorIds,
										set<std::string> setSelectedObjectIds,
                                        string& strPrefix);
};

}
}

#endif

