#ifndef INCLUDED_STC_EXCEPTION_H
#define INCLUDED_STC_EXCEPTION_H

#ifdef WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4251) // template element needs to have dll-interface
#endif

#include "StcExport.h"
#include <string>
#include <stdexcept>
#include <cassert>


#ifdef _DEBUG
#define STC_ASSERT( COND, ASSERT_MSG )     assert( COND )
#else
#define STC_ASSERT( COND, ASSERT_MSG ) \
    if ( !(COND) ) { \
        std::ostringstream ooostr;    \
        ooostr    << "Assertion Failed: " << ASSERT_MSG \
                << " Expression: " << #COND \
                << " File: " << __FILE__ \
                << " Line: " << __LINE__; \
        throw stc::framework::CStcLogicError( ooostr.str(), true ); \
    } else (void)0
#endif

class UnitTestException;

namespace stc {
namespace framework {

/*
    The StcExceptions mimics the std::exception hierarchy.
*/

/**
    The base class for StcException.
*/
class STC_PUBLIC CStcException : public std::exception {
public:
    /// \brief Empty ctor.
    CStcException();
    /// \brief Error string ctor.
    CStcException(const std::string& msg, bool logAsError = true);
        
    /// \brief dtor.
    virtual ~CStcException() throw() {}

    /// \brief Return exception reason.
    virtual const char* what() const throw () { 
        m_Msg = GetExceptionName();
        m_Msg += " " + m_UserMsg;
        return m_Msg.c_str();
    }

    /// \brief Returns the user message string.
    const char* GetUserMessage() const { return m_UserMsg.c_str(); }

protected:
    /// \brief Return exception name.
    virtual const char* GetExceptionName() const { return "CStcException"; }

    mutable std::string m_Msg;    ///< exception string
    std::string m_UserMsg;    ///< user exception string

    friend class ::UnitTestException;
};

/**
    Run time error exception.
*/
class STC_PUBLIC CStcRunTimeError : public CStcException {
public:
    /// \brief Empty ctor.
    CStcRunTimeError(){}
    /// \brief Error string ctor.
    CStcRunTimeError(const std::string& msg, bool logAsError = true)
        :CStcException(msg, logAsError)    {}
    /// \brief dtor.
    virtual ~CStcRunTimeError() throw() {}

protected:
    /// \brief Return exception name
    virtual const char* GetExceptionName() const { return "CStcRunTimeError"; }
};

/**
    Logic error exception.
*/
class STC_PUBLIC CStcLogicError : public CStcException {
public:
    /// \brief Empty ctor
    CStcLogicError() {}
    /// \brief Error string ctor
    CStcLogicError(const std::string& msg, bool isFatal = false);
    /// \brief dtor
    virtual ~CStcLogicError() throw() {}

protected:
    /// \brief Return exception name
    virtual const char* GetExceptionName() const { return "CStcLogicError"; }
};

/**
    Bad cast error exception.
*/
class STC_PUBLIC CStcBadCast : public CStcException {
public:
    /// \brief Empty ctor
    CStcBadCast(){}
    /// \brief Error string ctor
    CStcBadCast(const std::string& msg)
        :CStcException(msg)    {}
    /// \brief dtor
    virtual ~CStcBadCast() throw() {}

protected:
    /// \brief Return exception name
    virtual const char* GetExceptionName() const { return "CStcBadCast"; }
};

/**
    Range error exception.
*/
class STC_PUBLIC CStcRangeError : public CStcRunTimeError {
public:
    /// \brief Empty ctor
    CStcRangeError(){}
    /// \brief Error string ctor
    CStcRangeError(const std::string& msg)
        :CStcRunTimeError(msg)    {}
    /// \brief dtor
    virtual ~CStcRangeError() throw() {}

protected:
    /// \brief Return exception name
    virtual const char* GetExceptionName() const { return "CStcRangeError"; }
};

/**
    Overflow error exception.
*/
class STC_PUBLIC CStcOverFlowError : public CStcRunTimeError {
public:
    /// \brief Empty ctor
    CStcOverFlowError(){}
    /// \brief Error string ctor
    CStcOverFlowError(const std::string& msg)
        :CStcRunTimeError(msg)    {}
    /// \brief dtor
    virtual ~CStcOverFlowError() throw() {}

protected:
    /// \brief Return exception name
    virtual const char* GetExceptionName() const { return "CStcOverFlowError"; }
};

/**
    Underflow error exception.
*/
class STC_PUBLIC CStcUnderflowError : public CStcRunTimeError {
public:
    /// \brief Empty ctor
    CStcUnderflowError(){}
    /// \brief Error string ctor
    CStcUnderflowError(const std::string& msg)
        :CStcRunTimeError(msg)    {}
    /// \brief dtor
    virtual ~CStcUnderflowError() throw() {}

protected:
    /// \brief Return exception name
    virtual const char* GetExceptionName() const { return "CStcUnderflowError"; }
};

/**
    Length error exception.
*/
class STC_PUBLIC CStcLengthError : public CStcLogicError {
public:
    /// \brief Empty ctor
    CStcLengthError(){}
    /// \brief Error string ctor
    CStcLengthError(const std::string& msg)
        :CStcLogicError(msg){}
    /// \brief dtor
    virtual ~CStcLengthError() throw() {}

protected:
    /// \brief Return exception name
    virtual const char* GetExceptionName() const { return "CStcLengthError"; }
};

/**
    Domain error exception.
*/
class STC_PUBLIC CStcDomainError : public CStcLogicError {
public:
    /// \brief Empty ctor
    CStcDomainError(){}
    /// \brief Error string ctor
    CStcDomainError(const std::string& msg)
        :CStcLogicError(msg){}
    /// \brief dtor
    virtual ~CStcDomainError() throw() {}

protected:
    /// \brief Return exception name
    virtual const char* GetExceptionName() const { return "CStcDomainError"; }
};

/**
    Out of range error exception.
*/
class STC_PUBLIC CStcOutOfRange : public CStcLogicError {
public:
    /// \brief Empty ctor
    CStcOutOfRange(){}
    /// \brief Error string ctor
    CStcOutOfRange(const std::string& msg)
        :CStcLogicError(msg){}
    /// \brief dtor
    virtual ~CStcOutOfRange() throw() {}

protected:
    /// \brief Return exception name
    virtual const char* GetExceptionName() const { return "CStcOutOfRange"; }
};

/**
    Invalid argument error exception.
*/
class STC_PUBLIC CStcInvalidArgument : public CStcLogicError {
public:
    /// \brief Empty ctor
    CStcInvalidArgument(){}
    /// \brief Error string ctor
    CStcInvalidArgument(const std::string& msg)
        :CStcLogicError(msg){}
    /// \brief dtor
    virtual ~CStcInvalidArgument() throw() {}

protected:
    /// \brief Return exception name
    virtual const char* GetExceptionName() const { return "CStcInvalidArgument"; }
};


/**
    Abort task exception.
*/
class STC_PUBLIC CAbortTask : public CStcException {
public:
    /// \brief Empty ctor
    CAbortTask(){}
    /// \brief Error string ctor
    CAbortTask(const std::string& msg)
        :CStcException(msg){}
    /// \brief dtor
    virtual ~CAbortTask() throw() {}

protected:
    /// \brief Return exception name
    virtual const char* GetExceptionName() const { return "CAbortTask"; }
};


}
}

#endif // INCLUDED_STC_EXCEPTION_H

