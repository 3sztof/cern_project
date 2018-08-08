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

    def addSet(self, task_set, description):

        self.conn.execute("insert into Task_Sets values ('{0}','{1}')".format(task_set, description))
        return 'Success'

    # ---------------------------------------------------------------------------------------------------------------------

    def deleteSet(self, task_set):

        self.conn.execute("delete from Task_Sets where task_set='{0}'".format(task_set,))
        return 'Success'


    # ---------------------------------------------------------------------------------------------------------------------

    def modifySet(self, task_set, mod_task_set, mod_description):

        self.conn.execute("modify Task_Sets set task_set='{0}', description='{1}' where task_set='{2}'".format(mod_task_set,
                        mod_description, task_set))
        return 'Success'

    # ---------------------------------------------------------------------------------------------------------------------

    def getSet(self, task_set):
        
        query = self.conn.execute("select * from Task_Sets where task_set='{0}'".format(task_set,))
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        return self.json.dumps(result)
    
    # ---------------------------------------------------------------------------------------------------------------------

    def assignSet(self, task_set, node_class):

        self.conn.execute("insert into Task_Sets_to_Classes values ('{0}','{1}')".format(task_set, node_class))
        return 'Success'

    # ---------------------------------------------------------------------------------------------------------------------

    def unassignSet(self, task_set, node_class):

        self.conn.execute("delete from Task_Sets_to_Classes where task_set='{0}' and class='{1}'".format(task_set, 
                        node_class))
        return 'Success'

    # =====================================================================================================================
    #                           Node Classes
    # =====================================================================================================================

    def addClass(self, node_class, description):

        self.conn.execute("insert into Classes values ('{0}','{1}')".format(node_class, description))
        return 'Success'

    # ---------------------------------------------------------------------------------------------------------------------

    def deleteClass(self, node_class):

        self.conn.execute("delete from Classes where class='{0}'".format(node_class,))
        return 'Success'

    # ---------------------------------------------------------------------------------------------------------------------
  
    def modifyClass(self, node_class, mod_node_class, mod_description):

        self.conn.execute("update Classes set class='{0}', description='{1}' where class='{2}'".format(mod_node_class,
                        mod_description, node_class))
        return 'Success'

    # ---------------------------------------------------------------------------------------------------------------------
    
    def getClass(self, node_class):

        query = self.conn.execute("select * from Classes where class='{0}'".format(node_class,))
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        return self.json.dumps(result)

    # ---------------------------------------------------------------------------------------------------------------------
    
    def assignClass(self, node_class, node_regex):

        self.conn.execute("insert into Nodes values ('{0}', '{1}')".format(node_class, node_regex))
        return 'Success'

    # ---------------------------------------------------------------------------------------------------------------------

    def unassignClass(self, node_class, node_regex):

        self.conn.execute("delete from Nodes where class='{0}' and regex='{1}'".format(node_class, node_regex))
        return 'Success'

    # =====================================================================================================================
    #                              Nodes
    # =====================================================================================================================

    def addNode():

        self.conn.execute("".format())
        return 'Success'
    
    # ---------------------------------------------------------------------------------------------------------------------
    
    def deleteNode():
        self.conn.execute("".format())
        return 'Success'
    # ---------------------------------------------------------------------------------------------------------------------
    
    def modifyNode():
        self.conn.execute("".format())
        return 'Success'
    # ---------------------------------------------------------------------------------------------------------------------
    
    def getNode():
        self.conn.execute("".format())
        return 'Success'
    # =====================================================================================================================
    #                          Helper Methods
    # =====================================================================================================================



