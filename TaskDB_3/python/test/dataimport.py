"""


ssh hltb08     "cd `pwd` && python -c \"import dataflowBoot as db;db.start()\"" > HLT.py
ssh storectl01 "cd `pwd` && python -c \"import dataflowBoot as db;db.start()\"" > STORAGE.py
ssh mona08     "cd `pwd` && python -c \"import dataflowBoot as db;db.start()\"" > MONITORING.py
ssh mona09     "cd `pwd` && python -c \"import dataflowBoot as db;db.start()\"" > RECONSTRUCTION.py
ssh mona10     "cd `pwd` && python -c \"import dataflowBoot as db;db.start()\"" > DQ.py
ssh cald07     "cd `pwd` && python -c \"import dataflowBoot as db;db.start()\"" > CALD07.py
ssh hlt01      "cd `pwd` && python -c \"import dataflowBoot as db;db.start()\"" > HLT01.py
ssh fmc01      "cd `pwd` && python -c \"import dataflowBoot as db;db.start()\"" > FMC01.py
ssh ecs03      "cd `pwd` && python -c \"import dataflowBoot as db;db.start()\"" > ECS03.py
ssh ecs04      "cd `pwd` && python -c \"import dataflowBoot as db;db.start()\"" > ECS04.py
ssh logsrv01   "cd `pwd` && python -c \"import dataflowBoot as db;db.start()\"" > LOGSRV.py

scp *.py pcmf005:SW/TaskDB/test/




"""

# Stress test:
import imp, time, TaskDB


class ImportServer:
  def __init__(self):
    self.rpc = TaskDB.connect('jsonrpc', 'http://localhost:8081/TDBDATA/JSONRPC')
    self.errors = 0;

  def __del__(self):
    print 100*'='
    print 5*'=','  Import complete. %d errors occurred.'%(self.errors,)
    print 100*'='
  
  def log(self, a, b, c):
    print '%-16s  %-24s %s'%(str(a),str(b),str(c),)
    
  def getTask(self, task):
    return self.rpc.getTask(task)
  
  def addTask(self, arg):
    try:
      t = arg['task']
      self.log('AddTask:', self.rpc.addTask(arg), t)
    except Exception, e:
      fail = False
      try:
        task = self.getTask(t)[0]
        for k in task.keys():
          if not (arg.has_key(k) and arg[k] == task[k]):
            print '\n\n                   ', k
            fail = True
      except Exception,X:
        fail = True
      if fail:
        print 'Exception(AddTask):', str(e)
        print '                   ', arg
        print '                   ', str(task),'\n'
        self.errors = self.errors + 1
      else:
        self.log('AddTask','(Identical Exists)', t)
      
  def assignTask(self,task, task_set):
    try:
      self.log('AssignTask:', self.rpc.assignTask(task, task_set), task_set+' --> '+task)
    except Exception, e:
      try:
        tasks = self.rpc.tasksInSet(task_set)
        for t in tasks:
          if t['task'] == task:
            self.log('AssignTask','(Identical Exists)', task_set+' --> '+task)
            return
      except Exception,X:
        print str(X)
      self.errors = self.errors + 1
      print 'Exception(AssignTask):', str(e)
        
  def addSet(self, arg):
    try:
      self.log('AddSet:', self.rpc.addSet(arg), arg['task_set'])
    except Exception, e:
      fail = False
      try:
        s = arg['task_set']
        sets = self.rpc.getSet(s)[0]
        for k in sets.keys():
          if not (arg.has_key(k) and arg[k] == sets[k]):
            print '\n\n                   ', k
            fail = True
      except Exception,X:
        fail = True
      if fail:
        print 'Exception(AddSet):', str(e)
        self.errors = self.errors + 1
      else:
        self.log('AddSet','(Identical Exists)', s)
      
  def assignSet(self,task_set,node_class):
    try:
      self.log('AssignSet:', self.rpc.assignSet(task_set, node_class), node_class+' --> '+task_set)
    except Exception, e:
      try:
        task_sets = self.rpc.tasksetsInClass(node_class)
        for t in task_sets:
          if t['task_set'] == task_set:
            self.log('AssignSet','(Identical Exists)', node_class+' --> '+task_set)
            return
      except Exception,X:
        print str(X)
    
      self.errors = self.errors + 1
      print 'Exception(AssignSet):', str(e)
     
  def addClass(self, arg):
    try:
      self.log('AddClass:', self.rpc.addClass(arg), arg['node_class'])
    except Exception, e:
      fail = False
      try:
        s = arg['node_class']
        sets = self.rpc.getClass(s)[0]
        for k in sets.keys():
          if not (arg.has_key(k) and arg[k] == sets[k]):
            print '\n\n                   ', k
            fail = True
      except Exception,X:
        fail = True
      if fail:
        print 'Exception(AddClass):', str(e)
        self.errors = self.errors + 1
      else:
        self.log('AddClass','(Identical Exists)', s)
      
  def assignClass(self,node_class,regex):
    try:
      self.log('AssignClass:', self.rpc.assignClass(node_class, regex), regex+' --> '+node_class)
    except Exception, e:
      try:
        node_classs = self.rpc.nodeclassInNode(regex)
        for t in node_classs:
          if t['node_class'] == node_class:
            self.log('AssignClass','(Identical Exists)', regex+' --> '+node_class)
            return
      except Exception,X:
        print str(X)
        self.errors = self.errors + 1
      print 'Exception(AssignClass):', str(e)
     
  def addNode(self, arg):
    try:
      self.log('AddNode:', self.rpc.addNode(arg), arg['regex'])
    except Exception, e:
      fail = False
      try:
        s = arg['regex']
        sets = self.rpc.getNode(s)[0]
        for k in sets.keys():
          if not (arg.has_key(k) and arg[k] == sets[k]):
            print '\n\n                   ', k
            fail = True
      except Exception,X:
        fail = True
      if fail:
        print 'Exception(AddNode):', str(e)
        self.errors = self.errors + 1
      else:
        self.log('AddNode','(Identical Exists)', s)
      

def import_data(server, name):
  mod = imp.load_source(name,'./test/'+name+'.py')
  print 100*'='
  print 5*'=', '  Now processing: '+name
  print 100*'='
  mod.func(server)
  
def test_all():
  rpc = ImportServer()
  """
  import_data(rpc, 'HLT')
  import_data(rpc, 'STORAGE')
  import_data(rpc, 'MONITORING')
  import_data(rpc, 'RECONSTRUCTION')
  import_data(rpc, 'DQ')
  import_data(rpc, 'CALD07')
  import_data(rpc, 'HLT01')
  import_data(rpc, 'FMC01')
  import_data(rpc, 'ECS03')
  import_data(rpc, 'ECS04')
  import_data(rpc, 'LOGSRV')
  """
  import_data(rpc, 'ALL')
  #import_data(rpc, '')

def test_one():
  rpc = ImportServer()
  import_data(rpc, 'ECS03')


if __name__ == "__main__":
  test_all()
