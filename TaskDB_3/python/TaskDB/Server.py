# ======================================================================================
# TaskDB JSONRPC and XMLRPC server
#
# \author  M.Frank
# \version 1.0
# --------------------------------------------------------------------------------------
import os, time, socket, logging, threading, traceback, socketserver
import http.server, xmlrpc.server, xmlrpc.client, jsonrpc.server, jsonrpc.client
import HTTPWebSocketsHandler

# Configure output logging
#logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)
logging.basicConfig(format='%(threadName)-12s %(levelname)-8s: %(message)s', 
                    level=logging.INFO)

def log_socket_size(opt, sock):
  import fcntl, termios, array
  sock_size = array.array('i', [0])
  fcntl.ioctl(sock, termios.FIONREAD, sock_size)
  logging.info("%s: Socket size: %d"%(opt, sock_size[0],))

# ======================================================================================
# Worker definition for the thread POOL.
# Thread executing tasks from a given tasks queue.
#
# \author  M.Frank
# \version 1.0
# --------------------------------------------------------------------------------------
class Worker(threading.Thread):
  # ====================================================================================
  # Constructor
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def __init__(self, tasks):
    threading.Thread.__init__(self)
    self.tasks = tasks
    self.daemon = True
    self.start()

  def run(self):
    while True:
      func, args, kargs = self.tasks.get()
      try:
        func(*args, **kargs)
      except Exception as  e:
        logging.info('++ Worker exception: '+str(e))
      finally:
        self.tasks.task_done()

# ======================================================================================
# Thread POOL implementation:
# Pool of threads consuming tasks from a queue
#
# \author  M.Frank
# \version 1.0
# --------------------------------------------------------------------------------------
class ThreadPool:

  # ====================================================================================
  # Constructor
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def __init__(self, num_threads):
    import queue
    self.tasks = queue.Queue(num_threads)
    for _ in range(num_threads):
      Worker(self.tasks)

  def add_task(self, func, *args, **kargs):
    """Add a task to the queue"""
    self.tasks.put((func, args, kargs))

  def wait_completion(self):
    """Wait for completion of all the tasks in the queue"""
    self.tasks.join()

# ======================================================================================
# Request queue to be executed in a thread
#
# \author  M.Frank
# \version 1.0
# --------------------------------------------------------------------------------------
class ThreadRequestQueue:

  # ====================================================================================
  # Constructor
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def __init__(self, consumer, pre_handler=None, post_handler=None):
    self.post_handler = post_handler
    self.pre_handler  = pre_handler
    self.eventList    = []
    self.event        = threading.Event()
    self.lock         = threading.Lock()
    self.consumer     = consumer
    self.debug        = False
    self.data         = []
    self.event.clear()

  def __del__(self):
    self.lock = None

  def wait(self, evt):
    evt.wait()
    self.lock.acquire()
    self.eventList.append(evt)
    self.lock.release()

  def stop(self):
    self.lock.acquire()
    self.data.insert(0, 'stop')
    self.lock.release()
    self.event.set()

  def push(self, work):
    self.lock.acquire()
    self.data.insert(0, work)
    if len(self.eventList) == 0:
      event = threading.Event()
    else:
      event = self.eventList.pop()
    event.clear()
    work.event = event
    self.lock.release()
    self.event.set()
    return event

  def pop(self):
    obj = None
    self.lock.acquire()
    if len(self.data) > 0:
      obj = self.data.pop()
    else:
      self.event.clear()
    self.lock.release()
    return obj

  def run(self):
    #import pdb; pdb.set_trace()
    if self.debug:
      logging.info('++ Request queue running....')
    while 1:
      self.event.wait()
      if self.run_one(self.pop()):
        if self.debug:
          logging.info('++ Request queue stopped....')
        return True
      
  def run_one(self, obj):
      if obj:
        if obj.__class__ == str and obj == 'stop':
          return True
        if self.pre_handler:
          #import pdb; pdb.set_trace()
          self.pre_handler(obj)
        obj.execute()
        if self.post_handler: self.post_handler(obj)
      return False
    
  def start(self, name, startup=None):
    if not startup: startup = self.run
    if self.debug:
      logging.info('++ Starting request queue....')
    self.thread = threading.Thread(name=name, target=startup, args=())
    self.thread.start()

# ======================================================================================
# RPC call wrapper
#
# \author  M.Frank
# \version 1.0
# --------------------------------------------------------------------------------------
class RPCTrampolin:

  # ====================================================================================
  # Constructor
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def __init__(self, obj, function, dbg=False, workQueue=None):
    self.object   = obj
    self.func     = function
    self.dbg      = dbg
    self.work     = workQueue

  # ====================================================================================
  # Call to the instance object
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def make_call(self, *kw, **args):
    try:
      #import pdb; pdb.set_trace()
      if self.dbg:
        logging.info('++++ Args['+str(args.__class__)+']:'+str(args)+' kw:['+str(kw.__class__)+']:'+str(kw))
      is_tuple = kw.__class__ == tuple
      if len(kw) == 1 and is_tuple and kw[0].__class__ == dict:
        arg = kw[0]
        result = self.func(self.object, **arg)
      elif is_tuple and 0 == len(args):
        result = self.func(self.object, *kw)
      elif kw.__class__ == list:
        result = self.func(self.object, *kw)
      elif len(args) and args.__class__ == dict:
        result = self.func(self.object, **args)
      else:
        result = self.func(self.object, kw)
      if self.dbg:
        logging.info('++++ Result: Call to '+str(self.func)+' was successful')
      return result
    except Exception as e:
      logging.info('++++ Exception: '+str(e))
      raise e

  def execute(self):
    #import pdb; pdb.set_trace()
    try:
      if self.dbg:
        logging.info('++++ execute:           Args['+str(self.args.__class__)+']:'+str(self.args))
      self.result = self.make_call(*self.args)
    except Exception as e:
      self.result = e
    self.event.set()

  def make_consumer_call(self, *kw, **args):
    if self.dbg:
      logging.info('++++ make_consumer_call Args['+str(args.__class__)+']:'+str(args)+' kw:['+str(kw.__class__)+']:'+str(kw))
    #logging.info('++++ make_consumer_call: Args['+str(args.__class__)+']:'+str(args))
    #import pdb; pdb.set_trace()
    self.args   = kw
    self.result = None
    self.event  = None
    self.work.push(self)
    self.work.wait(self.event)
    ##logging.info('++++ make_consumer_call: '+str(self.result))
    if self.result.__class__ == Exception:
      m = str(self.result)
      idx = m.find(' | Exception:')
      if idx > 0: raise Exception(m[idx+len(' | Exception:'):])
      raise self.result
    return self.result

# ======================================================================================
# RPC client object
# Allows to forward commands to the clients -- if present
#
# \author  M.Frank
# \version 1.0
# --------------------------------------------------------------------------------------
class RPCClient:
  # ====================================================================================
  @staticmethod
  def id(host,port):
    return '%s:%s'%(host,port,)

  # ====================================================================================
  # Constructor
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def __init__(self, host, port):
    self.host   = host
    self.port   = str(port)
    self.server = None
    self.stamp  = time.time()
    self.id     = RPCClient.id(self.host, self.port)
    self.url    = 'http://%s:%s/TDBDATA/JSONRPC'%(socket.gethostbyname(self.host),self.port,)

  # ====================================================================================
  # Update time stamp of alst update/interaction
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def update(self):
    self.stamp = time.time()

  # ====================================================================================
  # Check the difference from the last action samp
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def expired(self):
    diff = float(time.time()-self.stamp)
    return diff > 30.0

  # ====================================================================================
  # Broadcast command to clients
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def broadcast(self, **args):
    import TaskDB as tdb
    result = None
    if self.server is None:
      self.server = tdb.connect('jsonrpc',self.url)
    try:
      result = self.server.broadcast(**args)
    except Exception as X:
      logging.error('Broadcast exception:'+str(X))
    return {'client': self.id, 'result': result}

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
    import socket
    #import pdb;  pdb.set_trace()
    self.db         = None
    self.opts       = opts
    self.work       = None
    self.connection = None
    self.cache      = None
    self.dbg        = opts.debug
    self.tmpdb      = None
    self.timer      = None
    self.host       = socket.gethostname().upper()
    self.clients    = {}
    self.commands   = {}

    # Define the API to be used
    if self.opts.cache:
      import CacheAPI
      self.api   = CacheAPI.TaskDB
      self.conn  = self.connect_cache_server
    else:
      import API
      API.Connection.debug = opts.debug
      self.api   = API.TaskDB
      self.conn  = API.Connection
    self.start()

  # ====================================================================================
  # Generic interface call forwarding all calls to a wrapped version of the TaskDB call
  # The wrapped call requires that all argument keywords are specified and then
  # forwards the call to the proper API instance
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def __getattr__(self, attr):
    ##logging.info('__getattr__: Attr:'+ str(attr))
    func   = getattr(self.api,attr)
    if func:
      if attr == '_dispatch':
        return func
      work = self.work
      cache = self.cache
      if work and not cache:
        caller = RPCTrampolin(self.db, func, self.dbg, work)
        return caller.make_consumer_call
      caller = RPCTrampolin(self.db, func, self.dbg)
      return caller.make_call
    #logging.info('__getattr__: UNKNOWN Attr:'+ str(attr))
    return None

  # ====================================================================================
  # Start the database service
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def start(self):
    # Setup connection to the database and multi-threading
    if (self.opts.cache) or (not self.opts.threads or self.opts.threads == 0):
      self.db = self.api(self.conn(self.opts.database),dbg=self.dbg)
    else: # The database does not access requests from multiple threads.
      #     We need to stream all requests to the DB handler thread:
      self.work  = ThreadRequestQueue(self, pre_handler=self.preHandleRequest)
      self.work.start(name='SQLite', startup=self.run)
    #
    # Aux stuff still to be done
    if self.opts.cache:
      self.timer = threading.Timer(2.0, self.checkIntegrity, args=[None])
      self.timer.start()

  # ====================================================================================
  # Callback called before every request. Ensure database is connected
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def preHandleRequest(self, req):
    if self.tmpdb is None or self.db is None or self.db.connection is None:
      logging.info('++ Reconnecting database handles....')
      self.db.connection = self.conn(self.opts.database)
      self.tmpdb = self.db.connection
  
  # ====================================================================================
  # Cached execution: Access an up-to-date version of the data cache
  # The cache is automatically re-build if found dirty
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def getCache(self, stamp):
    import tools
    now = time.time()
    if not self.cache or self.db.dirty:
      logging.info('++ (Re)Building TaskDB cache.....')
      data       = tools.Cache().build(self.db)
      self.cache = tools.Cache().compress(data)
      self.db.dirty = False
    return self.cache

  # ====================================================================================
  # Cached execution: Helper to invalidate cache
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def clearCache(self):
    logging.info('++ Clear TaskDB cache.....')
    self.cache = None
    if self.opts.cache:
      self.db.data = None
      
  # ====================================================================================
  # Set database dirty to enforce cache reloading
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def setDirty(self):
    logging.info('++ Set TaskDB cache dirty.....')
    self.db.dirty = True
      
  # ====================================================================================
  # Shutdown database service.
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def shutdown(self):
    try:
      if self.work:
        self.work.stop()
      if self.timer:
        self.timer.cancel()
        self.timer = None
    except Exception as X:
      logging.error('++ Failure during shudown: '+str(X))

  # ====================================================================================
  # Rebuild the database and install the test instances
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def rebuild(self):
    if self.opts.test:
      import Installer, apiTest
      ##if hasattr(self,'db'): self.db.connection = None
      Installer.destroy(self.opts.database)
      Installer.install(self.opts.database)
      #apiTest.fillTables(self.conn(database=self.opts.database))
      logging.info('++ The database '+self.opts.database+' has been rebuilt ....')
      self.cache = None
    try:
      return self.reconnect(0)
    except Exception as X:
      logging.error('Rebuild error: '+str(X))
    return 'Failed'

  # ====================================================================================
  # Multi-threading:
  # Runnable method for multi-threaded execution
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def run(self):
    logging.info('++ Starting multi-threaded mesage pump....')
    self.db    = self.api(self.conn(self.opts.database),self.opts.debug)
    self.tmpdb = self.db.connection
    work  = self.work
    event = self.work.event
    work.run()
    logging.info('++ Stop request seen. Shutdown server.')

  # ====================================================================================
  # Cached execution:
  # Wrapper to connect to a delayed rpc cache rather than a database
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def connect_cache_server(self, db):
    from TaskDB import connect as connect_server
    while 1:
      try:
        result = connect_server(self.opts.type, db)
        return result
      except Exception as X:
        logging.info('++ Failed to connect TaskDB:'+str(X)+' retrying...')

  # ====================================================================================
  # Cached execution:
  # Timer callback to check the cache integrity with the parent object.
  # Only used for pure cached running!
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def checkIntegrity(self, expired_timer):
    import tools
    try:
      data = None
      conn = None
      stamp = 0
      db   = self.db
      #import pdb; pdb.set_trace()
      if db is not None and db.connection is not None:
        conn = db.connection
      if conn is not None and conn.data is not None:
        data = conn.data
      if expired_timer is not None:
        expired_timer.join()
      if conn is not None:
        if data is not None: stamp = data.timeStamp
        check = db.dirty or db.connection.needUpdate(self.host, self.opts.port)
        if check or self.cache is None:
          logging.info('++ Need to update TaskDB cache.')
          db.reconnect(0)
          self.cache = tools.Cache().compress(db.data)
        elif self.dbg:
          logging.debug('++ TaskDB cache up to date. No action needed')
          pass
      elif self.dbg:
        logging.debug('++ TaskDB cache inactive. No action needed.')
        pass
    except Exception as X:
      logging.info('++ Intermittent error updating DB cache: ['+str(X)+']')
    curr_timer = self.timer
    self.timer = threading.Timer(2.0, self.checkIntegrity, args=[curr_timer])
    self.timer.start()

  # ====================================================================================
  # Cached execution: Parent callback to check if updates are needed
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def needUpdate(self, host, port, opt=None):
    #import pdb; pdb.set_trace()
    res = self.cache is None
    if not res and self.db: res = self.db.dirty
    if not res and type(opt) == dict and opt.get('stamp',None):
      res = self.db.timeStamp > int(opt['stamp'])
      if self.dbg and res:
        logging.info('++ Client Update host: %s port: %s triggered by db timestamp. DB: %d > Client: %d'%\
                     (host, str(port), self.db.timeStamp,int(opt['stamp'],)))
    client = RPCClient.id(host,port)
    if not self.clients.get(client,None):
      if self.dbg:
        logging.info('++ Registered command client: host: %s port: %s opt: %s'%\
                     (host, str(port),str(opt),))
      cl = RPCClient(host, port)
      self.clients[client] = cl
    else:
      cl = self.clients[client]
    cl.stamp = time.time()
    for k, cl in self.clients.items():
      if cl.expired():
        del self.clients[k]
    return res

  # ====================================================================================
  # Forward broadcasts to clients
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def broadcast(self, **args):
    logging.info('++ Processing broadcast: '+str(args))
    result = []
    for k,cl in self.clients.items():
      try:
        r = cl.broadcast(**args)
        result.append(r)
      except Exception as X:
        logging.error('broadcast Exception: '+str(X))
    return result
    
# ======================================================================================
# Combined request handler to serve
# - JSON RPC calls
# - XML RPC calls
# - HTTP file requests
#
# \author  M.Frank
# \version 1.0
# --------------------------------------------------------------------------------------
class CombiHandler(jsonrpc.server.SimpleJSONRPCRequestHandler,
                   xmlrpc.server.SimpleXMLRPCRequestHandler,
                   HTTPWebSocketsHandler.HTTPWebSocketsHandler,
                   http.server.SimpleHTTPRequestHandler):
  import base64, binascii
  debug = False
  authentication = 'Basic ' + base64.b64encode(binascii.a2b_qp('LHCb:CKM-best')).hex()
  
  def setup(self):
    jsonrpc.server.SimpleJSONRPCRequestHandler.setup(self)
    self.connected = False   # Required by HTTPWebSocketsHandler

  # ====================================================================================
  # Check if client is authenticated
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def authenticated(self):
    if self.server.authentication:
      hdr = self.headers.getheader('Authorization')
      if hdr is not None:
        if hdr == CombiHandler.authentication:
          return True
      return False
    return True

  # ====================================================================================
  # Request client to authtenticate itself
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def get_authentication(self):
    hdr = self.headers.getheader('Authorization')
    if hdr is None:
      self.do_AUTHHEAD()
      self.wfile.write('no auth header received')
      return
    self.do_AUTHHEAD()
    self.wfile.write(self.headers.getheader('Authorization'))
    self.wfile.write('not authenticated')
   
  # ====================================================================================
  # Execute OPTIONS request (File handling)
  #
  # \author  M.Frank
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
  # Execute HEAD request (Authentication handling)
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def do_HEAD(self):
    logging.debug("send header")
    self.send_response(200)
    self.send_header('Content-type', 'text/html')
    self.end_headers()

  # ====================================================================================
  # Execute AUTHHEAD request (Authentication handling)
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def do_AUTHHEAD(self):
    logging.debug("send header")
    self.send_response(401)
    self.send_header('WWW-Authenticate', 'Basic realm=\"TDB\"')
    self.send_header('Content-type', 'text/html')
    self.end_headers()

  # ====================================================================================
  # Execute GET request (File handling)
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def do_GET(self):
    #logging.info('++ GET: Path='+self.path)        
    if CombiHandler.debug:
      logging.info('++ GET: Path= '+self.path)        
      logging.info('++ GET: Headers=\n'+str(self.headers))
      logging.info('++ GET: Authenticated= '+str(self.authenticated()))

    if not self.authenticated():
      return self.get_authentication()
    elif self.headers.get("Upgrade", None) == "websocket":
      # Websocket handling only for multi-threaded servers
      if hasattr(self.server,'num_threads'):
        plen = len(self.path)
        if plen >= 15 and self.path[:15] == '/TDBDATA/XMLRPC':
          return HTTPWebSocketsHandler.HTTPWebSocketsHandler.do_GET(self)
        elif plen >= 16 and self.path[:16] == '/TDBDATA/JSONRPC':
          return HTTPWebSocketsHandler.HTTPWebSocketsHandler.do_GET(self)
    return http.server.SimpleHTTPRequestHandler.do_GET(self)

  def on_ws_message(self, message):
    if message is None:
        message = ''
    if CombiHandler.debug:
      logging.info('websocket REQUEST  "%s"',str(message))

    response = self.server._marshaled_dispatch(message,path=self.path)
    if CombiHandler.debug:
      logging.info('websocket RESPONSE "%s"',str(response))
    self.send_message(response)

  def on_ws_connected(self):
    logging.info('websocket connected')

  def on_ws_closed(self):
    logging.info('websocket closed')

  # ====================================================================================
  # Execute POST request (RPC/REST)
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def do_POST(self):
    try:
      if CombiHandler.debug:
        logging.debug('++ POST: Path='+self.path+\
                      ' RQ='+self.requestline+\
                      ' (Authenticated='+str(self.authenticated())+')')
      #import pdb; pdb.set_trace()

      if not self.authenticated():
        return self.get_authentication()

      plen = len(self.path)
      if plen >= 15 and self.path[:15] == '/TDBDATA/XMLRPC':
        return xmlrpc.server.SimpleXMLRPCRequestHandler.do_POST(self)
      elif plen >= 16 and self.path[:16] == '/TDBDATA/JSONRPC':
        return jsonrpc.server.SimpleJSONRPCRequestHandler.do_POST(self)
      elif plen >= 19 and self.path[:19] == '/TDBDATA/REST/json/':
        return self.rest_json_handle_request(self.path[19:])
      return http.server.SimpleHTTPRequestHandler.do_POST(self)
    except Exception as X:
      logging.error('POST: Exception: '+str(X))

  # ====================================================================================
  # REST API: Helper to create fault replies
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def rest_convert_fault(self, code, message):
    idq = message.find(' | Exception:')
    if idq > 0: message = message[idq+3:]
    #idx = message.find(' | ')
    #idq = message.find(':')
    #if idx >= 0 and idq >= 0:
    #  message = message[:idq+1] + message[idx:]
    resp = {'error': {'message': message, 'code': code}}
    return jsonrpc.client.jdumps(resp)

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
        target = jsonrpc.client.JSONTarget()
        if isinstance(L,list) or isinstance(L,tuple):
          for d in L: target.feed(d)
        else:
          target.feed(L)
        data = target.close()
        params = jsonrpc.client.jloads(data)
        if CombiHandler.debug:
          logging.info('REST: Method: %s Args[%s]: %s'%(method, str(params.__class__), str(params),))
        disp = self.server.get_dispatcher('/TDBDATA/JSONRPC')
        resp = disp._dispatch(method,params)
        if resp.__class__ == str:
          response = jsonrpc.client.jdumps(resp)
        elif resp.__class__ == jsonrpc.client.Fault:
          response = self.rest_convert_fault(resp.faultCode, resp.faultString)
        else:
          response = jsonrpc.client.jdumps({'data': resp})
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
class CombiServer(socketserver.ThreadingMixIn,
                  xmlrpc.server.MultiPathXMLRPCServer):

  # ====================================================================================
  # Enable multi-threading for the server
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def enableThreading(self, value):
    if not hasattr(self,'pool'):
      self.pool = ThreadPool(value)
    self.num_threads = value

  # ====================================================================================
  # En/disable authentication
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def enableAuthentication(self, value):
    self.authentication = value

  # ====================================================================================
  # Register server instance with a given path and of a given type
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def register_instance(self, instance, path, type, allow_dotted_names=False):
    xmlrpc.server.SimpleXMLRPCRequestHandler.rpc_paths.append(path)
    if type.lower() == 'jsonrpc':
      disp = jsonrpc.server.SimpleJSONRPCDispatcher(encoding=self.encoding)
      disp.register_instance (instance, allow_dotted_names)
      self.add_dispatcher(path, disp)
    elif type.lower() == 'xmlrpc':
      disp = xmlrpc.server.SimpleXMLRPCDispatcher(self.allow_none, self.encoding)
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
    return xmlrpc.server.MultiPathXMLRPCServer._marshaled_dispatch(self, data, dispatch_method, path)

  # ====================================================================================
  # Server process_request method.
  # Apply multi server approach if configured
  #
  # \author  M.Frank
  # \version 1.0
  # ------------------------------------------------------------------------------------
  def process_request(self, request, client_address):
    if self.num_threads > 0:
      return self.pool.add_task(xmlrpc.server.MultiPathXMLRPCServer.process_request, self, request, client_address)
    elif self.num_threads < 0:
      return socketserver.ThreadingMixIn.process_request(self, request, client_address)
    xmlrpc.server.MultiPathXMLRPCServer.process_request(self, request, client_address)

# ======================================================================================
# Really annoying, but nevertheless important:
# Required changes to jsonrpclib, which does not obeye the SimpleXMLRPC interface.
#
# \author  M.Frank
# \version 1.0
# --------------------------------------------------------------------------------------
def install_fixes():
  pass
"""
  original_json_disp = jsonrpc.server.SimpleJSONRPCDispatcher._marshaled_dispatch

  def fix_json_dispatch(self, data, dispatch_method=None, path=None):
    return original_json_disp(self, data, dispatch_method)

  def fix_json_do_POST(self):
    if not self.is_rpc_path_valid():
      self.report_404()
      return
    try:
      max_chunk_size = 10*1024*1024
      size_remaining = int(self.headers["content-length"])
      L = []
      while size_remaining:
        chunk_size = min(size_remaining, max_chunk_size)
        L.append(self.rfile.read(chunk_size))
        size_remaining -= len(L[-1])
      #import pdb; pdb.set_trace()
      data = self.decode_request_content(L)
      response = self.server._marshaled_dispatch(data, dispatch_method=None, path=self.path)
      #print('Response:'+str(response))
      self.send_response(200)
    except Exception:
      self.send_response(500)
      err_lines = traceback.format_exc().splitlines()
      trace_string = '%s | %s' % (err_lines[-3], err_lines[-1])
      fault = jsonrpclib.Fault(-32603, 'Server error: %s' % trace_string)
      response = fault.response()
    if response is None:
      response = ''
    self.send_header("Content-type", "application/json-rpc")
    self.send_header("Content-length", str(len(response)))
    self.end_headers()
    self.wfile.write(response)
    self.wfile.flush()
    self.connection.shutdown(1)

  jsonrpc.server.SimpleJSONRPCDispatcher._marshaled_dispatch = fix_json_dispatch
  jsonrpc.server.SimpleJSONRPCRequestHandler.do_POST         = fix_json_do_POST
"""  
# ======================================================================================
# Main entry point: Start server instance
#
# \author  M.Frank
# \version 1.0
# --------------------------------------------------------------------------------------
def run_server(opts):
  install_fixes()
  xmlrpc.server.SimpleXMLRPCRequestHandler.rpc_paths = []
  taskdb   = DB(opts)

  if opts.init:
    taskdb.rebuild()

  if opts.run:
    CombiHandler.debug = opts.debug


    server  = CombiServer(addr=(opts.hostname, opts.port),
                          logRequests=False,
                          requestHandler=CombiHandler)
    # If requested enable SSL (see subdirectory ssl for keys+certificates)
    if opts.ssl:
      import ssl
      server.socket = ssl.wrap_socket(server.socket,
                                      keyfile=opts.keyfile,
                                      certfile=opts.certfile,
                                      server_side=True)
    server.enableThreading(opts.threads)
    server.enableAuthentication(opts.authenticate)
    logging.info('++ Instantiated %s RPC server.'%(opts.type,))
    server.register_instance(taskdb, '/TDBDATA/JSONRPC', 'jsonrpc')
    server.register_instance(taskdb, '/TDBDATA/XMLRPC',  'xmlrpc')
    logging.info('++ Server ready to handle %s requests. Host=%s  Port=%d'%\
                 (opts.type.upper(), opts.hostname, opts.port,))

    try:
      server.serve_forever()
    except KeyboardInterrupt:
      taskdb.shutdown()

# Start from the command line
if __name__ == "__main__":
  import optparse
  parser = optparse.OptionParser()
  parser.description = "TaskDB JSON/XML RPC Server."
  parser.formatter.width = 132
  #
  #  Debug options
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
  parser.add_option("-c", "--cache", 
                    action='store_true',
                    dest="cache", default=False,
                    help="Run cache server version",
                    metavar="<boolean>")
  parser.add_option("-m", "--multi-threaded", 
                    dest="threads", default=0,
                    help="Run in threaded mode. Supply number of workers.[-m 0 Single threaded, -m -1 all you can get]",
                    metavar="<integer>")
  parser.add_option("-r", "--run", 
                    action='store_true',
                    dest="run", default=False,
                    help="Run the server instance",
                    metavar="<boolean>")
  parser.add_option("-s", "--source",
                    dest="database",
                    default='./taskdb_test.db',
                    help="DB file name",
                    metavar="<string>")
  parser.add_option("-t", "--type",
                    dest="type",
                    default='jsonrpc',
                    help="Server type",
                    metavar="<string>")
  parser.add_option("-H", "--hostname",
                    dest="hostname",
                    default='localhost',
                    help="Socket bind address",
                    metavar="<string>")
  parser.add_option("-p", "--port",
                    dest="port",
                    default=8081,
                    help="Server port",
                    metavar="<integer>")
  #
  # Test options: allow DB initialization
  parser.add_option("-T", "--test",
                    dest="test",
                    default=False,
                    action='store_true',
                    help="Set test mode",
                    metavar="<boolean>")
  parser.add_option("-i", "--init", 
                    action='store_true',
                    dest="init", default=False,
                    help="Initialize/create new database",
                    metavar="<boolean>")
  #
  # Security options: SSL configuration
  parser.add_option("-S", "--ssl", 
                    action='store_true',
                    dest="ssl", default=False,
                    help="Run server with secure protocol (SSL)",
                    metavar="<boolean>")
  parser.add_option("-C", "--certfile", 
                    dest="certfile", default='./ssl/server.crt',
                    help="Path to SSL server certificate (needs key) or pem (=cert+key)",
                    metavar="<string>")
  parser.add_option("-K", "--keyfile", 
                    dest="keyfile", default='./ssl/server.key',
                    help="Path to SSL server key file",
                    metavar="<string>")
  #
  # Security options: Authentication
  parser.add_option("-A", "--authentication", 
                    action='store_true',
                    dest="authenticate", default=False,
                    help="Require user authentication",
                    metavar="<boolean>")
  
  (opts, args) = parser.parse_args()
  opts.port    = int(opts.port)
  opts.threads = int(opts.threads)
  if opts.start_pdb:
    import pdb
    pdb.set_trace()
  run_server(opts)
