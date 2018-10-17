#ifndef INCLUDED_RELATION_MANAGER_H
#define INCLUDED_RELATION_MANAGER_H
#pragma once

#include "MetaInfo.h"
#include "StcHandle.h"
#include "StcExport.h"
#include "RelationData.h"

#include <vector>

namespace stc {
namespace framework {

class CScriptable;

/**
	CRelationManager manages both the meta relations between CScriptable classes
	and actual relation instances.  Relation are stored bi-directionally.
*/
class CRelationManager 
{
public:

	/// \brief Returns the singleton instance.
	STC_PUBLIC static CRelationManager& Instance();

	/// \brief Ctor.
	STC_PUBLIC CRelationManager();
	/// \brief Dtor.
	STC_PUBLIC virtual ~CRelationManager();

	/// \brief Validates meta relation info.
	STC_PUBLIC int ValidateMetaInfo() const;
	/// \brief Validates relation according to meta relation inf.
	STC_PUBLIC int ValidateRelation() const;

	/// \brief Adds new meta relation type info.
	virtual int AddMetaInfo(const MetaRelationTypeInfo& meta) = 0;

    /// \brief Adds new meta relation info.
	virtual int AddMetaInfo(const MetaRelationInfo& meta) = 0;

	/// \brief Returns the meta relation.
	/// @param clsId1 is the source class id.
	/// @param clsId2 is the target class id.
	/// @param relation is the relation type.
	virtual const MetaRelationInfo* GetMetaRelation(
		const ClassId clsId1,
		const ClassId clsId2,
		const RelationTypeId& relation) const = 0;

	/// \brief Returns a collection of meta relations associated with the class.
	/// @param clsId is the class we want to query
	/// @param metaInfos is the return meta infos.
	typedef std::vector<const MetaRelationInfo*> MetaInfos;
	virtual void GetClassMetaRelations(
		const ClassId clsId, 
		MetaInfos& metaInfos,
        bool recursive=false) const = 0;

    /// \brief Returns the meta relation type
    /// @param relation is the relation type
    virtual const MetaRelationTypeInfo* GetMetaRelationType(
        const RelationTypeId& relation) const = 0;

    /// \brief Returns the meta relation type
    /// @param relation is the relation type
    virtual const MetaRelationTypeInfo* GetMetaRelationType(
		const std::string& name) const = 0;

	// TODO: support removeMetaInfo?
};

}
}

#endif //INCLUDED_RELATION_MANAGER_H


	// xxx add predicate later
	/*
	int Query(
		HndList& retHndList,
		const Predicate& p) const;

	int Query(
		HndList& retHndList,
		const StcHandle hnd,
		const Predicate& p) const;
	*/

	 
	// attribute value match/compare
	// class name match
	// relation id match + isSrc match (e.g. parent_child (parent or child) + recur or depth?)

	// get all desecendent
	//  relation 
	// d = child's child (specify the depth of this recur)
	//  
	//
	// simple syntax? text base?
	// default == and (no or? too hard to implement
	// Predicate().equalClass(id).and().hasAttribute(attrId).attrib(attrId, "==", "123").
	// matchRelation(PARENT_CHILD, 1st or 2nd, depth)
	// p.evaluate(hnd)
	//
	// efficiency
	// worst = for all if match add to return 
	// 
	// analyze predicate
	//
	//
	//
	// get all rx port
	// 0, p().equalClass(Port).hasRelation(TRAFFIC_RX_PORT, 2nd)  search depth=0
	//
	// get all traffic under this port
	// pHnd, p().equalClass(Traffic).hasRelation(PARENT_CHILD, 2nd) depth=1
	//
	// get all router under port 1
	// get all bgp router
	// get all tx port

	// how to make recursive part of the definition?
	// get child vs get desce
	// isTxPort = "classId == AAA and cnt(child traffic) > 0"

	// query all with type


	//const Predicate * pred = 0,
	// query child
	// query ancestor
	// is there any need to query across relations?
	//query(testHnd, CLASS_Traffic); // we will only do recursive search for parent/child?
	//query(CLASS_Port, isTxPort);
	//query(CLASS_Router) //base class
	//query all nodes
	// iterate through all nodes
	//predicate txPort(CLASS_Port, CLASS_Traffic, "parent/child", GREATER_THAN, 0)
	//predicate linkupPort(CLASS_Port, ATTR_ucLinkStatus, EQUAL_TO, "1")
	//predicate txAndLinkupPort(txPort, AND, linkupPort)
	//query(all, bgp_session, bgpUp)

