##
## AM_RUNSIM([, action-if-found [,action-if-not-found]])
##   Configure ADL to build the RunSim integration.
##
##   action-if-found:  Executed if RunSim is found.
##   action-if-not-found:  Executed if RunSim is not found.
##
## Environment variables
##   RUNSIM_DIR     :  Directory to the RunSim executable.
##
## Output:
##
##   RUNSIM_EXISTS :  true | false
##   RUNSIM_PATH   :  Path to runsim.
##
AC_DEFUN([AM_RUNSIM],
[
  AC_ARG_VAR([RUNSIM_DIR],    [Specify the location of the RunSim executable.])

  AC_MSG_CHECKING([for runsim])

  RunSimExists=yes
  if [[ ! -f ${RUNSIM_DIR}/runsim ]] ; then
    AC_MSG_RESULT([no.  Could not find the RunSim executable.  Specify this with the RUNSIM_DIR environment variable.])
	RunSimExists=no
  fi

  if [[ $RunSimExists != "no" ]] ; then
	RUNSIM_PATH=${RUNSIM_DIR}
	AC_MSG_RESULT([yes:  ${RUNSIM_PATH}])
  fi

  if [[ $RunSimExists = yes ]]; then
	RUNSIM_EXISTS=true
    ifelse([$1], , :, [$1])
  else
	RUNSIM_EXISTS=false
    ifelse([$2], , :, [$2])
  fi

  AC_SUBST(RUNSIM_EXISTS)
  AC_SUBST(RUNSIM_PATH)
])

##
## AM_DTASM([, action-if-found [,action-if-not-found]])
##   Configure ADL to test the DevTech assembler integration.
##
##   action-if-found:  Executed if DevTech assembler is found.
##   action-if-not-found:  Executed if DevTech assembler is not found.
##
## Environment variables
##   DTASM_DIR     :  Directory to the DevTech assembler.
##
## Output:
##
##   DTASM_EXISTS :  true | false
##   DTASM_PATH   :  Path to runsim.
##
AC_DEFUN([AM_DTASM],
[
  AC_ARG_VAR([DTASM_DIR],    [Specify the location of the DevTech assembler.])

  AC_MSG_CHECKING([for the DevTech assembler])

  DtAsmExists=yes
  if [[ ! -f ${DTASM_DIR}/scasm ]] ; then
    AC_MSG_RESULT([no.  Could not find the DevTech assembler.  Specify this with the DTASM_DIR environment variable.])
	  DtAsmExists=no
  fi

  if [[ $DtAsmExists != "no" ]] ; then
	  DTASM_PATH=${DTASM_DIR}
	  AC_MSG_RESULT([yes:  ${DTASM_PATH}])
  fi

  if [[ $DtAsmExists = yes ]]; then
	  DTASM_EXISTS=true
    ifelse([$1], , :, [$1])
  else
	  DTASM_EXISTS=false
    ifelse([$2], , :, [$2])
  fi

  AC_SUBST(DTASM_EXISTS)
  AC_SUBST(DTASM_PATH)
])
