import websocketrpclib, logging
logging.basicConfig(format='%(threadName)-12s %(levelname)s: %(message)s', level=logging.INFO)

s1=websocketrpclib.XmlServer("ws://localhost:8081/TDBDATA/XMLRPC",verbose=0)
s2=websocketrpclib.JsonServer("ws://localhost:8081/TDBDATA/JSONRPC",verbose=0)

for i in xrange(3):
    tasks = s1.getTask('*')
    print '************************************', s1.uri
    print tasks
    tasks = s2.getTask('*')
    print '************************************', s2.uri
    print tasks
