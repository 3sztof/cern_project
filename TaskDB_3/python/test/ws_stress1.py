# Stress test:
import time, TaskDB
import websocket

def stress1(count):
  #rpc   = TaskDB.connect('jsonrpc', 'http://localhost:8081/TDBDATA/JSONRPC')
  rpc   = websocket.create_connection("ws://localhost:8081/WebSocket")
  #check = str(rpc.getTask('testTask0'))
  rpc.send('{"jsonrpc": "2.0", "id": 0, "method": "getTask", "params": [{"task":"testTask0"}] }')
  check = rpc.recv()
  print 'Check result:', check
  start = time.time()
  last  = start
  for i in xrange(count):
    try:
      #res = rpc.getTask('testTask0')
      rpc.send('{"jsonrpc": "2.0", "id": 0, "method": "getTask", "params": [{"task":"testTask0"}] }')
      res = rpc.recv()
      if str(res) != check:
        print 'Error:',i,res
      if i > 0 and i%1000 == 0:
        now  = time.time()
        diff = now-start
        frq1 = float(i)/diff
        frq2 = float(1000)/(now-last)
        last = now
        print ('Processed %9d requests %8.2f seconds  '+\
               'Instantaneous: %5.0f Hz Integrated: %5.0f Hz')%(i,diff,frq2,frq1,)
    except Exception, X:
      print 'Exception:', i, str(res)


if __name__ == "__main__":
  stress1(1000000)
