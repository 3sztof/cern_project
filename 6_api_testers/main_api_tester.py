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
            print 'Finished API testing without any error' + self.os.linesep
        else:
            print 'Finished API testing with ' + self.errCount + ' errors' + self.os.linesep

        return self.errCount
        
    # Run only add methods (specify which table will be optional: Tasks, Task_Sets, Clsses, Nodes)
    def testAdd(self, table='all'):

        # Tasks
        if(table == 'all' or table == '*' or table == 'All' or table == 'Tasks'):

            # Correct method calls tests
            print self.os.linesep + 'Running tests of correctly called add commands for Tasks table:' + self.os.linesep

            # Add task providing all parameters
            result = self.api.addTask('testTask', utgid='testUtgid', command='testCommand', command_parameters='testScriptParams', 
                                    task_parameters='testPcaddParams', description='testDescription')
            if(result == 'Success' and self.inDb('Tasks', task='testTask', utgid='testUtgid', command='testCommand', command_parameters='testScriptParams', 
                                    task_parameters='testPcaddParams', description='testDescription')):
                print 'addTask: success'
            else:
                print 'addTask: failure:'
                print result + self.os.linesep
                self.errCount += 1

            # Add task providing only required parameter (rest should be empty strings: '')
            result = self.api.addTask('testTask1')
            if(result == 'Success' and self.inDb('Tasks', task='testTask', utgid='', command='', command_parameters='', 
                                    task_parameters='', description='')):
                print 'addTask: success'
            else:
                print 'addTask: failure:'
                print result + self.os.linesep
                self.errCount += 1
            
            # Incorret method calls tests (error handling)
            print self.os.linesep + 'Running tests of error handling for Tasks table' + self.os.linesep
        
        # Task sets
        if(table == 'all' or table == '*' or table == 'All' or table == 'Task_Sets'):

            # Correct method calls tests
            print self.os.linesep + 'Running tests of correctly called add commands for Task_Sets table:' + self.os.linesep

            # Add task set providing all parameters
            result = self.api.addSet('testSet', description='testDescription')
            if(result == 'Success' and self.inDb('Task_Sets', task_set='testSet', description='testDescription')):
                print 'addSet: success'
            else:
                print 'addSet: failure:'
                print result + self.os.linesep
                self.errCount += 1

            # Incorret method calls tests (error handling)
            print self.os.linesep + 'Running tests of error handling for Task_Sets table' + self.os.linesep

        # Node Classes
        if(table == 'all' or table == '*' or table == 'All' or table == 'Classes'):

            # Correct method calls tests
            print self.os.linesep + 'Running tests of correctly called add commands for Classes table:' + self.os.linesep

            # Add node class providing all parameters
            result = self.api.addSet('testClass', description='testDescription')
            if(result == 'Success' and self.inDb('Classes', node_class='testClass', description='testDescription')):
                print 'addClass: success'                 
            else:
                print 'addClass: failure:'
                print result + self.os.linesep
                self.errCount += 1

            # Incorret method calls tests (error handling)
            print self.os.linesep + 'Running tests of error handling for Classes table' + self.os.linesep

        # Nodes
        if(table == 'all' or table == '*' or table == 'All' or table == 'Nodes'):

            # Correct method calls tests
            print self.os.linesep + 'Running tests of correctly called add commands for Nodes table:' + self.os.linesep

            # Add node providing all parameters
            result = self.api.addSet('testNode', description='testDescription')
            if(result == 'Success' and self.inDb('Nodes', regex='testNode', description='testDescription')):
                print 'addClass: success'
            else:
                print 'addClass: failure:'
                print result + self.os.linesep
                self.errCount += 1

            # Incorret method calls tests (error handling)
            print self.os.linesep + 'Running tests of error handling for Nodes table' + self.os.linesep
                     
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



# Run as executable cli script
if __name__ == "__main__":
    a = apiTester()
    a.testAll()