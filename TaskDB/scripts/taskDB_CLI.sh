#!/bin/bash
# ======================================================================================
# Basic taskDB CLI invocation script.
#
# \author  M.Frank
# \version 1.0
# --------------------------------------------------------------------------------------
if test "${TASKDB_ROOT}" = ""; then
   CMD=`dirname $0/TaskDB/Setup.py`;
   eval `python ${CMD}`;
fi;
action="";
args="";
while test -n "$*"; do
    a1=`echo $1 | tr A-Z a-z`;
    case ${a1} in
        -a)
            action="$2";  shift; shift;
            ;;
        -act)
            action="$2";  shift; shift;
            ;;
        -action)
            action="$2";  shift; shift;
            ;;
        *)
            if test -z "${args}"; then
                args="$1";
            else
                args="${args}, $1";
            fi;
            shift;
            ;;                
    esac
    shift;
done;
##echo "Arguments:  ${action} -o \"${args}\""
python ${TASKDB_ROOT}/TaskDB/CLI.py -a ${action} -o "${args}";
