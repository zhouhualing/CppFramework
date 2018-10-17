#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "Delegate.h"
#include "StcTypes.h"

#include <vector>

using namespace stc::framework;

class UnitTestDelegate : public CppUnit::TestFixture {

	STCUT_INIT( UnitTestDelegate );

public:

	UnitTestDelegate() {}
	virtual ~UnitTestDelegate() {}

	void setUp() {}
	void tearDown() {}

	
	typedef Delegate< FastDelegate2<int, std::string&> > TestDelegate2;

	/****************************************************************************/
	static void StandardStuff(int p1, std::string& p2) { p2 += "StandardStuff;"; }

	/****************************************************************************/
	class A {
	public:
		A(const char* n):name(n) {}
        virtual ~A() {}
		void DoStuff(int p1, std::string& p2) { p2 += name + "-A::DoStuff;"; }
		virtual void DoVirtualStuff(int p1, std::string& p2) { p2 += name + "-A::DoVirtualStuff;"; }
		std::string name;
	};

	/****************************************************************************/
	class B : public A 
	{
	public:
		B(const char* n):A(n) {}
        virtual ~B() {}
		virtual void DoVirtualStuff(int p1, std::string& p2) { p2 += name + "-B::DoVirtualStuff;"; }
		void DoBStuff(int p1, std::string& p2) const { p2 += name + "-B::DoBStuff;"; }
	};

	/****************************************************************************/
	STCUT( TestDefaultCtor )
	{
		TestDelegate2 d;
		CPPUNIT_ASSERT(d.size() == 0);

		std::string p2;
		d(1, p2);
		CPPUNIT_ASSERT(p2 == "");
	}

	/****************************************************************************/
	STCUT( TestFastDelegateCtor )
	{
		TestDelegate2::value_type fd = MakeDelegate(&StandardStuff);
		TestDelegate2 d ( fd );
		CPPUNIT_ASSERT(d.size() == 1);

		std::string p2;
		d(1, p2);
		CPPUNIT_ASSERT(p2 == "StandardStuff;");
	}

	/****************************************************************************/
	STCUT( TestDelegateCopyCtor )
	{
		TestDelegate2::value_type fd = MakeDelegate(&StandardStuff);
		TestDelegate2 d1 ( fd );
		TestDelegate2 d2 ( d1 );
		CPPUNIT_ASSERT(d2.size() == 1);

		std::string p2;
		d2(1, p2);
		CPPUNIT_ASSERT(p2 == "StandardStuff;");
	}

	/****************************************************************************/
	STCUT( TestStandardFunc )
	{
		TestDelegate2 d;
		d.insert( MakeDelegate(&StandardStuff) );
		CPPUNIT_ASSERT(d.size() == 1);

		std::string p2;
		d(1, p2);
		CPPUNIT_ASSERT(p2 == "StandardStuff;");
	}

	/****************************************************************************/
	STCUT( TestMemberFunc )
	{
		TestDelegate2 d;
		B b("b");
		d.insert( MakeDelegate(&b, &A::DoStuff) );
		CPPUNIT_ASSERT(d.size() == 1);

		std::string p2;
		d(1, p2);
		CPPUNIT_ASSERT(p2 == "b-A::DoStuff;");
	}
	/****************************************************************************/
	STCUT( TestConstMemberFunc )
	{
		TestDelegate2 d;
		const B b("b");
		d.insert( MakeDelegate(&b, &B::DoBStuff) );
		CPPUNIT_ASSERT(d.size() == 1);

		std::string p2;
		d(1, p2);
		CPPUNIT_ASSERT(p2 == "b-B::DoBStuff;");
	}

	/****************************************************************************/
	STCUT( TestVirtualFunc )
	{
		TestDelegate2 d;
		B b("b");
		d.insert( MakeDelegate(&b, &B::DoVirtualStuff) );
		CPPUNIT_ASSERT(d.size() == 1);

		std::string p2;
		d(1, p2);
		CPPUNIT_ASSERT(p2 == "b-B::DoVirtualStuff;");
	}

	/****************************************************************************/
	STCUT( TestDup )
	{
		TestDelegate2 d;
		B b("b");

		d.insert( MakeDelegate(&b, &A::DoVirtualStuff) );
		d.insert( MakeDelegate(&b, &B::DoVirtualStuff) );
		d.insert( MakeDelegate(&b, &B::DoVirtualStuff) );
		CPPUNIT_ASSERT(d.size() == 1);

		std::string p2;
		d(1, p2);
		CPPUNIT_ASSERT(p2 == "b-B::DoVirtualStuff;");
	}

	/****************************************************************************/
	STCUT( TestDupWithTwoInstances )
	{
		TestDelegate2 d;
		B b1("b1");
		B b2("b2");

		d.insert( MakeDelegate(&b1, &A::DoVirtualStuff) );
		d.insert( MakeDelegate(&b1, &B::DoVirtualStuff) );
		d.insert( MakeDelegate(&b1, &B::DoVirtualStuff) );
		CPPUNIT_ASSERT(d.size() == 1);

		d.insert( MakeDelegate(&b2, &A::DoVirtualStuff) );
		d.insert( MakeDelegate(&b2, &B::DoVirtualStuff) );
		d.insert( MakeDelegate(&b2, &B::DoVirtualStuff) );
		CPPUNIT_ASSERT(d.size() == 2);

		std::string p2;
		d(1, p2);
		CPPUNIT_ASSERT(p2.find("b1-B::DoVirtualStuff;") != std::string::npos);
		CPPUNIT_ASSERT(p2.find("b2-B::DoVirtualStuff;") != std::string::npos);
	}

	/****************************************************************************/
	class TestAddRemoveSetup {
	public:
		TestAddRemoveSetup()
			:a1("a1"),a2("a2"),b1("b1"),b2("b2")
		{
			fd1 = MakeDelegate(&a1, &A::DoStuff);
			fd2 = MakeDelegate(&a1, &A::DoVirtualStuff);
			fd3 = MakeDelegate(&a2, &A::DoStuff);
			fd4 = MakeDelegate(&a2, &A::DoVirtualStuff);
			fd5 = MakeDelegate(&b1, &A::DoVirtualStuff);
			fd6 = MakeDelegate(&b2, &A::DoVirtualStuff);
			fd7 = MakeDelegate(&StandardStuff);
		}
		A a1;
		A a2;
		B b1;
		B b2;

		TestDelegate2::value_type fd1;
		TestDelegate2::value_type fd2;
		TestDelegate2::value_type fd3;
		TestDelegate2::value_type fd4;
		TestDelegate2::value_type fd5;
		TestDelegate2::value_type fd6;
		TestDelegate2::value_type fd7;
	};

	STCUT( TestAddFastDelegate )
	{
		TestAddRemoveSetup s;
		TestDelegate2 d;

		d += s.fd1;
		CPPUNIT_ASSERT(d.find(s.fd1) != d.end());

		d += s.fd2;
		CPPUNIT_ASSERT(d.find(s.fd1) != d.end());
		CPPUNIT_ASSERT(d.find(s.fd2) != d.end());
	}

	STCUT( TestAddDelegate )
	{
		TestAddRemoveSetup s;
		TestDelegate2 d;

		d += s.fd1;
		d += s.fd2;
		d += s.fd3;

		TestDelegate2 d1;
		d1 += d;
		CPPUNIT_ASSERT(d == d1);

		d1 += s.fd4;
		CPPUNIT_ASSERT(d1.find(s.fd1) != d1.end());
		CPPUNIT_ASSERT(d1.find(s.fd2) != d1.end());
		CPPUNIT_ASSERT(d1.find(s.fd3) != d1.end());
		CPPUNIT_ASSERT(d1.find(s.fd4) != d1.end());
	}

	/****************************************************************************/
	STCUT( TestRemoveFastDelegate )
	{
		TestAddRemoveSetup s;
		TestDelegate2 d;

		CPPUNIT_ASSERT(d.empty());
		d -= s.fd1;
		CPPUNIT_ASSERT(d.empty());

		d += s.fd1;
		d += s.fd2;
		d += s.fd3;
		CPPUNIT_ASSERT(d.find(s.fd1) != d.end());
		CPPUNIT_ASSERT(d.find(s.fd2) != d.end());
		CPPUNIT_ASSERT(d.find(s.fd3) != d.end());

		d -= s.fd1;
		d -= s.fd3;
		CPPUNIT_ASSERT(d.find(s.fd1) == d.end());
		CPPUNIT_ASSERT(d.find(s.fd2) != d.end());
		CPPUNIT_ASSERT(d.find(s.fd3) == d.end());
	}

	STCUT( TestRemoveDelegate )
	{
		TestAddRemoveSetup s;
		TestDelegate2 d;

		d += s.fd1;
		d += s.fd2;
		d += s.fd3;

		TestDelegate2 d1;
		d1 += d;
		CPPUNIT_ASSERT(d == d1);

		d1 += s.fd4;
		d1 += s.fd5;
		d1 -= s.fd1;

		d1 -= d;

		CPPUNIT_ASSERT(d1.find(s.fd1) == d1.end());
		CPPUNIT_ASSERT(d1.find(s.fd2) == d1.end());
		CPPUNIT_ASSERT(d1.find(s.fd3) == d1.end());
		CPPUNIT_ASSERT(d1.find(s.fd4) != d1.end());
		CPPUNIT_ASSERT(d1.find(s.fd5) != d1.end());
	}

	/****************************************************************************/
	STCUT( TestMultiple )
	{
		TestAddRemoveSetup s;
		TestDelegate2 d;

		d += s.fd1;
		d += s.fd2;
		d += s.fd3;
		d += s.fd4;
		d += s.fd5;
		d += s.fd6;
		d += s.fd7;

		std::string p2;
		d(1, p2);
		CPPUNIT_ASSERT(p2.find("a1-A::DoStuff;") != std::string::npos);
		CPPUNIT_ASSERT(p2.find("a1-A::DoVirtualStuff;") != std::string::npos);
		CPPUNIT_ASSERT(p2.find("a2-A::DoStuff;") != std::string::npos);
		CPPUNIT_ASSERT(p2.find("a2-A::DoVirtualStuff;") != std::string::npos);
		CPPUNIT_ASSERT(p2.find("b1-B::DoVirtualStuff;") != std::string::npos);
		CPPUNIT_ASSERT(p2.find("b2-B::DoVirtualStuff;") != std::string::npos);
		CPPUNIT_ASSERT(p2.find("StandardStuff;") != std::string::npos);
	}

	/****************************************************************************/
	class DelegateRemoveSelf {
	public:
		DelegateRemoveSelf() {};
		~DelegateRemoveSelf() {};

		void delegate2RemoveSelf(int a, std::string& s)
		{
			mDel -= mToRemoveDel;
		}

		TestDelegate2 mDel;
		TestDelegate2 mToRemoveDel;
	};

	STCUT( TestRemoveDuringCallback )
	{
		DelegateRemoveSelf s;
		A a("a");
		B b("b");

		TestDelegate2::value_type fd1 = MakeDelegate(&a, &A::DoStuff);
		TestDelegate2::value_type fd2 = MakeDelegate(&s, &DelegateRemoveSelf::delegate2RemoveSelf);
		TestDelegate2::value_type fd3 = MakeDelegate(&b, &A::DoVirtualStuff);

		s.mDel.insert(fd1);
		s.mDel.insert(fd2);
		s.mDel.insert(fd3);

		CPPUNIT_ASSERT(s.mDel.size() == 3);
		
		std::string p2;

		s.mToRemoveDel.insert(fd2);
		s.mDel(1, p2);
		
		CPPUNIT_ASSERT(s.mDel.size() == 2);
		CPPUNIT_ASSERT(s.mDel.find(fd2) == s.mDel.end());
		CPPUNIT_ASSERT(s.mDel.find(fd1) != s.mDel.end());
		CPPUNIT_ASSERT(s.mDel.find(fd3) != s.mDel.end());
	}

	STCUT( TestRemoveAllDuringCallback )
	{
		DelegateRemoveSelf s;
		A a("a");
		B b("b");

		TestDelegate2::value_type fd1 = MakeDelegate(&a, &A::DoStuff);
		TestDelegate2::value_type fd2 = MakeDelegate(&s, &DelegateRemoveSelf::delegate2RemoveSelf);
		TestDelegate2::value_type fd3 = MakeDelegate(&b, &A::DoVirtualStuff);

		s.mDel.insert(fd1);
		s.mDel.insert(fd2);
		s.mDel.insert(fd3);

		CPPUNIT_ASSERT(s.mDel.size() == 3);
		
		std::string p2;
		s.mToRemoveDel += s.mDel;
		s.mDel(1, p2);
		
		CPPUNIT_ASSERT(s.mDel.empty());
	}

	STCUT( TestRemoveBeforeCallback )
	{
		DelegateRemoveSelf s;
		A a("a");
		B b("b");

		TestDelegate2::value_type fd1 = MakeDelegate(&a, &A::DoStuff);
		TestDelegate2::value_type fd2 = MakeDelegate(&s, &DelegateRemoveSelf::delegate2RemoveSelf);
		TestDelegate2::value_type fd3 = MakeDelegate(&b, &A::DoVirtualStuff);

		s.mDel.insert(fd1);
		s.mDel.insert(fd2);
		s.mDel.insert(fd3);

		CPPUNIT_ASSERT(s.mDel.size() == 3);
		
		std::string p2;
		s.mToRemoveDel.insert(fd1);
		s.mDel(1, p2);
		
		CPPUNIT_ASSERT(s.mDel.size() == 2);
		CPPUNIT_ASSERT(s.mDel.find(fd1) == s.mDel.end());
		CPPUNIT_ASSERT(s.mDel.find(fd2) != s.mDel.end());
		CPPUNIT_ASSERT(s.mDel.find(fd3) != s.mDel.end());
	}

	STCUT( TestRemoveAfterCallback )
	{
		DelegateRemoveSelf s;
		A a("a");
		B b("b");

		TestDelegate2::value_type fd1 = MakeDelegate(&a, &A::DoStuff);
		TestDelegate2::value_type fd2 = MakeDelegate(&s, &DelegateRemoveSelf::delegate2RemoveSelf);
		TestDelegate2::value_type fd3 = MakeDelegate(&b, &A::DoVirtualStuff);

		s.mDel.insert(fd1);
		s.mDel.insert(fd2);
		s.mDel.insert(fd3);

		CPPUNIT_ASSERT(s.mDel.size() == 3);
		
		std::string p2;
		s.mToRemoveDel.insert(fd3);
		s.mDel(1, p2);
		
		CPPUNIT_ASSERT(s.mDel.size() == 2);
		CPPUNIT_ASSERT(s.mDel.find(fd3) == s.mDel.end());
		CPPUNIT_ASSERT(s.mDel.find(fd1) != s.mDel.end());
		CPPUNIT_ASSERT(s.mDel.find(fd2) != s.mDel.end());
	}

	/****************************************************************************/
	class TestVarArg {
	public:
		bool b0, b1, b2, b3, b4, b5, b6, b7, b8;

		TestVarArg():
			b0(false),b1(false),b2(false),b3(false),b4(false),b5(false),b6(false),b7(false),b8(false) {}
		void P0() { b0 = true; }
		void P1(int8_t p1) { b1 = true; }
		void P2(int8_t p1, uint16_t p2) { b2 = true; }
		void P3(int8_t p1, uint16_t p2, double& p3) { b3 = true; }
		void P4(int8_t p1, uint16_t p2, double& p3, std::string p4) { b4 = true; }
		void P5(int8_t p1, uint16_t p2, double& p3, std::string p4, std::vector<char*> p5) { b5 = true; }
		void P6(int8_t p1, uint16_t p2, double& p3, std::string p4, std::vector<char*> p5, TestVarArg p6) { b6 = true; }
		void P7(int8_t p1, uint16_t p2, double& p3, std::string p4, std::vector<char*> p5, TestVarArg p6, char p7) { b7 = true; }
		void P8(int8_t p1, uint16_t p2, double& p3, std::string p4, std::vector<char*> p5, TestVarArg p6, char p7, void* p8) { b8 = true; }
	};
	STCUT( TestVariableArgs )
	{
		typedef Delegate< FastDelegate0<> > D0;
		typedef Delegate< FastDelegate1<int8_t> > D1;
		typedef Delegate< FastDelegate2<int8_t, uint16_t> > D2;
		typedef Delegate< FastDelegate3<int8_t, uint16_t, double&> > D3;
		typedef Delegate< FastDelegate4<int8_t, uint16_t, double&, std::string> > D4;
		typedef Delegate< FastDelegate5<int8_t, uint16_t, double&, std::string, std::vector<char*> > > D5;
		typedef Delegate< FastDelegate6<int8_t, uint16_t, double&, std::string, std::vector<char*>, TestVarArg> > D6;
		typedef Delegate< FastDelegate7<int8_t, uint16_t, double&, std::string, std::vector<char*>, TestVarArg, char> > D7;
		typedef Delegate< FastDelegate8<int8_t, uint16_t, double&, std::string, std::vector<char*>, TestVarArg, char, void*> > D8;

		TestVarArg s;

		D0 d0;
		D1 d1;
		D2 d2;
		D3 d3;
		D4 d4;
		D5 d5;
		D6 d6;
		D7 d7;
		D8 d8;

		d0 += MakeDelegate(&s, &TestVarArg::P0);
		d1 += MakeDelegate(&s, &TestVarArg::P1);
		d2 += MakeDelegate(&s, &TestVarArg::P2);
		d3 += MakeDelegate(&s, &TestVarArg::P3);
		d4 += MakeDelegate(&s, &TestVarArg::P4);
		d5 += MakeDelegate(&s, &TestVarArg::P5);
		d6 += MakeDelegate(&s, &TestVarArg::P6);
		d7 += MakeDelegate(&s, &TestVarArg::P7);
		d8 += MakeDelegate(&s, &TestVarArg::P8);

		std::vector<char*> p5;
		double p3 = 5.5;

		d0();
		CPPUNIT_ASSERT(s.b0);
		d1(1);
		CPPUNIT_ASSERT(s.b1);
		d2(1, 2);
		CPPUNIT_ASSERT(s.b2);
		d3(1, 2, p3);
		CPPUNIT_ASSERT(s.b3);
		d4(1, 2, p3, "ccc");
		CPPUNIT_ASSERT(s.b4);
		d5(1, 2, p3, "ccc", p5);
		CPPUNIT_ASSERT(s.b5);
		d6(1, 2, p3, "ccc", p5, s);
		CPPUNIT_ASSERT(s.b6);
		d7(1, 2, p3, "ccc", p5, s, 'd');
		CPPUNIT_ASSERT(s.b7);
		d8(1, 2, p3, "ccc", p5, s, 'd', (void*)0xff);
		CPPUNIT_ASSERT(s.b8);
	}

	/****************************************************************************/
	/*
	STCUT( TestNoCompile )
	{
		TestDelegate2 d;
		B b;
		d.insert( MakeDelegate(&b, &A::DoStuff) );
		CPPUNIT_ASSERT(d.size() == 1);

		std::string p2;
		d(1);
		d(2, p2, p2);
		d(2, 2);
	}
	*/
	
};

STCUT_REGISTER( UnitTestDelegate );
