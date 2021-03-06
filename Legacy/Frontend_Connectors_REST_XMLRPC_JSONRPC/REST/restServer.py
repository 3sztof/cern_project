#!/usr/bin/env python


# =====================================================================================================================
#             LHCb Online Farm Task Explorer GUI REST API for the Sqlite Database
#                                   K.Wilczynski 08.2018
# =====================================================================================================================



# =====================================================================================================================
#                                           Init
# =====================================================================================================================

from flask import Flask, request
from flask_restful import Resource, Api
from sqlalchemy import create_engine                                         
from json import dumps
import os
from flask_jsonpify import jsonify

app = Flask(__name__)
api = Api(app)
my_path = os.path.dirname(os.path.realpath(__file__))
_database = my_path + os.sep + '../../../0_main_api/LHCb.db'
db_connect = create_engine('sqlite:///' + _database)
conn = db_connect.connect()
conn.execute("PRAGMA foreign_keys = ON")

# =====================================================================================================================
#                           Define server's actions triggered by requests
# =====================================================================================================================

# Tasks
class Tasks(Resource):
    def get(self):
        conn = db_connect.connect()  # connect to database
        query = conn.execute("select * from Tasks")  # This line performs query and returns json result
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        #return {'Tasks': [i[0] for i in query.cursor.fetchall()]}  # Fetches first column that is Employee ID
        return jsonify(result)
    def post(self):
        conn = db_connect.connect()
        print(request.json)                                                     # Working! :D
        task = request.json['task']
        utgid = request.json['utgid']
        command = request.json['command']
        task_parameters = request.json['task_parameters']
        command_parameters = request.json['command_parameters']
        description = request.json['description']
        query = conn.execute("insert into Tasks values('{0}','{1}','{2}','{3}', \
                             '{4}','{5}')".format(task,utgid,command,
                             task_parameters, command_parameters, description))
        return {'status':'success'}
class Specific_Task(Resource):
    def get(self, task_name):
        conn = db_connect.connect()
        query = conn.execute("select * from Tasks where task=\'%s\' " % str(task_name))
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        return jsonify(result)
    def delete(self, task_name):
        conn = db_connect.connect()
        query = conn.execute("delete from Tasks where task=\'%s\' " % str(task_name))
        #result = 'Deleted'#{'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        return {'status':'success'}
    def patch(self, task_name):
        conn = db_connect.connect()
        print(request.json)                                                     # Working! :D
        task = request.json['task']
        utgid = request.json['utgid']
        command = request.json['command']
        task_parameters = request.json['task_parameters']
        command_parameters = request.json['command_parameters']
        description = request.json['description']
        query = conn.execute("update Tasks set task=\'{1}\', utgid=\'{2}\', command=\'{3}\', task_parameters=\'{4}\', command_parameters=\'{5}\'description=\'{6}\' where task_set = '{0}'".format(task_name,task,utgid,command,task_parameters,command_parameters,description))
        return {'status':'success'}

# Task sets
class TaskSets(Resource):
    def get(self):
        conn = db_connect.connect()  # connect to database
        query = conn.execute("select * from Task_Sets")  # This line performs query and returns json result
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        #return {'Tasks': [i[0] for i in query.cursor.fetchall()]}  # Fetches first column that is Employee ID
        return jsonify(result)
    def post(self):
        conn = db_connect.connect()
        print(request.json)                                                     # Working! :D
        task_set = request.json['task_set']
        description = request.json['description']
        query = conn.execute("insert into Task_Sets values('{0}','{1}')".format(task_set,description))
        return {'status':'success'}
class Specific_TaskSet(Resource):
    def get(self, set_name):
        conn = db_connect.connect()
        query = conn.execute("select * from Task_Sets where task_set=\'%s\' " % str(set_name))
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        return jsonify(result)
    def patch(self, set_name):
        conn = db_connect.connect()
        print(request.json)                                                     # Working! :D
        task_set = request.json['task_set']
        description = request.json['description']
        query = conn.execute("update Task_Sets set task_set=\'{1}\', description=\'{2}\' where task_set = '{0}'".format(set_name,task_set,description))
        return {'status':'success'}

# Node classes
class Classes(Resource):
    def get(self):
        conn = db_connect.connect()  # connect to database
        query = conn.execute("select * from Classes")  # This line performs query and returns json result
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        #return {'Tasks': [i[0] for i in query.cursor.fetchall()]}  # Fetches first column that is Employee ID
        return jsonify(result)
    def post(self):
        conn = db_connect.connect()
        print(request.json)                                                     # Working! :D
        node_class = request.json['node_class']
        description = request.json['description']
        query = conn.execute("insert into Classes values('{0}','{1}')".format(node_class,description))
        return {'status':'success'}
class Specific_Class(Resource):
    def get(self, class_name):
        conn = db_connect.connect()
        query = conn.execute("select * from Classes where class=\'%s\' " % str(class_name))
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        return jsonify(result)
    def patch(self, class_name):
        conn = db_connect.connect()
        print(request.json)                                                     # Working! :D
        class_mod = request.json['class']
        description = request.json['description']
        query = conn.execute("update Classes set class=\'{1}\', description=\'{2}\' where class = '{0}'".format(class_name,class_mod,description))
        return {'status':'success'}

# Nodes
class Nodes(Resource):
    def get(self):
        conn = db_connect.connect()  # connect to database
        query = conn.execute("select * from Nodes")  # This line performs query and returns json result
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        #return {'Tasks': [i[0] for i in query.cursor.fetchall()]}  # Fetches first column that is Employee ID
        return jsonify(result)
    def post(self):
        conn = db_connect.connect()
        print(request.json)                                                     # Working! :D
        node = request.json['regex']
        description = request.json['description']
        query = conn.execute("insert into Nodes values('{0}','{1}')".format(node,description))
        return {'status':'success'}
class Specific_Node(Resource):
    def get(self, node_name):
        conn = db_connect.connect()
        query = conn.execute("select * from Nodes where regex=\'%s\' " % str(node_name))
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        return jsonify(result)
    def patch(self, node_name):
        conn = db_connect.connect()
        print(request.json)                                                     # Working! :D
        node = request.json['regex']
        description = request.json['description']
        query = conn.execute("update Nodes set regex=\'{1}\', description=\'{2}\' where regex = '{0}'".format(node_name,node,description))
        return {'status':'success'}

# =====================================================================================================================
#                                       Serve the API on localhost
# =====================================================================================================================

# Tasks
api.add_resource(Tasks, '/tasks')                               # Get, Post 
api.add_resource(Specific_Task, '/tasks/<task_name>')           # Get, Delete, Patch

# Task Sets
api.add_resource(TaskSets, '/task_sets')                        # Get, Post 
api.add_resource(Specific_TaskSet, '/task_sets/<set_name>')     # Get, Delete, Patch

# Node Classes
api.add_resource(Classes, '/node_classes')                      # Get, Post 
api.add_resource(Specific_Class, '/node_classes/<class_name>')  # Get, Delete, Patch

# Nodes
api.add_resource(Nodes, '/nodes')                               # Get, Post 
api.add_resource(Specific_Node, '/nodes/<node_name>')           # Get, Delete, Patch

if __name__ == '__main__':
    app.run(port='80')

# Achtung! In Chrome tests I ran into CORS issue: an app is forbidden to access the resources from other ports, the plugin linked below fixes it by allowing foreign ajax requests:
# https://chrome.google.com/webstore/detail/allow-control-allow-origi/nlfbmbojpeacfghkpbjhddihlkkiljbi/related?utm_source=chrome-app-launcher-info-dialog










































# Examples from the original code:
""" 
class Tracks(Resource):
    def get(self):
        conn = db_connect.connect()
        query = conn.execute("select trackid, name, composer, unitprice from tracks;")
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        return jsonify(result)


class Employees_Name(Resource):
    def get(self, employee_id):
        conn = db_connect.connect()
        query = conn.execute("select * from employees where EmployeeId =%d " % int(employee_id))
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        return jsonify(result)

api.add_resource(Tracks, '/tracks')  # Route_2
api.add_resource(Employees_Name, '/employees/<employee_id>')  # Route_3

"""
