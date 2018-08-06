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
from flask_jsonpify import jsonify

db_connect = create_engine('sqlite:///../LHCb.db')
app = Flask(__name__)
api = Api(app)

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
class Specific_Task(Resource):
    def get(self, task_name):
        conn = db_connect.connect()
        query = conn.execute("select * from Tasks where task=\'%s\' " % str(task_name))
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        return jsonify(result)

# Task sets
class TaskSets(Resource):
    def get(self):
        conn = db_connect.connect()  # connect to database
        query = conn.execute("select * from Task_Sets")  # This line performs query and returns json result
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        #return {'Tasks': [i[0] for i in query.cursor.fetchall()]}  # Fetches first column that is Employee ID
        return jsonify(result)
class Specific_TaskSet(Resource):
    def get(self, set_name):
        conn = db_connect.connect()
        query = conn.execute("select * from Task_Sets where task_set=\'%s\' " % str(set_name))
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        return jsonify(result)

# Node classes
class Classes(Resource):
    def get(self):
        conn = db_connect.connect()  # connect to database
        query = conn.execute("select * from Classes")  # This line performs query and returns json result
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        #return {'Tasks': [i[0] for i in query.cursor.fetchall()]}  # Fetches first column that is Employee ID
        return jsonify(result)
class Specific_Class(Resource):
    def get(self, class_name):
        conn = db_connect.connect()
        query = conn.execute("select * from Classes where class=\'%s\' " % str(class_name))
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        return jsonify(result)

# Nodes
class Nodes(Resource):
    def get(self):
        conn = db_connect.connect()  # connect to database
        query = conn.execute("select * from Nodes")  # This line performs query and returns json result
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        #return {'Tasks': [i[0] for i in query.cursor.fetchall()]}  # Fetches first column that is Employee ID
        return jsonify(result)
class Specific_Node(Resource):
    def get(self, node_name):
        conn = db_connect.connect()
        query = conn.execute("select * from Nodes where regex=\'%s\' " % str(node_name))
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        return jsonify(result)

# =====================================================================================================================
#                                       Serve the API on localhost
# =====================================================================================================================

# Tasks
api.add_resource(Tasks, '/tasks')                       # Get
api.add_resource(Specific_Task, '/tasks/<task_name>')

# Task Sets
api.add_resource(TaskSets, '/task_sets')                # Get
api.add_resource(Specific_TaskSet, '/task_sets/<set_name>')

# Node Classes
api.add_resource(Classes, '/node_classes')              # Get
api.add_resource(Specific_Class, '/node_classes/<class_name>')

# Nodes
api.add_resource(Nodes, '/nodes')                       # Get
api.add_resource(Specific_Node, '/nodes/<node_name>')

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
