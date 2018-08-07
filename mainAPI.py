#!/usr/bin/env python


# =========================================================================================================================
#             LHCb Online Farm Task Manager Database API (main class)
#                            K.Wilczynski 08.2018
# =========================================================================================================================

class mainAPI():

    # =====================================================================================================================
    #                           Constructor
    # =====================================================================================================================

    def __init__(self):

        # Imports
        from sqlalchemy import create_engine
        import json

        # Configs
        self._database = '/home/leon/Desktop/Cern_Project/SQL_DB/LHCb.db'
        self.json = json

        # Initiate a connection, cursor object and turn on the constraints to protect the schema (created in initDB.py)
        self.db_connect = create_engine('sqlite:///' + self._database)
        self.conn = self.db_connect.connect()
        self.conn.execute("PRAGMA foreign_keys = ON")


    # =====================================================================================================================
    #                            Destructor
    # =====================================================================================================================

    def __del__(self):

        # Commit the changes and close the connection
        #self.conn.commit()
        #self.conn.close()
        return

    # =====================================================================================================================
    #                              Tasks
    # =====================================================================================================================

    def addTask(self, task, utgid, command, task_parameters, command_parameters, description):

        self.conn.execute("insert into Tasks values ('{0}','{1}','{2}','{3}','{4}','{5}')".format(task, utgid, command, 
                        task_parameters, command_parameters, description))
        return 'Success'

    # ---------------------------------------------------------------------------------------------------------------------

    def deleteTask(self, task):

        self.conn.execute("delete from Tasks where task='{0}'".format(task))
        return 'Success'

    # ---------------------------------------------------------------------------------------------------------------------

    def modifyTask(self, task, mod_task, mod_utgid, mod_command, mod_task_parameters, mod_command_parameters, mod_description):
        
        self.conn.execute("update Tasks set task='{0}', utgid='{1}', command='{2}', task_parameters='{3}', \
                        command_parameters='{4}', description='{5}' where task='{6}'".format(mod_task, mod_utgid, 
                        mod_command, mod_task_parameters, mod_command_parameters, mod_description, task))
        return 'Success'

    # ---------------------------------------------------------------------------------------------------------------------

    def getTask(self, task):

        query = self.conn.execute("select * from Tasks where task='{0}'".format(task))
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        return self.json.dumps(result)

    # ---------------------------------------------------------------------------------------------------------------------

    def assignTask(self, task, task_set):

        self.conn.execute("insert into Tasks_to_Task_Sets values ('{0}', '{1}')".format(task, task_set))
        return 'Success'

    # ---------------------------------------------------------------------------------------------------------------------
    
    def unassignTask(self, task, task_set):

        self.conn.execute("delete from Tasks_to_Task_Sets where task='{0}' and task_set='{1}'".format(task, task_set))
        return 'Success'

    # =====================================================================================================================
    #                            Task Sets
    # =====================================================================================================================

    def addSet(self, ):
        return

    # ---------------------------------------------------------------------------------------------------------------------

    def deleteSet():
        return

    # ---------------------------------------------------------------------------------------------------------------------

    def modifySet():
        return

    # ---------------------------------------------------------------------------------------------------------------------

    def getSet():
        return

    # ---------------------------------------------------------------------------------------------------------------------

    def assignSet():
        return

    # ---------------------------------------------------------------------------------------------------------------------

    def unassignSet():
        return

    # =====================================================================================================================
    #                           Node Classes
    # =====================================================================================================================

    def addClass():
        return

    # ---------------------------------------------------------------------------------------------------------------------

    def deleteClass():
        return

    # ---------------------------------------------------------------------------------------------------------------------
  
    def modifyClass():
        return

    # ---------------------------------------------------------------------------------------------------------------------
    
    def getClass():
        return

    # ---------------------------------------------------------------------------------------------------------------------
    
    def assignClass():
        return

    # ---------------------------------------------------------------------------------------------------------------------

    def unassignClass():
        return

    # =====================================================================================================================
    #                              Nodes
    # =====================================================================================================================

    def addNode():
        return

    # ---------------------------------------------------------------------------------------------------------------------
    
    def deleteNode():
        return

    # ---------------------------------------------------------------------------------------------------------------------
    
    def modifyNode():
        return

    # ---------------------------------------------------------------------------------------------------------------------
    
    def getNode():
        return

    # =====================================================================================================================
    #                          Helper Methods
    # =====================================================================================================================

    def handleDbError(e):
        try:
            print "Database Error [%d]: %s" % (e.args[0], e.args[1]) + os.linesep
            return None
        except IndexError:
            print "Database Error: %s" % str(e) + os.linesep
            return None
        except TypeError, e:
            print(e)
            return None
        except ValueError, e:
            print(e)
            return None


