import threading
import time, sys
import logging
from collections import deque
logging.basicConfig(level=logging.DEBUG,
                    format='(%(threadName)-9s) %(message)s',)
data = 1
queue = []
def consumer(cv,l):
    logging.debug('Consumer thread started ...')
    while 1:
              l.acquire()
              length = len(queue)
              if not length:
                  l.release()
                  time.sleep(1e-6)
              else:
                item = queue.pop()
                l.release()
                #logging.debug('Consumer consumed the resource: '+str(item)+' ['+str(length-1)+']')
                sys.stdout.write('.')
        
def producer(cv,l):
    global data
    ll = Lock()
    while 1:
          l.acquire()
          if len(queue)<20:
            data = data + 1
            queue.insert(0,data)
            l.release()
            #logging.debug('Notify the consumer '+str(data))
            #cv.notify()
          else:
            l.release()
            #sys.stdout.write('+')
            time.sleep(1e-5)


if __name__ == '__main__':
    lock = threading.Lock()
    condition = threading.Condition()
    cs = threading.Thread(name='consumer1', target=consumer, args=(condition,lock,))
    prod = []
    for i in xrange(20):
        pd = threading.Thread(name='producer_'+str(i), target=producer, args=(condition,lock))
        prod.append(pd)

    cs.start()
    time.sleep(1)
    for p in prod:
        p.start()
