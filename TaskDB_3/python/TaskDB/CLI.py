# =========================================================================================================================
#                      LHCb Online Farm Process Explorer 
#                  Command line interface class using Main API
#                           
#                            K.Wilczynski 08.2018
# =========================================================================================================================

import os, logging
from  Printout import *

DSC_DEFAULT = 'The description has not been specified.'

# =========================================================================================================================
# Main user claqss to interact with the task database from the command line
# The underlying api may be a direct database connection or a handle to a
# XMLRPC/JSONRPC server.
#
# \author  K.Wilczynski
# \version 1.0
# -------------------------------------------------------------------------------------------------------------------------
class Client:

    # =====================================================================================================================
    # Constructor
    #
    # \param  type               [REQUIRED]  Connection type (rpc, direct)
    # \param  connection         [OPTIONAL]  connection string depending on the type
    # \param  debug              [OPTIONAL]  Enab;e debug output
    # \param  output             [OPTIONAL]  Flag to print results for 'get'* calls
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def __init__(self, type, connection, debug=False, output=False, sslopt=None):
        import TaskDB as implementation
        #import pdb; pdb.set_trace()
        self.batch  = False
        self.debug  = debug
        self.output = output or debug
        self.api    = implementation.connect(type, connection, sslopt=sslopt)
        self.local  = type.find('rpc') == -1

    # =====================================================================================================================
    # Helper method to execute local or remote method
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def run(self, func, **args):
        try:
            if self.local:
                result = func(**args)
            else:
                result = func(args)
            if self.debug:  print (str(result))
            return result
        except Exception as X:
            # Need to massage RPC exceptions to get a proper format
            #import pdb; pdb.set_trace()
            err = str(X)
            idx = err.find('| Exception:')
            if idx >= 0:
                err = err[idx+len('| Exception:'):-1]
            else:
                idx = err.find('<Fault ')
                if idx >= 0:
                    idq = err[idx:].find('>:')
                    if idq >=0: err = err[idx+idq+2:-1]
            logging.info('         ===> Result (Exception): '+err)
            raise Exception(err)

    # =====================================================================================================================
    # Helper method to execute local or remote method
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def do_call(self,method, **args):
        call_args = {}
        for key in args:
            call_args[key] = args[key]
        call = 'self.run(self.api.'+method+', '+str(call_args)[1:-1]+')'
        call = call.replace(', \'',', ').replace('\': ','=')
        if self.debug: print (call)
        result = eval(call)
        return result

    # =====================================================================================================================
    # Helper method to inquire command line arguments
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def yes_or_no(self, question, default="yes"):
        import sys
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
            choice = input().lower()
            if default is not None and choice == '':
                return valid[default]
            elif choice in valid:
                return valid[choice]
            else:
                sys.stdout.write("Please respond with 'yes' or 'no' "
                                "(or 'y' or 'n').\n")

    # =====================================================================================================================
    # Add a Tasks instance to the Tasks table
    #
    # \param  task               [REQUIRED]  Name of the task to be added
    # \param  utgid              [OPTIONAL]  Task unique identifier
    # \param  command            [OPTIONAL]  Executable or script to be executed
    # \param  task_parameters    [OPTIONAL]  String with parameters required to submit the desired command
    # \param  command_parameters [OPTIONAL]  String with parameters required to execute the command
    # \param  description        [OPTIONAl]  String containing explicatory text describing the command
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def addTask(self, task, utgid='', command='', task_parameters='', command_parameters='', description=DSC_DEFAULT):
        if ( not self.batch and utgid == ''):
            if (self.yes_or_no('The required parameter - utgid of the task has not been specified, do you want to do this now?', 'yes')):
                utgid = input('Please provide the utgid for your task: \n')
                print ('Success.' + os.linesep)
        if ( not self.batch and command == ''):
            if (self.yes_or_no('The required parameter - script name of the task has not been specified, do you want to do this now?', 'yes')):
                command = input('Please provide the script name for your task: \n')
                print ('Success.' + os.linesep)
        if ( not self.batch and  description == DSC_DEFAULT):
            if(self.yes_or_no('The description of the task has not been specified, do you want to do this now?', 'no')):
                description = input('Please provide the description for your task: \n')
                print ('Success.' + os.linesep)
        if ( not self.batch and command_parameters == ''):
            if (self.yes_or_no('The parameters string for the task has not been specified, do you want to do this now?', 'no')):
                command_parameters = input('Please provide the parameters for your script in the line below: \n')
                print ('Success.' + os.linesep)
        if ( not self.batch and task_parameters == ''):
            if (self.yes_or_no('No special parameters for the pcAdd command has been specified, do you want to do this now?', 'no')):
                task_parameters = input('Please provide special parameters in the line below: \n')
                print ('Success.' + os.linesep)
        return self.do_call('addTask',task=task,
                            utgid=utgid,
                            command=command,
                            command_parameters=command_parameters,
                            task_parameters=task_parameters,
                            description=description)

    # =====================================================================================================================
    # Delete a Tasks instance from the Tasks table
    #
    # \param  task               [REQUIRED]  Name of the task to be deleted
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def deleteTask(self, task):
        return self.do_call('deleteTask',task=task)

    # =====================================================================================================================
    # Modify an existing Tasks instance in the Tasks table
    #
    # \param  original_task      [REQUIRED]  Name of the task to be deleted
    # \param  **args             [REQUIRED]  Entries of the selected row to be changed.
    #                                        For the list of parameter names see addTask(...)
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def modifyTask(self, original_task, **args):
        return self.do_call('modifyTask',original_task=original_task,**args)

    # =====================================================================================================================
    # Access an existing Tasks instance in the Tasks table
    # 
    # \param  task               [REQUIRED]  Name of the task to be accessed
    # \return                                The desired object in JSON format / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def getTask(self, task):
        result = self.do_call('getTask',task=task)
        if self.output and result.__class__ == list:
            printTasks(result)
        elif self.output:
            print ('# getTasks: ',result)
        return result

    # =====================================================================================================================
    # Assign an existing Tasks instance in the Tasks table to a given task set
    # 
    # \param  task               [REQUIRED]  Name of the task to be accessed
    # \param  task_set           [REQUIRED]  Name of the task set the task should be added to
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def assignTask(self, task, task_set):
        return self.do_call('assignTask',task=task,task_set=task_set)

    # =====================================================================================================================
    # De-assign a Task instance from a given task set
    # 
    # \param  task               [REQUIRED]  Name of the task to be accessed
    # \param  task_set           [REQUIRED]  Name of the task set the task should be added to
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def unassignTask(self, task, task_set):
        return self.do_call('unassignTask',task=task,task_set=task_set)

    # =====================================================================================================================
    # Query the tasks assigned to a given task set
    # 
    # \param  task_set           [REQUIRED]  Name of the task set the task should be queried
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def tasksInSet(self, task_set):
        ret = self.do_call('tasksInSet',task_set=task_set)
        if self.output:
            print ('# tasksInSet: ',ret)
        return ret

    # =====================================================================================================================
    # Create a new TaskSet instance to the Task_Sets table
    # A Task set is an associative object grouping a number of task instances together
    #
    # \param  task_set           [REQUIRED]  Name of the task set to be added
    # \param  description        [OPTIONAl]  String containing explicatory text describing the task set
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def addSet(self, task_set, description=DSC_DEFAULT):
        if ( not self.batch and  description == DSC_DEFAULT):
            if(self.yes_or_no('The description of the task set has not been specified, do you want to do this now?', 'no')):
                description = input('Please provide the description: \n')
                print ('Success.' + os.linesep)
        return self.do_call('addSet',task_set=task_set,description=description)

    # =====================================================================================================================
    # Delete an existing TaskSet instance from the Task_Sets table
    #
    # \param  task_set           [REQUIRED]  Name of the task set to be deleted
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def deleteSet(self, task_set):
        return self.do_call('deleteSet',task_set=task_set)

    # =====================================================================================================================
    # Modify an existing TaskSet instance from the Task_Sets table.
    # This allows to alter a set entry. (IS THIS USEFUL????)
    #
    # \param  task_set           [REQUIRED]  Name of the task set to be deleted
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def modifySet(self, original_task_set, **args):
        return self.do_call('modifySet',original_task_set=original_task_set,**args)

    # =====================================================================================================================
    # Access an existing Tasks instance in the Tasks table.
    # The result is returned as a JSON object to the client
    # 
    # \param  task               [REQUIRED]  Name of the task to be accessed
    # \return                                The desired object in JSON format / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def getSet(self, task_set):
        result = self.do_call('getSet',task_set=task_set)
        if self.output and result.__class__ == list:
            printTaskSets(result)
        elif self.output:
            print ('# getSets: ',result)
        return result

    # =====================================================================================================================
    # Assign an existing task set instance in the Task_Sets_to_Classes table to a given node class
    # 
    # \param  task_set           [REQUIRED]  Name of the task set to be accessed
    # \param  node_class         [REQUIRED]  Name of the node class the set should be added to
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def assignSet(self, task_set, node_class):
        return self.do_call('assignSet',task_set=task_set,node_class=node_class)

    # =====================================================================================================================
    # De-Assign an existing task set instance in the Task_Sets_to_Classes table from a given node class
    # 
    # \param  task_set           [REQUIRED]  Name of the task set to be accessed
    # \param  node_class         [REQUIRED]  Name of the node class the set should be removed from
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def unassignSet(self, task_set, node_class):
        return self.do_call('unassignSet',task_set=task_set,node_class=node_class)

    # =====================================================================================================================
    # Query the task sets assigned to a given node class
    # 
    # \param  task_set           [REQUIRED]  Name of the node class to be queried
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def tasksetsInClass(self, node_class):
        ret = self.do_call('tasksetsInClass',node_class=node_class)
        if self.output:
            print ('# tasksetsInClass: ',ret)
        return ret
        
    # =====================================================================================================================
    # Create a new (non-existing) node class object
    # 
    # \param  node_class         [REQUIRED]  Name of the node class to be created
    # \param  description        [OPTIONAl]  String containing explicatory text describing the class
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def addClass(self, node_class, description=DSC_DEFAULT):
        if ( not self.batch and  description == DSC_DEFAULT):
            if(self.yes_or_no('The description of the node class has not been specified, do you want to do this now?', 'no')):
                description = input('Please provide the description: \n')
                print ('Success.' + os.linesep)
        return self.do_call('addClass',node_class=node_class, description=description)

    # =====================================================================================================================
    # Delete a (hopefully existing) node class object
    # 
    # \param  node_class         [REQUIRED]  Name of the node class to be deleted
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def deleteClass(self, node_class):
        return self.do_call('deleteClass',node_class=node_class)

    # =====================================================================================================================
    # Modify an existing NodeClass instance from the Classes table.
    #
    # \param  task_set           [REQUIRED]  Name of the task set to be deleted
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def modifyClass(self, original_node_class, **args):
        return self.do_call('modifyClass',original_node_class=original_node_class,**args)

    # =====================================================================================================================
    # Access an existing NodeClass instance in the Classes table.
    # 
    # \param  node_class         [REQUIRED]  Name of the node_class to be accessed
    # \return                                The desired object in JSON format / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def getClass(self, node_class):
        result = self.do_call('getClass',node_class=node_class)
        if self.output and result.__class__ == list:
            printClasses(result)
        elif self.output:
            print ('# getClass: ',result)
        return result

    # =====================================================================================================================
    # Assign an existing node class instance in the Classes table to a given node type
    # 
    # \param  node_class         [REQUIRED]  Name of the node class to be accessed
    # \param  regex              [REQUIRED]  Name of the node type the class should be assigned to
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def assignClass(self, node_class, regex):
        return self.do_call('assignClass',node_class=node_class,regex=regex)

    # =====================================================================================================================
    # De-Assign an existing node class instance in the Classes table from a given node type
    # 
    # \param  node_class         [REQUIRED]  Name of the node class to be accessed
    # \param  regex              [REQUIRED]  Name of the node type the class should be assigned to
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def unassignClass(self, node_class, regex):
        return self.do_call('unassignClass',node_class=node_class,regex=regex)

    # =====================================================================================================================
    # Query the node classes assigned to a given node type
    # 
    # \param  regex              [REQUIRED]  Name of the node type of which the assigned objects should be returned
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def nodeclassInNode(self, regex):
        ret = self.do_call('nodeclassInNode',regex=regex)
        if self.output:
            print ('# nodeclassInNode: ',ret)
        return ret

    # =====================================================================================================================
    # Create a new (non-existing) node type object
    # 
    # \param  regex              [REQUIRED]  Name of the node types to be created
    # \param  description        [OPTIONAl]  String containing explicatory text describing the node type
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def addNode(self, regex, description=DSC_DEFAULT):
        if ( not self.batch and  description == DSC_DEFAULT):
            if(self.yes_or_no('The description of the node type has not been specified, do you want to do this now?', 'no')):
                description = input('Please provide the description: \n')
                print ('Success.' + os.linesep)
        return self.do_call('addNode',regex=regex,description=description)
    
    # =====================================================================================================================
    # Delete a (hopefully existing) node type object
    # 
    # \param  regex              [REQUIRED]  Name of the node types to be deleted
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def deleteNode(self, regex):
        return self.do_call('deleteNode',regex=regex)
    
    # =====================================================================================================================
    # Modify an existing node type instance from the Nodes table.
    #
    # \param  original_regex     [REQUIRED]  Name of the node type to be deleted
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def modifyNode(self, original_regex, **args):
        return self.do_call('modifyNode',original_regex=original_regex,**args)
    
    # =====================================================================================================================
    # Access an existing node type instance in the Nodes table.
    # 
    # \param  regex              [REQUIRED]  Name of the node type  to be accessed
    # \return                                The desired object in JSON format / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def getNode(self, regex):
        result = self.do_call('getNode',regex=regex)
        if self.output and result.__class__ == list:
            printNodeTypes(result)
        elif self.output:
            print ('# getNode: ',result)
        return result

    # =====================================================================================================================
    # Access an existing node type instance in the Nodes table.
    # 
    # \param  regex              [REQUIRED]  Name of the node type  to be accessed
    # \return                                The desired object in JSON format / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def getTasksByNode(self, node, fmc=None):
        result = self.do_call('getTasksByNode',node=node)
        if self.output and result.__class__ == list and not fmc:
            printTasks(result)
        elif self.output and result.__class__ == list:
            printTasksFMC(node,result)
        elif self.output:
            print ('# getTasksByNode: ',result)
        return result

def basic_test():
    c = Client()
    c.getTask('*')
    t = 'Myblalalabb'
    s = 'MyTaskset'
    """
    """
    c.addTask(t,utgid=t+'_0',command='cmd_'+t+'+.sh')
    c.getTask(t)
    c.modifyTask(t,utgid='Other'+t+'_0',command='cmd.sh')
    c.addSet(s,'Some stupid description')
    c.assignTask(t,s)
    c.getSet(s)
    c.unassignTask(t,s)
    c.deleteSet('MyTaskset')
    c.deleteTask(t)

if __name__ == "__main__":
    import sys, errno, optparse
    parser = optparse.OptionParser()
    parser.description = "TaskDB command line interface."
    parser.formatter.width = 132
    parser.add_option("-a", "--action", 
                      dest="action", default=None,
                      help="Specify the call to be made",
                      metavar="<string>")
    parser.add_option("-d", "--debug", 
                      action='store_true',
                      dest="debug", default=False,
                      help="Enable output debug statements",
                      metavar="<boolean>")
    parser.add_option("-D", "--pdb", 
                      action='store_true',
                      dest="run_pdb", default=False,
                      help="Start python debugger",
                      metavar="<boolean>")
    parser.add_option("-p", "--print", 
                      action='store_true',
                      dest="output", default=False,
                      help="Enable printout statements for 'get' calls",
                      metavar="<boolean>")
    parser.add_option("-c", "--connection",
                      dest = "connection",
                      default = 'http://localhost:8081/XMLRPC',
                      help="RPC server connection  (RPC calls only)",
                      metavar="<string>")
    parser.add_option("-C", "--certfile",
                      dest="certfile",
                      default='./ssl/server.crt',
                      help="Server SSL certificate",
                      metavar="<string>")
    parser.add_option("-o", "--options",
                      action='store_true',
                      dest="arguments",
                      default=False,
                      help="Start tag for functional arguments",
                      metavar="<boolean>")
    (opts, args) = parser.parse_args()

    if not opts.action or not hasattr(Client,opts.action):
        logging.error('Unknown TaskDB request: '+str(opts.action)+' [No action taken]')
        sys.exit(errno.EINVAL)

    conn   = opts.connection
    connl  = conn.lower()
    sslopt = {}
    if opts.certfile:
        sslopt['ca_certs'] = opts.certfile

    if opts.run_pdb:
        import pdb; pdb.set_trace()

    args = ''
    start = False
    for i in sys.argv:
        if i == '-o' or i == '--options':
            start = True
        elif start:
            a1, a2 = i.split('=')
            if len(args): args = args + ', '
            args = args + a1 + '='
            if a2[0]  != '"': args = args + '"'
            args = args + a2
            if a2[-1] != '"': args = args + '"'

    print (args, str(args))
    if connl[:4] == 'http' or conn[:2] == 'ws':
        if connl.find('/jsonrpc')  >=0: typ = 'jsonrpc'
        elif connl.find('/xmlrpc') >=0: typ = 'xmlrpc'
        cl = Client(typ, conn, debug=opts.debug, output=opts.output, sslopt=sslopt)
    elif connl[:7] == 'sqllite':
        cl = Client('direct', conn, debug=opts.debug, output=opts.output, sslopt=sslopt)
    else:
        logging.error('Unknown TaskDB connection:'+conn)
        sys.exit(errno.EINVAL)
    try:
        attr = getattr(cl,opts.action)
        #cmd  = 'getattr(cl,opts.action)('+str(opts.arguments)+')'
        cmd  = 'attr('+str(args)+')'
        print (cmd)
        result = eval(cmd)
        if result.__class__ == str and result == 'Success':
            logging.info('Request "'+opts.action+'(...)" was executed successfully.')
            sys.exit(0)
        else:
            logging.info('Request "'+opts.action+'(...)" was executed successfully.')
            sys.exit(0)
    except Exception as X:
        logging.error('Request "'+opts.action+'(...)" FAILED. Exception: '+str(X))
        sys.exit(errno.EINVAL)
