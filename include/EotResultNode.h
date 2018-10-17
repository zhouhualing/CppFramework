#ifndef INCLUDED_FRAMEWORK_EOTRESULTNODE_H
#define INCLUDED_FRAMEWORK_EOTRESULTNODE_H

#include "Scriptable.h"

namespace stc {
namespace framework {

class STC_PUBLIC CEotResultNode : public CScriptable {

public:

	/// \brief Sets the value result node parameter; 
	///        creates a new parameter if an existing one is not found.
	void SetResultNodeParameter(const std::string& name, const std::string& value);

	/// \brief Sets the value result node parameter (template version). 
	template <typename T>
	void SetResultNodeParameter(const std::string& name, const T& value)
	{
		std::ostringstream os;
		os << value;
		SetResultNodeParameter(name, os.str());
	}

	void SetCanSave(bool canSave) { m_canSave = canSave; }
	bool GetCanSave() const { return m_canSave; }

	virtual bool CanSave() const { return m_canSave; }


private:
	bool m_canSave;

protected:
	// Construct through ScriptableCreator instead.
	CEotResultNode();
	CEotResultNode(const CEotResultNode&);
	CEotResultNode& operator = (const CEotResultNode&);
	virtual ~CEotResultNode();

private:
	friend class CEotResultNodeCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "EotResultNode_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_EOTRESULTNODE_H

