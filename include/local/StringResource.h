#ifndef RESOURCE_CONST_H
#define RESOURCE_CONST_H


#include "ResourceManager.h"
#include "frameworkConst.h"


namespace stc {
namespace framework {

/* define the unique range of values to be used
    for logging messages. To guaranty the uniqueness,
    the first key in this ResourceKey must be the 
    plug-in ID obtained from the project constant file. */
class CResourceKey 
{
public:
    /* define the keys for logging messages */
    typedef enum {
        UNITTEST_MESSAGE = FRAMEWORK_Plugin_ID,

        TEST_RESOURCEMANAGER_ERROR,
        TEST_RESOURCEMANAGER_WARN,
        TEST_RESOURCEMANAGER_INFO,
        TEST_RESOURCEMANAGER_DEBUG,

        DB_IMPORT_UNKNOWN_CLASS_TABLE_1,
        DB_IMPORT_UNKNOWN_CLASS_PROPERTY_2,

        DB_EXPORT_APPEND_WITH_INCOMPATIBLE_CLASS,

        TASK_COMMAND_FAILED,
        TASK_COMMAND_FAILED_UNKNOWN_ERROR,

        IMPORT_ENCOUNTER_UNKNOWN_ERROR_WARN_RESET,

        MEMORY_GUARD_LOW_VIRTUAL_MEMORY,

        BLL_TERMINATE_REQUEST_CANCELLED_BY_USER,
        BLL_TERMINATE_REQUESTED_BY_USER,
        
        LIC_INTERNAL_ERROR,
        LIC_CHECKOUT_ERROR,
        LIC_UNSUPPORTED_FEATURE,
        LIC_RESTART_REQUIRED,
        LIC_SERVER_CONNECT_FAILED,
        LIC_VER_NOT_SUPPORTED,

    } MESSAGE_ID;
};

class FrameworkResourceRegisterer 
{
public:

    /* register the logging messages to the resource manager */
    FrameworkResourceRegisterer()
    {
        stc::framework::CResourceManager& rm = stc::framework::CResourceManager::Instance();

        // unittest
        rm.AddStringResource(CResourceKey::UNITTEST_MESSAGE, "some message '{2}' is a test number {1} of {1}. Cannot find {3}");
        rm.AddStringResource(CResourceKey::TEST_RESOURCEMANAGER_ERROR, "This error number {1} has message \'{2}\'");
        rm.AddStringResource(CResourceKey::TEST_RESOURCEMANAGER_WARN, "The message: {1}");
        rm.AddStringResource(CResourceKey::TEST_RESOURCEMANAGER_INFO, "The Info message");
        rm.AddStringResource(CResourceKey::TEST_RESOURCEMANAGER_DEBUG, "The Debug message");

        rm.AddStringResource(CResourceKey::TASK_COMMAND_FAILED, "{1} failed with error: {2}." );
        rm.AddStringResource(CResourceKey::TASK_COMMAND_FAILED_UNKNOWN_ERROR, "{1} failed with unknown error." );

        rm.AddStringResource(CResourceKey::DB_IMPORT_UNKNOWN_CLASS_TABLE_1, "Skipping unknown class table {1}" );
        rm.AddStringResource(CResourceKey::DB_IMPORT_UNKNOWN_CLASS_PROPERTY_2, "Skipping unused class property {1}:{2} " );

        rm.AddStringResource(CResourceKey::DB_EXPORT_APPEND_WITH_INCOMPATIBLE_CLASS, 
            "{1} contains a preexisting table {2} that is incompatible with the current version; "
            "please select a different database file instead." );

        rm.AddStringResource(CResourceKey::IMPORT_ENCOUNTER_UNKNOWN_ERROR_WARN_RESET, 
            "Encountered error(s) while loading the configuration, "
            "resetting the configuration back to the default. Error: {1}");

        rm.AddStringResource(CResourceKey::MEMORY_GUARD_LOW_VIRTUAL_MEMORY,
            "Virtual memory available to Spirent TestCenter is running low, there may be performance issues");

        rm.AddStringResource(CResourceKey::BLL_TERMINATE_REQUEST_CANCELLED_BY_USER, 
            "Test Session terminate is cancelled by {1}");
        rm.AddStringResource(CResourceKey::BLL_TERMINATE_REQUESTED_BY_USER, 
            "Test Session termination is requested by {1}");

        rm.AddStringResource(CResourceKey::LIC_INTERNAL_ERROR, 
            "Internal license error occurred.");
        rm.AddStringResource(CResourceKey::LIC_UNSUPPORTED_FEATURE, 
            "License server does not support feature {1}.");
        rm.AddStringResource(CResourceKey::LIC_CHECKOUT_ERROR, 
            "License checkout for feature {1} failed with error {2}.");
        rm.AddStringResource(CResourceKey::LIC_RESTART_REQUIRED,
            "Application must be restarted for the license server configuration changes to take effect");
        rm.AddStringResource(CResourceKey::LIC_SERVER_CONNECT_FAILED, 
            "Failed to connect to a license server.  Please check that the SPIRENTD_LICENSE_FILE environment variable is set to a valid license server address(e.g. SPIRENTD_LICENSE_FILE=@MyServer).");
        rm.AddStringResource(CResourceKey::LIC_VER_NOT_SUPPORTED, 
            "Software is not licensed to use this application date version {1}.  Please obtain necessary support services license and install.");
    };

};

}
}

#endif //RESOURCE_CONST_H

