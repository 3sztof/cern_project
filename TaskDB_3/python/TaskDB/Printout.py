def printObjects(type, key, objects):
  cnt = 0
  for i in objects:
    cnt = cnt+1
    pref = '%s.%-3d'%(type, cnt,)
    print ('%-16s:      %s'%(pref,i[key],))
    for k in i.keys():
      if k != key:
        print ('%-16s:      %-16s %s'%('',k+':',i[k],))

def printTasks(objects):      printObjects('Task',      'task',          objects)
def printTaskSets(objects):   printObjects('TaskSet',   'task_set',      objects)
def printClasses(objects):    printObjects('NodeClass', 'node_class',    objects)
def printNodeTypes(objects):  printObjects('NodeType',  'regex',         objects)

replacements = {}
replacements['<TOP_LOGGER>'] = 'ecs03'
replacements['<SCRIPTS>'] = '/group/online/dataflow/scripts'

def printTasksFMC(node, tasks):
  for t in tasks:
    cmd = 'pcAdd -u %s %s %s %s'%(t['utgid'],t['task_parameters'],t['command'],t['command_parameters'],)
    cmd = cmd.replace('<NODE>',node)
    for k,v in replacements.items():
      cmd = cmd.replace(k,v)
    print (cmd)

