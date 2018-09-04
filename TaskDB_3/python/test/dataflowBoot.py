import os
import re
import sys
import time
import socket
logDefaultFIFO  = '/run/fmc/logSrv.fifo'
onlineScripts   = '<SCRIPTS>'
default_exports = 'export PATH=/opt/FMC/sbin:/opt/FMC/bin:$PATH;export LD_LIBRARY_PATH=/opt/FMC/lib;'
top_logger_host = '<TOP_LOGGER>'
_scripts = onlineScripts + os.sep


use_tan = 1
use_log = 1
use_mbm = 1
use_pub = 1
stop_opts = ' --no-authentication '
start_opts = ' -e -o --no-authentication '

import inspect
import traceback

global dim_dns_node
dim_dns_node = ''

class TaskSet:
  def __init__(self, name, opts=None):
    self.name = name
    self.opts = opts
    self.tasks = []
  def addTask(self, name, cmd):
    self.tasks.append([name,cmd])
    

global all_taskset
all_taskset = {}

global current_taskset
current_taskset = None


def get_function_name():
    func = traceback.extract_stack(None, 2)[0][2]
    if func[:5] == 'start': func = func[5:]
    return func

def get_function_parameters_and_values():
    frame = inspect.currentframe().f_back
    args, _, _, values = inspect.getargvalues(frame)
    return ([(i, values[i]) for i in args])

    #print 'Running ' + get_function_name() + '(' + str(get_function_parameters_and_values()) +')'

def add_task(h, u='online', g='onliners', cmd='', fifo=None, X=300, M=5, Nice=0, name=None):
    log_opts = '-E '+logDefaultFIFO+' -O '+logDefaultFIFO
    if fifo:
        log_opts = '-E ' + str(fifo) + ' -O ' + str(fifo)
    cc = cmd.split()
    utgid = cc[0]
    command = cc[1]
    pars = cc[2:]
    params=''
    for p in pars:
        if len(params): params = params + ' '
        params = params + p

    if not len(params): params = '\'\''
    if params[0] != '"' and params[0] != '\'': params = '\'' + params + '\''
    if not name: name = utgid
    cmd = '  server.addTask({\'task\': \''+name+'\',\\\n'+\
          '                  \'utgid\': \''+utgid+'\',\\\n'+\
          '                  \'command\': \''+command+'\',\\\n'+\
          '                  \'task_parameters\': \'--no-authentication -K 120 -M ' + str(M) + ' -X ' + str(X) + \
          ' -m ' + h + ' ' + log_opts + ' -g ' + g + ' -p ' + str(Nice) + ' -n ' + u + '\',\\\n'+\
          '                  \'command_parameters\': '+params+', \'description\': \'\'})\n'
    current_taskset.addTask(name,cmd)
    return cmd


def add_task_root(h, cmd, fifo=None, X=300, M=5, name=None):
    return add_task(h, 'root', 'root', cmd=cmd, fifo=fifo, X=X, M=M, name=name)


def setupEnv():
    #print default_exports
    pass

def start_group(func, args):
    global current_taskset
    name = func
    if func[:5] == 'start': name = func[5:]
    current_taskset = TaskSet(name, args)
    all_taskset[name] = current_taskset

def end_group(func, args):
    global current_taskset
    global dim_dns_node 
    dim_dns_node = ''
    name = func
    if func[:5] == 'start': name = func[5:]
    ts = all_taskset[name]
    if len(ts.tasks)>0:
        print '  server.addSet({"task_set":"'+ts.name+'", "description": ""})    ### '+args
        for t in ts.tasks:
            print '  server.assignTask("'+t[0]+'", "'+ts.name+'")'
    next = traceback.extract_stack(None, 3)[0][2]
    if next[:5] == 'start': next = next[5:]
    if all_taskset.has_key(next):
        current_taskset = all_taskset[next]

def end_node_type(name, match):
    global all_taskset
    print '  server.addClass({"node_class": "'+name+'", "description": ""})'
    for n,s in all_taskset.items():
        if len(s.tasks) > 0:
            print '  server.assignSet("'+n+'", "'+name+'")'

    print '  server.addNode({"regex": "'+match+'", "description": ""})'
    print '  server.assignClass("'+name+'","'+match+'")'
    all_taskset = {}
    start_group('Tasks','None')


def startFMC(host):
    start_group(get_function_name(),str(get_function_parameters_and_values()))
    end_group(get_function_name(),str(get_function_parameters_and_values()))

def startInfrastructure(host, controlsPC, next_level_server=None, gaudilog=1, group=None):
    if group == None:
        grp   = ''
        group = get_function_name()
    else:
        grp   = '_'+group
        group = 'InfraStructure_'+group
    start_group(group+grp,str(get_function_parameters_and_values()))
    print add_task(host, name='LogDefaultSrv'+grp,  cmd='LogDefaultSrv ' + _scripts + 'LogServer.sh ' + controlsPC + ' -S 100 -l 1 -s default -p /tmp/logSrv.fifo')
    if use_log is not None and gaudilog is not None:
        print add_task(host, name='LogGaudiSrv'+grp, cmd='LogGaudiSrv ' + _scripts + 'LogServer.sh ' + controlsPC + ' -S 100 -l 1 -s gaudi -p /run/fmc/logGaudi.fifo')
        print add_task(host, name='LogLHCb1Srv'+grp, cmd='LogLHCb1Srv ' + _scripts + 'LogServer.sh ' + controlsPC + ' -S 100 -l 1 -s lhcb1 -p /run/fmc/logLHCb1.fifo')
        print add_task(host, name='LogLHCb2Srv'+grp, cmd='LogLHCb2Srv ' + _scripts + 'LogServer.sh ' + controlsPC + ' -S 100 -l 1 -s lhcb2 -p /run/fmc/logLHCb2.fifo')
        print add_task(host, name='LogLHCbASrv'+grp, cmd='LogLHCbASrv ' + _scripts + 'LogServer.sh ' + controlsPC + ' -S 100 -l 1 -s lhcba -p /run/fmc/logLHCbA.fifo')
    print add_task(host,      cmd='TANServ   ' + _scripts + 'TanServ.sh')
    print add_task(host,      cmd='pingSrv_u ' + _scripts + 'PingSrv.sh')
    print add_task_root(host, cmd='ROcollect ' + _scripts + 'ROMONCollect.sh')
    print add_task(host, u='beat',g='onliners',cmd=' SSSRecover /home/beat/ssscheck.sh', Nice=-15)
    end_group(group+grp,str(get_function_parameters_and_values()))

#------------------------------------------------------------------------------

def startSliceLogs(host, controlsPC, slice, num=16):
    "Start slice dependent log message tasks on a storage or Monitoring controls PC"
    pref = '_'+slice[:4].upper()
    start_group(get_function_name()+pref,str(get_function_parameters_and_values()))
    for i in xrange(num):
        sl = '%s_Slice%02X' % (slice, i)
        print add_task(host, cmd='%sSrv %sLogServer.sh %s -S 100 -l 1 -s %s -p /run/fmc/%s.fifo' % (sl, _scripts, controlsPC, sl, sl))
    end_group(get_function_name()+pref,str(get_function_parameters_and_values()))

#------------------------------------------------------------------------------

def startSliceCollectors(host, next_level_server, slice, num=16):
    pref = '_'+slice[:4].upper()
    start_group(get_function_name()+pref,str(get_function_parameters_and_values()))
    for i in xrange(num):
        sl = '%s_Slice%02X' % (slice, i)
        print add_task(host, cmd='%sSumSrv %sLogServer.sh "%s -S 100 -l 1 -s %s -p /run/fmc/%ssum.fifo"' % (sl, _scripts, next_level_server, sl, sl))
        print add_task(host, cmd='%sCollect %sLogCollect.sh "-l 1 -s %s -o /run/fmc/%ssum.fifo"' % (sl, _scripts, sl, sl))
    end_group(get_function_name()+pref,str(get_function_parameters_and_values()))

#------------------------------------------------------------------------------

def startSliceNodeTasks(host, controlsPC, slice, num=16):
    start_group(get_function_name(),str(get_function_parameters_and_values()))
    startInfrastructure(host, controlsPC, gaudilog=2)
    startSliceLogs(host, controlsPC, slice, num)
    end_group(get_function_name(),str(get_function_parameters_and_values()))

#------------------------------------------------------------------------------

def startGaudiControlsLogs(host, next_level_server, gaudi=1, LHCb1=1, LHCb2=1, LHCbA=1,group=None):
    if group == None:
        grp   = ''
        group = get_function_name()
    else:
        grp   = '_'+group
        group = get_function_name()+'_'+group
    start_group(get_function_name()+grp,str(get_function_parameters_and_values()))
    h = host
    s = next_level_server
    if not s: s = ''
    if gaudi:
        print add_task(h, name='LogGaudiSumSrv'+grp, cmd='LogGaudiSumSrv ' + _scripts + 'LogServer.sh "' + s + ' -l 1 -S 100 -s gaudi -p /run/fmc/gaudisum.fifo"')
        print add_task(h, cmd='LogGaudi       ' + _scripts + 'LogCollect.sh "-l 1 -s gaudi -o /run/fmc/gaudisum.fifo"')
    if LHCb1:
        print add_task(h, name='LogLHCb1SumSrv'+grp, cmd='LogLHCb1SumSrv ' + _scripts + 'LogServer.sh "' + s + ' -l 1 -S 100 -s lhcb1 -p /run/fmc/logLHCb1sum.fifo"')
        print add_task(h, cmd='LogLHCb1       ' + _scripts + 'LogCollect.sh "-l 1 -s lhcb1 -o /run/fmc/logLHCb1sum.fifo"')
    if LHCb2:
        print add_task(h, name='LogLHCb2SumSrv'+grp, cmd='LogLHCb2SumSrv ' + _scripts + 'LogServer.sh "' + s + ' -l 1 -S 100 -s lhcb2 -p /run/fmc/logLHCb2sum.fifo"')
        print add_task(h, cmd='LogLHCb2       ' + _scripts + 'LogCollect.sh "-l 1 -s lhcb2 -o /run/fmc/logLHCb2sum.fifo"')
    if LHCbA:
        print add_task(h, name='LogLHCbASumSrv'+grp, cmd='LogLHCbASumSrv ' + _scripts + 'LogServer.sh "' + s + ' -l 1 -S 100 -s lhcba -p /run/fmc/logLHCbAsum.fifo"')
        print add_task(h, cmd='LogLHCbA       ' + _scripts + 'LogCollect.sh "-l 1 -s lhcba -o /run/fmc/logLHCbAsum.fifo"')
    end_group(get_function_name()+grp,str(get_function_parameters_and_values()))

def startWinCCOALogs(host, project, next_level_server,group=''):
    start_group(get_function_name()+'_'+group,str(get_function_parameters_and_values()))
    s = next_level_server
    print add_task(host, name='LogPVSSSrv_'+group, cmd='LogPVSSSrv ' + _scripts + 'LogServer.sh "' + s + ' -l 1 -S 100 -s pvss -p /run/fmc/logPVSS.fifo"')
    print add_task(host, name='LogPVSS_'+group, cmd='LogPVSS ' + _scripts + 'PVSSlogger.sh ' + project, fifo='/run/fmc/logPVSS.fifo')
    end_group(get_function_name()+'_'+group,str(get_function_parameters_and_values()))

def startWinCCOAConfigLogs(host, project, next_level_server, group):
    start_group(get_function_name()+'_'+group,str(get_function_parameters_and_values()))
    s = next_level_server
    print add_task(host, name='LogPVSSConfigSrv_'+group, cmd='LogPVSSConfigSrv ' + _scripts + 'LogServer.sh "' + s + ' -l 1 -S 100 -s pvssconfig -p /run/fmc/logPVSSConfig.fifo"')
    end_group(get_function_name()+'_'+group,str(get_function_parameters_and_values()))

def startDIMTasks(host):
    start_group(get_function_name(),str(get_function_parameters_and_values()))
    print add_task(host, cmd=' webDID ' + _scripts + 'webDID.sh')
    end_group(get_function_name(),str(get_function_parameters_and_values()))

def startControlsNodeTasks(node, next_level_server=None, gaudilog=1, slice=''):
    start_group(get_function_name(),str(get_function_parameters_and_values()))
    if next_level_server is not None:
        if use_log is not None:
            print add_task(node, cmd=' LogSumDefSrv   ' + _scripts + 'LogServer.sh "' + next_level_server + ' -l 3 -S 100 -s fmc  -p /run/fmc/logsum.fifo"')
            print add_task(node, cmd=' LogDef         ' + _scripts + 'LogCollect.sh "-l 3 -s fmc -o /run/fmc/logsum.fifo"')
    if use_pub is not None:
        if next_level_server is not None:
            print add_task(node, cmd=' ROpublish ' + _scripts + 'MONPublish.sh "' + next_level_server + ' -statDelay=8000 -mbmDelay=1376"')
            print add_task(node, cmd=' TaskSupervisor ' + _scripts + 'TaskSupervisor.sh')
    startDIMTasks(node)
    print add_task(node, cmd='PropertyServer '   + _scripts + 'PropertyServer.sh')
    end_group(get_function_name(),str(get_function_parameters_and_values()))

def startTorrentTasksWorker(node, controlsPC):
    start_group(get_function_name(),str(get_function_parameters_and_values()))
    print add_task(node, cmd=' TorrentLoader ' + _scripts + 'BitTorrentLoader.sh')
    end_group(get_function_name(),str(get_function_parameters_and_values()))

def startTorrentTasksControl(node, controlsPC):
    start_group(get_function_name(),str(get_function_parameters_and_values()))
    print add_task(node, cmd=' TorrentPublish ' + _scripts + 'BitTorrentPublish.sh')
    end_group(get_function_name(),str(get_function_parameters_and_values()))

def startHLTControls(node, next_level_server, with_pvss=True):
    start_group(get_function_name(),str(get_function_parameters_and_values()))
    startControlsNodeTasks(node, next_level_server, gaudilog=None)
    startGaudiControlsLogs(node, next_level_server)
    startWinCCOALogs(node, node.upper(), next_level_server, group='HLT')
    print add_task(node, cmd=' HLTEQBridge ' + _scripts + 'HLTEQBridge.sh')
    print add_task(node, cmd=' HLTThrColl /group/online/dataflow/cmtuser/OnlineRelease/Online/FarmConfig/job/SFThrottleCollect.sh')
    # added by Beat
    print add_task(node, cmd=' BusyMonControl /group/online/dataflow/cmtuser/OnlineRelease/Online/FarmConfig/job/SFBusyMon.sh', Nice=-15)
    end_group(get_function_name(),str(get_function_parameters_and_values()))

def startSliceControlsTasks(node, next_level_server, slice, num=16):
    start_group(get_function_name(),str(get_function_parameters_and_values()))
    startControlsNodeTasks(node, next_level_server, gaudilog=None)
    startGaudiControlsLogs(node, next_level_server, gaudi=0, LHCb1=0, LHCb2=0, LHCbA=0)
    startSliceCollectors(node,   next_level_server, slice, num)
    startWinCCOALogs(node,  slice.upper(), next_level_server, group=slice.upper()[:4])
    startWinCCOAConfigLogs(node,    slice.upper(), next_level_server, group=slice.upper()[:4])
    end_group(get_function_name(),str(get_function_parameters_and_values()))

def startECS03Tasks(node, control):
    start_group(get_function_name(),str(get_function_parameters_and_values()))
    startDIMTasks(node)
    print add_task(node, cmd='LogDefaultSrv ' + _scripts + 'LogServer.sh ' + control + ' -S 100 -l 1 -s default -p /tmp/logSrv.fifo')
    print add_task(control, name='ROpublish_ECS03', cmd='ROpublish ' + _scripts + 'MONPublish.sh "'+control+' -statDelay=8000 -mbmDelay=1500"')
    print add_task_root(control, name='ROcollect_ECS03', cmd='ROcollect   ' + _scripts + 'ROMONCollect.sh')
    print add_task(node, cmd='TANServ ' + _scripts + 'TanServ.sh')
    print add_task(node, cmd='ROBridge_0 ' + _scripts + 'ROBridge.sh')
    print add_task(node, cmd='ROLogger_0 ' + _scripts + 'ROLogger.sh')
    print add_task(node, cmd='TaskSupervisor ' + _scripts + 'TaskSupervisor.sh')
    print add_task(node, cmd='FarmStatSrv_0 ' + _scripts + 'FarmStatSrv.sh -target=fmc01 -mbm')
    print add_task(node, cmd='FarmStatSrv_1 ' + _scripts + 'FarmStatSrv.sh -target=ecs03 -mbm -cpu')
    print add_task(node, cmd='ActiveMQ_0 ' + _scripts + 'ActiveMQ.sh ecs3', X=5, M=1)
    print add_task(node, cmd='StatusStomp_0 ' + _scripts + 'StompSrv.sh ecs04 -topic=/topic/status -protocol=AMQ', X=5, M=1)
    print add_task(node, cmd='StatusMQTT_0 ' + _scripts + 'StompSrv.sh ecs04 -topic=/topic/item -protocol=MQTT', X=5, M=1)
    print add_task(node, u='online', cmd='OrbitedSrv_0 ' + _scripts + 'OrbitedSrv.sh orbited/orbited.activeMQ.0.cfg')
    print add_task(node, cmd='ElogExtract   ' + _scripts + 'elog_extract.py')
    print add_task(node, cmd='ElogPublish   ' + _scripts + 'elog_publish.sh ecs04')
    print add_task(node, cmd='BootMonSrv_0  ' + _scripts + 'BootMonSrv.sh')
    print add_task(node, cmd='AMQWatch_0    ' + _scripts + 'AMQWatch.sh')
    print add_task(node, cmd='Hlt_DeferSrv  ' + _scripts + 'HltDeferSrv.sh -listen=/ROpublish/HLTDefer -publish=/HLT/Deferred')
    print add_task(node, cmd='Hlt_Hlt1Srv   ' + _scripts + 'HltDeferSrv.sh -listen=/ROpublish/HLT1 -publish=/HLT/HLT1')
    print add_task(node, cmd='Hlt_Hlt1DQSrv ' + _scripts + 'HltDeferSrv.sh -listen=/ROpublish/HLT1 -publish=/HLT/HLT1 -dis_dns=mona10')
    # Special service for HLT2 Monitoring
    print add_task(node, cmd='Hlt1Srv2fmc01 ' + _scripts + 'HltDeferSrv.sh -listen=/ROpublish/HLT1 -publish=/HLT/HLT1 -dis_dns=fmc01 -dic_dns=ecs03')
    print add_task(node, cmd='FileStatistics '   + _scripts + 'FileStatistics.sh')
    print add_task(node, cmd='HLTFileEqualizer ' + _scripts + 'HLTFileEqualizer.sh')
    print add_task(node, cmd='FarmStatusTrans '  + _scripts + 'FarmStatusTrans.sh')
    print add_task(node, cmd='SwDataScan '       + _scripts + 'SwDataScan.sh')
    print add_task(node, cmd='CleanOptions '     + _scripts + 'clean.nfs.sh')
    print add_task(node, cmd='PropertyServer '   + _scripts + 'PropertyServer.sh')
    print add_task(node, cmd='PropertyInfo '     + _scripts + 'PropertyInfo.sh')
    print add_task(node, cmd='DidServer '        + _scripts + 'DidServer.sh')
    print add_task(node, cmd='webDID_ecs01    ' + _scripts + 'webDID.sh -DDIM_DNS_NODE=ecs01    -DDIM_DID_PORT=2701 ')
    print add_task(node, cmd='webDID_ecs05    ' + _scripts + 'webDID.sh -DDIM_DNS_NODE=ecs05    -DDIM_DID_PORT=2702 ')
    print add_task(node, cmd='webDID_echvc01  ' + _scripts + 'webDID.sh -DDIM_DNS_NODE=echvc01  -DDIM_DID_PORT=2703 ')
    print add_task(node, cmd='webDID_tmudaq01 ' + _scripts + 'webDID.sh -DDIM_DNS_NODE=tmudaq01 -DDIM_DID_PORT=2704 ')
    print add_task(node, cmd='webDID_vedaqa01 ' + _scripts + 'webDID.sh -DDIM_DNS_NODE=vedaqa01 -DDIM_DID_PORT=2705 ')
    #print add_task(node, cmd='webDID_veecs01  ' + _scripts + 'webDID.sh -DDIM_DNS_NODE=veecs01  -DDIM_DID_PORT=2706 ')
    #print add_task(node, cmd='webDID_veecs02  ' + _scripts + 'webDID.sh -DDIM_DNS_NODE=veecs02  -DDIM_DID_PORT=2707 ')
    print add_task(node, cmd='webDID_itecs01  ' + _scripts + 'webDID.sh -DDIM_DNS_NODE=itecs01  -DDIM_DID_PORT=2708 ')
    print add_task(node, cmd='webDID_ttecs01  ' + _scripts + 'webDID.sh -DDIM_DNS_NODE=ttecs01  -DDIM_DID_PORT=2709 ')
    print add_task(node, cmd='webDID_otecs01  ' + _scripts + 'webDID.sh -DDIM_DNS_NODE=otecs01  -DDIM_DID_PORT=2710 ')
    print add_task(node, cmd='webDID_riecs01  ' + _scripts + 'webDID.sh -DDIM_DNS_NODE=riecs01  -DDIM_DID_PORT=2711 ')
    print add_task(node, cmd='webDID_ececs01  ' + _scripts + 'webDID.sh -DDIM_DNS_NODE=caecs01  -DDIM_DID_PORT=2712 ')
    print add_task(node, cmd='webDID_muecs01  ' + _scripts + 'webDID.sh -DDIM_DNS_NODE=muecs01  -DDIM_DID_PORT=2713 ')
    print add_task(node, cmd='webDID_muhva01  ' + _scripts + 'webDID.sh -DDIM_DNS_NODE=muhva01  -DDIM_DID_PORT=2714 ')
    print add_task(node, cmd='webDID_muhvc01  ' + _scripts + 'webDID.sh -DDIM_DNS_NODE=muhvc01  -DDIM_DID_PORT=2715 ')
    print add_task(node, cmd='webDID_othv01   ' + _scripts + 'webDID.sh -DDIM_DNS_NODE=othv01   -DDIM_DID_PORT=2716 ')
    print add_task(node, cmd='webDID_otdaqa01 ' + _scripts + 'webDID.sh -DDIM_DNS_NODE=otdaqa01 -DDIM_DID_PORT=2717 ')
    print add_task(node, cmd='webDID_otdaqc01 ' + _scripts + 'webDID.sh -DDIM_DNS_NODE=otdaqc01 -DDIM_DID_PORT=2718 ')

    print add_task(node, cmd='webDID_infdai01 ' + _scripts + 'webDID.sh -DDIM_DNS_NODE=infdai01 -DDIM_DID_PORT=2730 ')
    #print add_task(node, cmd='webDID_infdai02 ' + _scripts + 'webDID.sh -DDIM_DNS_NODE=infdai02 -DDIM_DID_PORT=2731 ')
    print add_task(node, cmd='webDID_infmag01 ' + _scripts + 'webDID.sh -DDIM_DNS_NODE=infmag01 -DDIM_DID_PORT=2732 ')
    end_group(get_function_name(),str(get_function_parameters_and_values()))

def startECS04Tasks(node,control=''):
    start_group(get_function_name(),str(get_function_parameters_and_values()))
    startDIMTasks(node)
    end_group(get_function_name(),str(get_function_parameters_and_values()))

def startLogSrvInfrastructure(node, control):
    start_group(get_function_name(),str(get_function_parameters_and_values()))
    print add_task(node,      cmd='LogDefaultSrv ' + _scripts + 'LogServer.sh ' + control + ' -S 100 -l 1 -s default -p /tmp/logSrv.fifo')
    print add_task(node,      cmd='TANServ   ' + _scripts + 'TanServ.sh')
    print add_task(node,      cmd='pingSrv_u ' + _scripts + 'PingSrv.sh')
    print add_task_root(node, cmd='ROcollect   ' + _scripts + 'ROMONCollect.sh')
    end_group(get_function_name(),str(get_function_parameters_and_values()))

def startLogSrvTasks(node, control):
    start_group(get_function_name(),str(get_function_parameters_and_values()))
    print add_task(control, name='ROpublishLOGSRV', cmd=' ROpublish ' + _scripts + 'MONPublish.sh ecs03 -statDelay=8000 -mbmDelay=1376"')
    print add_task(node,    cmd=' TaskSupervisor ' + _scripts + 'TaskSupervisor.sh')
    startDIMTasks(node)
    end_group(get_function_name(),str(get_function_parameters_and_values()))

def check(match, test):
    match = re.match(match.upper(), test.upper())
    if match:
        if match.pos == 0 and match.endpos == len(test):
            return 1
    return True
    return None

#------------------------------------------------------------------------------

def startCALD07Tasks(node,control=''):
    start_group(get_function_name(),str(get_function_parameters_and_values()))
    print add_task(node, cmd='AnalysisBridge_0 ' + _scripts + 'AnalysisBridge.sh mona08.lbdaq.cern.ch cald07.lbdaq.cern.ch "LHCbStatus/*"')
    end_group(get_function_name(),str(get_function_parameters_and_values()))

def startCALD0701Tasks(node,control=''):
    start_group(get_function_name(),str(get_function_parameters_and_values()))
    print add_task(node, cmd='LHCb_CALD0701_CalibHistos_0 ' + _scripts + 'HistDimBridge.sh  CalibHistBridge.opts')
    print add_task(node, cmd='AnalysisBridge_0 ' + _scripts + 'AnalysisBridge.sh mona08.lbdaq.cern.ch cald07.lbdaq.cern.ch "LHCbStatus/*"')
    end_group(get_function_name(),str(get_function_parameters_and_values()))

def startSTORECTL01Tasks(node,control=''):
    start_group(get_function_name(),str(get_function_parameters_and_values()))
    print add_task(node, cmd=' TorrentPublish ' + _scripts + 'BitTorrentPublish.sh')
    print add_task(node, cmd=' PartitionLogStamps ' + _scripts + 'PartitionLogStamps.sh')
    end_group(get_function_name(),str(get_function_parameters_and_values()))

def startSTORERECVTasks(node,control=''):
    start_group(get_function_name(),str(get_function_parameters_and_values()))
    print add_task(node, cmd=' TorrentTracker ' + _scripts + 'BitTorrentTracker.sh')
    end_group(get_function_name(),str(get_function_parameters_and_values()))

def startMONA0802Tasks(node,control=''):
    start_group(get_function_name(),str(get_function_parameters_and_values()))
    cmd = 'LHCb_MONA0802_WebGLSrv_0 -D LOGFIFO=/run/fmc/logGaudi.fifo -D PARTITION=LHCb '
    cmd = cmd + '/group/online/dataflow/cmtuser/WebGLDisplay/WebGLDisplaySys/scripts/runWebGLDisplay.sh'
    cmd = cmd + ' WebGLDisplay Class2 WebGLDisplaySrv mona08'
    print add_task(node, fifo='/run/fmc/logGaudi.fifo', cmd=cmd)
    print add_task(node, fifo='/run/fmc/logGaudi.fifo', cmd='LHCb_WebGLScan_0 ' + _scripts + 'WebGLEventScanner.sh')
    end_group(get_function_name(),str(get_function_parameters_and_values()))

def startMONA09Tasks(node,control=''):
    start_group(get_function_name(),str(get_function_parameters_and_values()))
    print add_task(node, cmd='RecHistos_0 ' + _scripts + 'HistDimBridge.sh  RecHistBridge.opts', X=30, M=15)
    print add_task(node, cmd='RecSaver_0 ' + _scripts + 'HistDimSaver.sh  RecHistSaver.opts', X=30, M=15)
    end_group(get_function_name(),str(get_function_parameters_and_values()))

def startMONA10Tasks(node,control=''):
    start_group(get_function_name(),str(get_function_parameters_and_values()))
    print add_task(node, cmd=' TorrentPublish ' + _scripts + 'BitTorrentPublish.sh')
    end_group(get_function_name(),str(get_function_parameters_and_values()))

def startDQTasks(node,control=''):
    start_group(get_function_name(),str(get_function_parameters_and_values()))
    print add_task(node, cmd=' DQScanner ' + _scripts + 'DQScanner.sh scan', fifo='/run/fmc/logGaudi.fifo', X=20, M=20)
    print add_task(node, cmd=' DQResults ' + _scripts + 'DQResults.sh', fifo='/run/fmc/logGaudi.fifo', X=20, M=20)
    print add_task(node, cmd=' DQPublish ' + _scripts + 'DQPublish.sh', fifo='/run/fmc/logGaudi.fifo', X=20, M=20)
    print add_task(node, cmd=' DQDump ' + _scripts + 'DQDump.sh', fifo='/run/fmc/logGaudi.fifo', X=20, M=20)
    print add_task(node, cmd='LogDataQuality ' + _scripts + 'LogDataQuality.sh mona10 -S 100 -l 1', fifo='/run/fmc/logGaudi.fifo', X=20, M=20)
    end_group(get_function_name(),str(get_function_parameters_and_values()))

def startHLT01Tasks(node,control=''):
    start_group(get_function_name(),str(get_function_parameters_and_values()))
    print add_task(node, cmd=' LogDef         ' + _scripts + 'LogCollect.sh "-l 3 -s fmc -o /run/fmc/logsum.fifo"')
    print add_task(node, cmd=' TaskSupervisor ' + _scripts + 'TaskSupervisor.sh')
    #print add_task(node, name='LogSumDefSrvHLTTOP', cmd=' LogSumDefSrv   ' + _scripts + 'LogServer.sh "' + top_logger_host + ' -l 3 -S 100 -s fmc  -p /run/fmc/logsum.fifo"')
    print add_task(node, name='ROpublish_HLTTOP', cmd=' ROpublish ' + _scripts + 'MONPublish.sh "' + top_logger_host + ' -statDelay=8000 -mbmDelay=1376"')
    startDIMTasks(node)
    print add_task(node, cmd='PropertyServer '   + _scripts + 'PropertyServer.sh')
    """
    print add_task(node, name='LogGaudiSumSrvHLTTOP', cmd='LogGaudiSumSrv ' + _scripts + 'LogServer.sh "' + top_logger_host + ' -l 1 -S 100 -s gaudi -p /run/fmc/gaudisum.fifo"')
    print add_task(node, cmd='LogGaudi       ' + _scripts + 'LogCollect.sh "-l 1 -s gaudi -o /run/fmc/gaudisum.fifo"')
    print add_task(node, name='LogLHCb1SumSrvHLTTOP', cmd='LogLHCb1SumSrv ' + _scripts + 'LogServer.sh "' + top_logger_host + ' -l 1 -S 100 -s lhcb1 -p /run/fmc/logLHCb1sum.fifo"')
    print add_task(node, cmd='LogLHCb1       ' + _scripts + 'LogCollect.sh "-l 1 -s lhcb1 -o /run/fmc/logLHCb1sum.fifo"')
    print add_task(node, name='LogLHCb2SumSrvHLTTOP', cmd='LogLHCb2SumSrv ' + _scripts + 'LogServer.sh "' + top_logger_host + ' -l 1 -S 100 -s lhcb2 -p /run/fmc/logLHCb2sum.fifo"')
    print add_task(node, cmd='LogLHCb2       ' + _scripts + 'LogCollect.sh "-l 1 -s lhcb2 -o /run/fmc/logLHCb2sum.fifo"')
    print add_task(node, name='LogLHCbASumSrvHLTTOP', cmd='LogLHCbASumSrv ' + _scripts + 'LogServer.sh "' + top_logger_host + ' -l 1 -S 100 -s lhcba -p /run/fmc/logLHCbAsum.fifo"')
    print add_task(node, cmd='LogLHCbA       ' + _scripts + 'LogCollect.sh "-l 1 -s lhcba -o /run/fmc/logLHCbAsum.fifo"')
    """
    startGaudiControlsLogs(node, top_logger_host, group='HLTTOP')
    startWinCCOALogs(node, 'HLT', top_logger_host, group='HLTTOP')
    startWinCCOAConfigLogs(node, 'HLT', top_logger_host, group='HLTTOP')
    end_group(get_function_name(),str(get_function_parameters_and_values()))

def startHLT02Tasks(node,control=''):
    start_group(get_function_name(),str(get_function_parameters_and_values()))
    print add_task(node, cmd=' CondDBXfer_0 ' + _scripts + 'internal/condDBXfer.sh')
    print add_task(node, cmd=' CondDBCheck_0 /group/online/AligWork/CheckConstants/checkConstants.sh')
    print add_task(node, cmd=' CondDBXfer_RunTICK_0 ' + _scripts + 'internal/condDBXfer_doRunTick.sh')
    print add_task(node, cmd=' LHCb_HLT02_MoniOnlineAlig_0 /group/online/AligWork/MoniPlots/runAlignMonitor.sh')
    # added by Beat
    print add_task(node, cmd=' BusyMon /group/online/dataflow/cmtuser/OnlineRelease/Online/FarmConfig/job/TopBusyMon.sh')
    # added by Beat
    print add_task(node, cmd=' BusyPub /group/online/dataflow/cmtuser/OnlineRelease/Online/FarmConfig/job/BusyPub.sh')
    end_group(get_function_name(),str(get_function_parameters_and_values()))

def startFMC01Tasks(node,control=''):
    start_group(get_function_name(),str(get_function_parameters_and_values()))
    print add_task_root(node, cmd='ipmi_farm_slow /home/beat/ipmistart.sh "-c /home/beat/ipmi_slow_nodes.config -n ipmi_farm_slow -p /FMC"')
    print add_task_root(node, cmd='ipmi_farm_medium /home/beat/ipmistart.sh "-c /home/beat/ipmi_medium_nodes.config -n ipmi_farm_medium -p /FMC"')
    print add_task_root(node, cmd='ipmi_farm_fast /home/beat/ipmistart.sh "-c /home/beat/ipmi_fast_nodes.config -n ipmi_farm_fast -p /FMC"')
    print add_task_root(node, cmd='ipmi_farm_faster /home/beat/ipmistart.sh "-c /home/beat/ipmi_faster_nodes.config -n ipmi_faster_nodes -p /FMC"')
    print add_task_root(node, cmd='ipmi_rest /home/beat/ipmistart.sh "-c /home/beat/ipmi_rest.conf -n ipmi_rest -p /FMC"', X=20, M=15)
    print add_task_root(node, cmd='ipmi_mona /home/beat/ipmistart.sh "-c /home/beat/ipmi_MONA.conf -n ipmi_mona -p /FMC"', X=20, M=15)
    print add_task(node, cmd=' webDID ' + _scripts + 'webDID.sh')
    end_group(get_function_name(),str(get_function_parameters_and_values()))

def create_partition_logger(node, host, partition, match):
    startLogSrvInfrastructure(node, host)
    start_group(partition+'_LoggerTasks',str(get_function_parameters_and_values()))
    print add_task(node,  u='online', g='onliners', cmd='PartitionLog_'+partition+'  '  + _scripts + 'PartitionLog.sh '+partition)
    end_group(partition+'_LoggerTasks',str(get_function_parameters_and_values()))
    end_node_type(partition+'_LogServer',match)

def make_hist_node(node):
    startDIMTasks(node)
    end_node_type('HistoAnalysisNode','hist0?')

def startTasks(host):
    start_group(get_function_name(),str(get_function_parameters_and_values()))
    HOST = host.upper()
    h = HOST[:6]
    node = host.lower()

    time.sleep(3)
    host = '<DIM_DNS>'
    node = '<NODE>'
    top_logger_host = '<TOP_LOGGER>'
    node_type = None
    print 'def func(server):'
    #print '  CONTROLS_PC = "'+node+'"'
    #print '  HOST_NAME   = "'+node+'"'
    #print '  # "This is the node:\'' + host + '\'";'

    if check('hlt[a-m][0-9][0-9]', HOST):
        startInfrastructure(node, host, top_logger_host)
        startTorrentTasksWorker(node, host)
        end_node_type('HLTWorker','hlt[a-m][0-9][0-9][0-9][0-9]')

        startInfrastructure(node, host, top_logger_host)
        startTorrentTasksWorker(node, host)
        startHLTControls(node, top_logger_host)
        startTorrentTasksControl(node, top_logger_host)
        end_node_type('HLTControlsPC', 'hlt[a-m][0-9][0-9]')

    if check('cal[a-f][0-9][0-9]', HOST):

        startInfrastructure(node, host, top_logger_host)
        startCALD0701Tasks(node, host)
        end_node_type('CalibWorker','cal[a-f][0-9][0-9][0-9][0-9]')

        startInfrastructure(node, host, top_logger_host)
        startHLTControls(node, top_logger_host)
        startCALD07Tasks(node, host)
        end_node_type('CalibControlsPC','cal[a-f][0-9][0-9]')

    if check('storectl01', HOST):

        startSliceNodeTasks(node, host, 'Storage', 16)
        startTorrentTasksWorker(node, host)
        startSTORERECVTasks(node, host)
        end_node_type('StorageWorker','storerecv0[0-9]')

        startSliceNodeTasks(node, host, 'Storage', 16)
        startTorrentTasksWorker(node, host)
        startSliceControlsTasks(node, top_logger_host, 'Storage', 16)
        startSTORECTL01Tasks(node, host)
        end_node_type('StorageControlsPC','storectl01')

    if check('mona08', HOST):

        startSliceNodeTasks(node, host, 'Monitoring', 16)
        end_node_type('MonitoringWorker','mona080[13456789]')

        startSliceNodeTasks(node, host, 'Monitoring', 16)
        startMONA0802Tasks(node,host)
        end_node_type('MonitoringWorker02','mona0802')

        startSliceNodeTasks(node, host, 'Monitoring', 16)
        startSliceControlsTasks(node, top_logger_host, 'Monitoring', 16)
        end_node_type('MonitoringControlsPC','mona08')

    if check('mona09', HOST):
            
        startInfrastructure(node, host, top_logger_host)
        startSliceNodeTasks(node, host, 'Reconstruction', 4)
        end_node_type('ReconstructionWorker','mona09[0-9][0-9]')

        startInfrastructure(node, host, top_logger_host)
        startSliceNodeTasks(node, host, 'Reconstruction', 4)
        startSliceControlsTasks(node, top_logger_host, 'Reconstruction', 4)
        startMONA09Tasks(node,host)
        end_node_type('ReconstructionControlsPC','mona09')

    if check('mona10', HOST):

        startInfrastructure(node, host, top_logger_host)
        startTorrentTasksWorker(node, host)
        end_node_type('DQWorker','mona100[2-9]')

        startInfrastructure(node, host, top_logger_host)
        startTorrentTasksWorker(node, host)
        startDQTasks(node, host)
        end_node_type('DQScanner','mona1001')

        startInfrastructure(node, host, top_logger_host)
        startHLTControls(node, top_logger_host)
        startMONA10Tasks(node, host)
        end_node_type('DQControlsPC','mona10')

    if check('hlt0[1-2]', HOST):

        startInfrastructure(node, host, top_logger_host, group='HLTTOP')
        startHLT01Tasks(node, host)
        end_node_type('HLT01','hlt01')

        startInfrastructure(node, host, top_logger_host, group='HLTTOP')
        startHLT02Tasks(node, host)
        end_node_type('HLT02','hlt02')

    if check('checkpoint01', HOST):

        startInfrastructure(node, host, top_logger_host)
        end_node_type('CheckPoint','checkpoint01')

    if check('fmc01', HOST):

        startFMC01Tasks(node, host)
        end_node_type('FMC01','fmc01')

    if check('ecs01', HOST):
        pass
    if check('ecs02', HOST):
        pass
    if check('ecs03', HOST):

        startECS03Tasks(node,host)
        end_node_type('ECS03','ecs03')

    if check('ecs04', HOST):

        startInfrastructure(node, host, top_logger_host)
        startWinCCOALogs(node, 'ECS04', top_logger_host, group='ECS04')
        startECS04Tasks(node, host)
        end_node_type('ECS04','ecs04')

    if check('logsrv01', HOST):

        startLogSrvInfrastructure(node, host)
        startLogSrvTasks(node, host)
        end_node_type('LogServerControl','logsrv01')

        create_partition_logger(node, host, 'LHCb','logspartlhcb01')
        create_partition_logger(node, host, 'LHCb1','logspartlhcb101')
        create_partition_logger(node, host, 'LHCbA','logspartlhcba01')
        create_partition_logger(node, host, 'FEST','logspartfest01')
        create_partition_logger(node, host, 'LHCb2','logspartfest01')
        create_partition_logger(node, host, 'VELO','logspartvelo01')
        create_partition_logger(node, host, 'IT','logspartit01')
        create_partition_logger(node, host, 'TT','logsparttt01')
        create_partition_logger(node, host, 'OT','logspartot01')
        create_partition_logger(node, host, 'TDET','logsparttdet01')
        create_partition_logger(node, host, 'CALO','logspartcalo01')
        create_partition_logger(node, host, 'RICH','logspartrich01')
        create_partition_logger(node, host, 'RICH1','logspartrich101')
        create_partition_logger(node, host, 'RICH2','logspartrich201')

    if check('hist0?', HOST):

        make_hist_node(node)

    end_group(get_function_name(),str(get_function_parameters_and_values()))

    return


def hostName():
    host = socket.gethostname()
    if host.find('.') > 0:
        host = host[:host.find('.')]
    return host


def start():
    #try:
      os.stat('/cvmfs/lhcb.cern.ch')
      host = hostName()
      setupEnv()
      startTasks(host)
      return
    #except Exception,X:
    #  print 'manipBootTasks will not be executed. cvmfs is not accessible:',str(X)
    #  return
      
