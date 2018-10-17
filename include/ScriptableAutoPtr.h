#ifndef SCRIPTABLE_AUTO_PTR_H
#define SCRIPTABLE_AUTO_PTR_H

#include "StcExport.h"

namespace stc {
namespace framework {


/**
	Auto Pointer for Scriptable
 */
// TODO: Refactor this to be policy based if we need this for something else
template <class Y> 
struct CScriptableAutoPtrRef {
	CScriptableAutoPtrRef(Y* y) : p(y) {}
	Y* p;
}; 

class CScriptable;

STC_PUBLIC void ReleaseScriptable(CScriptable* s);

template <class X = stc::framework::CScriptable> 
class CScriptableAutoPtr {
public: 
	typedef X element_type; 

	// construct/copy/destroy   
	explicit CScriptableAutoPtr (X* p = 0) throw() : m_ptr(p) {}

	CScriptableAutoPtr(CScriptableAutoPtr<X>& ap) throw() : m_ptr(ap.release()) {} 

	template <class Y> 
	CScriptableAutoPtr(CScriptableAutoPtr<Y>& ap) throw() : m_ptr(ap.release()) {}

	CScriptableAutoPtr<X>& operator=(CScriptableAutoPtr<X>& ap) throw() 
	{
		reset(ap.release());
		return *this;
	}

	template <class Y> 
	CScriptableAutoPtr<X>& operator= (CScriptableAutoPtr<Y>& ap) throw()
	{
		reset(ap.release());
		return *this;
	}

	~CScriptableAutoPtr() throw()
	{
		reset();
	}

	// members 
	X& operator* () const throw()
	{
		return *m_ptr;
	}

	X* operator-> () const throw()
	{
		return m_ptr;
	}

	X* get () const throw()
	{
		return m_ptr;
	}

	X* release() throw()
	{
		X* tmp = m_ptr;
		m_ptr = 0;
		return tmp;
	}

	void reset(X* p = 0) throw()
	{
		if (m_ptr && p != m_ptr)
			ReleaseScriptable(m_ptr);

		m_ptr = p;
	}

	// conversions
	CScriptableAutoPtr(CScriptableAutoPtrRef<X> rp) throw() : m_ptr(rp.p) {}

	template <class Y> 
	operator CScriptableAutoPtrRef<Y>() throw()
	{
		if (m_ptr)
		{
			Y* y = dynamic_cast<Y*>(release());
			assert(y && "Invalid downcast");
			return CScriptableAutoPtrRef<Y>(y);
		}
		else
		{
			return CScriptableAutoPtrRef<Y>(0);
		}
	}

	operator CScriptableAutoPtrRef<X>() throw()
	{
		return CScriptableAutoPtrRef<X>(release());
	}

	template <class Y> 
	operator CScriptableAutoPtr<Y>() throw()
	{
		return CScriptableAutoPtr<Y>(*this);
	}

private:
	X* m_ptr;
};

}
}

#endif // SCRIPTABLE_AUTO_PTR_H

