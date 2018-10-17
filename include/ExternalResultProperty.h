#ifndef INCLUDED_FRAMEWORK_EXTERNALRESULTPROPERTY_H
#define INCLUDED_FRAMEWORK_EXTERNALRESULTPROPERTY_H
#pragma once

#include "ExternalProperty.h"
#include "ExternalResultPropertyContainer.h"

namespace stc {
namespace framework {

class STC_PUBLIC CExternalResultProperty : public CExternalProperty {

	//
	// Add your code here.
	// Ctor and Dtor are declared as protected;
	// use CScriptableCtor and MarkDelete to create new/delete instances.
	//
public:
    /// Group operation information
    struct GroupOp
    {
        bool isCustom; ///< is custom or not?
        std::string groupAction;  ///< group Action
        //ctor
        GroupOp() { isCustom=false; }
        GroupOp(bool custom, const std::string& action) { isCustom=custom; groupAction=action;}
    };
    typedef std::map<std::string, GroupOp> GroupDefMap ;

    enum PropertySource { SRC_CONFIG, SRC_SRSA, SRC_RRSA, SRC_RESULT } ;

    static const std::string DEFAULT_GROUP_NAME ;
    
    ///////////////////////////////////////////////////////////////////////////
    // Pure virtuals
    ///////
    virtual bool IsBllProperty() const = 0 ;
    virtual bool IsEquationProperty() const = 0 ;
    virtual std::string GetEquationString() const = 0 ;

    ///////////////////////////////////////////////////////////////////////////
    // Common External Result Utilities
    //////
    /// \brief Method to get the SQL compatible group OP given the group key
    GroupOp GetGroupOp(const std::string &groupKey) const ;

    /// \brief Set the parent container for the external result property.
    void SetContainer ( CExternalResultPropertyContainer *container) { mContainer = container ; }

    /// \brief Get the parent container
    CExternalResultPropertyContainer *GetContainer(void) const { return mContainer; }

    /// \brief Set the result source container for the external result property.
    void SetResultSourceContainer ( CExternalResultPropertyContainer *resultSourceContainer) { mResultSourceContainer = resultSourceContainer ; }

    /// \brief Get the result source container
    CExternalResultPropertyContainer *GetResultSourceContainer(void) const { return mResultSourceContainer; }

    /// \brief Set the source of the property
    void SetPropertySource( PropertySource source ) { mPropSource = source ; }

    /// \brief Get the Property Source
    PropertySource GetPropertySource( void ) const { return mPropSource ; }

    /// \brief Get query to use with DB
    const std::string & GetDbQuery() const { return mDbQuery ; }

    /// \brief Set query to use with DB
    void SetDbQuery(const std::string &qry) { mDbQuery = qry ; }

    /// \brief Get query to use with BLL master DB
    const std::string& GetMasterDbQuery() const { return mMasterDbQuery ; }

    /// \brief Set query to use with BLL master DB
    void SetMasterDbQuery(const std::string& qry) { mMasterDbQuery = qry ; }

    /// \brief Get query to use with IL DB
    const std::string& GetIlDbQuery() const { return mIlDbQuery ; }

    /// \brief Set query to use with IL DB
    void SetIlDbQuery(const std::string& qry) { mIlDbQuery = qry ; }

    /// \brief Get supporting properties that the current property depends upon
    void GetSupportingProps(std::set< const CExternalResultProperty * > &props) { props = mSupportProps ; }

    /// \brief Set supporting properties that the current property depends upon
    void SetSupportingProps(const std::set< const CExternalResultProperty * > &props) { mSupportProps = props ; }

protected:
	// Construct through ScriptableCreator instead.
	CExternalResultProperty();
	CExternalResultProperty(const CExternalResultProperty&);
	CExternalResultProperty& operator = (const CExternalResultProperty&);
	virtual ~CExternalResultProperty();

    ///////////////////////////////////////////////////////////////////////////
    // Scriptable method overrides
    ///////////
    virtual bool OnLoadCompleted() ;
    virtual bool CanSave() const ; 
	virtual bool OnInit();
	virtual void OnPropertyModified(Property& prop);
    ///////////////////////////////////////////////////////////////////////////

    bool mIsLoaded ;
    GroupDefMap mGroups ;
    CExternalResultPropertyContainer *mContainer ;
    CExternalResultPropertyContainer *mResultSourceContainer;   // this property should be the same as mContainer except for the case where the property is coming from RSA (Port, Device)
    PropertySource mPropSource ;
    std::string mDbQuery ;
    std::string mMasterDbQuery ;
    std::string mIlDbQuery ;
    std::set< const CExternalResultProperty * > mSupportProps ;

private:
	friend class CExternalResultPropertyCtorDtorHelper;

    typedef std::set<std::string> SupportedSqlOps ;
    typedef std::set<std::string> SrsaKeys ;

    static SupportedSqlOps mSqlOps ;
    static SrsaKeys        mSrsaGrpKeys ;

    std::string mDebugEffectivePropName;    // for debug purpose only

// Keep this include as the last line of the class!!!
#include "ExternalResultProperty_AutoGen.h"
};

}
}

#endif //INCLUDED_FRAMEWORK_EXTERNALRESULTPROPERTY_H

