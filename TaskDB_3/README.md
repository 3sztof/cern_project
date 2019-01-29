
TaskDB module
=============

# ========================================================================
#
#  Start RPC service for jsonrpc and xmlrpc
#
# ------------------------------------------------------------------------
#
# Start the RPC server in regular mode:
python TaskDB/Server.py -d -p 8081 -s sqlite:///test.db --run -t json
# Start with SSL security
python TaskDB/Server.py -d -p 8081 -s sqlite:///test.db --run -t json --ssl -C <server-certificate> -K <key-file>
#
# Start the cache service
python TaskDB/Server.py -d -p 8081 -s sqlite:///test.db --run -t json --ssl -C <server-certificate> -K <key-file>
#
# Start the RPC server in test mode:
python TaskDB/Server.py -d -p 8081 -s sqlite:///test.db --run -t json -i -T
# Then fill the test data into the tables:
python TaskDB/apiTest.py -s sqlite:///test.db -c http://localhost:8081/TDBDATA/JSONRPC -t rpc
# Then connect to: http://localhost:8081/html/rpctest.html

# Stress test:
import time, TaskDB
rpc = TaskDB.connect('jsonrpc', 'http://localhost:8081/TDBDATA/JSONRPC')
check = str(rpc.getTask('testTask0'))
print check
start = time.time()
last  = start
for i in xrange(1000000):
  try:
    res = rpc.getTask('testTask0')
    if str(res) != check:
      print 'Error:',i,res
    if i > 0 and i%1000 == 0:
      now  = time.time()
      diff = now-start
      frq1 = float(i)/diff
      frq2 = float(1000)/(now-last)
      last = now
      print ('Processed %9d requests %8.2f seconds  '+\
             'Instantaneous: %5.0f Hz Integrated: %5.0f Hz')%(i,diff,frq2,frq1,)
  except Exception, X:
    print i, str(res)



# ========================================================================
#
#  Start RPC service for jsonrpc and xmlrpc
#
# ------------------------------------------------------------------------
#
# Instantiate RPC client
import TaskDB as implementation
#
# Instantiate a 'direct' db client returning a TaskDB.API.TaskDB instance
cl_dir  = implementation.connect('direct', 'sqlite:///test.db')
#
# Instantiate a 'xmlrpc' client
cl_xml  = implementation.connect('rpc', 'http://localhost:8081/TDBDATA/XMLRPC')
#
# Instantiate a 'jsonrpc' client
cl_json = implementation.connect('rpc', 'http://localhost:8081/TDBDATA/JSONRPC')
#
#
# ========================================================================
#
#  Testing the API and the CLI interface:
#
# The procedures below test the various clients and the CLI
# based on the various clients.
#
# When adding new tests ensure all client types are satisfied!
#
# ------------------------------------------------------------------------
#    Usage: apiTest.py [options]
#    
#    TaskDB API checker. Module to test all TaskDB api calls.
#    
#    Options:
#      -h, --help            show this help message and exit
#      -d, --debug           Enable output debug statements
#      -n, --no-init         Initialize/create new database
#      -f <string>, --file=<string>
#                            DB file name
#      -c <string>, --connection=<string>
#                            RPC server connection  (RPC calls only)
#      -t <string>, --type=<string>
#                            API type (xmlrpc,jsonrpc,direct)
# ------------------------------------------------------------------------
#    
# Run API test to test the programmers interface using the direct plug:
python TaskDB/apiTest.py -f test.db -c sqlite:///test.db -t direct

# Run API test to test the programmers interface using the XMLRPC plug:
python TaskDB/apiTest.py -f test.db -c http://localhost:8081/TDBDATA/XMLRPC -t rpc

# Run API test to test the programmers interface using the JSONRPC plug:
python TaskDB/apiTest.py -f test.db -c http://localhost:8081/TDBDATA/JSONRPC -t rpc

# Run API test to test the programmers interface using the WebSocket JSONRPC plug:
python TaskDB/apiTest.py -f test.db -c ws://localhost:8081/TDBDATA/JSONRPC -t rpc

# Run API test to test the programmers interface using the CLI (command line interface):
# --------------------------------------------------------------------------------------
# a) xmlrpc mode
python TaskDB/apiTest.py -f test.db -c sqlite:///test.db -t cli
# b) xmlrpc mode
python TaskDB/apiTest.py -f test.db -c http://localhost:8081/TDBDATA/XMLRPC -t cli
# c) jsonrpc mode
python TaskDB/apiTest.py -f test.db -c http://localhost:8081/TDBDATA/JSONRPC -t cli

# Run API test with SSL encryption
# ---------------------------------
# a) xmlrpc mode
python TaskDB/apiTest.py -f test.db -c https://localhost:8081/TDBDATA/XMLRPC -t rpc -C `pwd`/ssl/server.crt
# a) jsonrpc mode
python TaskDB/apiTest.py -f test.db -c https://localhost:8081/TDBDATA/JSONRPC -t rpc -C `pwd`/ssl/server.crt
