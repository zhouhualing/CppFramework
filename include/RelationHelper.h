#ifndef INCLUDED_RELATION_HELPER_H
#define INCLUDED_RELATION_HELPER_H

#include "StcHandle.h"
#include "HandleMap.h"
#include "Scriptable.h"
#include "StcExport.h"
#include <vector>

class UnitTestRelationHelper;


namespace stc {
namespace framework {

/**
	Some common typedefs when working with CScriptable
 */

/// \the pair is the relation typename and relation string handle
typedef std::vector<std::pair<std::string, std::string> > RelationPairVector;

/// \the pair is the relation typename and relation string handle
typedef std::pair<std::string, std::string> RelationPair;

/** 
	A object to process the relations
 */
class CRelationHelper
{
public:

	/// \brief Returns the singleton instance.
	STC_PUBLIC static CRelationHelper & Instance();

	/// \brief Dtor.
	~CRelationHelper();

	/// \ get object's children 
	STC_PUBLIC void GetChildren(CScriptable* pTarget, ClassId classId, RelationPair & pair);
	/// \ get object's parent 
	STC_PUBLIC void Getparent(CScriptable* pTarget, RelationPair & pair);
	/// \ get object's relation only base on the relation typename  
	STC_PUBLIC void GetTheRelation(CScriptable* pTarget, const std::string& typeName, RelationPair & pair);
	/// \ validdate the relation type name  
	STC_PUBLIC bool IsValidRealtionName(CScriptable* pTarget, const std::string& typeName);
	/// \ add a relation to the scriptable object  
	STC_PUBLIC bool AddRelation(CScriptable* pTarget, const std::string& typeName, const std::vector <StcHandle>& handleVec, std::string & errorMsg);
	/// \ to retrive all the relations from the object  
	STC_PUBLIC void GetAllRelations(CScriptable* pTarget, RelationPairVector & outputPair);
	
	///\* 1. validate the side name --- only use the side name to config and get */  
	///\* 2. validate the releation type name and direction      */
	STC_PUBLIC bool IsValidSideName(CScriptable* pTarget, const std::string& sideName, RelationPair & pair);

private:
	/// \ get object's relation base on the relation typename and classid 
	void GetRelation(CScriptable* pTarget, const std::string& typeName, ClassId classId, 
					const RelationType& relationType, RelationPairVector & outputPair);
    /// \ combine duplicate relation types, remove duplicate relation values
    STC_PUBLIC static void ConsolidateRelationVector(RelationPairVector& target, const RelationPairVector& source);
	bool IsValidObjectType(const std::string& type);
	bool IsValidObjectType4Pdu(CScriptable* pTarget, const std::string& type);
	std::string GetStringHandle(CScriptable* pTarget);
	ClassId GetClassId(const std::string& type);

    friend class ::UnitTestRelationHelper;
};

}
}

#endif // INCLUDED_RELATION_HELPER_H

