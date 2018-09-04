#!/usr/bin/env python
import os, sys, errno, logging

# =========================================================================================================================
#                    LHCb Online Farm Process Explorer
#      Main API tester - validates api against desired DB changes and errors
#                            
#                           K.Wilczynski 08.2018
# =========================================================================================================================
SUCCESS = 'Success'
logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.INFO)

# Print statements colouring - when script is finished, those guys should make it fancy (last development step)
class bcolors:
    # Colour macros
    HEADER    = '\033[95m'
    OKBLUE    = '\033[94m'
    OKGREEN   = '\033[92m'
    WARNING   = '\033[93m'
    FAIL      = '\033[91m'
    ENDC      = '\033[0m'
    BOLD      = '\033[1m'
    UNDERLINE = '\033[4m'

# ------------------------------------------------------------------------------------------------------
def print_test(msg):
  logging.info(bcolors.UNDERLINE + msg + bcolors.ENDC)
  #print os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called add commands for Tasks table:' + bcolors.ENDC + os.linesep


# ------------------------------------------------------------------------------------------------------
class Test:
  tester = None
  def __init__(self,func,title):
    self.func  = func
    self.title = title
    self.log   = Test.tester.log
    self.error = Test.tester.error
  def report(self,ok):
    if ok:
      self.log(self.func,self.title)
    else:
      self.error(self.func,self.title)


def fillTables(conn):
    c = conn.handle
    # ===================================================================
    #                            TEST DATA
    # ===================================================================
    TASKS = (
        ('Task1', 'task_1', 'sleep1.sh', '4', '-c test1', 'Sleep for 4 seconds'),
        ('Task2', 'task_2', 'sleep1.sh', '3', '-c test2', 'Sleep for 3 seconds'),
        ('Task3', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task4', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task5', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task6', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task7', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task8', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task9', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task10', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task11', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task12', 'task_3', 'sleep1.sh', '7', '', 'Sleep for 7 seconds'),
        ('Task13', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task14', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task15', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task16', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task17', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task18', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task19', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task20', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task21', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task22', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task23', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task24', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task25', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task26', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task27', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task28', 'task_3', 'sleep1.sh', '7', '', 'Sleep for 7 seconds'),
        ('Task29', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task30', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task31', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task32', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
        ('Task33', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
    )

    TASKS_SETS = (
        ('Task1', 'Task_Set_1'),
        ('Task1', 'Task_Set_2'),
        ('Task2', 'Task_Set_2'),
        ('Task2', 'Task_Set_3'),
        ('Task3', 'Task_Set_1'),
    )

    SETS = (
        ('Task_Set_1', 'Contains Task1 and Task3'),
        ('Task_Set_2', 'Containst Task1 and Task2'),
        ('Task_Set_3', 'Containst Task2'),
    )

    SETS_CLASSES = (
        ('Task_Set_1', 'Sleeping_node'),
        ('Task_Set_2', 'Sleeping_node'),
        ('Task_Set_3', 'Sleeping_node'),
    )

    CLASSES = (
        ('Sleeping_node', 'Nodes that are very sleepy'),
    )

    CLASSES_NODES = (
        ('Sleeping_node', 'slp[a-z][0-9][0-9]'),
    )

    NODES = (
        ('slp[a-z][0-9][0-9]', 'Set of nodes blablabla'),
    )
    # ===================================================================
    #                        TEST DATA INJECTION
    # ===================================================================

    for row in TASKS:
        c.execute('insert into Tasks values ("%s","%s","%s","%s","%s","%s")'%row)
    for row in SETS:
        c.execute('insert into Task_Sets values ("%s","%s")'%row)
    for row in TASKS_SETS:
        c.execute('insert into Tasks_to_Task_Sets values ("%s","%s")'%row)
    for row in CLASSES:
        c.execute('insert into Classes values ("%s","%s")'%row)
    for row in SETS_CLASSES:
        c.execute('insert into Task_Sets_to_Classes values ("%s","%s")'%row)
    for row in NODES:
        c.execute('insert into Nodes values ("%s","%s")'%row)
    for row in CLASSES_NODES:
        c.execute('insert into Classes_to_Nodes values ("%s","%s")'%row)

    # ===================================================================
    #                 SAVE AND CLOSE THE DB CONNECTION
    # ===================================================================
    conn.close()
    logging.info('++ Successfully populated the database...')

# ------------------------------------------------------------------------------------------------------
class ApiTest:
    # Constructor
    def __init__(self, opts):
        import TaskDB

        TaskDB.Connection.debug = opts.debug
        Test.tester = self

        # Configs
        self.testCount = 0
        self.errCount  = 0        
        self.database = opts.database

        # Initiate a connection (and a separate test query connection in api object, 
        # cursor object and turn on the constraints to protect the schema (created in initDB.py)
        self.opts = opts
        sslopt = {}
        if opts.certfile:
            sslopt['ca_certs'] = opts.certfile
        typ = opts.type.lower()
        self.local = False
        if typ.find('direct') >= 0 or typ.find('cli') >= 0:
            use_cli = typ.find('cli') >= 0
            # Reinitialize the database
            self.local = True
            conn = self.opts.connection
            if opts.init and use_cli and (conn[:4] == 'http' or conn[:2] == 'ws'):
                self.local = False
                if   self.opts.connection.lower().find('/jsonrpc') >=0: conn_typ = 'jsonrpc'
                elif self.opts.connection.lower().find('/xmlrpc')  >=0: conn_typ = 'xmlrpc'
                c = TaskDB.connect(conn_typ, address=self.opts.connection, sslopt=sslopt)
                c.rebuild()
            elif opts.init:
                import Installer
                Installer.destroy(self.database)
                Installer.install('sqlite:///'+self.database)
                fillTables(TaskDB.Connection(database='sqlite:///'+self.database))
                logging.info('++ The database has been rebuilt ....')
            self.api = TaskDB.connect(typ, address=self.opts.connection, sslopt=sslopt)
            logging.info('++ Connected to interface : ' + self.opts.connection)
            if use_cli:
                self.api.batch = True

        elif typ.find('rpc') >= 0:
            if   self.opts.connection.lower().find('/jsonrpc') >=0: typ = 'jsonrpc'
            elif self.opts.connection.lower().find('/xmlrpc')  >=0: typ = 'xmlrpc'
            self.local = False
            self.api = TaskDB.connect(typ, address=self.opts.connection, sslopt=sslopt)
            self.api.rebuild()
            logging.info('++ Connected to ' + typ + ' RPC server: ' + self.opts.connection)
        else:
            logging.error('++ Undefined execution mode: ' + typ + ' [' + os.strerror(errno.EINVAL) + ']')
            sys.exit(errno.EINVAL)
        logging.info('++ Connected to the database at: ' + self.database)
        self.direct = TaskDB.connect('direct','sqlite:///'+self.database)

    def __del__(self):
        self.database = self.api = self.direct = None
        print ('++ Calling apiTest destructor')

    # ------------------------------------------------------------------------------------------------------
    def run_test(self, func, **args):
        try:
            #print (str(args), self.local)
            if self.local:
                result = func(**args)
            else:
                result = func(args)
            #print str(result)
            return result
        except Exception as X:
            err = str(X)
            idx = err.find('| Exception: ')
            if idx >= 0:
                err = err[idx+len('| Exception: '):-1]
            else:
                idx = err.find('<Fault ')
                if idx >= 0:
                    idq = err[idx:].find('>:')
                    if idq >=0: err = err[idx+idq+2:-1]
            if err[-1] == "'": err = err[:-1]
            logging.info('         ===> Test result (Exception): "'+err+'"')
            return err
    # =====================================================================================================================
    #                         Tester methods
    # =====================================================================================================================

    # Run all of the tests
    def testAll(self):
        self.errCount = 0
        self.testCount = 0

        self.testAdd()
        self.testDelete()
        self.testModify()
        self.testGet()
        self.testAssign()
        self.testUnassign()
        logging.info('')
        if(self.errCount == 0):
            logging.info(bcolors.OKBLUE + 'Finished API testing without any error' + bcolors.ENDC)
        else:
            logging.info(bcolors.FAIL + 'Finished API testing with ' + str(self.errCount) + ' errors' + bcolors.ENDC)
        logging.info('')
        if self.opts.type.lower().find('direct') >= 0 and self.opts.init:
            os.unlink(self.database)
        return self.errCount

    # =====================================================================================================================

    # Run only add methods (specify which table will be optional: Tasks, Task_Sets, Clsses, Nodes)
    def testAdd(self, table='all'):
        
        #                    .d8b.  d8888b. d8888b. 
        #                   d8' `8b 88  `8D 88  `8D 
        #                   88ooo88 88   88 88   88 
        #                   88~~~88 88   88 88   88 
        #                   88   88 88  .8D 88  .8D 
        #                   YP   YP Y8888D' Y8888D' 

        logging.info(os.linesep)
        logging.info('===================================================================')
        logging.info('                       ADD                                         ')
        logging.info('===================================================================')


        # Tasks
        if(table == 'all' or table == '*' or table == 'All' or table == 'Tasks'):
            # Correct method calls tests
            print_test('Running tests to add task rows to Tasks table:')
            # Add task providing all parameters
            itemName = 'testTask' + str(self.testCount)
            result   = self.run_test(self.api.addTask,
                                task=itemName, utgid='testUtgid', command='testCommand', command_parameters='testScriptParams',
                                task_parameters='testPcaddParams', description='testDescription')
            success  = result == SUCCESS and \
                       self.inDb('Tasks', task=itemName, 
                                 utgid='testUtgid', command='testCommand', command_parameters='testScriptParams', 
                                 task_parameters='testPcaddParams', description='testDescription')
            Test('addTask', 'Add a task providing all parameters').report(success)

            # Add task providing only required parameter (rest should be empty strings: '')
            itemName = 'testTask' + str(self.testCount)
            result   = self.run_test(self.api.addTask,task=itemName)
            success  = result == SUCCESS and self.inDb('Tasks',task=itemName)
            Test('addTask', 'Add a task providing only the primary key').report(success)

            # Unique constraint violation test
            itemName = 'testTask' + str(self.testCount)
            self.direct.execStatement("insert into Tasks values ('{0}','','','','','')".format(itemName,))
            present  = "NO"
            if self.inDb('Tasks', task=itemName, utgid='', command='', command_parameters='', 
                         task_parameters='', description=''): present = "YES"
            result   = self.run_test(self.api.addTask,
                                task=itemName, utgid='', command='', command_parameters='', 
                                task_parameters='', description='')
            success  = result != SUCCESS and present == 'YES'
            Test('addTask', 'Add a task violating unique constraint').report(success)

        # =================================================================================================================
        # Task sets
        if(table == 'all' or table == '*' or table == 'All' or table == 'Task_Sets'):
            # Correct method calls tests
            print_test('Running tests to add to Task_Sets table:')

            # Add task set providing all parameters
            itemName = 'testSet' + str(self.testCount)
            result   = self.run_test(self.api.addSet,task_set=itemName, description='testDescription')
            success  = result == SUCCESS and self.inDb('Task_Sets', task_set=itemName, description='testDescription')
            Test('addSet', 'Add a task set providing all parameters').report(success)

            # Add task set providing only required parameter (rest should be empty strings: '')
            itemName = 'testSet' + str(self.testCount)
            result   = self.run_test(self.api.addSet,task_set=itemName)
            in_db    = self.inDb('Task_Sets', task_set=itemName)
            success  = result == SUCCESS and in_db
            Test('addSet', 'Add a task set providing only the primary key').report(success)

            # Unique constraint violation test
            itemName = 'testSet' + str(self.testCount)
            self.direct.execStatement("insert into Task_Sets values ('{0}','')".format(itemName,))
            result   = self.run_test(self.api.addSet,task_set=itemName, description='bla')
            in_db    = self.inDb('Task_Sets', task_set=itemName)
            success  = result != SUCCESS and in_db
            Test('addSet', 'Add a task set violating unique constraint').report(success)

        # =================================================================================================================

        # Node Classes
        if(table == 'all' or table == '*' or table == 'All' or table == 'Classes'):
            # Correct method calls tests
            print_test('Running tests to add to Classes table:')

            # Add node class providing all parameters
            itemName = 'testClass' + str(self.testCount)
            result   = self.run_test(self.api.addClass, node_class=itemName, description='testDescription')
            success  = result == SUCCESS and self.inDb('Classes', node_class=itemName, description='testDescription')
            Test('addClass', 'Add a node class providing all the parameters').report(success)

            # Add node class providing only required parameter (rest should be empty strings: '')
            itemName = 'testClass' + str(self.testCount)
            result   = self.run_test(self.api.addClass, node_class=itemName)
            in_db    = self.inDb('Classes', node_class=itemName)
            success  = result == SUCCESS and in_db
            Test('addClass', 'Add a node class providing only the primary key').report(success)

            # Unique constraint violation test
            itemName = 'testClass' + str(self.testCount)
            self.direct.execStatement("insert into Classes values ('{0}','')".format(itemName,))
            result   = self.run_test(self.api.addClass, node_class=itemName)
            success  = result != SUCCESS and self.inDb('Classes', node_class=itemName)
            Test('addClass', 'Add a node class violating unique constraint').report(success)

        # =================================================================================================================

        # Nodes
        if(table == 'all' or table == '*' or table == 'All' or table == 'Nodes'):
            # Correct method calls tests
            print_test('Running tests to add to Nodes table:')

            # Add node providing all parameters
            itemName = 'testNode' + str(self.testCount)
            result   = self.run_test(self.api.addNode,regex=itemName, description='testDescription')
            success  = result == SUCCESS and self.inDb('Nodes', regex=itemName, description='testDescription')
            Test('addNode', 'Add a node providing all the parameters').report(success)

            # Add task set providing only required parameter (rest should be empty strings: '')
            itemName = 'testNode' + str(self.testCount)
            result   = self.run_test(self.api.addNode,regex=itemName)
            success  = result == SUCCESS and self.inDb('Nodes', regex=itemName)
            Test('addNode', 'Add a node providing only the primary key').report(success)

            # Unique constraint violation test
            itemName = 'testNode' + str(self.testCount)
            self.direct.execStatement("insert into Nodes values ('{0}','')".format(itemName,))
            result   = self.run_test(self.api.addNode,regex=itemName, description='bla')
            success  = result != SUCCESS and self.inDb('Nodes', regex=itemName)
            Test('addNode', 'Add a node violating the unique constraint').report(success)
        
    # =====================================================================================================================

    def testDelete(self, table = 'all'):


        #           d8888b. d88888b db      d88888b d888888b d88888b 
        #           88  `8D 88'     88      88'     `~~88~~' 88'     
        #           88   88 88ooooo 88      88ooooo    88    88ooooo 
        #           88   88 88~~~~~ 88      88~~~~~    88    88~~~~~ 
        #           88  .8D 88.     88booo. 88.        88    88.     
        #           Y8888D' Y88888P Y88888P Y88888P    YP    Y88888P 


        logging.info(1*os.linesep)
        logging.info('===================================================================')
        logging.info('                      DELETE                                       ')
        logging.info('===================================================================')

        # Tasks
        if(table == 'all' or table == '*' or table == 'All' or table == 'Tasks'):
            # Correct method calls tests
            print_test('Running tests of correctly called delete commands for Tasks table:')

            # Delete a task providing the primary key
            itemName = 'testTask' + str(self.testCount)
            self.direct.execStatement("insert into Tasks values ('{0}','','','','','')".format(itemName,))
            result   = self.run_test(self.api.deleteTask,task=itemName)
            success  = result == SUCCESS and not self.inDb('Tasks', task=itemName)
            Test('deleteTask', 'Delete a task providing primary key').report(success)

            # Delete a task and check if dependency in Tasks_to_Task_Sets was deleted
            itemName = 'testTask' + str(self.testCount)
            self.direct.execStatement("insert into Tasks values ('{0}','','','','','')".format(itemName,))
            self.direct.execStatement("insert into Task_Sets values ('{0}','')".format(itemName + 'Set',))
            self.direct.execStatement("insert into Tasks_to_Task_Sets values ('{0}','{1}')".format(itemName, itemName + 'Set'))
            result  = self.run_test(self.api.deleteTask,task=itemName)
            success = result == SUCCESS and not self.inDb('Tasks', task=itemName) and not self.inDb('Tasks_To_Task_Sets', task=itemName, task_set=(itemName+'Set'))
            Test('deleteTask', 'Delete a task and check if dependency in Tasks_to_Task_Sets was deleted').report(success)

            # Delete a non-existing task
            itemName = 'testTask' + str(self.testCount)
            result   = self.run_test(self.api.deleteTask,task=itemName)
            success  = result != SUCCESS and not self.inDb('Tasks', task=itemName)
            Test('deleteTask', 'Delete a task providing primary key').report(success)

    # =================================================================================================================

        # Task Sets
        if(table == 'all' or table == '*' or table == 'All' or table == 'Task_Sets'):
            # Correct method calls tests
            print_test('Running tests to delete from Task_Sets table:')

            # Delete a task set providing the primary key
            itemName = 'testSet' + str(self.testCount)
            self.direct.execStatement("insert into Task_Sets values ('{0}','')".format(itemName,))
            result   = self.run_test(self.api.deleteSet, task_set=itemName)
            in_db    = self.inDb('Task_Sets', task_set = itemName)
            success  = result == SUCCESS and not in_db
            Test('deleteSet', 'Delete a task set providing primary key').report(success)

            # Delete a task set and check if dependency in Task_Sets_to_Classes was deleted
            itemName = 'testSet' + str(self.testCount)
            self.direct.execStatement("insert into Task_Sets values ('{0}','')".format(itemName,))
            self.direct.execStatement("insert into Classes values ('{0}','')".format(itemName + 'Set',))
            self.direct.execStatement("insert into Task_Sets_to_Classes values ('{0}','{1}')".format(itemName, itemName + 'Set'))
            result  = self.run_test(self.api.deleteSet,task_set=itemName)
            in_db1  = self.inDb('Task_Sets', task_set=itemName)
            in_db2  = self.inDb('Task_Sets_to_Classes', task_set=itemName, node_class=(itemName+'Set'))
            success = result == SUCCESS and not in_db1 and not in_db2
            Test('deleteSet', 'Delete a task set and check if dependency in Task_Sets_to_Classes was deleted').report(success)

            # Delete a non-existing task set
            itemName = 'testSet' + str(self.testCount)
            result   = self.run_test(self.api.deleteSet,task_set=itemName)
            success  = result != SUCCESS and not self.inDb('Task_Sets', task_set=itemName)
            Test('deleteSet', 'Delete a non-existing task set').report(success)

    # =================================================================================================================
        # Node Classes
        if(table == 'all' or table == '*' or table == 'All' or table == 'Classes'):
            # Correct method calls tests
            print_test('Running tests to delete items from Classes table:')

            # Delete a node class providing the primary key
            itemName = 'testClass' + str(self.testCount)
            self.direct.execStatement("insert into Classes values ('{0}','')".format(itemName,))
            result   = self.run_test(self.api.deleteClass,node_class=itemName)
            success  = result == SUCCESS and not self.inDb('Classes', node_class = itemName)
            Test('deleteClass', 'Delete a node class providing primary key').report(success)

            # Delete a node class and check if dependency in Classes_to_Nodes was deleted
            itemName = 'testClass' + str(self.testCount)
            self.direct.execStatement("insert into Classes values ('{0}','')".format(itemName,))
            self.direct.execStatement("insert into Nodes values ('{0}','')".format(itemName + 'Set',))
            self.direct.execStatement("insert into Classes_to_Nodes values ('{0}','{1}')".format(itemName, itemName + 'Set'))
            result   = self.run_test(self.api.deleteClass,node_class=itemName)
            success  = result == SUCCESS and \
                       not self.inDb('Classes', node_class=itemName) and \
                       not self.inDb('Classes_to_Nodes', node_class=itemName, regex=(itemName+'Set'))
            Test('deleteClass', 'Delete a node class and check if dependency in Classes_to_Nodes was deleted').report(success)

            # Delete a non-existing node class
            self.testCount += 1
            itemName = 'testClass' + str(self.testCount)
            result   = self.run_test(self.api.deleteClass,node_class=itemName)
            success  = result != SUCCESS and not self.inDb('Classes', node_class=itemName)
            Test('deleteClass', 'Delete a non-existing node class').report(success)

    # =================================================================================================================

        # Nodes
        if(table == 'all' or table == '*' or table == 'All' or table == 'Nodes'):
            # Correct method calls tests
            print_test('Running tests to delete from Nodes table:')

            # Delete a node class providing the primary key
            itemName = 'testNode' + str(self.testCount)
            self.direct.execStatement("insert into Nodes values ('{0}','')".format(itemName,))
            result   = self.run_test(self.api.deleteNode,regex=itemName)
            success  = result == SUCCESS and not self.inDb('Nodes', regex = itemName)
            Test('deleteNode', 'Delete a node providing the primary key').report(success)

            # Delete a non-existing node(s) entry
            itemName = 'testNode' + str(self.testCount)
            result   = self.run_test(self.api.deleteNode,regex=itemName)
            success  = result != SUCCESS and not self.inDb('Nodes', regex=itemName)
            Test('deleteNode', 'Delete a non-existing node(s) entry').report(success)

    # =====================================================================================================================

    def testModify(self, table = 'all'):

        #       .88b  d88.  .d88b.  d8888b. d888888b d88888b db    db 
        #       88'YbdP`88 .8P  Y8. 88  `8D   `88'   88'     `8b  d8' 
        #       88  88  88 88    88 88   88    88    88ooo    `8bd8'  
        #       88  88  88 88    88 88   88    88    88~~~      88    
        #       88  88  88 `8b  d8' 88  .8D   .88.   88         88    
        #       YP  YP  YP  `Y88P'  Y8888D' Y888888P YP         YP    


        logging.info(1*os.linesep)
        logging.info('===================================================================')
        logging.info('                      MODIFY                                       ')
        logging.info('===================================================================')

        # Tasks
        if(table == 'all' or table == '*' or table == 'All' or table == 'Tasks'):
            # Correct method calls tests
            print_test('Running tests to modify rows from the Tasks table:')

            # Modify an existing task
            itemName    = 'testTask' + str(self.testCount)
            itemNameMod = itemName + 'Mod'
            self.direct.execStatement("insert into Tasks values ('{0}','','','','','')".format(itemName,))
            result = self.run_test(self.api.modifyTask,original_task=itemName, task=itemNameMod, description='Modified succesfully by unit test script')
            success = result == SUCCESS and \
                              not self.inDb('Tasks', task=itemName) and \
                              self.inDb('Tasks', task=itemNameMod, description='Modified succesfully by unit test script')
            Test('modifyTask', 'Modify an existing task').report(success)

            # Modify an existing task and check if the dependencies were updated (foreign key)
            itemName    = 'testTask' + str(self.testCount)
            itemNameMod = itemName + 'Mod'
            setName     = itemName + 'Set'
            self.direct.execStatement("insert into Tasks values ('{0}','','','','','')".format(itemName,))
            self.direct.execStatement("insert into Task_Sets values ('{0}','')".format(setName,))
            self.direct.execStatement("insert into Tasks_to_Task_Sets values ('{0}','{1}')".format(itemName, setName))
            result   = self.run_test(self.api.modifyTask,original_task=itemName, task=itemNameMod)

            success  = result == SUCCESS and \
                        not self.inDb('Tasks', task=itemName) and \
                        self.inDb('Tasks', task=itemNameMod) and \
                        self.inDb('Tasks_to_Task_Sets', task=itemNameMod, task_set=setName)
            Test('modifyTask', 'Modify an existing task and check if the dependencies were updated (foreign key)').report(success)

            # Modify a non-existing task
            itemName = 'testTask' + str(self.testCount)
            itemNameMod = itemName + 'Mod'
            result   = self.run_test(self.api.modifyTask,original_task=itemName, task=itemNameMod, description='Modified succesfully by unit test script')
            success  = result != SUCCESS and not self.inDb('Tasks', task=itemName)
            Test('modifyTask', 'Modify a non-existing task').report(success)

        # =================================================================================================================

        # Task Sets
        if(table == 'all' or table == '*' or table == 'All' or table == 'Task_Sets'):
            # Correct method calls tests
            print_test('Running teststo modify the Task Sets table:')

            # Modify an existing task set
            itemName    = 'testSet' + str(self.testCount)
            itemNameMod = itemName + 'Mod'
            self.direct.execStatement("insert into Task_Sets values ('{0}','')".format(itemName,))
            result   = self.run_test(self.api.modifySet,original_task_set=itemName, task_set=itemNameMod, description='Modified succesfully by unit test script')
            success  = result == SUCCESS and \
                       not self.inDb('Task_Sets', task_set=itemName) and \
                       self.inDb('Task_Sets', task_set=itemNameMod, description='Modified succesfully by unit test script')
            Test('modifySet', 'Modify an existing task set').report(success)

            # Modify an existing task set and check if the dependencies were updated (foreign key)
            itemName    = 'testSet' + str(self.testCount)
            itemNameMod = itemName + 'Mod'
            setName     = itemName + 'Set'
            self.direct.execStatement("insert into Task_Sets values ('{0}','')".format(itemName,))
            self.direct.execStatement("insert into Classes values ('{0}','')".format(setName,))
            self.direct.execStatement("insert into Task_Sets_to_Classes values ('{0}','{1}')".format(itemName, setName))
            self.direct.execStatement("insert into Tasks values ('{0}','','','','','')".format(setName,))
            self.direct.execStatement("insert into Tasks_to_Task_Sets values ('{0}','{1}')".format(setName, itemName))
            result  = self.run_test(self.api.modifySet,original_task_set=itemName, task_set=itemNameMod)
            success = str(result) == SUCCESS and \
                      not self.inDb('Task_Sets', task_set=itemName) and \
                      self.inDb('Task_Sets', task_set=itemNameMod) and \
                      self.inDb('Task_Sets_to_Classes', task_set=itemNameMod, node_class=setName) and \
                      self.inDb('Tasks_to_Task_Sets', task=setName, task_set=itemNameMod)
            Test('modifySet', 'Modify an existing task set and check if the dependencies were updated (foreign key)').report(success)

            # Modify a non-existing task set
            itemName    = 'testSet' + str(self.testCount)
            itemNameMod = itemName + 'Mod'
            result  = self.run_test(self.api.modifySet,original_task_set=itemName, task_set=itemNameMod, description='Modified succesfully by unit test script')
            success = result != SUCCESS and not self.inDb('Task_Sets', task_set=itemName)
            Test('modifySet', 'Modify a non-existing task set').report(success)

        # =================================================================================================================

        # Node Classes
        if(table == 'all' or table == '*' or table == 'All' or table == 'Classes'):
            # Correct method calls tests
            print_test('Running tests to modify Classes table:')

            # Modify an existing node class
            itemName    = 'testSet' + str(self.testCount)
            itemNameMod = itemName + 'Mod'
            self.direct.execStatement("insert into Classes values ('{0}','')".format(itemName,))
            result  = self.run_test(self.api.modifyClass, original_node_class=itemName, node_class=itemNameMod, description='Modified succesfully by unit test script')
            success = result == SUCCESS and \
                      not self.inDb('Classes', node_class=itemName) and \
                      self.inDb('Classes', node_class=itemNameMod, description='Modified succesfully by unit test script')
            Test('modifyClass', 'Modify an existing node class').report(success)

            # Modify an existing node class and check if the dependencies were updated (foreign key)
            itemName    = 'testClass' + str(self.testCount)
            itemNameMod = itemName + 'Mod'
            setName     = itemName + 'Set'
            self.direct.execStatement("insert into Classes values ('{0}','')".format(itemName,))
            self.direct.execStatement("insert into Nodes values ('{0}','')".format(setName,))
            self.direct.execStatement("insert into Classes_to_Nodes values ('{0}','{1}')".format(itemName, setName))
            self.direct.execStatement("insert into Task_Sets values ('{0}','')".format(setName,))
            self.direct.execStatement("insert into Task_Sets_to_Classes values ('{0}','{1}')".format(setName, itemName))
            result  = self.run_test(self.api.modifyClass,original_node_class=itemName, node_class=itemNameMod)
            success = result == SUCCESS and \
                      not self.inDb('Classes', node_class=itemName) and \
                      self.inDb('Classes', node_class=itemNameMod) and \
                      self.inDb('Classes_to_Nodes', node_class=itemNameMod, regex=setName) and \
                      self.inDb('Task_Sets_to_Classes', task_set=setName, node_class=itemNameMod)
            Test('modifyClass', 'Modify an existing node class and check if the dependencies were updated (foreign key)').report(success)

            # Modify a non-existing node class
            itemName = 'testClass' + str(self.testCount)
            itemNameMod = itemName + 'Mod'
            result  = self.run_test(self.api.modifyClass,original_node_class=itemName, node_class=itemNameMod, description='Modified succesfully by unit test script')
            success = result != SUCCESS and not self.inDb('Classes', node_class=itemName)
            Test('modifyClass', 'Modify a non-existing node class').report(success)

        # =================================================================================================================

        # Nodes
        if(table == 'all' or table == '*' or table == 'All' or table == 'Nodes'):
            # Correct method calls tests
            print_test('Running tests to modify Nodes table:')

            # Modify an existing node(s) entry
            itemName    = 'testNode' + str(self.testCount)
            itemNameMod = itemName + 'Mod'
            self.direct.execStatement("insert into Nodes values ('{0}','')".format(itemName,))
            result  = self.run_test(self.api.modifyNode,original_regex=itemName, regex=itemNameMod, description='Modified succesfully by unit test script')
            success = result == SUCCESS and \
                      not self.inDb('Nodes', regex=itemName) and \
                      self.inDb('Nodes', regex=itemNameMod, description='Modified succesfully by unit test script')
            Test('modifyNode', 'Modify an existing node(s) entry').report(success)

            # Modify a non-existing node(s) entry
            itemName    = 'testNode' + str(self.testCount)
            itemNameMod = itemName + 'Mod'
            result  = self.run_test(self.api.modifyNode,original_regex=itemName, regex=itemNameMod, description='Modified succesfully by unit test script')
            success = result != SUCCESS and not self.inDb('Nodes', regex=itemName)
            Test('modifyNode', 'Modify a non-existing node(s) entry').report(success)

    # =====================================================================================================================
    def testGet(self, table = 'all'):

        #                    d888b  d88888b d888888b 
        #                   88' Y8b 88'     `~~88~~' 
        #                   88      88ooooo    88    
        #                   88  ooo 88~~~~~    88    
        #                   88. ~8~ 88.        88    
        #                    Y888P  Y88888P    YP 

        logging.info(1*os.linesep)
        logging.info('===================================================================')
        logging.info('                       GET                                         ')
        logging.info('===================================================================')

        # Tasks
        if(table == 'all' or table == '*' or table == 'All' or table == 'Tasks'):
            # Correct method calls tests
            print_test('Running tests with get commands for Tasks table:')

            # Get an existing task
            itemName = 'testTask' + str(self.testCount)
            self.direct.execStatement("insert into Tasks values ('{0}','bla','bla','bla','bla','bla')".format(itemName,))
            result   = self.run_test(self.api.getTask,task=itemName)
            result   = str(result).replace("u'",'"').replace("'",'"')
            success  = result.find('"task": "' + itemName) >= 0 and self.inDb('Tasks', task=itemName)
            Test('getTask', 'Get an existing task').report(success)

            # Get all tasks
            itemName = 'testTask' + str(self.testCount)
            self.direct.execStatement("insert into Tasks values ('{0}','bla','bla','bla','bla','bla')".format(itemName,))
            result   = self.run_test(self.api.getTask,task='*')
            success  = len(result) > 0 and self.inDb('Tasks', task=itemName)
            Test('getTask', 'Get all tasks').report(success)

            # Get a non-existing task
            itemName = 'testTask' + str(self.testCount)
            result   = self.run_test(self.api.getTask,task=itemName)
            success  = result != SUCCESS and not self.inDb('Tasks', task=itemName)
            Test('getTask', 'Get a non-existing task').report(success)

        # =================================================================================================================
        # Task Sets
        if(table == 'all' or table == '*' or table == 'All' or table == 'Task_Sets'):
            # Correct method calls tests
            print_test('Running tests of get commands for Task_Sets table:')

            # Get an existing task set
            itemName = 'testSet' + str(self.testCount)
            self.direct.execStatement("insert into Task_Sets values ('{0}','bla')".format(itemName,))
            result   = self.run_test(self.api.getSet,task_set=itemName)
            result   = str(result).replace("u'",'"').replace("'",'"')
            success  = result.find('"'+itemName+'"') > 0 and result.find('"bla"') > 0 and \
                       self.inDb('Task_Sets', task_set=itemName)
            Test('getSet', 'Get an existing task set').report(success)

            # Get a non-existing task set
            itemName = 'testSet' + str(self.testCount)
            result   = self.run_test(self.api.getSet,task_set=itemName)
            success  = result.find('Failed to access task set "'+itemName+'" from database') >=0 and \
                       not self.inDb('Task_Sets', task_set=itemName)
            Test('getSet', 'Get a non-existing task set').report(success)

        # =================================================================================================================
        # Node Classes
        if(table == 'all' or table == '*' or table == 'All' or table == 'Classes'):
            # Correct method calls tests
            print_test('Running tests of get commands for Classes table:')

            # Get an existing node class
            itemName = 'testClass' + str(self.testCount)
            self.direct.execStatement("insert into Classes values ('{0}','bla')".format(itemName,))
            result   = self.run_test(self.api.getClass,node_class=itemName)
            result   = str(result).replace("u'",'"').replace("'",'"')
            success  = result.find('"'+itemName+'"') > 0 and result.find('"bla"') > 0 and \
                       self.inDb('Classes', node_class=itemName)
            Test('getClass', 'Get an existing node class').report(success)

            # Get a non-existing node class
            itemName = 'testClass' + str(self.testCount)
            result   = self.run_test(self.api.getClass,node_class=itemName)
            success  = result != 'Success' and \
                       not self.inDb('Classes', node_class=itemName)
            Test('getClass', 'Get a non-existing node class').report(success)

        # =================================================================================================================
        # Nodes
        if(table == 'all' or table == '*' or table == 'All' or table == 'Nodes'):
            # Correct method calls tests
            print_test('Running tests calling get commands on Nodes table:')

            # Get an existing node(s) entry
            itemName = 'testNode' + str(self.testCount)
            self.direct.execStatement("insert into Nodes values ('{0}','bla')".format(itemName,))
            result   = self.run_test(self.api.getNode,regex=itemName)
            result   = str(result).replace("u'",'"').replace("'",'"')
            success  = result.find('"'+itemName+'"') > 0 and result.find('"bla"') > 0 and \
                       self.inDb('Nodes', regex=itemName)
            Test('getNode', 'Get an existing node(s) entry').report(success)

            # Get a non-existing node(s)
            itemName = 'testNode' + str(self.testCount)
            result   = self.run_test(self.api.getNode, regex=itemName)
            success  = result != 'Success' and not self.inDb('Nodes', regex=itemName)
            Test('getNode', 'Get a non-existing node(s)').report(success)

    # =====================================================================================================================
    
    def testAssign(self, table = 'all'):

        #            .d8b.  .d8888. .d8888. d888888b  d888b  d8b   db 
        #           d8' `8b 88'  YP 88'  YP   `88'   88' Y8b 888o  88 
        #           88ooo88 `8bo.   `8bo.      88    88      88V8o 88 
        #           88~~~88   `Y8b.   `Y8b.    88    88  ooo 88 V8o88 
        #           88   88 db   8D db   8D   .88.   88. ~8~ 88  V888 
        #           YP   YP `8888Y' `8888Y' Y888888P  Y888P  VP   V8P 


        logging.info(1*os.linesep)
        logging.info('===================================================================')
        logging.info('                      ASSIGN                                       ')
        logging.info('===================================================================')

        # Tasks
        if(table == 'all' or table == '*' or table == 'All' or table == 'Tasks'):
            # Correct method calls tests
            print_test('Running tests of calling assign commands for Tasks table:')

            # Assign an existing task to an existing task set
            itemName = 'testTask' + str(self.testCount)
            setName  = itemName + 'Set'
            self.direct.execStatement("insert into Tasks values ('{0}','','','','','')".format(itemName,))
            self.direct.execStatement("insert into Task_Sets values ('{0}','')".format(setName,))
            result  = self.run_test(self.api.assignTask, task=itemName, task_set=setName)
            success = result == SUCCESS and self.inDb('Tasks_to_Task_Sets', task=itemName, task_set=setName)
            Test('assignTask', 'Assign an existing task to an existing task set').report(success)

            # Assign an non-existing task to an existing task set (or vice versa or both non-existing)
            itemName = 'testTask' + str(self.testCount)
            result   = self.run_test(self.api.assignTask, task=itemName, task_set=setName)
            success  =  result != SUCCESS
            Test('assignTask', 'Assign an non-existing task to an existing task set (or vice versa or both non-existing)').report(success)

            # Query for tasks of a given task set
            result  = self.run_test(self.api.tasksInSet, task_set=setName)
            success = len(result) == 1 and result[0].get('task',None) is not None
            Test('tasksInSet', 'Retrieve tasks assigned to a given set').report(success)

            # Query for task sets of a non existing node class
            result  = self.run_test(self.api.tasksInSet, task_set=setName+'_Not_Existing')
            success = "FAILED"
            if result.find('The task set "testTask44Set_Not_Existing" does not exist!') >= 0: success = SUCCESS
            Test('tasksetsInClass', 'Retrieve tasks from a non existing task set').report(success)

        # =================================================================================================================
        # Task Sets
        if(table == 'all' or table == '*' or table == 'All' or table == 'Task_Sets'):
            # Correct method calls tests
            print_test('Running tests calling assign commands for Task_Sets table:')

            # Assign an existing task set to an existing node class
            itemName = 'testSet' + str(self.testCount)
            className  = itemName + 'Class'
            self.direct.execStatement("insert into Task_Sets values ('{0}','')".format(itemName,))
            self.direct.execStatement("insert into Classes values ('{0}','')".format(className,))
            result   = self.run_test(self.api.assignSet,task_set=itemName, node_class=className)
            success  = result == SUCCESS and self.inDb('Task_Sets_to_Classes', task_set=itemName, node_class=className)
            Test('assignSet', 'Assign an existing task set to an existing node class').report(success)

            # Assign an non-existing task set to an existing node class
            itemName = 'testSet' + str(self.testCount)
            result   = self.run_test(self.api.assignSet,task_set=itemName, node_class=className)
            success  = result != SUCCESS
            Test('assignSet', 'Assign an non-existing task set to an existing node class').report(success)

            # Assign an non-existing task set to a non- existing node class
            itemName = 'testSet' + str(self.testCount)
            self.direct.execStatement("insert into Task_Sets values ('{0}','')".format(itemName,))
            result   = self.run_test(self.api.assignSet,task_set=itemName, node_class=className+'_Non_Existing')
            success  = result != SUCCESS
            Test('assignSet', 'Assign an existing task set to a non existing node class').report(success)

            # Query for task sets of a given node class
            result  = self.run_test(self.api.tasksetsInClass, node_class=className)
            success = len(result) == 1 and result[0].get('task_set',None) is not None
            Test('tasksetsInClass', 'Retrieve all task sets assigned to a given node class').report(success)
            
            # Query for task sets of a non existing node class
            result  = self.run_test(self.api.tasksetsInClass, node_class=className+'_Non_Existing')
            success = "FAILED"
            if result.find('The node class "testSet48Class_Non_Existing" does not exist!') >= 0: success = SUCCESS
            Test('tasksetsInClass', 'Retrieve task sets from a non existing node class').report(success)
            
        # =================================================================================================================

        # Node classes
        if(table == 'all' or table == '*' or table == 'All' or table == 'Classes'):
            # Correct method calls tests
            print_test('Running tests calling assign commands for Classes table:')

            # Assign an existing node class to an existing node(s) entry
            itemName = 'testClass' + str(self.testCount)
            regexVal = itemName + 'Set'
            self.direct.execStatement("insert into Classes values ('{0}','')".format(itemName,))
            self.direct.execStatement("insert into Nodes values ('{0}','')".format(regexVal,))
            result   = self.run_test(self.api.assignClass,node_class=itemName, regex=regexVal)
            success  = result == SUCCESS and self.inDb('Classes_to_Nodes', node_class=itemName, regex=regexVal)
            Test('assignClass', 'Assign an existing node class to an existing node(s) entry').report(success)

            # Assign an existing node class to a non existing node(s)
            itemName = 'testClass' + str(self.testCount)
            self.direct.execStatement("insert into Classes values ('{0}','')".format(itemName,))
            result   = self.run_test(self.api.assignClass,node_class=itemName, regex=regexVal+'_Non_Existing')
            success  = result != SUCCESS
            Test('assignClass', 'Assign an existing node class to a non-existing node(s)').report(success)

            # Assign an non-existing node class to an existing node(s)
            itemName = 'testClass' + str(self.testCount)
            result   = self.run_test(self.api.assignClass,node_class=itemName, regex=regexVal)
            success  = result != SUCCESS
            Test('assignClass', 'Assign an non-existing node class to an existing node(s)').report(success)

            # Query for task sets of a given node class
            result  = self.run_test(self.api.nodeclassInNode, regex=regexVal)
            success = len(result) == 1 and result[0].get('node_class',None) is not None
            Test('nodeclassInNode', 'Retrieve all node classes assigned to a given node type').report(success)
            
            # Query for task sets of a non existing node class
            result  = self.run_test(self.api.nodeclassInNode, regex=regexVal+'_Non_Existing')
            success  = result != SUCCESS
            Test('nodeclassInNode', 'Retrieve node classes of a non existing node type').report(success)
            
    # =====================================================================================================================
    
    def testUnassign(self, table = 'all'):

        #   db    db d8b   db  .d8b.  .d8888. .d8888. d888888b  d888b  d8b   db 
        #   88    88 888o  88 d8' `8b 88'  YP 88'  YP   `88'   88' Y8b 888o  88 
        #   88    88 88V8o 88 88ooo88 `8bo.   `8bo.      88    88      88V8o 88 
        #   88    88 88 V8o88 88~~~88   `Y8b.   `Y8b.    88    88  ooo 88 V8o88 
        #   88b  d88 88  V888 88   88 db   8D db   8D   .88.   88. ~8~ 88  V888 
        #   ~Y8888P' VP   V8P YP   YP `8888Y' `8888Y' Y888888P  Y888P  VP   V8P 


        logging.info(1*os.linesep)
        logging.info('===================================================================')
        logging.info('                     UNASSIGN                                      ')
        logging.info('===================================================================')

        # Tasks
        if(table == 'all' or table == '*' or table == 'All' or table == 'Tasks'):
            # Correct method calls tests
            print_test('Running tests with unassign commands for Tasks table:')

            # Unassign an existing task from an existing task set
            itemName = 'testTask' + str(self.testCount)
            setName  = itemName + 'Set'
            self.direct.execStatement("insert into Tasks values ('{0}','','','','','')".format(itemName,))
            self.direct.execStatement("insert into Task_Sets values ('{0}','')".format(setName,))
            self.direct.execStatement("insert into Tasks_to_Task_Sets values ('{0}','{1}')".format(itemName, setName))
            result   = self.run_test(self.api.unassignTask,task=itemName, task_set=setName)
            success  = result == SUCCESS and not self.inDb('Tasks_to_Task_Sets', task=itemName, task_set=setName)
            Test('unassignTask', 'Unassign an existing task from an existing task set').report(success)

            # Unassign an non-existing task from an existing task set
            itemName = 'testTask' + str(self.testCount)
            setName  = itemName + 'Set'
            result   = self.run_test(self.api.unassignTask,task=itemName, task_set=setName)
            success  = result != SUCCESS
            Test('unassignTask', 'Unassign an non-existing task from an existing task set').report(success)

            # Unassign an existing task from a non existing task set
            itemName = 'testTask' + str(self.testCount)
            self.direct.execStatement("insert into Tasks values ('{0}','','','','','')".format(itemName,))
            setName  = itemName + 'Set'
            result   = self.run_test(self.api.unassignTask,task=itemName, task_set=setName+'_Non_Existing')
            success  = result != SUCCESS
            Test('unassignTask', 'Unassign an existing task from a non existing task set').report(success)

        # =================================================================================================================

        # Task Sets
        if(table == 'all' or table == '*' or table == 'All' or table == 'Task_Sets'):
            # Correct method calls tests
            print_test('Running tests of calling unassign commands for Task_Sets table:')

            # Unassign an existing task set from an existing node class
            itemName = 'testSet' + str(self.testCount)
            setName  = itemName + 'Set'
            self.direct.execStatement("insert into Task_Sets values ('{0}','')".format(itemName,))
            self.direct.execStatement("insert into Classes values ('{0}','')".format(setName,))
            self.direct.execStatement("insert into Task_Sets_to_Classes values ('{0}','{1}')".format(itemName, setName))
            result   = self.run_test(self.api.unassignSet, task_set=itemName, node_class=setName)
            success  = result == SUCCESS and \
                       not self.inDb('Task_Sets_to_Classes', task_set=itemName, node_class=setName)
            Test('unassignSet', 'Unassign an existing task set from an existing node class').report(success)

            # Unassign an non-existing task set from an existing node class (or vice versa or both non-existing)
            itemName = 'testSet' + str(self.testCount)
            setName  = itemName + 'Set'
            result   = self.run_test(self.api.unassignSet, task_set=itemName, node_class=setName)
            success  = result != SUCCESS
            Test('unassignSet', 'Unassign an non-existing task set from an existing node class').report(success)

        # =================================================================================================================
        # Node Classes
        if(table == 'all' or table == '*' or table == 'All' or table == 'Classes'):
            # Correct method calls tests
            print_test('Running tests of calling unassign commands for Classes table:')

            # Unassign an existing node class from an existing node(s) entry
            itemName = 'testClass' + str(self.testCount)
            setName  = itemName + 'Set'
            self.direct.execStatement("insert into Classes values ('{0}','')".format(itemName,))
            self.direct.execStatement("insert into Nodes values ('{0}','')".format(setName,))
            self.direct.execStatement("insert into Classes_to_Nodes values ('{0}','{1}')".format(itemName, setName))
            result   = self.run_test(self.api.unassignClass, node_class=itemName, regex=setName)
            success  = result == SUCCESS and not self.inDb('Classes_to_Nodes', node_class=itemName, regex=setName)
            Test('unassignClass', 'Unassign an existing node class from an existing node(s) entry').report(success)

            # Unassign an non-existing node class from an existing node(s) (or vice versa or both non-existing)
            itemName = 'testClass' + str(self.testCount)
            setName  = itemName + 'Set'
            result   = self.run_test(self.api.unassignClass, node_class=itemName, regex=setName)
            success  = result != SUCCESS
            Test('unassignClass', 'Unassign an non-existing node class from an existing node(s) (or vice versa or both non-existing)').report(success)

    # =====================================================================================================================
    #                         Helper Methods
    # =====================================================================================================================
    def inDb(self, table, **args):
        return self.direct.inDb(table, **args)

    # =====================================================================================================================
    def yes_or_no(self, question, default="yes"):
        #Ask a question specified, default answer is executed if the user hits enter, returns true or false
        valid = {"yes": True, "y": True, "ye": True,
                "no": False, "n": False}
        if default is None:
            prompt = " [y/n] "
        elif default == "yes":
            prompt = " [Y/n] "
        elif default == "no":
            prompt = " [y/N] "
        else:
            raise ValueError("invalid default answer: '%s'" % default)
        while True:
            sys.stdout.write(question + prompt)
            choice = raw_input().lower()
            if default is not None and choice == '':
                return valid[default]
            elif choice in valid:
                return valid[choice]
            else:
                sys.stdout.write("Please respond with 'yes' or 'no' "
                                "(or 'y' or 'n').\n")            

    # =========================================================================================================================
    def _log(self, function_name, function_description, result):
        self.testCount += 1
        ##tests_total_number = 59
        description_field_width = 100
        FORMAT = '%03d%-s%03d  %-18s  %-' + str(description_field_width) + 's  %-6s' # %-10s' # Possibly add timer
        logging.info(FORMAT % (self.testCount-self.errCount, '/', self.testCount, function_name + (18 - len(function_name))*'.', 
                               function_description + (description_field_width - len(function_description))*'.', result) + bcolors.ENDC)

    def log(self, function_name, function_description):
        self._log(function_name, function_description, 'passed')

    def error(self, function_name, function_description):
        self.errCount += 1
        print (bcolors.FAIL)
        self._log(function_name, function_description, 'FAILED')

# =========================================================================================================================
def testColors():
    # Run colouring demo
    logging.info('++ Demonstration of font colouring:')
    logging.info(bcolors.HEADER + 'Header' + bcolors.ENDC)
    logging.info(bcolors.OKBLUE + 'Okblue' + bcolors.ENDC)
    logging.info(bcolors.OKGREEN + 'Okgreen' + bcolors.ENDC)
    logging.info(bcolors.WARNING + 'Warning' + bcolors.ENDC)
    logging.info(bcolors.FAIL + 'Fail' + bcolors.ENDC)
    logging.info(bcolors.ENDC + 'Endc' + bcolors.ENDC)
    logging.info(bcolors.BOLD + 'Bold' + bcolors.ENDC)
    logging.info(bcolors.UNDERLINE + 'Underline' + bcolors.ENDC)

# =========================================================================================================================
# Run as executable cli script
if __name__ == "__main__":
    import optparse
    parser = optparse.OptionParser()
    parser.description = "TaskDB API checker. Module to test all TaskDB api calls."
    parser.formatter.width = 132
    parser.add_option("-d", "--debug", 
                      action='store_true',
                      dest="debug", default=False,
                      help="Enable output debug statements",
                      metavar="<boolean>")
    parser.add_option("-n", "--no-init", 
                      action='store_false',
                      dest="init", default=True,
                      help="Initialize/create new database",
                      metavar="<boolean>")
    parser.add_option("-f", "--file",
                      dest="database",
                      default='./taskdb_test.db',
                      help="DB file name",
                      metavar="<string>")
    parser.add_option("-c", "--connection",
                      dest="connection",
                      default='http://localhost:8081/XMLRPC',
                      help="RPC server connection  (RPC calls only)",
                      metavar="<string>")
    parser.add_option("-p", "--pdb",
                      action='store_true',
                      dest="run_pdb",
                      default=False,
                      help="Invoke python debugger",
                      metavar="<boolean>")
    parser.add_option("-C", "--certfile",
                      dest="certfile",
                      default='./ssl/server.crt',
                      help="Server SSL certificate",
                      metavar="<string>")
    parser.add_option("-t", "--type",
                      dest="type",
                      default='direct',
                      help="API type (xmlrpc,jsonrpc,direct)",
                      metavar="<string>")
    (opts, args) = parser.parse_args()
    if opts.run_pdb:
        import pdb
        pdb.set_trace()
    tester = ApiTest(opts)    # Run the main api tester with all tests
    tester.testAll()
    testColors()
    tester = None

