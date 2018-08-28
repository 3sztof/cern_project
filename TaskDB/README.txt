
TaskDB module
=============

# ========================================================================
#
#  Start RPC service for jsonrpc and xmlrpc
#
# ------------------------------------------------------------------------
#
# Start the RPC server in regular mode:
python TaskDB/Server.py -d -p 8081 -f test.db --run -t json
#
# Start the RPC server in test mode:
python TaskDB/Server.py -d -p 8081 -f test.db --run -t json -i -T
# Then fill the test data into the tables:
python TaskDB/apiTest.py -f test.db -c http://localhost:8081/TDBDATA/JSONRPC -t rpc
# Then connect to: http://localhost:8081/html/rpctest.html

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
cl_json = implementation.connect('rpc', LHCb.Application.Globals_request_path)
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

# Run API test to test the programmers interface using the CLI (command line interface):
# a) xmlrpc mode
python TaskDB/apiTest.py -f test.db -c sqlite:///test.db -t cli
# b) xmlrpc mode
python TaskDB/apiTest.py -f test.db -c http://localhost:8081/TDBDATA/XMLRPC -t cli
# c) jsonrpc mode
python TaskDB/apiTest.py -f test.db -c http://localhost:8081/TDBDATA/JSONRPC -t cli
