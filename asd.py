#!/usr/bin/env python

import sqlite3
import os
import sys


# =====================================================================================================================
#                           DEFINITIONS
# =====================================================================================================================

_scripts = 'group/online/dataflow/scripts' + os.sep
_database = '/home/leon/Desktop/Cern_Project/SQL_DB/LHCb.db'


# =====================================================================================================================
#                              TASKS
# =====================================================================================================================

def addTask(name, utgid = '', script = '', script_parameters = '', pcAdd_parameters = '', description = 'The description has not been specified.'):
    c = connectDb()

    if (utgid == ''):
        if (yes_or_no('The required parameter - utgid of the task has not been specified, do you want to do this now?', 'yes')):
            utgid = raw_input('Please provide the utgid for your task: \n')
            print 'Success.' + os.linesep
    if (script == ''):
        if (yes_or_no('The required parameter - script name of the task has not been specified, do you want to do this now?', 'yes')):
            script = raw_input('Please provide the script name for your task: \n')
            print 'Success.' + os.linesep
    if(description == 'The description has not been specified.'):
        if(yes_or_no('The description of the task has not been specified, do you want to do this now?', 'no')):
            description = raw_input('Please provide the description for your task: \n')
            print 'Success.' + os.linesep

    if (script_parameters == ''):
        if (yes_or_no('The parameters string for the task has not been specified, do you want to do this now?', 'no')):
            script_parameters = raw_input('Please provide the parameters for your script in the line below: \n')
            print 'Success.' + os.linesep

    if (pcAdd_parameters == ''):
        if (yes_or_no('No special parameters for the pcAdd command has been specified, do you want to do this now?', 'no')):
            pcAdd_parameters = raw_input('Please provide special parameters in the line below: \n')
            print 'Success.' + os.linesep

    TASK = (name, utgid, script, script_parameters, pcAdd_parameters, description)

    try:
        c.execute('insert into Tasks values (?,?,?,?,?,?)', TASK)
        print 'The task has been added to the database.' + os.linesep
    except sqlite3.Error as e:
        handleDbError(e)
    #finally:
        #conn.commit()
        #conn.close()

# ---------------------------------------------------------------------------------------------------------------------

def deleteTask(name):
    c = connectDb()

    try:
        c.execute(''.join([

            'DELETE ',
            'FROM Tasks ',
            'WHERE Tasks.task="', name, '\"',

        ]))
        print 'The task has been deleted from the database.' + os.linesep
    except sqlite3.Error as e:
        handleDbError(e)

# ---------------------------------------------------------------------------------------------------------------------

def modifyTask(name):
    c = connectDb()

    try:
        c.execute(''.join([

            'SELECT Tasks.task, Tasks.utgid, Tasks.command, Tasks.task_parameters, Tasks.command_parameters, Tasks.description ',
            'FROM Tasks ',
            'WHERE Tasks.task="', name, '\"',

        ]))

        i = 0
        for row in c:
            if i == 0:
                print "The task you want to modify:" + os.linesep
                FORMAT = '%-16s%-16s%-16s%-16s%-20s%-40s'
                print FORMAT % ('task', 'utgid', 'command', 'parameters', 'pcAdd parameters', 'description')
                print '-' * 100
            print FORMAT % row
            i += 1
        print os.linesep

    except sqlite3.Error as e:
        handleDbError(e)

    # Break if the specified task was not found in the database
    if i == 0:
        print 'The specified task was not found in the database, please check your spelling.' + os.linesep
        return

    try:
        # See the properties of Tasks table, then iterate trough the column names and modify the records
        c.execute(''.join([

            'SELECT * ',
            'FROM Tasks ',

        ]))

        # Dummy variable, needed for adressing the database row in SQL statements in the for loop
        mod_task = ''
        colnames = c.description
        for row in colnames:
            if row[0] == 'task':
                if yes_or_no('Do you want to modify the name of the task?', 'no'):
                    mod_task = raw_input("Please specify a new name (must be unique!): ")
                    # First we need to modify the Tasks_to_Task_Sets table and drop the foreign key constraint between the tables
                    c.execute("PRAGMA foreign_keys = OFF")
                    c.execute(''.join([

                        'UPDATE Tasks_to_Task_Sets ',
                        'SET ',
                        'task = "', mod_task, '\" ',
                        'WHERE Tasks_to_Task_Sets.task = "', name, '\"',

                    ]))
                    # Now we can modify the Tasks table and bring back the constraint to prevent deleting tasks assigned to task sets
                    c.execute(''.join([

                        'UPDATE Tasks ',
                        'SET ',
                        'task = "', mod_task, '\" ',
                        'WHERE Tasks.task = "', name, '\"',

                    ]))
                    c.execute("PRAGMA foreign_keys = ON")
            elif row[0] == 'utgid':
                if yes_or_no('Do you want to modify the utgid of the task?', 'no'):
                    mod_utgid = raw_input("Please specify a new utgid: ")
                    c.execute(''.join([

                        'UPDATE Tasks ',
                        'SET ',
                        'utgid = "', mod_utgid, '\" ',
                        'WHERE Tasks.task = "', name, '\"', 'OR Tasks.task = "', mod_task, '\"',

                    ]))
            elif row[0] == 'command':
                if yes_or_no('Do you want to modify the script name of the task?', 'no'):
                    mod_command = raw_input("Please specify a new script name: ")
                    c.execute(''.join([

                        'UPDATE Tasks ',
                        'SET ',
                        'command = "', mod_command, '\" ',
                        'WHERE Tasks.task = "', name, '\"', 'OR Tasks.task = "', mod_task, '\"',

                    ]))
            elif row[0] == 'task_parameters':
                if yes_or_no('Do you want to modify the parameters string of the task?', 'no'):
                    mod_task_parameters = raw_input("Please specify a new parameters string: ")
                    c.execute(''.join([

                        'UPDATE Tasks ',
                        'SET ',
                        'task_parameters = "', mod_task_parameters, '\" ',
                        'WHERE Tasks.task = "', name, '\"', 'OR Tasks.task = "', mod_task, '\"',

                    ]))
            elif row[0] == 'command_parameters':
                if yes_or_no('Do you want to modify the additional pcAdd parameters associated with the task?', 'no'):
                    mod_command_parameters = raw_input("Please specify a new pcAdd parameters string: ")
                    c.execute(''.join([

                        'UPDATE Tasks ',
                        'SET ',
                        'command_parameters = "', mod_command_parameters, '\" ',
                        'WHERE Tasks.task = "', name, '\"', 'OR Tasks.task = "', mod_task, '\"',

                    ]))
            elif row[0] == 'description':
                if yes_or_no('Do you want to modify the description of the task?', 'no'):
                    mod_description = raw_input("Please specify a new description: ")
                    c.execute(''.join([

                        'UPDATE Tasks ',
                        'SET ',
                        'description = "', mod_description, '\" ',
                        'WHERE Tasks.task = "', name, '\"', 'OR Tasks.task = "', mod_task, '\"',

                    ]))

        print os.linesep + 'The task has modified.' + os.linesep

    except sqlite3.Error as e:
        handleDbError(e)

# ---------------------------------------------------------------------------------------------------------------------

def selectTask(name):
    c = connectDb()

    try:
        c.execute(''.join([

            'SELECT Tasks.task, Tasks.utgid, Tasks.command, Tasks.task_parameters, Tasks.command_parameters, Tasks.description ',
            'FROM Tasks ',
            'WHERE Tasks.task="', name, '\"',

        ]))

        data = c.fetchall()
        if len(data) == 0:
            print 'The specified task does not exist in the database.'

        i = 0
        for row in c:
            if i == 0:
                FORMAT = '%-16s%-16s%-16s%-16s%-20s%-40s'
                print FORMAT % ('task', 'utgid', 'command', 'parameters', 'pcAdd parameters', 'description')
                print '-' * 124
            print FORMAT % row
            i += 1
        print os.linesep

    except sqlite3.Error as e:
        handleDbError(e)

# =====================================================================================================================
#                             TASK SETS
# =====================================================================================================================

def addTaskSet(task_set, description = 'The description has not been specified.'):
    c = connectDb()

    try:
        nexttask = 1
        if yes_or_no('Do you want to add tasks to this new task set?', 'yes'):
            while nexttask == 1:
                addtask = raw_input('Specify the name of the task: ')
                # Check if the task exists in the Tasks table, if it does, add it
                c.execute('SELECT Tasks.task FROM Tasks WHERE task="' + addtask + '\"')
                data = c.fetchall()
                if len(data) == 0:
                    if yes_or_no("The specified task doesn't exist in the database, do you want to define it now?", 'yes'):
                        print 'Use addtask(task) command to add a new task.'
                    else:
                        print "The non existant task is not will not be added to the set." + os.linesep
                        if yes_or_no('Do you want to add another task?', 'no'):
                            nexttask = 1
                        else:
                            nexttask = 0
                # The task exists in the Tasks table, so it can be added to a set safely
                else:
                    c.execute("PRAGMA foreign_keys = OFF")
                    c.execute('insert into Tasks_to_Task_Sets values (?,?)', (addtask, task_set))
                    c.execute("PRAGMA foreign_keys = ON")
                    if yes_or_no('Do you want to add another task?', 'no'):
                        nexttask = 1
                    else:
                        nexttask = 0
        else:
            # No task exists and no task has been added - only then the nexttask variable remains set to 1
            if nexttask == 1:
                print "Can't create an empty task set, breaking." + os.linesep
                return

        if description == 'The description has not been specified.':
            if yes_or_no('The description of the task has not been specified, do you want to do this now?', 'no'):
                description = raw_input('Please provide the description for your task: \n')
                print 'Success.' + os.linesep

        SET = (task_set, description)
        c.execute("PRAGMA foreign_keys = OFF")
        c.execute('insert into Task_Sets values (?,?)', SET)
        c.execute("PRAGMA foreign_keys = ON")
        print 'The task set has been added to the database.' + os.linesep

    except sqlite3.Error as e:
        handleDbError(e)

# ---------------------------------------------------------------------------------------------------------------------

def deleteTaskSet(task_set):
    c = connectDb()

    # TODO: check if the task set is assigned to a node class, prompt the user if he is sure, if yes, delete the binding

    try:
        c.execute("PRAGMA foreign_keys = OFF")
        c.execute(''.join([

            'DELETE ',
            'FROM Task_Sets ',
            'WHERE task_set="', task_set, '\"',

        ]))
        c.execute(''.join([

            'DELETE ',
            'FROM Tasks_to_Task_Sets ',
            'WHERE task_set="', task_set, '\"',

        ]))
        c.execute("PRAGMA foreign_keys = ON")
        print 'The task_set has been deleted from the database.' + os.linesep

    except sqlite3.Error as e:
        handleDbError(e)

# ---------------------------------------------------------------------------------------------------------------------

def modifyTaskSet(task_set):
    c = connectDb()

    # Do nothing if there is no such task
    if not existsInDb(c, task_set, 'task_set', 'Task_Sets'):
        print 'The specified task could not be found in the database.' + os.linesep
        return

    if yes_or_no('Do you want to modify the tasks assigned to the specified task set?', 'yes'):
        try:
            c.execute(''.join([

                'SELECT DISTINCT task ',
                'FROM Tasks_to_Task_Sets ',
                'WHERE task_set="', task_set, '\" ',
                'ORDER BY task ASC ',

            ]))
            i = 0
            for row in c:
                if i == 0:
                    print "The task set consists of the following tasks:" + os.linesep
                    FORMAT = '%-16s'
                    print FORMAT % ('task')
                    print '-' * 20
                print FORMAT % row
                i += 1
            print os.linesep
        except sqlite3.Error as e:
            handleDbError(e)

        if yes_or_no('Do you want to add tasks to the task set?', 'no'):
            iaddtask = 1
            while iaddtask == 1:
                addtask = raw_input('Please specify a name of the task you want to add: ')
                if existsInDb(c, addtask, 'task', 'Tasks') and not existsInDbPair(c, addtask, 'task', task_set, 'task_set', 'Tasks_to_Task_Sets'):
                    try:
                        c.execute("PRAGMA foreign_keys = OFF")
                        c.execute('insert into Tasks_to_Task_Sets values (?,?)', (addtask, task_set))
                        c.execute("PRAGMA foreign_keys = ON")
                        print 'The task has been added to the task set.' + os.linesep
                    except sqlite3.Error as e:
                        handleDbError(e)
                else:
                    print 'The task is not defined or already exists in this task set, please use the function addTask() to add it to the database and selectTaskSet() to check if it is not already there.' + os.linesep
                if not yes_or_no('Do you want to add another task?', 'no'):
                    iaddtask = 0


        if yes_or_no('Do you want to delete tasks from the task set?', 'no'):
            iremtask = 1
            while iremtask == 1:
                remtask = raw_input('Please specify a name of the task you want to remove: ')
                if existsInDbPair(c, remtask, 'task', task_set, 'task_set', 'Tasks_to_Task_Sets'):
                    try:
                        c.execute("PRAGMA foreign_keys = OFF")
                        c.execute('delete from Tasks_to_Task_Sets where task="' + remtask + '\" AND task_set="' + task_set + '\"')
                        c.execute("PRAGMA foreign_keys = ON")
                        print 'The task has been removed from the task set.' + os.linesep
                    except sqlite3.Error as e:
                        handleDbError(e)
                else:
                    print 'The specified task could not be found in the database, please check your spelling.' + os.linesep
                if not yes_or_no('Do you want to remove another task?', 'no'):
                    iremtask = 0


    if yes_or_no('Do you want to modify the task set\'s description?', 'no'):
            mod_description = raw_input('Please specify the new description for the tas set: ')
            try:
                c.execute(''.join([

                    'UPDATE Task_Sets ',
                    'SET ',
                    'description = "', mod_description, '\" ',
                    'WHERE Task_Sets.task_set = "', task_set, '\"',

                ]))
            except sqlite3.Error as e:
                handleDbError(e)
    if yes_or_no('Do you want to modify the task set\'s name?', 'no'):
            mod_task_set = raw_input('Please specify the new name for the task set: ')
            try:
                c.execute("PRAGMA foreign_keys = OFF")
                c.execute(''.join([

                    'UPDATE Task_Sets ',
                    'SET ',
                    'task_set = "', mod_task_set, '\" ',
                    'WHERE Task_Sets.task_set = "', task_set, '\"',

                ]))
                c.execute("PRAGMA foreign_keys = ON")
                print 'The task set has been updated.' + os.linesep
            except sqlite3.Error as e:
                handleDbError(e)


# ---------------------------------------------------------------------------------------------------------------------

def selectTaskSet(task_set):
    c = connectDb()

    # Display the task set info:
    try:
        c.execute(''.join([

            'SELECT Task_Sets.task_set, Task_Sets.description ',
            'FROM Task_Sets ',
            'WHERE Task_Sets.task_set="', task_set, '\"',

        ]))

        data = c.fetchall()
        if len(data) == 0:
            print 'The specified task set does not exist in the database.'

        i = 0
        for row in c:
            if i == 0:
                FORMAT = '%-16s%-40s'
                print FORMAT % ('task set', 'description')
                print '-' * 56
            print FORMAT % row
            i += 1
        print os.linesep

        # Display the tasks in the set
        # Thats super cool because it wont display non-existant tasks (although there should not be those in the set)
        c.execute(''.join([

            'SELECT DISTINCT Tasks.* ',
            'FROM Tasks ',
            'INNER JOIN Tasks_to_Task_Sets ON Tasks_to_Task_Sets.task = Tasks.task ',
            'WHERE Tasks_to_Task_Sets.task_set="', task_set, '\"',

        ]))

        i = 0
        for row in c:
            if i == 0:
                FORMAT = '%-16s%-16s%-16s%-16s%-20s%-40s'
                print FORMAT % ('task', 'utgid', 'command', 'parameters', 'pcAdd parameters', 'description')
                print '-' * 124
            print FORMAT % row
            i += 1
        print os.linesep

    except sqlite3.Error as e:
        handleDbError(e)

# =====================================================================================================================
#                           NODE CLASSES
# =====================================================================================================================

def addClass(node_class):
    c = connectDb()

    if existsInDb(c, node_class, 'class', 'Classes')

    if yes_or_no('Do you want to add task sets to this new node class?', 'yes'):
        nextclass = 1
        addedclasses = 0
        while nextclass == 1:
            addset = raw_input('Specify the name of the task set: ')
            # Check if the task exists in the Tasks table, if it does, add it
            if existsInDb(c, addset, 'task_set', 'Task_Sets'):
                # The task exists in the Task_Sets table, so it can be added to a class safely
                try:
                    c.execute("PRAGMA foreign_keys = OFF")
                    c.execute('insert into Tasks_to_Task_Sets values (?,?)', (addtask, task_set))
                    c.execute("PRAGMA foreign_keys = ON")
                    addedclasses += 1
                    print 'Node class added succesfully.' + os.linesep
                except sqlite3.Error as e:
                    handleDbError(e)
            else:
                if not yes_or_no("The specified task set doesn't exist in the database, do you want to add another task set?"):
                    nextclass = 0
                    if addedclasses == 0:
                        print 'It is impossible to define a node class with no task sets assigned to it, breaking.' + os.linesep
                        break

            if yes_or_no('Do you want to add another task?', 'no'):
                nexttask = 1
            else:
                nexttask = 0


    if description == 'The description has not been specified.':
        if yes_or_no('The description of the task has not been specified, do you want to do this now?', 'no'):
            description = raw_input('Please provide the description for your task: \n')
            print 'Success.' + os.linesep

    SET = (task_set, description)
    c.execute("PRAGMA foreign_keys = OFF")
    c.execute('insert into Task_Sets values (?,?)', SET)
    c.execute("PRAGMA foreign_keys = ON")
    print 'The task set has been added to the database.' + os.linesep



# ---------------------------------------------------------------------------------------------------------------------

def deleteClass():
    return

# ---------------------------------------------------------------------------------------------------------------------

def modifyClass():
    return

# ---------------------------------------------------------------------------------------------------------------------

def selectClass():
    return

# =====================================================================================================================
#                              NODES
# =====================================================================================================================

def addNodes():
    return
# =====================================================================================================================
#                              OTHERS
# =====================================================================================================================

# ---------------------------------------------------------------------------------------------------------------------

def connectDb():
    try:
        conn = sqlite3.connect(_database)
        c = conn.cursor()
        c.execute("PRAGMA foreign_keys = ON")
        print 'Connected to the database.' + os.linesep
        return c
    except:
        print 'ERROR: Couldn\'t connect to the database.'

# ---------------------------------------------------------------------------------------------------------------------

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

# ---------------------------------------------------------------------------------------------------------------------

def yes_or_no(question, default="yes"):

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

# ---------------------------------------------------------------------------------------------------------------------

def LHCb():

    # Just a nice easter egg :)

    from pyfiglet import Figlet
    #f = Figlet(font='speed')
    f = Figlet(font='slant')
    print(f.renderText('LHCb'))
    f = Figlet(font='smkeyboard')
    print(f.renderText('Online'))

# ---------------------------------------------------------------------------------------------------------------------

def existsInDb(db_connection, variable, column, table):
    c = db_connection
    c.execute('SELECT ' + column + ' FROM ' + table + ' WHERE ' + column + '="' + variable + '\"')
    data = c.fetchall()
    if len(data) == 0:
        return False
    else:
        return True

def existsInDbPair(db_connection, variable1, column1, variable2, column2, table):
    c = db_connection
    c.execute('SELECT * ' + 'FROM ' + table + ' WHERE ' + column1 + '="' + variable1 + '\" AND ' + column2 + '="' + variable2 + '\"')
    data = c.fetchall()
    if len(data) == 0:
        return False
    else:
        return True



