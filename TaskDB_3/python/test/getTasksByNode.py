# Stress test:
import re, time, TaskDB
from TaskDB.tools import *

class Test:
  def __init__(self, opts):
    self.opts = opts
    self.rpc = TaskDB.connect('jsonrpc', 'http://localhost:8082/TDBDATA/JSONRPC')
    self.replacements = {}
    self.replacements['<TOP_LOGGER>'] = 'ecs03'
    self.replacements['<SCRIPTS>'] = '/group/online/dataflow/scripts'
    
  def log(self, m):
    print 5*'=', m

  def printTask(self, task):
    self.log('Task:%-30s UTGID:%-30s %s'%(task.name(), task.utgid(), task.command(),))

  def printCommand(self, task):
    cmd = 'pcAdd '+task.params()+' -u '+task.utgid()+' '+task.command() + ' ' + task.cmd_params()
    if self.opts.dns:
      cmd = cmd.replace('<DIM_DNS>',self.opts.dns)
    if self.opts.node:
      cmd = cmd.replace('<NODE>',self.opts.node)
    for k,v in self.replacements.items():
      cmd = cmd.replace(k,v)
    print cmd

  def tasksByNode(self, node, dns):
    tasks = self.rpc.getTasksByNode(node)
    for t in tasks:
        task = Task(t)
        if opts.printout: self.printTask(task)
        if opts.commands: self.printCommand(task)     

if __name__ == "__main__":
  import optparse
  parser = optparse.OptionParser()
  parser.description = "TaskDB command line interface."
  parser.formatter.width = 132
  parser.add_option("-m", "--node", 
                    dest="node", default=None,
                    help="Specify node name",
                    metavar="<string>")
  parser.add_option("-N", "--dns", 
                    dest="dns", default=None,
                    help="Specify DIM DNS node name",
                    metavar="<string>")
  parser.add_option("-p", "--print", 
                    dest="printout", default=False,
                    action='store_true',
                    help="Require pretty print of output",
                    metavar="<boolean>")

  parser.add_option("-c", "--commands", 
                    dest="commands", default=False,
                    action='store_true',
                    help="Print pcAdd commands",
                    metavar="<boolean>")

  (opts, args) = parser.parse_args()
  #import pdb; pdb.set_trace()
  test = Test(opts);
  test.tasksByNode(opts.node, opts.dns)
