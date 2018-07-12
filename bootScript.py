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
#                            BOOT SCRIPTS
# =====================================================================================================================

def boot(regex):
    a = connectDb()

    if not existsInDb(a, regex, 'regex', 'Nodes'):
        print 'The specified node name (or regular expression) was not found in the database, breaking.' + os.linesep
        return

    # select distinct Task_Sets_to_Classes.class from Task_Sets_to_Classes join Nodes on Nodes.class = Task_Sets_To_Classes.class where Nodes.regex = 'slp[a-z][0-9][0-9]';
    try:
        a.execute(''.join([

            'SELECT DISTINCT Task_Sets_to_Classes.class ',
            'FROM Task_Sets_to_Classes ',
            'JOIN Nodes ON Nodes.class = Task_Sets_to_Classes.class ',
            'WHERE Nodes.regex = "', regex, '\"',

        ]))
    except sqlite3.Error as err1:
        handleDbError(err1)

    for row1 in a:
        # select distinct Task_Sets_to_Classes.task_set from Task_Sets_to_Classes join Classes on Task_Sets_to_Classes.class = (select distinct Classes.class from Classes join Nodes on Nodes.class = Classes.class);
        try:
            b = connectDb()
            b.execute(''.join([

                'SELECT DISTINCT Task_Sets_to_Classes.task_set ',
                'FROM Task_Sets_to_Classes ',
                'JOIN Classes on Task_Sets_to_Classes.class = "', row1[0], '\"'

            ]))
        except sqlite3.Error as err2:
            handleDbError(err2)

        for row2 in b:
            try:
                c = connectDb()
                c.execute(''.join([

                    'SELECT DISTINCT Tasks_to_Task_Sets.task ',
                    'FROM Tasks_to_Task_Sets ',
                    'JOIN Task_Sets_to_Classes on  Tasks_to_Task_Sets.task_set = "', row2[0], '\"'

                ]))
            except sqlite3.Error as err3:
                handleDbError(err3)

            for row3 in c:
                try:
                    d = connectDb()
                    d.execute(''.join([

                        'SELECT DISTINCT Tasks.* ',
                        'FROM Tasks ',
                        'WHERE Tasks.task = "', row3[0], '\"',

                    ]))
                except sqlite3.Error as err3:
                    handleDbError(err3)

                i = 0
                for row4 in d:
                    if i == 0:
                        FORMAT = '%-16s%-16s%-16s%-16s%-20s%-40s'
                        print FORMAT % ('task', 'utgid', 'command', 'parameters', 'pcAdd parameters', 'description')
                        print '-' * 124
                    print FORMAT % row4
                    i += 1




# =====================================================================================================================
#                              OTHERS
# =====================================================================================================================

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

def existsInDb(db_connection, variable, column, table):
    c = db_connection
    c.execute('SELECT ' + column + ' FROM ' + table + ' WHERE ' + column + '="' + variable + '\"')
    data = c.fetchall()
    if len(data) == 0:
        return False
    else:
        return True

# ---------------------------------------------------------------------------------------------------------------------




