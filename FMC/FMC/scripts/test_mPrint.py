#!/usr/bin/env python
import os
os.putenv("UTGID","test_py")
os.system("/opt/FMC/bin/mPrint \"bla bla bla\"")                # default: VERB
os.system("/opt/FMC/bin/mPrint -s WARN -p /tmp/logSrv.fifo \"bla bla bla\"")
os.system("/opt/FMC/bin/mPrint -s ERROR -f main \"bla bla bla\"")
