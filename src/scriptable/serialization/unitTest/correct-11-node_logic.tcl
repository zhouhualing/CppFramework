# Spirent TestCenter Logic Script
# Generated on Wed Aug 12 10:50:31 2009 by bandrews
# Framework ver. w.xx.yyy.zzzz
#
# Comments: 
# 
#
# This logic script contains the following routines invoked from the
# test-loaded-from-xml.tcl script.

# Load Spirent TestCenter
package require SpirentTestCenter
#source SpirentTestCenter.tcl

#    init - set the logging level and logging location (stdout).
#           Possible logLevel values are: 
#             DEBUG - Display DEBUG, INFO, WARN, and ERROR messages
#             INFO  - Display INFO, WARN, and ERROR messages
#             WARN  - Display WARN and ERROR messages
#             ERROR - Display only ERROR messages
#
#           Possible values for logTo are "stdout" or a file name (can include
#           the path). Use forward slashes between directory names.
proc init {} {
    stc::config automationoptions -logTo stdout -logLevel WARN 
}

#    configResultLocation -  set the location for results files.
#            Possible values are: 
#              INSTALL_DIR - Spirent TestCenter installation directory.
#              CURRENT_WORKING_DIR - Current working directory. This 
#                  is the directory that Spirent TestCenter currently
#                  has open.
#              USER_WORKING_DIR - User working directory.
#              CURRENT_CONFIG_DIR - Current configuration directory. 
#                  This is the directory where the saved or loaded
#                  .xml or .tcc file is located. If no .xml or .tcc 
#                  file has been saved or loaded, files are saved
#                  to the user working directory.
#
#            The location of the results files can be modified in the
#            launcher file. The saveResultsRelativeTo parameter sets a path 
#            that is prepended to the value of the ResultsDirectory 
#            parameter. To set an fully qualified (absolute) path for 
#            results, set the ResultsDirectory parameter and set 
#            SaveResultsRelativeTo to NONE.
proc configResultLocation  { location } {
    set TestResultSetting(1) [lindex [stc::get System1.Project -children-TestResultSetting] 0]
    stc::config $TestResultSetting(1) -saveResultsRelativeTo NONE -resultsDirectory $location
}

#    configMiscOptions - set up the sequencer. Currently sets the sequencer
#                        to stop on any error.  Other options are IGNORE_ERROR and 
#                        PAUSE_ON_ERROR.
proc configMiscOptions {} {
    set Sequencer(1) [lindex [stc::get system1 -children-sequencer] 0]
    stc::config $Sequencer(1) -ErrorHandler STOP_ON_ERROR
}

#    config - load the configuration into memory and set the STC port locations.
#             The port locations can be modified in the launcher file. This 
#             routine contains the majority of the object creation and attribute
#             setting code taken from the GUI configuration. Near the end of 
#             this routine is a set of lines that specifies the protocol stack 
#             linkages for both the stream blocks and any configured protocols. 
#             Following that code is the command sequence construction code.
proc config {portLocations} {
stc::config system1 -IsLoadingFromConfiguration "true"

set Project(1) [stc::create "Project" \
        -property1 "value1" \
        -property2 "value2" \
        -property3 "value3" \
        -property4 "value4" \
        -property5 "value5" \
        -property6 "value6" \
        -property7 "value7" \
        -property8 "value8" \
        -property9 "value9" \
        -property10 "value10" ]

set RFC2544ThroughputTest(1) [stc::create "RFC2544ThroughputTest" \
        -under $Project(1) \
        -property1 "value1" \
        -property2 "value2" \
        -property3 "value3" \
        -property4 "value4" \
        -property5 "value5" \
        -property6 "value6" \
        -property7 "value7" \
        -property8 "value8" \
        -property9 "value9" \
        -property10 "value10" ]

set RFC2544ThroughputTest(2) [stc::create "RFC2544ThroughputTest" \
        -under $Project(1) \
        -property1 "value1" \
        -property2 "value2" \
        -property3 "value3" \
        -property4 "value4" \
        -property5 "value5" \
        -property6 "value6" \
        -property7 "value7" \
        -property8 "value8" \
        -property9 "value9" \
        -property10 "value10" ]

set RFC2544ThroughputTest(3) [stc::create "RFC2544ThroughputTest" \
        -under $Project(1) \
        -property1 "value1" \
        -property2 "value2" \
        -property3 "value3" \
        -property4 "value4" \
        -property5 "value5" \
        -property6 "value6" \
        -property7 "value7" \
        -property8 "value8" \
        -property9 "value9" \
        -property10 "value10" ]

set RFC2544ThroughputTest(4) [stc::create "RFC2544ThroughputTest" \
        -under $Project(1) \
        -property1 "value1" \
        -property2 "value2" \
        -property3 "value3" \
        -property4 "value4" \
        -property5 "value5" \
        -property6 "value6" \
        -property7 "value7" \
        -property8 "value8" \
        -property9 "value9" \
        -property10 "value10" ]

set RFC2544ThroughputTest(5) [stc::create "RFC2544ThroughputTest" \
        -under $Project(1) \
        -property1 "value1" \
        -property2 "value2" \
        -property3 "value3" \
        -property4 "value4" \
        -property5 "value5" \
        -property6 "value6" \
        -property7 "value7" \
        -property8 "value8" \
        -property9 "value9" \
        -property10 "value10" ]

set RFC2544ThroughputTest(6) [stc::create "RFC2544ThroughputTest" \
        -under $Project(1) \
        -property1 "value1" \
        -property2 "value2" \
        -property3 "value3" \
        -property4 "value4" \
        -property5 "value5" \
        -property6 "value6" \
        -property7 "value7" \
        -property8 "value8" \
        -property9 "value9" \
        -property10 "value10" ]

set RFC2544ThroughputTest(7) [stc::create "RFC2544ThroughputTest" \
        -under $Project(1) \
        -property1 "value1" \
        -property2 "value2" \
        -property3 "value3" \
        -property4 "value4" \
        -property5 "value5" \
        -property6 "value6" \
        -property7 "value7" \
        -property8 "value8" \
        -property9 "value9" \
        -property10 "value10" ]

set RFC2544ThroughputTest(8) [stc::create "RFC2544ThroughputTest" \
        -under $Project(1) \
        -property1 "value1" \
        -property2 "value2" \
        -property3 "value3" \
        -property4 "value4" \
        -property5 "value5" \
        -property6 "value6" \
        -property7 "value7" \
        -property8 "value8" \
        -property9 "value9" \
        -property10 "value10" ]

set RFC2544ThroughputTest(9) [stc::create "RFC2544ThroughputTest" \
        -under $Project(1) \
        -property1 "value1" \
        -property2 "value2" \
        -property3 "value3" \
        -property4 "value4" \
        -property5 "value5" \
        -property6 "value6" \
        -property7 "value7" \
        -property8 "value8" \
        -property9 "value9" \
        -property10 "value10" ]

set RFC2544ThroughputTest(10) [stc::create "RFC2544ThroughputTest" \
        -under $Project(1) \
        -property1 "value1" \
        -property2 "value2" \
        -property3 "value3" \
        -property4 "value4" \
        -property5 "value5" \
        -property6 "value6" \
        -property7 "value7" \
        -property8 "value8" \
        -property9 "value9" \
        -property10 "value10" ]

stc::config system1 -IsLoadingFromConfiguration "false"

    # Save to an XML file, if desired
    #stc::perform saveAsXml -config system1 -filename sampleSavedFilename.xml
    
    # Save to a Tcl file, if desired
    #stc::perform saveAsTcl -config system1 -filename sampleSavedFilename.tcl

}

#    connect - perform the logical to physical port mapping, connect to the 
#              chassis' and reserve the ports. This routine performs the connect,
#              reserve, and logical to physical port mappings directly.
#              The port list is retrieved from the in-memory configuration.
proc connect {} {
    stc::perform attachPorts -autoConnect true -portList [ stc::get project1 -children-Port ]
}

#    apply - apply writes the logical information held in memory on the 
#            workstation to the ports in the STC chassis'.
proc apply {} {    
    stc::apply
}

#    run - subscribe to any results views located in the in-memory configuration
#          and execute the sequencer and return the test status from the 
#          command sequence, if any. Test status is set by the Stopped Reason
#          in the Stop Command Sequence command. This is a string value and 
#          can be anything. If there is no sequence defined or no Stop 
#          Command Sequence command is executed, then the test state is 
#          returned. Test state can take the values: NONE, PASSED or FAILED.
proc run {} {    
    # Start the sequencer
    stc::perform sequencerStart

    # Wait for sequencer to finish
    stc::waituntilcomplete

    # check the sequencer status and test state
    set sqrHandle [stc::get System1 -Children-Sequencer]
    set sqrStatus [stc::get $sqrHandle -Status]
    set sqrTestState [stc::get $sqrHandle -TestState]
    if { $sqrStatus eq "" }  {
        return $sqrTestState
    } else {
        return $sqrStatus
    }

}

#    cleanup - release the ports, disconnect from the chassis' and reset 
#              the in-memory configuration.
proc cleanup {} {
    stc::perform chassisDisconnectAll 
    stc::perform resetConfig
}
