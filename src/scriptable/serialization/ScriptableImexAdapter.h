#ifndef __SCRIPTABLEIMEXADAPTER_H__
#define __SCRIPTABLEIMEXADAPTER_H__

#include <vector>

#include "Scriptable.h"
#include "ImportableExportable.h"

using namespace std;

using namespace stc::framework;

namespace stc {
namespace framework {

/**
 * A ScriptableImexAdapter presents an Imex interface for a Scriptable object
 *
 * @author <a href="mailto:Andrew.Oliver@spirentcom.com">Andy Oliver</a>
 */
class ScriptableImexAdapter : public ImportableExportable {
  public:
    
	static  ScriptableImexAdapter* Create(CScriptable* pscriptable);
	static  ScriptableImexAdapter* Create(CScriptable* pscriptable, bool skipNoSaveCheck);
	static void ClearAllAdapter();
    static void EnableClearAllAdapter(bool enable=true);
   	
	// TODO: we need to make this private and fix the leak from save
	ScriptableImexAdapter():m_skipNoSaveAttribute(false), pclazz(NULL), pscriptable(NULL) { };    
    ScriptableImexAdapter(CScriptable* pscriptable);

    virtual ~ScriptableImexAdapter();

    /** Return the string name of the clas to which this Exportable belongs */
    virtual std::string getType();
    /** Return a unique ID for this Exportable */
    virtual std::string getId();
    /** 
     * Return the ordered collection of property identifiers present
     * in this Exportable
     */
    virtual vector<int> getPropertyIds();
    /** Return the string name of the property with the given ID */
    virtual std::string getPropertyNameForId(int id);
    /** Return the string value of the property with the given ID */
    virtual std::string getPropertyValueForId(int id);
    /** Return the string type name of the property with the given ID */
    virtual std::string getPropertyTypeNameForId(int id);
    /** Determine if a property is visible or hidden */
    virtual bool isPropertyVisible(int id);
    /// \brief Indicates whether a property is read-only or not
    virtual bool isPropertyReadOnly(int id);
    /// \brief Indicates whether a property is read-only or not
    virtual bool isPropertyDeprecated(int id);
    /// \brief Indicates whether a property is a collection
    virtual bool isPropertyCollection(int id);
    /** Return the parent of this object, or NULL if it is an orphan */
    virtual Exportable* getParent();
    /** Return an ordered collection of children of this object */
    virtual vector<Exportable*> getChildren();
    /** 
     * Return an ordered collection containing all of the relationship
     * types in which this Exportable participates. */
    virtual set<int> getRelationshipTypes();
    /** 
     * Returns the string for an integer Relation type id. */
    virtual string getRelationshipName(int id);
    /** 
     * Returns the integer for a string Relation type. */
    virtual int getRelationshipId(bool bIsSource, const string& name);
    /** 
     * Return an ordered collection of related items for the given
     * relationship type where this item is the source if bSource is
     * true, the target otherwise. */
    virtual vector<Exportable*> getRelatives(int relationship, bool bSource);
    /**
     * Returns true if this Exportable has any non-parent-child
     * relationships, false otherwise.  
     */
    virtual bool hasNonParentChildNonDefaultRelationships();

    /** Get a property ID in a ScriptableObject by its string name */
    virtual int getPropertyIdForName(string strPropName);

    /** Set a property value in a ScriptableObject using a property ID */
    virtual void setPropertyValue(int iPropertyId, string strValue);

    /** Add an object underneath this object */
    virtual void addChild(ImportableExportable* pimexB);

    /** 
     * Add a relationship between this object and another in which
     * this object is the source */
    virtual void addRelationship(ImportableExportable* pimexTarget, int iRelationship);

    virtual CScriptable* getScriptable();

    virtual bool isSuitableForExport();

    /// \brief Indicates whether this Exportable is created manually or automatically
    virtual bool isManuallyCreated(int positionInParent);

    /// \brief Indicates whether this Exportable must destroy any siblings.
    virtual bool isFratricidal();

    virtual void yield();

	/// \brief Indicates the object's attributes have been changed.
	virtual bool isOnePropertyVisible();  

	/// \brief Get skip no save flag check
	bool GetSkipNoSaveAttributeFlag() const { return m_skipNoSaveAttribute; }

	/// \brief Get skip no save flag check
	void SetSkipNoSaveAttributeFlag(bool flag) { m_skipNoSaveAttribute = flag; }

  protected:	
    CScriptable* pscriptable;
    CMetaClass* pclazz;

 private:
	typedef std::map<StcHandle, ScriptableImexAdapter*> ScriptableMap;
	typedef std::map<CThread::ID, ScriptableMap> PerThreadScriptableMap;
	static PerThreadScriptableMap s_perThreadScriptableMap;    
    static bool s_clearAllAdapterEnabled;
	bool m_skipNoSaveAttribute;
};

}
}

#endif

