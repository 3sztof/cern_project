# =========================================================================================================================
#                     LHCb Online Farm Process Explorer
#                                 Main API 
#                           
#                           M.Frank 08.2018
# =========================================================================================================================
import os, time, logging, threading
import tools

SUCCESS = 'Success'
FAILED  = 'FAILED'

NONE    = 0
SIMPLE  = 1
FULL    = 2
EXCEPTION_HANDLING = SIMPLE


# =========================================================================================================================
# Main user claqss to interact with the task database
# This class is supposed to shield any application from the
# nitty gritty details of the underlying SQL implementation.
#
# \author  M.Frank
# \version 1.0
# -------------------------------------------------------------------------------------------------------------------------
class TaskDB:

    # =====================================================================================================================
    # Standard object constructor
    #
    # \author  M.Frank
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def __init__(self, connection, dbg=False):
        # import pdb; pdb.set_trace()
        # Configs
        self.dbg        = dbg
        self.rpc        = connection
        self.connection = connection
        #self.data       = tools.Cache().build(connection)
        while True:
            try:
                data            = self.connection.getCache(0)
                self.data       = tools.Cache().decompress(data)
                break
            except Exception as e:
                logging.info('++ Failed to connect server for cache. Re-trying. '+str(e))
            time.sleep(15)
        self.dirty      = False
        self.lock       = threading.Lock()
        self.timeStamp  = int(time.time())
        self.nodeTasks  = {}
        
    def reconnect(self, arg=None):
        conn  = self.connection
        data  = conn.getCache(0)
        obj   = tools.Cache().decompress(data)

        self.lock.acquire()
        self.timeStamp = int(time.time())
        self.nodeTasks = {}
        self.data      = obj
        self.dirty     = False
        self.lock.release()
        return 'Success'

    # =====================================================================================================================
    # Helper function to allow generic and common exception handling for the 
    # whole interface. The actual behaviour depends on the value of the 
    # module parameter EXCEPTION_HANDLING (NONE, SIMPLE, FULL)
    #
    # \param   exception         [REQUIRED]  Python exception object to be handled
    # \param   value             [OPTIONAL]  Optional explanation of the exception cause
    # \return  None                          Erro code or exception raises
    #
    # \author  M.Frank
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def handleException(self, exception, value=None):
        if self.dbg: logging.info('++         '+str(exception))
        if EXCEPTION_HANDLING == SIMPLE:
            if value: raise Exception(value)
            raise Exception(str(exception))
        elif EXCEPTION_HANDLING == FULL:
            e = str(exception)
            err = ''
            if value: err = err + value + '\n'
            err = err + e
            raise Exception(err)
        elif EXCEPTION_HANDLING == NONE:
            if value: return value
            return str(exception)
        if value: raise Exception(value)
        raise Exception(str(exception))

    # =====================================================================================================================
    # Access an existing Tasks instance in the Tasks table
    # 
    # \param  task               [REQUIRED]  Name of the task to be accessed
    # \return                                The desired object in JSON format / Exception
    #
    # \author  M.Frank
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def getTask(self, task):
        data = self.data
        if not data:
            raise KeyError('Task "%s" is not present in the database.'%(task,))
        try:
            if task == '*':
                #import pdb; pdb.set_trace()
                return [v.task for k,v in data.tasks.items()]
            return [data.tasks[task].task]
        except Exception as e:
            return self.handleException(e,'Failed to access task "%s" in database'%(task,))

    # =====================================================================================================================
    # Query the tasks assigned to a given task set
    # 
    # \param  task_set           [REQUIRED]  Name of the task set the task should be queried
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  M.Frank
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def tasksInSet(self, task_set):
        data = self.data
        if not(data is not None and data.sets.get(task_set,None) is not None):
            raise KeyError('Taskset "%s" is not present in the database.'%(task,))
        return data.sets[task_set].tasks

    # =====================================================================================================================
    # Access an existing Tasks instance in the Tasks table.
    # 
    # \param  task               [REQUIRED]  Name of the task to be accessed
    # \return                                The desired object in JSON format / Exception
    #
    # \author  M.Frank
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def getSet(self, task_set):
        data = self.data
        if not data:
            raise KeyError('Taskset "%s" is not present in the database.'%(task_set,))
        try:
            if(task_set == '*'):
                return [v.taskset for k,v in data.sets.items()]
            return [data.sets[task_set].taskset]
        except Exception as e:
            return self.handleException(e,'Failed to access taskset "%s" in database'%(task_set,))

    # =====================================================================================================================
    # Query the task sets assigned to a given node class
    # 
    # \param  task_set           [REQUIRED]  Name of the node class to be queried
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  M.Frank
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def tasksetsInClass(self, node_class):
        data = self.data
        if not (data is not None and data.classes.get(node_class,None) is not None):
            raise KeyError('Node-class "%s" is not present in the database.'%(node_class,))
        return data.classes[node_class].sets

    # =====================================================================================================================
    # Access an existing node class instance in the Classes table.
    # 
    # \param  node_class         [REQUIRED]  Name of the node class to be accessed
    # \return                                The desired object in JSON format / Exception
    #
    # \author  M.Frank
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def getClass(self, node_class):
        try:
            data = self.data
            if node_class == '*':
                return [v.cls for k,v in data.classes.items()]
            return [data.classes[node_class].cls]
        except Exception as e:
            return self.handleException(e,'Failed to access the node class "%s" in database'%(node_class,))

    # =====================================================================================================================
    # Query the node classes assigned to a given node type
    # 
    # \param  regex              [REQUIRED]  Name of the node type of which the assigned objects should be returned
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  M.Frank
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def nodeclassInNode(self, regex):
        data = self.data
        if not (data is not None and data.types.get(regex,None) is not None):
            raise KeyError('Node-type "%s" is not present in the database.'%(regex,))
        return data.types[regex].classes

    # =====================================================================================================================
    # Access an existing node type instance in the Nodes table.
    # 
    # \param  regex              [REQUIRED]  Name of the node type  to be accessed
    # \return                                The desired object in JSON format / Exception
    #
    # \author  M.Frank
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def getNode(self, regex):
        try:
            data = self.data
            if(regex == '*'):
                return [v.type for k,v in data.types.items()]
            return [data.classes[regex].taskset]
        except Exception as e:
            return self.handleException(e,'Failed to access the node type "%s" in database'%(regex,))

    # =====================================================================================================================
    # Access all tasks for a given node
    # 
    # \param  node               [REQUIRED]  Name of the node to be accessed
    # \return                                The desired objects in JSON format / Exception
    #
    # \author  M.Frank
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def getTasksByNode(self, node):
        tasks = self.nodeTasks
        if tasks.get(node,None) is not None:
            return tasks[node]
        data = self.data
        result = []
        for typ, node_typ in data.types.items():
            if node_typ.match(node):
                if self.dbg:
                    logging.info('Regex %s matched node type: %s'%(node_typ.regex(), node,))
                for c in node_typ.classes:
                    cls = data.classes[c['node_class']]
                    if self.dbg:
                        logging.info('\t----> Node class: %s'%(cls.name(),))
                    for s in cls.sets:
                        ts = data.sets[s['task_set']]
                        if self.dbg:
                            logging.info('\t\t----> Task set: %s'%(ts.name(),))
                        for t in ts.tasks:
                            task = data.tasks[t['task']]
                            result.append(task.task)
                            if self.dbg:
                                logging.info('\t\t\t----> Task: %s'%(task.name(),))
        self.lock.acquire()
        self.nodeTasks[node] = result
        self.lock.release()
        return result
