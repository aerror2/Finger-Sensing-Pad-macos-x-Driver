#!/bin/sh

FSPC_EXEC=/usr/bin/fspc
TMP_PERF_FILE=/tmp/fsp-current-pref.ini
FSP_FLAG_NODE=/sys/devices/platform/i8042/serio2/flags

case "$1" in
  suspend)
  	${FSPC_EXEC} -t -s ${TMP_PERF_FILE}
  	;;
  resume)
  	echo -n R > ${FSP_FLAG_NODE}
  	${FSPC_EXEC} -t -l ${TMP_PERF_FILE}
	rm -f ${TMP_PERF_FILE}
  	;;
  *)
  	;;
esac
