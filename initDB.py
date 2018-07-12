#!/usr/bin/env python

import sqlite3
import os

# ===================================================================
#                           DEFINITIONS
# ===================================================================

os.remove('/home/leon/Desktop/Cern_Project/SQL_DB/LHCb.db')
conn = sqlite3.connect('/home/leon/Desktop/Cern_Project/SQL_DB/LHCb.db')
c = conn.cursor()
c.execute("PRAGMA foreign_keys = ON")

# ===================================================================
#                            TEST DATA
# ===================================================================

TASKS = (
    ('Task1', 'task_1', 'sleep1.sh', '4', '-c test1', 'Sleep for 4 seconds'),
    ('Task2', 'task_2', 'sleep1.sh', '3', '-c test2', 'Sleep for 3 seconds'),
    ('Task3', 'task_3', 'sleep1.sh', '7', '-c test3', 'Sleep for 7 seconds'),
)

TASKS_SETS = (
    ('Task1', 'Task_Set_1'),
    ('Task1', 'Task_Set_2'),
    ('Task2', 'Task_Set_2'),
    ('Task2', 'Task_Set_3'),
    ('Task3', 'Task_Set_1'),
)

SETS = (
    ('Task_Set_1', 'Contains Task1 and Task3'),
    ('Task_Set_2', 'Containst Task1 and Task2'),
    ('Task_Set_3', 'Containst Task2'),
)

SETS_CLASSES = (
    ('Task_Set_1', 'Sleeping_node'),
    ('Task_Set_2', 'Sleeping_node'),
    ('Task_Set_3', 'Sleeping_node'),
)

CLASSES = (
    ('Sleeping_node', 'Nodes that are very sleepy'),
)

NODES = (
    ('slp[a-z][0-9][0-9]', 'Sleeping_node'),
)

# ===================================================================
#                          TABLE CREATION
# ===================================================================

# Tasks
c.execute('''create table Tasks (

        task text NOT NULL,
        utgid text NOT NULL,
        command text NOT NULL
        task_parameters text,
        command_parameters text,
        description text,
        
        PRIMARY KEY (task),
        CONSTRAINT unique_tasks UNIQUE (task, utgid, command, task_parameters, command_parameters)
        
        )''')

# Tasks_to_Task_Sets
c.execute('''create table Tasks_to_Task_Sets (
        
        task text NOT NULL,
        task_set text NOT NULL,
        
        CONSTRAINT unique_task_to_task_set UNIQUE (task, task_set),
        FOREIGN KEY(task) REFERENCES Tasks(task),
        FOREIGN KEY(task_set) REFERENCES Task_Sets(task_set)
                
        )''')

# Task_Sets
c.execute('''create table Task_Sets(

        task_set text NOT NULL,
        description text,
        
        PRIMARY KEY (task_set)

        )''')

# Task_Sets_to_Classes
c.execute('''create table Task_Sets_to_Classes(
        
        task_set text NOT NULL,
        class text NOT NULL,
        
        CONSTRAINT unique_task_set_to_class UNIQUE (task_set, class),
        FOREIGN KEY(task_set) REFERENCES Task_Sets(task_set),
        FOREIGN KEY(class) REFERENCES Classes(class)
        
        )''')

# Classes
c.execute('''create table Classes(

        class text NOT NULL,
        description text,
        
        PRIMARY KEY (class)

        )''')

# Nodes
c.execute('''create table Nodes(

        regex text NOT NULL,
        class text NOT NULL,
        
        CONSTRAINT unique_regex_to_node_class UNIQUE (regex, class),
        FOREIGN KEY(class) REFERENCES Classes(class)

        )''')


# ===================================================================
#                        TEST DATA INJECTION
# ===================================================================

for row in TASKS:
    c.execute('insert into Tasks values (?,?,?,?,?,?)', row)
for row in SETS:
    c.execute('insert into Task_Sets values (?,?)', row)
for row in TASKS_SETS:
    c.execute('insert into Tasks_to_Task_Sets values (?,?)', row)
for row in CLASSES:
    c.execute('insert into Classes values (?,?)', row)
for row in SETS_CLASSES:
    c.execute('insert into Task_Sets_to_Classes values (?,?)', row)
for row in NODES:
    c.execute('insert into Nodes values (?,?)', row)


# ===================================================================
#                 SAVE AND CLOSE THE DB CONNECTION
# ===================================================================

conn.commit()
conn.close()