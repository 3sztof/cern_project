#!/bin/bash
export UTGID="test_sh"
/opt/FMC/bin/mPrint "bla bla bla"                                # default VERB
/opt/FMC/bin/mPrint -s WARN -p /tmp/logSrv.fifo "bla bla bla"
/opt/FMC/bin/mPrint -s ERROR -f main "bla bla bla"
