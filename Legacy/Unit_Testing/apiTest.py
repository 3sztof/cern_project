#!/usr/bin/env python




# =========================================================================================================================
#                    LHCb Online Farm Process Explorer
#      Main API tester - validates api against desired DB changes and errors
#                            
#                           K.Wilczynski 08.2018
# =========================================================================================================================





class apiTest():

    def __init__(self):

        # Imports
        from sqlalchemy import create_engine
        import json
        import os
        import mainAPI
        import sys

        # Configs
        self.os = os
        self.sys = sys
        self.create_engine = create_engine
        self.api = None
        self.my_path = self.os.path.dirname(self.os.path.realpath(__file__))
        self._database = self.my_path + self.os.sep + '..' + self.os.sep + '0_main_api' + self.os.sep + 'LHCb.db'
        self._database_root_path = self.my_path + self.os.sep + '..' + self.os.sep + '0_main_api'
        self._initDB_path = self.my_path + self.os.sep + '..' + self.os.sep + '0_main_api' + self.os.sep + 'initDB.py'
        self.testCount = 0
        self.errCount = 0
        
        # Reinitialize the database using initDB.py script (initDB.pt must be executable: chmod +x)
        if(self.yes_or_no('Do you want to rebuild the main database using settings and data from initDB script? (reccomended)')):      
            try:
                self.os.system(self._initDB_path)
                print self.os.linesep + 'The database has been rebuilt'
            except: 
                print self.os.linesep + 'WARNING: Database rebuilding failed. Are you running Linux? Please make sure that 664 permissions\
                    are set on database root folder and initDB script.'

        # Initiate a connection (and a separate test query connection in api object, cursor object and turn on the constraints to protect the schema (created in initDB.py)
        try:
            self.api = mainAPI.mainAPI()
            self.db_connect = self.create_engine('sqlite:///' + self._database)
            self.conn = self.db_connect.connect()
            self.conn.execute("PRAGMA foreign_keys = ON")
            print 'Connected to the database at: ' + self._database
        except:
            print 'Couldn\'t connect to the database at ' + self._database
            return

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
        if(self.errCount == 0):
            print 4*self.os.linesep + bcolors.OKBLUE + 'Finished API testing without any error' + bcolors.ENDC + 4*self.os.linesep
        else:
            print 4*self.os.linesep + bcolors.FAIL + 'Finished API testing with ' + str(self.errCount) + ' errors' + bcolors.ENDC + 4*self.os.linesep

        if(self.yes_or_no('The main database has been modified during the tests, do you want to rebuild it using settings and data from initDB script? (reccomended)')):      
            try:
                self.os.system(self._initDB_path)
                print self.os.linesep + 'The database has been rebuilt, returning test result - error count' + self.os.linesep
            except: 
                print self.os.linesep + 'WARNING: Database rebuilding failed. Are you running Linux? Please make sure that 664 permissions\
                    are set on database root folder and initDB script.' + self.os.linesep

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

        print 4*self.os.linesep
        print '==================================================================='
        print '                       ADD                                         '
        print '==================================================================='

        # Tasks
        if(table == 'all' or table == '*' or table == 'All' or table == 'Tasks'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called add commands for Tasks table:' + bcolors.ENDC + self.os.linesep

            # Add task providing all parameters
            self.testCount += 1
            itemName = 'testTask' + str(self.testCount)
            result = self.api.addTask(itemName, utgid='testUtgid', command='testCommand', command_parameters='testScriptParams', 
                                    task_parameters='testPcaddParams', description='testDescription')
            if(result == 'Success' and self.inDb('Tasks', task=itemName, utgid='testUtgid', command='testCommand', command_parameters='testScriptParams', 
                                    task_parameters='testPcaddParams', description='testDescription')):
                self.log(self.testCount, 'addTask', 'Add a task providing all parameters', True)                    
            else:
                self.log(self.testCount, 'addTask', 'Add a task providing all parameters', False)                    
                self.errCount += 1

            # -----------------------------------------

            # Add task providing only required parameter (rest should be empty strings: '')
            self.testCount += 1
            itemName = 'testTask' + str(self.testCount)
            result = self.api.addTask(itemName)
            if(result == 'Success' and self.inDb('Tasks', task=itemName, utgid='', command='', command_parameters='', 
                                    task_parameters='', description='')):
                self.log(self.testCount, 'addTask', 'Add a task providing only the primary key', True)                    
            else:
                self.log(self.testCount, 'addTask', 'Add a task providing only the primary key', False)                    
                self.errCount += 1
            
            # -------------------------------------------------------------------------------------------------------------

            # Incorret method calls tests (error handling)
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of error handling for Tasks table' + bcolors.ENDC + self.os.linesep

            # Unique constraint violation test
            self.testCount += 1
            itemName = 'testTask' + str(self.testCount)
            self.conn.execute("insert into Tasks values ('{0}','','','','','')".format(itemName,))
            result = self.api.addTask(itemName, utgid='', command='', command_parameters='', 
                                    task_parameters='', description='')
            expected_error = "(sqlite3.IntegrityError) UNIQUE constraint failed: Tasks.task [SQL: \"insert into Tasks values ('" + itemName + "','','','','','')\"] (Background on this error at: http://sqlalche.me/e/gkpj)"
            if(str(result) == expected_error and self.inDb('Tasks', task=itemName, utgid='', command='', command_parameters='', 
                                    task_parameters='', description='')):
                self.log(self.testCount, 'addTask', 'Add a task violating unique constraint', True)                    
            else:
                self.log(self.testCount, 'addTask', 'Add a task violating unique constraint', False)                    
                self.errCount += 1
        
        # =================================================================================================================

        # Task sets
        if(table == 'all' or table == '*' or table == 'All' or table == 'Task_Sets'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called add commands for Task_Sets table:' + bcolors.ENDC + self.os.linesep

            # Add task set providing all parameters
            self.testCount += 1
            itemName = 'testSet' + str(self.testCount)
            result = self.api.addSet(itemName, description='testDescription')
            if(result == 'Success' and self.inDb('Task_Sets', task_set=itemName, description='testDescription')):
                self.log(self.testCount, 'addSet', 'Add a task set providing all parameters', True)                    
            else:
                self.log(self.testCount, 'addSet', 'Add a task set providing all parameters', False)                    
                self.errCount += 1

            # -----------------------------------------

            # Add task set providing only required parameter (rest should be empty strings: '')
            self.testCount += 1
            itemName = 'testSet' + str(self.testCount)
            result = self.api.addSet(itemName)
            if(result == 'Success' and self.inDb('Task_Sets', task_set=itemName, description='')):
                self.log(self.testCount, 'addSet', 'Add a task set providing only the primary key', True)                    
            else:
                self.log(self.testCount, 'addSet', 'Add a task set providing only the primary key', False)                    
                self.errCount += 1

            # -------------------------------------------------------------------------------------------------------------

            # Incorret method calls tests (error handling)
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of error handling for Task_Sets table' + bcolors.ENDC + self.os.linesep

            # Unique constraint violation test
            self.testCount += 1
            itemName = 'testSet' + str(self.testCount)
            self.conn.execute("insert into Task_Sets values ('{0}','')".format(itemName,))
            result = self.api.addSet(itemName, description='')
            expected_error = "(sqlite3.IntegrityError) UNIQUE constraint failed: Task_Sets.task_set [SQL: \"insert into Task_Sets values ('" + itemName + "','')\"] (Background on this error at: http://sqlalche.me/e/gkpj)"
            if(str(result) == expected_error and self.inDb('Task_Sets', task_set=itemName, description='')):
                self.log(self.testCount, 'addSet', 'Add a task set violating unique constraint', True)                    
            else:
                self.log(self.testCount, 'addSet', 'Add a task set violating unique constraint', False)                    
                self.errCount += 1

        # =================================================================================================================

        # Node Classes
        if(table == 'all' or table == '*' or table == 'All' or table == 'Classes'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called add commands for Classes table:' + bcolors.ENDC + self.os.linesep

            # Add node class providing all parameters
            self.testCount += 1
            itemName = 'testClass' + str(self.testCount)
            result = self.api.addClass(itemName, description='testDescription')
            if(result == 'Success' and self.inDb('Classes', node_class=itemName, description='testDescription')):
                self.log(self.testCount, 'addClass', 'Add a node class providing all the parameters', True)                    
            else:
                self.log(self.testCount, 'addClass', 'Add a node class providing all the parameters', False)                    
                self.errCount += 1

            # -----------------------------------------

            # Add node class providing only required parameter (rest should be empty strings: '')
            self.testCount += 1
            itemName = 'testClass' + str(self.testCount)
            result = self.api.addClass(itemName)
            if(result == 'Success' and self.inDb('Classes', node_class=itemName, description='')):
                self.log(self.testCount, 'addClass', 'Add a node class providing only the primary key', True)                    
            else:
                self.log(self.testCount, 'addClass', 'Add a node class providing only the primary key', False)                    
                self.errCount += 1

            # -------------------------------------------------------------------------------------------------------------

            # Incorret method calls tests (error handling)
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of error handling for Classes table' + bcolors.ENDC + self.os.linesep

            # Unique constraint violation test
            self.testCount += 1
            itemName = 'testClass' + str(self.testCount)
            self.conn.execute("insert into Classes values ('{0}','')".format(itemName,))
            result = self.api.addClass(itemName, description='')
            expected_error = "(sqlite3.IntegrityError) UNIQUE constraint failed: Classes.node_class [SQL: \"insert into Classes values ('" + itemName + "','')\"] (Background on this error at: http://sqlalche.me/e/gkpj)"
            if(str(result) == expected_error and self.inDb('Classes', node_class=itemName, description='')):
                self.log(self.testCount, 'addClass', 'Add a node class violating unique constraint', True)                    
            else:
                self.log(self.testCount, 'addClass', 'Add a node class violating unique constraint', False)                    
                self.errCount += 1

        # =================================================================================================================

        # Nodes
        if(table == 'all' or table == '*' or table == 'All' or table == 'Nodes'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called add commands for Nodes table:' + bcolors.ENDC + self.os.linesep

            # Add node providing all parameters
            self.testCount += 1
            itemName = 'testNode' + str(self.testCount)
            result = self.api.addNode(itemName, description='testDescription')
            if(result == 'Success' and self.inDb('Nodes', regex=itemName, description='testDescription')):
                self.log(self.testCount, 'addNode', 'Add a node providing all the parameters', True)                    
            else:
                self.log(self.testCount, 'addNode', 'Add a node providing all the parameters', False)                    
                self.errCount += 1

            # -----------------------------------------

            # Add task set providing only required parameter (rest should be empty strings: '')
            self.testCount += 1
            itemName = 'testNode' + str(self.testCount)
            result = self.api.addNode(itemName)
            if(result == 'Success' and self.inDb('Nodes', regex=itemName, description='')):
                self.log(self.testCount, 'addNode', 'Add a node providing only the primary key', True)                    
            else:
                self.log(self.testCount, 'addNode', 'Add a node providing only the primary key', False)                    
                self.errCount += 1

            # -------------------------------------------------------------------------------------------------------------

            # Incorret method calls tests (error handling)
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of error handling for Nodes table' + bcolors.ENDC + self.os.linesep
                     
            # Unique constraint violation test
            self.testCount += 1
            itemName = 'testNode' + str(self.testCount)
            self.conn.execute("insert into Nodes values ('{0}','')".format(itemName,))
            result = self.api.addNode(itemName, description='')
            expected_error = "(sqlite3.IntegrityError) UNIQUE constraint failed: Nodes.regex [SQL: \"insert into Nodes values ('" + itemName + "','')\"] (Background on this error at: http://sqlalche.me/e/gkpj)"
            if(str(result) == expected_error and self.inDb('Nodes', regex=itemName, description='')):
                self.log(self.testCount, 'addNode', 'Add a node violating the unique constraint', True)                    
            else:
                self.log(self.testCount, 'addNode', 'Add a node violating the unique constraint', False)                    
                self.errCount += 1
        
    # =====================================================================================================================

    def testDelete(self, table = 'all'):


        #           d8888b. d88888b db      d88888b d888888b d88888b 
        #           88  `8D 88'     88      88'     `~~88~~' 88'     
        #           88   88 88ooooo 88      88ooooo    88    88ooooo 
        #           88   88 88~~~~~ 88      88~~~~~    88    88~~~~~ 
        #           88  .8D 88.     88booo. 88.        88    88.     
        #           Y8888D' Y88888P Y88888P Y88888P    YP    Y88888P 


        print 4*self.os.linesep
        print '==================================================================='
        print '                      DELETE                                       '
        print '==================================================================='

        # Tasks
        if(table == 'all' or table == '*' or table == 'All' or table == 'Tasks'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called delete commands for Tasks table:' + bcolors.ENDC + self.os.linesep

            # Delete a task providing the primary key
            self.testCount += 1
            itemName = 'testTask' + str(self.testCount)
            self.conn.execute("insert into Tasks values ('{0}','','','','','')".format(itemName,))
            result = self.api.deleteTask(itemName)
            if(result == 'Success' and not self.inDb('Tasks', task=itemName)):
                self.log(self.testCount, 'deleteTask', 'Delete a task providing primary key', True)                    
            else:
                self.log(self.testCount, 'deleteTask', 'Delete a task providing primary key', False)                    
                self.errCount += 1

            # -----------------------------------------

            # Delete a task and check if dependency in Tasks_to_Task_Sets was deleted
            self.testCount += 1
            itemName = 'testTask' + str(self.testCount)
            self.conn.execute("insert into Tasks values ('{0}','','','','','')".format(itemName,))
            self.conn.execute("insert into Task_Sets values ('{0}','')".format(itemName + 'Set',))
            self.conn.execute("insert into Tasks_to_Task_Sets values ('{0}','{1}')".format(itemName, itemName + 'Set'))
            result = self.api.deleteTask(itemName)
            if(result == 'Success' and not self.inDb('Tasks', task=itemName) and not self.inDb('Tasks_To_Task_Sets', task=itemName, task_set=(itemName+'Set'))):
                self.log(self.testCount, 'deleteTask', 'Delete a task and check if dependency in Tasks_to_Task_Sets was deleted', True)                    
            else:
                self.log(self.testCount, 'deleteTask', 'Delete a task and check if dependency in Tasks_to_Task_Sets was deleted', False)                    
                self.errCount += 1

            # ---------------------------------------------------------------------------------------------------------

            # Incorrect method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of incorrectly called delete commands for Tasks table:' + bcolors.ENDC + self.os.linesep

            # Delete a non-existing task
            self.testCount += 1
            itemName = 'testTask' + str(self.testCount)
            result = self.api.deleteTask(itemName)
            if(result == ('deleteTask: Unknown task with name: ' + itemName) and not self.inDb('Tasks', task=itemName)):
                self.log(self.testCount, 'deleteTask', 'Delete a task providing primary key', True)                    
            else:
                self.log(self.testCount, 'deleteTask', 'Delete a task providing primary key', False)                    
                self.errCount += 1

    # =================================================================================================================

        # Task Sets
        if(table == 'all' or table == '*' or table == 'All' or table == 'Task_Sets'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called delete commands for Task_Sets table:' + bcolors.ENDC + self.os.linesep

            # Delete a task set providing the primary key
            self.testCount += 1
            itemName = 'testSet' + str(self.testCount)
            self.conn.execute("insert into Task_Sets values ('{0}','')".format(itemName,))
            result = self.api.deleteSet(itemName)
            if(result == 'Success' and not self.inDb('Task_Sets', task_set = itemName)):
                self.log(self.testCount, 'deleteSet', 'Delete a task set providing primary key', True)                    
            else:
                self.log(self.testCount, 'deleteTask', 'Delete a task set providing primary key', False)                    
                self.errCount += 1

            # -----------------------------------------

            # Delete a task set and check if dependency in Task_Sets_to_Classes was deleted
            self.testCount += 1
            itemName = 'testSet' + str(self.testCount)
            self.conn.execute("insert into Task_Sets values ('{0}','')".format(itemName,))
            self.conn.execute("insert into Classes values ('{0}','')".format(itemName + 'Set',))
            self.conn.execute("insert into Task_Sets_to_Classes values ('{0}','{1}')".format(itemName, itemName + 'Set'))
            result = self.api.deleteSet(itemName)
            if(result == 'Success' and not self.inDb('Task_Sets', task_set=itemName) and not self.inDb('Task_Sets_to_Classes', task_set=itemName, node_class=(itemName+'Set'))):
                self.log(self.testCount, 'deleteSet', 'Delete a task set and check if dependency in Task_Sets_to_Classes was deleted', True)                    
            else:
                self.log(self.testCount, 'deleteSet', 'Delete a task set and check if dependency in Task_Sets_to_Classes was deleted', False)                    
                self.errCount += 1

            # ---------------------------------------------------------------------------------------------------------

            # Incorrect method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of incorrectly called delete commands for Task_Sets table:' + bcolors.ENDC + self.os.linesep

            # Delete a non-existing task set
            self.testCount += 1
            itemName = 'testSet' + str(self.testCount)
            result = self.api.deleteSet(itemName)
            if(result == ('deleteSet: Unknown task set with name: ' + itemName) and not self.inDb('Task_Sets', task_set=itemName)):
                self.log(self.testCount, 'deleteSet', 'Delete a non-existing task set', True)                    
            else:
                self.log(self.testCount, 'deleteSet', 'Delete a non-existing task set', False)                    
                self.errCount += 1

    # =================================================================================================================

        # Node Classes
        if(table == 'all' or table == '*' or table == 'All' or table == 'Classes'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called delete commands for Classes table:' + bcolors.ENDC + self.os.linesep

            # Delete a node class providing the primary key
            self.testCount += 1
            itemName = 'testClass' + str(self.testCount)
            self.conn.execute("insert into Classes values ('{0}','')".format(itemName,))
            result = self.api.deleteClass(itemName)
            if(result == 'Success' and not self.inDb('Classes', node_class = itemName)):
                self.log(self.testCount, 'deleteClass', 'Delete a node class providing primary key', True)                    
            else:
                self.log(self.testCount, 'deleteClass', 'Delete a node class providing primary key', False)                    
                self.errCount += 1

            # -----------------------------------------

            # Delete a node class and check if dependency in Classes_to_Nodes was deleted
            self.testCount += 1
            itemName = 'testClass' + str(self.testCount)
            self.conn.execute("insert into Classes values ('{0}','')".format(itemName,))
            self.conn.execute("insert into Nodes values ('{0}','')".format(itemName + 'Set',))
            self.conn.execute("insert into Classes_to_Nodes values ('{0}','{1}')".format(itemName, itemName + 'Set'))
            result = self.api.deleteClass(itemName)
            if(result == 'Success' and not self.inDb('Classes', node_class=itemName) and not self.inDb('Classes_to_Nodes', node_class=itemName, regex=(itemName+'Set'))):
                self.log(self.testCount, 'deleteClass', 'Delete a node class and check if dependency in Classes_to_Nodes was deleted', True)                    
            else:
                self.log(self.testCount, 'deleteClass', 'Delete a node class and check if dependency in Classes_to_Nodes was deleted', False)                    
                self.errCount += 1

            # ---------------------------------------------------------------------------------------------------------

            # Incorrect method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of incorrectly called delete commands for Classes table:' + bcolors.ENDC + self.os.linesep

            # Delete a non-existing node class
            self.testCount += 1
            itemName = 'testClass' + str(self.testCount)
            result = self.api.deleteClass(itemName)
            if(result == ('deleteClass: Unknown node class with name: ' + itemName) and not self.inDb('Classes', node_class=itemName)):
                self.log(self.testCount, 'deleteClass', 'Delete a non-existing node class', True)                    
            else:
                self.log(self.testCount, 'deleteClass', 'Delete a non-existing node class', False)                    
                self.errCount += 1

    # =================================================================================================================

        # Nodes
        if(table == 'all' or table == '*' or table == 'All' or table == 'Nodes'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called delete commands for Classes table:' + bcolors.ENDC + self.os.linesep

            # Delete a node class providing the primary key
            self.testCount += 1
            itemName = 'testNode' + str(self.testCount)
            self.conn.execute("insert into Nodes values ('{0}','')".format(itemName,))
            result = self.api.deleteNode(itemName)
            if(result == 'Success' and not self.inDb('Nodes', regex = itemName)):
                self.log(self.testCount, 'deleteNode', 'Delete a node providing the primary key', True)                    
            else:
                self.log(self.testCount, 'deleteNode', 'Delete a node providing the primary key', False)                    
                self.errCount += 1

            # ---------------------------------------------------------------------------------------------------------

            # Incorrect method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of incorrectly called delete commands for Nodes table:' + bcolors.ENDC + self.os.linesep

            # Delete a non-existing node(s) entry
            self.testCount += 1
            itemName = 'testNode' + str(self.testCount)
            result = self.api.deleteNode(itemName)
            if(result == ('deleteNode: Unknown nodes with regex: ' + itemName) and not self.inDb('Nodes', regex=itemName)):
                self.log(self.testCount, 'deleteNode', 'Delete a non-existing node(s) entry', True)                    
            else:
                self.log(self.testCount, 'deleteNode', 'Delete a non-existing node(s) entry', False)                    
                self.errCount += 1

    # =====================================================================================================================

    def testModify(self, table = 'all'):

        #       .88b  d88.  .d88b.  d8888b. d888888b d88888b db    db 
        #       88'YbdP`88 .8P  Y8. 88  `8D   `88'   88'     `8b  d8' 
        #       88  88  88 88    88 88   88    88    88ooo    `8bd8'  
        #       88  88  88 88    88 88   88    88    88~~~      88    
        #       88  88  88 `8b  d8' 88  .8D   .88.   88         88    
        #       YP  YP  YP  `Y88P'  Y8888D' Y888888P YP         YP    


        print 4*self.os.linesep
        print '==================================================================='
        print '                      MODIFY                                       '
        print '==================================================================='

        # Tasks
        if(table == 'all' or table == '*' or table == 'All' or table == 'Tasks'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called modify commands for Tasks table:' + bcolors.ENDC + self.os.linesep

            # Modify an existing task
            self.testCount += 1
            itemName = 'testTask' + str(self.testCount)
            itemNameMod = itemName + 'Mod'
            self.conn.execute("insert into Tasks values ('{0}','','','','','')".format(itemName,))
            result = self.api.modifyTask(itemName, task=itemNameMod, description='Modified succesfully by unit test script')
            if(result == 'Success' and not self.inDb('Tasks', task=itemName) and self.inDb('Tasks', task=itemNameMod, description='Modified succesfully by unit test script')):
                self.log(self.testCount, 'modifyTask', 'Modify an existing task', True)                    
            else:
                self.log(self.testCount, 'modifyTask', 'Modify an existing task', False)                    
                self.errCount += 1

            # -----------------------------------------

            # Modify an existing task and check if the dependencies were updated (foreign key)
            self.testCount += 1
            itemName = 'testTask' + str(self.testCount)
            itemNameMod = itemName + 'Mod'
            setName = itemName + 'Set'
            self.conn.execute("insert into Tasks values ('{0}','','','','','')".format(itemName,))
            self.conn.execute("insert into Task_Sets values ('{0}','')".format(setName,))
            self.conn.execute("insert into Tasks_to_Task_Sets values ('{0}','{1}')".format(itemName, setName))
            result = self.api.modifyTask(itemName, task=itemNameMod)
            if(result == 'Success' and not self.inDb('Tasks', task=itemName) and self.inDb('Tasks', task=itemNameMod) and self.inDb('Tasks_to_Task_Sets', task=itemNameMod, task_set=setName)):
                self.log(self.testCount, 'modifyTask', 'Modify an existing task and check if the dependencies were updated (foreign key)', True)                    
            else:
                self.log(self.testCount, 'modifyTask', 'Modify an existing task and check if the dependencies were updated (foreign key)', False)                    
                self.errCount += 1

            # -------------------------------------------------------------------------------------------------------------

            # Incorrect method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of incorrectly called modify commands for Tasks table:' + bcolors.ENDC + self.os.linesep

            # Modify a non-existing task
            self.testCount += 1
            itemName = 'testTask' + str(self.testCount)
            itemNameMod = itemName + 'Mod'
            result = self.api.modifyTask(itemName, task=itemNameMod, description='Modified succesfully by unit test script')
            if(result == ('modifyTask: Unknown task with name: ' + itemName) and not self.inDb('Tasks', task=itemName)):
                self.log(self.testCount, 'modifyTask', 'Modify a non-existing task', True)                    
            else:
                self.log(self.testCount, 'modifyTask', 'Modify a non-existing task', False)                    
                self.errCount += 1

        # =================================================================================================================

        # Task Sets
        if(table == 'all' or table == '*' or table == 'All' or table == 'Task_Sets'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called modify commands for Task Sets table:' + bcolors.ENDC + self.os.linesep

            # Modify an existing task set
            self.testCount += 1
            itemName = 'testSet' + str(self.testCount)
            itemNameMod = itemName + 'Mod'
            self.conn.execute("insert into Task_Sets values ('{0}','')".format(itemName,))
            result = self.api.modifySet(itemName, task_set=itemNameMod, description='Modified succesfully by unit test script')
            if(result == 'Success' and not self.inDb('Task_Sets', task_set=itemName) and self.inDb('Task_Sets', task_set=itemNameMod, description='Modified succesfully by unit test script')):
                self.log(self.testCount, 'modifySet', 'Modify an existing task set', True)                    
            else:
                self.log(self.testCount, 'modifySet', 'Modify an existing task set', False)                    
                self.errCount += 1

            # -----------------------------------------

            # Modify an existing task set and check if the dependencies were updated (foreign key)
            self.testCount += 1
            itemName = 'testSet' + str(self.testCount)
            itemNameMod = itemName + 'Mod'
            setName = itemName + 'Set'
            self.conn.execute("insert into Task_Sets values ('{0}','')".format(itemName,))
            self.conn.execute("insert into Classes values ('{0}','')".format(setName,))
            self.conn.execute("insert into Task_Sets_to_Classes values ('{0}','{1}')".format(itemName, setName))
            self.conn.execute("insert into Tasks values ('{0}','','','','','')".format(setName,))
            self.conn.execute("insert into Tasks_to_Task_Sets values ('{0}','{1}')".format(setName, itemName))
            result = self.api.modifySet(itemName, task_set=itemNameMod)
            if(result == 'Success' and not self.inDb('Task_Sets', task_set=itemName) and self.inDb('Task_Sets', task_set=itemNameMod) and self.inDb('Task_Sets_to_Classes', task_set=itemNameMod, node_class=setName) and self.inDb('Tasks_to_Task_Sets', task=setName, task_set=itemNameMod)):
                self.log(self.testCount, 'modifySet', 'Modify an existing task set and check if the dependencies were updated (foreign key)', True)                    
            else:
                self.log(self.testCount, 'modifySet', 'Modify an existing task set and check if the dependencies were updated (foreign key)', False)                    
                self.errCount += 1

            # -------------------------------------------------------------------------------------------------------------

            # Incorrect method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of incorrectly called modify commands for Task_Sets table:' + bcolors.ENDC + self.os.linesep

            # Modify a non-existing task set
            self.testCount += 1
            itemName = 'testSet' + str(self.testCount)
            itemNameMod = itemName + 'Mod'
            result = self.api.modifySet(itemName, task_set=itemNameMod, description='Modified succesfully by unit test script')
            if(result == ('modifySet: Unknown task set with name: ' + itemName) and not self.inDb('Task_Sets', task_set=itemName)):
                self.log(self.testCount, 'modifySet', 'Modify a non-existing task set', True)                    
            else:
                self.log(self.testCount, 'modifySet', 'Modify a non-existing task set', False)                    
                self.errCount += 1

        # =================================================================================================================

        # Node Classes
        if(table == 'all' or table == '*' or table == 'All' or table == 'Classes'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called modify commands for Classes table:' + bcolors.ENDC + self.os.linesep

            # Modify an existing node class
            self.testCount += 1
            itemName = 'testSet' + str(self.testCount)
            itemNameMod = itemName + 'Mod'
            self.conn.execute("insert into Classes values ('{0}','')".format(itemName,))
            result = self.api.modifyClass(itemName, node_class=itemNameMod, description='Modified succesfully by unit test script')
            if(result == 'Success' and not self.inDb('Classes', node_class=itemName) and self.inDb('Classes', node_class=itemNameMod, description='Modified succesfully by unit test script')):
                self.log(self.testCount, 'modifyClass', 'Modify an existing node class', True)                    
            else:
                self.log(self.testCount, 'modifyClass', 'Modify an existing node class', False)                    
                self.errCount += 1

            # -----------------------------------------

            # Modify an existing node class and check if the dependencies were updated (foreign key)
            self.testCount += 1
            itemName = 'testClass' + str(self.testCount)
            itemNameMod = itemName + 'Mod'
            setName = itemName + 'Set'
            self.conn.execute("insert into Classes values ('{0}','')".format(itemName,))
            self.conn.execute("insert into Nodes values ('{0}','')".format(setName,))
            self.conn.execute("insert into Classes_to_Nodes values ('{0}','{1}')".format(itemName, setName))
            self.conn.execute("insert into Task_Sets values ('{0}','')".format(setName,))
            self.conn.execute("insert into Task_Sets_to_Classes values ('{0}','{1}')".format(setName, itemName))
            result = self.api.modifyClass(itemName, node_class=itemNameMod)
            if(result == 'Success' and not self.inDb('Classes', node_class=itemName) and self.inDb('Classes', node_class=itemNameMod) and self.inDb('Classes_to_Nodes', node_class=itemNameMod, regex=setName) and self.inDb('Task_Sets_to_Classes', task_set=setName, node_class=itemNameMod)):
                self.log(self.testCount, 'modifyClass', 'Modify an existing node class and check if the dependencies were updated (foreign key)', True)                    
            else:
                self.log(self.testCount, 'modifyClass', 'Modify an existing node class and check if the dependencies were updated (foreign key)', False)                    
                self.errCount += 1

            # -------------------------------------------------------------------------------------------------------------

            # Incorrect method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of incorrectly called modify commands for Classes table:' + bcolors.ENDC + self.os.linesep

            # Modify a non-existing node class
            self.testCount += 1
            itemName = 'testClass' + str(self.testCount)
            itemNameMod = itemName + 'Mod'
            result = self.api.modifyClass(itemName, node_class=itemNameMod, description='Modified succesfully by unit test script')
            if(result == ('modifyClass: Unknown node class with name: ' + itemName) and not self.inDb('Classes', node_class=itemName)):
                self.log(self.testCount, 'modifyClass', 'Modify a non-existing node class', True)                    
            else:
                self.log(self.testCount, 'modifyClass', 'Modify a non-existing node class', False)                    
                self.errCount += 1

        # =================================================================================================================

        # Nodes
        if(table == 'all' or table == '*' or table == 'All' or table == 'Nodes'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called modify commands for Nodes table:' + bcolors.ENDC + self.os.linesep

            # Modify an existing node(s) entry
            self.testCount += 1
            itemName = 'testNode' + str(self.testCount)
            itemNameMod = itemName + 'Mod'
            self.conn.execute("insert into Nodes values ('{0}','')".format(itemName,))
            result = self.api.modifyNode(itemName, regex=itemNameMod, description='Modified succesfully by unit test script')
            if(result == 'Success' and not self.inDb('Nodes', regex=itemName) and self.inDb('Nodes', regex=itemNameMod, description='Modified succesfully by unit test script')):
                self.log(self.testCount, 'modifyNode', 'Modify an existing node(s) entry', True)                    
            else:
                self.log(self.testCount, 'modifyNode', 'Modify an existing node(s) entry', False)                    
                self.errCount += 1

            # -------------------------------------------------------------------------------------------------------------

            # Incorrect method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of incorrectly called modify commands for Nodes table:' + bcolors.ENDC + self.os.linesep

            # Modify a non-existing node(s) entry
            self.testCount += 1
            itemName = 'testNode' + str(self.testCount)
            itemNameMod = itemName + 'Mod'
            result = self.api.modifyNode(itemName, regex=itemNameMod, description='Modified succesfully by unit test script')
            if(result == ('modifyNode: Unknown nodes with regex: ' + itemName) and not self.inDb('Nodes', regex=itemName)):
                self.log(self.testCount, 'modifyNode', 'Modify a non-existing node(s) entry', True)                    
            else:
                self.log(self.testCount, 'modifyNode', 'Modify a non-existing node(s) entry', False)                    
                self.errCount += 1


    # =====================================================================================================================

    def testGet(self, table = 'all'):

        #                    d888b  d88888b d888888b 
        #                   88' Y8b 88'     `~~88~~' 
        #                   88      88ooooo    88    
        #                   88  ooo 88~~~~~    88    
        #                   88. ~8~ 88.        88    
        #                    Y888P  Y88888P    YP 

        print 4*self.os.linesep
        print '==================================================================='
        print '                       GET                                         '
        print '==================================================================='

        # Tasks
        if(table == 'all' or table == '*' or table == 'All' or table == 'Tasks'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called get commands for Tasks table:' + bcolors.ENDC + self.os.linesep

            # Get an existing task
            self.testCount += 1
            itemName = 'testTask' + str(self.testCount)
            self.conn.execute("insert into Tasks values ('{0}','bla','bla','bla','bla','bla')".format(itemName,))
            result = self.api.getTask(itemName)
            if(result == '{"data": [{"task": "' + itemName + '", "description": "bla", "utgid": "bla", "command_parameters": "bla", "task_parameters": "bla", "command": "bla"}]}' and self.inDb('Tasks', task=itemName)):
                self.log(self.testCount, 'getTask', 'Get an existing task', True)                    
            else:
                self.log(self.testCount, 'getTask', 'Get an existing task', False)                    
                self.errCount += 1

            # -------------------------------------------------------------------------------------------------------------

            # Incorrect method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of incorrectly called get commands for Tasks table:' + bcolors.ENDC + self.os.linesep

            # Get a non-existing task
            self.testCount += 1
            itemName = 'testTask' + str(self.testCount)
            result = self.api.getTask(itemName)
            if(result == ('getTask: Unknown task with name: ' + itemName) and not self.inDb('Tasks', task=itemName)):
                self.log(self.testCount, 'getTask', 'Get a non-existing task', True)                    
            else:
                self.log(self.testCount, 'getTask', 'Get a non-existing task', False)                    
                self.errCount += 1

        # =================================================================================================================

        # Task Sets
        if(table == 'all' or table == '*' or table == 'All' or table == 'Task_Sets'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called get commands for Task_Sets table:' + bcolors.ENDC + self.os.linesep

            # Get an existing task set
            self.testCount += 1
            itemName = 'testSet' + str(self.testCount)
            self.conn.execute("insert into Task_Sets values ('{0}','bla')".format(itemName,))
            result = self.api.getSet(itemName)
            if(result == '{"data": [{"task_set": "' + itemName + '", "description": "bla"}]}' and self.inDb('Task_Sets', task_set=itemName)):
                self.log(self.testCount, 'getSet', 'Get an existing task set', True)                    
            else:
                self.log(self.testCount, 'getSet', 'Get an existing task set', False)                    
                self.errCount += 1

            # -------------------------------------------------------------------------------------------------------------

            # Incorrect method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of incorrectly called get commands for Task_Sets table:' + bcolors.ENDC + self.os.linesep

            # Get a non-existing task set
            self.testCount += 1
            itemName = 'testSet' + str(self.testCount)
            result = self.api.getSet(itemName)
            if(result == ('getSet: Unknown task set with name: ' + itemName) and not self.inDb('Task_Sets', task_set=itemName)):
                self.log(self.testCount, 'getSet', 'Get a non-existing task set', True)                    
            else:
                self.log(self.testCount, 'getSet', 'Get a non-existing task set', False)                    
                self.errCount += 1

        # =================================================================================================================

        # Node Classes
        if(table == 'all' or table == '*' or table == 'All' or table == 'Classes'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called get commands for Classes table:' + bcolors.ENDC + self.os.linesep

            # Get an existing node class
            self.testCount += 1
            itemName = 'testClass' + str(self.testCount)
            self.conn.execute("insert into Classes values ('{0}','bla')".format(itemName,))
            result = self.api.getClass(itemName)
            if(result == '{"data": [{"node_class": "' + itemName + '", "description": "bla"}]}' and self.inDb('Classes', node_class=itemName)):
                self.log(self.testCount, 'getClass', 'Get an existing node class', True)                    
            else:
                self.log(self.testCount, 'getClass', 'Get an existing node class', False)                    
                self.errCount += 1

            # -------------------------------------------------------------------------------------------------------------

            # Incorrect method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of incorrectly called get commands for Classes table:' + bcolors.ENDC + self.os.linesep

            # Get a non-existing node class
            self.testCount += 1
            itemName = 'testClass' + str(self.testCount)
            result = self.api.getClass(itemName)
            if(result == ('getClass: Unknown node class with name: ' + itemName) and not self.inDb('Classes', node_class=itemName)):
                self.log(self.testCount, 'getClass', 'Get a non-existing node class', True)                    
            else:
                self.log(self.testCount, 'getClass', 'Get a non-existing node class', False)                    
                self.errCount += 1

        # =================================================================================================================

        # Nodes
        if(table == 'all' or table == '*' or table == 'All' or table == 'Nodes'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called get commands for Nodes table:' + bcolors.ENDC + self.os.linesep

            # Get an existing node(s) entry
            self.testCount += 1
            itemName = 'testNode' + str(self.testCount)
            self.conn.execute("insert into Nodes values ('{0}','bla')".format(itemName,))
            result = self.api.getNode(itemName)
            if(result == '{"data": [{"regex": "' + itemName + '", "description": "bla"}]}' and self.inDb('Nodes', regex=itemName)):
                self.log(self.testCount, 'getNode', 'Get an existing node(s) entry', True)                    
            else:
                self.log(self.testCount, 'getNode', 'Get an existing node(s) entry', False)                    
                self.errCount += 1

            # -------------------------------------------------------------------------------------------------------------

            # Incorrect method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of incorrectly called get commands for Nodes table:' + bcolors.ENDC + self.os.linesep

            # Get a non-existing node(s)
            self.testCount += 1
            itemName = 'testNode' + str(self.testCount)
            result = self.api.getNode(itemName)
            if(result == ('getNode: Unknown node with regex: ' + itemName) and not self.inDb('Nodes', regex=itemName)):
                self.log(self.testCount, 'getNode', 'Get a non-existing node(s)', True)                    
            else:
                self.log(self.testCount, 'getNode', 'Get a non-existing node(s)', False)                    
                self.errCount += 1

    # =====================================================================================================================
    
    def testAssign(self, table = 'all'):

        #            .d8b.  .d8888. .d8888. d888888b  d888b  d8b   db 
        #           d8' `8b 88'  YP 88'  YP   `88'   88' Y8b 888o  88 
        #           88ooo88 `8bo.   `8bo.      88    88      88V8o 88 
        #           88~~~88   `Y8b.   `Y8b.    88    88  ooo 88 V8o88 
        #           88   88 db   8D db   8D   .88.   88. ~8~ 88  V888 
        #           YP   YP `8888Y' `8888Y' Y888888P  Y888P  VP   V8P 


        print 4*self.os.linesep
        print '==================================================================='
        print '                      ASSIGN                                       '
        print '==================================================================='

        # Tasks
        if(table == 'all' or table == '*' or table == 'All' or table == 'Tasks'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called assign commands for Tasks table:' + bcolors.ENDC + self.os.linesep

            # Assign an existing task to an existing task set
            self.testCount += 1
            itemName = 'testTask' + str(self.testCount)
            setName = itemName + 'Set'
            self.conn.execute("insert into Tasks values ('{0}','','','','','')".format(itemName,))
            self.conn.execute("insert into Task_Sets values ('{0}','')".format(setName,))
            result = self.api.assignTask(itemName, setName)
            if(result == 'Success' and self.inDb('Tasks_to_Task_Sets', task=itemName, task_set=setName)):
                self.log(self.testCount, 'assignTask', 'Assign an existing task to an existing task set', True)                    
            else:
                self.log(self.testCount, 'assignTask', 'Assign an existing task to an existing task set', False)                    
                self.errCount += 1

            # -------------------------------------------------------------------------------------------------------------

            # Incorrect method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of incorrectly called assign commands for Tasks table:' + bcolors.ENDC + self.os.linesep

            # Assign an non-existing task to an existing task set (or vice versa or both non-existing)
            self.testCount += 1
            itemName = 'testTask' + str(self.testCount)
            setName = itemName + 'Set'
            result = self.api.assignTask(itemName, setName)
            expected_error = "(sqlite3.IntegrityError) FOREIGN KEY constraint failed [SQL: \"insert into Tasks_to_Task_Sets values ('" + itemName + "', '" + setName + "')\"] (Background on this error at: http://sqlalche.me/e/gkpj)"
            if(str(result) == expected_error):
                self.log(self.testCount, 'assignTask', 'Assign an non-existing task to an existing task set (or vice versa or both non-existing)', True)                    
            else:
                self.log(self.testCount, 'assignTask', 'Assign an non-existing task to an existing task set (or vice versa or both non-existing)', False)                    
                self.errCount += 1

        # =================================================================================================================

        # Task Sets
        if(table == 'all' or table == '*' or table == 'All' or table == 'Task_Sets'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called assign commands for Task_Sets table:' + bcolors.ENDC + self.os.linesep

            # Assign an existing task set to an existing node class
            self.testCount += 1
            itemName = 'testSet' + str(self.testCount)
            setName = itemName + 'Set'
            self.conn.execute("insert into Task_Sets values ('{0}','')".format(itemName,))
            self.conn.execute("insert into Classes values ('{0}','')".format(setName,))
            result = self.api.assignSet(itemName, setName)
            if(result == 'Success' and self.inDb('Task_Sets_to_Classes', task_set=itemName, node_class=setName)):
                self.log(self.testCount, 'assignSet', 'Assign an existing task set to an existing node class', True)                    
            else:
                self.log(self.testCount, 'assignSet', 'Assign an existing task set to an existing node class', False)                    
                self.errCount += 1

            # -------------------------------------------------------------------------------------------------------------

            # Incorrect method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of incorrectly called assign commands for Task_Sets table:' + bcolors.ENDC + self.os.linesep

            # Assign an non-existing task set to an existing node class (or vice versa or both non-existing)
            self.testCount += 1
            itemName = 'testSet' + str(self.testCount)
            setName = itemName + 'Set'
            result = self.api.assignSet(itemName, setName)
            expected_error = "(sqlite3.IntegrityError) FOREIGN KEY constraint failed [SQL: \"insert into Task_Sets_to_Classes values ('" + itemName + "','" + setName + "')\"] (Background on this error at: http://sqlalche.me/e/gkpj)"
            if(str(result) == expected_error):
                self.log(self.testCount, 'assignSet', 'Assign an non-existing task set to an existing node class (or vice versa or both non-existing)', True)                    
            else:
                self.log(self.testCount, 'assignSet', 'Assign an non-existing task set to an existing node class (or vice versa or both non-existing)', False)                    
                self.errCount += 1

        # =================================================================================================================

        # Node classes
        if(table == 'all' or table == '*' or table == 'All' or table == 'Classes'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called assign commands for Classes table:' + bcolors.ENDC + self.os.linesep

            # Assign an existing node class to an existing node(s) entry
            self.testCount += 1
            itemName = 'testClass' + str(self.testCount)
            setName = itemName + 'Set'
            self.conn.execute("insert into Classes values ('{0}','')".format(itemName,))
            self.conn.execute("insert into Nodes values ('{0}','')".format(setName,))
            result = self.api.assignClass(itemName, setName)
            if(result == 'Success' and self.inDb('Classes_to_Nodes', node_class=itemName, regex=setName)):
                self.log(self.testCount, 'assignClass', 'Assign an existing node class to an existing node(s) entry', True)                    
            else:
                self.log(self.testCount, 'assignClass', 'Assign an existing node class to an existing node(s) entry', False)                    
                self.errCount += 1

            # -------------------------------------------------------------------------------------------------------------

            # Incorrect method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of incorrectly called assign commands for Classes table:' + bcolors.ENDC + self.os.linesep

            # Assign an non-existing node class to an existing node(s) (or vice versa or both non-existing)
            self.testCount += 1
            itemName = 'testClass' + str(self.testCount)
            setName = itemName + 'Set'
            result = self.api.assignClass(itemName, setName)
            expected_error = "(sqlite3.IntegrityError) FOREIGN KEY constraint failed [SQL: \"insert into Classes_to_Nodes values ('" + itemName + "', '" + setName + "')\"] (Background on this error at: http://sqlalche.me/e/gkpj)"
            if(str(result) == expected_error):
                self.log(self.testCount, 'assignClass', 'Assign an non-existing node class to an existing node(s) (or vice versa or both non-existing)', True)                    
            else:
                self.log(self.testCount, 'assignClass', 'Assign an non-existing node class to an existing node(s) (or vice versa or both non-existing)', False)                    
                self.errCount += 1

    # =====================================================================================================================
    
    def testUnassign(self, table = 'all'):

        #   db    db d8b   db  .d8b.  .d8888. .d8888. d888888b  d888b  d8b   db 
        #   88    88 888o  88 d8' `8b 88'  YP 88'  YP   `88'   88' Y8b 888o  88 
        #   88    88 88V8o 88 88ooo88 `8bo.   `8bo.      88    88      88V8o 88 
        #   88    88 88 V8o88 88~~~88   `Y8b.   `Y8b.    88    88  ooo 88 V8o88 
        #   88b  d88 88  V888 88   88 db   8D db   8D   .88.   88. ~8~ 88  V888 
        #   ~Y8888P' VP   V8P YP   YP `8888Y' `8888Y' Y888888P  Y888P  VP   V8P 


        print 4*self.os.linesep
        print '==================================================================='
        print '                     UNASSIGN                                      '
        print '==================================================================='

        # Tasks
        if(table == 'all' or table == '*' or table == 'All' or table == 'Tasks'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called unassign commands for Tasks table:' + bcolors.ENDC + self.os.linesep

            # Unassign an existing task from an existing task set
            self.testCount += 1
            itemName = 'testTask' + str(self.testCount)
            setName = itemName + 'Set'
            self.conn.execute("insert into Tasks values ('{0}','','','','','')".format(itemName,))
            self.conn.execute("insert into Task_Sets values ('{0}','')".format(setName,))
            self.conn.execute("insert into Tasks_to_Task_Sets values ('{0}','{1}')".format(itemName, setName))
            result = self.api.unassignTask(itemName, setName)
            if(result == 'Success' and not self.inDb('Tasks_to_Task_Sets', task=itemName, task_set=setName)):
                self.log(self.testCount, 'unassignTask', 'Unassign an existing task from an existing task set', True)                    
            else:
                self.log(self.testCount, 'unassignTask', 'Unassign an existing task from an existing task set', False)                    
                self.errCount += 1

            # -------------------------------------------------------------------------------------------------------------

            # Incorrect method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of incorrectly called unassign commands for Tasks table:' + bcolors.ENDC + self.os.linesep

            # Unassign an non-existing task from an existing task set (or vice versa or both non-existing)
            self.testCount += 1
            itemName = 'testTask' + str(self.testCount)
            setName = itemName + 'Set'
            result = self.api.unassignTask(itemName, setName)
            expected_error = 'unassignTask: The specified assignment pair does not exist in the database: ' + itemName + ' <-> ' + setName
            if(str(result) == expected_error):
                self.log(self.testCount, 'unassignTask', 'Unassign an non-existing task from an existing task set (or vice versa or both non-existing)', True)                    
            else:
                self.log(self.testCount, 'unassignTask', 'Unassign an non-existing task from an existing task set (or vice versa or both non-existing)', False)                    
                self.errCount += 1

        # =================================================================================================================

        # Task Sets
        if(table == 'all' or table == '*' or table == 'All' or table == 'Task_Sets'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called unassign commands for Task_Sets table:' + bcolors.ENDC + self.os.linesep

            # Unassign an existing task set from an existing node class
            self.testCount += 1
            itemName = 'testSet' + str(self.testCount)
            setName = itemName + 'Set'
            self.conn.execute("insert into Task_Sets values ('{0}','')".format(itemName,))
            self.conn.execute("insert into Classes values ('{0}','')".format(setName,))
            self.conn.execute("insert into Task_Sets_to_Classes values ('{0}','{1}')".format(itemName, setName))
            result = self.api.unassignSet(itemName, setName)
            if(result == 'Success' and not self.inDb('Task_Sets_to_Classes', task_set=itemName, node_class=setName)):
                self.log(self.testCount, 'unassignSet', 'Unassign an existing task set from an existing node class', True)                    
            else:
                self.log(self.testCount, 'unassignSet', 'Unassign an existing task set from an existing node class', False)                    
                self.errCount += 1

            # -------------------------------------------------------------------------------------------------------------

            # Incorrect method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of incorrectly called unassign commands for Task_Sets table:' + bcolors.ENDC + self.os.linesep

            # Unassign an non-existing task set from an existing node class (or vice versa or both non-existing)
            self.testCount += 1
            itemName = 'testSet' + str(self.testCount)
            setName = itemName + 'Set'
            result = self.api.unassignSet(itemName, setName)
            expected_error = 'unassignSet: The specified assignment pair does not exist in the database: ' + itemName + ' <-> ' + setName
            if(str(result) == expected_error):
                self.log(self.testCount, 'unassignSet', 'Unassign an non-existing task set from an existing node class (or vice versa or both non-existing)', True)                    
            else:
                self.log(self.testCount, 'unassignSet', 'Unassign an non-existing task set from an existing node class (or vice versa or both non-existing)', False)                    
                self.errCount += 1

        # =================================================================================================================

        # Node Classes
        if(table == 'all' or table == '*' or table == 'All' or table == 'Classes'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called unassign commands for Classes table:' + bcolors.ENDC + self.os.linesep

            # Unassign an existing node class from an existing node(s) entry
            self.testCount += 1
            itemName = 'testClass' + str(self.testCount)
            setName = itemName + 'Set'
            self.conn.execute("insert into Classes values ('{0}','')".format(itemName,))
            self.conn.execute("insert into Nodes values ('{0}','')".format(setName,))
            self.conn.execute("insert into Classes_to_Nodes values ('{0}','{1}')".format(itemName, setName))
            result = self.api.unassignClass(itemName, setName)
            if(result == 'Success' and not self.inDb('Classes_to_Nodes', node_class=itemName, regex=setName)):
                self.log(self.testCount, 'unassignClass', 'Unassign an existing node class from an existing node(s) entry', True)                    
            else:
                self.log(self.testCount, 'unassignClass', 'Unassign an existing node class from an existing node(s) entry', False)                    
                self.errCount += 1

            # -------------------------------------------------------------------------------------------------------------

            # Incorrect method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of incorrectly called unassign commands for Classes table:' + bcolors.ENDC + self.os.linesep

            # Unassign an non-existing node class from an existing node(s) (or vice versa or both non-existing)
            self.testCount += 1
            itemName = 'testClass' + str(self.testCount)
            setName = itemName + 'Set'
            result = self.api.unassignClass(itemName, setName)
            expected_error = 'unassignClass: The specified assignment pair does not exist in the database: ' + itemName + ' <-> ' + setName
            if(str(result) == expected_error):
                self.log(self.testCount, 'unassignClass', 'Unassign an non-existing node class from an existing node(s) (or vice versa or both non-existing)', True)                    
            else:
                self.log(self.testCount, 'unassignClass', 'Unassign an non-existing node class from an existing node(s) (or vice versa or both non-existing)', False)                    
                self.errCount += 1


    # =====================================================================================================================
    #                         Helper Methods
    # =====================================================================================================================

    def inDb(self, table, **args):

        if (len(args) == 0):
            print 'Error in api tester inDb method: provided no column: value pairs as search parameters' + self.os.linesep
            return

        # Construct the SQL statement looking for the entry specified with keys in args in table
        statement = "select * from " + table + " where "
        cnt = 0
        for key in args:
            if(cnt != 0):
                statement += ' and '
            statement += (key + "='" + args[key] + "'")
            cnt += 1

        # Execute the statement
        result = self.conn.execute(statement)

        # Check if the query returned the same object, return true if it did (result array will have size of > 1 - at least 1 result with specified params should be found - in case of PK)
        arr = []
        for row in result:
            arr.append(row)
        if len(arr) == 0:
            return False
        else:
            return True

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
            self.sys.stdout.write(question + prompt)
            choice = raw_input().lower()
            if default is not None and choice == '':
                return valid[default]
            elif choice in valid:
                return valid[choice]
            else:
                self.sys.stdout.write("Please respond with 'yes' or 'no' "
                                "(or 'y' or 'n').\n")            

# =========================================================================================================================

    def log(self, test_number, function_name, function_description, result):
        tests_total_number = 54
        description_field_width = 100
        if(result):
            result = 'passed'
        else:
            result = 'FAILED'
            print bcolors.FAIL

        FORMAT = '%03d%-s%03d  %-13s  %-' + str(description_field_width) + 's  %-6s' # %-10s' # Possibly add timer
        print FORMAT % (test_number, '/', tests_total_number, function_name + (13 - len(function_name))*'.', 
            function_description + (description_field_width - len(function_description))*'.', result) + bcolors.ENDC

# =========================================================================================================================

# Print statements colouring - when script is finished, those guys should make it fancy (last development step)
class bcolors:

    # Colour macros
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

# =========================================================================================================================

# Run as executable cli script
if __name__ == "__main__":
    
    # Run colouring demo
    # print 'Demonstration of font colouring:'
    # print bcolors.HEADER + 'Header' + bcolors.ENDC
    # print bcolors.OKBLUE + 'Okblue' + bcolors.ENDC
    # print bcolors.OKGREEN + 'Okgreen' + bcolors.ENDC
    # print bcolors.WARNING + 'Warning' + bcolors.ENDC
    # print bcolors.FAIL + 'Fail' + bcolors.ENDC
    # print bcolors.ENDC + 'Endc' + bcolors.ENDC
    # print bcolors.BOLD + 'Bold' + bcolors.ENDC
    # print bcolors.UNDERLINE + 'Underline' + bcolors.ENDC

    # Run the main api tester with all tests
    tester = apiTest()
    tester.testAll()
