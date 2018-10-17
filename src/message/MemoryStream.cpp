#include "StdAfx.h"

#include "MemoryStream.h"
#include "StcException.h"

/////////////////////////////////////////////////////////////////////////////////////////////
///
using namespace stc::framework;

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Construct a new memory stream instance with the requested initial capacity.
///
CMemoryStream::CMemoryStream
( 
	uint32_t nInitialCapacity	///< The initial capacity of the stream buffer. The buffer
								///< will grow if you try to stream more data than the initial 
								///< capacity. 
)	
	:
	m_pBuffer( NULL ),
	m_nCapacity( 0 ),
	m_bExternalBuffer( false ),
	m_nLength( 0 ),
	m_nPosition( 0 )
{
	AllocBuffer( nInitialCapacity );
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Construct a new memory stream instance attached to an existing memory buffer.
///
CMemoryStream::CMemoryStream
( 
	uint8_t* pBuffer,		///< Pointer to a valid memory buffer.
	uint32_t nLength, 		///< Size in bytes of pBuffer.
	bool manageBuffer		///< Whether the stream will own the memory buffer. (default false)
)
	:
	m_pBuffer( pBuffer ),
	m_nCapacity( nLength ),
	m_bExternalBuffer( true ),
	m_nLength( 0 ),
	m_nPosition( 0 )
{
	AttachBuffer( pBuffer, nLength, manageBuffer );
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Construct a new memory stream instance from the data contained in another instance.
///
CMemoryStream::CMemoryStream
( 
	const CMemoryStream& that		///< The CMemoryStream to copy.
)
{
	*this = that;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Destruct a CMemoryStream instance.
///
CMemoryStream::~CMemoryStream()
{
	FreeBuffer();
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Attach this instance to an existing memory buffer.  The currently attached buffer will 
///	be deleted.
///
void CMemoryStream::AttachBuffer
( 
	uint8_t* pBuffer,			///< Pointer to a valid memory buffer.
	uint32_t nLength,			///< Size in bytes of pBuffer.
	bool manageBuffer			///< Whether the stream will own the memory buffer. (default false)
)
{
	FreeBuffer();

	m_pBuffer = pBuffer;
	m_nCapacity = m_nLength = nLength;
	m_nPosition = 0;
	m_bExternalBuffer = !manageBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Copy the data contained in another instance.
///
CMemoryStream& CMemoryStream::operator=
( 
	const CMemoryStream& that		///< The CMemoryStream to copy.
)
{
	if ( that.m_nCapacity > m_nCapacity )
		ReallocBuffer( that.m_nCapacity );

	memcpy( m_pBuffer, that.m_pBuffer, that.m_nLength );

	m_bExternalBuffer = false;
	m_nLength = that.m_nLength;
	m_nPosition = that.m_nPosition;

	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Write the contents of a buffer into the stream.  The stream capacity will be expanded as 
///	necessary to fit nLength bytes.
///
void CMemoryStream::Write
(
	const void* pBuffer,			///< Buffer from which to write data.
	uint32_t nCount					///< Number of bytes to write into stream .
)
{
	uint32_t nRequiredCapacity = m_nPosition + nCount;
	
	// Make sure there is enough room
	if ( nRequiredCapacity > m_nCapacity )
	{
		if ( !m_bExternalBuffer )
		{
			uint32_t nNewCapacity = nRequiredCapacity * 2;
			ReallocBuffer( nNewCapacity );
		}
		else
			throw CStcRunTimeError( "CMemoryStream: can't resize stream attached to external buffer" );
	}

	memcpy( m_pBuffer + m_nPosition, pBuffer, nCount );

	// Adjust the position
	m_nPosition += nCount;

	// Adjust the "Consumed" length if we have written past the old length
	if ( m_nPosition >= m_nLength )
		m_nLength = m_nPosition;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Read nCount bytes from the stream into a external buffer.
///
void CMemoryStream::Read
(
	void* pBuffer,					///< Buffer from which to read data.
	uint32_t nCount					///< Number of bytes to read from pBuffer.
)
{
	if ( m_nPosition + nCount > m_nLength )
		throw CStcRunTimeError( "CMemoryStream: Can't read past the current stream length" );

	memcpy( pBuffer, m_pBuffer + m_nPosition, nCount );

	m_nPosition += nCount;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Move the current position to a new offset.  You can seek anywhere but you can't read
///	past the length so be careful.
///
void CMemoryStream::Seek
( 
	uint32_t nOffset,			///< The new offset position relative to the base defined in 
								///< eSeekOrigin.
	SeekOrigin eSeekOrigin		///< The reference points in the stream from which to seek.
)
{
	switch ( eSeekOrigin )
	{
		case OriginBegin:
			m_nPosition = nOffset;
			break;
			
		case OriginCurrent:
			m_nPosition += nOffset;
			break;

		case OriginEnd:
			m_nPosition = m_nLength - nOffset;
			break;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Moves the bytes from the current position till the end of the buffer to the start of the buffer. 
/// Implements buffer re-use.
///
void CMemoryStream::Recycle
( 

)
{
	uint32_t nLength = m_nLength-m_nPosition;
	memmove(m_pBuffer,m_pBuffer+m_nPosition,nLength);
	m_nPosition=0;
	m_nLength=nLength;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Allocate the buffer used by the stream
///
void CMemoryStream::AllocBuffer
( 
	uint32_t nCapacity					///< The capacity of the stream buffer.
)	
{
	m_pBuffer = new uint8_t[ nCapacity ];
	memset(m_pBuffer, 0, nCapacity);
	m_nCapacity = nCapacity;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Allocate the buffer used by the stream
///
void CMemoryStream::FreeBuffer()	
{
	if ( !m_bExternalBuffer )
	{
		delete [] m_pBuffer;
		m_nCapacity = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Resize the buffer used by the stream
///
void CMemoryStream::ReallocBuffer
( 
	uint32_t nNewCapacity					///< The new capacity of the stream buffer.
)	
{
	// TBD: Can realloc be used on new'ed memory? 	
	// m_pBuffer = (uint8_t*)realloc( m_pBuffer, nNewCapacity );

	// Remember where the existing data is
	uint8_t* pSaveBuffer = m_pBuffer;

	// Make the new buffer
	AllocBuffer( nNewCapacity );
	
	// Now copy the existing data
	memcpy( m_pBuffer, pSaveBuffer, m_nLength );

	// And get rid of the old
	delete [] pSaveBuffer;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Create an new CBinaryWriter instance attached to a CMemoryStream.
///
CBinaryWriter::CBinaryWriter
( 
	CMemoryStream& memoryStream			///<  The stream to attach.
)
	:
	m_ms( memoryStream )
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// Write a binary encoded null terminated string into the stream attached to this instance.
///
CBinaryWriter& CBinaryWriter::operator<<
(
	const char* pszValue		///< The null terminated string to write into the attached stream.
)
{
	uint32_t nLength = strlen( pszValue );
	
	Write7BitEncodedInt( nLength );
	m_ms.Write( pszValue, nLength );
	
	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Write a 7 bit encoded length value into the attached stream.  Variable length encoding 
///	defines the first 7 bits as the length and the 8th bit indicates if the next byte is to 
///	be included. Thus values less than 128 can be encoded in a single byte.
///
void CBinaryWriter::Write7BitEncodedInt
(
	int value		///< The value to encode and write into the attached stream.
)
{
	do 
	{
		int high = ( value >> 7 ) & 0x01ffffff;
		uint8_t b = (uint8_t)( value & 0x7f );

		if ( high != 0 )
			b = (uint8_t)( b | 0x80 );

		m_ms.Write( &b, sizeof( uint8_t ) );

		value = high;
	} while ( value != 0 );
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Create an new CBinaryReader instance attached to a CMemoryStream.
///
CBinaryReader::CBinaryReader
( 
	CMemoryStream& memoryStream			///<  The stream to attach.
)
	:
	m_ms( memoryStream )
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Read a binary encoded variable length string from the stream attached to this instance.
///
CBinaryReader& CBinaryReader::operator>>
( 
	std::string& sValue			///< String in which to read.
)
{
	// First read the string length
	uint32_t nCount = Read7BitEncodedInt();
	
	// Make sure there really is length remaining in the buffer
	if ( m_ms.GetPosition() + nCount > m_ms.GetLength() )
		throw CStcOverFlowError( "CMemoryStream read string overflow" );

	// Get the underlying buffer
	const void* pBuffer = m_ms.GetBuffer();
	const char* pszBuffer = (const char*)pBuffer + m_ms.GetPosition();
	
	// And copy nCount bytes from the stream buffer into the input std::string
  	sValue.replace( 0, sValue.length(), pszBuffer, nCount );

	// Adjust the stream position past the string
	m_ms.Seek( nCount, OriginCurrent );

	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////////
///
///	Read a 7 bit encoded length integer from the attached stream.  Variable length encoding 
///	defines the first 7 bits as the length and the 8th bit indicates if the next byte is to 
///	be included. Thus values less than 128 can be encoded in a single byte.
///
/// \return A decoded length value.
///
int CBinaryReader::Read7BitEncodedInt()
{
	int ret = 0;
	int shift = 0;
	uint8_t b;

	do 
	{
		m_ms.Read( &b, sizeof( uint8_t ) );		
		ret = ret | ((b & 0x7f) << shift);
		shift += 7;
	} while ((b & 0x80) == 0x80);

	return ret;
}

