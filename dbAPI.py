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

def addTask(name, utgid, script, script_parameters = '', pcAdd_parameters = '', description = 'The description has not been specified.'):
    connectDb()


    if(description == 'The description has not been specified.'):
        if(yes_or_no('The description of the task has not been specified, do you want to do this now?')):
            description = raw_input('Please provide the description for your task: \n')
            print 'Success.' + os.linesep

    if (script_parameters == ''):
        if (yes_or_no('The parameters string for the task has not been specified, do you want to do this now?')):
            script_parameters = raw_input('Please provide the parameters for your script in the line below: \n')
            print 'Success.' + os.linesep

    if (pcAdd_parameters == ''):
        if (yes_or_no('No special parameters for the pcAdd command has been specified, do you want to do this now?')):
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
                if yes_or_no('Do you want to modify the name of the task?'):
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
                if yes_or_no('Do you want to modify the utgid of the task?'):
                    mod_utgid = raw_input("Please specify a new utgid: ")
                    c.execute(''.join([

                        'UPDATE Tasks ',
                        'SET ',
                        'utgid = "', mod_utgid, '\" ',
                        'WHERE Tasks.task = "', name, '\"', 'OR Tasks.task = "', mod_task, '\"',

                    ]))
            elif row[0] == 'command':
                if yes_or_no('Do you want to modify the script name of the task?'):
                    mod_command = raw_input("Please specify a new script name: ")
                    c.execute(''.join([

                        'UPDATE Tasks ',
                        'SET ',
                        'command = "', mod_command, '\" ',
                        'WHERE Tasks.task = "', name, '\"', 'OR Tasks.task = "', mod_task, '\"',

                    ]))
            elif row[0] == 'task_parameters':
                if yes_or_no('Do you want to modify the parameters string of the task?'):
                    mod_task_parameters = raw_input("Please specify a new parameters string: ")
                    c.execute(''.join([

                        'UPDATE Tasks ',
                        'SET ',
                        'task_parameters = "', mod_task_parameters, '\" ',
                        'WHERE Tasks.task = "', name, '\"', 'OR Tasks.task = "', mod_task, '\"',

                    ]))
            elif row[0] == 'command_parameters':
                if yes_or_no('Do you want to modify the additional pcAdd parameters associated with the task?'):
                    mod_command_parameters = raw_input("Please specify a new pcAdd parameters string: ")
                    c.execute(''.join([

                        'UPDATE Tasks ',
                        'SET ',
                        'command_parameters = "', mod_command_parameters, '\" ',
                        'WHERE Tasks.task = "', name, '\"', 'OR Tasks.task = "', mod_task, '\"',

                    ]))
            elif row[0] == 'description':
                if yes_or_no('Do you want to modify the description of the task?'):
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

        i = 0
        for row in c:
            if i == 0:
                FORMAT = '%-16s%-16s%-16s%-16s%-20s%-40s'
                print FORMAT % ('task', 'utgid', 'command', 'parameters', 'pcAdd parameters', 'description')
                print '-' * 100
            print FORMAT % row
            i += 1
        print os.linesep

    except sqlite3.Error as e:
        handleDbError(e)


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
    """Ask a yes/no question via raw_input() and return their answer.

    "question" is a string that is presented to the user.
    "default" is the presumed answer if the user just hits <Enter>.
        It must be "yes" (the default), "no" or None (meaning
        an answer is required of the user).

    The "answer" return value is True for "yes" or False for "no".
    """
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
    from pyfiglet import Figlet
    #f = Figlet(font='speed')
    f = Figlet(font='slant')
    print(f.renderText('LHCb'))
    f = Figlet(font='smkeyboard')
    print(f.renderText('Online'))









