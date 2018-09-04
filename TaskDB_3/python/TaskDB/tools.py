import time, logging

class Task:
  def __init__(self, dic):
    self.task = dic
  def name(self):
    return self.task['task']
  def description(self):
    return self.task['description']
  def utgid(self):
    return self.task['utgid']
  def command(self):
    return self.task['command']
  def params(self):
    return self.task['task_parameters']
  def cmd_params(self):
    return self.task['command_parameters']

class TaskSet:
  def __init__(self, dic):
    self.taskset = dic
    self.tasks   = []
  def name(self):
    return self.taskset['task_set']
  def description(self):
    return self.taskset['description']

class NodeClass:
  def __init__(self, dic):
    self.cls = dic
    self.sets = []
  def name(self):
    return self.cls['node_class']
  def description(self):
    return self.cls['description']

class NodeType:
  def __init__(self, dic):
    self.type = dic
    self.classes = []
  def name(self):
    return self.type['regex']
  def regex(self):
    return self.type['regex']
  def description(self):
    return self.type['description']
  def match(self, test):
    import re
    match_result = re.match(self.regex().upper()+'X', test.upper()+'X')
    if match_result:
      if match_result.pos == 0 and match_result.endpos == len(test)+1:
        return True
    return False
    
class Cache:
  class Data:
    def __init__(self):
      self.timeStamp   = 0
      self.tasks   = {}
      self.sets    = {}
      self.classes = {}
      self.types   = {}

  def __init__(self):
      pass

  def compress(self, data):
    import gzip, base64, xmlrpc.client
    s = xmlrpc.client.dumps((data,))
    c = gzip.zlib.compress(s.encode())
    r = base64.b64encode(c).decode()
    #logging.info('Compress: Raw data length: %d bytes. Zipped:%d bytes. Encoded:%d bytes'%\
    #             (len(s), len(c), len(r),))
    #logging.info('Compress: # Types: %d # Classes: %d #Sets: %d  #YTasks: %d'%\
    #             (len(data.types), len(data.classes), len(data.sets), len(data.tasks),))
    return r

  def decompress(self, data):
    import gzip, base64, xmlrpc.client
    c = base64.b64decode(data)
    u = gzip.zlib.decompress(c)
    o = xmlrpc.client.loads(u)[0]
    #logging.info('Decompress: Compressed data length: %d bytes. Decoded:%d bytes Unzipped:%d bytes'%\
    #             (len(data), len(c), len(o),))
    data = Cache.Data()
    for i in o:
      for k,v in i.items():
        #print k, v.__class__
        if k == 'tasks':
          for a,b in v.items():
              data.tasks[a]   = Task(b['task'])
        elif k == 'sets':
          for a,b in v.items():
              s = TaskSet(b['taskset'])
              s.tasks = b['tasks']
              data.sets[a]    = s
        elif k == 'classes':
          for a,b in v.items():
              c = NodeClass(b['cls'])
              c.sets = b['sets']
              data.classes[a] = c
        elif k == 'types':
          for a,b in v.items():
              t = NodeType(b['type'])
              t.classes = b['classes']
              data.types[a]   = t
        elif k == 'timeStamp':
          data.timeStamp = int(v)
    #logging.info('Decompress: # Types: %d # Classes: %d #Sets: %d  #YTasks: %d'%\
    #             (len(data.types), len(data.classes), len(data.sets), len(data.tasks),))
    u = None
    return data


  def build(self, rpc, printout=None):
    data = Cache.Data()
    objects = rpc.getTask('*')
    for t in objects:
      task = Task(t)
      data.tasks[task.name()] = task
      if printout:
        logging.info('Processing Task: %s'%(task.name(),))

    objects = rpc.getSet('*')
    for s in objects:
      ts = TaskSet(s)
      if printout:
        logging.info('Processing task set: %s'%(ts.name(),))
      tasks = rpc.tasksInSet(ts.name())
      for t in tasks:  ts.tasks.append(t)
      data.sets[ts.name()] = ts
 
    objects = rpc.getClass('*')
    for c in objects:
      cls = NodeClass(c)
      if printout:
        logging.info('Processing node class: %s'%(cls.name(),))
      sets = rpc.tasksetsInClass(cls.name())
      for s in sets:  cls.sets.append(s)
      data.classes[cls.name()] = cls
 
    objects = rpc.getNode('*')
    for t in objects:
      typ = NodeType(t)
      if printout:
        logging.info('Processing node type: %s'%(typ.regex(),))
      cls = rpc.nodeclassInNode(typ.regex())
      for c in cls:  typ.classes.append(c)
      data.types[typ.name()] = typ
    data.timeStamp = int(time.time())
    return data
