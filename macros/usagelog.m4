##
## ADL_USAGE_LOG
##   Configure a program to use usage logging.
##
## Command-line options:
##   --enable-usage-logging to enable/disable usage logging.  Default is enabled.
##   --with-usage-log-server to let a user specify the server to use for usage logging.
##
## Output:
##
##   LOG_USAGE:         Set to 1 if usage logging is enabled.
##   USAGE_LOG_SERVER:  Server name to use for usage logging.
##   USAGE_LOG_PORT:    Port number to use for usage logging.
##
AC_DEFUN([ADL_USAGE_LOG],
[
  AC_MSG_CHECKING([if usage logging should be enabled])
  ul_enable=yes
  AC_ARG_ENABLE([usage-logging],AC_HELP_STRING([--enable-usage-logging],[Turn usage logging on or off.  Default is on.]),ul_enable=$enableval)
  LogUsage=
  if [[ x$ul_enable != xno ]]; then
    LogUsage=1
  fi

  AC_ARG_WITH([usage-log-server],AC_HELP_STRING([--with-usage-log-server],[specify a server to use for usage logging.]),ac_ul_server=$withval)
  if [[ x$ac_ul_server = x ]] ; then
    ac_ul_server="usagelog.am.freescale.net";
  fi

  # Try to extract a port number from the server.  If we don't find one, then use a default.
  ac_ul_port=$(echo ${ac_ul_server} | awk -F: '{print $[2];}')
  ac_ul_server=$(echo ${ac_ul_server} | awk -F: '{print $[1];}')
  if [[ -z ${ac_ul_port} ]]; then
    ac_ul_port=1999
  fi

  # Try to ping the server to see if it's there.  If we don't get anything 
  # within 20 seconds, then time out and fail.
  if [[ ! -z $LogUsage ]]; then
    `ping -c 1 ${ac_ul_server} > /dev/null` || LogUsage=
  fi

  AH_TEMPLATE([LOG_USAGE],[Set to 1 if usage logging is enabled.])
  AH_TEMPLATE([USAGE_LOG_SERVER],[Set to the name of the server to use for usage logging, if enabled.])
  AH_TEMPLATE([USAGE_LOG_PORT],[Set to the port number of the server to use for usage logging, if enabled.])

  if [[ ! -z $LogUsage ]]; then
    USAGE_LOG_SERVER="$ac_ul_server"
    USAGE_LOG_PORT="$ac_ul_port"
    AC_MSG_RESULT([yes: ${USAGE_LOG_SERVER}:${USAGE_LOG_PORT}]) 
    LOG_USAGE=1
    AC_DEFINE([LOG_USAGE],1)
  else
    AC_MSG_RESULT([no]) 
    LOG_USAGE=0
    USAGE_LOG_SERVER=
    USAGE_LOG_PORT=0
  fi

  AC_DEFINE_UNQUOTED([USAGE_LOG_SERVER],["${USAGE_LOG_SERVER}"])
  AC_DEFINE_UNQUOTED([USAGE_LOG_PORT],[${USAGE_LOG_PORT}])

  AC_SUBST(LOG_USAGE)
  AC_SUBST(USAGE_LOG_SERVER)
  AC_SUBST(USAGE_LOG_PORT)
])
