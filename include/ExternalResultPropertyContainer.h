#ifndef _INC_EXTERNAL_RESULT_PROPERTY_CONTAINER_H
#define _INC_EXTERNAL_RESULT_PROPERTY_CONTAINER_H

#include "StcExport.h"
#include <string>

namespace stc {
namespace framework {

/**
    Simple, abstract interface class for external
    result property container classes
*/
class STC_PUBLIC CExternalResultPropertyContainer
{
public:
    enum ContainerTypes
    {
        CONFIG,
        RESULT,
        EQUATION
    } ;

    /// \brief dtor.
    virtual ~CExternalResultPropertyContainer() {}
    /// \brief Get Container type
    virtual ContainerTypes GetContainerType(void) const = 0 ;
    /// \brief Get the container name
    virtual std::string GetContainerTypeName(void) const = 0 ;
    /// \brief Get the container name
    virtual std::string GetContainerName(void) const = 0 ;
} ;

}
}
#endif //_INC_EXTERNAL_RESULT_PROPERTY_CONTAINER_H

