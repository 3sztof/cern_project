#!/usr/bin/python
import sys, os, time, asyncore, websocket    

global ws
ws = None

def on_open():
  ws.send(sys.argv[1])

def on_close():
  sys.stdout.write('Connection closed.\n')

def on_message(m):
  sys.stdout.write('Echo: %s\n' % m),

if __name__ == '__main__':
  if len(sys.argv) != 2:
      sys.stderr.write('usage: %s <message>\n' % os.path.abspath(__file__))
      sys.exit(1)
  import pdb
  #pdb.set_trace()
  ws = websocket.WebSocket('ws://localhost:8081/WebSocket', 
                           onopen=on_open,
                           onmessage=on_message,
                           onclose=on_close)
    
  print 'request sent'
  #while 1: time.sleep(1)
    
  try:
      asyncore.loop()
  except KeyboardInterrupt:
      ws.close()
