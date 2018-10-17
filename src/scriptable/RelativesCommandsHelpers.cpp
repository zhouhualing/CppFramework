#include "StdAfx.h"

#include "RelativesCommandsHelpers.h"
#include "MetaClassManager.h"
#include "MetaClass.h"
#include <map>

namespace stc {
namespace framework {

    //this function is so that the string comparison is case insensitive, which is nice
    struct ltstr
    {
      bool operator()(std::string s1, std::string s2) const
      {
        //override microsoft's pragmatism and gimme back my stricmp() ;-)
        #ifdef _MSC_VER
            #pragma warning( disable : 4996 ) 
        #elif defined __GNUC__
            #define stricmp strcasecmp
        #endif
        return stricmp(s1.c_str(), s2.c_str()) < 0;
      }
    };    

    typedef map<std::string, RelationTypeId, ltstr> RelationTypeMap;
    typedef pair <std::string, RelationTypeId> RelationTypeEntry;
    static RelationTypeMap _mapRelationTypes;

    void ReadRelationTypes()
    {
		    CMetaClassIterator it = CMetaClassManager::Instance().GetMetaClassIterator();
		    while ( it.HasNext() )
		    {
			    CMetaClass* cls = it.Next(); 
                CMetaRelationIterator iterRelations = cls->RelationBegin();
                while ( iterRelations != cls->RelationEnd() )
                {
                    MetaRelationInfo mr = *iterRelations;
                    _mapRelationTypes.insert( RelationTypeEntry(mr.typeName, mr.typeId) );
                    ++iterRelations; 
                }
		    }
    }

    //HACK! i mean this really should go into some kind of init(), don't know where though
    //right now, working on a demo, so there's no time, sorry... ;-(

    void _ReadRelationsMetaInfos()
    {
        static bool _bInitialized = false;
        if( _bInitialized == false )
        {
            ReadRelationTypes();
            _bInitialized = true;
        }
    }
 
	RelationType::Direction GetRelationTypeDirection(CGetRelationsMetaInfoCommand::EnumDirection direction) 
	{
        switch (direction)
        {
            case CGetRelationsMetaInfoCommand::EnumDirection_FORWARD:  
                return RelationType::DIR_FORWARD;
            case CGetRelationsMetaInfoCommand::EnumDirection_BACKWARD: 
                return RelationType::DIR_BACKWARD;
            case CGetRelationsMetaInfoCommand::EnumDirection_BIDIRECTIONAL: 
                return RelationType::DIR_BIDIRECTION;
 		    default: 
                return RelationType::DIR_INVALID;
        }
	}

	RelationType::Direction GetRelationTypeDirection(uint8_t direction) 
    {
        return GetRelationTypeDirection(CGetRelationsMetaInfoCommand::EnumDirection(direction));
    }

   	CGetRelationsMetaInfoCommand::EnumDirection GetRelationDirection( RelationType::Direction direction )
    {
        switch (direction)
        {
            case RelationType::DIR_FORWARD:  
                return CGetRelationsMetaInfoCommand::EnumDirection_FORWARD;
            case RelationType::DIR_BACKWARD: 
                return CGetRelationsMetaInfoCommand::EnumDirection_BACKWARD;
            case RelationType::DIR_BIDIRECTION: 
                return CGetRelationsMetaInfoCommand::EnumDirection_BIDIRECTIONAL;
            default:
                assert(0 && "missing enum mapping");
                return CGetRelationsMetaInfoCommand::EnumDirection_FORWARD;
        }
    }


	RelationTypeId GetRelationTypeId( const string & typeId )
	{
        _ReadRelationsMetaInfos();
        RelationTypeMap::iterator iter = _mapRelationTypes.find( typeId );
        if ( iter != _mapRelationTypes.end() ) 
        {
            return iter->second;
        }
        else
        {
            return NULL_RELATION_TYPE_ID();
        }
	}
   	const char * GetRelationType( RelationTypeId typeId )
    {
        _ReadRelationsMetaInfos();
        for( RelationTypeMap::iterator iter = _mapRelationTypes.begin(); 
            iter != _mapRelationTypes.end(); 
            iter++ )
        {
            if ( iter->second == typeId )
                return (iter->first).c_str();
        }
        return (const char *)0;
    }

}} // namespace stc::framework

/*
these are sample types, the function above gets these from the right places, so no worries here

	typedef stc::framework::RelationTypeIdT<0x00000001> ParentChild;
	typedef stc::framework::RelationTypeIdT<0x00000002> ResultChild;
	typedef stc::framework::RelationTypeIdT<0x00000003> CommandStatusChild;
	typedef stc::framework::RelationTypeIdT<0x00000004> CompositeChild;
	typedef stc::framework::RelationTypeIdT<0x00000005> PropertyChild;

	typedef stc::framework::RelationTypeIdT<0x01000001> PhysicalLogical;
	typedef stc::framework::RelationTypeIdT<0x01000002> NetworkEndPoint;
	typedef stc::framework::RelationTypeIdT<0x01000003> ProtocolChild;
	typedef stc::framework::RelationTypeIdT<0x01000004> ExternalInterface;
	typedef stc::framework::RelationTypeIdT<0x01000005> AffiliationPort;


relationType {	
						ParentChild
						PhysicalLogical 
						NetworkEndPoint 
						ProtocolChild 
						ExternalInterface 
						AffiliationPort
					}

relation {
					type:	AffiliationPort			name: Affiliation
					type:	CompositeChild		name: ParentComponent
					type:	CompositeChild		name: ParentTestable

					type:	ParentChild			name: BlockModifier
					type:	ParentChild			name: NetworkInterface
					type:	ParentChild			name: PhysicalPort
					type:	ParentChild			name: ParentPhysicalChassis
					type:	ParentChild			name: ParentPhysicalChassisManager
					type:	ParentChild			name: ParentPort
					type:	ParentChild			name: ParentPortGroup
					type:	ParentChild			name: ParentProject
					type:	ParentChild			name: ParentStcSystem
					type:	ParentChild			name: ParentTestable
					type:	ParentChild			name: ParentTestModule
					type:	ParentChild			name: ResultParentTestable
					type:	ParentChild			name: VirtualInterface
					type:	ParentChild			name: VlanStack

					type:	PhysicalLogical		name: PhysicalPort
					type:	PropertyChild			name: PropertyChild
					type:	ResultChild			name: ResultParentPort
					type:	ResultChild			name: ResultParentTestable
			}
*/

