#!/usr/bin/env python




# =========================================================================================================================
#                     LHCb Online Farm Process Explorer
#                                 Main API 
#                           
#                           K.Wilczynski 08.2018
# =========================================================================================================================





class mainAPI():

    # =====================================================================================================================
    #                           Constructor
    # =====================================================================================================================

    def __init__(self):

        # Imports
        from sqlalchemy import create_engine
        import json
        import os

        # Configs
        self.json = json
        self.os = os
        self.my_path = self.os.path.dirname(os.path.realpath(__file__))
        self._database = self.my_path + self.os.sep + 'LHCb.db'

        # Initiate a connection, cursor object and turn on the constraints to protect the schema (created in initDB.py)
        self.db_connect = create_engine('sqlite:///' + self._database)
        self.conn = self.db_connect.connect()
        self.conn.execute("PRAGMA foreign_keys = ON")

    # =====================================================================================================================
    #                              Tasks
    # =====================================================================================================================

    def addTask(self, task, utgid='', command='', task_parameters='', command_parameters='', description=''):

        try:
            self.conn.execute("insert into Tasks values ('{0}','{1}','{2}','{3}','{4}','{5}')".format(task, utgid, 
                            command, task_parameters, command_parameters, description))
            return 'Success'

        except Exception as e: return e

    # ---------------------------------------------------------------------------------------------------------------------

    def deleteTask(self, task):
        
        query = self.conn.execute("delete from Tasks where task='{0}'".format(task))
        if(query.rowcount >= 1):
            return 'Success'

        return 'deleteTask: Unknown task with name: ' + task

    # ---------------------------------------------------------------------------------------------------------------------

    def modifyTask(self, original_task, **args):

        if len(args) == 0:
            return 'No arguments provided, modification impossible'

        statement = "update Tasks set "
        cnt=0
        for key in args:
            if(cnt != 0):
                statement += ", "
            statement += (key + "='" + args[key] + "'")
            cnt += 1

        statement += (" where task='" + original_task + "'")
        
        query = self.conn.execute(statement)
        if(query.rowcount >= 1):
            return 'Success'

        return 'modifyTask: Unknown task with name: ' + original_task

    # ---------------------------------------------------------------------------------------------------------------------

    def getTask(self, task):

        query = self.conn.execute("select * from Tasks where task='{0}'".format(task,))
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        if(len(result['data']) == 0):
            return 'getTask: Unknown task with name: ' + task
        
        return self.json.dumps(result)

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

        query = self.conn.execute("delete from Tasks_to_Task_Sets where task='{0}' and task_set='{1}'".format(task, 
                        task_set))

        if(query.rowcount >= 1):
            return 'Success'

        return 'unassignTask: The specified assignment pair does not exist in the database: ' + task + ' <-> ' + task_set

    # =====================================================================================================================
    #                            Task Sets
    # =====================================================================================================================

    def addSet(self, task_set, description=''):

        try:
            self.conn.execute("insert into Task_Sets values ('{0}','{1}')".format(task_set, description))
            return 'Success'

        except Exception as e: return e

    # ---------------------------------------------------------------------------------------------------------------------

    def deleteSet(self, task_set):

        query = self.conn.execute("delete from Task_Sets where task_set='{0}'".format(task_set,))
        if(query.rowcount >= 1):
            return 'Success'

        return 'deleteSet: Unknown task set with name: ' + task_set

    # ---------------------------------------------------------------------------------------------------------------------

    def modifySet(self, original_task_set, **args):

        if len(args) == 0:
            return 'No arguments provided, modification impossible'

        statement = "update Task_Sets set "
        cnt=0
        for key in args:
            if(cnt != 0):
                statement += ", "
            statement += (key + "='" + args[key] + "'")
            cnt += 1

        statement += (" where task_set='" + original_task_set + "'")
        
        query = self.conn.execute(statement)
        if(query.rowcount >= 1):
            return 'Success'

        return 'modifySet: Unknown task set with name: ' + original_task_set

    # ---------------------------------------------------------------------------------------------------------------------

    def getSet(self, task_set):

        query = self.conn.execute("select * from Task_Sets where task_set='{0}'".format(task_set,))
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        if(len(result['data']) == 0):
            return 'getSet: Unknown task set with name: ' + task_set
        
        return self.json.dumps(result)
    
    # ---------------------------------------------------------------------------------------------------------------------

    def assignSet(self, task_set, node_class):

        try:
            self.conn.execute("insert into Task_Sets_to_Classes values ('{0}','{1}')".format(task_set, node_class))
            return 'Success'

        except Exception as e: return e

    # ---------------------------------------------------------------------------------------------------------------------

    def unassignSet(self, task_set, node_class):

        query = self.conn.execute("delete from Task_Sets_to_Classes where task_set='{0}' and node_class='{1}'".format(task_set, 
                        node_class))

        if(query.rowcount >= 1):
            return 'Success'

        return 'unassignSet: The specified assignment pair does not exist in the database: ' + task_set + ' <-> ' + node_class

    # =====================================================================================================================
    #                           Node Classes
    # =====================================================================================================================

    def addClass(self, node_class, description=''):

        try:
            self.conn.execute("insert into Classes values ('{0}','{1}')".format(node_class, description))
            return 'Success'

        except Exception as e: return e

    # ---------------------------------------------------------------------------------------------------------------------

    def deleteClass(self, node_class):

        query = self.conn.execute("delete from Classes where node_class='{0}'".format(node_class,))
        if(query.rowcount >= 1):
            return 'Success'

        return 'deleteClass: Unknown node class with name: ' + node_class

    # ---------------------------------------------------------------------------------------------------------------------
  
    def modifyClass(self, original_node_class, **args):

        if len(args) == 0:
            return 'No arguments provided, modification impossible'

        statement = "update Classes set "
        cnt=0
        for key in args:
            if(cnt != 0):
                statement += ", "
            statement += (key + "='" + args[key] + "'")
            cnt += 1

        statement += (" where node_class='" + original_node_class + "'")
        
        query = self.conn.execute(statement)
        if(query.rowcount >= 1):
            return 'Success'

        return 'modifyClass: Unknown node class with name: ' + original_node_class

    # ---------------------------------------------------------------------------------------------------------------------
    
    def getClass(self, node_class):

        query = self.conn.execute("select * from Classes where node_class='{0}'".format(node_class,))
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        if(len(result['data']) == 0):
            return 'getClass: Unknown node class with name: ' + node_class
        
        return self.json.dumps(result)
    
    # ---------------------------------------------------------------------------------------------------------------------
    
    def assignClass(self, node_class, node_regex):

        try:
            self.conn.execute("insert into Classes_to_Nodes values ('{0}', '{1}')".format(node_class, node_regex))
            return 'Success'
        
        except Exception as e: return e

    # ---------------------------------------------------------------------------------------------------------------------

    def unassignClass(self, node_class, node_regex):

        query = self.conn.execute("delete from Classes_to_Nodes where node_class='{0}' and regex='{1}'".format(node_class, 
                        node_regex))

        if(query.rowcount >= 1):
            return 'Success'

        return 'unassignClass: The specified assignment pair does not exist in the database: ' + node_class + ' <-> ' + node_regex

    # =====================================================================================================================
    #                              Nodes
    # =====================================================================================================================

    def addNode(self, regex, description=''):

        try:
            self.conn.execute("insert into Nodes values ('{0}','{1}')".format(regex, description))
            return 'Success'

        except Exception as e: return e
    
    # ---------------------------------------------------------------------------------------------------------------------
    
    def deleteNode(self, regex):

        query = self.conn.execute("delete from Nodes where regex='{0}'".format(regex,))
        if(query.rowcount >= 1):
            return 'Success'

        return 'deleteNode: Unknown nodes with regex: ' + regex
    
    # ---------------------------------------------------------------------------------------------------------------------
    
    def modifyNode(self, original_regex, **args):

        if len(args) == 0:
            return 'No arguments provided, modification impossible'

        statement = "update Nodes set "
        cnt=0
        for key in args:
            if(cnt != 0):
                statement += ", "
            statement += (key + "='" + args[key] + "'")
            cnt += 1

        statement += (" where regex='" + original_regex + "'")
        
        query = self.conn.execute(statement)
        if(query.rowcount >= 1):
            return 'Success'

        return 'modifyNode: Unknown nodes with regex: ' + original_regex
    
    # ---------------------------------------------------------------------------------------------------------------------
    
    def getNode(self, regex):

        query = self.conn.execute("select * from Nodes where regex='{0}'".format(regex,))
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        if(len(result['data']) == 0):
            return 'getNode: Unknown node with regex: ' + regex
        
        return self.json.dumps(result)

    # =====================================================================================================================
    #                          Helper Methods
    # =====================================================================================================================

    # def inDb(self, table, column, value):

    #     arr = []
    #     result = self.conn.execute('SELECT ' + column + ' FROM ' + table + ' WHERE ' + column + '="' + value + '\"')
    #     for row in result:
    #         arr.append(row)
    #     if len(arr) == 0:
    #         return False
    #     else:
    #         return True

    # More advanced version avaliable in main api tester script, under /6_api_testers/
