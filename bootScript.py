#!/usr/bin/env python

import sqlite3
import os


# =====================================================================================================================
#                           DEFINITIONS
# =====================================================================================================================

_scripts = 'group/online/dataflow/scripts' + os.sep
_database = '/home/leon/Desktop/Cern_Project/SQL_DB/LHCb.db'
logDefaultFIFO = '/run/fmc/logSrv.fifo'
dataFlowDirScripts = '/group/online/dataflow/cmtuser/Gaudi_v19r2/Online/OnlineControls/scripts'
default_exports = 'export PATH=/opt/FMC/sbin:/opt/FMC/bin:$PATH;export LD_LIBRARY_PATH=/opt/FMC/lib;'
top_logger_host = 'ecs03'
top_farm_host = 'hlt01'

# =====================================================================================================================
#                            BOOT SCRIPTS
# =====================================================================================================================

def boot(regex = '*'):

    tasklist = []

    conn = connectDb()

    # Full system boot - need to read all node regex
    if regex == '*':
        print 'Starting full system boot sequence...'
        try:

            conn.execute(''.join([

                'SELECT DISTINCT Nodes.regex ',
                'FROM Nodes ',

            ]))
        except sqlite3.Error as error:
            handleDbError(error)

        for host in conn:
            # Boot nodes specified by all of the regular expressions found in the database
            bootnode(host[0])

    # Abort boot if the specified regex is not found in the database
    elif not existsInDb(conn, regex, 'regex', 'Nodes'):
        print 'The specified node name (or regular expression) was not found in the database, breaking.' + os.linesep
        return

    # Single regex boot
    else:
        print 'Starting boot sequence on ' + regex + '...'
        bootnode(regex)


# ==============================    =======================================================================================
#                              OTHERS
# =====================================================================================================================

def connectDb():
    try:
        conn = sqlite3.connect(_database)
        c = conn.cursor()
        c.execute("PRAGMA foreign_keys = ON")
        # print 'Connected to the database.' + os.linesep
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

def bootnode(regex):

    tasklist = []

    # select distinct Task_Sets_to_Classes.class from Task_Sets_to_Classes join Nodes on Nodes.class = Task_Sets_To_Classes.class where Nodes.regex = 'slp[a-z][0-9][0-9]';
    try:
        a = connectDb()
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
                    # Collect the tasks for the current host in a list
                    tasklist.append(row4)

    # Remove duplicate tasks from the list created by looping SQL queries above
    tasklist = set(tasklist)
    print os.linesep + 'Tasks to be started on ' + regex + ":" + os.linesep
    for task in tasklist:
        # pcAdd
        log_opts = '-E ' + logDefaultFIFO + ' -O ' + logDefaultFIFO
        if task[3] != '':
            script_params = task[3]
        if task[4] != '':
            pcAdd_Params = task[4]
        else:
            pcAdd_Params = '-K 120 -M 5 -X 300 -g onliners -p 0 -n online'

        #             user def or default       logs              host              utgid            path     script      script params
        cmd = 'pcAdd ' + pcAdd_Params + ' ' + log_opts + ' -m ' + regex + ' -u ' + task[1] + ' ' + _scripts + task[2] + ' ' + script_params + ';'
        print 'echo "' + cmd + '";'

        # log_opts = '-E ' + str(fifo) + ' -O ' + str(fifo) was ommited for now! it depends on default None variable in the original script


