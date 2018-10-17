#ifndef INCLUDED_BASE_VALIDATION_CONTEXT_H
#define INCLUDED_BASE_VALIDATION_CONTEXT_H

#include "Scriptable.h"
#include "StcExport.h"

#include <sstream>

#define MAX_LOG_USER_SIZE 1024
#define STC_ADD_VALIDATION_ERROR_RES(ctx, MSG_RES_ID, LOG_MSG_EVENT) \
    if (1) { \
        stc::framework::ResourceFormatter XXXLOGGING_formatter(MSG_RES_ID); \
        XXXLOGGING_formatter << LOG_MSG_EVENT; \
        std::ostringstream ctxxstr; \
        ctxxstr << __FILE__ << __LINE__; \
        ctx.AddValidationError(ctxxstr.str(), XXXLOGGING_formatter.ToString()); \
    } else (void)0

#define STC_ADD_VALIDATION_WARNING_RES(ctx, MSG_RES_ID, LOG_MSG_EVENT) \
    if (1) { \
        stc::framework::ResourceFormatter XXXLOGGING_formatter(MSG_RES_ID); \
        XXXLOGGING_formatter << LOG_MSG_EVENT; \
        std::ostringstream ctxxstr; \
        ctxxstr << __FILE__ << __LINE__; \
        ctx.AddValidationWarning(ctxxstr.str(), XXXLOGGING_formatter.ToString()); \
    } else (void)0

#define STC_ADD_VALIDATION_ERROR(ctx, msg) \
    if (1) { \
        std::ostringstream ooostr;  \
        ooostr << msg; \
        std::ostringstream ctxxstr; \
        ctxxstr << __FILE__ << __LINE__; \
        ctx.AddValidationError(ctxxstr.str(), ooostr.str()); \
    } else (void)0

#define STC_ADD_VALIDATION_WARNING(ctx, msg) \
    if (1) { \
        std::ostringstream ooostr;  \
        ooostr << msg; \
        std::ostringstream ctxxstr; \
        ctxxstr << __FILE__ << __LINE__; \
        ctx.AddValidationWarning(ctxxstr.str(), ooostr.str()); \
    } else (void)0


namespace stc {
namespace framework {

/**

 */
class STC_PUBLIC CBaseValidationContext {

public:

    /// \brief Ctor.
    CBaseValidationContext();

    /// \brief Dtor.
    virtual ~CBaseValidationContext();

    /// \brief Sets the current object the command is validating.
    virtual void SetCurrentObject(stc::framework::CScriptable& obj);

    /// \brief Returns the current object the command is validating.
    virtual stc::framework::CScriptable& GetCurrentObject() const;

    /// \brief Sets the current validation category.
    virtual void SetCurrentCategory(const std::string& cate);

    /// \brief Returns the current validation category.
    virtual std::string GetCurrentCategory() const;

    /// \brief Adds a validation error.
    virtual void AddValidationError(const std::string& msgCtx, const std::string& msg);

    /// \brief Adds a validation warning.
    virtual void AddValidationWarning(const std::string& msgCtx, const std::string& msg);

    /// \brief Sets whether to generate active event for validation errors/warnings.
    virtual void SetGenerateActiveEvent(bool generateActiveEvent);

    /// \brief Returns all the messages added since the last Clear, separated
    ///        by newlines.
    std::string GetAllValidationMessages();

    /// \brief Clears all Validation Errors (to be used by unit tests)
    void ClearPreviousValidationEvent();

protected:

    void AddValidationEvent(
        const std::string& msgCtx,
        const std::string& msg,
        bool isWarning);

    stc::framework::CScriptable* m_currentObject;
    std::string m_currentCategory;
    bool m_isFirstEvent;
    bool m_generateActiveEvent;

private:

    // Not implemented.
    CBaseValidationContext(const CBaseValidationContext&);
    CBaseValidationContext& operator= (const CBaseValidationContext&);
};


}
}

#endif // INCLUDED_BASE_VALIDATION_CONTEXT_H

