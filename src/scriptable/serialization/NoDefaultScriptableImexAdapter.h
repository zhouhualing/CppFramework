#ifndef __NODEFAULTSCRIPTABLEIMEXADAPTER_H__
#define __NODEFAULTSCRIPTABLEIMEXADAPTER_H__

#include <ostream>
#include <string>
#include <sstream>

#include "Scriptable.h"
#include "ScriptableImexAdapter.h"

using namespace std;

using namespace stc::framework;

namespace stc {
namespace framework {
/**
 * NoDefaultScriptableImexAdapter is an derive class from ScriptableImexAdapaters, 
 * it will handle the no default values for the GUI to script.
 *
 * @author <a href="mailto:shujun.hou@spirentcom.com">Shujun Hou</a>
 */
class NoDefaultScriptableImexAdapter : public ScriptableImexAdapter
{
  public:
	NoDefaultScriptableImexAdapter(CScriptable* pscriptable);
	static NoDefaultScriptableImexAdapter* Create(CScriptable* pscriptable, bool skipNoSaveCheck);
    	static void ClearAllAdapter();
	virtual ~NoDefaultScriptableImexAdapter();
    /** Determine if a property is visible or hidden */
    virtual bool isPropertyVisible(int id);
    /** Return the parent of this object, or NULL if it is an orphan */
    virtual Exportable* getParent();
    /** Return an ordered collection of children of this object */
    virtual vector<Exportable*> getChildren();
    /** 
     * Return an ordered collection of related items for the given
     * relationship type where this item is the source if bSource is
     * true, the target otherwise. */
    virtual vector<Exportable*> getRelatives(int relationship, bool bSource);
    /** 
     * Add a relationship between this object and another in which
     * this object is the source */
    virtual void addRelationship(ImportableExportable* pimexTarget, int iRelationship);

private:
	bool isPropertyDefaultValueChanged(int id);
	bool isDefaultName(const string& expectedFirstPart, const string& name);

	typedef std::map<StcHandle, ScriptableImexAdapter*> noDefaultScriptableMap;
	typedef std::map<CThread::ID, noDefaultScriptableMap> noDefaultPerThreadScriptableMap;
	static noDefaultPerThreadScriptableMap s_noDefaultPerThreadScriptableMap;

  
};

}
}

#endif

