#!/usr/bin/env python

import requests
import json
import os

def send(method, parameters):
    url = "http://localhost:4000/jsonrpc"
    headers = {'content-type': 'application/json'}

    payload = {
        "method": method,
        "params": [parameters],
        "jsonrpc": "2.0",
        "id": 0,
    }
    response = requests.post(
        url, data=json.dumps(payload), headers=headers).json()

    #assert response["result"] == "echome!"
    #assert response["jsonrpc"]
    #assert response["id"] == 0


    print 'Response: ' + os.linesep
    print json.dumps(response, indent=4, sort_keys=True)

if __name__ == "__main__":
    send()