#ifndef INCLUDED_MEMORY_STREAM_H
#define INCLUDED_MEMORY_STREAM_H
#include "StcTypes.h"
#include "StcExport.h"

#include "IpAddress.h"
#include "Ipv6Address.h"
#include "MacAddress.h"
#include "WorldWideName.h"
#include "MetaInfo.h"

#include <string>
#include <vector>
#include <list>
#include <deque>

namespace stc {
namespace framework {

using namespace stc::common ;
/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Defines the location from which to seek.
///
enum SeekOrigin 
{ 
	OriginBegin,		///< Seek from the beginning of the stream. 
	OriginCurrent,		///< Seek from the current stream position.
	OriginEnd			///< Seek from the end of the stream as determined by GetLength.
};

// NOTE: move this to a more common header if ever necessary to reuse outside of this file
#ifdef SUNOS_OS
#define STC_BIG_ENDIAN
#else
#define STC_LITTLE_ENDIAN
#endif

#if defined(STC_BIG_ENDIAN) 
inline void beswap(uint16_t& data) 
{
    data = ((data & 0x00ff) << 8) | 
           ((data & 0xff00) >> 8);  
}

inline void beswap(int16_t& data) 
{
    beswap(reinterpret_cast<uint16_t&>(data));
}

inline void beswap(uint32_t& data) 
{
    data = ((data & 0x000000ff) << 24) | 
           ((data & 0x0000ff00) << 8)  |   
           ((data & 0x00ff0000) >> 8)  | 
           ((data & 0xff000000) >> 24);  
}

inline void beswap(int32_t& data) 
{
    beswap(reinterpret_cast<uint32_t&>(data));
}

inline void beswap(uint64_t& data) 
{
    uint32_t lowdata = uint32_t(data & 0xffffffff);
    uint32_t highdata = uint32_t(data >> 32);
    beswap(lowdata);
    beswap(highdata);
    data = uint64_t(lowdata) << 32 | 
           uint64_t(highdata);
}

inline void beswap(int64_t& data) 
{
    beswap(reinterpret_cast<uint64_t&>(data));
}

inline void beswap(double& data)
{
    beswap(reinterpret_cast<uint64_t&>(data));
}
#elif defined(STC_LITTLE_ENDIAN) 
inline void beswap(int16_t& data)  {} // NOP
inline void beswap(uint16_t& data) {} // NOP
inline void beswap(int32_t& data)  {} // NOP
inline void beswap(uint32_t& data) {} // NOP
inline void beswap(int64_t& data)  {} // NOP
inline void beswap(uint64_t& data) {} // NOP
inline void beswap(double& data)   {} // NOP
#else
#error "Either STC_BIG_ENDIAN or STC_LITTLE_ENDIAN must be defined."
#endif

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	A stream with a backing store in memory.  The stream will automatically resize as 
///	required to hold the data.
///
class STC_PUBLIC CMemoryStream
{
	public:
		CMemoryStream( uint32_t nInitialCapacity = 1024 );
		CMemoryStream( uint8_t* pBuffer, uint32_t nLength, bool manageBuffer = false );
		CMemoryStream( const CMemoryStream& that );
		~CMemoryStream();

		CMemoryStream& operator=( const CMemoryStream& that );
		void AttachBuffer( uint8_t* pBuffer, uint32_t nLength, bool manageBuffer = false );

		void Write( const void* pBuffer, uint32_t nCount );
		void Read( void* pBuffer, uint32_t nCount );
		void Seek( uint32_t nOffset, SeekOrigin eSeekOrigin );
		
		/////////////////////////////////////////////////////////////////////////////////////
		///	Recycle the buffer by moving the data at the current position till the length to the beginning of the stream.
		void Recycle();
			
		/////////////////////////////////////////////////////////////////////////////////////
		///	Reset the position to the beginning of the stream.
		void Reset()
			{ m_nPosition = m_nLength = 0; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Get a pointer to the internal buffer used by the stream.  \return A pointer to 
		///	the internal buffer used by the stream.
		uint8_t* GetBuffer()
			{ return m_pBuffer; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Gets the number of bytes allocated for this stream.					
		uint32_t GetCapacity() const
			{ return m_nCapacity; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Returns the number of bytes used by data in the stream.
		uint32_t GetLength() const
			{ return m_nLength; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Returns the current read\write position.
		uint32_t GetPosition() const
			{ return m_nPosition; }

	private:
		uint8_t*			m_pBuffer;			///< Buffer for storing stream data
		uint32_t			m_nCapacity;		///< The current stream capacity
		bool				m_bExternalBuffer;	///< If true the stream is attached to an external buffer
		
		uint32_t			m_nLength;			///< Number of bytes within the memory stream
		uint32_t			m_nPosition;		///< The current seek position

		void AllocBuffer( uint32_t nCapacity );
		void ReallocBuffer( uint32_t nNewCapacity );
		void FreeBuffer();
};

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Reads binary encoded primitive data types from a stream.
///
class STC_PUBLIC CBinaryReader
{
	public:
		CBinaryReader( CMemoryStream& memoryStream );
		~CBinaryReader() {}

		/////////////////////////////////////////////////////////////////////////////////////
		///	Return the base stream attached to this instance.
		CMemoryStream& GetBaseStream() { return m_ms; }
		
		/////////////////////////////////////////////////////////////////////////////////////
		///	Read a binary encoded 1 byte int from the stream attached to this instance.
		CBinaryReader& operator>>( int8_t& value ) { m_ms.Read( &value, sizeof( value ) ); return *this; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Read a binary encoded 1 byte unsigned int from the stream attached to this instance.
		CBinaryReader& operator>>( uint8_t& value ) { m_ms.Read( &value, sizeof( value ) ); return *this; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Read a binary encoded 2 byte int from the stream attached to this instance.
		CBinaryReader& operator>>( int16_t& value ) { m_ms.Read( &value, sizeof( value ) ); beswap(value); return *this; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Read a binary encoded 2 byte unsigned int from the stream attached to this instance.
		CBinaryReader& operator>>( uint16_t& value ) { m_ms.Read( &value, sizeof( value ) ); beswap(value); return *this; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Read a binary encoded 4 byte int from the stream attached to this instance.
		CBinaryReader& operator>>( int32_t& value ) { m_ms.Read( &value, sizeof( value ) ); beswap(value); return *this; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Read a binary encoded 4 byte unsigned int from the stream attached to this instance.
		CBinaryReader& operator>>( uint32_t& value ) { m_ms.Read( &value, sizeof( value ) ); beswap(value); return *this; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Read a binary encoded 8 byte int from the stream attached to this instance.
		CBinaryReader& operator>>( int64_t& value )	{ m_ms.Read( &value, sizeof( value ) ); beswap(value); return *this; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Read a binary encoded 8 byte unsigned int from the stream attached to this instance.
		CBinaryReader& operator>>( uint64_t& value ) { m_ms.Read( &value, sizeof( value ) ); beswap(value); return *this; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Read a binary encoded 1 byte bool from the stream attached to this instance.
		CBinaryReader& operator>>( bool& value ) { m_ms.Read( &value, sizeof( bool ) ); return *this; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Read a binary encoded 8 byte double from the stream attached to this instance.
		CBinaryReader& operator>>( double& value ) { m_ms.Read( &value, sizeof( double ) ); beswap(value); return *this; }

		CBinaryReader& operator>>( std::string& value );
		int Read7BitEncodedInt();

		/////////////////////////////////////////////////////////////////////////////////////
		///	Read an Ipv4 Address type
        CBinaryReader& operator >>( stc::common::CIpAddress& value )
        {
            uint32_t val ;
            operator>>(val) ;
            value.CopyFrom(val) ;
            return *this ;
        }
		/////////////////////////////////////////////////////////////////////////////////////
		///	Read an Ipv6 Address type
        CBinaryReader& operator >>( stc::common::CIpv6Address& value )
        {
            m_ms.Read( (void *) value.Data(), stc::common::CIpv6Address::BYTE_COUNT );
            std::vector<uint8_t> formatting;
            value.EncodeFormatting(formatting);  // Encode so formatting buffer is initialzied to correct size
            m_ms.Read( (void *) & formatting[0], (uint32_t)formatting.size() );
            value.DecodeFormatting(formatting);
            return *this ;
        }
		/////////////////////////////////////////////////////////////////////////////////////
		///	Read an MAC Address type
        CBinaryReader& operator >>( stc::common::CMacAddress& value ) 
        {
            m_ms.Read( (void *)value.Data(), stc::common::CMacAddress::BYTE_COUNT);
            return *this ;
        }
		/////////////////////////////////////////////////////////////////////////////////////
		///	Read a ClassId type
        CBinaryReader& operator >>( stc::framework::ClassId& value ) 
        {
            m_ms.Read( &value.m_id, sizeof(value.m_id) );
            beswap(value.m_id);
            return *this ;
        }
		/////////////////////////////////////////////////////////////////////////////////////
		///	Read a PropId type
        CBinaryReader& operator >>( stc::framework::PropId& value ) 
        {
            m_ms.Read( &value.m_id, sizeof(value.m_id) );
            beswap(value.m_id);
            return *this ;
        }
		/////////////////////////////////////////////////////////////////////////////////////
		///	Read a World Wide Name type
        CBinaryReader& operator >>( stc::common::CWorldWideName& value ) 
        {
            m_ms.Read( (void *)value.Data(), stc::common::CWorldWideName::BYTE_COUNT);
            return *this ;
        }
        /////////////////////////////////////////////////////////////////////////////////////////////
		///	Read a STL collection from the stream attached to this instance.  Note that
		/// the class contained by the collection must support overloaded operator>>.
		template <typename T> CBinaryReader& operator>>( T& collection )
		{
			uint32_t nCount = Read7BitEncodedInt();
			collection.resize( nCount );
			
			for ( typename T::iterator it = collection.begin(); it != collection.end(); ++it )
				*this >> *it;
			
			return *this;
		}

	private:
		CMemoryStream&				m_ms;		///< The stream attached to this instance.
};

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Writes primitive data types as binary encoded data into a stream.
///
class STC_PUBLIC CBinaryWriter
{
	public:
		CBinaryWriter( CMemoryStream& memoryStream );

		/////////////////////////////////////////////////////////////////////////////////////
		///	Destruct a CBinaryWriter instance..
		~CBinaryWriter() {}

		/////////////////////////////////////////////////////////////////////////////////////
		///	Return the base stream attached to this instance.
		CMemoryStream& GetBaseStream() { return m_ms; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Write a binary encoded 1 byte int into the stream attached to this instance.
		CBinaryWriter& operator<<( int8_t value ) { m_ms.Write( &value, sizeof( value ) ); return *this; } 

		/////////////////////////////////////////////////////////////////////////////////////
		///	Write a binary encoded 1 byte unsigned int into the stream attached to this instance.
		CBinaryWriter& operator<<( uint8_t value ) { m_ms.Write( &value, sizeof( value ) ); return *this; } 

		/////////////////////////////////////////////////////////////////////////////////////
		///	Write a binary encoded 2 byte int into the stream attached to this instance.
		CBinaryWriter& operator<<( int16_t value ) { beswap(value); m_ms.Write( &value, sizeof( value ) ); return *this; } 

		/////////////////////////////////////////////////////////////////////////////////////
		///	Write a binary encoded 2 byte unsigned int into the stream attached to this instance.
		CBinaryWriter& operator<<( uint16_t value ) { beswap(value); m_ms.Write( &value, sizeof( value ) ); return *this; } 

		/////////////////////////////////////////////////////////////////////////////////////
		///	Write a binary encoded 4 byte int into the stream attached to this instance.
		CBinaryWriter& operator<<( int32_t value ) { beswap(value); m_ms.Write( &value, sizeof( value ) ); return *this; } 

		/////////////////////////////////////////////////////////////////////////////////////
		///	Write a binary encoded 4 byte unsigned int into the stream attached to this instance.
		CBinaryWriter& operator<<( uint32_t value ) { beswap(value); m_ms.Write( &value, sizeof( value ) ); return *this; } 

		/////////////////////////////////////////////////////////////////////////////////////
		///	Write a binary encoded 8 byte int into the stream attached to this instance.
		CBinaryWriter& operator<<( int64_t value ) { beswap(value); m_ms.Write( &value, sizeof( value ) ); return *this; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Write a binary encoded 8 byte unsigned int into the stream attached to this instance.
		CBinaryWriter& operator<<( uint64_t value ) { beswap(value); m_ms.Write( &value, sizeof( value ) ); return *this; }

		/////////////////////////////////////////////////////////////////////////////////////
		///	Write a binary encoded 1 byte bool into the stream attached to this instance.
		CBinaryWriter& operator<<( bool value ) { m_ms.Write( &value, sizeof( value ) ); return *this; } 

		/////////////////////////////////////////////////////////////////////////////////////
		///	Write a binary encoded 8 byte bool double the stream attached to this instance.
		CBinaryWriter& operator<<( double value ) { beswap(value); m_ms.Write( &value, sizeof( value ) ); return *this; } 

		/////////////////////////////////////////////////////////////////////////////////////////////
		///	Write a binary encoded variable length string into the stream attached to this instance.
		CBinaryWriter& operator<<( const std::string& value ) { return operator<<( value.c_str() ); }

		CBinaryWriter& operator<<( const char* value );
		void Write7BitEncodedInt( int value );

		/////////////////////////////////////////////////////////////////////////////////////
		///	Write an Ipv4 Address type
        CBinaryWriter& operator <<( const stc::common::CIpAddress& value ) 
        {
            operator<< (value.ToU32()) ;
            return *this ;
        }
		/////////////////////////////////////////////////////////////////////////////////////
		///	Write an Ipv6 Address type
        CBinaryWriter& operator <<( const stc::common::CIpv6Address& value )
        {
            m_ms.Write( value.Data(), stc::common::CIpv6Address::BYTE_COUNT );
            std::vector<uint8_t> formatting;
            value.EncodeFormatting(formatting);
            m_ms.Write( &formatting[0], (uint32_t)formatting.size() );
            return *this ;
        }
		/////////////////////////////////////////////////////////////////////////////////////
		///	Write an MAC Address type
        CBinaryWriter& operator <<( const stc::common::CMacAddress& value )
        {
            m_ms.Write( value.Data(), stc::common::CMacAddress::BYTE_COUNT );
            return *this ;
        }
		/////////////////////////////////////////////////////////////////////////////////////
		///	Write a ClassId type
        CBinaryWriter& operator <<( const stc::framework::ClassId& value )
        {
            stc::framework::ClassId swapvalue(value);
            beswap(swapvalue.m_id);
            m_ms.Write( &swapvalue.m_id, sizeof(swapvalue.m_id) );
            return *this ;
        }
		/////////////////////////////////////////////////////////////////////////////////////
		///	Write a PropId type
        CBinaryWriter& operator <<( const stc::framework::PropId& value )
        {
            stc::framework::PropId swapvalue(value);
            beswap(swapvalue.m_id);
            m_ms.Write( &swapvalue.m_id, sizeof(swapvalue.m_id) );
            return *this ;
        }
		/////////////////////////////////////////////////////////////////////////////////////
		///	Write a World Wide Name type
        CBinaryWriter& operator <<( const stc::common::CWorldWideName& value )
        {
            m_ms.Write( value.Data(), stc::common::CWorldWideName::BYTE_COUNT );
            return *this ;
        }
        /////////////////////////////////////////////////////////////////////////////////////////////
		///	Write a STL collection into the stream attached to this instance. Note that
		/// the class contained in the collection must support overloaded operator<<.
		template <typename T> CBinaryWriter& operator<<( const T& collection )
		{
			Write7BitEncodedInt( (int) collection.size() );
			
			for ( typename T::const_iterator it = collection.begin(); it != collection.end(); ++it )
				*this << *it;

			return *this;
		}

	private:
		CMemoryStream&				m_ms;		///< The stream attached to this instance.
};

}
}

#endif

