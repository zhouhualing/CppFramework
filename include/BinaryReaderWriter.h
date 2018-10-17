#ifndef INCLUDED_BINARY_READER_WRITER_H
#define INCLUDED_BINARY_READER_WRITER_H

#include "MemoryStream.h"
#include "StringUtil.h"

#include <string>
#include <cassert>
#include <vector>
#include <list>

namespace stc {
namespace framework {

/**
		Binary Reader template
 */
template <typename IoStream, typename Decoder>
class BinaryReaderT {
public:

	/// \brief Ctor.
	BinaryReaderT(IoStream& stream):m_strm(stream) {}
	/// \brief Dtor.
	~BinaryReaderT() {}

	/// \brief Gets the underlying IO stream.
	IoStream& GetBaseStream() { return m_strm; }

	/// \brief Reads value from the stream.
	template<typename T>
	BinaryReaderT& operator >> (T& value) 
	{ 
		Decoder::Decode(m_strm, value); 
		return *this; 
	}

private:
	IoStream& m_strm;
};

/**
		Binary Writer template
 */
template<typename IoStream, typename Encoder >
class BinaryWriterT {
public:

	/// \brief Ctor
	BinaryWriterT(IoStream& stream):m_strm(stream) {}
	/// \brief Dtor
	~BinaryWriterT() {}

	/// \brief Gets the underlying IO stream.
	IoStream& GetBaseStream() { return m_strm; }

	/// \brief Writes value to the stream.
	template<typename T>
	BinaryWriterT& operator << (const T& value) 
	{ 
		Encoder::Encode(m_strm, value); 
		return *this; 
	}

private:
	IoStream& m_strm;
};

/**
		Binary Codec that byteswaps fields.
 */
template<typename Stream>
class NetBinaryCodec
{
public:

	/**************************************************************************/
	// Decode Section

	// Default Error
	template<typename T>
	static void Decode(Stream& strm, T& value)
	{
#ifdef WIN32
		T.IsNotSupported();
#else
        extern void fakeFunction() ;
        fakeFunction() ; // Force linker error here
#endif
	}

	// byte
	static void Decode(Stream& strm, uint8_t& value)
	{
		strm.Read(&value, sizeof(value)); 
	}
	static void Decode(Stream& strm, int8_t& value)
	{
		strm.Read(&value, sizeof(value)); 
	}

	// short
	static void Decode(Stream& strm, uint16_t& value)
	{
		strm.Read(&value, sizeof(value)); 
		value = stc::framework::ntohU16(value);
	}
	static void Decode(Stream& strm, int16_t& value)
	{
		strm.Read(&value, sizeof(value)); 
		value = stc::framework::ntohU16(value);
	}

	// long
	static void Decode(Stream& strm, uint32_t& value)
	{
		strm.Read(&value, sizeof(value)); 
		value = stc::framework::ntohU32(value);
	}
	static void Decode(Stream& strm, int32_t& value)
	{
		strm.Read(&value, sizeof(value)); 
		value = stc::framework::ntohU32(value);
	}

	// long long
	static void Decode(Stream& strm, uint64_t& value)
	{
		strm.Read(&value, sizeof(value)); 
		value = stc::framework::ntohU64(value);
	}
	static void Decode(Stream& strm, int64_t& value)
	{
		strm.Read(&value, sizeof(value)); 
		value = stc::framework::ntohU64(value);
	}
	static void Decode(Stream& strm, double& value)
	{
		strm.Read(&value, sizeof(value)); 
		value = stc::framework::ntohd(value);
	}

	// bool
	static void Decode(Stream& strm, bool& value)
	{
		strm.Read(&value, sizeof(value)); 
	}

	/**************************************************************************/
	// Encode Section

	// Default Error
	template<typename T>
	static void Encode(Stream& strm, const T& value)
	{
#ifdef WIN32
		T.IsNotSupported();
#else
        extern void fakeFunction() ;
        fakeFunction() ; // Force linker error here
#endif
	}

	// byte
	static void Encode(Stream& strm, const uint8_t& value)
	{
		strm.Write(&value, sizeof(value)); 
	}
	static void Encode(Stream& strm, const int8_t& value)
	{
		strm.Write(&value, sizeof(value)); 
	}

	// short
	static void Encode(Stream& strm, const uint16_t& value)
	{
		uint16_t v = stc::framework::htonU16(value);
		strm.Write(&v, sizeof(value)); 
	}
	static void Encode(Stream& strm, const int16_t& value)
	{
		int16_t v = stc::framework::htonU16(value);
		strm.Write(&v, sizeof(value)); 
	}

	// long
	static void Encode(Stream& strm, const uint32_t& value)
	{
		uint32_t v = stc::framework::htonU32(value);
		strm.Write(&v, sizeof(value)); 
	}
	static void Encode(Stream& strm, const int32_t& value)
	{
		int32_t v = stc::framework::htonU32(value);
		strm.Write(&v, sizeof(value)); 
	}

	// long long
	static void Encode(Stream& strm, const uint64_t& value)
	{
		uint64_t v = stc::framework::htonU64(value);
		strm.Write(&v, sizeof(value)); 
	}
	static void Encode(Stream& strm, const int64_t& value)
	{
		int64_t v = stc::framework::htonU64(value);
		strm.Write(&v, sizeof(value)); 
	}
	static void Encode(Stream& strm, const double& value)
	{
		double v = stc::framework::htond(value);
		strm.Write(&v, sizeof(value)); 
	}

	// bool
	static void Encode(Stream& strm, const bool& value)
	{
		strm.Write(&value, sizeof(value)); 
	}

};

typedef BinaryWriterT<stc::framework::CMemoryStream, NetBinaryCodec<stc::framework::CMemoryStream> > NetBinaryWriter;
typedef BinaryReaderT<stc::framework::CMemoryStream, NetBinaryCodec<stc::framework::CMemoryStream> > NetBinaryReader;

}
}

#endif // INCLUDED_BINARY_READER_WRITER_H

