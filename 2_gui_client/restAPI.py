#!/usr/bin/env python

import sys, os, re, shutil, json, urllib, urllib2, BaseHTTPServer, mainAPI

# =====================================================================================================================
#                      Mother object - singleton, to maintain only one database connection
# =====================================================================================================================

class Singleton:

    def __init__(self, decorated):
        self._decorated = decorated

    def instance(self):

        try:
            return self._instance
        except AttributeError:
            self._instance = self._decorated()
            return self._instance

    def __call__(self):
        raise TypeError('Singletons must be accessed through `instance()`.')

    def __instancecheck__(self, inst):
        return isinstance(inst, self._decorated)

@Singleton
class ApiSingleton:
   def __init__(self):
        self.api = mainAPI.MainAPI()
        print 'API Singleton created'

reload(sys)
sys.setdefaultencoding('utf8')

here = os.path.dirname(os.path.realpath(__file__))

# =====================================================================================================================
#                                           GET
# =====================================================================================================================

def get_tasks(handler):
    result = ApiSingleton.instance().api.getTask('*')
    result = json.loads(result)
    return result

def get_task(handler):
    key = urllib.unquote(handler.path[6:])
    result = ApiSingleton.instance().api.getTask(key)
    result = json.loads(result)
    return result

def get_sets(handler):
    result = ApiSingleton.instance().api.getSet('*')
    result = json.loads(result)
    return result

def get_set(handler):
    key = urllib.unquote(handler.path[10:])
    result = ApiSingleton.instance().api.getSet(key)
    result = json.loads(result)
    return result

def get_classes(handler):
    result = ApiSingleton.instance().api.getClass('*')
    result = json.loads(result)
    return result

def get_class(handler):
    key = urllib.unquote(handler.path[12:])
    result = ApiSingleton.instance().api.getClass(key)
    result = json.loads(result)
    return result

def get_nodes(handler):
    result = ApiSingleton.instance().api.getNode('*')
    result = json.loads(result)
    return result

def get_node(handler):
    key = urllib.unquote(handler.path[6:])
    result = ApiSingleton.instance().api.getNode(key)
    result = json.loads(result)
    return result

# =====================================================================================================================
#                                           PUT
# =====================================================================================================================

def put_task(handler):
    payload = handler.get_payload()
    task = payload['task'] if 'task' in payload else ''
    description = payload['description'] if 'description' in payload else ''
    utgid = payload['utgid'] if 'utgid' in payload else ''
    command = payload['command'] if 'command' in payload else ''
    task_parameters = payload['task_parameters'] if 'task_parameters' in payload else ''
    command_parameters = payload['command_parameters'] if 'command_parameters' in payload else ''
    result = ApiSingleton.instance().api.addTask(
            task = task,
            description = description,
            utgid = utgid,
            command = command,
            task_parameters = task_parameters,
            command_parameters = command_parameters
        )
    return result

def put_set(handler):
    payload = handler.get_payload()
    task_set = payload['task_set'] if 'task_set' in payload else ''
    description = payload['description'] if 'description' in payload else ''
    result = ApiSingleton.instance().api.addSet(
            task_set = task_set, 
            description = description
        )
    return result

def put_class(handler):
    payload = handler.get_payload()
    node_class = payload['node_class'] if 'node_class' in payload else ''
    description = payload['description'] if 'description' in payload else ''
    result = ApiSingleton.instance().api.addClass(
            node_class = node_class, 
            description = description
        )
    return result

def put_node(handler):
    payload = handler.get_payload()
    regex = payload['regex'] if 'regex' in payload else ''
    description = payload['description'] if 'description' in payload else ''
    result = ApiSingleton.instance().api.addNode(
        regex = regex, 
        description = description
        )
    return result

# =====================================================================================================================
#                                          DELETE
# =====================================================================================================================

def delete_task(handler):
    key = urllib.unquote(handler.path[6:])
    result = ApiSingleton.instance().api.deleteTask(key)
    return result

def delete_set(handler):
    key = urllib.unquote(handler.path[10:])
    result = ApiSingleton.instance().api.deleteSet(key)
    return result

def delete_class(handler):
    key = urllib.unquote(handler.path[12:])
    result = ApiSingleton.instance().api.deleteClass(key)
    return result

def delete_node(handler):
    key = urllib.unquote(handler.path[6:])
    result = ApiSingleton.instance().api.deleteNode(key)
    return result

# =====================================================================================================================
#                                   POST (Patch - Update)
# =====================================================================================================================

def modify_task(handler):
    key = urllib.unquote(handler.path[6:])
    payload = handler.get_payload()
    task = payload['task'] if 'task' in payload else ''
    description = payload['description'] if 'description' in payload else ''
    utgid = payload['utgid'] if 'utgid' in payload else ''
    command = payload['command'] if 'command' in payload else ''
    task_parameters = payload['task_parameters'] if 'task_parameters' in payload else ''
    command_parameters = payload['command_parameters'] if 'command_parameters' in payload else ''
    result = ApiSingleton.instance().api.modifyTask(
            key,
            task = task,
            description = description,
            utgid = utgid,
            command = command,
            task_parameters = task_parameters,
            command_parameters = command_parameters
        )
    return result

def modify_set(handler):
    key = urllib.unquote(handler.path[10:])
    payload = handler.get_payload()
    task_set = payload['task_set'] if 'task_set' in payload else ''
    description = payload['description'] if 'description' in payload else ''
    result = ApiSingleton.instance().api.modifySet(
            key,
            task_set = task_set, 
            description = description
        )
    return result

def modify_class(handler):
    key = urllib.unquote(handler.path[12:])
    payload = handler.get_payload()
    node_class = payload['node_class'] if 'node_class' in payload else ''
    description = payload['description'] if 'description' in payload else ''
    result = ApiSingleton.instance().api.modifyClass(
            key,
            node_class = node_class, 
            description = description
        )
    return result

def modify_node(handler):
    key = urllib.unquote(handler.path[6:])
    payload = handler.get_payload()
    regex = payload['regex'] if 'regex' in payload else ''
    description = payload['description'] if 'description' in payload else ''
    result = ApiSingleton.instance().api.modifyNode(
        key,
        regex = regex, 
        description = description
        )
    return result

# =====================================================================================================================

def rest_call_json(url, payload=None, with_payload_method='PUT'):
    'REST call with JSON decoding of the response and JSON payloads'
    if payload:
        if not isinstance(payload, basestring):
            payload = json.dumps(payload)
        # PUT or POST
        response = urllib2.urlopen(MethodRequest(url, payload, {'Content-Type': 'application/json'}, method=with_payload_method))
    else:
        # GET
        response = urllib2.urlopen(url)
    response = response.read().decode()
    return json.loads(response)

class MethodRequest(urllib2.Request):
    'See: https://gist.github.com/logic/2715756'
    def __init__(self, *args, **kwargs):
        if 'method' in kwargs:
            self._method = kwargs['method']
            del kwargs['method']
        else:
            self._method = None
        return urllib2.Request.__init__(self, *args, **kwargs)

    def get_method(self, *args, **kwargs):
        return self._method if self._method is not None else urllib2.Request.get_method(self, *args, **kwargs)

class RESTRequestHandler(BaseHTTPServer.BaseHTTPRequestHandler):

    def __init__(self, *args, **kwargs):

        self.routes = {

                r'^/tasks$': {'GET': get_tasks, 'media_type': 'application/json'},
                r'^/task/': {'GET': get_task, 'PUT': put_task, 'DELETE': delete_task, 'POST': modify_task, 'media_type': 'application/json'},

                r'^/task_sets$': {'GET': get_sets, 'media_type': 'application/json'},
                r'^/task_set/': {'GET': get_set, 'PUT': put_set, 'DELETE': delete_set, 'POST': modify_set, 'media_type': 'application/json'},

                r'^/node_classes$': {'GET': get_classes, 'media_type': 'application/json'},
                r'^/node_class/': {'GET': get_class, 'DELETE': delete_class, 'POST': modify_class, 'media_type': 'application/json'},

                r'^/nodes$': {'GET': get_nodes, 'media_type': 'application/json'},
                r'^/node/': {'GET': get_node, 'DELETE': delete_node, 'POST': modify_node, 'media_type': 'application/json'}

            }

        return BaseHTTPServer.BaseHTTPRequestHandler.__init__(self, *args, **kwargs)
    
    def do_HEAD(self):
        self.handle_method('HEAD')
    
    def do_GET(self):
        self.handle_method('GET')

    def do_POST(self):
        self.handle_method('POST')

    def do_PUT(self):
        self.handle_method('PUT')

    def do_DELETE(self):
        self.handle_method('DELETE')
    
    def get_payload(self):
        payload_len = int(self.headers.getheader('content-length', 0))
        payload = self.rfile.read(payload_len)
        payload = json.loads(payload)
        return payload
        
    def handle_method(self, method):
        route = self.get_route()
        if route is None:
            self.send_response(404)
            self.end_headers()
            self.wfile.write('Route not found\n')
        else:
            if method == 'HEAD':
                self.send_response(200)
                if 'media_type' in route:
                    self.send_header('Content-type', route['media_type'])
                self.end_headers()
            else:
                if 'file' in route:
                    if method == 'GET':
                        try:
                            f = open(os.path.join(here, route['file']))
                            try:
                                self.send_response(200)
                                if 'media_type' in route:
                                    self.send_header('Content-type', route['media_type'])
                                self.end_headers()
                                shutil.copyfileobj(f, self.wfile)
                            finally:
                                f.close()
                        except:
                            self.send_response(404)
                            self.end_headers()
                            self.wfile.write('File not found\n')
                    else:
                        self.send_response(405)
                        self.end_headers()
                        self.wfile.write('Only GET is supported\n')
                else:
                    if method in route:
                        content = route[method](self)
                        if content is not None:
                            self.send_response(200)
                            if 'media_type' in route:
                                self.send_header('Content-type', route['media_type'])
                            self.end_headers()
                            if method != 'DELETE':
                                self.wfile.write(json.dumps(content))
                        else:
                            self.send_response(404)
                            self.end_headers()
                            self.wfile.write('Not found\n')
                    else:
                        self.send_response(405)
                        self.end_headers()
                        self.wfile.write(method + ' is not supported\n')
                    
    
    def get_route(self):
        for path, route in self.routes.iteritems():
            if re.match(path, self.path):
                return route
        return None

def rest_server(port):
    'Starts the REST server'
    http_server = BaseHTTPServer.HTTPServer(('', port), RESTRequestHandler)
    print 'Starting HTTP server at port %d' % port
    try:
        http_server.serve_forever()
    except KeyboardInterrupt:
        pass
    print 'Stopping HTTP server'
    http_server.server_close()

def main(argv):
    rest_server(1841)

if __name__ == '__main__':
    main(sys.argv[1:])