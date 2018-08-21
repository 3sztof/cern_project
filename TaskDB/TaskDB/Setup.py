# ======================================================================================
# Module to generate command line shortcuts for bash shells
#
# \author  M.Frank
# \version 1.0
# --------------------------------------------------------------------------------------
import os

def cmdLines():
    print "echo \"... Evaluating statements to get the proper TaskDB environment. \";"
    dir_name = os.path.realpath(os.path.dirname(__file__))
    pack_dir = os.path.dirname(dir_name)
    if os.environ.has_key('PYTHONPATH'):
        print "export PYTHONPATH="+pack_dir+":${PYTHONPATH};"
    else:
        print "export PYTHONPATH="+pack_dir+";"
    print "export TASKDB_ROOT="+pack_dir+";"
    routines=['addTask',  'getTask',  'modifyTask',  'deleteTask',  'assignTask',  'unassignTask',
              'addSet',   'getSet',   'modifySet',   'deleteSet',   'assignSet',   'unassignSet',
              'addClass', 'getClass', 'modifyClass', 'deleteClass', 'assignClass', 'unassignClass',
              'addNode',  'getNode',  'modifyNode',  'deleteNode'
              ]
    for r in routines:
        print "alias taskdb_"+r+"='"+pack_dir+"/scripts/taskDB_CLI.sh    -action "+r+"';"

if __name__ == '__main__':
    cmdLines()
