# ======================================================================================
# TaskDB JSONRPC and XMLRPC server
#
# \author  M.Frank
# \version 1.0
# --------------------------------------------------------------------------------------
import os, sys, errno, logging, jsonrpclib, traceback
import SocketServer, SimpleHTTPServer, SimpleXMLRPCServer
from jsonrpclib import SimpleJSONRPCServer

# Configure output logging
logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)

def log_socket_size(opt, s):
  import fcntl, termios, array
  sock_size = array.array('i', [0])
  fcntl.ioctl(s, termios.FIONREAD, sock_size)
  print "%s: Socket size: %d"%(opt, sock_size[0], )

# ======================================================================================
# RPC call wrapper
#
# \author  M.Frank
# \version 1.0
# --------------------------------------------------------------------------------------
class Func:
  # ====================================================================================
  # Constructor
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def __init__(self, o, f, d=False):
    self.object = o
    self.func = f
    self.dbg = d

  # ====================================================================================
  # Call to the instance object
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def make_call(self, args):
    try:
      #logging.info('++++ Args['+str(args.__class__)+']:'+str(args))
      if args.__class__ == dict:
        result = self.func(self.object, **args)
      else:
        result = self.func(self.object, args)
      if self.dbg:
        logging.info('++++ Result: Call to '+str(self.func)+' was successful')
      return result
    except Exception,e:
      logging.info('++++ Exception: '+str(e))
      raise Exception(str(e))

# ======================================================================================
# RPC interface object
# All functions are required to contain the argument dictionary with valid key-names
#
# \author  M.Frank
# \version 1.0
# --------------------------------------------------------------------------------------
class DB:
  # ====================================================================================
  # Constructor
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def __init__(self, opts):
    import API
    API.Connection.debug = opts.debug
    self.opts = opts
    self.dbg  = opts.debug
    self.api  = API.TaskDB
    self.conn = API.Connection
    self.db   = self.api(self.conn(database='sqlite:///'+opts.database),opts.debug)

  # ====================================================================================
  # Generic interface call forwarding all calls to a wrapped version of the TaskDB call
  # The wrapped call requires that all argument keywords are specified and then
  # forwards the call to the proper API instance
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def __getattr__(self, attr):
    #logging.info('__getattr__: Attr:'+ str(attr))
    func   = getattr(self.api,attr)
    if func:
      if attr == '_dispatch':
        return func
      caller = Func(self.db, func, self.dbg)
      return caller.make_call
    ##logging.info('__getattr__: None')
    return None

  # ====================================================================================
  # Reconnect to the existing database
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def reconnect(self):
    logging.info('++ Reconnection to existing database '+self.opts.database)
    self.db.connection = self.conn(database='sqlite:///'+self.opts.database)
    return 'Success'

  # ====================================================================================
  # Rebuild the database and install the test instances
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def rebuild(self):
    if self.opts.test:
      import Installer, apiTest
      self.db.connection = None
      Installer.destroy(self.opts.database)
      Installer.install('sqlite:///'+self.opts.database)
      #apiTest.fillTables(self.conn(database='sqlite:///'+self.opts.database))
      logging.info('++ The database '+self.opts.database+' has been rebuilt ....')
      return self.reconnect()
    return 'Failed'

  # ====================================================================================
  # Test function
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def baz(self, a0, a1, args):
    logging.info('++++ Args['+str(a0.__class__)+']:'+str(a0))
    logging.info('++++ Args['+str(a1.__class__)+']:'+str(a1))
    logging.info('++++ Args['+str(args.__class__)+']:'+str(args))
    return 'Success'

# ======================================================================================
# Combined request handler to serve
# - JSON RPC calls
# - XML RPC calls
# - HTTP file requests
#
# \author  M.Frank
# \version 1.0
# --------------------------------------------------------------------------------------
class CombiHandler(SimpleJSONRPCServer.SimpleJSONRPCRequestHandler,
                   #SimpleXMLRPCServer.SimpleXMLRPCRequestHandler,
                   SimpleHTTPServer.SimpleHTTPRequestHandler):  
  debug = False

  def setup(self):
    SimpleJSONRPCServer.SimpleJSONRPCRequestHandler.setup(self)
    #logging.info('++ Performing setup....')

  def handle(self):
    #log_socket_size('CombiHandler::handle', self.request._sock)
    SimpleJSONRPCServer.SimpleJSONRPCRequestHandler.handle(self)

  # ====================================================================================
  # Execute GET request (File handling)
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def do_GET(self):
    #logging.info('++ GET: Path='+self.path)        
    if CombiHandler.debug:
      logging.info('++ GET: Path='+self.path)        
    return SimpleHTTPServer.SimpleHTTPRequestHandler.do_GET(self)

  # ====================================================================================
  # Execute POST request (RPC/REST)
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def do_POST(self):
    #logging.info('++ GET: Path='+self.path)
    try:
      if CombiHandler.debug:
        logging.info('++ POST: Path='+self.path+' RQ='+self.requestline)
      if self.path[:15] == '/TDBDATA/XMLRPC':
        print 'XML RPC'
        return SimpleXMLRPCServer.SimpleXMLRPCRequestHandler.do_POST(self)
      elif self.path[:16] == '/TDBDATA/JSONRPC':
        print 'JSON RPC'
        return SimpleJSONRPCServer.SimpleJSONRPCRequestHandler.do_POST(self)
      elif self.path[:19] == '/TDBDATA/REST/json/':
        return self.rest_json_handle_request(self.path[19:])
      return SimpleHTTPServer.SimpleHTTPRequestHandler.do_POST(self)
    except Exception,X:
      print 'POST: Exception:',str(X)


  # ====================================================================================
  # Execute POST request (RPC/REST)
  #
  # \author  K.Wilczynski
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def do_OPTIONS(self):
        self.send_response(200, "ok")
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, PUT, DELETE, OPTIONS')
        self.send_header("Access-Control-Allow-Headers", "X-Requested-With")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.end_headers()

  # ====================================================================================
  # REST API: Helper to create fault replies
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def rest_convert_fault(self, code, message):
    if message.find(' | ')>=0 and message.find(':')>=0:
      message = message[:message.find(':')+1]+message[message.find(' | '):]
    resp = {'error': {'message': message, 'code': code}}
    return jsonrpclib.jsonrpc.jdumps(resp)

  # ====================================================================================
  # REST API: Helper to handle REST requests
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def rest_json_handle_request(self, method):
      try:
        max_chunk_size = 10*1024*1024
        size_remaining = int(self.headers["content-length"])
        L = []
        while size_remaining:
          chunk_size = min(size_remaining, max_chunk_size)
          L.append(self.rfile.read(chunk_size))
          size_remaining -= len(L[-1])
        data = ''.join(L)
        args = jsonrpclib.jsonrpc.jloads(data)
        if CombiHandler.debug:
          logging.info('REST: Method: %s Args[%s]: %s'%(method, str(args.__class__), str(args),))
        disp = self.server.get_dispatcher('/TDBDATA/JSONRPC')
        resp = disp._dispatch(method,args)
        if resp.__class__ == str:
          response = jsonrpclib.jsonrpc.jdumps(resp)
        elif resp.__class__ == jsonrpclib.jsonrpc.Fault:
          response = self.rest_convert_fault(resp.faultCode, resp.faultString)
        else:
          response = jsonrpclib.jsonrpc.jdumps({'data': resp})
        self.send_response(200)
      except Exception:
        self.send_response(500)
        err_lines = traceback.format_exc().splitlines()
        trace_string = '%s | %s' % (err_lines[-3], err_lines[-1])
        response = self.rest_convert_fault(-32603,'Server error: %s' % trace_string)
      if response is None:
        response = ''
      if CombiHandler.debug:
        logging.info('REST: Response [%s]: %s'%(str(response.__class__),response,))
      self.send_header("Content-type", "application/json-rpc")
      self.send_header("Content-length", str(len(response)))
      self.end_headers()
      self.wfile.write(response)
      self.wfile.flush()
      self.connection.shutdown(1)

# ======================================================================================
# Combined server for multiple purposes
# - JSON RPC calls
# - XML RPC calls
# - HTTP file requests
#
# Note: Multi-threading support in SQLite is currently not possible,
#       because we share the connection object!
#
# \author  M.Frank
# \version 1.0
# --------------------------------------------------------------------------------------
#class CombiServer(SocketServer.ThreadingMixIn,
#                  SimpleXMLRPCServer.MultiPathXMLRPCServer):
class CombiServer(SimpleXMLRPCServer.MultiPathXMLRPCServer):
  # ====================================================================================
  # Register server instance with a given path and of a given type
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def register_instance(self, instance, path, type, allow_dotted_names=False):
    SimpleXMLRPCServer.SimpleXMLRPCRequestHandler.rpc_paths.append(path)
    if type.lower() == 'jsonrpc':
      disp = SimpleJSONRPCServer.SimpleJSONRPCDispatcher(encoding=self.encoding)
      disp.register_instance (instance, allow_dotted_names)
      self.add_dispatcher(path, disp)
    elif type.lower() == 'xmlrpc':
      disp = SimpleXMLRPCServer.SimpleXMLRPCDispatcher(self.allow_none, self.encoding)
      disp.register_instance (instance, allow_dotted_names)
      self.add_dispatcher(path, disp)
    else:
      raise Exception("Unknown RPC type: "+type)

  # ====================================================================================
  # Server dispatch method
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def _marshaled_dispatch(self, data, dispatch_method=None, path=None):
    #print 'Data:', "'"+str(path)+"'", path
    return SimpleXMLRPCServer.MultiPathXMLRPCServer._marshaled_dispatch(self, data, dispatch_method, path)

  def process_request(self, request, client_address):
    #log_socket_size('CombiServer::process_request', request._sock)
    return SimpleXMLRPCServer.MultiPathXMLRPCServer.process_request(self, request, client_address)

# ======================================================================================
# Main entry point: Start server instance
#
# \author  M.Frank
# \version 1.0
# --------------------------------------------------------------------------------------
def run_server(opts):
  SimpleXMLRPCServer.SimpleXMLRPCRequestHandler.rpc_paths = []
  port   = int(opts.port)
  taskdb = DB(opts)
  if opts.init:
    taskdb.rebuild()
    taskdb.reconnect()
  if opts.run:
    CombiHandler.debug = opts.debug
    server  = CombiServer(addr=('localhost', port),
                          logRequests=False,
                          requestHandler=CombiHandler)
    logging.info('++ Instantiated %s RPC server.'%(opts.type,))
    logging.info('++ Server ready to handle %s requests. Port=%s'%(opts.type.upper(), opts.port,))
    server.register_instance(taskdb, '/TDBDATA/JSONRPC', 'jsonrpc')
    server.register_instance(taskdb, '/TDBDATA/XMLRPC', 'xmlrpc')
    server.serve_forever()

# Start from the command line
if __name__ == "__main__":
  import optparse
  parser = optparse.OptionParser()
  parser.description = "TaskDB JSON/XML RPC Server."
  parser.formatter.width = 132
  parser.add_option("-d", "--debug", 
                    action='store_true',
                    dest="debug", default=False,
                    help="Enable output debug statements",
                    metavar="<boolean>")
  parser.add_option("-D", "--pdb", 
                    action='store_true',
                    dest="start_pdb", default=False,
                    help="Start pdb for debugging",
                    metavar="<boolean>")
  parser.add_option("-i", "--init", 
                    action='store_true',
                    dest="init", default=False,
                    help="Initialize/create new database",
                    metavar="<boolean>")
  parser.add_option("-r", "--run", 
                    action='store_true',
                    dest="run", default=False,
                    help="Run the server instance",
                    metavar="<boolean>")
  parser.add_option("-f", "--file",
                    dest="database",
                    default='./taskdb_test.db',
                    help="DB file name",
                    metavar="<string>")
  parser.add_option("-t", "--type",
                    dest="type",
                    default='jsonrpc',
                    help="Server type",
                    metavar="<string>")
  parser.add_option("-T", "--test",
                    dest="test",
                    default=False,
                    action='store_true',
                    help="Set test mode",
                    metavar="<boolean>")
  parser.add_option("-p", "--port",
                    dest="port",
                    default=8081,
                    help="Server port",
                    metavar="<integer>")
  (opts, args) = parser.parse_args()
  if opts.start_pdb:
    import pdb
    pdb.set_trace()
  run_server(opts)
