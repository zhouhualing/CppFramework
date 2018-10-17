#ifndef INCLUDED_FRAMEWORK_CLIENTINFO_H
#define INCLUDED_FRAMEWORK_CLIENTINFO_H
#pragma once

#include "Result.h"

#include "StcExport.h"

namespace stc {
namespace framework {

class STC_PUBLIC CClientInfo : public CResult {

public:

	void SetCanDelete(bool canDelete) { m_canDelete = canDelete; }
	virtual bool CanDelete() const { return m_canDelete; }

	bool IsInternalClient() const;

	std::string GetClientStringId() const;
	std::string GetClientUserTemplatePath() const;

	StcHandle GenerateHandleBlock(int count);
	void ReleaseUnusedHandleBlocks();

    static bool IsInLoopbackMode();

protected:
	/// \brief Invoked when the object is being initialized.
	virtual bool OnInit();

	/// \brief Invoked when the object is being mark as deleted.
	virtual bool OnMarkDelete();

private:

	bool m_canDelete;
	typedef std::pair<StcHandle, int> HandleBlock;
	typedef std::list<HandleBlock> HandleBlocks;

	HandleBlocks m_handleBlocks;

protected:
	// Construct through ScriptableCreator instead.
	CClientInfo();
	CClientInfo(const CClientInfo&);
	CClientInfo& operator = (const CClientInfo&);
	virtual ~CClientInfo();

private:
	friend class CClientInfoCtorDtorHelper;

// Keep this include as the last line of the class!!!
#include "ClientInfo_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_CLIENTINFO_H

