#ifndef INCLUDED_NOTIFICATION_SERVICE_H
#define INCLUDED_NOTIFICATION_SERVICE_H

#include "StcExport.h"
#include "TaskManager.h"
#include "Scriptable.h"
#include <set>


class UnitTestUserDefinedDefaults;

namespace stc {
namespace framework {

/////////////////////////////////////////////////////////////////////////////////////////////
///
///    Service responsible for sending notification events to clients of the BLL.  This is the
///    long version of the class description.
///
class CNotificationService
    :
    public stc::framework::CRunnable
{
    public:
        ~CNotificationService();
        
        STC_PUBLIC static CNotificationService& Instance( void );
        
        STC_PUBLIC bool InstantEvent( EventType eType, CScriptable* pScriptable, const Property* pProperty = NULL );
        STC_PUBLIC bool WriteEvent( EventType eType, CScriptable* pScriptable, const Property* pProperty = NULL );
        STC_PUBLIC void FlushBatchedEvents( void );
        STC_PUBLIC bool WriteInitCreateEvent( const CScriptable* pScriptable, const CScriptable* pParent );

        STC_PUBLIC bool RegisterCallback( CScriptable* pScriptable );
        STC_PUBLIC void UnregisterCallback( CScriptable* pScriptable );
        
        STC_PUBLIC void BlockEvents( const StcHandle& hScriptable );
        STC_PUBLIC void UnblockEvents( const StcHandle& hScriptable );

        STC_PUBLIC void BroadcastSessionStart();
        STC_PUBLIC void BroadcastSessionStop();

        class ScopedUpdateDisabler 
        {
        public:
            ScopedUpdateDisabler() { CNotificationService::Instance().BroadcastSessionStop(); }
            ~ScopedUpdateDisabler() { CNotificationService::Instance().BroadcastSessionStart(); }
        private:
            ScopedUpdateDisabler(const ScopedUpdateDisabler&);
            ScopedUpdateDisabler& operator = (const ScopedUpdateDisabler&);
        };


    protected:
        virtual void DoRun();

    private:
        typedef std::set< CScriptable* >    CALLBACK_SET;
        typedef std::set< StcHandle >        BLOCK_SET;
        CALLBACK_SET                        m_CallbackSet;
        BLOCK_SET                            m_BlockedSet;
        int                                    m_WriteIndex;
        int                                    m_SendIndex;
        int                                    m_TicksSinceLastUpdate;
        bool                                m_IsUpdating;
        typedef std::vector< CMessage >        MessageVec;
        MessageVec                            m_BatchMessageVec;
        
        CNotificationService();
        bool IsBlocked( StcHandle h )
            { return m_BlockedSet.find( h ) != m_BlockedSet.end(); }
        void DoUpdate( void );

        friend class ::UnitTestUserDefinedDefaults;
};
}
}

#endif

