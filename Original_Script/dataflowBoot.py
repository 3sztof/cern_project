import os
import re
import sys
import time
import socket
logDefaultFIFO = '/run/fmc/logSrv.fifo'
dataFlowDirScripts = '/group/online/dataflow/cmtuser/Gaudi_v19r2/Online/OnlineControls/scripts'
onlineScripts = '/group/online/dataflow/scripts'
default_exports = 'export PATH=/opt/FMC/sbin:/opt/FMC/bin:$PATH;export LD_LIBRARY_PATH=/opt/FMC/lib;'
top_logger_host = 'ecs03'
top_farm_host = 'hlt01'
_scripts = onlineScripts + os.sep


use_tan = 1
use_log = 1
use_mbm = 1
use_pub = 1
stop_opts = ' --no-authentication '
start_opts = ' -e -o --no-authentication '


def pcAddNull(h, u='online', g='onliners', cmd='', fifo=None, X=300, M=5):
    cmd = 'pcAdd --no-authentication -K 120 -M ' + \
        str(M) + ' -X ' + str(X) + ' -m ' + h + \
        ' -g ' + g + ' -n ' + u + ' -u ' + cmd + ';'
    return cmd


def pcAdd(h, u='online', g='onliners', cmd='', fifo=None, X=300, M=5, Nice=0):
    log_opts = '-E '+logDefaultFIFO+' -O '+logDefaultFIFO
    if fifo:
        log_opts = '-E ' + str(fifo) + ' -O ' + str(fifo)
    cmd = 'pcAdd --no-authentication -K 120 -M ' + str(M) + ' -X ' + str(X) + \
          ' -m ' + h + ' ' + log_opts + ' -g ' + g + ' -p ' + str(Nice) + \
          ' -n ' + u + ' -u ' + cmd + ';'
    print 'echo "' + cmd + '";'
    return cmd


def pcAdd_root(h, cmd, fifo=None, X=300, M=5):
    return pcAdd(h, 'root', 'root', cmd=cmd, fifo=fifo, X=X, M=M)


def pcRm(h, utgid): return 'pcRm --no-authentication -d 5 -m "' + \
    h + '" ' + utgid + ';'
#------------------------------------------------------------------------------


def setupEnv():
    print default_exports


def startFMC(host):
    print pcRm('hlt*', utgid='gaudiLog')
    print pcRm('hlt*', utgid='gaudiLog')
    print pcRm('hlt*', utgid='gaudiLog')
    print pcRm('hlt*', utgid='gaudiLog')
    print pcRm('hlt*', utgid='smartSrv_u')
    print pcRm('hlt*', utgid='meprxiptables')
    print pcRm('hlt*', utgid='meprxiptablesLOG')
    print pcRm('hlt*', utgid='*MEPRxSTAT*')
    print 'tmStop --no-authentication -d 5 -m hlt* gaudiLog;'
    print 'tmStop --no-authentication -d 5 -m hlt* gaudiLog;'
    print 'tmStop --no-authentication -d 5 -m hlt* gaudiLog;'
    print 'tmStop --no-authentication -d 5 -m hlt* smartSrv_u;'
    print 'tmStop --no-authentication -d 5 -m hlt* smartSrv_u;'
    print 'tmStop --no-authentication -d 5 -m hlt* smartSrv_u;'
    print 'tmStop --no-authentication -d 5 -m hlt* smartSrv_u;'

    # print pcAdd_root(host, cmd='coalSrv_u    /opt/FMC/sbin/coalSrv -l 1')
    # print pcAdd(host,      cmd='cpuinfoSrv_u /opt/FMC/sbin/cpuinfoSrv -l 1')
    # print pcAdd(host,      cmd='cpustatSrv_u /opt/FMC/sbin/cpustatSrv -l 1')
    # print pcAdd(host,      cmd='irqSrv_u     /opt/FMC/sbin/irqSrv -l 1')
    # print pcAdd(host,      cmd='memSrv_u     /opt/FMC/sbin/memSrv -l 1')
    # print pcAdd_root(host, cmd='nifSrv_u     /opt/FMC/sbin/nifSrv -l 1 -u 10')
    # print pcAdd_root(host, cmd='psSrv_u      /opt/FMC/sbin/psSrv -l 1')
    # print pcAdd(host,      cmd='tcpipSrv_u   /opt/FMC/sbin/tcpipSrv -l 1')
    # print pcAdd_root(host, cmd='smartSrv_u   /opt/FMC/sbin/smartSrv -l 1')
    # print pcAdd(host,      cmd='fsSrv_u      /opt/FMC/sbin/fsSrv -l 1')
    if use_tan is not None:
        print pcAdd(host,    cmd='TANServ     ' + _scripts + 'TanServ.sh')
    print pcAdd(host,      cmd='pingSrv_u   ' + _scripts + 'PingSrv.sh')


def stopFMC(host):
    # print pcRm(host, utgid='coalSrv_u')
    # print pcRm(host, utgid='cpustatSrv_u')
    # print pcRm(host, utgid='cpuinfoSrv_u')
    # print pcRm(host, utgid='irqSrv_u')
    # print pcRm(host, utgid='memSrv_u')
    # print pcRm(host, utgid='nifSrv_u')
    # print pcRm(host, utgid='psSrv_u')
    # print pcRm(host, utgid='tcpipSrv_u')
    # print pcRm(host, utgid='smartSrv_u')
    # print pcRm(host, utgid='fsSrv_u')
    print pcRm(host, utgid='TANServ')
    print pcRm(host, utgid='pingSrv_u')

#------------------------------------------------------------------------------


def stopInfrastructure(host):
    if use_mbm is not None:
        print pcRm(host, utgid='MBMMON_0')
    if use_pub is not None:
        print pcRm(host, utgid='ROcollect')

#------------------------------------------------------------------------------


def startInfrastructure(host):
    if use_pub is not None:
        print pcAdd_root(host, cmd='ROcollect   ' + _scripts + 'ROMONCollect.sh')
    # if use_mbm is not None:    print pcAdd(host,      cmd='MBMMON_0    '+_scripts+'MBMMON.upi.sh')

#------------------------------------------------------------------------------


def stopProcessingNodeTasks(host, gaudilog=1):
    stopInfrastructure(host)
    print pcRm(host, 'LogDefault')
    if use_log is not None and gaudilog is not None:
        print pcRm(host, 'LogLHCb1Srv')
        print pcRm(host, 'LogGaudiSrv')
        print pcRm(host, 'LogLHCb2Srv')
        print pcRm(host, 'LogLHCbASrv')

#------------------------------------------------------------------------------


def startProcessingNodeTasks(host, controlsPC, next_level_server=None, gaudilog=1):
    startInfrastructure(host)
    print pcAdd(host, cmd='LogDefaultSrv ' + _scripts + 'LogServer.sh ' + controlsPC + ' -S 100 -l 1 -s default -p /tmp/logSrv.fifo')
    if use_log is not None and gaudilog is not None:
        print pcAdd(host, cmd='LogGaudiSrv ' + _scripts + 'LogServer.sh ' + controlsPC + ' -S 100 -l 1 -s gaudi -p /run/fmc/logGaudi.fifo')
        print pcAdd(host, cmd='LogLHCb1Srv ' + _scripts + 'LogServer.sh ' + controlsPC + ' -S 100 -l 1 -s lhcb1 -p /run/fmc/logLHCb1.fifo')
        print pcAdd(host, cmd='LogLHCb2Srv ' + _scripts + 'LogServer.sh ' + controlsPC + ' -S 100 -l 1 -s lhcb2 -p /run/fmc/logLHCb2.fifo')
        print pcAdd(host, cmd='LogLHCbASrv ' + _scripts + 'LogServer.sh ' + controlsPC + ' -S 100 -l 1 -s lhcba -p /run/fmc/logLHCbA.fifo')

#------------------------------------------------------------------------------


def stopSliceLogs(host, slice, num=16):
    "Stop slice dependent log message tasks on a storage or Monitoring controls PC"
    for i in xrange(num):
        print pcRm(host, utgid='%s_Slice%02XSrv' % (slice, i))

#------------------------------------------------------------------------------


def startSliceLogs(host, controlsPC, slice, num=16):
    "Start slice dependent log message tasks on a storage or Monitoring controls PC"
    for i in xrange(num):
        sl = '%s_Slice%02X' % (slice, i)
        print pcAdd(host, cmd='%sSrv %sLogServer.sh %s -S 100 -l 1 -s %s -p /run/fmc/%s.fifo' % (sl, _scripts, controlsPC, sl, sl))

#------------------------------------------------------------------------------


def startSliceCollectors(host, next_level_server, slice, num=16):
    for i in xrange(num):
        sl = '%s_Slice%02X' % (slice, i)
        print pcAdd(host, cmd='%sSumSrv %sLogServer.sh "%s -S 100 -l 1 -s %s -p /run/fmc/%ssum.fifo"' % (sl, _scripts, next_level_server, sl, sl))
        print pcAdd(host, cmd='%sCollect %sLogCollect.sh "-l 1 -s %s -o /run/fmc/%ssum.fifo"' % (sl, _scripts, sl, sl))

#------------------------------------------------------------------------------


def stopSliceCollectors(host, slice, num=16):
    for i in xrange(num):
        print pcRm(host, utgid='%s_Slice%02XSumSrv' % (slice, i,))
        print pcRm(host, utgid='%s_Slice%02XCollect' % (slice, i,))

#------------------------------------------------------------------------------


def stopSliceNodeTasks(host, slice, num=16):
    stopProcessingNodeTasks(host, gaudilog=2)
    print pcRm(host, 'LogLHCb1Srv')
    print pcRm(host, 'LogLHCb2Srv')
    stopSliceLogs(host, slice, num)

#------------------------------------------------------------------------------


def startSliceNodeTasks(host, controlsPC, slice, num=16):
    startProcessingNodeTasks(host, controlsPC, gaudilog=2)
    startSliceLogs(host, controlsPC, slice, num)

#------------------------------------------------------------------------------


def stopGaudiControlsLogs(host, gaudi=1, LHCb1=1, LHCb2=1, LHCbA=1):
    print pcRm(host, 'LogDefaultSrv')
    if gaudi:
        print pcRm(host, 'LogGaudiSrv')
        print pcRm(host, 'LogGaudiSumSrv')
        print pcRm(host, 'LogGaudi')
    if LHCb1:
        print pcRm(host, 'LogLHCb1Srv')
        print pcRm(host, 'LogLHCb1')
        print pcRm(host, 'LogLHCb1SumSrv')
    if LHCb2:
        print pcRm(host, 'LogLHCb2Srv')
        print pcRm(host, 'LogLHCb2')
        print pcRm(host, 'LogLHCb2SumSrv')
    if LHCbA:
        print pcRm(host, 'LogLHCbASrv')
        print pcRm(host, 'LogLHCbA')
        print pcRm(host, 'LogLHCbASumSrv')

#------------------------------------------------------------------------------


def startGaudiControlsLogs(host, next_level_server, gaudi=1, LHCb1=1, LHCb2=1, LHCbA=1):
    h = host
    s = next_level_server
    print pcAdd(h, cmd='LogDefaultSrv ' + _scripts + 'LogServer.sh "' + host + ' -l 1 -S 100 -s default -p /tmp/logSrv.fifo"')
    if gaudi:
        print pcAdd(h, cmd='LogGaudiSrv    ' + _scripts + 'LogServer.sh "' + h + ' -l 1 -S 100 -s gaudi -p /run/fmc/logGaudi.fifo"')
        print pcAdd(h, cmd='LogGaudiSumSrv ' + _scripts + 'LogServer.sh "' + s + ' -l 1 -S 100 -s gaudi -p /run/fmc/gaudisum.fifo"')
        print pcAdd(h, cmd='LogGaudi       ' + _scripts + 'LogCollect.sh "-l 1 -s gaudi -o /run/fmc/gaudisum.fifo"')
    if LHCb1:
        print pcAdd(h, cmd='LogLHCb1Srv    ' + _scripts + 'LogServer.sh "' + h + ' -l 1 -S 100 -s lhcb1 -p /run/fmc/logLHCb1.fifo"')
        print pcAdd(h, cmd='LogLHCb1SumSrv ' + _scripts + 'LogServer.sh "' + s + ' -l 1 -S 100 -s lhcb1 -p /run/fmc/logLHCb1sum.fifo"')
        print pcAdd(h, cmd='LogLHCb1       ' + _scripts + 'LogCollect.sh "-l 1 -s lhcb1 -o /run/fmc/logLHCb1sum.fifo"')
    if LHCb2:
        print pcAdd(h, cmd='LogLHCb2Srv    ' + _scripts + 'LogServer.sh "' + h + ' -l 1 -S 100 -s lhcb2 -p /run/fmc/logLHCb2.fifo"')
        print pcAdd(h, cmd='LogLHCb2SumSrv ' + _scripts + 'LogServer.sh "' + s + ' -l 1 -S 100 -s lhcb2 -p /run/fmc/logLHCb2sum.fifo"')
        print pcAdd(h, cmd='LogLHCb2       ' + _scripts + 'LogCollect.sh "-l 1 -s lhcb2 -o /run/fmc/logLHCb2sum.fifo"')
    if LHCbA:
        print pcAdd(h, cmd='LogLHCbASrv    ' + _scripts + 'LogServer.sh "' + h + ' -l 1 -S 100 -s lhcba -p /run/fmc/logLHCbA.fifo"')
        print pcAdd(h, cmd='LogLHCbASumSrv ' + _scripts + 'LogServer.sh "' + s + ' -l 1 -S 100 -s lhcba -p /run/fmc/logLHCbAsum.fifo"')
        print pcAdd(h, cmd='LogLHCbA       ' + _scripts + 'LogCollect.sh "-l 1 -s lhcba -o /run/fmc/logLHCbAsum.fifo"')


#------------------------------------------------------------------------------
def stopPVSSControlsLogs(host):
    print pcRm(host, 'LogPVSS')
    print pcRm(host, 'LogPVSSSrv')

#------------------------------------------------------------------------------


def startPVSSControlsLogs(host, project, next_level_server):
    s = next_level_server
    print pcAdd(host, cmd='LogPVSSSrv ' + _scripts + 'LogServer.sh "' + s + ' -l 1 -S 100 -s pvss -p /run/fmc/logPVSS.fifo"')
    print pcAdd(host, cmd='LogPVSS -E /run/fmc/logPVSS.fifo -O /run/fmc/logPVSS.fifo ' + _scripts + 'PVSSlogger.sh ' + project)

#------------------------------------------------------------------------------


def stopPVSSConfigLogs(host):
    print pcRm(host, 'LogPVSSConfigSrv')

#------------------------------------------------------------------------------


def startPVSSConfigLogs(host, next_level_server):
    s = next_level_server
    print pcAdd(host, cmd='LogPVSSConfigSrv ' + _scripts + 'LogServer.sh "' + s + ' -l 1 -S 100 -s pvssconfig -p /run/fmc/logPVSSConfig.fifo"')

#------------------------------------------------------------------------------


def stopControlsNodeTasks(host, next_level_server=None, gaudilog=1):
    if use_pub is not None:
        print pcRm(host, utgid='ROcollect')
        print pcRm(host, utgid='ROpublish')
        print pcRm(host, utgid='TaskSupervisor')
    if use_log is not None:
        print pcRm(host, utgid='LogCollect')
    if next_level_server is not None:
        if use_log is not None:
            print pcRm(host, utgid='LogSumDefSrv')
            print pcRm(host, utgid='LogDef')
    print pcRm(host, utgid='webDID')
    print pcRm(host, utgid='PropertyServer')

#------------------------------------------------------------------------------


def startControlsNodeTasks(host, next_level_server=None, gaudilog=1):
    h = host
    if next_level_server is not None:
        if use_log is not None:
            print pcAdd(h, cmd=' LogSumDefSrv   ' + _scripts + 'LogServer.sh "' + next_level_server + ' -l 3 -S 100 -s fmc  -p /run/fmc/logsum.fifo"')
            print pcAdd(h, cmd=' LogDef         ' + _scripts + 'LogCollect.sh "-l 3 -s fmc -o /run/fmc/logsum.fifo"')
    if use_pub is not None:
        print pcAdd(h, cmd=' ROcollect -n root -g root ' + _scripts + 'ROMONCollect.sh')
        print pcAdd(h, cmd=' pingSrv_u ' + _scripts + 'PingSrv.sh')
        if next_level_server is not None:
            print 'sleep 1;'
            # Set the update time to something, which inhibits things getting in phase. ie 1376 insetad of 1500
            print pcAdd(h, cmd=' ROpublish ' + _scripts + 'MONPublish.sh "' + next_level_server + ' -statDelay=8000 -mbmDelay=1376"')
            print pcAdd(h, cmd=' TaskSupervisor ' + _scripts + 'TaskSupervisor.sh')
    print pcAdd(h, cmd=' webDID ' + _scripts + 'webDID.sh')
    print pcAdd(host, cmd='PropertyServer '   + _scripts + 'PropertyServer.sh')


#------------------------------------------------------------------------------


def stopTorrentTasks(host):
    print pcRm(host, utgid='TorrentLoader')

#------------------------------------------------------------------------------


def startTorrentTasks(host, controlsPC):
    print pcAdd(host, cmd=' TorrentLoader ' + _scripts + 'BitTorrentLoader.sh')

#------------------------------------------------------------------------------


def stopHLTControlsTasks(host):
    stopControlsNodeTasks(host, next_level_server, None)
    stopGaudiControlsLogs(host)
    stopPVSSControlsLogs(host)
    print pcRm(host, utgid='HLTEQBridge')

#------------------------------------------------------------------------------


def startHLTControlsTasks(host, next_level_server, with_pvss=True):
    startControlsNodeTasks(host, next_level_server, gaudilog=None)
    startGaudiControlsLogs(host, next_level_server)
    if with_pvss:
        startPVSSControlsLogs(host, host.upper(), next_level_server)
    print pcAdd(host, cmd=' HLTEQBridge ' + _scripts + 'HLTEQBridge.sh')

#------------------------------------------------------------------------------


def stopHLTTopTasks(host, next_level_server):
    stopControlsNodeTasks(host, next_level_server, None)
    stopGaudiControlsLogs(host)
    stopPVSSControlsLogs(host)
    stopPVSSConfigLogs(host)

#------------------------------------------------------------------------------


def startHLTTopTasks(host, next_level_server):
    startControlsNodeTasks(host, next_level_server, gaudilog=None)
    startGaudiControlsLogs(host, next_level_server)
    startPVSSControlsLogs(host, 'HLT', next_level_server)
    startPVSSConfigLogs(host, next_level_server)

#------------------------------------------------------------------------------


def stopSliceControlsTasks(host, next_level_server, slice, num=16):
    stopControlsNodeTasks(host, next_level_server, None)
    stopGaudiControlsLogs(host, gaudi=0, LHCb1=0, LHCb2=0, LHCbA=0)
    stopSliceLogs(host, slice, num)
    stopSliceCollectors(host, slice, num)
    stopPVSSControlsLogs(host)
    stopPVSSConfigLogs(host)

#------------------------------------------------------------------------------


def startSliceControlsTasks(host, next_level_server, slice, num=16):
    startControlsNodeTasks(host, next_level_server, gaudilog=None)
    startGaudiControlsLogs(host, next_level_server,
                           gaudi=0, LHCb1=0, LHCb2=0, LHCbA=0)
    startSliceLogs(host, host, slice, num)
    startSliceCollectors(host, next_level_server, slice, num)
    startPVSSControlsLogs(host, slice.upper(), next_level_server)
    startPVSSConfigLogs(host, next_level_server)

#------------------------------------------------------------------------------


def stopECS03Tasks(host):    
    print pcRm(host, utgid='LogDefaultSrv')
    if use_tan is not None:
        print pcRm(host, utgid='TANServ')
    print pcRm(host, utgid='webDID')
    print pcRm(host, utgid='FarmMon_0')
    print pcRm(host, utgid='ROcollect')
    print pcRm(host, utgid='ROpublish')
    print pcRm(host, utgid='ROBridge_0')
    print pcRm(host, utgid='ROLogger_0')
    # print pcRm(host, utgid='LHCb_ErrorLog_0')
    # print pcRm(host, utgid='FEST_ErrorLog_0')
    # print pcRm(host, utgid='TDET_ErrorLog_0')
    # print pcRm(host, utgid='RICH_ErrorLog_0')
    # print pcRm(host, utgid='CALO_ErrorLog_0')
    print pcRm(host, utgid='TaskSupervisor')
    print pcRm(host, utgid='OrbitedSrv_0')
    # print pcRm(host, utgid='OrbitedSrv_1')
    # print pcRm(host, utgid='OrbitedSrv_2')
    print pcRm(host, utgid='ActiveMQ_0')
    print pcRm(host, utgid='StatusStomp_0')
    print pcRm(host, utgid='StatusMQTT_0')
    # print pcRm(host, utgid='FarmStomp_0')
    print pcRm(host, utgid='ElogExtract')
    print pcRm(host, utgid='ElogPublish')
    print pcRm(host, utgid='BootMonSrv_0')
    print pcRm(host, utgid='FarmStatSrv_0')
    print pcRm(host, utgid='FarmStatSrv_1')
    print pcRm(host, utgid='AMQWatch_0')
    print pcRm(host, utgid='Hlt_DeferSrv')
    print pcRm(host, utgid='Hlt_Hlt1Srv')
    print pcRm(host, utgid='Hlt1Srv2fmc01')
    print pcRm(host, utgid='HLTFileEqualizer')
    print pcRm(host, utgid='FileStatistics')
    print pcRm(host, utgid='FarmStatusTrans')
    print pcRm(host, utgid='SwDataScan')
    print pcRm(host, utgid='CleanOptions')
    print pcRm(host, utgid='PropertyServer')
    print pcRm(host, utgid='PropertyInfo')
    print pcRm(host, utgid='DidServer')
    print pcRm(host, utgid='webDID_ECS01')
    print pcRm(host, utgid='webDID_ECS05')

#------------------------------------------------------------------------------


def startECS03Tasks(host):
    print pcAdd(host, cmd='LogDefaultSrv ' + _scripts + 'LogServer.sh "' + host + ' -l 1 -S 100 -s default -p /tmp/logSrv.fifo"')
    if use_tan is not None:
        print pcAdd(host, cmd='TANServ ' + _scripts + 'TanServ.sh')
    print pcAdd_root(host, cmd='ROcollect   ' + _scripts + 'ROMONCollect.sh')
    print pcAdd(host, cmd='ROpublish ' + _scripts + 'MONPublish.sh "ecs03 -statDelay=8000 -mbmDelay=1500"')
    print pcAdd(host, cmd='ROBridge_0 ' + _scripts + 'ROBridge.sh')
    print pcAdd(host, cmd='ROLogger_0 ' + _scripts + 'ROLogger.sh')
    print pcAdd(host, cmd='webDID ' + _scripts + 'webDID.sh')
    # print pcAdd(host,cmd='LHCb_ErrorLog_0 '+_scripts+'PartitionLog.sh LHCb')
    # print pcAdd(host,cmd='FEST_ErrorLog_0 '+_scripts+'PartitionLog.sh FEST')
    # print pcAdd(host,cmd='TDET_ErrorLog_0 '+_scripts+'PartitionLog.sh TDET')
    # print pcAdd(host,cmd='RICH_ErrorLog_0 '+_scripts+'PartitionLog.sh RICH')
    # print pcAdd(host,cmd='CALO_ErrorLog_0 '+_scripts+'PartitionLog.sh CALO')
    print pcAdd(host, cmd='TaskSupervisor ' + _scripts + 'TaskSupervisor.sh')
    print pcAdd(host, cmd='FarmStatSrv_0 ' + _scripts + 'FarmStatSrv.sh -target=fmc01 -mbm')
    print pcAdd(host, cmd='FarmStatSrv_1 ' + _scripts + 'FarmStatSrv.sh -target=ecs03 -mbm -cpu')
    #
    # Online displays using MorbidQ:
    # print pcAdd(host,u='frankm',cmd='OrbitedSrv '+_scripts+'OrbitedSrv.sh orbited/orbited.morbidQ.cfg')
    # print pcAdd(host,cmd='StatusStomp_0 '+_scripts+'StompSrv.sh ecs04 -topic=/topic/status',X=5,M=1)
    #
    # ActiveMQ has some problems with the C-client, when it comes to dropped connections.
    #
    print pcAdd(host, cmd='ActiveMQ_0 ' + _scripts + 'ActiveMQ.sh ecs3', X=5, M=1)
    print pcAdd(host, cmd='StatusStomp_0 ' + _scripts + 'StompSrv.sh ecs04 -topic=/topic/status -protocol=AMQ', X=5, M=1)
    print pcAdd(host, cmd='StatusMQTT_0 ' + _scripts + 'StompSrv.sh ecs04 -topic=/topic/item -protocol=MQTT', X=5, M=1)
    print pcAdd(host, u='online', cmd='OrbitedSrv_0 ' + _scripts + 'OrbitedSrv.sh orbited/orbited.activeMQ.0.cfg')
    # print pcAdd(host,u='frankm',cmd='OrbitedSrv_1 '+_scripts+'OrbitedSrv.sh orbited/orbited.activeMQ.1.cfg')
    # print pcAdd(host,u='frankm',cmd='OrbitedSrv_2 '+_scripts+'OrbitedSrv.sh orbited/orbited.activeMQ.2.cfg')
    # print pcAdd(host,cmd='FarmStomp_0  '+_scripts+'StompSrv.sh ecs03 -topic=/topic/farm',X=10,M=1)
    print pcAdd(host, cmd='ElogExtract   ' + _scripts + 'elog_extract.py')
    print pcAdd(host, cmd='ElogPublish   ' + _scripts + 'elog_publish.sh ecs04')
    print pcAdd(host, cmd='BootMonSrv_0  ' + _scripts + 'BootMonSrv.sh')
    print pcAdd(host, cmd='AMQWatch_0    ' + _scripts + 'AMQWatch.sh')
    print pcAdd(host, cmd='Hlt_DeferSrv  ' + _scripts + 'HltDeferSrv.sh -listen=/ROpublish/HLTDefer -publish=/HLT/Deferred')
    print pcAdd(host, cmd='Hlt_Hlt1Srv   ' + _scripts + 'HltDeferSrv.sh -listen=/ROpublish/HLT1 -publish=/HLT/HLT1')
    print pcAdd(host, cmd='Hlt_Hlt1DQSrv ' + _scripts + 'HltDeferSrv.sh -listen=/ROpublish/HLT1 -publish=/HLT/HLT1 -dis_dns=mona10')
    # Special service for HLT2 Monitoring
    print pcAdd(host, cmd='Hlt1Srv2fmc01 ' + _scripts + 'HltDeferSrv.sh -listen=/ROpublish/HLT1 -publish=/HLT/HLT1 -dis_dns=fmc01 -dic_dns=ecs03')
    print pcAdd(host, cmd='FileStatistics '   + _scripts + 'FileStatistics.sh')
    print pcAdd(host, cmd='HLTFileEqualizer ' + _scripts + 'HLTFileEqualizer.sh')
    print pcAdd(host, cmd='FarmStatusTrans '  + _scripts + 'FarmStatusTrans.sh')
    print pcAdd(host, cmd='SwDataScan '       + _scripts + 'SwDataScan.sh')
    print pcAdd(host, cmd='CleanOptions '     + _scripts + 'clean.nfs.sh')
    print pcAdd(host, cmd='PropertyServer '   + _scripts + 'PropertyServer.sh')
    print pcAdd(host, cmd='PropertyInfo '     + _scripts + 'PropertyInfo.sh')
    print pcAdd(host, cmd='DidServer '        + _scripts + 'DidServer.sh')
    print pcAdd(host, cmd='webDID_ecs01    -DDIM_DNS_NODE=ecs01    -DDIM_DID_PORT=2701 ' + _scripts + 'webDID.sh')
    print pcAdd(host, cmd='webDID_ecs05    -DDIM_DNS_NODE=ecs05    -DDIM_DID_PORT=2702 ' + _scripts + 'webDID.sh')
    print pcAdd(host, cmd='webDID_echvc01  -DDIM_DNS_NODE=echvc01  -DDIM_DID_PORT=2703 ' + _scripts + 'webDID.sh')
    print pcAdd(host, cmd='webDID_tmudaq01 -DDIM_DNS_NODE=tmudaq01 -DDIM_DID_PORT=2704 ' + _scripts + 'webDID.sh')
    print pcAdd(host, cmd='webDID_vedaqa01 -DDIM_DNS_NODE=vedaqa01 -DDIM_DID_PORT=2705 ' + _scripts + 'webDID.sh')
    #print pcAdd(host, cmd='webDID_veecs01  -DDIM_DNS_NODE=veecs01  -DDIM_DID_PORT=2706 ' + _scripts + 'webDID.sh')
    #print pcAdd(host, cmd='webDID_veecs02  -DDIM_DNS_NODE=veecs02  -DDIM_DID_PORT=2707 ' + _scripts + 'webDID.sh')
    print pcAdd(host, cmd='webDID_itecs01  -DDIM_DNS_NODE=itecs01  -DDIM_DID_PORT=2708 ' + _scripts + 'webDID.sh')
    print pcAdd(host, cmd='webDID_ttecs01  -DDIM_DNS_NODE=ttecs01  -DDIM_DID_PORT=2709 ' + _scripts + 'webDID.sh')
    print pcAdd(host, cmd='webDID_otecs01  -DDIM_DNS_NODE=otecs01  -DDIM_DID_PORT=2710 ' + _scripts + 'webDID.sh')
    print pcAdd(host, cmd='webDID_riecs01  -DDIM_DNS_NODE=riecs01  -DDIM_DID_PORT=2711 ' + _scripts + 'webDID.sh')
    print pcAdd(host, cmd='webDID_ececs01  -DDIM_DNS_NODE=caecs01  -DDIM_DID_PORT=2712 ' + _scripts + 'webDID.sh')
    print pcAdd(host, cmd='webDID_muecs01  -DDIM_DNS_NODE=muecs01  -DDIM_DID_PORT=2713 ' + _scripts + 'webDID.sh')
    print pcAdd(host, cmd='webDID_muhva01  -DDIM_DNS_NODE=muhva01  -DDIM_DID_PORT=2714 ' + _scripts + 'webDID.sh')
    print pcAdd(host, cmd='webDID_muhvc01  -DDIM_DNS_NODE=muhvc01  -DDIM_DID_PORT=2715 ' + _scripts + 'webDID.sh')
    print pcAdd(host, cmd='webDID_othv01   -DDIM_DNS_NODE=othv01   -DDIM_DID_PORT=2716 ' + _scripts + 'webDID.sh')
    print pcAdd(host, cmd='webDID_otdaqa01 -DDIM_DNS_NODE=otdaqa01 -DDIM_DID_PORT=2717 ' + _scripts + 'webDID.sh')
    print pcAdd(host, cmd='webDID_otdaqc01 -DDIM_DNS_NODE=otdaqc01 -DDIM_DID_PORT=2718 ' + _scripts + 'webDID.sh')

    print pcAdd(host, cmd='webDID_infdai01 -DDIM_DNS_NODE=infdai01 -DDIM_DID_PORT=2730 ' + _scripts + 'webDID.sh')
    #print pcAdd(host, cmd='webDID_infdai02 -DDIM_DNS_NODE=infdai02 -DDIM_DID_PORT=2731 ' + _scripts + 'webDID.sh')
    print pcAdd(host, cmd='webDID_infmag01 -DDIM_DNS_NODE=infmag01 -DDIM_DID_PORT=2732 ' + _scripts + 'webDID.sh')

#------------------------------------------------------------------------------
def startLogSrvTasks(host):
    print pcAdd("log*", cmd='LogDefaultSrv ' + _scripts + 'LogServer.sh "' + host + ' -l 1 -S 100 -s default -p /run/fmc/logSrv.fifo"')
    print pcAdd("log*", cmd='TANServ   ' + _scripts + 'TanServ.sh')
    print pcAdd('log*', cmd='pingSrv_u ' + _scripts + 'PingSrv.sh')
    print pcAdd_root("log*", cmd='ROcollect   ' + _scripts + 'ROMONCollect.sh')
    print pcAdd('logspartlhcb01',  u='online', g='onliners', cmd='PartitionLog_LHCb  '  + _scripts + 'PartitionLog.sh LHCb')
    print pcAdd('logspartlhcb101', u='online', g='onliners', cmd='PartitionLog_LHCb1 ' + _scripts + 'PartitionLog.sh LHCb1')
    ##print pcAdd('logspartlhcb201', u='online', g='onliners', cmd='PartitionLog_LHCb2 ' + _scripts + 'PartitionLog.sh LHCb2')
    print pcAdd('logspartlhcba01', u='online', g='onliners', cmd='PartitionLog_LHCbA ' + _scripts + 'PartitionLog.sh LHCbA')
    print pcAdd('logspartfest01',  u='online', g='onliners', cmd='PartitionLog_FEST  ' + _scripts + 'PartitionLog.sh FEST')
    print pcAdd('logspartfest01',  u='online', g='onliners', cmd='PartitionLog_LHCb2 ' + _scripts + 'PartitionLog.sh LHCb2')
    print pcAdd('logspartvelo01',  u='online', g='onliners', cmd='PartitionLog_VELO  ' + _scripts + 'PartitionLog.sh VELO')
    print pcAdd('logspartit01',    u='online', g='onliners', cmd='PartitionLog_IT    ' + _scripts + 'PartitionLog.sh IT')
    print pcAdd('logsparttt01',    u='online', g='onliners', cmd='PartitionLog_TT '    + _scripts + 'PartitionLog.sh TT')
    print pcAdd('logspartot01',    u='online', g='onliners', cmd='PartitionLog_OT '    + _scripts + 'PartitionLog.sh OT')
    print pcAdd('logsparttdet01',  u='online', g='onliners', cmd='PartitionLog_TDET '  + _scripts + 'PartitionLog.sh TDET')
    print pcAdd('logspartcalo01',  u='online', g='onliners', cmd='PartitionLog_CALO '  + _scripts + 'PartitionLog.sh CALO')
    print pcAdd('logspartrich01',  u='online', g='onliners', cmd='PartitionLog_RICH '  + _scripts + 'PartitionLog.sh RICH')
    print pcAdd('logspartrich101', u='online', g='onliners', cmd='PartitionLog_RICH1 ' + _scripts + 'PartitionLog.sh RICH1')
    print pcAdd('logspartrich201', u='online', g='onliners', cmd='PartitionLog_RICH2 ' + _scripts + 'PartitionLog.sh RICH2')
    print pcAdd('logsrv01', cmd=' ROpublish ' + _scripts + 'MONPublish.sh ecs03 -statDelay=8000 -mbmDelay=1376"')
    print pcAdd('logsrv01', cmd=' TaskSupervisor ' + _scripts + 'TaskSupervisor.sh')
    print pcAdd('logsrv01', cmd=' webDID ' + _scripts + 'webDID.sh')

#------------------------------------------------------------------------------
def stopLogSrvTasks(host):
    print pcRm("log*",            utgid='TANServ')
    print pcRm("log*",            utgid='ROcollect')
    print pcRm("logsrv01",        utgid='webDID')
    print pcRm("logsrv01",        utgid='ROpublish')
    print pcRm("logsrv01",        utgid='TaskSupervisor')
    print pcRm('logspartlhcb01',  utgid='PartitionLog_LHCb')
    print pcRm('logspartlhcb101', utgid='PartitionLog_LHCb1')
    print pcRm('logspartlhcb201', utgid='PartitionLog_LHCb2')
    print pcRm('logspartlhcba01', utgid='PartitionLog_LHCbA')
    print pcRm('logspartfest01',  utgid='PartitionLog_FEST')
    print pcRm('logspartvelo01',  utgid='PartitionLog_VELO')
    print pcRm('logspartit01',    utgid='PartitionLog_IT')
    print pcRm('logsparttt01',    utgid='PartitionLog_TT')
    print pcRm('logspartot01',    utgid='PartitionLog_OT')
    print pcRm('logsparttdet01',  utgid='PartitionLog_TDET')
    print pcRm('logspartcalo01',  utgid='PartitionLog_CALO')
    print pcRm('logspartrich01',  utgid='PartitionLog_RICH')
    print pcRm('logspartrich101', utgid='PartitionLog_RICH1')
    print pcRm('logspartrich201', utgid='PartitionLog_RICH2')
    print pcRm('log*',            utgid='LogDefaultSrv')

#------------------------------------------------------------------------------


def stopRecoControlsTasks(host):
    print pcRm(host, 'LogRecDbMgr')

#------------------------------------------------------------------------------


def startRecoControlsTasks(host, top_logger_host):
    h = host
    s = top_logger_host
    print pcAdd(h, cmd='LogRecDbMgr    ' + _scripts + 'LogServer.sh "' + s + ' -l 1 -S 100 -s recomgr -p /run/fmc/logRecManager.fifo"')

#------------------------------------------------------------------------------


def check(match, test):
    match = re.match(match.upper(), test.upper())
    if match:
        if match.pos == 0 and match.endpos == len(test):
            return 1
    return None

#------------------------------------------------------------------------------


def startTasks(host):
    HOST = host.upper()
    h = HOST[:6]
    node = host.lower()
    if check('hlt[a-m][0-9][0-9][0-9][0-9]', HOST):
        return
    elif check('cal[a-f][0-9][0-9][0-9][0-9]', HOST):
        return
    elif check('store[rs][et][cr][mv]0[1234]', HOST):
        return
    elif check('mon[a-f][0-9][0-9][0-9][0-9]', HOST):
        return
    elif node.upper().find('logspart') == 0:
        return

    try:
        os.stat('/cvmfs/lhcb.cern.ch')
    except Exception, X:
        print 'echo "No special tasks can be registered for ' + node + ' if cvmfs is not present.";'
        return

    time.sleep(3)
    print 'echo "This is the node:\'' + host + '\'";'
    print 'echo "top_farm_host:   \'' + top_farm_host + '\'";'
    print pcRm(node, utgid='memSrv_u')
    print pcRm(node, utgid='smartSrv_u')
    startFMC(node + '*')
    print pcAdd(node+'*', u='beat',g='onliners',cmd=' SSSRecover /home/beat/ssscheck.sh', Nice=-15)

    if check('ecs03', HOST):
        print 'echo "ERROR: Triggered node ecs03!";'
    if check('hlt[a-m][0-9][0-9]', HOST):

        print 'echo "This is a farm controls node:' + host + '";'
        startProcessingNodeTasks(node + '??', host, top_logger_host)
        startHLTControlsTasks(node, top_logger_host)
        startTorrentTasks(node, top_logger_host)
        startTorrentTasks(node + '??', host)
        print pcAdd(node, cmd=' TorrentPublish ' + _scripts + 'BitTorrentPublish.sh')
        print pcAdd(node + '[0-9][0-9]', u='root', g='root', cmd=' FileDeleter ' + _scripts + 'FileDeleter.sh')
        # added by Beat
        print pcAdd(node + '[0-9][0-9]', cmd=' BusyMon /group/online/dataflow/cmtuser/OnlineRelease/Online/FarmConfig/job/NodeBusyMon.sh', Nice=-15)
        # added by Beat
        print pcAdd(node, cmd=' BusyMon /group/online/dataflow/cmtuser/OnlineRelease/Online/FarmConfig/job/SFBusyMon.sh', Nice=-15)

    elif check('cal[a-f][0-9][0-9]', HOST):

        print 'echo "This is a farm controls node:' + host + '";'
        startProcessingNodeTasks(node + '??', host, top_logger_host)
        startHLTControlsTasks(node, top_logger_host)
        print pcAdd(node + '01', cmd='LHCb_CALD0701_CalibHistos_0 ' + _scripts + 'HistDimBridge.sh  CalibHistBridge.opts')
        print pcAdd(node, cmd='AnalysisBridge_0 ' + _scripts + 'AnalysisBridge.sh mona08.lbdaq.cern.ch cald07.lbdaq.cern.ch "LHCbStatus/*"')

    elif check('hlt0[1-2]', HOST):

        print pcAdd('hlt01', cmd='LogDefaultSrv ' + _scripts + 'LogServer.sh hlt01 -l 1 -S 100 -s default -p /tmp/logSrv.fifo"')
        print pcAdd('hlt02', cmd='LogDefaultSrv ' + _scripts + 'LogServer.sh hlt01 -l 1 -S 100 -s default -p /tmp/logSrv.fifo"')
        print 'echo "This is the TOP farm controls node:' + host + '";'
        startProcessingNodeTasks('hlt0?', 'hlt01', top_logger_host)
        startHLTTopTasks('hlt01', top_logger_host)
        print pcAdd('hlt02', cmd=' pingSrv_u ' + _scripts + 'PingSrv.sh')
        print pcAdd('hlt02', cmd=' CondDBXfer_0 ' + _scripts + 'internal/condDBXfer.sh')
        print pcAdd('hlt02', cmd=' CondDBCheck_0 /group/online/AligWork/CheckConstants/checkConstants.sh')
        print pcAdd('hlt02', cmd=' CondDBXfer_RunTICK_0 ' + _scripts + 'internal/condDBXfer_doRunTick.sh')
        print pcAdd('hlt02', cmd=' LHCb_HLT02_MoniOnlineAlig_0 /group/online/AligWork/MoniPlots/runAlignMonitor.sh')
        # added by Beat
        print pcAdd('hlt02', cmd=' BusyMon /group/online/dataflow/cmtuser/OnlineRelease/Online/FarmConfig/job/TopBusyMon.sh')
        # added by Beat
        print pcAdd('hlt02', cmd=' BusyPub /group/online/dataflow/cmtuser/OnlineRelease/Online/FarmConfig/job/BusyPub.sh')

    elif check('hlt02aaaaa', HOST + 'bbb'):

        print 'echo "This is the farm node:' + host + '";'
        print pcAdd_root(host, cmd='ROcollect -N hlt02   ' + _scripts + 'ROMONCollect.sh')
        print pcAdd(host, cmd='LogGaudiSrv -N hlt02 ' + _scripts + 'LogServer.sh ' + controlsPC + ' -S 100 -l 1 -s gaudi -p /run/fmc/logGaudi.fifo')
        print pcAdd(host, cmd='LogLHCb1Srv -N hlt02 ' + _scripts + 'LogServer.sh ' + controlsPC + ' -S 100 -l 1 -s lhcb1 -p /run/fmc/logLHCb1.fifo')
        print pcAdd(host, cmd='LogLHCb2Srv -N hlt02 ' + _scripts + 'LogServer.sh ' + controlsPC + ' -S 100 -l 1 -s lhcb2 -p /run/fmc/logLHCb2.fifo')
        print pcAdd(host, cmd='LogLHCbASrv -N hlt02 ' + _scripts + 'LogServer.sh ' + controlsPC + ' -S 100 -l 1 -s lhcba -p /run/fmc/logLHCbA.fifo')
        # startProcessingNodeTasks(host,'hlt01',top_logger_host)
        print pcAdd(host, cmd=' CondDBXfer_0 -N hlt02 ' + _scripts + 'internal/condDBXfer.sh')
        print pcAdd(host, cmd=' CondDBCheck_0 -N hlt02 /group/online/AligWork/CheckConstants/checkConstants.sh')
        print pcAdd(host, cmd=' CondDBXfer_RunTICK_0 -N hlt02 ' + _scripts + 'internal/condDBXfer_doRunTick.sh')
        print pcAdd(host, cmd=' MoniOnlineAlig_0 -N hlt02 /group/online/AligWork/MoniPlots/runAlignMonitor.sh')
        # added by Beat
        print pcAdd(host, cmd=' BusyMon -N hlt02 /group/online/dataflow/cmtuser/OnlineRelease/Online/FarmConfig/job/TopBusyMon.sh')
        # added by Beat
        print pcAdd(host, cmd=' BusyPub -N hlt02 /group/online/dataflow/cmtuser/OnlineRelease/Online/FarmConfig/job/BusyPub.sh')

    elif check('mona08', HOST):

        print 'echo "This is a monitoring controls node:' + host + '";'
        startSliceNodeTasks(node + '??', node, 'Monitoring', 16)
        startSliceControlsTasks(node, top_logger_host, 'Monitoring', 16)
        cmd = 'LHCb_MONA0802_WebGLSrv_0 -D LOGFIFO=/run/fmc/logGaudi.fifo -D PARTITION=LHCb '
        cmd = cmd + '/group/online/dataflow/cmtuser/WebGLDisplay/WebGLDisplaySys/scripts/runWebGLDisplay.sh'
        cmd = cmd + ' WebGLDisplay Class2 WebGLDisplaySrv mona08'
        print pcAdd('mona0802', fifo='/run/fmc/logGaudi.fifo', cmd=cmd)
        print pcAdd('mona0802', fifo='/run/fmc/logGaudi.fifo', cmd='LHCb_WebGLScan_0 ' + _scripts + 'WebGLEventScanner.sh')
        ##No! print pcAdd('mona0801', cmd='TANServ_CN ' + _scripts + 'TanServCN.sh')

    elif check('mona09', HOST):

        print 'echo "  [FATAL] This is a reconstruction controls node:' + host + '";'
        startProcessingNodeTasks(node, host, top_logger_host)
        startSliceNodeTasks(node + '??', node, 'Reconstruction', 4)
        startSliceControlsTasks(node, top_logger_host, 'Reconstruction', 4)
        print pcAdd(node, cmd='RecHistos_0 ' + _scripts + 'HistDimBridge.sh  RecHistBridge.opts', X=30, M=15)
        print pcAdd(node, cmd='RecSaver_0 ' + _scripts + 'HistDimSaver.sh  RecHistSaver.opts', X=30, M=15)
        # startTorrentTasks(node,node)
        # startTorrentTasks(node+'??',node)
        # print pcAdd(host,cmd='TorrentPublish '+_scripts+'BitTorrentPublish.sh')
        # print pcAdd('mona0901',cmd=' TorrentTracker '+_scripts+'BitTorrentTracker.sh')

    elif check('mona10', HOST):

        print 'echo "This is the data quality controls node:' + host + '";'
        startProcessingNodeTasks(node + '??', host, top_logger_host)
        startHLTControlsTasks(node, top_logger_host)
        startTorrentTasks(node, top_logger_host)
        startTorrentTasks(node + '??', host)
        print pcAdd(node, cmd=' TorrentPublish ' + _scripts + 'BitTorrentPublish.sh')
        print pcAdd("mona1001", cmd=' DQScanner ' + _scripts + 'DQScanner.sh scan', fifo='/run/fmc/logGaudi.fifo', X=20, M=20)
        # print pcAdd("mona1001", cmd=' DQChecker '+_scripts+'DQScanner.sh check',fifo='/run/fmc/logGaudi.fifo',X=20,M=20)
        print pcAdd("mona1001", cmd=' DQResults ' + _scripts + 'DQResults.sh', fifo='/run/fmc/logGaudi.fifo', X=20, M=20)
        print pcAdd("mona1001", cmd=' DQPublish ' + _scripts + 'DQPublish.sh', fifo='/run/fmc/logGaudi.fifo', X=20, M=20)
        print pcAdd("mona1001", cmd=' DQDump ' + _scripts + 'DQDump.sh', fifo='/run/fmc/logGaudi.fifo', X=20, M=20)
        # print pcAdd("mona1001", cmd=' H2Scanner '+_scripts+'H2Scanner.sh')
        print pcAdd('mona1001', cmd='LogDataQuality ' + _scripts + 'LogDataQuality.sh mona10 -S 100 -l 1', fifo='/run/fmc/logGaudi.fifo', X=20, M=20)

    elif check('mon[a-f][0-9][6-9]', HOST):

        print 'echo "This is a monitoring controls node:' + host + '";'
        startSliceNodeTasks(node + '??', node, 'Monitoring', 16)
        startSliceControlsTasks(node, top_logger_host, 'Monitoring', 16)

    elif check('storectl01', HOST):

        print 'echo "This is a storage controls node:' + host + '";'
        startFMC('store*')
        startSliceNodeTasks(
            'store[rs][et][cr][mv]0[1234]', node, 'Storage', 16)
        startSliceControlsTasks(node, top_logger_host, 'Storage', 16)
        print pcAdd('storerecv01', cmd=' TorrentTracker ' + _scripts + 'BitTorrentTracker.sh')
        print pcAdd('storerecv02', cmd=' TorrentTracker ' + _scripts + 'BitTorrentTracker.sh')
        print pcAdd('storerecv03', cmd=' TorrentTracker ' + _scripts + 'BitTorrentTracker.sh')
        print pcAdd('storerecv04', cmd=' TorrentTracker ' + _scripts + 'BitTorrentTracker.sh')
        print pcAdd(host, cmd=' TorrentPublish ' + _scripts + 'BitTorrentPublish.sh')
        print pcAdd(host, cmd=' PartitionLogStamps ' + _scripts + 'PartitionLogStamps.sh')
        # startTorrentTasks(host,host)
        startTorrentTasks('storerecv01', host)
        startTorrentTasks('storerecv02', host)
        startTorrentTasks('storerecv03', host)
        startTorrentTasks('storerecv04', host)
        # startTorrentTasks('storestrm01',host)
        # startTorrentTasks('storestrm02',host)
        # startTorrentTasks('storestrm03',host)
        # startTorrentTasks('storestrm04',host)

    elif check('checkpoint01', HOST):
        startProcessingNodeTasks(node, host, top_logger_host)

    elif check('fmc01', HOST):
        print pcAdd_root(node, cmd='ipmi_farm_slow /home/beat/ipmistart.sh "-c /home/beat/ipmi_slow_nodes.config -n ipmi_farm_slow -p /FMC"')
        print pcAdd_root(node, cmd='ipmi_farm_medium /home/beat/ipmistart.sh "-c /home/beat/ipmi_medium_nodes.config -n ipmi_farm_medium -p /FMC"')
        print pcAdd_root(node, cmd='ipmi_farm_fast /home/beat/ipmistart.sh "-c /home/beat/ipmi_fast_nodes.config -n ipmi_farm_fast -p /FMC"')
        print pcAdd_root(node, cmd='ipmi_farm_faster /home/beat/ipmistart.sh "-c /home/beat/ipmi_faster_nodes.config -n ipmi_faster_nodes -p /FMC"')
        print pcAdd_root(node, cmd='ipmi_rest /home/beat/ipmistart.sh "-c /home/beat/ipmi_rest.conf -n ipmi_rest -p /FMC"', X=20, M=15)
        print pcAdd_root(node, cmd='ipmi_mona /home/beat/ipmistart.sh "-c /home/beat/ipmi_MONA.conf -n ipmi_mona -p /FMC"', X=20, M=15)
        print pcAdd(node, cmd=' webDID ' + _scripts + 'webDID.sh')
        print 'echo "Nothing to be done yet....";'

    elif check('ecs01', HOST):
        # startInfrastructure(host)
        pass
    elif check('ecs02', HOST):
        startInfrastructure(host)
    elif check('ecs03', HOST):
        startECS03Tasks(host)
    elif check('ecs04', HOST):
        print 'echo "This is the TOP host:' + host + '";'
        startProcessingNodeTasks(node, host, top_logger_host)
        startHLTControlsTasks(node, None)  # top_logger_host)
    elif node == 'hist03':
        print pcAdd(node, cmd=' webDID ' + _scripts + 'webDID.sh')
    elif node == 'hist02':
        print pcAdd(node, cmd=' webDID ' + _scripts + 'webDID.sh')
    elif node == 'hist01':
        print pcAdd(node, cmd=' webDID ' + _scripts + 'webDID.sh')
        # print pcAdd(node,cmd='LogRichCalibSrv '+_scripts+'LogServer.sh '+node+' -S 100 -l 1 -m '+node+' -s richcalib -p /run/fmc/logRichCalib.fifo')

        # print pcAdd(node,cmd='LogRichCalib    '+_scripts+'LogCollectFile.sh "-l 1 -m '+node+' -s richcalib -O /localdisk/logs/RichCalib.log"')
        # print pcAdd(node,cmd='RichOnlineCalib_0 /group/rich/scripts/RichOnlineCalib.sh', fifo='/run/fmc/logRichCalib.fifo')
        """
        print pcAdd(node, u='online', g='onliners', cmd='cameraFIFO           /opt/FMC/sbin/logSrv -S 100 -m ' + node + ' -p /run/fmc/logcamera.fifo -s camera')
        print pcAdd(node, u='online', g='onliners', cmd='camerawebFIFO        /opt/FMC/sbin/logSrv -S 100 -m ' + node + ' -p /run/fmc/logcameraweb.fifo -s cameraweb')
        print pcAdd(node, u='online', g='onliners', cmd='cameraofflineFIFO    /opt/FMC/sbin/logSrv -S 100 -m ' + node + ' -p /run/fmc/logcameraoffline.fifo -s cameraoffline')
        print pcAdd(node, u='online', g='onliners', cmd='RichOnlineCalibFIFO  /opt/FMC/sbin/logSrv -S 100 -m ' + node + ' -p /run/fmc/logRichOnlineCalib.fifo -s RichOnlineCalib_0')
        print pcAdd(node, u='online', g='onliners', cmd='RichOnlineCalibLOG   /opt/FMC/bin/logViewer -l 1 -m hist01 -s RichOnlineCalib_0 -O /localdisk/logs/RichCalib.log')

        print pcAdd(node, u='online', g='onliners', cmd='camera               /group/rich/scripts/StartCameraServer.sh',       fifo='/run/fmc/logcamera.fifo')
        print pcAdd(node, u='online', g='onliners', cmd='cameraweb            /group/rich/scripts/StartCameraWebServer.sh',    fifo='/run/fmc/logcameraweb.fifo')
        print pcAdd(node, u='online', g='onliners', cmd='cameraoffline        /group/rich/scripts/StartCameraServerOffline.sh', fifo='/run/fmc/logcameraoffline.fifo')
        print pcAdd(node, u='online', g='onliners', cmd='RichOnlineCalib_0    /group/rich/scripts/RichOnlineCalib.sh',         fifo='/run/fmc/logRichOnlineCalib.fifo')

        # Calo STUFF:
        print pcAdd(node, u='online', g='onliners', cmd='CaloCalibAnaFIFO     /opt/FMC/sbin/logSrv -S 100 -m ' + node + ' -p /run/fmc/logCaloCalibAna.fifo -s LHCb_CALD0701_CaloCalibAnalysis_00')
        print pcAdd(node, u='online', g='onliners', cmd='CaloOccupancyAnaFIFO /opt/FMC/sbin/logSrv -S 100 -m ' + node + ' -p /run/fmc/logCaloOccupancyAna.fifo -s LHCb_MONA0801_CaloOccupancyAnalysis_00')

        print pcAdd(node, u='online', g='onliners', cmd='LHCb_CALD0701_CaloCalibAnalysis_00_DimBridge /usr/local/bin/DimBridge cald07.lbdaq.cern.ch mona08.lbdaq.cern.ch *CaloCalibAnalysis*')

        print pcAdd(node, u='online', g='onliners', cmd='LHCb_MONA0801_CaloOccupancyAction_00 /group/calo/sw/scripts/CaloOccupancyAction.sh', fifo='/run/fmc/logCaloOccupancyAction.fifo')

        # OT Calib stuff
        print pcAdd(node, cmd='LogOTCalibSrv ' + _scripts + 'LogServer.sh ' + node + ' -S 100 -l 1 -m ' + node + ' -s otcalib -p /run/fmc/logOTCalib.fifo')
        print pcAdd(node, cmd='LogOTCalib    ' + _scripts + 'LogCollectFile.sh -l 1 -m ' + node + ' -s otcalib -O /localdisk/logs/OTCalib.log')
        print pcAdd(node, cmd='OTOnlineCalib_0 /group/online/dataflow/cmtuser/AlignmentRelease/Calibration/OTOnlineCalibration/scripts/OTOnlineCalib_LHCb.sh', fifo='/run/fmc/logOTCalib.fifo')

        # MUON stuff
        print pcAdd(node, u='online', g='onliners', cmd='MuonHistMonFIFO      /opt/FMC/sbin/logSrv -S 100 -m ' + node + ' -p /run/fmc/logMuonHistMon.fifo -s MuonHistMon')
        print pcAdd(node, u='online', g='onliners', cmd='MuonHistMon          /group/online/HistTools/init/MuonHistMon', fifo='/run/fmc/logMuonHistMon.fifo')

        # HistDB stuff
        print pcAdd(node, u='online', g='onliners', cmd='HistDBAna1FIFO       /opt/FMC/sbin/logSrv -S 100 -m ' + node + ' -p /run/fmc/logHistDBAna1.fifo -s HistDBAna1')
        print pcAdd(node, u='online', g='onliners', cmd='HistDBAna2FIFO       /opt/FMC/sbin/logSrv -S 100 -m ' + node + ' -p /run/fmc/logHistDBAna1.fifo -s HistDBAna2')
        print pcAdd(node, u='online', g='onliners', cmd='HistDBAna1 /group/online/HistTools/init/histDBAna1', fifo='/run/fmc/logHistDBAna1.fifo')
        print pcAdd(node, u='online', g='onliners', cmd='HistDBAna1 /group/online/HistTools/init/histDBAna2', fifo='/run/fmc/logHistDBAna2.fifo')

        # HLT2 analysis tasks
        print pcAdd(node, u='online', g='onliners', cmd='MonRegistrarLog      /opt/FMC/sbin/logSrv -S 100 -p /run/fmc/logMonRegistrar.fifo -s MonRegistrar_0')
        print pcAdd(node, u='online', g='onliners', cmd='MonRegistrar_0       /group/hlt/monitoring/ONLINE/ONLINE_RELEASE/Online/Monitoring/scripts/MonRegistrar.sh', fifo='/run/fmc/logMonRegistrar.fifo')
        """
    elif check('inj01', HOST):
        startProcessingNodeTasks(node, host, top_logger_host)
        startHLTControlsTasks(node, top_logger_host)
        startRecoControlsTasks(node, top_logger_host)
    elif check('storeio01', HOST):
        startProcessingNodeTasks(node, host, top_logger_host)
        startHLTControlsTasks(node, top_logger_host)
        startRecoControlsTasks(node, top_logger_host)
        print pcAdd('storeio01', u='lhcbprod', g='z5', cmd='RecoDiracMgr ' + _scripts + 'ReproManager.sh DiracManager')
        print pcAdd('storeio01', u='lhcbprod', g='z5', cmd='RecoMgr      ' + _scripts + 'ReproManager.sh RecoManager')
        print pcAdd('storeio01', cmd='RecoSliceMgr      ' + _scripts + 'ReproManager.sh SliceManager')
        print pcAdd('storeio01', cmd='RecoAccountingMgr ' + _scripts + 'ReproManager.sh AccountingManager')
    elif check('logsrv01', HOST):
        print 'echo "This is the LOG SERVER host:' + host + '";'
        startLogSrvTasks(host)
    else:
        print 'echo "Got node of unknown type:', HOST, '";'
    return


def stopTasks(host):
    HOST = host.upper()
    h = HOST[:6]
    node = host.lower()

    if check('hlt[a-m][0-9][0-9][0-9][0-9]', HOST):
        return
    elif check('cal[a-f][0-9][0-9][0-9][0-9]', HOST):
        return
    elif check('store[rs][et][cr][mv]0[12]', HOST):
        return
    elif check('mon[a-f][0-9][0-9][0-9][0-9]', HOST):
        return

    stopFMC(host)
    if check('hlt[a-m][0-9][0-9]', HOST) or check('cal[a-f][0-9][0-9]', HOST):
        print 'echo "This is a farm controls node:' + host + '";'
        stopFMC(node + '*')
        stopProcessingNodeTasks(node + '??')
        stopTorrentTasks(node + '??')
        stopHLTControlsTasks(node, top_logger_host)
        print pcRm(host, utgid='TorrentPublish')
        stopTorrentTasks(host)
    elif check('hlt[0-9][0-9]', HOST):
        print 'echo "This is a farm controls node:' + host + '";'
        stopFMC(host)
        stopHLTTopTasks(host)
    elif check('mona09', HOST):
        print 'echo "This is a reconstruction controls node:' + host + '";'
        stopFMC(node + '*')
        stopSliceNodeTasks(node + '??', 'Reconstruction')
        stopSliceControlsTasks(node, top_logger_host, 'Reconstruction')
        print pcRm(host, utgid='RecHistos_0')
        print pcRm(host, utgid='TorrentPublish')
        stopTorrentTasks(host)
    elif check('mon[a-f][0-9][0-9]', HOST):
        print 'echo "This is a monitoring controls node:' + host + '";'
        stopFMC(node + '*')
        stopSliceNodeTasks(node + '??', 'Monitoring')
        stopSliceControlsTasks(node, top_logger_host, 'Monitoring')
    elif check('storectl01', HOST):
        print 'echo "This is a storage controls node:' + host + '";'
        stopFMC('store*')
        stopSliceNodeTasks('store[rs][et][cr][mv]0[1234]', 'Storage')
        stopSliceControlsTasks(host, top_logger_host, 'Storage')
        print pcRm('storestrm02', utgid='TorrentTracker')
        print pcRm(host, utgid='TorrentPublish')
        stopTorrentTasks('storestrm02')
    elif check(top_farm_host, HOST):
        print 'echo "This is the TOP farm controls node:' + host + '";'
        stopFMC(top_farm_host)
        stopControlsNodeTasks(host, top_logger_host)
    elif check('splunk', HOST):
        print 'echo "This is the LOG SERVER host:' + host + '";'
        stopLogSrvTasks(host)
    elif check('ecs03', HOST):
        print 'echo "This is the TOP LOGGER host:' + host + '";'
        stopFMC('ecs03')
        stopECS03Tasks(host)
    elif check('ecs04', HOST):
        stopFMC(node)
        stopProcessingNodeTasks(node)
        stopHLTControlsTasks(node, top_logger_host)
    elif check('fmc01', HOST):
        print 'echo "Nothing to be done yet....";'
    elif check('hist01', HOST):
        pass
    elif check('inj01', HOST) or check('storeio01', HOST):
        stopFMC(node + '*')
        stopProcessingNodeTasks(node)
        stopHLTControlsTasks(node, top_logger_host)
        stopRecoControlsTasks(node)
    elif check('logsrv01', HOST):
        print 'echo "This is the LOG SERVER host:' + host + '";'
        stopFMC(node + '*')
        stopLogSrvTasks(host)
    else:
        print 'echo "Got node of unknown type:', HOST, '";'
    return


def hostName():
    host = socket.gethostname()
    if host.find('.') > 0:
        host = host[:host.find('.')]
    return host


def start():
    try:
      os.stat('/cvmfs/lhcb.cern.ch')
      host = hostName()
      setupEnv()
      startTasks(host)
      return
    except Exception,X:
      print 'manipBootTasks will not be executed. cvmfs is not accessible:',str(X)
      return
      

def stop():
    host = hostName()
    setupEnv()
    stopTasks(host)
    return


def restart():
    host = hostName()
    setupEnv()
    stopTasks(host)
    print 'sleep 1;'
    startTasks(host)

# host_names = ['storectl01','storerecv01','storerecv02','storestrm01','storestrm02',
#              'mona08','mona0801','mona0802','mona0803','mona0804',
#              'hlt01','hlte08','hlte0801','hlte0802','hlte0803','hlte0804'
#              ]
# for i in host_names:
#  startTasks(i)
# if __name__ == "__main__":
#  print 'echo "Executing main.....";'
#  start()
