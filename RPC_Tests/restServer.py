#!/usr/bin/env python


from flask import Flask, request
from flask_restful import Resource, Api
from sqlalchemy import create_engine
from json import dumps
from flask_jsonpify import jsonify

db_connect = create_engine('sqlite:///../LHCb.db')
app = Flask(__name__)
api = Api(app)


class Tasks(Resource):
    def get(self):
        conn = db_connect.connect()  # connect to database
        query = conn.execute("select * from Tasks")  # This line performs query and returns json result
        result = {'data': [dict(zip(tuple(query.keys()), i)) for i in query.cursor]}
        #return {'Tasks': [i[0] for i in query.cursor.fetchall()]}  # Fetches first column that is Employee ID
        return jsonify(result)




api.add_resource(Tasks, '/tasks')  # Route_1


if __name__ == '__main__':
    app.run(port='80')

# Achtung! In Chrome tests I ran intu CORS issue: an app is forbidden to access the resources from other ports, the plugin linked below fixes it by allowing foreign ajax requests:
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
