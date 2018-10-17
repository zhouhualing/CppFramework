#ifndef INCLUDED_RELATIONDATA_H
#define INCLUDED_RELATIONDATA_H

#include "StcExport.h"
#include "StcHandle.h"
#include "StcTypes.h"

#include <string>

namespace stc {
namespace framework {

class CBinaryReader;
class CBinaryWriter;

class STC_PUBLIC CRelationData
{
public:
	CRelationData();
	CRelationData( 
		StcHandle sourceHandle, 
		StcHandle targetHandle, 
		uint32_t relationType, 
		bool isDelete );
	CRelationData( const CRelationData& rd );
	~CRelationData();

	CRelationData& operator=( const CRelationData& rd );

    StcHandle GetSourceHandle() const { return m_SourceHandle; }
	StcHandle GetTargetCommand() const { return m_TargetCommand; }
    uint32_t GetType() const { return m_RelationType; }
    bool GetIsDelete() const { return m_IsDelete; }

    void SetSourceHandle( StcHandle sourceHandle ) { m_SourceHandle = sourceHandle; }
	void SetTargetCommand( StcHandle targetHandle ) { m_TargetCommand = targetHandle; }
    void SetType( uint32_t relationType ) { m_RelationType = relationType; }
    void SetIsDelete( bool isDelete ) { m_IsDelete = isDelete; }

	friend STC_PUBLIC CBinaryReader& operator >>( CBinaryReader& br, CRelationData& value );
	friend STC_PUBLIC CBinaryWriter& operator <<( CBinaryWriter& bw, const CRelationData& value );

private:
	StcHandle	m_SourceHandle;
	StcHandle	m_TargetCommand;
	uint32_t	m_RelationType;
	bool		m_IsDelete;
};

STC_PUBLIC CBinaryReader& operator >>( CBinaryReader& br, CRelationData& value );
STC_PUBLIC CBinaryWriter& operator <<( CBinaryWriter& bw, const CRelationData& value );

}
}

#endif // INCLUDED_RELATIONDATA_H

