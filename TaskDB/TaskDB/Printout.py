def printObjects(type, key, objects):
    cnt = 0
    for i in objects:
        cnt = cnt+1
        pref = '%s.%-3d'%(type, cnt,)
        print '%-16s:      %s'%(pref,i[key],)
        for k in i.keys():
            if k != key:
                print '%-16s:      %-16s %s'%('',k+':',i[k],)
    
def printTasks(objects):      printObjects('Task',      'task',          objects)
def printTaskSets(objects):   printObjects('TaskSet',   'task_set',      objects)
def printClasses(objects):    printObjects('NodeClass', 'node_class',    objects)
def printNodeTypes(objects):  printObjects('NodeType',  'regex',         objects)
