# ======================================================================================
# Module to generate command line shortcuts for bash shells
#
# \author  M.Frank
# \version 1.0
# --------------------------------------------------------------------------------------
import os

def cmdLines():
    print('echo "... Evaluating statements to get the proper TaskDB environment.";')
    dir_name = os.path.realpath(os.path.dirname(__file__))
    pack_dir = os.path.dirname(os.path.dirname(dir_name))
    if os.environ.get('PYTHONPATH',None) is not None:
        print('export PYTHONPATH='+pack_dir+'/python:${PYTHONPATH};')
    else:
        print('export PYTHONPATH='+pack_dir+'/python;')
    print('export TASKDB_ROOT='+pack_dir+';')
    print('echo "python TaskDB/Server.py -d -p 8081 -f test.db --run -t json -T";')
    routines=['addTask',  'getTask',  'modifyTask',  'deleteTask',  'assignTask',  'unassignTask',
              'addSet',   'getSet',   'modifySet',   'deleteSet',   'assignSet',   'unassignSet',
              'addClass', 'getClass', 'modifyClass', 'deleteClass', 'assignClass', 'unassignClass',
              'addNode',  'getNode',  'modifyNode',  'deleteNode', 
              'tasksInSet', 'tasksetsInClass', 'nodeclassInNode',
              'getTasksByNode'
              ]
    for r in routines:
        print('alias taskdb_'+r+'=\''+pack_dir+'/scripts/taskDB_CLI.sh    -action '+r+'\';')

if __name__ == '__main__':
    cmdLines()
