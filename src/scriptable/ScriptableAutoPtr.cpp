#include "StdAfx.h"

#include "ScriptableAutoPtr.h"
#include "Command.h"

#include <cassert>


namespace stc {
namespace framework {

void ReleaseScriptable(CScriptable* s)
{
	assert(s);

	// delete scoped ptr to command right away
	if (dynamic_cast<CCommand*>(s))
	{
		if (!s->IsDeleted())
		{
			s->MarkDelete(true, false);
			s->Delete();
		}
	}
	else
	{
		s->MarkDelete();
	}
}

}
}

