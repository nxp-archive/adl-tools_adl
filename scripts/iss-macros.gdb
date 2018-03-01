#
# Useful macros when using GDB.
#

# Start a simulator in the background.
# arg0:  Path to ISS executable.
# arg1:  Path to ELF file to run on the simulator and debug.
define startsim
	file $arg1
	target extended-remote | $arg0 $arg1 -g -gdb-stdio
end

# Start a simulator in the background and force the architecture to the specified type.
# Use this macro to avoid the bug in GDB, whereby it thinks that any BookE part should
# have the e500 register model.
# assumes that a BookE part is an e500.
# arg0:  Path to ISS executable.
# arg1:  Path to ELF file to run on the simulator and debug.
define startsimarch
	file $arg1
	set archi $arg2
	target extended-remote | $arg0 $arg1 -g -gdb-stdio
end
