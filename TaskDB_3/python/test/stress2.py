# Stress test:
import time, TaskDB

def stress1(count):
  rpc   = TaskDB.connect('jsonrpc', 'http://localhost:8082/TDBDATA/JSONRPC')
  start = time.time()
  last  = start
  res = ''
  try:
    farms = ['a','b','c','d','e','f','g','m']
    i = 0
    for j in xrange(count):
      for f in farms:
        for row in xrange(15):
          for m in xrange(32):
            name = 'hlt%s%02d%02d'%(f,row+1,m+1)
            res = rpc.getTasksByNode(name)
            i = i + 1
            if i > 0 and i%1000 == 0:
              now  = time.time()
              diff = now-start
              frq1 = float(i)/diff
              frq2 = float(1000)/(now-last)
              last = now
              print ('Processed %9d requests %8.2f seconds  '+\
                     'Instantaneous: %5.0f Hz Integrated: %5.0f Hz')%(i,diff,frq2,frq1,)
  except Exception, X:
    print 'Exception:', X, str(res)


if __name__ == "__main__":
  stress1(10000)
