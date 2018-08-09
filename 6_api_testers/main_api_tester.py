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
        self.api = mainAPI.mainAPI()
        self.my_path = self.os.path.dirname(self.os.path.realpath(__file__))
        self._database = self.my_path + self.os.sep + '..' + self.os.sep + '0_main_api' + self.os.sep + 'LHCb.db'
        self._database_root_path = self.my_path + self.os.sep + '..' + self.os.sep + '0_main_api'
        self._initDB_path = self.my_path + self.os.sep + '..' + self.os.sep + '0_main_api' + self.os.sep + 'initDB.py'

    # =====================================================================================================================
    #                          Tester method
    # =====================================================================================================================

    def runTest(self):

        # Reinitialize the database using initDB.py script (initDB.pt must be executable: chmod +x)
        if(self.yes_or_no('Do you want to rebuild the main database using settings and data from initDB script? (reccomended)')):      
            try:
                self.os.system(self._initDB_path)
                print self.os.linesep + 'The database has been rebuilt'
            except: 
                print self.os.linesep + 'WARNING: Database rebuilding failed. Are you running Linux? Please make sure that 664 permissions\
                    are set on database root folder and initDB script.'

        # Initiate a connection, cursor object and turn on the constraints to protect the schema (created in initDB.py)
        self.db_connect = self.create_engine('sqlite:///' + self._database)
        self.conn = self.db_connect.connect()
        self.conn.execute("PRAGMA foreign_keys = ON")
        print 'Connected to the database at: ' + self._database

        # =================================================================================================================
        #               Correct method calls tests
        # =================================================================================================================

        print self.os.linesep + 'Running tests of correctly called commands...' + self.os.linesep

        # Add task providing all parameters
        result = self.api.addTask('apiTester', utgid='test_utgid', command='test.sh', command_parameters='script params', 
                                task_parameters='pcadd params', description='testtest')
        if(result == 'Success' and self.inDb('Tasks', 'task', 'apiTester')):
            print 'addTask: success'
        else:
            print 'addTask: failure:' + self.os.linesep
            print result
            return

    # =====================================================================================================================
    #                          Helper Methods
    # =====================================================================================================================

    def inDb(self, table, column, value):

        arr = []
        result = self.conn.execute('SELECT ' + column + ' FROM ' + table + ' WHERE ' + column + '="' + value + '\"')
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
    a.runTest()