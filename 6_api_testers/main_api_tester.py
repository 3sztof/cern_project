#!/usr/bin/env python




# =========================================================================================================================
#                    LHCb Online Farm Process Explorer
#      Main API tester - validates api against desired DB changes and errors
#                            
#                           K.Wilczynski 08.2018
# =========================================================================================================================





class apiTester():

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

        self.testAdd()
        # self.testDelete()
        # self.testModify()
        # self.testGet()
        # self.testAssign()
        # self.testUnassign()
        if(self.errCount == 0):
            print bcolors.OKBLUE + 'Finished API testing without any error' + bcolors.ENDC + self.os.linesep
        else:
            print bcolors.FAIL + 'Finished API testing with ' + str(self.errCount) + ' errors' + bcolors.ENDC + self.os.linesep

        if(self.yes_or_no('The main database has been modified during the tests, do you want to rebuild it using settings and data from initDB script? (reccomended)')):      
            try:
                self.os.system(self._initDB_path)
                print self.os.linesep + 'The database has been rebuilt, returning test result - error count'
            except: 
                print self.os.linesep + 'WARNING: Database rebuilding failed. Are you running Linux? Please make sure that 664 permissions\
                    are set on database root folder and initDB script.'

        return self.errCount
        
    # =====================================================================================================================

    # Run only add methods (specify which table will be optional: Tasks, Task_Sets, Clsses, Nodes)
    def testAdd(self, table='all'):

        # =================================================================================================================

        # Tasks
        if(table == 'all' or table == '*' or table == 'All' or table == 'Tasks'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called add commands for Tasks table:' + bcolors.ENDC + self.os.linesep

            # Add task providing all parameters
            result = self.api.addTask('testTask', utgid='testUtgid', command='testCommand', command_parameters='testScriptParams', 
                                    task_parameters='testPcaddParams', description='testDescription')
            if(result == 'Success' and self.inDb('Tasks', task='testTask', utgid='testUtgid', command='testCommand', command_parameters='testScriptParams', 
                                    task_parameters='testPcaddParams', description='testDescription')):
                print 'addTask providing all parameters: success'
            else:
                print bcolors.FAIL + 'addTask providing all parameters: failure:' + bcolors.ENDC
                self.errCount += 1

            # Add task providing only required parameter (rest should be empty strings: '')
            result = self.api.addTask('testTask1')
            if(result == 'Success' and self.inDb('Tasks', task='testTask1', utgid='', command='', command_parameters='', 
                                    task_parameters='', description='')):
                print 'addTask providing only required parameter: success'
            else:
                print bcolors.FAIL + 'addTask providing only required parameter: failure:' + bcolors.ENDC
                self.errCount += 1
            
            # Incorret method calls tests (error handling)
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of error handling for Tasks table' + bcolors.ENDC + self.os.linesep

            # Unique constraint violation test
            self.conn.execute("insert into Tasks values ('{0}','','','','','')".format('testTask2',))
            result = self.api.addTask('testTask2', utgid='', command='', command_parameters='', 
                                    task_parameters='', description='')
            expected_error = "(sqlite3.IntegrityError) UNIQUE constraint failed: Tasks.task, Tasks.utgid, Tasks.command, Tasks.task_parameters, Tasks.command_parameters [SQL: \"insert into Tasks values ('testTask2','','','','','')\"] (Background on this error at: http://sqlalche.me/e/gkpj)"
            if(str(result) == expected_error and self.inDb('Tasks', task='testTask2', utgid='', command='', command_parameters='', 
                                    task_parameters='', description='')):
                print 'addTask unique constraint violation test: success'
            else:
                print bcolors.FAIL + 'addTask unique constraint violation test: failure:' + bcolors.ENDC
                self.errCount += 1
        
        # =================================================================================================================

        # Task sets
        if(table == 'all' or table == '*' or table == 'All' or table == 'Task_Sets'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called add commands for Task_Sets table:' + bcolors.ENDC + self.os.linesep

            # Add task set providing all parameters
            result = self.api.addSet('testSet', description='testDescription')
            if(result == 'Success' and self.inDb('Task_Sets', task_set='testSet', description='testDescription')):
                print 'addSet providing all parameters: success'
            else:
                print bcolors.FAIL + 'addSet providing all parameters: failure:' + bcolors.ENDC
                self.errCount += 1

            # -------------------------------------------------------------------------------------------------------------

            # Add task set providing only required parameter (rest should be empty strings: '')
            result = self.api.addSet('testSet1')
            if(result == 'Success' and self.inDb('Task_Sets', task_set='testSet1', description='')):
                print 'addSet providing only required parameter: success'
            else:
                print bcolors.FAIL + 'addSet providing only required parameter: failure:' + bcolors.ENDC
                self.errCount += 1

            # Incorret method calls tests (error handling)
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of error handling for Task_Sets table' + bcolors.ENDC + self.os.linesep

        # =================================================================================================================

        # Node Classes
        if(table == 'all' or table == '*' or table == 'All' or table == 'Classes'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called add commands for Classes table:' + bcolors.ENDC + self.os.linesep

            # Add node class providing all parameters
            result = self.api.addClass('testClass', description='testDescription')
            if(result == 'Success' and self.inDb('Classes', node_class='testClass', description='testDescription')):
                print 'addClass providing all parameters: success'                 
            else:
                print bcolors.FAIL + 'addClass providing all parameters: failure:'  + bcolors.ENDC
                self.errCount += 1

            # Add node class providing only required parameter (rest should be empty strings: '')
            result = self.api.addClass('testClass1')
            if(result == 'Success' and self.inDb('Classes', node_class='testClass1', description='')):
                print 'addClass providing only required parameter: success'
            else:
                print bcolors.FAIL + 'addClass providing only required parameter: failure:' + bcolors.ENDC
                self.errCount += 1

            # -------------------------------------------------------------------------------------------------------------

            # Incorret method calls tests (error handling)
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of error handling for Classes table' + bcolors.ENDC + self.os.linesep

        # =================================================================================================================

        # Nodes
        if(table == 'all' or table == '*' or table == 'All' or table == 'Nodes'):

            # Correct method calls tests
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of correctly called add commands for Nodes table:' + bcolors.ENDC + self.os.linesep

            # Add node providing all parameters
            result = self.api.addNode('testNode', description='testDescription')
            if(result == 'Success' and self.inDb('Nodes', regex='testNode', description='testDescription')):
                print 'addNode providing all parameters: success'
            else:
                print bcolors.FAIL + 'addNode providing all parameters: failure:' + bcolors.ENDC
                self.errCount += 1

            # Add task set providing only required parameter (rest should be empty strings: '')
            result = self.api.addNode('testNode1')
            if(result == 'Success' and self.inDb('Nodes', regex='testNode1', description='')):
                print 'addNode providing only required parameter: success'
            else:
                print bcolors.FAIL + 'addNode providing only required parameter: failure:' + bcolors.ENDC
                self.errCount += 1

            # -------------------------------------------------------------------------------------------------------------

            # Incorret method calls tests (error handling)
            print self.os.linesep + bcolors.UNDERLINE + 'Running tests of error handling for Nodes table' + bcolors.ENDC + self.os.linesep
                     
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
    a = apiTester()
    a.testAll()
