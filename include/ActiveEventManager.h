#ifndef INCLUDED_FRAMEWORK_ACTIVEEVENTMANAGER_H
#define INCLUDED_FRAMEWORK_ACTIVEEVENTMANAGER_H

#include "Scriptable.h"
#include "ActiveEvent.h"

#include "StcExport.h"

namespace stc {
namespace framework {

class STC_PUBLIC CActiveEventManager : public CScriptable {
public:
	/// \brief Returns the singleton instance.
	static CActiveEventManager& Instance();

	/// \brief Adds a new active event.
	///        Returns true if the event was added, false otherwise.
	bool AddActiveEvent(CActiveEvent* actEvent);	

protected:
	// Construct through ScriptableCreator instead.
	CActiveEventManager();
	CActiveEventManager(const CActiveEventManager&);
	CActiveEventManager& operator = (const CActiveEventManager&);
	virtual ~CActiveEventManager();

private:
	friend class CActiveEventManagerCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "ActiveEventManager_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_ACTIVEEVENTMANAGER_H

