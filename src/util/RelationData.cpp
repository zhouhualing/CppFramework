#include "StdAfx.h"

#include "RelationData.h"
#include "MemoryStream.h"

using namespace stc::framework;

CRelationData::CRelationData()
	:
	m_SourceHandle( NULL_STCHANDLE ),
	m_TargetCommand( NULL_STCHANDLE ),
	m_RelationType( 0 ),
	m_IsDelete( false )
{
}

CRelationData::CRelationData
( 
	StcHandle sourceHandle, 
	StcHandle targetHandle, 
	uint32_t relationType, 
	bool isDelete
)
	:
	m_SourceHandle( sourceHandle ),
	m_TargetCommand( targetHandle ),
	m_RelationType( relationType ),
	m_IsDelete( isDelete )
{
}


CRelationData::CRelationData( const CRelationData& rd )
{
	m_SourceHandle = rd.m_SourceHandle;
	m_TargetCommand = rd.m_TargetCommand;
    m_RelationType = rd.m_RelationType;
    m_IsDelete = rd.m_IsDelete;
}

CRelationData::~CRelationData()
{
}

CRelationData& 
CRelationData::operator = (const CRelationData& rd )
{
	if (this == &rd)
		return (*this);
	
	m_SourceHandle = rd.m_SourceHandle;
	m_TargetCommand = rd.m_TargetCommand;
    m_RelationType = rd.m_RelationType;
    m_IsDelete = rd.m_IsDelete;
	
	return (*this);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
CBinaryReader& stc::framework::operator >>
( 
	CBinaryReader& br, 
	CRelationData& value 
)
{
	br >> value.m_SourceHandle;
	br >> value.m_TargetCommand;
	br >> value.m_RelationType;
	br >> value.m_IsDelete;

	return br;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
CBinaryWriter& stc::framework::operator <<
( 
	CBinaryWriter& bw, 
	const CRelationData& value 
)
{
	bw << value.m_SourceHandle;
	bw << value.m_TargetCommand;
	bw << value.m_RelationType;
	bw << value.m_IsDelete;

	return bw;
}
