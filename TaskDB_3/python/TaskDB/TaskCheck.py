from __future__ import print_function
import time, pydim, logging

# Configure output logging
logging.basicConfig(format='%(asctime)s %(levelname)-8s: %(message)s',
                    datefmt='%Y-%m-%d %H:%M:%S',
                    level=logging.INFO)

#----------------------------------------------------------------------------------------
class TaskManagerClient:

  #--------------------------------------------------------------------------------------
  def __init__(self, opts):
    import xmlrpclib as rpcclient
    import threading
    name = '/FMC/'+self.__host.upper()+'/task_manager/'
    self.__timeout = opts.timeout
    self.__dns     = opts.dns
    self.__host    = opts.hostname
    self.__srvConn = opts.connection
    self.__server  = rpcclient.ServerProxy(self.__srvConn)
    self.__lock    = threading.Lock()
    self.__taskDB  = None
    self.__tasks   = None
    self.__port    = 3502
    self.__infoID   = pydim.dic_info_service(name+'longList','C',self.inquire)
    self.__killID   = name+'kill'
    self.__startID  = name+'start'
    self.tmConnection = 0
    self.dbConnection = int(time.time()) # This ensures a data update from DB

  #--------------------------------------------------------------------------------------
  def tm_answer(self, tag, ret):
    #print ('The client callback function has received tag %d and return code %d' %(tag, ret))
    pass

  #--------------------------------------------------------------------------------------
  def kill(self, task):
    cmd = ''+task['utgid']
    args = ( cmd, )
    res = pydim.dic_cmnd_callback(self.__killID, args, 'C', self.tm_answer, 1)
    logging.warning('++ Kill  Task:  '+cmd)
    
  #--------------------------------------------------------------------------------------
  def start(self, task):
    cmd = '-u %s %s %s %s'%(task['utgid'],task['task_parameters'],task['command'],task['command_parameters'],)
    cmd = cmd.replace('<TOP_LOGGER>', 'ecs03')     \
             .replace('<NODE>',       self.__host) \
             .replace('<DIM_DNS>',    self.__dns)  \
             .replace('<SCRIPTS>',    '/group/online/dataflow/scripts')
    args = ( cmd, )
    res = pydim.dic_cmnd_callback(self.__startID, args, 'C', self.tm_answer, 2)
    logging.warning('++ Start Task:  '+cmd)
    
  #--------------------------------------------------------------------------------------
  def inquire(self, *args):
    #logging.info('++ TaskManagerClient callback. Args are %s'%str(args))
    if len(args) > 1:
      try:
        r = args[1]
        items = r.split('\0')
        tasks = {}
        task  = {'state': 'terminated'}
        count = 0
        for r in items:
          rest = count%6
          if 0 == rest:
            task = {}
            task['pid'] = r
          elif 1 == rest:
            task['exe'] = r
          elif 2 == rest:
            task['utgid'] = r
          elif 3 == rest:
            task['time'] = r
          elif 4 == rest:
            task['state'] = r
          elif 5 == rest:  #   Terminator
            if task['state'].find('terminated') < 0:
              u = task['utgid']
              tasks[u] = task
              #if u[0] == 'A': logging.info('\t %s  -> %s'%(u,str(task),))
          count = count + 1

        self.__lock.acquire()
        self.__tasks = tasks
        self.tmConnection = time.time()
        self.__lock.release()
        logging.debug('++ Host %s received task manager information (%d tasks)'%(self.__host,len(self.__tasks),))
      except Exception as e:
        logging.error('++ Invalid task manager response: '+str(e))

  #--------------------------------------------------------------------------------------
  def getTasks(self):
    import copy
    self.__lock.acquire()
    tasks = copy.deepcopy(self.__tasks)
    self.__lock.release()
    return tasks

  #--------------------------------------------------------------------------------------
  def loadTaskFromDB(self, tasks_real):
    try:
      print ('Reloading database')
      tasks = self.__server.getTasksByNode(self.__host)
      logging.info('++ Host %s received database information (%d tasks)'%(self.__host,len(tasks),))
      print ('++ Host %s received database information (%d tasks)'%(self.__host,len(tasks),))
      self.dbConnection = int(time.time())
      new_tasks = {}
      for t in tasks:
        new_tasks[t['utgid']] = t

      if self.__taskDB is not None:
        killed = {}
        if tasks_real is not None:
          for k,t in tasks_real.items():
            if self.__taskDB.get(k,None) is not None and new_tasks.get(k,None) is None:
              self.kill(t)
              killed[k] = t
        for k,t in self.__taskDB.items():
          # If the task is not in the new list, kill it!
          if killed.get(k,None) is not None:
            continue
          if new_tasks.get(k,None) is None:
            self.kill(t)
            continue
          # Now check if some option changed:
          tt = new_tasks[k]
          if t['task'] != tt['task'] or \
             t['command'] != tt['command'] or \
             t['task_parameters'] != tt['task_parameters'] or \
             t['command_parameters'] != tt['command_parameters']:
            self.kill(t)
            continue
      self.__taskDB = new_tasks
      return self.__taskDB
    except Exception as e:
      logging.error('++ Invalid DATABASE response: '+str(e))
      import pdb; pdb.set_trace()
      #tasks = self.__server.getTasksByNode(self.__host)
    return self.__taskDB

  #--------------------------------------------------------------------------------------
  def getDB(self, tasks_real):
    srv   = self.__server
    tasks = self.__taskDB
    try:
      if not tasks:
        return self.loadTaskFromDB(tasks_real)
      else:
        res = srv.needUpdate(self.__host, self.__port, {'stamp': self.dbConnection})
        if res:
          tasks = self.loadTaskFromDB(tasks_real)
          self.dbConnection = int(time.time())
          return tasks
        self.dbConnection = int(time.time())
    except Exception as e:
      logging.error('++ Invalid DATABASE check : '+str(e))
    return tasks

  #--------------------------------------------------------------------------------------
  def checkTasks(self):
    import math
    try:
      now        = time.time()
      tasks_real = self.getTasks()
      tasks_db   = self.getDB(tasks_real)
      diff_db    = math.fabs(now - self.dbConnection)
      diff_tm    = math.fabs(now - self.tmConnection)
      len_db     = None
      len_tm     = None
      if tasks_db is not None: len_db  = len(tasks_db)
      if tasks_real  is not None: len_tm  = len(tasks_real)
      logging.info('++ Checking task status on node %s.  DB:%s tasks TM:%s tasks'%\
                   (self.__host,str(len_db), str(len_tm),))
      if   diff_db  > self.__timeout:
        logging.error('++ No response from the DATABASE     since at least %.1f seconds'%(diff_db,))
      elif diff_tm  > self.__timeout:
        logging.error('++ No response from the TASK_MANAGER since at least %.1f seconds'%(diff_tm,))

      if tasks_real and tasks_db:
        missing = 0
        for k,t in tasks_db.items():
          if not tasks_real.has_key(k):
            self.start(t)
            missing = missing + 1
            continue
    except Exception as e:
      logging.error('++ Exception(check) : '+str(e))

#----------------------------------------------------------------------------------------
def start_task_checker():
  import os, socket, optparse
  dns   = os.environ['DIM_DNS_NODE']
  host  = socket.gethostname()
  srv = 'http://'+dns+':3501/TDBDATA/XMLRPC'
  utgid = 'UTGID'
  if os.environ.has_key('UTGID'): utgid = os.environ['UTGID']

  parser = optparse.OptionParser()
  parser.description = "TaskDB checker."
  parser.formatter.width = 132
  parser.add_option("-d", "--debug", 
                    action='store_true',
                    dest="debug", default=False,
                    help="Enable output debug statements. Default=False",
                    metavar="<boolean>")
  parser.add_option("-P", "--pdb", 
                    action='store_true',
                    dest="run_pdb", default=False,
                    help="Start python debugger. Default=False",
                    metavar="<boolean>")
  parser.add_option("-c", "--connection",
                    dest="connection",
                    default=srv,
                    help="TaskDB server to be contacted. Default="+srv,
                    metavar="<string>")
  parser.add_option("-D", "--dim-dns",
                    dest="dns",
                    default=dns,
                    help="DIM DNS domain. Default="+dns,
                    metavar="<string>")
  parser.add_option("-H", "--hostname",
                    dest="hostname",
                    default=host,
                    help="Host name. Default="+host,
                    metavar="<string>")
  parser.add_option("-u", "--utgid",
                    dest="utgid",
                    default=host,
                    help="UTGID. Default="+utgid,
                    metavar="<string>")
  parser.add_option("-i", "--interval",
                    dest="interval",
                    default=10.0,
                    help="Scan timeout Default=10 seconds",
                    metavar="<string>")
  parser.add_option("-m", "--max-timeout",
                    dest="timeout",
                    default=30.0,
                    help="Max. timeout without answer drom task "+\
                    "manager or database. Default:30 seconds",
                    metavar="<string>")

  (opts, args) = parser.parse_args()
  if opts.run_pdb:
      import pdb
      pdb.set_trace()
  if opts.debug:
      logging.basicConfig(level=logging.DEBUG)

  c = TaskManagerClient(opts)
  pydim.dis_start_serving(opts.utgid)
  tmo = float(opts.interval)/4.0
  while 1:
    time.sleep(tmo)
    c.checkTasks()
    time.sleep(3*tmo)

if __name__ == "__main__":
    start_task_checker()
