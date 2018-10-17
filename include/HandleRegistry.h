#ifndef INCLUDED_HANDLE_REGISTRY_H
#define INCLUDED_HANDLE_REGISTRY_H
#pragma once

#include "StcHandle.h"
#include "StcExport.h"
#include "StcException.h"
#include "StcCollections.h"

#include <sstream>
#include <vector>
#include <list>
#include <set>
#include <deque>

// Handle must be int

class UnitTestHandleRegistry;

namespace stc {
namespace framework {

/** 
    A singleton registry that maintains the StcHandle to CScriptable mapping.
 */
class CHandleRegistry
{
public:

    /// \brief Returns the singleton instance.
    STC_PUBLIC static CHandleRegistry& Instance();

    /// \brief Dtor.
    ~CHandleRegistry();

private:
    /// \brief Clears the registry mappings.
    STC_PUBLIC void ClearAll();
    
    /// \brief Adds the CScriptable and returns its handle.
    STC_PUBLIC const StcHandle Add(CScriptable* pItem);

    /// \brief Adds the CScriptable with user specified handle and returns its handle.
    STC_PUBLIC const StcHandle Add(CScriptable* pItem, const StcHandle key);

    /// \brief Removes the handle from the registry. 
    STC_PUBLIC CScriptable* Remove(const StcHandle key);

    /// \brief Returns the number of free handles.
    STC_PUBLIC int GetFreeHandleCount() const;

public:

    /// \brief Finds the CScriptable with the handle; returns NULL if not found.
    STC_PUBLIC CScriptable* Find(const StcHandle key) const;

    /// \brief Gets the CScriptable with the handle; throws CStcInvalidArgument if not found.
    STC_PUBLIC CScriptable* Get(const StcHandle key) const;

    /// \brief Finds multiple CScriptable; return false if some are not found.
    template <typename SrcCollection, typename DstCollection>
    bool Find(const SrcCollection& src, DstCollection& dst) const
    {
        for (typename SrcCollection::const_iterator it = src.begin(); it != src.end(); ++it)
        {
            typedef typename DstCollection::value_type T;
            T s = dynamic_cast<T>(Find(*it));
            if (s == 0)
                return false;

            dst.push_back(s);
        }
        return true;
    }

    /// \brief Finds multiple CScriptable; return error handle list for not found.
    template <typename SrcCollection, typename DstCollection >
    bool FindAllValid(const SrcCollection& src, DstCollection& dst, std::string& err) const
    {
        bool status = true;
        std::ostringstream os; 
        for (typename SrcCollection::const_iterator it = src.begin(); it != src.end(); ++it)
        {
            typedef typename DstCollection::value_type T;
            T s = dynamic_cast<T>(Find(*it));
            if (s == 0)
            {
                if(!status)
                {
                    os << ", ";
                }
                os << (*it);
                status = false;
                continue;
            }
            dst.push_back(s);
        }
        err = os.str();
        return status;
    }

    /// \brief Gets multiple CScriptable; throws CStcInvalidArgument if not found.
    template <typename SrcCollection, typename DstCollection>
    void Get(const SrcCollection& src, DstCollection& dst) const
    {
        for (typename SrcCollection::const_iterator it = src.begin(); it != src.end(); ++it)
        {
            typedef typename DstCollection::value_type T;
            T s = dynamic_cast<T>(Find(*it));
            if (s == 0)
                throw stc::framework::CStcInvalidArgument("Invalid HandleRegistry Key");

            dst.push_back(s);
        }
    }

    /// \brief Returns the number of handles in used.
    STC_PUBLIC int GetInUseHandleCount() const;

    /// \brief Returns the handles that are currently in use.
    STC_PUBLIC int GetInUseHandles(HandleVec& hndVec) const;


    /// \brief Generates handle block for future use.  All handle values are sequential.
    ///        Note: once a generated handles have been removed, handle registry will relcaim it.
    STC_PUBLIC StcHandle GenerateHandleBlock(int count);

    /// \brief Release unused handles allocated from GenerateHandles.
    STC_PUBLIC void ReleaseUnusedHandleBlock(const StcHandle startHnd, const int count);
    
    /// \brief Sets the recycle unused handle threshold.
    STC_PUBLIC void SetRecycleUnusedHandleThreshold(int threshold);

    /// \brief Returns the recycle unused handle threshold.
    STC_PUBLIC int GetRecycleUnusedHandleThreshold() const { return mRecycleThreshold; } 

private:

    /// \brief Allocats an unused handle.
    const StcHandle AllocateHandle();

    ScriptableVec mItemVec;
    std::deque<StcHandle> mReclaimedHandles;
    bool mReuseOldHnd;
    int mInUseHandleCount;
    int mReservedHandleCount;
    int mRecycleThreshold;

    CHandleRegistry();
    CHandleRegistry(const CHandleRegistry&);
    CHandleRegistry& operator= (const CHandleRegistry&);

    friend class ::UnitTestHandleRegistry;
    friend class CScriptable;
};

}
}

#endif // INCLUDED_HANDLE_REGISTRY_H

