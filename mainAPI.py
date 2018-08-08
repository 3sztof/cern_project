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

        # Commit the changes and close the connection - no longer needed as I switched from sqlite3 to sqlalchemy
        #self.conn.commit()
        #self.conn.close()
        return

    # =====================================================================================================================
    #                              Tasks
    # =====================================================================================================================

    def addTask(self, task, utgid, command, task_parameters, command_parameters, description):

        try:
            self.conn.execute("insert into Tasks values ('{0}','{1}','{2}','{3}','{4}','{5}')".format(task, utgid, 
                            command, task_parameters, command_parameters, description))
            return 'Success'

        except Exception as e: return e

    # ---------------------------------------------------------------------------------------------------------------------

    def deleteTask(self, task):

        if(not self.inDb('Tasks', 'task', task)):
            return 'Error: the specified entry does not exist in the database'
        
        try:
            self.conn.execute("delete from Tasks where task='{0}'".format(task))
            return 'Success'

        except Exception as e: return e

    # ---------------------------------------------------------------------------------------------------------------------

    def modifyTask(self, task, mod_task, mod_utgid, mod_command, mod_task_parameters, mod_command_parameters, 
                mod_description):

        if(not self.inDb('Tasks', 'task', task)):
            return 'Error: the specified entry does not exist in the database'
        
        try:
            self.conn.execute("update Tasks set task='{0}', utgid='{1}', command='{2}', task_parameters='{3}', \
                            command_parameters='{4}', description='{5}' where task='{6}'".format(mod_task, 
                            mod_utgid, mod_command, mod_task_parameters, mod_command_parameters, mod_description, 
                            task))
            return 'Success'

        except Exception as e: return e

    # ---------------------------------------------------------------------------------------------------------------------

    def getTask(self, task):

        if(not self.inDb('Tasks', 'task', task)):
            return 'Error: the specified entry does not exist in the database'

        try:
            query = self.conn.execute("select * from Tasks where task='{0}'".format(task))
            result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
            return self.json.dumps(result)

        except Exception as e: return e

    # ---------------------------------------------------------------------------------------------------------------------

    def assignTask(self, task, task_set):

        # if(not self.inDb('Tasks', 'task', task)):
        #     return 'Error: the specified entry does not exist in the database'

        try:
            self.conn.execute("insert into Tasks_to_Task_Sets values ('{0}', '{1}')".format(task, task_set))
            return 'Success'

        except Exception as e: return e

    # ---------------------------------------------------------------------------------------------------------------------
    
    def unassignTask(self, task, task_set):

        try:
            self.conn.execute("delete from Tasks_to_Task_Sets where task='{0}' and task_set='{1}'".format(task, 
                            task_set))
            return 'Success'

        except Exception as e: return e

    # =====================================================================================================================
    #                            Task Sets
    # =====================================================================================================================

    def addSet(self, task_set, description):

        try:
            self.conn.execute("insert into Task_Sets values ('{0}','{1}')".format(task_set, description))
            return 'Success'

        except Exception as e: return e

    # ---------------------------------------------------------------------------------------------------------------------

    def deleteSet(self, task_set):

        if(not self.inDb('Task_Sets', 'task_set', task_set)):
            return 'Error: the specified entry does not exist in the database'

        try:
            self.conn.execute("delete from Task_Sets where task_set='{0}'".format(task_set,))
            return 'Success'

        except Exception as e: return e

    # ---------------------------------------------------------------------------------------------------------------------

    def modifySet(self, task_set, mod_task_set, mod_description):

        if(not self.inDb('Task_Sets', 'task_set', task_set)):
            return 'Error: the specified entry does not exist in the database'
        
        try:
            self.conn.execute("modify Task_Sets set task_set='{0}', description='{1}' where task_set=\
                            '{2}'".format(mod_task_set, mod_description, task_set))
            return 'Success'

        except Exception as e: return e

    # ---------------------------------------------------------------------------------------------------------------------

    def getSet(self, task_set):
        
        if(not self.inDb('Task_Sets', 'task_set', task_set)):
            return 'Error: the specified entry does not exist in the database'

        try:
            query = self.conn.execute("select * from Task_Sets where task_set='{0}'".format(task_set,))
            result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
            return self.json.dumps(result)

        except Exception as e: return e
    
    # ---------------------------------------------------------------------------------------------------------------------

    def assignSet(self, task_set, node_class):

        # if(not self.inDb('Task_Sets', 'task_set', task_set)):
        #     return 'Error: the specified entry does not exist in the database'

        try:
            self.conn.execute("insert into Task_Sets_to_Classes values ('{0}','{1}')".format(task_set, node_class))
            return 'Success'

        except Exception as e: return e

    # ---------------------------------------------------------------------------------------------------------------------

    def unassignSet(self, task_set, node_class):

        try:
            self.conn.execute("delete from Task_Sets_to_Classes where task_set='{0}' and class=\
                            '{1}'".format(task_set, node_class))
            return 'Success'

        except Exception as e: return e

    # =====================================================================================================================
    #                           Node Classes
    # =====================================================================================================================

    def addClass(self, node_class, description):

        try:
            self.conn.execute("insert into Classes values ('{0}','{1}')".format(node_class, description))
            return 'Success'

        except Exception as e: return e

    # ---------------------------------------------------------------------------------------------------------------------

    def deleteClass(self, node_class):

        if(not self.inDb('Classes', 'class', node_class)):
            return 'Error: the specified entry does not exist in the database'

        try:
            self.conn.execute("delete from Classes where class='{0}'".format(node_class,))
            return 'Success'
        
        except Exception as e: return e

    # ---------------------------------------------------------------------------------------------------------------------
  
    def modifyClass(self, node_class, mod_node_class, mod_description):

        if(not self.inDb('Classes', 'class', node_class)):
            return 'Error: the specified entry does not exist in the database'

        try:
            self.conn.execute("update Classes set class='{0}', description='{1}' where class=\
                            '{2}'".format(mod_node_class, mod_description, node_class))
            return 'Success'

        except Exception as e: return e

    # ---------------------------------------------------------------------------------------------------------------------
    
    def getClass(self, node_class):

        if(not self.inDb('Classes', 'class', node_class)):
            return 'Error: the specified entry does not exist in the database'

        try:
            query = self.conn.execute("select * from Classes where class='{0}'".format(node_class,))
            result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
            return self.json.dumps(result)

        except Exception as e: return e

    # ---------------------------------------------------------------------------------------------------------------------
    
    def assignClass(self, node_class, node_regex):

        # if(not self.inDb('Classes', 'class', node_class)):
        #     return 'Error: the specified entry does not exist in the database'

        try:
            self.conn.execute("insert into Classes_to_Nodes values ('{0}', '{1}')".format(node_class, node_regex))
            return 'Success'
        
        except Exception as e: return e

    # ---------------------------------------------------------------------------------------------------------------------

    def unassignClass(self, node_class, node_regex):

        try:
            self.conn.execute("delete from Classes_to_Nodes where class='{0}' and regex='{1}'".format(node_class,
                            node_regex))
            return 'Success'
        
        except Exception as e: return e

    # =====================================================================================================================
    #                              Nodes
    # =====================================================================================================================

    def addNode(self, regex, description):

        try:
            self.conn.execute("insert into Nodes values ('{0}','{1}')".format(regex, description))
            return 'Success'

        except Exception as e: return e
    
    # ---------------------------------------------------------------------------------------------------------------------
    
    def deleteNode(self, regex):

        try:
            self.conn.execute("delete from Nodes where regex='{0}'".format(regex,))
            return 'Success'

        except Exception as e: return e
    
    # ---------------------------------------------------------------------------------------------------------------------
    
    def modifyNode(self, regex, mod_regex, mod_description):

        try:
            self.conn.execute("update Nodes set regex='{0}', description='{1}' where regex='{2}'".format(mod_regex,
                            mod_description, regex))
            return 'Success'

        except Exception as e: return e
    
    # ---------------------------------------------------------------------------------------------------------------------
    
    def getNode(self, regex):

        try:
            query = self.conn.execute("select * from Nodes where regex='{0}'".format(regex,))
            result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
            return self.json.dumps(result)

        except Exception as e: return e

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

