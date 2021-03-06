#!/usr/bin/env python




# =========================================================================================================================
#                      LHCb Online Farm Process Explorer 
#                  Command line interface class using Main API
#                           
#                            K.Wilczynski 08.2018
# =========================================================================================================================




class cliClient():

    # =====================================================================================================================
    #                           Constructor
    # =====================================================================================================================

    def __init__(self):

        # Imports
        import mainAPI
        import os
        import json
        import sys

        # Setup
        self.api = mainAPI.mainAPI()
        self.json = json
        self.os = os
        self.sys = sys
        #self._scripts = 'group/online/dataflow/scripts' + self.os.sep

    # =====================================================================================================================
    #                              TASKS
    # =====================================================================================================================

    def addTask(self, task, utgid = '', script = '', script_parameters = '', pcAdd_parameters = '', description = 'The description has not been specified.'):

        if (utgid == ''):
            if (self.yes_or_no('The required parameter - utgid of the task has not been specified, do you want to do this now?', 'yes')):
                utgid = raw_input('Please provide the utgid for your task: \n')
                print 'Success.' + self.os.linesep
        if (script == ''):
            if (self.yes_or_no('The required parameter - script name of the task has not been specified, do you want to do this now?', 'yes')):
                script = raw_input('Please provide the script name for your task: \n')
                print 'Success.' + self.os.linesep
        if(description == 'The description has not been specified.'):
            if(self.yes_or_no('The description of the task has not been specified, do you want to do this now?', 'no')):
                description = raw_input('Please provide the description for your task: \n')
                print 'Success.' + self.os.linesep

        if (script_parameters == ''):
            if (self.yes_or_no('The parameters string for the task has not been specified, do you want to do this now?', 'no')):
                script_parameters = raw_input('Please provide the parameters for your script in the line below: \n')
                print 'Success.' + self.os.linesep

        if (pcAdd_parameters == ''):
            if (self.yes_or_no('No special parameters for the pcAdd command has been specified, do you want to do this now?', 'no')):
                pcAdd_parameters = raw_input('Please provide special parameters in the line below: \n')
                print 'Success.' + self.os.linesep

        try:
            result = self.api.addTask(task, utgid=utgid, script=script, script_parameters, pcAdd_parameters, description)
            print result

        except Exception as e: return e

    # ---------------------------------------------------------------------------------------------------------------------

    def deleteTask(self, task):
      
        try:
            result = self.api.deleteTask(task)
            print result

        except Exception as e: return e

    # ---------------------------------------------------------------------------------------------------------------------

    # def modifyTask(self, task):

    #     if (not self.api.inDb('Tasks', 'task', task)):
    #         print 'The specified task could not be found in the database. Breaking.'
    #         return

    #     try:
    #         task_string = self.api.getTask(task)
    #         task_object = self.json.loads(task_string)

    #         # Modify to properly display using task_object
    #         print "The task you want to modify:" + self.os.linesep
    #         print self.json.dumps(task_json)
    #         print self.os.linesep
    #     except Exception as e: return e

    #     if self.yes_or_no('Do you want to modify the name of the task?', 'no'):
    #         mod_task = raw_input("Please specify a new name (must be unique!): ") # Modify task as user requested
    #     else:
    #         mod_task = task_object['data'][0]['task']                             # Or leave it as is

    #     if self.yes_or_no('Do you want to modify the utgid of the task?', 'no'):
    #         mod_utgid = raw_input("Please specify a new utgid: ")
    #     else:
    #         mod_utgid = task_object['data'][0]['utgid']

    #     if self.yes_or_no('Do you want to modify the script name of the task?', 'no'):
    #         mod_command = raw_input("Please specify a new script name: ")
    #     else:
    #         mod_command = task_object['data'][0]['command']

    #     if self.yes_or_no('Do you want to modify the parameters string of the task?', 'no'):
    #         mod_task_parameters = raw_input("Please specify a new parameters string: ")
    #     else:
    #         mod_task = task_object['data'][0]['task']

    #     if self.yes_or_no('Do you want to modify the additional pcAdd parameters associated with the task?', 'no'):
    #         mod_command_parameters = raw_input("Please specify a new pcAdd parameters string: ")

    #     if self.yes_or_no('Do you want to modify the description of the task?', 'no'):
    #         mod_description = raw_input("Please specify a new description: ")

    #     try:
    #         result = self.api.modifyTask(task, mod)
    #         print result

    #     except Exception as e: return e





    #         print self.os.linesep + 'The task has modified.' + self.os.linesep

    #     except sqlite3.Error as e:
    #         handleDbError(e)

    # # ---------------------------------------------------------------------------------------------------------------------

    # def selectTask(name):
    #     c = connectDb()

    #     if not existsInDb(c, name, 'task', 'Tasks'):
    #         print 'The specified task does not exist in the database.'

    #     try:
    #         c.execute(''.join([

    #             'SELECT Tasks.task, Tasks.utgid, Tasks.command, Tasks.task_parameters, Tasks.command_parameters, Tasks.description ',
    #             'FROM Tasks ',
    #             'WHERE Tasks.task="', name, '\"',

    #         ]))

    #         i = 0

    #         for row in c:
    #             if i == 0:
    #                 FORMAT = '%-16s%-16s%-16s%-16s%-20s%-40s'
    #                 print FORMAT % ('task', 'utgid', 'command', 'parameters', 'pcAdd parameters', 'description')
    #                 print '-' * 124
    #             print FORMAT % row
    #             i += 1
    #         print self.os.linesep



    #     except sqlite3.Error as e:
    #         handleDbError(e)

    # # =====================================================================================================================
    # #                             TASK SETS
    # # =====================================================================================================================

    # def addTaskSet(task_set, description = 'The description has not been specified.'):
    #     c = connectDb()

    #     try:
    #         nexttask = 1
    #         if self.yes_or_no('Do you want to add tasks to this new task set?', 'yes'):
    #             while nexttask == 1:
    #                 addtask = raw_input('Specify the name of the task: ')
    #                 # Check if the task exists in the Tasks table, if it does, add it
    #                 c.execute('SELECT Tasks.task FROM Tasks WHERE task="' + addtask + '\"')
    #                 data = c.fetchall()
    #                 if len(data) == 0:
    #                     if self.yes_or_no("The specified task doesn't exist in the database, do you want to define it now?", 'yes'):
    #                         print 'Use addtask(task) command to add a new task.'
    #                     else:
    #                         print "The non existant task is not will not be added to the set." + self.os.linesep
    #                         if self.yes_or_no('Do you want to add another task?', 'no'):
    #                             nexttask = 1
    #                         else:
    #                             nexttask = 0
    #                 # The task exists in the Tasks table, so it can be added to a set safely
    #                 else:
    #                     c.execute("PRAGMA foreign_keys = OFF")
    #                     c.execute('insert into Tasks_to_Task_Sets values (?,?)', (addtask, task_set))
    #                     c.execute("PRAGMA foreign_keys = ON")
    #                     if self.yes_or_no('Do you want to add another task?', 'no'):
    #                         nexttask = 1
    #                     else:
    #                         nexttask = 0
    #         else:
    #             # No task exists and no task has been added - only then the nexttask variable remains set to 1
    #             if nexttask == 1:
    #                 print "Can't create an empty task set, breaking." + self.os.linesep
    #                 return

    #         if description == 'The description has not been specified.':
    #             if self.yes_or_no('The description of the task has not been specified, do you want to do this now?', 'no'):
    #                 description = raw_input('Please provide the description for your task: \n')
    #                 print 'Success.' + self.os.linesep

    #         SET = (task_set, description)
    #         c.execute("PRAGMA foreign_keys = OFF")
    #         c.execute('insert into Task_Sets values (?,?)', SET)
    #         c.execute("PRAGMA foreign_keys = ON")
    #         print 'The task set has been added to the database.' + self.os.linesep

    #     except sqlite3.Error as e:
    #         handleDbError(e)

    # # ---------------------------------------------------------------------------------------------------------------------

    # def deleteTaskSet(task_set):
    #     c = connectDb()

    #     # TODO: check if the task set is assigned to a node class, prompt the user if he is sure, if yes, delete the binding (handled by db constraints for now - should be safe anyway)

    #     try:
    #         c.execute("PRAGMA foreign_keys = OFF")
    #         c.execute(''.join([

    #             'DELETE ',
    #             'FROM Task_Sets ',
    #             'WHERE task_set="', task_set, '\"',

    #         ]))
    #         c.execute(''.join([

    #             'DELETE ',
    #             'FROM Tasks_to_Task_Sets ',
    #             'WHERE task_set="', task_set, '\"',

    #         ]))
    #         c.execute("PRAGMA foreign_keys = ON")
    #         print 'The task_set has been deleted from the database.' + self.os.linesep

    #     except sqlite3.Error as e:
    #         handleDbError(e)

    # # ---------------------------------------------------------------------------------------------------------------------

    # def modifyTaskSet(task_set):
    #     c = connectDb()

    #     # Do nothing if there is no such task
    #     if not existsInDb(c, task_set, 'task_set', 'Task_Sets'):
    #         print 'The specified task could not be found in the database.' + self.os.linesep
    #         return

    #     if self.yes_or_no('Do you want to modify the tasks assigned to the specified task set?', 'yes'):
    #         try:
    #             c.execute(''.join([

    #                 'SELECT DISTINCT task ',
    #                 'FROM Tasks_to_Task_Sets ',
    #                 'WHERE task_set="', task_set, '\" ',
    #                 'ORDER BY task ASC ',

    #             ]))
    #             i = 0
    #             for row in c:
    #                 if i == 0:
    #                     print "The task set consists of the following tasks:" + self.os.linesep
    #                     FORMAT = '%-16s'
    #                     print FORMAT % ('task')
    #                     print '-' * 20
    #                 print FORMAT % row
    #                 i += 1
    #             print self.os.linesep
    #         except sqlite3.Error as e:
    #             handleDbError(e)

    #         if self.yes_or_no('Do you want to add tasks to the task set?', 'no'):
    #             iaddtask = 1
    #             while iaddtask == 1:
    #                 addtask = raw_input('Please specify a name of the task you want to add: ')
    #                 if existsInDb(c, addtask, 'task', 'Tasks') and not existsInDbPair(c, addtask, 'task', task_set, 'task_set', 'Tasks_to_Task_Sets'):
    #                     try:
    #                         c.execute("PRAGMA foreign_keys = OFF")
    #                         c.execute('insert into Tasks_to_Task_Sets values (?,?)', (addtask, task_set))
    #                         c.execute("PRAGMA foreign_keys = ON")
    #                         print 'The task has been added to the task set.' + self.os.linesep
    #                     except sqlite3.Error as e:
    #                         handleDbError(e)
    #                 else:
    #                     print 'The task is not defined or already exists in this task set, please use the function addTask() to add it to the database and selectTaskSet() to check if it is not already there.' + self.os.linesep
    #                 if not self.yes_or_no('Do you want to add another task?', 'no'):
    #                     iaddtask = 0


    #         if self.yes_or_no('Do you want to delete tasks from the task set?', 'no'):
    #             iremtask = 1
    #             while iremtask == 1:
    #                 remtask = raw_input('Please specify a name of the task you want to remove: ')
    #                 if existsInDbPair(c, remtask, 'task', task_set, 'task_set', 'Tasks_to_Task_Sets'):
    #                     try:
    #                         c.execute("PRAGMA foreign_keys = OFF")
    #                         c.execute('delete from Tasks_to_Task_Sets where task="' + remtask + '\" AND task_set="' + task_set + '\"')
    #                         c.execute("PRAGMA foreign_keys = ON")
    #                         print 'The task has been removed from the task set.' + self.os.linesep
    #                     except sqlite3.Error as e:
    #                         handleDbError(e)
    #                 else:
    #                     print 'The specified task could not be found in the database, please check your spelling.' + self.os.linesep
    #                 if not self.yes_or_no('Do you want to remove another task?', 'no'):
    #                     iremtask = 0


    #     if self.yes_or_no('Do you want to modify the task set\'s description?', 'no'):
    #             mod_description = raw_input('Please specify the new description for the tas set: ')
    #             try:
    #                 c.execute(''.join([

    #                     'UPDATE Task_Sets ',
    #                     'SET ',
    #                     'description = "', mod_description, '\" ',
    #                     'WHERE Task_Sets.task_set = "', task_set, '\"',

    #                 ]))
    #             except sqlite3.Error as e:
    #                 handleDbError(e)

    #     if self.yes_or_no('Do you want to modify the task set\'s name?', 'no'):
    #             mod_task_set = raw_input('Please specify the new name for the task set: ')
    #             try:
    #                 c.execute("PRAGMA foreign_keys = OFF")
    #                 c.execute(''.join([

    #                     'UPDATE Task_Sets ',
    #                     'SET ',
    #                     'task_set = "', mod_task_set, '\" ',
    #                     'WHERE Task_Sets.task_set = "', task_set, '\"',

    #                 ]))

    #                 c.execute(''.join([

    #                     'UPDATE Tasks_to_Task_Sets ',
    #                     'SET ',
    #                     'task_set = "', mod_task_set, '\" ',
    #                     'WHERE Tasks_to_Task_Sets.task_set = "', task_set, '\"',

    #                 ]))

    #                 c.execute(''.join([

    #                     'UPDATE Task_Sets_to_Classes ',
    #                     'SET ',
    #                     'task_set = "', mod_task_set, '\" ',
    #                     'WHERE Classes.task_set = "', task_set, '\"',

    #                 ]))
    #                 c.execute("PRAGMA foreign_keys = ON")
    #                 print 'The task set has been updated.' + self.os.linesep
    #             except sqlite3.Error as e:
    #                 handleDbError(e)


    # # ---------------------------------------------------------------------------------------------------------------------

    # def selectTaskSet(task_set):
    #     c = connectDb()

    #     if not existsInDb(c, task_set, 'task_set', 'Task_Sets'):
    #         print 'The specified task set could not be found in the database. Breaking.' + self.os.linesep
    #         return

    #     # Display the task set info:
    #     try:
    #         c.execute(''.join([

    #             'SELECT Task_Sets.task_set, Task_Sets.description ',
    #             'FROM Task_Sets ',
    #             'WHERE Task_Sets.task_set="', task_set, '\"',

    #         ]))


    #         i = 0
    #         for row in c:
    #             if i == 0:
    #                 FORMAT = '%-16s%-40s'
    #                 print FORMAT % ('task set', 'description')
    #                 print '-' * 56
    #             print FORMAT % row
    #             i += 1
    #         print self.os.linesep

    #         # Display the tasks in the set
    #         # Thats super cool because it wont display non-existant tasks (although there should not be those in the set)
    #         c.execute(''.join([

    #             'SELECT DISTINCT Tasks.* ',
    #             'FROM Tasks ',
    #             'INNER JOIN Tasks_to_Task_Sets ON Tasks_to_Task_Sets.task = Tasks.task ',
    #             'WHERE Tasks_to_Task_Sets.task_set="', task_set, '\"',

    #         ]))

    #         i = 0
    #         for row in c:
    #             if i == 0:
    #                 FORMAT = '%-16s%-16s%-16s%-16s%-20s%-40s'
    #                 print FORMAT % ('task', 'utgid', 'command', 'parameters', 'pcAdd parameters', 'description')
    #                 print '-' * 124
    #             print FORMAT % row
    #             i += 1
    #         print self.os.linesep

    #     except sqlite3.Error as e:
    #         handleDbError(e)

    # # =====================================================================================================================
    # #                           NODE CLASSES
    # # =====================================================================================================================

    # def addClass(node_class, description = 'The description has not been specified.'):
    #     c = connectDb()

    #     if existsInDb(c, node_class, 'class', 'Classes'):
    #         print 'A node class using specified name already exists in the database. Please use another name.' + self.os.linesep
    #         return

    #     if self.yes_or_no('Do you want to add task sets to this new node class?', 'yes'):
    #         nextclass = 1
    #         addedclasses = 0
    #         while nextclass == 1:
    #             addset = raw_input('Specify the name of the task set: ')
    #             # Check if the task exists in the Tasks table, if it does, add it
    #             if existsInDb(c, addset, 'task_set', 'Task_Sets'):
    #                 # The task exists in the Task_Sets table, so it can be added to a class safely
    #                 try:
    #                     c.execute("PRAGMA foreign_keys = OFF")
    #                     c.execute('insert into Task_Sets_to_Classes values (?,?)', (addset, node_class))
    #                     c.execute("PRAGMA foreign_keys = ON")
    #                     addedclasses += 1
    #                     print 'Node class added succesfully.' + self.os.linesep
    #                 except sqlite3.Error as e:
    #                     handleDbError(e)
    #             else:
    #                 if not self.yes_or_no("The specified task set doesn't exist in the database, do you want to add another task set?"):
    #                     nextclass = 0
    #                     if addedclasses == 0:
    #                         print 'It is impossible to define a node class with no task sets assigned to it, breaking.' + self.os.linesep
    #                         break

    #             if self.yes_or_no('Do you want to add another task?', 'no'):
    #                 nextclass = 1
    #             else:
    #                 nextclass = 0


    #     if description == 'The description has not been specified.':
    #         if self.yes_or_no('The description of the node class has not been specified, do you want to do this now?', 'no'):
    #             description = raw_input('Please provide the description for your node class: \n')
    #             print 'Success.' + self.os.linesep

    #     CLASS = (node_class, description)
    #     c.execute("PRAGMA foreign_keys = OFF")
    #     c.execute('insert into Classes values (?,?)', CLASS)
    #     c.execute("PRAGMA foreign_keys = ON")
    #     print 'The node class has been added to the database.' + self.os.linesep



    # # ---------------------------------------------------------------------------------------------------------------------

    # def deleteClass(node_class):
    #     c = connectDb()

    #     if not existsInDb(c, node_class, 'class', 'Classes'):
    #         print 'The specified node class could not be found in the database, breaking.'
    #         return

    #     try:
    #         c.execute("PRAGMA foreign_keys = OFF")
    #         c.execute(''.join([

    #             'DELETE ',
    #             'FROM Classes ',
    #             'WHERE class = "', node_class, '\"',

    #         ]))

    #         c.execute(''.join([

    #             'DELETE ',
    #             'FROM Task_Sets_to_Classes ',
    #             'WHERE class = "', node_class, '\"',

    #         ]))
    #         c.execute("PRAGMA foreign_keys = ON")
    #         print 'The node class have been deleted from the database.' + self.os.linesep

    #     except sqlite3.Error as e:
    #         handleDbError(e)

    # # ---------------------------------------------------------------------------------------------------------------------

    # def modifyClass(node_class):
    #     c = connectDb()

    #     # Do nothing if there is no such class
    #     if not existsInDb(c, node_class, 'class', 'Classes'):
    #         print 'The specified node class could not be found in the database.' + self.os.linesep
    #         return

    #     if self.yes_or_no('Do you want to modify the task sets assigned to the specified node class?', 'yes'):
    #         try:
    #             c.execute(''.join([

    #                 'SELECT DISTINCT task_set ',
    #                 'FROM Task_Sets_to_Classes ',
    #                 'WHERE class="', node_class, '\" ',
    #                 'ORDER BY task_set ASC ',

    #             ]))
    #             i = 0
    #             for row in c:
    #                 if i == 0:
    #                     print "The node class consists of the following task sets:" + self.os.linesep
    #                     FORMAT = '%-16s'
    #                     print FORMAT % ('task set')
    #                     print '-' * 20
    #                 print FORMAT % row
    #                 i += 1
    #             print self.os.linesep
    #         except sqlite3.Error as e:
    #             handleDbError(e)

    #         if self.yes_or_no('Do you want to add task sets to the node class?', 'no'):
    #             iaddset = 1
    #             while iaddset == 1:
    #                 addset = raw_input('Please specify a name of the task set you want to add: ')
    #                 if existsInDb(c, addset, 'task_set', 'Task_Sets') and not existsInDbPair(c, addset, 'task_set', node_class, 'class', 'Task_Sets_to_Classes'):
    #                     try:
    #                         c.execute("PRAGMA foreign_keys = OFF")
    #                         c.execute('insert into Task_Sets_to_Classes values (?,?)', (addset, node_class))
    #                         c.execute("PRAGMA foreign_keys = ON")
    #                         print 'The task set has been added to the node class.' + self.os.linesep
    #                     except sqlite3.Error as e:
    #                         handleDbError(e)
    #                 else:
    #                     print 'The task set is not defined or already exists in this node class, please use the function addTaskSet() to add it to the database and selectClass() to check if it is not already there.' + self.os.linesep
    #                 if not self.yes_or_no('Do you want to add another task set?', 'no'):
    #                     iaddtask = 0

    #         if self.yes_or_no('Do you want to delete task sets from the node class?', 'no'):
    #             iremset = 1
    #             while iremset == 1:
    #                 remset = raw_input('Please specify a name of the task set you want to remove: ')
    #                 if existsInDbPair(c, remset, 'task_set', node_class, 'class', 'Task_Sets_to_Classes'):
    #                     try:
    #                         c.execute("PRAGMA foreign_keys = OFF")
    #                         c.execute('delete from Task_Sets_to_Classes where task_set="' + remset + '\" AND class="' + node_class + '\"')
    #                         c.execute("PRAGMA foreign_keys = ON")
    #                         print 'The task set has been removed from the node class.' + self.os.linesep
    #                     except sqlite3.Error as e:
    #                         handleDbError(e)
    #                 else:
    #                     print 'The specified task set could not be found in the database, please check your spelling.' + self.os.linesep
    #                 if not self.yes_or_no('Do you want to remove another task set?', 'no'):
    #                     iremset = 0

    #     if self.yes_or_no('Do you want to modify the node class description?', 'no'):
    #         mod_description = raw_input('Please specify the new description for the node class: ')
    #         try:
    #             c.execute(''.join([

    #                 'UPDATE Classes ',
    #                 'SET ',
    #                 'description = "', mod_description, '\" ',
    #                 'WHERE Task_Sets_to_Classes = "', node_class, '\"',

    #             ]))
    #         except sqlite3.Error as e:
    #             handleDbError(e)

    #     if self.yes_or_no('Do you want to modify the node class name?', 'no'):
    #         mod_node_class = raw_input('Please specify the new name for the node class: ')
    #         try:
    #             c.execute("PRAGMA foreign_keys = OFF")
    #             c.execute(''.join([

    #                 'UPDATE Classes ',
    #                 'SET ',
    #                 'class = "', mod_node_class, '\" ',
    #                 'WHERE Classes.class = "', node_class, '\"',

    #             ]))

    #             c.execute(''.join([

    #                 'UPDATE Task_Sets_to_Classes ',
    #                 'SET ',
    #                 'class = "', mod_node_class, '\" ',
    #                 'WHERE Task_Sets_to_Classes.class = "', node_class, '\"',

    #             ]))
    #             c.execute("PRAGMA foreign_keys = ON")
    #             print 'The node class has been updated.' + self.os.linesep
    #         except sqlite3.Error as e:
    #             handleDbError(e)

    # # ---------------------------------------------------------------------------------------------------------------------

    # def selectClass(node_class):
    #     c = connectDb()

    #     if not existsInDb(c, node_class, 'class', 'Classes'):
    #         print 'The specified node class could not be found in the database. Breaking.' + self.os.linesep
    #         return

    #     # Display the node class info:
    #     try:
    #         c.execute(''.join([

    #             'SELECT class, description ',
    #             'FROM Classes ',
    #             'WHERE Classes.class = "', node_class, '\"',

    #         ]))

    #         i = 0
    #         for row in c:
    #             if i == 0:
    #                 FORMAT = '%-16s%-40s'
    #                 print FORMAT % ('node class', 'description')
    #                 print '-' * 56
    #             print FORMAT % row
    #             i += 1
    #         print self.os.linesep


    #         c.execute(''.join([

    #             'SELECT DISTINCT Task_Sets.* ',
    #             'FROM Task_Sets ',
    #             'INNER JOIN Task_Sets_to_Classes ON Task_Sets_to_Classes.task_set = Task_Sets.task_set ',
    #             'WHERE Task_Sets_to_Classes.class = "', node_class, '\"',

    #         ]))

    #         # Display task sets in the node class

    #         print 'The node class has the following task sets assigned to it:' + self.os.linesep
    #         i = 0
    #         for row in c:
    #             if i == 0:
    #                 FORMAT = '%-16s%-40s'
    #                 print FORMAT % ('task_set', 'description')
    #                 print '-' * 124
    #             print FORMAT % row
    #             i += 1
    #         print self.os.linesep

    #     except sqlite3.Error as e:
    #         handleDbError(e)

    # # =====================================================================================================================
    # #                              NODES
    # # =====================================================================================================================

    # def addNodes(regex):
    #     c = connectDb()

    #     if existsInDb(c, regex, 'regex', 'Nodes'):
    #         print 'Nodes using this regular expression already exist in the database, please consider modifying them with modifyNodes() or use another name.' + self.os.linesep
    #         return


    #     nextclass = 1
    #     addedclasses = 0
    #     if self.yes_or_no('Do you want to add node classes associated with the specified nodes regular expression?', 'yes'):
    #         while nextclass == 1:
    #             addclass = raw_input('Specify the name of the node class: ')
    #             # Check if the class exists in the class, if it does, add it
    #             if existsInDb(c, addclass, 'class', 'Classes'):
    #                 # The class exists in the Classes table, so it can be added to a nodes safely
    #                 try:
    #                     c.execute("PRAGMA foreign_keys = OFF")
    #                     c.execute('insert into Nodes values (?,?)', (regex, addclass))
    #                     c.execute("PRAGMA foreign_keys = ON")
    #                     addedclasses += 1
    #                     print 'Class associated succesfully.' + self.os.linesep
    #                 except sqlite3.Error as e:
    #                     handleDbError(e)
    #             else:
    #                 print 'The specified node class does not exist in the database' + self.os.linesep

    #             if self.yes_or_no('Do you want to associate another node class to the specified regular expression?', 'no'):
    #                 nextclass = 1
    #             else:
    #                 nextclass = 0

    #     if addedclasses == 0:
    #         print 'It is impossible to define a regular expression without assigning any node class to it, breaking.' + self.os.linesep
    #         return
    #     else:
    #         print 'The node regex and associated classes have been added to the database.' + self.os.linesep

    # # ---------------------------------------------------------------------------------------------------------------------

    # def deleteNodes(regex):
    #     c = connectDb()

    #     try:
    #         c.execute("PRAGMA foreign_keys = OFF")
    #         c.execute(''.join([

    #             'DELETE ',
    #             'FROM Nodes ',
    #             'WHERE regex="', regex, '\"',

    #         ]))
    #         c.execute("PRAGMA foreign_keys = ON")
    #         print 'The records of classes assigned to the specified regular expression have been deleted from the database.' + self.os.linesep

    #     except sqlite3.Error as e:
    #         handleDbError(e)

    # # ---------------------------------------------------------------------------------------------------------------------

    # def modifyNodes(regex):
    #     c = connectDb()

    #     # Do nothing if there are no such nodes
    #     if not existsInDb(c, regex, 'regex', 'Nodes'):
    #         print 'The specified regular expression could not be found in the database.' + self.os.linesep
    #         return

    #     if self.yes_or_no('Do you want to modify the node classes assigned to the specified regular expression?', 'yes'):
    #         try:
    #             c.execute(''.join([

    #                 'SELECT DISTINCT class ',
    #                 'FROM Nodes ',
    #                 'WHERE regex="', regex, '\" ',
    #                 'ORDER BY class ASC ',

    #             ]))
    #             i = 0
    #             for row in c:
    #                 if i == 0:
    #                     print "The specified regular expression is associated with the following node classes:" + self.os.linesep
    #                     FORMAT = '%-16s'
    #                     print FORMAT % ('node class')
    #                     print '-' * 20
    #                 print FORMAT % row
    #                 i += 1
    #             print self.os.linesep
    #         except sqlite3.Error as e:
    #             handleDbError(e)

    #         if self.yes_or_no('Do you want to add node class to the association list?', 'no'):
    #             iaddclass = 1
    #             while iaddclass == 1:
    #                 addclass = raw_input('Please specify a name of the node class you want to add: ')
    #                 if existsInDb(c, addclass, 'class', 'Classes') and not existsInDbPair(c, addclass, 'class', regex, 'regex', 'Nodes'):
    #                     try:
    #                         c.execute("PRAGMA foreign_keys = OFF")
    #                         c.execute('insert into Nodes values (?,?)', (regex, addclass))
    #                         c.execute("PRAGMA foreign_keys = ON")
    #                         print 'The node class has been associated with the specified regular expression' + self.os.linesep
    #                     except sqlite3.Error as e:
    #                         handleDbError(e)
    #                 else:
    #                     print 'The node class is not defined or is already associated with the specified regular expression, please use the function addClass() to add it to the database and selectNodes() to check if it is not already there.' + self.os.linesep
    #                 if not self.yes_or_no('Do you want to associate another node class?', 'no'):
    #                     iaddclass = 0

    #         if self.yes_or_no('Do you want to delete node class from the association list?', 'no'):
    #             iremclass = 1
    #             while iremclass == 1:
    #                 remclass = raw_input('Please specify a name of the node class you want to remove: ')
    #                 if existsInDbPair(c, remclass, 'class', regex, 'regex', 'Nodes'):
    #                     try:
    #                         c.execute("PRAGMA foreign_keys = OFF")
    #                         c.execute(
    #                             'delete from Nodes where regex="' + regex + '\" AND class="' + remclass + '\"')
    #                         c.execute("PRAGMA foreign_keys = ON")
    #                         print 'The node class has been removed from the association list.' + self.os.linesep
    #                     except sqlite3.Error as e:
    #                         handleDbError(e)
    #                 else:
    #                     print 'The specified node class could not be found in the database, please check your spelling.' + self.os.linesep
    #                 if not self.yes_or_no('Do you want to remove another node class?', 'no'):
    #                     iremclass = 0


    #     if self.yes_or_no('Do you want to modify the regular expression (or node name)?', 'no'):
    #         mod_regex = raw_input('Please specify the new regular expression: ')
    #         try:
    #             c.execute("PRAGMA foreign_keys = OFF")
    #             c.execute(''.join([

    #                 'UPDATE Nodes ',
    #                 'SET ',
    #                 'regex = "', mod_regex, '\" ',
    #                 'WHERE Nodes.regex = "', regex, '\"',

    #             ]))
    #             c.execute("PRAGMA foreign_keys = ON")
    #             print 'The regular expression has been updated.' + self.os.linesep
    #         except sqlite3.Error as e:
    #             handleDbError(e)

    # # ---------------------------------------------------------------------------------------------------------------------

    # def selectNodes(regex):
    #     c = connectDb()

    #     if not existsInDb(c, regex, 'regex', 'Nodes'):
    #         print 'The specified regular expression (or node name) could not be found in the database. Please check your spelling.' + self.os.linesep

    #     # Display the nodes info:
    #     try:
    #         c.execute(''.join([

    #             'SELECT DISTINCT regex, class ',
    #             'FROM Nodes ',
    #             'WHERE regex="', regex, '\"',
    #             'ORDER BY regex ASC',

    #         ]))

    #         i = 0
    #         for row in c:
    #             if i == 0:
    #                 FORMAT = '%-30s%-26s'
    #                 print FORMAT % ('regular expression', 'class')
    #                 print '-' * 56
    #             print FORMAT % row
    #             i += 1
    #         print self.os.linesep

    #     except sqlite3.Error as e:
    #         handleDbError(e)

    # =====================================================================================================================
    #                              OTHERS
    # =====================================================================================================================

    # def connectDb():
    #     try:
    #         conn = sqlite3.connect(_database)
    #         c = conn.cursor()
    #         c.execute("PRAGMA foreign_keys = ON")
    #         print 'Connected to the database.' + self.os.linesep
    #         return c
    #     except:
    #         print 'ERROR: Couldn\'t connect to the database.'

    # # ---------------------------------------------------------------------------------------------------------------------

    # def handleDbError(e):
    #     try:
    #         print "Database Error [%d]: %s" % (e.args[0], e.args[1]) + self.os.linesep
    #         return None
    #     except IndexError:
    #         print "Database Error: %s" % str(e) + self.os.linesep
    #         return None
    #     except TypeError, e:
    #         print(e)
    #         return None
    #     except ValueError, e:
    #         print(e)
    #         return None

    # ---------------------------------------------------------------------------------------------------------------------

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

    # ---------------------------------------------------------------------------------------------------------------------

    def LHCb(self):

        # Just a nice easter egg :)

        from pyfiglet import Figlet
        #f = Figlet(font='speed')
        #f = Figlet(font='slant')
        #print(f.renderText('LHCb'))
        print'                                                                          %%%%%%%                  '
        print'                                                                             %%%                   '
        print'                                                                            %%%%                   '
        print'                                                                           .%%%                    '
        print'                                                                           %%%#                    '
        print'         .%%%%%%             %%%%%%      %%%%%%        %%%%      %%%,     %%%%   %%%%%%%%%         '
        print'          %%%%%              %%%%%       %%%%%      %%%%%         %%      %%%  %      .%%%%        '
        print'         (%%%%              ,%%%%       .%%%%      %%%%%          %.     %%%%%         %%%%        '
        print'         %%%%               %%%%.       %%%%,     %%%%%                 %%%%%          %%%%        '
        print'        %%%%%              %%%%%%%%%%%%%%%%%     %%%%%                  %%%%          %%%%         '
        print'        %%%%               %%%%        %%%%      %%%%%                 %%%%          %%%%          '
        print'       %%%%#          %   %%%%#       %%%%%      %%%%%                ,%%%         .%%%            '
        print'      %%%%%         %%   %%%%%       %%%%%        %%%%.        %%     %%%%        %%%              '
        print'     %%%%%%    *%%%%%   %%%%%%      %%%%%%          %%%%%%%%%%         %%%*   %%%                  '
        print'%%%%                 %%        *%%/        /%%%%%%%%%        #%%%%%%%%%%        %%%%%%%%%%%%%%%%%%'
        print'%%%%%%     %%%%%%%%   %%%     %%%%%%%     %%%%%%%%     %%%%%%%%  %%%%%    %%%%%%%   %%%%%%%%%%%%%%%'
        print'%%%%%%%    %%%%%%%%%% %%%%    %%%%%%%%    #%%%%%%     %%%%%%%%%%%%%%%%    %%%%%%%%%    %%%%%%%%%%%%'
        print'%%%%%%%     %%%%%%%%%%%%%%/    %%%%%%%/    %%%%%%     %%%%%%%%%%%%%%%%%    %%%%%%%%%%   #%%%%%%%%%%'
        print'%%%%%%%%    .%%%%%%%%%%%%%%                 %%%%%     %%%%%%%%%%%%%%%%%%   /%%%%%%%%%%    %%%%%%%%%'
        print'%%%%%%%%%    %%%%%%%%%%%%%%%    %%%%%%%%    %%%%%*    (%%%%%%%%%%%%%%%%%    *%%%%%%%%%%    %%%%%%%%'
        print'%%%%%%%%%     %%%%%%%%%%%%%%     %%%%%%%     %%%%%.    %%%%%%%%%%%#%%%%%%   / %%%%%%%%%    %%%%%%%%'
        print'%%%%%%%%%%    %%%%%%%%%%%%%%%     %%%%%%%     %%%%%%     %%%%%%%%%  %%%%%(   % .%%%%%%%    %%%%%%%%'
        print'%%%%%%%%%%     %%%%%%%%%%%%%%     *%%%%%%     /%%%%%%%     %%%%%%.  %%%%%%    %%          %%%%%%%%%'
        print'%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%,   .%%%% %%%%%%   %%%%%%%%%%%%%%%%%%%%%'
        print'%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    %%%%%%%%%%%%%%%%%%%%'
        print'%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    %%%%%%%%%%%%%%%%%%%'
        print'%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%   %%%%%%%%%%%%%%%%%%%'
        print'%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%     %%%%%%%%%%%%%%%%%%'
        print'%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%'
        print'%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%'
        print'%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%'
        f = Figlet(font='smkeyboard')
        print(f.renderText('Online'))

    # ---------------------------------------------------------------------------------------------------------------------

    # def existsInDb(db_connection, variable, column, table):
    #     c = db_connection
    #     c.execute('SELECT ' + column + ' FROM ' + table + ' WHERE ' + column + '="' + variable + '\"')
    #     data = c.fetchall()
    #     if len(data) == 0:
    #         return False
    #     else:
    #         return True

    # # ---------------------------------------------------------------------------------------------------------------------

    # def existsInDbPair(db_connection, variable1, column1, variable2, column2, table):
    #     c = db_connection
    #     c.execute('SELECT * ' + 'FROM ' + table + ' WHERE ' + column1 + '="' + variable1 + '\" AND ' + column2 + '="' + variable2 + '\"')
    #     data = c.fetchall()
    #     if len(data) == 0:
    #         return False
    #     else:
    #         return True

    # ---------------------------------------------------------------------------------------------------------------------
