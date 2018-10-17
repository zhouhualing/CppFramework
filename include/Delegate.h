#ifndef INCLUDED_DELEGATE_H
#define INCLUDED_DELEGATE_H

#include "FastDelegate.h"

#include <set>
#include <algorithm>

namespace stc {
namespace framework {

	using namespace fastdelegate;

///
/// Provides Delegate like construct with functors.
/// Supported functors are static functions and member functions.
/// Currently only upto 8 arguments are supported
/// 
/// Example:
///   
///   void callback1(int a, char b);
///   void A::onSomethingWeird(int spooky, char action);
///
///   typedef Delegate< FastDelegate2<int, char> >MyDelegate;
///   MyDelegate myDelegate;
///
///   A cls;
///
///   // registers delegate
///   myDelegate += MakeDelegate(&callback1);
///   myDelegate += MakeDelegate(cls, &cls::onSomethingWeird);
///
///   ...
///   int i = 29;
///   char c = 'a';
///   myDelegate(i, c); // Both callback1 and cls::onSomethingWeird
///                     // will be invoked.
///
///   // unregisters delegate
///   myDelegate -= MakeDelegate(&callback1);
///
///   myDelegate(i, c); // This time only cls::onSomethingWeird will be called.
///
///   Note: MakeDelegate won't be able to deduce const param; 
///   you will have to specialize the template in those cases
///   (e.g. MakeDelegate<cls, const type, ...> () );
///

	template <typename _FastDelegate>
	class Delegate : public std::set< _FastDelegate >
	{
	public:

		typedef _FastDelegate FastDelegate;
		typedef typename FastDelegate::Param1Type Param1;
		typedef typename FastDelegate::Param2Type Param2;
		typedef typename FastDelegate::Param3Type Param3;
		typedef typename FastDelegate::Param4Type Param4;
		typedef typename FastDelegate::Param5Type Param5;
		typedef typename FastDelegate::Param6Type Param6;
		typedef typename FastDelegate::Param7Type Param7;
		typedef typename FastDelegate::Param8Type Param8;

		Delegate() {}
		Delegate(const FastDelegate& fd) 
		{
			this->insert(fd);
		}

		~Delegate() {}

		Delegate& operator += (const FastDelegate& fd)
		{
			this->insert(fd);
			return *this;
		}

		Delegate& operator -= (const FastDelegate& fd)
		{
			this->erase(fd);
			return *this;
		}

		Delegate& operator += (const Delegate& d)
		{
			for (typename Delegate::const_iterator it = d.begin(); it != d.end(); ++it)
			{
				this->insert(*it);
			}

			return *this;
		}

		Delegate& operator -= (const Delegate& d)
		{
			for (typename Delegate::const_iterator it = d.begin(); it != d.end(); ++it)
			{
				this->erase(*it);
			}

			return *this;
		}

		void operator() () const
		{
			Delegate d(*this);
			for (typename Delegate::const_iterator it = d.begin(); it != d.end(); ++it)
			{
				(*it)();
			}
		}

		void operator() (Param1 p1) const
		{
			Delegate d(*this);
			for (typename Delegate::const_iterator it = d.begin(); it != d.end(); ++it)
			{
				(*it)(p1);
			}
		}

		void operator() (Param1 p1, Param2 p2) const
		{
			Delegate d(*this);
			for (typename Delegate::const_iterator it = d.begin(); it != d.end(); ++it)
			{
				(*it)(p1, p2);
			}
		}

		void operator() (Param1 p1, Param2 p2, Param3 p3) const
		{
			Delegate d(*this);
			for (typename Delegate::const_iterator it = d.begin(); it != d.end(); ++it)
			{
				(*it)(p1, p2, p3);
			}
		}

		void operator() (Param1 p1, Param2 p2, Param3 p3, Param4 p4) const
		{
			Delegate d(*this);
			for (typename Delegate::const_iterator it = d.begin(); it != d.end(); ++it)
			{
				(*it)(p1, p2, p3, p4);
			}
		}

		void operator() (Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) const
		{
			Delegate d(*this);
			for (typename Delegate::const_iterator it = d.begin(); it != d.end(); ++it)
			{
				(*it)(p1, p2, p3, p4, p5);
			}
		}

		void operator() (Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) const
		{
			Delegate d(*this);
			for (typename Delegate::const_iterator it = d.begin(); it != d.end(); ++it)
			{
				(*it)(p1, p2, p3, p4, p5, p6);
			}
		}

		void operator() (Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7) const
		{
			Delegate d(*this);
			for (typename Delegate::const_iterator it = d.begin(); it != d.end(); ++it)
			{
				(*it)(p1, p2, p3, p4, p5, p6, p7);
			}
		}

		void operator() (Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8) const
		{
			Delegate d(*this);
			for (typename Delegate::const_iterator it = d.begin(); it != d.end(); ++it)
			{
				(*it)(p1, p2, p3, p4, p5, p6, p7, p8);
			}
		}
	};
}
}

#endif // INCLUDED_DELEGATE_H

