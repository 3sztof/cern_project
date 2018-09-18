# =========================================================================================================================
#                     LHCb Online Farm Process Explorer
#                                 Main API 
#                           
#                           K.Wilczynski 08.2018
# =========================================================================================================================
import os, logging

SUCCESS = 'Success'
FAILED  = 'FAILED'

NONE    = 0
SIMPLE  = 1
FULL    = 2
EXCEPTION_HANDLING = SIMPLE


# =========================================================================================================================
# Database connection object.
# For the time being only sqlite is supported.
#
# \author  K.Wilczynski
# \version 1.0
# -------------------------------------------------------------------------------------------------------------------------
class Connection:
    debug = False

    def __init__(self, database='sqlite:///./LHCb.db'):
        if database is None:
            my_path = os.path.dirname(os.path.realpath(__file__))
            database = 'sqlite:///' + my_path + os.sep + 'LHCb.db'
        self.database = database
        self.init()

    def init(self):
        if Connection.debug: 
            logging.info('++ Connection to database:'+self.database)
        # Initiate a connection, cursor object and turn on the constraints to protect the schema (created in initDB.py)
        self.open()
        self.configure()

    def __del__(self):
        if Connection.debug: logging.info('++ Closing database connection: '+self.database)

    def open(self):
        from sqlalchemy import create_engine
        self.engine = create_engine(self.database)
        self.handle = self.engine.connect()
        return SUCCESS

    def configure(self):
        items = self.database.split(':')
        if items[0].lower() == 'sqlite':
            self.handle.execute("PRAGMA foreign_keys = ON")
        elif items[0].lower() == 'oracle':
            pass
        return SUCCESS

    def close(self):
        return self.handle.close()

    def reconnect(self):
        self.init()


# =========================================================================================================================
# Database creator
# All the required tables for the TaskDB applications are created on demand.
#
# \author  K.Wilczynski
# \version 1.0
# -------------------------------------------------------------------------------------------------------------------------
class Creator:
    """Object to create the SQL table structure for a TaskDB instance.
    """

    def __init__(self,connection):
        """Constructor
           \param  connection    Database connection handle
        """
        self.connection = connection

    def __del__(self):
        """Destructor
        """
        self.connection = None

    def create(self):
        """Create database tables
        """
        c = self.connection.handle
        # Tasks
        c.execute('''create table Tasks (

                task               text NOT NULL,
                utgid              text NOT NULL,
                command            text NOT NULL,
                task_parameters    text,
                command_parameters text,
                description        text,

                PRIMARY KEY (task),
                CONSTRAINT unique_task UNIQUE (task)

                )''')

        # Tasks_to_Task_Sets
        c.execute('''create table Tasks_to_Task_Sets (

                task     text NOT NULL,
                task_set text NOT NULL,

                CONSTRAINT unique_task_to_task_set UNIQUE (task, task_set),
                FOREIGN KEY(task) REFERENCES Tasks(task) ON UPDATE CASCADE ON DELETE CASCADE,
                FOREIGN KEY(task_set) REFERENCES Task_Sets(task_set) ON UPDATE CASCADE ON DELETE CASCADE

                )''')

        # Task_Sets
        c.execute('''create table Task_Sets(

                task_set     text NOT NULL,
                description  text,

                PRIMARY KEY (task_set),
                CONSTRAINT unique_task_set UNIQUE (task_set)

                )''')

        # Task_Sets_to_Classes
        c.execute('''create table Task_Sets_to_Classes(

                task_set   text NOT NULL,
                node_class text NOT NULL,

                CONSTRAINT unique_task_set_to_class UNIQUE (task_set, node_class),
                FOREIGN KEY(task_set) REFERENCES Task_Sets(task_set) ON UPDATE CASCADE ON DELETE CASCADE,
                FOREIGN KEY(node_class) REFERENCES Classes(node_class) ON UPDATE CASCADE ON DELETE CASCADE

                )''')

        # Classes
        c.execute('''create table Classes(

                node_class text NOT NULL,
                description text,

                PRIMARY KEY (node_class),
                CONSTRAINT unique_node_class UNIQUE (node_class)

                )''')

        # Classes_to_Nodes
        c.execute('''create table Classes_to_Nodes(

                node_class text NOT NULL,
                regex text NOT NULL,

                CONSTRAINT unique_regex_to_node_class UNIQUE (regex, node_class),
                FOREIGN KEY(node_class) REFERENCES Classes(node_class) ON UPDATE CASCADE ON DELETE CASCADE
                FOREIGN KEY(regex) REFERENCES Nodes(regex) ON UPDATE CASCADE ON DELETE CASCADE

                )''')

        # Nodes
        c.execute('''create table Nodes(

                regex text NOT NULL,
                description text,

                PRIMARY KEY (regex),
                CONSTRAINT unique_regex UNIQUE (regex)

                )''')
        return SUCCESS


# =========================================================================================================================
# Main user claqss to interact with the task database
# This class is supposed to shield any application from the
# nitty gritty details of the underlying SQL implementation.
#
# \author  K.Wilczynski
# \version 1.0
# -------------------------------------------------------------------------------------------------------------------------
class TaskDB:

    # =====================================================================================================================
    # Standard object constructor
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def __init__(self, connection, dbg=False):
        # Configs
        self.dbg = dbg
        self.connection = connection

    def reconnect(self, arg=None):
        if self.connection:
            self.connection.reconnect()
            return 'Success'
        return 'FAILED'
    
    # =====================================================================================================================
    # Helper function to execute a single SQL statement within the engine
    # \param   statement     SQL string describing the statement to be executed
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def execStatement(self,statement):
        if self.dbg: logging.info('++         '+statement)
        query = self.connection.handle.execute(statement)
        return query

    # =====================================================================================================================
    # Helper function to allow generic and common exception handling for the 
    # whole interface. The actual behaviour depends on the value of the 
    # module parameter EXCEPTION_HANDLING (NONE, SIMPLE, FULL)
    #
    # \param   exception         [REQUIRED]  Python exception object to be handled
    # \param   value             [OPTIONAL]  Optional explanation of the exception cause
    # \return  None                          Erro code or exception raises
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def handleException(self, exception, value=None):
        if self.dbg: logging.info('++         '+str(exception))
        if EXCEPTION_HANDLING == SIMPLE:
            if value: raise Exception(value)
            raise Exception(str(exception))
        elif EXCEPTION_HANDLING == FULL:
            e = str(exception)
            err = ''
            if value: err = err + value + '\n'
            err = err + e
            raise Exception(err)
        elif EXCEPTION_HANDLING == NONE:
            if value: return value
            return str(exception)
        if value: raise Exception(value)
        raise Exception(str(exception))
         
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
    def addTask(self, task, utgid='', command='', task_parameters='', command_parameters='', description=''):
        try:
            cmd = "insert into Tasks values ('{0}','{1}','{2}','{3}','{4}','{5}')"\
                .format(task, utgid, command, task_parameters, command_parameters, description)
            query = self.execStatement(cmd)
            return SUCCESS
        except Exception as e:
            return self.handleException(e,'Failed to add task "%s" to task database'%(task,))

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
        try:
          cmd   = "delete from Tasks where task='{0}'".format(task)
          query = self.execStatement(cmd)
          if(query.rowcount >= 1):
              return SUCCESS
          raise Exception('deleteTask: Unknown task with name: ' + task)
        except Exception, e:
            return self.handleException(e,'Failed to delete task "%s" from database'%(task,))

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
        try:
          query = self.execStatement(statement)
          if(query.rowcount >= 1):
              return SUCCESS
          raise Exception('modifyTask: Unknown task with name: ' + original_task)
        except Exception, e:
            return self.handleException(e,'Failed to modify task "%s" in database'%(original_task,))

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
        try:
            if(task == '*'):
                query = self.execStatement("select * from Tasks")
            else:
                query = self.execStatement("select * from Tasks where task='{0}'".format(task,))
            result = [dict(zip(tuple(query.keys()), i)) for i in query.cursor]
            if len(result) == 0:
                raise Exception('getTask: Unknown task with name: ' + task)
            return result
        except Exception, e:
            return self.handleException(e,'Failed to access task "%s" in database'%(task,))

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
        try:
            # Handle bulk assignments/unassignments
            if task.__class__ == list:
                for t in task:
                    self.execStatement("insert into Tasks_to_Task_Sets values ('{0}', '{1}')".format(t, task_set))
                return SUCCESS
            # Handle single assignments/unassignments
            self.execStatement("insert into Tasks_to_Task_Sets values ('{0}', '{1}')".format(task, task_set))
            return SUCCESS
        except Exception, e:
            return self.handleException(e,'Failed to assign task "%s" to task set "%s"'%(task,task_set,))

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
        try:
            # Handle bulk assignments/unassignments
            if task.__class__ == list:
                for t in task:
                    self.execStatement("delete from Tasks_to_Task_Sets where task='{0}' and task_set='{1}'".format(t, task_set))
                return SUCCESS
            # Handle single assignments/unassignments
            query = self.execStatement("delete from Tasks_to_Task_Sets where task='{0}' and task_set='{1}'".format(task,task_set))
            if(query.rowcount >= 1):
                return SUCCESS
            raise Exception('unassignTask: The specified assignment pair does not exist in the database: ' + task + ' <-> ' + task_set)
        except Exception, e:
            return self.handleException(e,'Failed to un-assign task "%s" from task set "%s"'%(task,task_set,))

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
        message = None
        try:
            query = self.execStatement("SELECT task FROM Tasks_to_Task_Sets WHERE task_set='{0}'".format(task_set))
            result = [dict(zip(tuple(query.keys()), i)) for i in query.cursor]
            if len(result) > 0:
                return result
            if self.inDb('Task_Sets',task_set=task_set):
                return result
            message = 'The task set "%s" does not exist!'%(task_set,)
        except Exception, e:
            return self.handleException(e,'Failed to retrieve tasks for task set "%s"'%(task_set,))
        if message:
            raise Exception(message)

    # =====================================================================================================================
    # Query the task sets assigned to a given node class
    # 
    # \param  node_class         [REQUIRED]  Name of the node class the task set should be queried
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def tasksetsInClass(self, node_class):
        message = None
        try:
            query = self.execStatement("SELECT task_set FROM Task_Sets_to_Classes WHERE node_class='{0}'".format(node_class))
            result = [dict(zip(tuple(query.keys()), i)) for i in query.cursor]
            if len(result) > 0:
                return result
            if self.inDb('Classes',node_class=node_class):
                return result
            message = 'The node class "%s" does not exist!'%(node_class,)
        except Exception, e:
            return self.handleException(e,'Failed to retrieve task sets for node class "%s"'%(node_class,))
        if message:
            raise Exception(message)

    # =====================================================================================================================
    # Query the node classes assigned to a given node
    # 
    # \param  regex              [REQUIRED]  Name of the node the node classes should be queried
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def nodeclassInNode(self, regex):
        message = None
        try:
            query = self.execStatement("SELECT node_class FROM Classes_to_Nodes WHERE regex='{0}'".format(regex))
            result = [dict(zip(tuple(query.keys()), i)) for i in query.cursor]
            if len(result) > 0:
                return result
            if self.inDb('Nodes',regex=regex):
                return result
            message = 'The node "%s" does not exist!'%(regex,)
        except Exception, e:
            return self.handleException(e,'Failed to retrieve node classes for node "%s"'%(regex,))
        if message:
            raise Exception(message)


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
    def addSet(self, task_set, description=''):
        try:
            self.execStatement("insert into Task_Sets values ('{0}','{1}')".format(task_set, description))
            return SUCCESS
        except Exception, e:
            return self.handleException(e, 'Failed to add task set "%s" to database'%(task_set,))

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
        try:
            query = self.execStatement("delete from Task_Sets where task_set='{0}'".format(task_set,))
            if(query.rowcount >= 1):
                return SUCCESS
            raise Exception('deleteSet: Unknown task set with name: ' + task_set)
        except Exception, e:
            return self.handleException(e, 'Failed to delete task set "%s" from database'%(task_set,))

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
        if len(args) == 0: 
            raise Exception('No arguments provided, modification impossible')

        statement = "update Task_Sets set "
        cnt=0
        for key in args:
            if(cnt != 0):
                statement += ", "
            statement += (key + "='" + args[key] + "'")
            cnt += 1

        statement += (" where task_set='" + original_task_set + "'")
        try:
            query = self.execStatement(statement)
            if(query.rowcount >= 1):
               return SUCCESS
            raise Exception('modifySet: Unknown task set with name: ' + original_task_set)
        except Exception, e:
            return self.handleException(e, 'Failed to modify task set "%s" in database'%(task_set,))

    # =====================================================================================================================
    # Access an existing Tasks instance in the Tasks table.
    # 
    # \param  task               [REQUIRED]  Name of the task to be accessed
    # \return                                The desired object in JSON format / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def getSet(self, task_set):
        try:
            if task_set == '*':
                query = self.execStatement("select * from Task_Sets")
            else:
                query = self.execStatement("select * from Task_Sets where task_set='{0}'".format(task_set,))
            result = [dict(zip(tuple(query.keys()), i)) for i in query.cursor]
            if len(result) == 0:
                raise Exception('getSet: Unknown task set with name: ' + task_set)
            return result
        except Exception, e:
            return self.handleException(e, 'Failed to access task set "%s" from database'%(task_set,))

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
        try:
            self.execStatement("insert into Task_Sets_to_Classes values ('{0}','{1}')".format(task_set, node_class))
            return SUCCESS
        except Exception, e:
            return self.handleException(e,'Failed to assign task set "%s" to node class "%s"'%(task_set, node_class, ))

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
        try:
            cmd = "delete from Task_Sets_to_Classes where task_set='{0}' and node_class='{1}'".format(task_set, node_class)
            query = self.execStatement(cmd)
            if(query.rowcount >= 1):
                return SUCCESS
            raise Exception('unassignSet: The specified assignment pair does not exist in the database: ' + task_set + ' <-> ' + node_class)
        except Exception as e:
            return self.handleException(e,'Failed to un-assign task set "%s" from node class "%s"'%(task_set, node_class,))

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
        message = None
        try:
            query = self.execStatement("SELECT task_set as task_set FROM Task_Sets_to_Classes WHERE node_class='{0}'".format(node_class))
            result = [dict(zip(tuple(query.keys()), i)) for i in query.cursor]
            if len(result) > 0:
                return result
            if self.inDb('Classes',node_class=node_class):
                return result
            message = 'The node class "%s" does not exist!'%(node_class,)
        except Exception, e:
            return self.handleException(e,'Failed to retrieve task sets for node class "%s"'%(node_class,))
        if message:
            raise Exception(message)

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
    def addClass(self, node_class, description=''):
        try:
            self.execStatement("insert into Classes values ('{0}','{1}')".format(node_class, description))
            return SUCCESS
        except Exception as e:
            return self.handleException(e,'Failed to add a new node class with name "%s"'%(node_class,))

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
        try:
            query = self.execStatement("delete from Classes where node_class='{0}'".format(node_class,))
            if(query.rowcount >= 1):
                return SUCCESS
            raise Exception('deleteClass: Unknown node class with name: ' + node_class)
        except Exception, e:
            return self.handleException(e,'Failed to delete node class with name "%s"'%(node_class,))

    # =====================================================================================================================
    # Modify an existing node class instance from the Classes table.
    #
    # \param  original_node_class  [REQUIRED]  Name of the node class to be modified
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def modifyClass(self, original_node_class, **args):
        try:
            if len(args) == 0:
                raise Exception('No arguments provided, modification of node class "%s" impossible'%(original_node_class,))

            statement = "update Classes set "
            cnt=0
            for key in args:
                if(cnt != 0):
                    statement += ", "
                statement += (key + "='" + args[key] + "'")
                cnt += 1
            statement += (" where node_class='" + original_node_class + "'")
        
            query = self.execStatement(statement)
            if(query.rowcount >= 1):
                return SUCCESS
            raise Exception('modifyClass: Unknown node class with name: ' + original_node_class)
        except Exception,e:
            return self.handleException(e)

    # =====================================================================================================================
    # Access an existing node class instance in the Classes table.
    # 
    # \param  node_class         [REQUIRED]  Name of the node class to be accessed
    # \return                                The desired object in JSON format / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def getClass(self, node_class):
        try:
            if node_class == '*':
                query = self.execStatement("select * from Classes")
            else:
                query = self.execStatement("select * from Classes where node_class='{0}'".format(node_class,))
            result = [dict(zip(tuple(query.keys()), i)) for i in query.cursor]
            if len(result) == 0:
                raise Exception('getClass: Unknown node class with name: ' + node_class)
            return result
        except Exception, e:
            return self.handleException(e, 'Failed to access node class "%s" in database'%(node_class,))

    # =====================================================================================================================
    # Assign an existing node class instance in the Classes table to a given node type
    # 
    # \param  node_class         [REQUIRED]  Name of the node class to be accessed
    # \param  node_regex         [REQUIRED]  Name of the node type the class should be assigned to
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def assignClass(self, node_class, regex):
        try:
            self.execStatement("insert into Classes_to_Nodes values ('{0}', '{1}')".format(node_class, regex))
            return SUCCESS        
        except Exception,e:
            return self.handleException(e,'Failed to assign node class "%s" to node type "%s"'%(node_class, regex,))

    # =====================================================================================================================
    # De-Assign an existing node class instance in the Classes table from a given node type
    # 
    # \param  node_class         [REQUIRED]  Name of the node class to be accessed
    # \param  node_regex         [REQUIRED]  Name of the node type the class should be assigned to
    # \return                                Statuscode indicating success or failure / Exception
    #
    # \author  K.Wilczynski
    # \version 1.0
    # ---------------------------------------------------------------------------------------------------------------------
    def unassignClass(self, node_class, regex):
        try:
            cmd = "delete from Classes_to_Nodes where node_class='{0}' and regex='{1}'".format(node_class, regex)
            query = self.execStatement(cmd)
            if(query.rowcount >= 1):
                return SUCCESS
            raise Exception('unassignClass: The specified assignment pair does not exist in the database: ' + node_class + ' <-> ' + regex)
        except Exception,e:
            return self.handleException(e,'Failed to un-assign node class "%s" from node type "%s"'%(node_class, regex,))

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
        message = None
        try:
            query = self.execStatement("SELECT node_class as node_class FROM Classes_to_Nodes WHERE regex='{0}'".format(regex))
            result = [dict(zip(tuple(query.keys()), i)) for i in query.cursor]
            if len(result) > 0:
                return result
            if self.inDb('Nodes',regex=regex):
                return result
            message = 'The node type "%s" does not exist!'%(regex,)
        except Exception, e:
            return self.handleException(e,'Failed to retrieve the node classes for node type "%s"'%(regex,))
        if message:
            raise Exception(message)

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
    def addNode(self, regex, description=''):
        try:
            self.execStatement("insert into Nodes values ('{0}','{1}')".format(regex, description))
            return SUCCESS
        except Exception, e:
            return self.handleException(e,'Failed to add node type "%s"'%(regex,))
    
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
        try:
            cmd = "delete from Nodes where regex='{0}'".format(regex,)
            query = self.execStatement(cmd)
            if(query.rowcount >= 1):
                return SUCCESS
        except Exception, e:
            return self.handleException(e, 'deleteNode: Unknown nodes with regex: ' + regex)
        raise Exception('deleteNode: Failed to delete node with regex: ' + regex)
    
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
        if len(args) == 0:
            raise Exception('No arguments provided, modification impossible')

        statement = "update Nodes set "
        cnt=0
        for key in args:
            if(cnt != 0):
                statement += ", "
            statement += (key + "='" + args[key] + "'")
            cnt += 1

        statement += (" where regex='" + original_regex + "'")
        try:
          query = self.execStatement(statement)
          if(query.rowcount >= 1):
             return SUCCESS
        except Exception, e:
            return self.handleException(e,'Failed to modify node type "%s"'%(regex,))
        raise Exception('modifyNode: Unknown nodes with regex: ' + original_regex)
    
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
        try:
            if regex == '*':
                query = self.execStatement("select * from Nodes")
            else:
                query = self.execStatement("select * from Nodes where regex='{0}'".format(regex,))
            result = [dict(zip(tuple(query.keys()), i)) for i in query.cursor]
            if(len(result) == 0):
                raise Exception('getNode: Unknown node with regex: ' + regex)
            return result
        except Exception, e:
            return self.handleException(e,'Failed to access node type "%s"'%(regex,))

    # =====================================================================================================================
    #                          Helper Methods
    # =====================================================================================================================
    def inDb(self, table, **args):
        if (len(args) == 0):
            logging.warn('Error in api tester inDb method: provided no column: value pairs as search parameters')
            return

        # Construct the SQL statement looking for the entry specified with keys in args in table
        statement = "select * from " + table + " where "
        cnt = 0
        for key in args:
            if(cnt != 0):
                statement += ' and '
            statement += (key + "='" + args[key] + "'")
            cnt += 1

        # Execute the statement
        result = self.connection.handle.execute(statement)

        # Check if the query returned the same object, return true if it did (result array will have size of > 1 - at least 1 result with specified params should be found - in case of PK)
        arr = []
        for row in result:
            arr.append(row)
        if len(arr) == 0:
            return False
        else:
            return True