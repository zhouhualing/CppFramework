# Spirent TestCenter Launcher Script
# Generated on Wed Aug 12 10:38:39 2009 by bandrews
# Framework ver. w.xx.yyy.zzzz
#
# Comments: 
# 
# 
# This launcher script invokes the following routines contained in the
# test-save-loaded-3-level_logic.tcl script. After sourcing the logic script,
# the logic flow is as follows:
#
# init - set the logging level and logging location (stdout).
#
# config - load the configuration into memory and set the STC port locations.
#          The port locations can be modified in this file.
#
# configResultLocation -  set the location for results files.
#                         The location of the results files can be modified in this file.
#                         This does not set the location of the log files. Set 
#                         the STC_LOG_OUTPUT_DIRECTORY environment variable to choose 
#                         a log file location.
#
# configMiscOptions - set up the sequencer.
#
# connect - perform the logical to physical port mapping, connect to the 
#           chassis' and reserve the ports.
#
# apply - write the configuration to the ports.
#
# run - execute the sequencer and obtain the test status from the 
#       Basic: Stop Command Sequence "Stopped Reason" value. If there
#       is no sequence defined, then the test state is returned: NONE,
#       PASSED or FAILED.
#
# cleanup - disconnect from the chassis (releases the ports) and reset 
#           the in memory configuration.
#
# return - return the test status obtained from the sequencer execution
#          to the caller.

source [ file normalize [ file join [ file dirname [ info script ] ] {test-save-loaded-3-level_logic.tcl} ] ]

init
config [list "" ]
configResultLocation [ file normalize [ file dirname [ info script ] ] ]
configMiscOptions
connect
apply
set test_status [eval [concat run ]]
cleanup
return $test_status
