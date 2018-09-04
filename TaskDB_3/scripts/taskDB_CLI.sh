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
conn="http://ecs03.lbdaq.cern.ch:3500/TDBDATA/JSONRPC";
action="";
args="";
call_args="";
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
        -c)
            conn="$2";  shift; shift;
            ;;
        -connection)
            conn="$2";  shift; shift;
            ;;
        -cert)
            call_args="${call_args} --certfile=$2";  shift; shift;
            ;;
        -certificate)
            call_args="${call_args} --certfile=$2";  shift; shift;
            ;;
        -d)
            call_args="${call_args} --pdb";  shift;
            ;;
        -p)
            call_args="${call_args} --print";  shift;
            ;;
        -print)
            call_args="${call_args} --print";  shift;
            ;;
        -o) shift;
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
done;
#echo "Arguments:  ${action} -o \"${args}\""
#python3.5 ${TASKDB_ROOT}/TaskDB/CLI.py ${call_args} -c http://ecs03.lbdaq.cern.ch:3500/TDBDATA/JSONRPC -a ${action} -o "${args}";
if test "`hostname -s | tr A-Z a-z`" = "pcmf005"; then
    MINICONDA=/home/frankm/SW/Miniconda3;
else
    MINICONDA=/group/online/dataflow/cmtuser/libraries/Miniconda;
fi;
export PATH=${MINICONDA}/bin:$PATH;
export PYTHONHOME=${MINICONDA};
source ${MINICONDA}/bin/activate;
echo python3.6 ${TASKDB_ROOT}/python/TaskDB/CLI.py ${call_args} --connection=${conn} --action=${action} -o "${args}";
python3.6 ${TASKDB_ROOT}/python/TaskDB/CLI.py ${call_args} --connection=${conn} --action=${action} -o "${args}";
