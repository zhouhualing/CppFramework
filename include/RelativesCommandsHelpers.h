#ifndef INCLUDED_FRAMEWORK_RELATIVESCOMMANDSHELPERS_H
#define INCLUDED_FRAMEWORK_RELATIVESCOMMANDSHELPERS_H

#include "MetaInfo.h"
#include "GetRelationsMetaInfoCommand.h"
#include <string>

using namespace std;

namespace stc {
namespace framework {

	//Convert from data model enum to relation type direction value
	STC_PUBLIC RelationType::Direction GetRelationTypeDirection(CGetRelationsMetaInfoCommand::EnumDirection direction);
	STC_PUBLIC RelationType::Direction GetRelationTypeDirection(uint8_t direction);
	STC_PUBLIC CGetRelationsMetaInfoCommand::EnumDirection GetRelationTypeDirection(RelationType::Direction direction);
	//Convert from a string to a relation type name value
	STC_PUBLIC RelationTypeId GetRelationTypeId( const string & typeId );
	STC_PUBLIC const char * GetRelationType( RelationTypeId typeId );

	}
}

#endif //INCLUDED_FRAMEWORK_RELATIVESCOMMANDSHELPERS_H

