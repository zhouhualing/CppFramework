#include "StdAfx.h"

#include "cppunit/TestCase.h"
#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"
#include "StcUnitTestHelperMacro.h"

#include "BinaryReaderWriter.h"

using namespace std;
using namespace stc::framework;

class UnitTestBinaryReaderWriter : public CppUnit::TestFixture {

	STCUT_INIT( UnitTestBinaryReaderWriter );

public:

	UnitTestBinaryReaderWriter() {}
	virtual ~UnitTestBinaryReaderWriter() {}

	void setUp() {}
	void tearDown() {}

	STCUT( TestNetBinaryReader )
	{
		CMemoryStream stream;
		uint8_t  eu8 = 0xF5;
		int8_t   ei8 = 0x5F;
		uint16_t eu16 = htonU16(0xFFCC);
		int16_t  ei16 = htonU16((uint16_t)-1300);
		uint32_t eu32 = htonU32(0xAABBCCDD);
		int32_t  ei32 = htonU32(0xAABBCCDD);
		uint64_t eu64 = htonU64(0xAABBCCDDEEFF0011LL);
		int64_t  ei64 = htonU64(0xAABBCCDDEEFF0011LL);
		double   edbl = htond(33333333.0e8);
		bool     eblean = true;

		stream.Write(&eu8, sizeof(eu8));
		stream.Write(&ei8, sizeof(ei8));
		stream.Write(&eu16, sizeof(eu16));
		stream.Write(&ei16, sizeof(ei16));
		stream.Write(&eu32, sizeof(eu32));
		stream.Write(&ei32, sizeof(ei32));
		stream.Write(&eu64, sizeof(eu64));
		stream.Write(&ei64, sizeof(ei64));
		stream.Write(&edbl, sizeof(edbl));
		stream.Write(&eblean, sizeof(eblean));

		eu8 = 0xF5;
		ei8 = 0x5F;
		eu16 = 0xFFCC;
		ei16 = -1300;
		eu32 = 0xAABBCCDD;
		ei32 = 0xAABBCCDD;
		eu64 = 0xAABBCCDDEEFF0011LL;
		ei64 = 0xAABBCCDDEEFF0011LL;
		edbl = 33333333.0e8;
		eblean = true;

		uint8_t  u8;
		int8_t   i8;
		uint16_t u16;
		int16_t  i16;
		uint32_t u32;
		int32_t  i32;
		uint64_t u64;
		int64_t  i64;
		double   dbl;
		bool     blean;

		stream.Seek(0, OriginBegin);
		NetBinaryReader os(stream);
		os >> u8 >> i8 >> u16 >> i16 >> u32 >> i32 >> u64 >> i64 >> dbl >> blean;

		CPPUNIT_ASSERT_EQUAL(eu8, u8);
		CPPUNIT_ASSERT_EQUAL(eu8, u8);
		CPPUNIT_ASSERT_EQUAL(eu16, u16);
		CPPUNIT_ASSERT_EQUAL(ei16, i16);
		CPPUNIT_ASSERT_EQUAL(eu32, u32);
		CPPUNIT_ASSERT_EQUAL(ei32, i32);
		CPPUNIT_ASSERT_EQUAL(eu64, u64);
		CPPUNIT_ASSERT_EQUAL(ei64, i64);
		CPPUNIT_ASSERT_EQUAL(edbl, dbl);
		CPPUNIT_ASSERT_EQUAL(eblean, blean);
	}

	STCUT( TestNetBinaryWriter )
	{
		CMemoryStream stream;
		NetBinaryWriter os(stream);

		uint8_t  u8 = 0xF5;
		int8_t   i8 = 0x5F;
		uint16_t u16 = 0xFFCC;
		int16_t  i16 = -1300;
		uint32_t u32 = 0xAABBCCDD;
		int32_t  i32 = 0xAABBCCDD;
		uint64_t u64 = 0xAABBCCDDEEFF0011LL;
		int64_t  i64 = 0xAABBCCDDEEFF0011LL;
		double   dbl = 33333333.0e8;
		bool     blean = true;

		os << u8 << i8 << u16 << i16 << u32 << i32 << u64 << i64 << dbl << blean;

		uint8_t  eu8 = 0xF5;
		//int8_t   ei8 = 0x5F;
		uint16_t eu16 = htonU16(0xFFCC);
		int16_t  ei16 = htonU16((uint16_t)-1300);
		uint32_t eu32 = htonU32(0xAABBCCDD);
		int32_t  ei32 = htonU32(0xAABBCCDD);
		uint64_t eu64 = htonU64(0xAABBCCDDEEFF0011LL);
		int64_t  ei64 = htonU64(0xAABBCCDDEEFF0011LL);
		double   edbl = htond(33333333.0e8);
		bool     eblean = true;

		stream.Seek(0, OriginBegin);

		stream.Read(&u8, sizeof(u8));
		stream.Read(&i8, sizeof(i8));
		stream.Read(&u16, sizeof(u16));
		stream.Read(&i16, sizeof(i16));
		stream.Read(&u32, sizeof(u32));
		stream.Read(&i32, sizeof(i32));
		stream.Read(&u64, sizeof(u64));
		stream.Read(&i64, sizeof(i64));
		stream.Read(&dbl, sizeof(dbl));
		stream.Read(&blean, sizeof(blean));

		CPPUNIT_ASSERT_EQUAL(eu8, u8);
		CPPUNIT_ASSERT_EQUAL(eu8, u8);
		CPPUNIT_ASSERT_EQUAL(eu16, u16);
		CPPUNIT_ASSERT_EQUAL(ei16, i16);
		CPPUNIT_ASSERT_EQUAL(eu32, u32);
		CPPUNIT_ASSERT_EQUAL(ei32, i32);
		CPPUNIT_ASSERT_EQUAL(eu64, u64);
		CPPUNIT_ASSERT_EQUAL(ei64, i64);
		CPPUNIT_ASSERT_EQUAL(edbl, dbl);
		CPPUNIT_ASSERT_EQUAL(eblean, blean);
	}
};


STCUT_REGISTER( UnitTestBinaryReaderWriter );

