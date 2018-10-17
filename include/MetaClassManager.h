#ifndef INCLUDED_META_CLASS_MANAGER_H
#define INCLUDED_META_CLASS_MANAGER_H

#ifdef WIN32
#pragma warning(disable : 4786)
#endif

#include "MetaInfo.h"

#include "StcExport.h"

#include <map>
#include <string>

namespace stc {
namespace framework {

class CMetaClass;
class CMetaClassIterator;

/**
    Singleton manager for metaclasses.
 */

class CMetaClassManager {
public:

    /// \brief Returns the CMetaClassManager instance.
    STC_PUBLIC static CMetaClassManager& Instance();

    /// \brief Ctor.
    STC_PUBLIC CMetaClassManager();

    /// \brief Dtor.
    STC_PUBLIC virtual ~CMetaClassManager();

    /// \brief Registers the metaclass with the manager.
    STC_PUBLIC void Register(CMetaClass* pCls);

    /// \brief Registers the metaclass with the manager using the fully-qualified name.
    STC_PUBLIC void Register(CMetaClass* pCls, const std::string& fqClassName);

    /// \brief Unregisters the metaclass from the manager.
    STC_PUBLIC CMetaClass* Unregister(const ClassId& id);

    /// \brief Returns the registered metaclass by class id.
    STC_PUBLIC CMetaClass* GetClass(const ClassId& id) const;

    /// \brief Returns the registered metaclass by class name (case-insensitive).
    //  Note that the parent is also given when it makes a difference --
    //  that should only happen in special cases (i.e. w/ PduScriptables)
    STC_PUBLIC CMetaClass* GetClass(const std::string& name,
                                    const CMetaClass* prntClass = 0,
                                    bool useAltCreator = false) const;

    /// \brief Returns the plugin name registered with a given class
    STC_PUBLIC std::string GetPluginName(const ClassId &id) const;

    /// \brief Returns the registered metaclass by class name (case-insensitive).
    STC_PUBLIC const MetaEnumerationInfo* GetEnumeration(const MetaPropertyInfo& mpi) const;

    /// \brief Returns the number of metaclasses.
    STC_PUBLIC size_t GetMetaClassCount() const;

    /// \brief Returns an metaclass iterator.
    STC_PUBLIC CMetaClassIterator GetMetaClassIterator() const;

    /// \brief Explicitly activate data model for a plugin
    STC_PUBLIC void ActivateDataModel(uint32_t pluginId);

    /// \brief Load Stak Commands
    STC_PUBLIC void LoadStakCommands();

    // Determine if class is a STAK command. Checks STAK, STAKGroup and STAKWhile.
    STC_PUBLIC bool IsStakCommand(const MetaClassInfo& mci) const;

    // Determine if class is a STAK Group command.
    STC_PUBLIC bool IsStakGroupCommand(const MetaClassInfo& mci) const;

	// Determine if class is a STAK While command.
    STC_PUBLIC bool IsStakWhileCommand(const MetaClassInfo& mci) const;

    // Determine if class is a DynamicScriptable.
    STC_PUBLIC bool IsDynamicScriptable(const MetaClassInfo& mci) const;

    // Determine if class is a DynamicScriptable.
    STC_PUBLIC bool IsDynamicScriptable(const std::string& className) const;

    /// \brief Returns a comma separated list of suggestions for the given clsName.
    STC_PUBLIC static std::string GetSuggestions(const std::string& clsName,
                                                 bool prefixErrorMsg = false);

    class AlternateMetaClassCreator
    {
      public:
        AlternateMetaClassCreator() {}
        virtual ~AlternateMetaClassCreator() {}
    	/// \brief Returns the registered class by class name.
    	virtual CMetaClass* GetClass(const std::string& name) const = 0;
    };

    /// \brief Allows non-SCG-generated scriptables to be created via a second mechanism.
	STC_PUBLIC void SetAlternateMetaClassCreator(AlternateMetaClassCreator* altMetaClassCreator)
    {
		mAltMetaClassCreator = altMetaClassCreator;
	}

    friend class stc::framework::CMetaClassIterator;

private:

    typedef std::map<ClassId, CMetaClass*> ClassMap;
    ClassMap mClassMap;

    typedef std::map<const std::string, CMetaClass*> ClassNameMap;
    ClassNameMap mClassNameMap;
    AlternateMetaClassCreator* mAltMetaClassCreator;

    CMetaClassManager(const CMetaClassManager&);
    CMetaClassManager& operator= (const CMetaClassManager&);
};


/**
    Iterates over the metaclasses in the CMetaClassManager.
*/
class CMetaClassIterator {
public:

    /// \brief Ctor.
    STC_PUBLIC CMetaClassIterator();
    /// \brief Dtor.
    STC_PUBLIC virtual ~CMetaClassIterator();

    /// \brief Copy Ctor
    STC_PUBLIC CMetaClassIterator(const CMetaClassIterator&);
    /// \brief Assignment operator.
    STC_PUBLIC CMetaClassIterator operator = (const CMetaClassIterator&);

    /// \brief Returns the next CMetaClasses and advances the iterator.
    STC_PUBLIC CMetaClass* Next();

    /// \brief Returns if there are more CMetaClasses.
    STC_PUBLIC bool HasNext() const;


private:

    friend class stc::framework::CMetaClassManager;

    /// \brief Ctor.
    CMetaClassIterator(
        CMetaClassManager::ClassMap::const_iterator begin,
        CMetaClassManager::ClassMap::const_iterator end);


    CMetaClassManager::ClassMap::const_iterator mIt;
    CMetaClassManager::ClassMap::const_iterator mEndIt;
};


}
}
#endif // INCLUDED_META_CLASS_MANAGER_H
