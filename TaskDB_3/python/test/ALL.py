def func(server):
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'LogGaudiSrv',\
                  'utgid': 'LogGaudiSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s gaudi -p /run/fmc/logGaudi.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb1Srv',\
                  'utgid': 'LogLHCb1Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb1 -p /run/fmc/logLHCb1.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb2Srv',\
                  'utgid': 'LogLHCb2Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb2 -p /run/fmc/logLHCb2.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCbASrv',\
                  'utgid': 'LogLHCbASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcba -p /run/fmc/logLHCbA.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'SSSRecover',\
                  'utgid': 'SSSRecover',\
                  'command': '/home/beat/ssscheck.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p -15 -n beat',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"Infrastructure", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('next_level_server', '<TOP_LOGGER>'), ('gaudilog', 1), ('group', 'Infrastructure')]
  server.assignTask("LogDefaultSrv", "Infrastructure")
  server.assignTask("LogGaudiSrv", "Infrastructure")
  server.assignTask("LogLHCb1Srv", "Infrastructure")
  server.assignTask("LogLHCb2Srv", "Infrastructure")
  server.assignTask("LogLHCbASrv", "Infrastructure")
  server.assignTask("TANServ", "Infrastructure")
  server.assignTask("pingSrv_u", "Infrastructure")
  server.assignTask("ROcollect", "Infrastructure")
  server.assignTask("SSSRecover", "Infrastructure")
  server.addTask({'task': 'TorrentLoader',\
                  'utgid': 'TorrentLoader',\
                  'command': '<SCRIPTS>/BitTorrentLoader.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"TorrentTasksWorker", "description": ""})    ### [('node', '<NODE>'), ('controlsPC', '<DIM_DNS>')]
  server.assignTask("TorrentLoader", "TorrentTasksWorker")
  server.addClass({"node_class": "HLTWorker", "description": ""})
  server.assignSet("TorrentTasksWorker", "HLTWorker")
  server.assignSet("Infrastructure", "HLTWorker")
  server.addNode({"regex": "hlt[a-m][0-9][0-9][0-9][0-9]", "description": ""})
  server.assignClass("HLTWorker","hlt[a-m][0-9][0-9][0-9][0-9]")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'LogGaudiSrv',\
                  'utgid': 'LogGaudiSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s gaudi -p /run/fmc/logGaudi.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb1Srv',\
                  'utgid': 'LogLHCb1Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb1 -p /run/fmc/logLHCb1.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb2Srv',\
                  'utgid': 'LogLHCb2Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb2 -p /run/fmc/logLHCb2.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCbASrv',\
                  'utgid': 'LogLHCbASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcba -p /run/fmc/logLHCbA.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'SSSRecover',\
                  'utgid': 'SSSRecover',\
                  'command': '/home/beat/ssscheck.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p -15 -n beat',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"Infrastructure", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('next_level_server', '<TOP_LOGGER>'), ('gaudilog', 1), ('group', 'Infrastructure')]
  server.assignTask("LogDefaultSrv", "Infrastructure")
  server.assignTask("LogGaudiSrv", "Infrastructure")
  server.assignTask("LogLHCb1Srv", "Infrastructure")
  server.assignTask("LogLHCb2Srv", "Infrastructure")
  server.assignTask("LogLHCbASrv", "Infrastructure")
  server.assignTask("TANServ", "Infrastructure")
  server.assignTask("pingSrv_u", "Infrastructure")
  server.assignTask("ROcollect", "Infrastructure")
  server.assignTask("SSSRecover", "Infrastructure")
  server.addTask({'task': 'TorrentLoader',\
                  'utgid': 'TorrentLoader',\
                  'command': '<SCRIPTS>/BitTorrentLoader.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"TorrentTasksWorker", "description": ""})    ### [('node', '<NODE>'), ('controlsPC', '<DIM_DNS>')]
  server.assignTask("TorrentLoader", "TorrentTasksWorker")
  server.addTask({'task': 'LogSumDefSrv',\
                  'utgid': 'LogSumDefSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 3 -S 100 -s fmc -p /run/fmc/logsum.fifo", 'description': ''})

  server.addTask({'task': 'LogDef',\
                  'utgid': 'LogDef',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 3 -s fmc -o /run/fmc/logsum.fifo", 'description': ''})

  server.addTask({'task': 'ROpublish',\
                  'utgid': 'ROpublish',\
                  'command': '<SCRIPTS>/MONPublish.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -statDelay=8000 -mbmDelay=1376", 'description': ''})

  server.addTask({'task': 'TaskSupervisor',\
                  'utgid': 'TaskSupervisor',\
                  'command': '<SCRIPTS>/TaskSupervisor.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'webDID',\
                  'utgid': 'webDID',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"DIMTasks", "description": ""})    ### [('host', '<NODE>')]
  server.assignTask("webDID", "DIMTasks")
  server.addTask({'task': 'PropertyServer',\
                  'utgid': 'PropertyServer',\
                  'command': '<SCRIPTS>/PropertyServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"ControlsNodeTasks", "description": ""})    ### [('node', '<NODE>'), ('next_level_server', '<TOP_LOGGER>'), ('gaudilog', None), ('slice', '')]
  server.assignTask("LogSumDefSrv", "ControlsNodeTasks")
  server.assignTask("LogDef", "ControlsNodeTasks")
  server.assignTask("ROpublish", "ControlsNodeTasks")
  server.assignTask("TaskSupervisor", "ControlsNodeTasks")
  server.assignTask("PropertyServer", "ControlsNodeTasks")
  server.addTask({'task': 'LogGaudiSumSrv',\
                  'utgid': 'LogGaudiSumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s gaudi -p /run/fmc/gaudisum.fifo", 'description': ''})

  server.addTask({'task': 'LogGaudi',\
                  'utgid': 'LogGaudi',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s gaudi -o /run/fmc/gaudisum.fifo", 'description': ''})

  server.addTask({'task': 'LogLHCb1SumSrv',\
                  'utgid': 'LogLHCb1SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s lhcb1 -p /run/fmc/logLHCb1sum.fifo", 'description': ''})

  server.addTask({'task': 'LogLHCb1',\
                  'utgid': 'LogLHCb1',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s lhcb1 -o /run/fmc/logLHCb1sum.fifo", 'description': ''})

  server.addTask({'task': 'LogLHCb2SumSrv',\
                  'utgid': 'LogLHCb2SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s lhcb2 -p /run/fmc/logLHCb2sum.fifo", 'description': ''})

  server.addTask({'task': 'LogLHCb2',\
                  'utgid': 'LogLHCb2',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s lhcb2 -o /run/fmc/logLHCb2sum.fifo", 'description': ''})

  server.addTask({'task': 'LogLHCbASumSrv',\
                  'utgid': 'LogLHCbASumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s lhcba -p /run/fmc/logLHCbAsum.fifo", 'description': ''})

  server.addTask({'task': 'LogLHCbA',\
                  'utgid': 'LogLHCbA',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s lhcba -o /run/fmc/logLHCbAsum.fifo", 'description': ''})

  server.addSet({"task_set":"GaudiControlsLogs", "description": ""})    ### [('host', '<NODE>'), ('next_level_server', '<TOP_LOGGER>'), ('gaudi', 1), ('LHCb1', 1), ('LHCb2', 1), ('LHCbA', 1), ('group', 'GaudiControlsLogs')]
  server.assignTask("LogGaudiSumSrv", "GaudiControlsLogs")
  server.assignTask("LogGaudi", "GaudiControlsLogs")
  server.assignTask("LogLHCb1SumSrv", "GaudiControlsLogs")
  server.assignTask("LogLHCb1", "GaudiControlsLogs")
  server.assignTask("LogLHCb2SumSrv", "GaudiControlsLogs")
  server.assignTask("LogLHCb2", "GaudiControlsLogs")
  server.assignTask("LogLHCbASumSrv", "GaudiControlsLogs")
  server.assignTask("LogLHCbA", "GaudiControlsLogs")
  server.addTask({'task': 'LogPVSSSrv_HLT',\
                  'utgid': 'LogPVSSSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s pvss -p /run/fmc/logPVSS.fifo", 'description': ''})

  server.addTask({'task': 'LogPVSS_HLT',\
                  'utgid': 'LogPVSS',\
                  'command': '<SCRIPTS>/PVSSlogger.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logPVSS.fifo -O /run/fmc/logPVSS.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<NODE>', 'description': ''})

  server.addSet({"task_set":"WinCCOALogs_HLT", "description": ""})    ### [('host', '<NODE>'), ('project', '<NODE>'), ('next_level_server', '<TOP_LOGGER>'), ('group', 'HLT')]
  server.assignTask("LogPVSSSrv_HLT", "WinCCOALogs_HLT")
  server.assignTask("LogPVSS_HLT", "WinCCOALogs_HLT")
  server.addTask({'task': 'HLTEQBridge',\
                  'utgid': 'HLTEQBridge',\
                  'command': '<SCRIPTS>/HLTEQBridge.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'HLTThrColl',\
                  'utgid': 'HLTThrColl',\
                  'command': '/group/online/dataflow/cmtuser/OnlineRelease/Online/FarmConfig/job/SFThrottleCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'BusyMonControl',\
                  'utgid': 'BusyMonControl',\
                  'command': '/group/online/dataflow/cmtuser/OnlineRelease/Online/FarmConfig/job/SFBusyMon.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p -15 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"HLTControls", "description": ""})    ### [('node', '<NODE>'), ('next_level_server', '<TOP_LOGGER>'), ('with_pvss', True)]
  server.assignTask("HLTEQBridge", "HLTControls")
  server.assignTask("HLTThrColl", "HLTControls")
  server.assignTask("BusyMonControl", "HLTControls")
  server.addTask({'task': 'TorrentPublish',\
                  'utgid': 'TorrentPublish',\
                  'command': '<SCRIPTS>/BitTorrentPublish.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"TorrentTasksControl", "description": ""})    ### [('node', '<NODE>'), ('controlsPC', '<TOP_LOGGER>')]
  server.assignTask("TorrentPublish", "TorrentTasksControl")
  server.addClass({"node_class": "HLTControlsPC", "description": ""})
  server.assignSet("DIMTasks", "HLTControlsPC")
  server.assignSet("Infrastructure", "HLTControlsPC")
  server.assignSet("HLTControls", "HLTControlsPC")
  server.assignSet("WinCCOALogs_HLT", "HLTControlsPC")
  server.assignSet("ControlsNodeTasks", "HLTControlsPC")
  server.assignSet("TorrentTasksControl", "HLTControlsPC")
  server.assignSet("TorrentTasksWorker", "HLTControlsPC")
  server.assignSet("GaudiControlsLogs", "HLTControlsPC")
  server.addNode({"regex": "hlt[a-m][0-9][0-9]", "description": ""})
  server.assignClass("HLTControlsPC","hlt[a-m][0-9][0-9]")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'LogGaudiSrv',\
                  'utgid': 'LogGaudiSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s gaudi -p /run/fmc/logGaudi.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb1Srv',\
                  'utgid': 'LogLHCb1Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb1 -p /run/fmc/logLHCb1.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb2Srv',\
                  'utgid': 'LogLHCb2Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb2 -p /run/fmc/logLHCb2.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCbASrv',\
                  'utgid': 'LogLHCbASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcba -p /run/fmc/logLHCbA.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'SSSRecover',\
                  'utgid': 'SSSRecover',\
                  'command': '/home/beat/ssscheck.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p -15 -n beat',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"Infrastructure", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('next_level_server', '<TOP_LOGGER>'), ('gaudilog', 1), ('group', 'Infrastructure')]
  server.assignTask("LogDefaultSrv", "Infrastructure")
  server.assignTask("LogGaudiSrv", "Infrastructure")
  server.assignTask("LogLHCb1Srv", "Infrastructure")
  server.assignTask("LogLHCb2Srv", "Infrastructure")
  server.assignTask("LogLHCbASrv", "Infrastructure")
  server.assignTask("TANServ", "Infrastructure")
  server.assignTask("pingSrv_u", "Infrastructure")
  server.assignTask("ROcollect", "Infrastructure")
  server.assignTask("SSSRecover", "Infrastructure")
  server.addTask({'task': 'LHCb_CALD0701_CalibHistos_0',\
                  'utgid': 'LHCb_CALD0701_CalibHistos_0',\
                  'command': '<SCRIPTS>/HistDimBridge.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'CalibHistBridge.opts', 'description': ''})

  server.addTask({'task': 'AnalysisBridge_0',\
                  'utgid': 'AnalysisBridge_0',\
                  'command': '<SCRIPTS>/AnalysisBridge.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'mona08.lbdaq.cern.ch cald07.lbdaq.cern.ch "LHCbStatus/*"', 'description': ''})

  server.addSet({"task_set":"CALD0701Tasks", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("LHCb_CALD0701_CalibHistos_0", "CALD0701Tasks")
  server.assignTask("AnalysisBridge_0", "CALD0701Tasks")
  server.addClass({"node_class": "CalibWorker", "description": ""})
  server.assignSet("Infrastructure", "CalibWorker")
  server.assignSet("CALD0701Tasks", "CalibWorker")
  server.addNode({"regex": "cal[a-f][0-9][0-9][0-9][0-9]", "description": ""})
  server.assignClass("CalibWorker","cal[a-f][0-9][0-9][0-9][0-9]")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'LogGaudiSrv',\
                  'utgid': 'LogGaudiSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s gaudi -p /run/fmc/logGaudi.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb1Srv',\
                  'utgid': 'LogLHCb1Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb1 -p /run/fmc/logLHCb1.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb2Srv',\
                  'utgid': 'LogLHCb2Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb2 -p /run/fmc/logLHCb2.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCbASrv',\
                  'utgid': 'LogLHCbASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcba -p /run/fmc/logLHCbA.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'SSSRecover',\
                  'utgid': 'SSSRecover',\
                  'command': '/home/beat/ssscheck.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p -15 -n beat',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"Infrastructure", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('next_level_server', '<TOP_LOGGER>'), ('gaudilog', 1), ('group', 'Infrastructure')]
  server.assignTask("LogDefaultSrv", "Infrastructure")
  server.assignTask("LogGaudiSrv", "Infrastructure")
  server.assignTask("LogLHCb1Srv", "Infrastructure")
  server.assignTask("LogLHCb2Srv", "Infrastructure")
  server.assignTask("LogLHCbASrv", "Infrastructure")
  server.assignTask("TANServ", "Infrastructure")
  server.assignTask("pingSrv_u", "Infrastructure")
  server.assignTask("ROcollect", "Infrastructure")
  server.assignTask("SSSRecover", "Infrastructure")
  server.addTask({'task': 'LogSumDefSrv',\
                  'utgid': 'LogSumDefSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 3 -S 100 -s fmc -p /run/fmc/logsum.fifo", 'description': ''})

  server.addTask({'task': 'LogDef',\
                  'utgid': 'LogDef',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 3 -s fmc -o /run/fmc/logsum.fifo", 'description': ''})

  server.addTask({'task': 'ROpublish',\
                  'utgid': 'ROpublish',\
                  'command': '<SCRIPTS>/MONPublish.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -statDelay=8000 -mbmDelay=1376", 'description': ''})

  server.addTask({'task': 'TaskSupervisor',\
                  'utgid': 'TaskSupervisor',\
                  'command': '<SCRIPTS>/TaskSupervisor.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'webDID',\
                  'utgid': 'webDID',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"DIMTasks", "description": ""})    ### [('host', '<NODE>')]
  server.assignTask("webDID", "DIMTasks")
  server.addTask({'task': 'PropertyServer',\
                  'utgid': 'PropertyServer',\
                  'command': '<SCRIPTS>/PropertyServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"ControlsNodeTasks", "description": ""})    ### [('node', '<NODE>'), ('next_level_server', '<TOP_LOGGER>'), ('gaudilog', None), ('slice', '')]
  server.assignTask("LogSumDefSrv", "ControlsNodeTasks")
  server.assignTask("LogDef", "ControlsNodeTasks")
  server.assignTask("ROpublish", "ControlsNodeTasks")
  server.assignTask("TaskSupervisor", "ControlsNodeTasks")
  server.assignTask("PropertyServer", "ControlsNodeTasks")
  server.addTask({'task': 'LogGaudiSumSrv',\
                  'utgid': 'LogGaudiSumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s gaudi -p /run/fmc/gaudisum.fifo", 'description': ''})

  server.addTask({'task': 'LogGaudi',\
                  'utgid': 'LogGaudi',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s gaudi -o /run/fmc/gaudisum.fifo", 'description': ''})

  server.addTask({'task': 'LogLHCb1SumSrv',\
                  'utgid': 'LogLHCb1SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s lhcb1 -p /run/fmc/logLHCb1sum.fifo", 'description': ''})

  server.addTask({'task': 'LogLHCb1',\
                  'utgid': 'LogLHCb1',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s lhcb1 -o /run/fmc/logLHCb1sum.fifo", 'description': ''})

  server.addTask({'task': 'LogLHCb2SumSrv',\
                  'utgid': 'LogLHCb2SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s lhcb2 -p /run/fmc/logLHCb2sum.fifo", 'description': ''})

  server.addTask({'task': 'LogLHCb2',\
                  'utgid': 'LogLHCb2',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s lhcb2 -o /run/fmc/logLHCb2sum.fifo", 'description': ''})

  server.addTask({'task': 'LogLHCbASumSrv',\
                  'utgid': 'LogLHCbASumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s lhcba -p /run/fmc/logLHCbAsum.fifo", 'description': ''})

  server.addTask({'task': 'LogLHCbA',\
                  'utgid': 'LogLHCbA',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s lhcba -o /run/fmc/logLHCbAsum.fifo", 'description': ''})

  server.addSet({"task_set":"GaudiControlsLogs", "description": ""})    ### [('host', '<NODE>'), ('next_level_server', '<TOP_LOGGER>'), ('gaudi', 1), ('LHCb1', 1), ('LHCb2', 1), ('LHCbA', 1), ('group', 'GaudiControlsLogs')]
  server.assignTask("LogGaudiSumSrv", "GaudiControlsLogs")
  server.assignTask("LogGaudi", "GaudiControlsLogs")
  server.assignTask("LogLHCb1SumSrv", "GaudiControlsLogs")
  server.assignTask("LogLHCb1", "GaudiControlsLogs")
  server.assignTask("LogLHCb2SumSrv", "GaudiControlsLogs")
  server.assignTask("LogLHCb2", "GaudiControlsLogs")
  server.assignTask("LogLHCbASumSrv", "GaudiControlsLogs")
  server.assignTask("LogLHCbA", "GaudiControlsLogs")
  server.addTask({'task': 'LogPVSSSrv_HLT',\
                  'utgid': 'LogPVSSSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s pvss -p /run/fmc/logPVSS.fifo", 'description': ''})

  server.addTask({'task': 'LogPVSS_HLT',\
                  'utgid': 'LogPVSS',\
                  'command': '<SCRIPTS>/PVSSlogger.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logPVSS.fifo -O /run/fmc/logPVSS.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<NODE>', 'description': ''})

  server.addSet({"task_set":"WinCCOALogs_HLT", "description": ""})    ### [('host', '<NODE>'), ('project', '<NODE>'), ('next_level_server', '<TOP_LOGGER>'), ('group', 'HLT')]
  server.assignTask("LogPVSSSrv_HLT", "WinCCOALogs_HLT")
  server.assignTask("LogPVSS_HLT", "WinCCOALogs_HLT")
  server.addTask({'task': 'HLTEQBridge',\
                  'utgid': 'HLTEQBridge',\
                  'command': '<SCRIPTS>/HLTEQBridge.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'HLTThrColl',\
                  'utgid': 'HLTThrColl',\
                  'command': '/group/online/dataflow/cmtuser/OnlineRelease/Online/FarmConfig/job/SFThrottleCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'BusyMonControl',\
                  'utgid': 'BusyMonControl',\
                  'command': '/group/online/dataflow/cmtuser/OnlineRelease/Online/FarmConfig/job/SFBusyMon.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p -15 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"HLTControls", "description": ""})    ### [('node', '<NODE>'), ('next_level_server', '<TOP_LOGGER>'), ('with_pvss', True)]
  server.assignTask("HLTEQBridge", "HLTControls")
  server.assignTask("HLTThrColl", "HLTControls")
  server.assignTask("BusyMonControl", "HLTControls")
  server.addTask({'task': 'AnalysisBridge_0',\
                  'utgid': 'AnalysisBridge_0',\
                  'command': '<SCRIPTS>/AnalysisBridge.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'mona08.lbdaq.cern.ch cald07.lbdaq.cern.ch "LHCbStatus/*"', 'description': ''})

  server.addSet({"task_set":"CALD07Tasks", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("AnalysisBridge_0", "CALD07Tasks")
  server.addClass({"node_class": "CalibControlsPC", "description": ""})
  server.assignSet("DIMTasks", "CalibControlsPC")
  server.assignSet("Infrastructure", "CalibControlsPC")
  server.assignSet("HLTControls", "CalibControlsPC")
  server.assignSet("WinCCOALogs_HLT", "CalibControlsPC")
  server.assignSet("CALD07Tasks", "CalibControlsPC")
  server.assignSet("ControlsNodeTasks", "CalibControlsPC")
  server.assignSet("GaudiControlsLogs", "CalibControlsPC")
  server.addNode({"regex": "cal[a-f][0-9][0-9]", "description": ""})
  server.assignClass("CalibControlsPC","cal[a-f][0-9][0-9]")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'LogGaudiSrv',\
                  'utgid': 'LogGaudiSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s gaudi -p /run/fmc/logGaudi.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb1Srv',\
                  'utgid': 'LogLHCb1Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb1 -p /run/fmc/logLHCb1.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb2Srv',\
                  'utgid': 'LogLHCb2Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb2 -p /run/fmc/logLHCb2.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCbASrv',\
                  'utgid': 'LogLHCbASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcba -p /run/fmc/logLHCbA.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'SSSRecover',\
                  'utgid': 'SSSRecover',\
                  'command': '/home/beat/ssscheck.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p -15 -n beat',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"Infrastructure", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('next_level_server', None), ('gaudilog', 2), ('group', 'Infrastructure')]
  server.assignTask("LogDefaultSrv", "Infrastructure")
  server.assignTask("LogGaudiSrv", "Infrastructure")
  server.assignTask("LogLHCb1Srv", "Infrastructure")
  server.assignTask("LogLHCb2Srv", "Infrastructure")
  server.assignTask("LogLHCbASrv", "Infrastructure")
  server.assignTask("TANServ", "Infrastructure")
  server.assignTask("pingSrv_u", "Infrastructure")
  server.assignTask("ROcollect", "Infrastructure")
  server.assignTask("SSSRecover", "Infrastructure")
  server.addTask({'task': 'Storage_Slice00Srv',\
                  'utgid': 'Storage_Slice00Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice00 -p /run/fmc/Storage_Slice00.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice01Srv',\
                  'utgid': 'Storage_Slice01Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice01 -p /run/fmc/Storage_Slice01.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice02Srv',\
                  'utgid': 'Storage_Slice02Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice02 -p /run/fmc/Storage_Slice02.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice03Srv',\
                  'utgid': 'Storage_Slice03Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice03 -p /run/fmc/Storage_Slice03.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice04Srv',\
                  'utgid': 'Storage_Slice04Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice04 -p /run/fmc/Storage_Slice04.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice05Srv',\
                  'utgid': 'Storage_Slice05Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice05 -p /run/fmc/Storage_Slice05.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice06Srv',\
                  'utgid': 'Storage_Slice06Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice06 -p /run/fmc/Storage_Slice06.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice07Srv',\
                  'utgid': 'Storage_Slice07Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice07 -p /run/fmc/Storage_Slice07.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice08Srv',\
                  'utgid': 'Storage_Slice08Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice08 -p /run/fmc/Storage_Slice08.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice09Srv',\
                  'utgid': 'Storage_Slice09Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice09 -p /run/fmc/Storage_Slice09.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice0ASrv',\
                  'utgid': 'Storage_Slice0ASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice0A -p /run/fmc/Storage_Slice0A.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice0BSrv',\
                  'utgid': 'Storage_Slice0BSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice0B -p /run/fmc/Storage_Slice0B.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice0CSrv',\
                  'utgid': 'Storage_Slice0CSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice0C -p /run/fmc/Storage_Slice0C.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice0DSrv',\
                  'utgid': 'Storage_Slice0DSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice0D -p /run/fmc/Storage_Slice0D.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice0ESrv',\
                  'utgid': 'Storage_Slice0ESrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice0E -p /run/fmc/Storage_Slice0E.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice0FSrv',\
                  'utgid': 'Storage_Slice0FSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice0F -p /run/fmc/Storage_Slice0F.fifo', 'description': ''})

  server.addSet({"task_set":"SliceLogs_STOR", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('slice', 'Storage'), ('num', 16)]
  server.assignTask("Storage_Slice00Srv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice01Srv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice02Srv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice03Srv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice04Srv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice05Srv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice06Srv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice07Srv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice08Srv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice09Srv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice0ASrv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice0BSrv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice0CSrv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice0DSrv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice0ESrv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice0FSrv", "SliceLogs_STOR")
  server.addTask({'task': 'TorrentLoader',\
                  'utgid': 'TorrentLoader',\
                  'command': '<SCRIPTS>/BitTorrentLoader.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"TorrentTasksWorker", "description": ""})    ### [('node', '<NODE>'), ('controlsPC', '<DIM_DNS>')]
  server.assignTask("TorrentLoader", "TorrentTasksWorker")
  server.addTask({'task': 'TorrentTracker',\
                  'utgid': 'TorrentTracker',\
                  'command': '<SCRIPTS>/BitTorrentTracker.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"STORERECVTasks", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("TorrentTracker", "STORERECVTasks")
  server.addClass({"node_class": "StorageWorker", "description": ""})
  server.assignSet("Infrastructure", "StorageWorker")
  server.assignSet("SliceLogs_STOR", "StorageWorker")
  server.assignSet("STORERECVTasks", "StorageWorker")
  server.assignSet("TorrentTasksWorker", "StorageWorker")
  server.addNode({"regex": "storerecv0[0-9]", "description": ""})
  server.assignClass("StorageWorker","storerecv0[0-9]")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'LogGaudiSrv',\
                  'utgid': 'LogGaudiSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s gaudi -p /run/fmc/logGaudi.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb1Srv',\
                  'utgid': 'LogLHCb1Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb1 -p /run/fmc/logLHCb1.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb2Srv',\
                  'utgid': 'LogLHCb2Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb2 -p /run/fmc/logLHCb2.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCbASrv',\
                  'utgid': 'LogLHCbASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcba -p /run/fmc/logLHCbA.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'SSSRecover',\
                  'utgid': 'SSSRecover',\
                  'command': '/home/beat/ssscheck.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p -15 -n beat',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"Infrastructure", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('next_level_server', None), ('gaudilog', 2), ('group', 'Infrastructure')]
  server.assignTask("LogDefaultSrv", "Infrastructure")
  server.assignTask("LogGaudiSrv", "Infrastructure")
  server.assignTask("LogLHCb1Srv", "Infrastructure")
  server.assignTask("LogLHCb2Srv", "Infrastructure")
  server.assignTask("LogLHCbASrv", "Infrastructure")
  server.assignTask("TANServ", "Infrastructure")
  server.assignTask("pingSrv_u", "Infrastructure")
  server.assignTask("ROcollect", "Infrastructure")
  server.assignTask("SSSRecover", "Infrastructure")
  server.addTask({'task': 'Storage_Slice00Srv',\
                  'utgid': 'Storage_Slice00Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice00 -p /run/fmc/Storage_Slice00.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice01Srv',\
                  'utgid': 'Storage_Slice01Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice01 -p /run/fmc/Storage_Slice01.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice02Srv',\
                  'utgid': 'Storage_Slice02Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice02 -p /run/fmc/Storage_Slice02.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice03Srv',\
                  'utgid': 'Storage_Slice03Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice03 -p /run/fmc/Storage_Slice03.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice04Srv',\
                  'utgid': 'Storage_Slice04Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice04 -p /run/fmc/Storage_Slice04.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice05Srv',\
                  'utgid': 'Storage_Slice05Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice05 -p /run/fmc/Storage_Slice05.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice06Srv',\
                  'utgid': 'Storage_Slice06Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice06 -p /run/fmc/Storage_Slice06.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice07Srv',\
                  'utgid': 'Storage_Slice07Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice07 -p /run/fmc/Storage_Slice07.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice08Srv',\
                  'utgid': 'Storage_Slice08Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice08 -p /run/fmc/Storage_Slice08.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice09Srv',\
                  'utgid': 'Storage_Slice09Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice09 -p /run/fmc/Storage_Slice09.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice0ASrv',\
                  'utgid': 'Storage_Slice0ASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice0A -p /run/fmc/Storage_Slice0A.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice0BSrv',\
                  'utgid': 'Storage_Slice0BSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice0B -p /run/fmc/Storage_Slice0B.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice0CSrv',\
                  'utgid': 'Storage_Slice0CSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice0C -p /run/fmc/Storage_Slice0C.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice0DSrv',\
                  'utgid': 'Storage_Slice0DSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice0D -p /run/fmc/Storage_Slice0D.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice0ESrv',\
                  'utgid': 'Storage_Slice0ESrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice0E -p /run/fmc/Storage_Slice0E.fifo', 'description': ''})

  server.addTask({'task': 'Storage_Slice0FSrv',\
                  'utgid': 'Storage_Slice0FSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Storage_Slice0F -p /run/fmc/Storage_Slice0F.fifo', 'description': ''})

  server.addSet({"task_set":"SliceLogs_STOR", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('slice', 'Storage'), ('num', 16)]
  server.assignTask("Storage_Slice00Srv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice01Srv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice02Srv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice03Srv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice04Srv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice05Srv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice06Srv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice07Srv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice08Srv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice09Srv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice0ASrv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice0BSrv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice0CSrv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice0DSrv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice0ESrv", "SliceLogs_STOR")
  server.assignTask("Storage_Slice0FSrv", "SliceLogs_STOR")
  server.addTask({'task': 'TorrentLoader',\
                  'utgid': 'TorrentLoader',\
                  'command': '<SCRIPTS>/BitTorrentLoader.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"TorrentTasksWorker", "description": ""})    ### [('node', '<NODE>'), ('controlsPC', '<DIM_DNS>')]
  server.assignTask("TorrentLoader", "TorrentTasksWorker")
  server.addTask({'task': 'LogSumDefSrv',\
                  'utgid': 'LogSumDefSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 3 -S 100 -s fmc -p /run/fmc/logsum.fifo", 'description': ''})

  server.addTask({'task': 'LogDef',\
                  'utgid': 'LogDef',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 3 -s fmc -o /run/fmc/logsum.fifo", 'description': ''})

  server.addTask({'task': 'ROpublish',\
                  'utgid': 'ROpublish',\
                  'command': '<SCRIPTS>/MONPublish.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -statDelay=8000 -mbmDelay=1376", 'description': ''})

  server.addTask({'task': 'TaskSupervisor',\
                  'utgid': 'TaskSupervisor',\
                  'command': '<SCRIPTS>/TaskSupervisor.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'webDID',\
                  'utgid': 'webDID',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"DIMTasks", "description": ""})    ### [('host', '<NODE>')]
  server.assignTask("webDID", "DIMTasks")
  server.addTask({'task': 'PropertyServer',\
                  'utgid': 'PropertyServer',\
                  'command': '<SCRIPTS>/PropertyServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"ControlsNodeTasks", "description": ""})    ### [('node', '<NODE>'), ('next_level_server', '<TOP_LOGGER>'), ('gaudilog', None), ('slice', '')]
  server.assignTask("LogSumDefSrv", "ControlsNodeTasks")
  server.assignTask("LogDef", "ControlsNodeTasks")
  server.assignTask("ROpublish", "ControlsNodeTasks")
  server.assignTask("TaskSupervisor", "ControlsNodeTasks")
  server.assignTask("PropertyServer", "ControlsNodeTasks")
  server.addTask({'task': 'Storage_Slice00SumSrv',\
                  'utgid': 'Storage_Slice00SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Storage_Slice00 -p /run/fmc/Storage_Slice00sum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice00Collect',\
                  'utgid': 'Storage_Slice00Collect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Storage_Slice00 -o /run/fmc/Storage_Slice00sum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice01SumSrv',\
                  'utgid': 'Storage_Slice01SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Storage_Slice01 -p /run/fmc/Storage_Slice01sum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice01Collect',\
                  'utgid': 'Storage_Slice01Collect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Storage_Slice01 -o /run/fmc/Storage_Slice01sum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice02SumSrv',\
                  'utgid': 'Storage_Slice02SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Storage_Slice02 -p /run/fmc/Storage_Slice02sum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice02Collect',\
                  'utgid': 'Storage_Slice02Collect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Storage_Slice02 -o /run/fmc/Storage_Slice02sum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice03SumSrv',\
                  'utgid': 'Storage_Slice03SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Storage_Slice03 -p /run/fmc/Storage_Slice03sum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice03Collect',\
                  'utgid': 'Storage_Slice03Collect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Storage_Slice03 -o /run/fmc/Storage_Slice03sum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice04SumSrv',\
                  'utgid': 'Storage_Slice04SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Storage_Slice04 -p /run/fmc/Storage_Slice04sum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice04Collect',\
                  'utgid': 'Storage_Slice04Collect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Storage_Slice04 -o /run/fmc/Storage_Slice04sum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice05SumSrv',\
                  'utgid': 'Storage_Slice05SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Storage_Slice05 -p /run/fmc/Storage_Slice05sum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice05Collect',\
                  'utgid': 'Storage_Slice05Collect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Storage_Slice05 -o /run/fmc/Storage_Slice05sum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice06SumSrv',\
                  'utgid': 'Storage_Slice06SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Storage_Slice06 -p /run/fmc/Storage_Slice06sum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice06Collect',\
                  'utgid': 'Storage_Slice06Collect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Storage_Slice06 -o /run/fmc/Storage_Slice06sum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice07SumSrv',\
                  'utgid': 'Storage_Slice07SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Storage_Slice07 -p /run/fmc/Storage_Slice07sum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice07Collect',\
                  'utgid': 'Storage_Slice07Collect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Storage_Slice07 -o /run/fmc/Storage_Slice07sum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice08SumSrv',\
                  'utgid': 'Storage_Slice08SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Storage_Slice08 -p /run/fmc/Storage_Slice08sum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice08Collect',\
                  'utgid': 'Storage_Slice08Collect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Storage_Slice08 -o /run/fmc/Storage_Slice08sum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice09SumSrv',\
                  'utgid': 'Storage_Slice09SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Storage_Slice09 -p /run/fmc/Storage_Slice09sum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice09Collect',\
                  'utgid': 'Storage_Slice09Collect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Storage_Slice09 -o /run/fmc/Storage_Slice09sum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice0ASumSrv',\
                  'utgid': 'Storage_Slice0ASumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Storage_Slice0A -p /run/fmc/Storage_Slice0Asum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice0ACollect',\
                  'utgid': 'Storage_Slice0ACollect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Storage_Slice0A -o /run/fmc/Storage_Slice0Asum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice0BSumSrv',\
                  'utgid': 'Storage_Slice0BSumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Storage_Slice0B -p /run/fmc/Storage_Slice0Bsum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice0BCollect',\
                  'utgid': 'Storage_Slice0BCollect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Storage_Slice0B -o /run/fmc/Storage_Slice0Bsum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice0CSumSrv',\
                  'utgid': 'Storage_Slice0CSumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Storage_Slice0C -p /run/fmc/Storage_Slice0Csum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice0CCollect',\
                  'utgid': 'Storage_Slice0CCollect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Storage_Slice0C -o /run/fmc/Storage_Slice0Csum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice0DSumSrv',\
                  'utgid': 'Storage_Slice0DSumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Storage_Slice0D -p /run/fmc/Storage_Slice0Dsum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice0DCollect',\
                  'utgid': 'Storage_Slice0DCollect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Storage_Slice0D -o /run/fmc/Storage_Slice0Dsum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice0ESumSrv',\
                  'utgid': 'Storage_Slice0ESumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Storage_Slice0E -p /run/fmc/Storage_Slice0Esum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice0ECollect',\
                  'utgid': 'Storage_Slice0ECollect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Storage_Slice0E -o /run/fmc/Storage_Slice0Esum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice0FSumSrv',\
                  'utgid': 'Storage_Slice0FSumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Storage_Slice0F -p /run/fmc/Storage_Slice0Fsum.fifo", 'description': ''})

  server.addTask({'task': 'Storage_Slice0FCollect',\
                  'utgid': 'Storage_Slice0FCollect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Storage_Slice0F -o /run/fmc/Storage_Slice0Fsum.fifo", 'description': ''})

  server.addSet({"task_set":"SliceCollectors_STOR", "description": ""})    ### [('host', '<NODE>'), ('next_level_server', '<TOP_LOGGER>'), ('slice', 'Storage'), ('num', 16)]
  server.assignTask("Storage_Slice00SumSrv", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice00Collect", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice01SumSrv", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice01Collect", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice02SumSrv", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice02Collect", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice03SumSrv", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice03Collect", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice04SumSrv", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice04Collect", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice05SumSrv", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice05Collect", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice06SumSrv", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice06Collect", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice07SumSrv", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice07Collect", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice08SumSrv", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice08Collect", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice09SumSrv", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice09Collect", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice0ASumSrv", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice0ACollect", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice0BSumSrv", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice0BCollect", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice0CSumSrv", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice0CCollect", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice0DSumSrv", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice0DCollect", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice0ESumSrv", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice0ECollect", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice0FSumSrv", "SliceCollectors_STOR")
  server.assignTask("Storage_Slice0FCollect", "SliceCollectors_STOR")
  server.addTask({'task': 'LogPVSSSrv_STOR',\
                  'utgid': 'LogPVSSSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s pvss -p /run/fmc/logPVSS.fifo", 'description': ''})

  server.addTask({'task': 'LogPVSS_STOR',\
                  'utgid': 'LogPVSS',\
                  'command': '<SCRIPTS>/PVSSlogger.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logPVSS.fifo -O /run/fmc/logPVSS.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'STORAGE', 'description': ''})

  server.addSet({"task_set":"WinCCOALogs_STOR", "description": ""})    ### [('host', '<NODE>'), ('project', 'STORAGE'), ('next_level_server', '<TOP_LOGGER>'), ('group', 'STOR')]
  server.assignTask("LogPVSSSrv_STOR", "WinCCOALogs_STOR")
  server.assignTask("LogPVSS_STOR", "WinCCOALogs_STOR")
  server.addTask({'task': 'LogPVSSConfigSrv_STOR',\
                  'utgid': 'LogPVSSConfigSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s pvssconfig -p /run/fmc/logPVSSConfig.fifo", 'description': ''})

  server.addSet({"task_set":"WinCCOAConfigLogs_STOR", "description": ""})    ### [('host', '<NODE>'), ('project', 'STORAGE'), ('next_level_server', '<TOP_LOGGER>'), ('group', 'STOR')]
  server.assignTask("LogPVSSConfigSrv_STOR", "WinCCOAConfigLogs_STOR")
  server.addTask({'task': 'TorrentPublish',\
                  'utgid': 'TorrentPublish',\
                  'command': '<SCRIPTS>/BitTorrentPublish.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'PartitionLogStamps',\
                  'utgid': 'PartitionLogStamps',\
                  'command': '<SCRIPTS>/PartitionLogStamps.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"STORECTL01Tasks", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("TorrentPublish", "STORECTL01Tasks")
  server.assignTask("PartitionLogStamps", "STORECTL01Tasks")
  server.addClass({"node_class": "StorageControlsPC", "description": ""})
  server.assignSet("DIMTasks", "StorageControlsPC")
  server.assignSet("Infrastructure", "StorageControlsPC")
  server.assignSet("ControlsNodeTasks", "StorageControlsPC")
  server.assignSet("WinCCOAConfigLogs_STOR", "StorageControlsPC")
  server.assignSet("SliceLogs_STOR", "StorageControlsPC")
  server.assignSet("WinCCOALogs_STOR", "StorageControlsPC")
  server.assignSet("TorrentTasksWorker", "StorageControlsPC")
  server.assignSet("STORECTL01Tasks", "StorageControlsPC")
  server.assignSet("SliceCollectors_STOR", "StorageControlsPC")
  server.addNode({"regex": "storectl01", "description": ""})
  server.assignClass("StorageControlsPC","storectl01")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'LogGaudiSrv',\
                  'utgid': 'LogGaudiSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s gaudi -p /run/fmc/logGaudi.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb1Srv',\
                  'utgid': 'LogLHCb1Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb1 -p /run/fmc/logLHCb1.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb2Srv',\
                  'utgid': 'LogLHCb2Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb2 -p /run/fmc/logLHCb2.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCbASrv',\
                  'utgid': 'LogLHCbASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcba -p /run/fmc/logLHCbA.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'SSSRecover',\
                  'utgid': 'SSSRecover',\
                  'command': '/home/beat/ssscheck.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p -15 -n beat',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"Infrastructure", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('next_level_server', None), ('gaudilog', 2), ('group', 'Infrastructure')]
  server.assignTask("LogDefaultSrv", "Infrastructure")
  server.assignTask("LogGaudiSrv", "Infrastructure")
  server.assignTask("LogLHCb1Srv", "Infrastructure")
  server.assignTask("LogLHCb2Srv", "Infrastructure")
  server.assignTask("LogLHCbASrv", "Infrastructure")
  server.assignTask("TANServ", "Infrastructure")
  server.assignTask("pingSrv_u", "Infrastructure")
  server.assignTask("ROcollect", "Infrastructure")
  server.assignTask("SSSRecover", "Infrastructure")
  server.addTask({'task': 'Monitoring_Slice00Srv',\
                  'utgid': 'Monitoring_Slice00Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice00 -p /run/fmc/Monitoring_Slice00.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice01Srv',\
                  'utgid': 'Monitoring_Slice01Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice01 -p /run/fmc/Monitoring_Slice01.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice02Srv',\
                  'utgid': 'Monitoring_Slice02Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice02 -p /run/fmc/Monitoring_Slice02.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice03Srv',\
                  'utgid': 'Monitoring_Slice03Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice03 -p /run/fmc/Monitoring_Slice03.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice04Srv',\
                  'utgid': 'Monitoring_Slice04Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice04 -p /run/fmc/Monitoring_Slice04.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice05Srv',\
                  'utgid': 'Monitoring_Slice05Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice05 -p /run/fmc/Monitoring_Slice05.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice06Srv',\
                  'utgid': 'Monitoring_Slice06Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice06 -p /run/fmc/Monitoring_Slice06.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice07Srv',\
                  'utgid': 'Monitoring_Slice07Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice07 -p /run/fmc/Monitoring_Slice07.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice08Srv',\
                  'utgid': 'Monitoring_Slice08Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice08 -p /run/fmc/Monitoring_Slice08.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice09Srv',\
                  'utgid': 'Monitoring_Slice09Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice09 -p /run/fmc/Monitoring_Slice09.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0ASrv',\
                  'utgid': 'Monitoring_Slice0ASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice0A -p /run/fmc/Monitoring_Slice0A.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0BSrv',\
                  'utgid': 'Monitoring_Slice0BSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice0B -p /run/fmc/Monitoring_Slice0B.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0CSrv',\
                  'utgid': 'Monitoring_Slice0CSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice0C -p /run/fmc/Monitoring_Slice0C.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0DSrv',\
                  'utgid': 'Monitoring_Slice0DSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice0D -p /run/fmc/Monitoring_Slice0D.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0ESrv',\
                  'utgid': 'Monitoring_Slice0ESrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice0E -p /run/fmc/Monitoring_Slice0E.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0FSrv',\
                  'utgid': 'Monitoring_Slice0FSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice0F -p /run/fmc/Monitoring_Slice0F.fifo', 'description': ''})

  server.addSet({"task_set":"SliceLogs_MONI", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('slice', 'Monitoring'), ('num', 16)]
  server.assignTask("Monitoring_Slice00Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice01Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice02Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice03Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice04Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice05Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice06Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice07Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice08Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice09Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice0ASrv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice0BSrv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice0CSrv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice0DSrv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice0ESrv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice0FSrv", "SliceLogs_MONI")
  server.addClass({"node_class": "MonitoringWorker", "description": ""})
  server.assignSet("Infrastructure", "MonitoringWorker")
  server.assignSet("SliceLogs_MONI", "MonitoringWorker")
  server.addNode({"regex": "mona080[13456789]", "description": ""})
  server.assignClass("MonitoringWorker","mona080[13456789]")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'LogGaudiSrv',\
                  'utgid': 'LogGaudiSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s gaudi -p /run/fmc/logGaudi.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb1Srv',\
                  'utgid': 'LogLHCb1Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb1 -p /run/fmc/logLHCb1.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb2Srv',\
                  'utgid': 'LogLHCb2Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb2 -p /run/fmc/logLHCb2.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCbASrv',\
                  'utgid': 'LogLHCbASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcba -p /run/fmc/logLHCbA.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'SSSRecover',\
                  'utgid': 'SSSRecover',\
                  'command': '/home/beat/ssscheck.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p -15 -n beat',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"Infrastructure", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('next_level_server', None), ('gaudilog', 2), ('group', 'Infrastructure')]
  server.assignTask("LogDefaultSrv", "Infrastructure")
  server.assignTask("LogGaudiSrv", "Infrastructure")
  server.assignTask("LogLHCb1Srv", "Infrastructure")
  server.assignTask("LogLHCb2Srv", "Infrastructure")
  server.assignTask("LogLHCbASrv", "Infrastructure")
  server.assignTask("TANServ", "Infrastructure")
  server.assignTask("pingSrv_u", "Infrastructure")
  server.assignTask("ROcollect", "Infrastructure")
  server.assignTask("SSSRecover", "Infrastructure")
  server.addTask({'task': 'Monitoring_Slice00Srv',\
                  'utgid': 'Monitoring_Slice00Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice00 -p /run/fmc/Monitoring_Slice00.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice01Srv',\
                  'utgid': 'Monitoring_Slice01Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice01 -p /run/fmc/Monitoring_Slice01.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice02Srv',\
                  'utgid': 'Monitoring_Slice02Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice02 -p /run/fmc/Monitoring_Slice02.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice03Srv',\
                  'utgid': 'Monitoring_Slice03Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice03 -p /run/fmc/Monitoring_Slice03.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice04Srv',\
                  'utgid': 'Monitoring_Slice04Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice04 -p /run/fmc/Monitoring_Slice04.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice05Srv',\
                  'utgid': 'Monitoring_Slice05Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice05 -p /run/fmc/Monitoring_Slice05.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice06Srv',\
                  'utgid': 'Monitoring_Slice06Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice06 -p /run/fmc/Monitoring_Slice06.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice07Srv',\
                  'utgid': 'Monitoring_Slice07Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice07 -p /run/fmc/Monitoring_Slice07.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice08Srv',\
                  'utgid': 'Monitoring_Slice08Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice08 -p /run/fmc/Monitoring_Slice08.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice09Srv',\
                  'utgid': 'Monitoring_Slice09Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice09 -p /run/fmc/Monitoring_Slice09.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0ASrv',\
                  'utgid': 'Monitoring_Slice0ASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice0A -p /run/fmc/Monitoring_Slice0A.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0BSrv',\
                  'utgid': 'Monitoring_Slice0BSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice0B -p /run/fmc/Monitoring_Slice0B.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0CSrv',\
                  'utgid': 'Monitoring_Slice0CSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice0C -p /run/fmc/Monitoring_Slice0C.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0DSrv',\
                  'utgid': 'Monitoring_Slice0DSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice0D -p /run/fmc/Monitoring_Slice0D.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0ESrv',\
                  'utgid': 'Monitoring_Slice0ESrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice0E -p /run/fmc/Monitoring_Slice0E.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0FSrv',\
                  'utgid': 'Monitoring_Slice0FSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice0F -p /run/fmc/Monitoring_Slice0F.fifo', 'description': ''})

  server.addSet({"task_set":"SliceLogs_MONI", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('slice', 'Monitoring'), ('num', 16)]
  server.assignTask("Monitoring_Slice00Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice01Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice02Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice03Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice04Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice05Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice06Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice07Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice08Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice09Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice0ASrv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice0BSrv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice0CSrv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice0DSrv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice0ESrv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice0FSrv", "SliceLogs_MONI")
  server.addTask({'task': 'LHCb_MONA0802_WebGLSrv_0',\
                  'utgid': 'LHCb_MONA0802_WebGLSrv_0',\
                  'command': '-D',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logGaudi.fifo -O /run/fmc/logGaudi.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'LOGFIFO=/run/fmc/logGaudi.fifo -D PARTITION=LHCb /group/online/dataflow/cmtuser/WebGLDisplay/WebGLDisplaySys/scripts/runWebGLDisplay.sh WebGLDisplay Class2 WebGLDisplaySrv mona08', 'description': ''})

  server.addTask({'task': 'LHCb_WebGLScan_0',\
                  'utgid': 'LHCb_WebGLScan_0',\
                  'command': '<SCRIPTS>/WebGLEventScanner.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logGaudi.fifo -O /run/fmc/logGaudi.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"MONA0802Tasks", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("LHCb_MONA0802_WebGLSrv_0", "MONA0802Tasks")
  server.assignTask("LHCb_WebGLScan_0", "MONA0802Tasks")
  server.addClass({"node_class": "MonitoringWorker02", "description": ""})
  server.assignSet("Infrastructure", "MonitoringWorker02")
  server.assignSet("MONA0802Tasks", "MonitoringWorker02")
  server.assignSet("SliceLogs_MONI", "MonitoringWorker02")
  server.addNode({"regex": "mona0802", "description": ""})
  server.assignClass("MonitoringWorker02","mona0802")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'LogGaudiSrv',\
                  'utgid': 'LogGaudiSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s gaudi -p /run/fmc/logGaudi.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb1Srv',\
                  'utgid': 'LogLHCb1Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb1 -p /run/fmc/logLHCb1.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb2Srv',\
                  'utgid': 'LogLHCb2Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb2 -p /run/fmc/logLHCb2.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCbASrv',\
                  'utgid': 'LogLHCbASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcba -p /run/fmc/logLHCbA.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'SSSRecover',\
                  'utgid': 'SSSRecover',\
                  'command': '/home/beat/ssscheck.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p -15 -n beat',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"Infrastructure", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('next_level_server', None), ('gaudilog', 2), ('group', 'Infrastructure')]
  server.assignTask("LogDefaultSrv", "Infrastructure")
  server.assignTask("LogGaudiSrv", "Infrastructure")
  server.assignTask("LogLHCb1Srv", "Infrastructure")
  server.assignTask("LogLHCb2Srv", "Infrastructure")
  server.assignTask("LogLHCbASrv", "Infrastructure")
  server.assignTask("TANServ", "Infrastructure")
  server.assignTask("pingSrv_u", "Infrastructure")
  server.assignTask("ROcollect", "Infrastructure")
  server.assignTask("SSSRecover", "Infrastructure")
  server.addTask({'task': 'Monitoring_Slice00Srv',\
                  'utgid': 'Monitoring_Slice00Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice00 -p /run/fmc/Monitoring_Slice00.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice01Srv',\
                  'utgid': 'Monitoring_Slice01Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice01 -p /run/fmc/Monitoring_Slice01.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice02Srv',\
                  'utgid': 'Monitoring_Slice02Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice02 -p /run/fmc/Monitoring_Slice02.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice03Srv',\
                  'utgid': 'Monitoring_Slice03Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice03 -p /run/fmc/Monitoring_Slice03.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice04Srv',\
                  'utgid': 'Monitoring_Slice04Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice04 -p /run/fmc/Monitoring_Slice04.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice05Srv',\
                  'utgid': 'Monitoring_Slice05Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice05 -p /run/fmc/Monitoring_Slice05.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice06Srv',\
                  'utgid': 'Monitoring_Slice06Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice06 -p /run/fmc/Monitoring_Slice06.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice07Srv',\
                  'utgid': 'Monitoring_Slice07Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice07 -p /run/fmc/Monitoring_Slice07.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice08Srv',\
                  'utgid': 'Monitoring_Slice08Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice08 -p /run/fmc/Monitoring_Slice08.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice09Srv',\
                  'utgid': 'Monitoring_Slice09Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice09 -p /run/fmc/Monitoring_Slice09.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0ASrv',\
                  'utgid': 'Monitoring_Slice0ASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice0A -p /run/fmc/Monitoring_Slice0A.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0BSrv',\
                  'utgid': 'Monitoring_Slice0BSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice0B -p /run/fmc/Monitoring_Slice0B.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0CSrv',\
                  'utgid': 'Monitoring_Slice0CSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice0C -p /run/fmc/Monitoring_Slice0C.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0DSrv',\
                  'utgid': 'Monitoring_Slice0DSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice0D -p /run/fmc/Monitoring_Slice0D.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0ESrv',\
                  'utgid': 'Monitoring_Slice0ESrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice0E -p /run/fmc/Monitoring_Slice0E.fifo', 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0FSrv',\
                  'utgid': 'Monitoring_Slice0FSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Monitoring_Slice0F -p /run/fmc/Monitoring_Slice0F.fifo', 'description': ''})

  server.addSet({"task_set":"SliceLogs_MONI", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('slice', 'Monitoring'), ('num', 16)]
  server.assignTask("Monitoring_Slice00Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice01Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice02Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice03Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice04Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice05Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice06Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice07Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice08Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice09Srv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice0ASrv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice0BSrv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice0CSrv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice0DSrv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice0ESrv", "SliceLogs_MONI")
  server.assignTask("Monitoring_Slice0FSrv", "SliceLogs_MONI")
  server.addTask({'task': 'LogSumDefSrv',\
                  'utgid': 'LogSumDefSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 3 -S 100 -s fmc -p /run/fmc/logsum.fifo", 'description': ''})

  server.addTask({'task': 'LogDef',\
                  'utgid': 'LogDef',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 3 -s fmc -o /run/fmc/logsum.fifo", 'description': ''})

  server.addTask({'task': 'ROpublish',\
                  'utgid': 'ROpublish',\
                  'command': '<SCRIPTS>/MONPublish.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -statDelay=8000 -mbmDelay=1376", 'description': ''})

  server.addTask({'task': 'TaskSupervisor',\
                  'utgid': 'TaskSupervisor',\
                  'command': '<SCRIPTS>/TaskSupervisor.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'webDID',\
                  'utgid': 'webDID',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"DIMTasks", "description": ""})    ### [('host', '<NODE>')]
  server.assignTask("webDID", "DIMTasks")
  server.addTask({'task': 'PropertyServer',\
                  'utgid': 'PropertyServer',\
                  'command': '<SCRIPTS>/PropertyServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"ControlsNodeTasks", "description": ""})    ### [('node', '<NODE>'), ('next_level_server', '<TOP_LOGGER>'), ('gaudilog', None), ('slice', '')]
  server.assignTask("LogSumDefSrv", "ControlsNodeTasks")
  server.assignTask("LogDef", "ControlsNodeTasks")
  server.assignTask("ROpublish", "ControlsNodeTasks")
  server.assignTask("TaskSupervisor", "ControlsNodeTasks")
  server.assignTask("PropertyServer", "ControlsNodeTasks")
  server.addTask({'task': 'Monitoring_Slice00SumSrv',\
                  'utgid': 'Monitoring_Slice00SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Monitoring_Slice00 -p /run/fmc/Monitoring_Slice00sum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice00Collect',\
                  'utgid': 'Monitoring_Slice00Collect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Monitoring_Slice00 -o /run/fmc/Monitoring_Slice00sum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice01SumSrv',\
                  'utgid': 'Monitoring_Slice01SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Monitoring_Slice01 -p /run/fmc/Monitoring_Slice01sum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice01Collect',\
                  'utgid': 'Monitoring_Slice01Collect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Monitoring_Slice01 -o /run/fmc/Monitoring_Slice01sum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice02SumSrv',\
                  'utgid': 'Monitoring_Slice02SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Monitoring_Slice02 -p /run/fmc/Monitoring_Slice02sum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice02Collect',\
                  'utgid': 'Monitoring_Slice02Collect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Monitoring_Slice02 -o /run/fmc/Monitoring_Slice02sum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice03SumSrv',\
                  'utgid': 'Monitoring_Slice03SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Monitoring_Slice03 -p /run/fmc/Monitoring_Slice03sum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice03Collect',\
                  'utgid': 'Monitoring_Slice03Collect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Monitoring_Slice03 -o /run/fmc/Monitoring_Slice03sum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice04SumSrv',\
                  'utgid': 'Monitoring_Slice04SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Monitoring_Slice04 -p /run/fmc/Monitoring_Slice04sum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice04Collect',\
                  'utgid': 'Monitoring_Slice04Collect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Monitoring_Slice04 -o /run/fmc/Monitoring_Slice04sum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice05SumSrv',\
                  'utgid': 'Monitoring_Slice05SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Monitoring_Slice05 -p /run/fmc/Monitoring_Slice05sum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice05Collect',\
                  'utgid': 'Monitoring_Slice05Collect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Monitoring_Slice05 -o /run/fmc/Monitoring_Slice05sum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice06SumSrv',\
                  'utgid': 'Monitoring_Slice06SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Monitoring_Slice06 -p /run/fmc/Monitoring_Slice06sum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice06Collect',\
                  'utgid': 'Monitoring_Slice06Collect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Monitoring_Slice06 -o /run/fmc/Monitoring_Slice06sum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice07SumSrv',\
                  'utgid': 'Monitoring_Slice07SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Monitoring_Slice07 -p /run/fmc/Monitoring_Slice07sum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice07Collect',\
                  'utgid': 'Monitoring_Slice07Collect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Monitoring_Slice07 -o /run/fmc/Monitoring_Slice07sum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice08SumSrv',\
                  'utgid': 'Monitoring_Slice08SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Monitoring_Slice08 -p /run/fmc/Monitoring_Slice08sum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice08Collect',\
                  'utgid': 'Monitoring_Slice08Collect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Monitoring_Slice08 -o /run/fmc/Monitoring_Slice08sum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice09SumSrv',\
                  'utgid': 'Monitoring_Slice09SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Monitoring_Slice09 -p /run/fmc/Monitoring_Slice09sum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice09Collect',\
                  'utgid': 'Monitoring_Slice09Collect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Monitoring_Slice09 -o /run/fmc/Monitoring_Slice09sum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0ASumSrv',\
                  'utgid': 'Monitoring_Slice0ASumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Monitoring_Slice0A -p /run/fmc/Monitoring_Slice0Asum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0ACollect',\
                  'utgid': 'Monitoring_Slice0ACollect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Monitoring_Slice0A -o /run/fmc/Monitoring_Slice0Asum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0BSumSrv',\
                  'utgid': 'Monitoring_Slice0BSumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Monitoring_Slice0B -p /run/fmc/Monitoring_Slice0Bsum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0BCollect',\
                  'utgid': 'Monitoring_Slice0BCollect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Monitoring_Slice0B -o /run/fmc/Monitoring_Slice0Bsum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0CSumSrv',\
                  'utgid': 'Monitoring_Slice0CSumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Monitoring_Slice0C -p /run/fmc/Monitoring_Slice0Csum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0CCollect',\
                  'utgid': 'Monitoring_Slice0CCollect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Monitoring_Slice0C -o /run/fmc/Monitoring_Slice0Csum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0DSumSrv',\
                  'utgid': 'Monitoring_Slice0DSumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Monitoring_Slice0D -p /run/fmc/Monitoring_Slice0Dsum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0DCollect',\
                  'utgid': 'Monitoring_Slice0DCollect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Monitoring_Slice0D -o /run/fmc/Monitoring_Slice0Dsum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0ESumSrv',\
                  'utgid': 'Monitoring_Slice0ESumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Monitoring_Slice0E -p /run/fmc/Monitoring_Slice0Esum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0ECollect',\
                  'utgid': 'Monitoring_Slice0ECollect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Monitoring_Slice0E -o /run/fmc/Monitoring_Slice0Esum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0FSumSrv',\
                  'utgid': 'Monitoring_Slice0FSumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Monitoring_Slice0F -p /run/fmc/Monitoring_Slice0Fsum.fifo", 'description': ''})

  server.addTask({'task': 'Monitoring_Slice0FCollect',\
                  'utgid': 'Monitoring_Slice0FCollect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Monitoring_Slice0F -o /run/fmc/Monitoring_Slice0Fsum.fifo", 'description': ''})

  server.addSet({"task_set":"SliceCollectors_MONI", "description": ""})    ### [('host', '<NODE>'), ('next_level_server', '<TOP_LOGGER>'), ('slice', 'Monitoring'), ('num', 16)]
  server.assignTask("Monitoring_Slice00SumSrv", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice00Collect", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice01SumSrv", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice01Collect", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice02SumSrv", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice02Collect", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice03SumSrv", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice03Collect", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice04SumSrv", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice04Collect", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice05SumSrv", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice05Collect", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice06SumSrv", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice06Collect", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice07SumSrv", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice07Collect", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice08SumSrv", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice08Collect", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice09SumSrv", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice09Collect", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice0ASumSrv", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice0ACollect", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice0BSumSrv", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice0BCollect", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice0CSumSrv", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice0CCollect", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice0DSumSrv", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice0DCollect", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice0ESumSrv", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice0ECollect", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice0FSumSrv", "SliceCollectors_MONI")
  server.assignTask("Monitoring_Slice0FCollect", "SliceCollectors_MONI")
  server.addTask({'task': 'LogPVSSSrv_MONI',\
                  'utgid': 'LogPVSSSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s pvss -p /run/fmc/logPVSS.fifo", 'description': ''})

  server.addTask({'task': 'LogPVSS_MONI',\
                  'utgid': 'LogPVSS',\
                  'command': '<SCRIPTS>/PVSSlogger.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logPVSS.fifo -O /run/fmc/logPVSS.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'MONITORING', 'description': ''})

  server.addSet({"task_set":"WinCCOALogs_MONI", "description": ""})    ### [('host', '<NODE>'), ('project', 'MONITORING'), ('next_level_server', '<TOP_LOGGER>'), ('group', 'MONI')]
  server.assignTask("LogPVSSSrv_MONI", "WinCCOALogs_MONI")
  server.assignTask("LogPVSS_MONI", "WinCCOALogs_MONI")
  server.addTask({'task': 'LogPVSSConfigSrv_MONI',\
                  'utgid': 'LogPVSSConfigSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s pvssconfig -p /run/fmc/logPVSSConfig.fifo", 'description': ''})

  server.addSet({"task_set":"WinCCOAConfigLogs_MONI", "description": ""})    ### [('host', '<NODE>'), ('project', 'MONITORING'), ('next_level_server', '<TOP_LOGGER>'), ('group', 'MONI')]
  server.assignTask("LogPVSSConfigSrv_MONI", "WinCCOAConfigLogs_MONI")
  server.addClass({"node_class": "MonitoringControlsPC", "description": ""})
  server.assignSet("DIMTasks", "MonitoringControlsPC")
  server.assignSet("Infrastructure", "MonitoringControlsPC")
  server.assignSet("SliceLogs_MONI", "MonitoringControlsPC")
  server.assignSet("ControlsNodeTasks", "MonitoringControlsPC")
  server.assignSet("SliceCollectors_MONI", "MonitoringControlsPC")
  server.assignSet("WinCCOALogs_MONI", "MonitoringControlsPC")
  server.assignSet("WinCCOAConfigLogs_MONI", "MonitoringControlsPC")
  server.addNode({"regex": "mona08", "description": ""})
  server.assignClass("MonitoringControlsPC","mona08")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'LogGaudiSrv',\
                  'utgid': 'LogGaudiSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s gaudi -p /run/fmc/logGaudi.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb1Srv',\
                  'utgid': 'LogLHCb1Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb1 -p /run/fmc/logLHCb1.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb2Srv',\
                  'utgid': 'LogLHCb2Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb2 -p /run/fmc/logLHCb2.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCbASrv',\
                  'utgid': 'LogLHCbASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcba -p /run/fmc/logLHCbA.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'SSSRecover',\
                  'utgid': 'SSSRecover',\
                  'command': '/home/beat/ssscheck.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p -15 -n beat',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"Infrastructure", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('next_level_server', '<TOP_LOGGER>'), ('gaudilog', 1), ('group', 'Infrastructure')]
  server.assignTask("LogDefaultSrv", "Infrastructure")
  server.assignTask("LogGaudiSrv", "Infrastructure")
  server.assignTask("LogLHCb1Srv", "Infrastructure")
  server.assignTask("LogLHCb2Srv", "Infrastructure")
  server.assignTask("LogLHCbASrv", "Infrastructure")
  server.assignTask("TANServ", "Infrastructure")
  server.assignTask("pingSrv_u", "Infrastructure")
  server.assignTask("ROcollect", "Infrastructure")
  server.assignTask("SSSRecover", "Infrastructure")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'LogGaudiSrv',\
                  'utgid': 'LogGaudiSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s gaudi -p /run/fmc/logGaudi.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb1Srv',\
                  'utgid': 'LogLHCb1Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb1 -p /run/fmc/logLHCb1.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb2Srv',\
                  'utgid': 'LogLHCb2Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb2 -p /run/fmc/logLHCb2.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCbASrv',\
                  'utgid': 'LogLHCbASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcba -p /run/fmc/logLHCbA.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'SSSRecover',\
                  'utgid': 'SSSRecover',\
                  'command': '/home/beat/ssscheck.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p -15 -n beat',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"Infrastructure", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('next_level_server', None), ('gaudilog', 2), ('group', 'Infrastructure')]
  server.assignTask("LogDefaultSrv", "Infrastructure")
  server.assignTask("LogGaudiSrv", "Infrastructure")
  server.assignTask("LogLHCb1Srv", "Infrastructure")
  server.assignTask("LogLHCb2Srv", "Infrastructure")
  server.assignTask("LogLHCbASrv", "Infrastructure")
  server.assignTask("TANServ", "Infrastructure")
  server.assignTask("pingSrv_u", "Infrastructure")
  server.assignTask("ROcollect", "Infrastructure")
  server.assignTask("SSSRecover", "Infrastructure")
  server.addTask({'task': 'Reconstruction_Slice00Srv',\
                  'utgid': 'Reconstruction_Slice00Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Reconstruction_Slice00 -p /run/fmc/Reconstruction_Slice00.fifo', 'description': ''})

  server.addTask({'task': 'Reconstruction_Slice01Srv',\
                  'utgid': 'Reconstruction_Slice01Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Reconstruction_Slice01 -p /run/fmc/Reconstruction_Slice01.fifo', 'description': ''})

  server.addTask({'task': 'Reconstruction_Slice02Srv',\
                  'utgid': 'Reconstruction_Slice02Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Reconstruction_Slice02 -p /run/fmc/Reconstruction_Slice02.fifo', 'description': ''})

  server.addTask({'task': 'Reconstruction_Slice03Srv',\
                  'utgid': 'Reconstruction_Slice03Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Reconstruction_Slice03 -p /run/fmc/Reconstruction_Slice03.fifo', 'description': ''})

  server.addSet({"task_set":"SliceLogs_RECO", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('slice', 'Reconstruction'), ('num', 4)]
  server.assignTask("Reconstruction_Slice00Srv", "SliceLogs_RECO")
  server.assignTask("Reconstruction_Slice01Srv", "SliceLogs_RECO")
  server.assignTask("Reconstruction_Slice02Srv", "SliceLogs_RECO")
  server.assignTask("Reconstruction_Slice03Srv", "SliceLogs_RECO")
  server.addClass({"node_class": "ReconstructionWorker", "description": ""})
  server.assignSet("Infrastructure", "ReconstructionWorker")
  server.assignSet("SliceLogs_RECO", "ReconstructionWorker")
  server.addNode({"regex": "mona09[0-9][0-9]", "description": ""})
  server.assignClass("ReconstructionWorker","mona09[0-9][0-9]")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'LogGaudiSrv',\
                  'utgid': 'LogGaudiSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s gaudi -p /run/fmc/logGaudi.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb1Srv',\
                  'utgid': 'LogLHCb1Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb1 -p /run/fmc/logLHCb1.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb2Srv',\
                  'utgid': 'LogLHCb2Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb2 -p /run/fmc/logLHCb2.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCbASrv',\
                  'utgid': 'LogLHCbASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcba -p /run/fmc/logLHCbA.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'SSSRecover',\
                  'utgid': 'SSSRecover',\
                  'command': '/home/beat/ssscheck.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p -15 -n beat',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"Infrastructure", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('next_level_server', '<TOP_LOGGER>'), ('gaudilog', 1), ('group', 'Infrastructure')]
  server.assignTask("LogDefaultSrv", "Infrastructure")
  server.assignTask("LogGaudiSrv", "Infrastructure")
  server.assignTask("LogLHCb1Srv", "Infrastructure")
  server.assignTask("LogLHCb2Srv", "Infrastructure")
  server.assignTask("LogLHCbASrv", "Infrastructure")
  server.assignTask("TANServ", "Infrastructure")
  server.assignTask("pingSrv_u", "Infrastructure")
  server.assignTask("ROcollect", "Infrastructure")
  server.assignTask("SSSRecover", "Infrastructure")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'LogGaudiSrv',\
                  'utgid': 'LogGaudiSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s gaudi -p /run/fmc/logGaudi.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb1Srv',\
                  'utgid': 'LogLHCb1Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb1 -p /run/fmc/logLHCb1.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb2Srv',\
                  'utgid': 'LogLHCb2Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb2 -p /run/fmc/logLHCb2.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCbASrv',\
                  'utgid': 'LogLHCbASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcba -p /run/fmc/logLHCbA.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'SSSRecover',\
                  'utgid': 'SSSRecover',\
                  'command': '/home/beat/ssscheck.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p -15 -n beat',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"Infrastructure", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('next_level_server', None), ('gaudilog', 2), ('group', 'Infrastructure')]
  server.assignTask("LogDefaultSrv", "Infrastructure")
  server.assignTask("LogGaudiSrv", "Infrastructure")
  server.assignTask("LogLHCb1Srv", "Infrastructure")
  server.assignTask("LogLHCb2Srv", "Infrastructure")
  server.assignTask("LogLHCbASrv", "Infrastructure")
  server.assignTask("TANServ", "Infrastructure")
  server.assignTask("pingSrv_u", "Infrastructure")
  server.assignTask("ROcollect", "Infrastructure")
  server.assignTask("SSSRecover", "Infrastructure")
  server.addTask({'task': 'Reconstruction_Slice00Srv',\
                  'utgid': 'Reconstruction_Slice00Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Reconstruction_Slice00 -p /run/fmc/Reconstruction_Slice00.fifo', 'description': ''})

  server.addTask({'task': 'Reconstruction_Slice01Srv',\
                  'utgid': 'Reconstruction_Slice01Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Reconstruction_Slice01 -p /run/fmc/Reconstruction_Slice01.fifo', 'description': ''})

  server.addTask({'task': 'Reconstruction_Slice02Srv',\
                  'utgid': 'Reconstruction_Slice02Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Reconstruction_Slice02 -p /run/fmc/Reconstruction_Slice02.fifo', 'description': ''})

  server.addTask({'task': 'Reconstruction_Slice03Srv',\
                  'utgid': 'Reconstruction_Slice03Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s Reconstruction_Slice03 -p /run/fmc/Reconstruction_Slice03.fifo', 'description': ''})

  server.addSet({"task_set":"SliceLogs_RECO", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('slice', 'Reconstruction'), ('num', 4)]
  server.assignTask("Reconstruction_Slice00Srv", "SliceLogs_RECO")
  server.assignTask("Reconstruction_Slice01Srv", "SliceLogs_RECO")
  server.assignTask("Reconstruction_Slice02Srv", "SliceLogs_RECO")
  server.assignTask("Reconstruction_Slice03Srv", "SliceLogs_RECO")
  server.addTask({'task': 'LogSumDefSrv',\
                  'utgid': 'LogSumDefSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 3 -S 100 -s fmc -p /run/fmc/logsum.fifo", 'description': ''})

  server.addTask({'task': 'LogDef',\
                  'utgid': 'LogDef',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 3 -s fmc -o /run/fmc/logsum.fifo", 'description': ''})

  server.addTask({'task': 'ROpublish',\
                  'utgid': 'ROpublish',\
                  'command': '<SCRIPTS>/MONPublish.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -statDelay=8000 -mbmDelay=1376", 'description': ''})

  server.addTask({'task': 'TaskSupervisor',\
                  'utgid': 'TaskSupervisor',\
                  'command': '<SCRIPTS>/TaskSupervisor.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'webDID',\
                  'utgid': 'webDID',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"DIMTasks", "description": ""})    ### [('host', '<NODE>')]
  server.assignTask("webDID", "DIMTasks")
  server.addTask({'task': 'PropertyServer',\
                  'utgid': 'PropertyServer',\
                  'command': '<SCRIPTS>/PropertyServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"ControlsNodeTasks", "description": ""})    ### [('node', '<NODE>'), ('next_level_server', '<TOP_LOGGER>'), ('gaudilog', None), ('slice', '')]
  server.assignTask("LogSumDefSrv", "ControlsNodeTasks")
  server.assignTask("LogDef", "ControlsNodeTasks")
  server.assignTask("ROpublish", "ControlsNodeTasks")
  server.assignTask("TaskSupervisor", "ControlsNodeTasks")
  server.assignTask("PropertyServer", "ControlsNodeTasks")
  server.addTask({'task': 'Reconstruction_Slice00SumSrv',\
                  'utgid': 'Reconstruction_Slice00SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Reconstruction_Slice00 -p /run/fmc/Reconstruction_Slice00sum.fifo", 'description': ''})

  server.addTask({'task': 'Reconstruction_Slice00Collect',\
                  'utgid': 'Reconstruction_Slice00Collect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Reconstruction_Slice00 -o /run/fmc/Reconstruction_Slice00sum.fifo", 'description': ''})

  server.addTask({'task': 'Reconstruction_Slice01SumSrv',\
                  'utgid': 'Reconstruction_Slice01SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Reconstruction_Slice01 -p /run/fmc/Reconstruction_Slice01sum.fifo", 'description': ''})

  server.addTask({'task': 'Reconstruction_Slice01Collect',\
                  'utgid': 'Reconstruction_Slice01Collect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Reconstruction_Slice01 -o /run/fmc/Reconstruction_Slice01sum.fifo", 'description': ''})

  server.addTask({'task': 'Reconstruction_Slice02SumSrv',\
                  'utgid': 'Reconstruction_Slice02SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Reconstruction_Slice02 -p /run/fmc/Reconstruction_Slice02sum.fifo", 'description': ''})

  server.addTask({'task': 'Reconstruction_Slice02Collect',\
                  'utgid': 'Reconstruction_Slice02Collect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Reconstruction_Slice02 -o /run/fmc/Reconstruction_Slice02sum.fifo", 'description': ''})

  server.addTask({'task': 'Reconstruction_Slice03SumSrv',\
                  'utgid': 'Reconstruction_Slice03SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -S 100 -l 1 -s Reconstruction_Slice03 -p /run/fmc/Reconstruction_Slice03sum.fifo", 'description': ''})

  server.addTask({'task': 'Reconstruction_Slice03Collect',\
                  'utgid': 'Reconstruction_Slice03Collect',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s Reconstruction_Slice03 -o /run/fmc/Reconstruction_Slice03sum.fifo", 'description': ''})

  server.addSet({"task_set":"SliceCollectors_RECO", "description": ""})    ### [('host', '<NODE>'), ('next_level_server', '<TOP_LOGGER>'), ('slice', 'Reconstruction'), ('num', 4)]
  server.assignTask("Reconstruction_Slice00SumSrv", "SliceCollectors_RECO")
  server.assignTask("Reconstruction_Slice00Collect", "SliceCollectors_RECO")
  server.assignTask("Reconstruction_Slice01SumSrv", "SliceCollectors_RECO")
  server.assignTask("Reconstruction_Slice01Collect", "SliceCollectors_RECO")
  server.assignTask("Reconstruction_Slice02SumSrv", "SliceCollectors_RECO")
  server.assignTask("Reconstruction_Slice02Collect", "SliceCollectors_RECO")
  server.assignTask("Reconstruction_Slice03SumSrv", "SliceCollectors_RECO")
  server.assignTask("Reconstruction_Slice03Collect", "SliceCollectors_RECO")
  server.addTask({'task': 'LogPVSSSrv_RECO',\
                  'utgid': 'LogPVSSSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s pvss -p /run/fmc/logPVSS.fifo", 'description': ''})

  server.addTask({'task': 'LogPVSS_RECO',\
                  'utgid': 'LogPVSS',\
                  'command': '<SCRIPTS>/PVSSlogger.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logPVSS.fifo -O /run/fmc/logPVSS.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'RECONSTRUCTION', 'description': ''})

  server.addSet({"task_set":"WinCCOALogs_RECO", "description": ""})    ### [('host', '<NODE>'), ('project', 'RECONSTRUCTION'), ('next_level_server', '<TOP_LOGGER>'), ('group', 'RECO')]
  server.assignTask("LogPVSSSrv_RECO", "WinCCOALogs_RECO")
  server.assignTask("LogPVSS_RECO", "WinCCOALogs_RECO")
  server.addTask({'task': 'LogPVSSConfigSrv_RECO',\
                  'utgid': 'LogPVSSConfigSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s pvssconfig -p /run/fmc/logPVSSConfig.fifo", 'description': ''})

  server.addSet({"task_set":"WinCCOAConfigLogs_RECO", "description": ""})    ### [('host', '<NODE>'), ('project', 'RECONSTRUCTION'), ('next_level_server', '<TOP_LOGGER>'), ('group', 'RECO')]
  server.assignTask("LogPVSSConfigSrv_RECO", "WinCCOAConfigLogs_RECO")
  server.addTask({'task': 'RecHistos_0',\
                  'utgid': 'RecHistos_0',\
                  'command': '<SCRIPTS>/HistDimBridge.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 15 -X 30 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'RecHistBridge.opts', 'description': ''})

  server.addTask({'task': 'RecSaver_0',\
                  'utgid': 'RecSaver_0',\
                  'command': '<SCRIPTS>/HistDimSaver.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 15 -X 30 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'RecHistSaver.opts', 'description': ''})

  server.addSet({"task_set":"MONA09Tasks", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("RecHistos_0", "MONA09Tasks")
  server.assignTask("RecSaver_0", "MONA09Tasks")
  server.addClass({"node_class": "ReconstructionControlsPC", "description": ""})
  server.assignSet("DIMTasks", "ReconstructionControlsPC")
  server.assignSet("Infrastructure", "ReconstructionControlsPC")
  server.assignSet("WinCCOAConfigLogs_RECO", "ReconstructionControlsPC")
  server.assignSet("SliceCollectors_RECO", "ReconstructionControlsPC")
  server.assignSet("MONA09Tasks", "ReconstructionControlsPC")
  server.assignSet("SliceLogs_RECO", "ReconstructionControlsPC")
  server.assignSet("ControlsNodeTasks", "ReconstructionControlsPC")
  server.assignSet("WinCCOALogs_RECO", "ReconstructionControlsPC")
  server.addNode({"regex": "mona09", "description": ""})
  server.assignClass("ReconstructionControlsPC","mona09")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'LogGaudiSrv',\
                  'utgid': 'LogGaudiSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s gaudi -p /run/fmc/logGaudi.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb1Srv',\
                  'utgid': 'LogLHCb1Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb1 -p /run/fmc/logLHCb1.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb2Srv',\
                  'utgid': 'LogLHCb2Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb2 -p /run/fmc/logLHCb2.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCbASrv',\
                  'utgid': 'LogLHCbASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcba -p /run/fmc/logLHCbA.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'SSSRecover',\
                  'utgid': 'SSSRecover',\
                  'command': '/home/beat/ssscheck.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p -15 -n beat',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"Infrastructure", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('next_level_server', '<TOP_LOGGER>'), ('gaudilog', 1), ('group', 'Infrastructure')]
  server.assignTask("LogDefaultSrv", "Infrastructure")
  server.assignTask("LogGaudiSrv", "Infrastructure")
  server.assignTask("LogLHCb1Srv", "Infrastructure")
  server.assignTask("LogLHCb2Srv", "Infrastructure")
  server.assignTask("LogLHCbASrv", "Infrastructure")
  server.assignTask("TANServ", "Infrastructure")
  server.assignTask("pingSrv_u", "Infrastructure")
  server.assignTask("ROcollect", "Infrastructure")
  server.assignTask("SSSRecover", "Infrastructure")
  server.addTask({'task': 'TorrentLoader',\
                  'utgid': 'TorrentLoader',\
                  'command': '<SCRIPTS>/BitTorrentLoader.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"TorrentTasksWorker", "description": ""})    ### [('node', '<NODE>'), ('controlsPC', '<DIM_DNS>')]
  server.assignTask("TorrentLoader", "TorrentTasksWorker")
  server.addClass({"node_class": "DQWorker", "description": ""})
  server.assignSet("TorrentTasksWorker", "DQWorker")
  server.assignSet("Infrastructure", "DQWorker")
  server.addNode({"regex": "mona100[2-9]", "description": ""})
  server.assignClass("DQWorker","mona100[2-9]")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'LogGaudiSrv',\
                  'utgid': 'LogGaudiSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s gaudi -p /run/fmc/logGaudi.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb1Srv',\
                  'utgid': 'LogLHCb1Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb1 -p /run/fmc/logLHCb1.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb2Srv',\
                  'utgid': 'LogLHCb2Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb2 -p /run/fmc/logLHCb2.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCbASrv',\
                  'utgid': 'LogLHCbASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcba -p /run/fmc/logLHCbA.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'SSSRecover',\
                  'utgid': 'SSSRecover',\
                  'command': '/home/beat/ssscheck.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p -15 -n beat',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"Infrastructure", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('next_level_server', '<TOP_LOGGER>'), ('gaudilog', 1), ('group', 'Infrastructure')]
  server.assignTask("LogDefaultSrv", "Infrastructure")
  server.assignTask("LogGaudiSrv", "Infrastructure")
  server.assignTask("LogLHCb1Srv", "Infrastructure")
  server.assignTask("LogLHCb2Srv", "Infrastructure")
  server.assignTask("LogLHCbASrv", "Infrastructure")
  server.assignTask("TANServ", "Infrastructure")
  server.assignTask("pingSrv_u", "Infrastructure")
  server.assignTask("ROcollect", "Infrastructure")
  server.assignTask("SSSRecover", "Infrastructure")
  server.addTask({'task': 'TorrentLoader',\
                  'utgid': 'TorrentLoader',\
                  'command': '<SCRIPTS>/BitTorrentLoader.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"TorrentTasksWorker", "description": ""})    ### [('node', '<NODE>'), ('controlsPC', '<DIM_DNS>')]
  server.assignTask("TorrentLoader", "TorrentTasksWorker")
  server.addTask({'task': 'DQScanner',\
                  'utgid': 'DQScanner',\
                  'command': '<SCRIPTS>/DQScanner.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 20 -X 20 -m <NODE> -E /run/fmc/logGaudi.fifo -O /run/fmc/logGaudi.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'scan', 'description': ''})

  server.addTask({'task': 'DQResults',\
                  'utgid': 'DQResults',\
                  'command': '<SCRIPTS>/DQResults.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 20 -X 20 -m <NODE> -E /run/fmc/logGaudi.fifo -O /run/fmc/logGaudi.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'DQPublish',\
                  'utgid': 'DQPublish',\
                  'command': '<SCRIPTS>/DQPublish.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 20 -X 20 -m <NODE> -E /run/fmc/logGaudi.fifo -O /run/fmc/logGaudi.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'DQDump',\
                  'utgid': 'DQDump',\
                  'command': '<SCRIPTS>/DQDump.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 20 -X 20 -m <NODE> -E /run/fmc/logGaudi.fifo -O /run/fmc/logGaudi.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'LogDataQuality',\
                  'utgid': 'LogDataQuality',\
                  'command': '<SCRIPTS>/LogDataQuality.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 20 -X 20 -m <NODE> -E /run/fmc/logGaudi.fifo -O /run/fmc/logGaudi.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'mona10 -S 100 -l 1', 'description': ''})

  server.addSet({"task_set":"DQTasks", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("DQScanner", "DQTasks")
  server.assignTask("DQResults", "DQTasks")
  server.assignTask("DQPublish", "DQTasks")
  server.assignTask("DQDump", "DQTasks")
  server.assignTask("LogDataQuality", "DQTasks")
  server.addClass({"node_class": "DQScanner", "description": ""})
  server.assignSet("TorrentTasksWorker", "DQScanner")
  server.assignSet("Infrastructure", "DQScanner")
  server.assignSet("DQTasks", "DQScanner")
  server.addNode({"regex": "mona1001", "description": ""})
  server.assignClass("DQScanner","mona1001")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'LogGaudiSrv',\
                  'utgid': 'LogGaudiSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s gaudi -p /run/fmc/logGaudi.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb1Srv',\
                  'utgid': 'LogLHCb1Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb1 -p /run/fmc/logLHCb1.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb2Srv',\
                  'utgid': 'LogLHCb2Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb2 -p /run/fmc/logLHCb2.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCbASrv',\
                  'utgid': 'LogLHCbASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcba -p /run/fmc/logLHCbA.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'SSSRecover',\
                  'utgid': 'SSSRecover',\
                  'command': '/home/beat/ssscheck.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p -15 -n beat',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"Infrastructure", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('next_level_server', '<TOP_LOGGER>'), ('gaudilog', 1), ('group', 'Infrastructure')]
  server.assignTask("LogDefaultSrv", "Infrastructure")
  server.assignTask("LogGaudiSrv", "Infrastructure")
  server.assignTask("LogLHCb1Srv", "Infrastructure")
  server.assignTask("LogLHCb2Srv", "Infrastructure")
  server.assignTask("LogLHCbASrv", "Infrastructure")
  server.assignTask("TANServ", "Infrastructure")
  server.assignTask("pingSrv_u", "Infrastructure")
  server.assignTask("ROcollect", "Infrastructure")
  server.assignTask("SSSRecover", "Infrastructure")
  server.addTask({'task': 'LogSumDefSrv',\
                  'utgid': 'LogSumDefSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 3 -S 100 -s fmc -p /run/fmc/logsum.fifo", 'description': ''})

  server.addTask({'task': 'LogDef',\
                  'utgid': 'LogDef',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 3 -s fmc -o /run/fmc/logsum.fifo", 'description': ''})

  server.addTask({'task': 'ROpublish',\
                  'utgid': 'ROpublish',\
                  'command': '<SCRIPTS>/MONPublish.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -statDelay=8000 -mbmDelay=1376", 'description': ''})

  server.addTask({'task': 'TaskSupervisor',\
                  'utgid': 'TaskSupervisor',\
                  'command': '<SCRIPTS>/TaskSupervisor.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'webDID',\
                  'utgid': 'webDID',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"DIMTasks", "description": ""})    ### [('host', '<NODE>')]
  server.assignTask("webDID", "DIMTasks")
  server.addTask({'task': 'PropertyServer',\
                  'utgid': 'PropertyServer',\
                  'command': '<SCRIPTS>/PropertyServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"ControlsNodeTasks", "description": ""})    ### [('node', '<NODE>'), ('next_level_server', '<TOP_LOGGER>'), ('gaudilog', None), ('slice', '')]
  server.assignTask("LogSumDefSrv", "ControlsNodeTasks")
  server.assignTask("LogDef", "ControlsNodeTasks")
  server.assignTask("ROpublish", "ControlsNodeTasks")
  server.assignTask("TaskSupervisor", "ControlsNodeTasks")
  server.assignTask("PropertyServer", "ControlsNodeTasks")
  server.addTask({'task': 'LogGaudiSumSrv',\
                  'utgid': 'LogGaudiSumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s gaudi -p /run/fmc/gaudisum.fifo", 'description': ''})

  server.addTask({'task': 'LogGaudi',\
                  'utgid': 'LogGaudi',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s gaudi -o /run/fmc/gaudisum.fifo", 'description': ''})

  server.addTask({'task': 'LogLHCb1SumSrv',\
                  'utgid': 'LogLHCb1SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s lhcb1 -p /run/fmc/logLHCb1sum.fifo", 'description': ''})

  server.addTask({'task': 'LogLHCb1',\
                  'utgid': 'LogLHCb1',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s lhcb1 -o /run/fmc/logLHCb1sum.fifo", 'description': ''})

  server.addTask({'task': 'LogLHCb2SumSrv',\
                  'utgid': 'LogLHCb2SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s lhcb2 -p /run/fmc/logLHCb2sum.fifo", 'description': ''})

  server.addTask({'task': 'LogLHCb2',\
                  'utgid': 'LogLHCb2',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s lhcb2 -o /run/fmc/logLHCb2sum.fifo", 'description': ''})

  server.addTask({'task': 'LogLHCbASumSrv',\
                  'utgid': 'LogLHCbASumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s lhcba -p /run/fmc/logLHCbAsum.fifo", 'description': ''})

  server.addTask({'task': 'LogLHCbA',\
                  'utgid': 'LogLHCbA',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s lhcba -o /run/fmc/logLHCbAsum.fifo", 'description': ''})

  server.addSet({"task_set":"GaudiControlsLogs", "description": ""})    ### [('host', '<NODE>'), ('next_level_server', '<TOP_LOGGER>'), ('gaudi', 1), ('LHCb1', 1), ('LHCb2', 1), ('LHCbA', 1), ('group', 'GaudiControlsLogs')]
  server.assignTask("LogGaudiSumSrv", "GaudiControlsLogs")
  server.assignTask("LogGaudi", "GaudiControlsLogs")
  server.assignTask("LogLHCb1SumSrv", "GaudiControlsLogs")
  server.assignTask("LogLHCb1", "GaudiControlsLogs")
  server.assignTask("LogLHCb2SumSrv", "GaudiControlsLogs")
  server.assignTask("LogLHCb2", "GaudiControlsLogs")
  server.assignTask("LogLHCbASumSrv", "GaudiControlsLogs")
  server.assignTask("LogLHCbA", "GaudiControlsLogs")
  server.addTask({'task': 'LogPVSSSrv_HLT',\
                  'utgid': 'LogPVSSSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s pvss -p /run/fmc/logPVSS.fifo", 'description': ''})

  server.addTask({'task': 'LogPVSS_HLT',\
                  'utgid': 'LogPVSS',\
                  'command': '<SCRIPTS>/PVSSlogger.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logPVSS.fifo -O /run/fmc/logPVSS.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<NODE>', 'description': ''})

  server.addSet({"task_set":"WinCCOALogs_HLT", "description": ""})    ### [('host', '<NODE>'), ('project', '<NODE>'), ('next_level_server', '<TOP_LOGGER>'), ('group', 'HLT')]
  server.assignTask("LogPVSSSrv_HLT", "WinCCOALogs_HLT")
  server.assignTask("LogPVSS_HLT", "WinCCOALogs_HLT")
  server.addTask({'task': 'HLTEQBridge',\
                  'utgid': 'HLTEQBridge',\
                  'command': '<SCRIPTS>/HLTEQBridge.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'HLTThrColl',\
                  'utgid': 'HLTThrColl',\
                  'command': '/group/online/dataflow/cmtuser/OnlineRelease/Online/FarmConfig/job/SFThrottleCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'BusyMonControl',\
                  'utgid': 'BusyMonControl',\
                  'command': '/group/online/dataflow/cmtuser/OnlineRelease/Online/FarmConfig/job/SFBusyMon.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p -15 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"HLTControls", "description": ""})    ### [('node', '<NODE>'), ('next_level_server', '<TOP_LOGGER>'), ('with_pvss', True)]
  server.assignTask("HLTEQBridge", "HLTControls")
  server.assignTask("HLTThrColl", "HLTControls")
  server.assignTask("BusyMonControl", "HLTControls")
  server.addTask({'task': 'TorrentPublish',\
                  'utgid': 'TorrentPublish',\
                  'command': '<SCRIPTS>/BitTorrentPublish.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"MONA10Tasks", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("TorrentPublish", "MONA10Tasks")
  server.addClass({"node_class": "DQControlsPC", "description": ""})
  server.assignSet("DIMTasks", "DQControlsPC")
  server.assignSet("Infrastructure", "DQControlsPC")
  server.assignSet("HLTControls", "DQControlsPC")
  server.assignSet("WinCCOALogs_HLT", "DQControlsPC")
  server.assignSet("ControlsNodeTasks", "DQControlsPC")
  server.assignSet("MONA10Tasks", "DQControlsPC")
  server.assignSet("GaudiControlsLogs", "DQControlsPC")
  server.addNode({"regex": "mona10", "description": ""})
  server.assignClass("DQControlsPC","mona10")
  server.addTask({'task': 'LogDefaultSrv_HLTTOP',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'LogGaudiSrv_HLTTOP',\
                  'utgid': 'LogGaudiSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s gaudi -p /run/fmc/logGaudi.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb1Srv_HLTTOP',\
                  'utgid': 'LogLHCb1Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb1 -p /run/fmc/logLHCb1.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb2Srv_HLTTOP',\
                  'utgid': 'LogLHCb2Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb2 -p /run/fmc/logLHCb2.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCbASrv_HLTTOP',\
                  'utgid': 'LogLHCbASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcba -p /run/fmc/logLHCbA.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'SSSRecover',\
                  'utgid': 'SSSRecover',\
                  'command': '/home/beat/ssscheck.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p -15 -n beat',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"InfraStructure_HLTTOP_HLTTOP", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('next_level_server', '<TOP_LOGGER>'), ('gaudilog', 1), ('group', 'InfraStructure_HLTTOP')]
  server.assignTask("LogDefaultSrv_HLTTOP", "InfraStructure_HLTTOP_HLTTOP")
  server.assignTask("LogGaudiSrv_HLTTOP", "InfraStructure_HLTTOP_HLTTOP")
  server.assignTask("LogLHCb1Srv_HLTTOP", "InfraStructure_HLTTOP_HLTTOP")
  server.assignTask("LogLHCb2Srv_HLTTOP", "InfraStructure_HLTTOP_HLTTOP")
  server.assignTask("LogLHCbASrv_HLTTOP", "InfraStructure_HLTTOP_HLTTOP")
  server.assignTask("TANServ", "InfraStructure_HLTTOP_HLTTOP")
  server.assignTask("pingSrv_u", "InfraStructure_HLTTOP_HLTTOP")
  server.assignTask("ROcollect", "InfraStructure_HLTTOP_HLTTOP")
  server.assignTask("SSSRecover", "InfraStructure_HLTTOP_HLTTOP")
  server.addTask({'task': 'LogDef',\
                  'utgid': 'LogDef',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 3 -s fmc -o /run/fmc/logsum.fifo", 'description': ''})

  server.addTask({'task': 'TaskSupervisor',\
                  'utgid': 'TaskSupervisor',\
                  'command': '<SCRIPTS>/TaskSupervisor.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROpublish_HLTTOP',\
                  'utgid': 'ROpublish',\
                  'command': '<SCRIPTS>/MONPublish.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -statDelay=8000 -mbmDelay=1376", 'description': ''})

  server.addTask({'task': 'webDID',\
                  'utgid': 'webDID',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"DIMTasks", "description": ""})    ### [('host', '<NODE>')]
  server.assignTask("webDID", "DIMTasks")
  server.addTask({'task': 'PropertyServer',\
                  'utgid': 'PropertyServer',\
                  'command': '<SCRIPTS>/PropertyServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'LogGaudiSumSrv_HLTTOP',\
                  'utgid': 'LogGaudiSumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s gaudi -p /run/fmc/gaudisum.fifo", 'description': ''})

  server.addTask({'task': 'LogGaudi',\
                  'utgid': 'LogGaudi',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s gaudi -o /run/fmc/gaudisum.fifo", 'description': ''})

  server.addTask({'task': 'LogLHCb1SumSrv_HLTTOP',\
                  'utgid': 'LogLHCb1SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s lhcb1 -p /run/fmc/logLHCb1sum.fifo", 'description': ''})

  server.addTask({'task': 'LogLHCb1',\
                  'utgid': 'LogLHCb1',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s lhcb1 -o /run/fmc/logLHCb1sum.fifo", 'description': ''})

  server.addTask({'task': 'LogLHCb2SumSrv_HLTTOP',\
                  'utgid': 'LogLHCb2SumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s lhcb2 -p /run/fmc/logLHCb2sum.fifo", 'description': ''})

  server.addTask({'task': 'LogLHCb2',\
                  'utgid': 'LogLHCb2',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s lhcb2 -o /run/fmc/logLHCb2sum.fifo", 'description': ''})

  server.addTask({'task': 'LogLHCbASumSrv_HLTTOP',\
                  'utgid': 'LogLHCbASumSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s lhcba -p /run/fmc/logLHCbAsum.fifo", 'description': ''})

  server.addTask({'task': 'LogLHCbA',\
                  'utgid': 'LogLHCbA',\
                  'command': '<SCRIPTS>/LogCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "-l 1 -s lhcba -o /run/fmc/logLHCbAsum.fifo", 'description': ''})

  server.addSet({"task_set":"GaudiControlsLogs_HLTTOP", "description": ""})    ### [('host', '<NODE>'), ('next_level_server', '<TOP_LOGGER>'), ('gaudi', 1), ('LHCb1', 1), ('LHCb2', 1), ('LHCbA', 1), ('group', 'GaudiControlsLogs_HLTTOP')]
  server.assignTask("LogGaudiSumSrv_HLTTOP", "GaudiControlsLogs_HLTTOP")
  server.assignTask("LogGaudi", "GaudiControlsLogs_HLTTOP")
  server.assignTask("LogLHCb1SumSrv_HLTTOP", "GaudiControlsLogs_HLTTOP")
  server.assignTask("LogLHCb1", "GaudiControlsLogs_HLTTOP")
  server.assignTask("LogLHCb2SumSrv_HLTTOP", "GaudiControlsLogs_HLTTOP")
  server.assignTask("LogLHCb2", "GaudiControlsLogs_HLTTOP")
  server.assignTask("LogLHCbASumSrv_HLTTOP", "GaudiControlsLogs_HLTTOP")
  server.assignTask("LogLHCbA", "GaudiControlsLogs_HLTTOP")
  server.addTask({'task': 'LogPVSSSrv_HLTTOP',\
                  'utgid': 'LogPVSSSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s pvss -p /run/fmc/logPVSS.fifo", 'description': ''})

  server.addTask({'task': 'LogPVSS_HLTTOP',\
                  'utgid': 'LogPVSS',\
                  'command': '<SCRIPTS>/PVSSlogger.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logPVSS.fifo -O /run/fmc/logPVSS.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'HLT', 'description': ''})

  server.addSet({"task_set":"WinCCOALogs_HLTTOP", "description": ""})    ### [('host', '<NODE>'), ('project', 'HLT'), ('next_level_server', '<TOP_LOGGER>'), ('group', 'HLTTOP')]
  server.assignTask("LogPVSSSrv_HLTTOP", "WinCCOALogs_HLTTOP")
  server.assignTask("LogPVSS_HLTTOP", "WinCCOALogs_HLTTOP")
  server.addTask({'task': 'LogPVSSConfigSrv_HLTTOP',\
                  'utgid': 'LogPVSSConfigSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s pvssconfig -p /run/fmc/logPVSSConfig.fifo", 'description': ''})

  server.addSet({"task_set":"WinCCOAConfigLogs_HLTTOP", "description": ""})    ### [('host', '<NODE>'), ('project', 'HLT'), ('next_level_server', '<TOP_LOGGER>'), ('group', 'HLTTOP')]
  server.assignTask("LogPVSSConfigSrv_HLTTOP", "WinCCOAConfigLogs_HLTTOP")
  server.addSet({"task_set":"HLT01Tasks", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("LogDef", "HLT01Tasks")
  server.assignTask("TaskSupervisor", "HLT01Tasks")
  server.assignTask("ROpublish_HLTTOP", "HLT01Tasks")
  server.assignTask("PropertyServer", "HLT01Tasks")
  server.addClass({"node_class": "HLT01", "description": ""})
  server.assignSet("DIMTasks", "HLT01")
  server.assignSet("GaudiControlsLogs_HLTTOP", "HLT01")
  server.assignSet("HLT01Tasks", "HLT01")
  server.assignSet("WinCCOALogs_HLTTOP", "HLT01")
  server.assignSet("InfraStructure_HLTTOP_HLTTOP", "HLT01")
  server.assignSet("WinCCOAConfigLogs_HLTTOP", "HLT01")
  server.addNode({"regex": "hlt01", "description": ""})
  server.assignClass("HLT01","hlt01")
  server.addTask({'task': 'LogDefaultSrv_HLTTOP',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'LogGaudiSrv_HLTTOP',\
                  'utgid': 'LogGaudiSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s gaudi -p /run/fmc/logGaudi.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb1Srv_HLTTOP',\
                  'utgid': 'LogLHCb1Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb1 -p /run/fmc/logLHCb1.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb2Srv_HLTTOP',\
                  'utgid': 'LogLHCb2Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb2 -p /run/fmc/logLHCb2.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCbASrv_HLTTOP',\
                  'utgid': 'LogLHCbASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcba -p /run/fmc/logLHCbA.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'SSSRecover',\
                  'utgid': 'SSSRecover',\
                  'command': '/home/beat/ssscheck.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p -15 -n beat',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"InfraStructure_HLTTOP_HLTTOP", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('next_level_server', '<TOP_LOGGER>'), ('gaudilog', 1), ('group', 'InfraStructure_HLTTOP')]
  server.assignTask("LogDefaultSrv_HLTTOP", "InfraStructure_HLTTOP_HLTTOP")
  server.assignTask("LogGaudiSrv_HLTTOP", "InfraStructure_HLTTOP_HLTTOP")
  server.assignTask("LogLHCb1Srv_HLTTOP", "InfraStructure_HLTTOP_HLTTOP")
  server.assignTask("LogLHCb2Srv_HLTTOP", "InfraStructure_HLTTOP_HLTTOP")
  server.assignTask("LogLHCbASrv_HLTTOP", "InfraStructure_HLTTOP_HLTTOP")
  server.assignTask("TANServ", "InfraStructure_HLTTOP_HLTTOP")
  server.assignTask("pingSrv_u", "InfraStructure_HLTTOP_HLTTOP")
  server.assignTask("ROcollect", "InfraStructure_HLTTOP_HLTTOP")
  server.assignTask("SSSRecover", "InfraStructure_HLTTOP_HLTTOP")
  server.addTask({'task': 'CondDBXfer_0',\
                  'utgid': 'CondDBXfer_0',\
                  'command': '<SCRIPTS>/internal/condDBXfer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'CondDBCheck_0',\
                  'utgid': 'CondDBCheck_0',\
                  'command': '/group/online/AligWork/CheckConstants/checkConstants.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'CondDBXfer_RunTICK_0',\
                  'utgid': 'CondDBXfer_RunTICK_0',\
                  'command': '<SCRIPTS>/internal/condDBXfer_doRunTick.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'LHCb_HLT02_MoniOnlineAlig_0',\
                  'utgid': 'LHCb_HLT02_MoniOnlineAlig_0',\
                  'command': '/group/online/AligWork/MoniPlots/runAlignMonitor.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'BusyMon',\
                  'utgid': 'BusyMon',\
                  'command': '/group/online/dataflow/cmtuser/OnlineRelease/Online/FarmConfig/job/TopBusyMon.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'BusyPub',\
                  'utgid': 'BusyPub',\
                  'command': '/group/online/dataflow/cmtuser/OnlineRelease/Online/FarmConfig/job/BusyPub.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"HLT02Tasks", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("CondDBXfer_0", "HLT02Tasks")
  server.assignTask("CondDBCheck_0", "HLT02Tasks")
  server.assignTask("CondDBXfer_RunTICK_0", "HLT02Tasks")
  server.assignTask("LHCb_HLT02_MoniOnlineAlig_0", "HLT02Tasks")
  server.assignTask("BusyMon", "HLT02Tasks")
  server.assignTask("BusyPub", "HLT02Tasks")
  server.addClass({"node_class": "HLT02", "description": ""})
  server.assignSet("InfraStructure_HLTTOP_HLTTOP", "HLT02")
  server.assignSet("HLT02Tasks", "HLT02")
  server.addNode({"regex": "hlt02", "description": ""})
  server.assignClass("HLT02","hlt02")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'LogGaudiSrv',\
                  'utgid': 'LogGaudiSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s gaudi -p /run/fmc/logGaudi.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb1Srv',\
                  'utgid': 'LogLHCb1Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb1 -p /run/fmc/logLHCb1.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb2Srv',\
                  'utgid': 'LogLHCb2Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb2 -p /run/fmc/logLHCb2.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCbASrv',\
                  'utgid': 'LogLHCbASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcba -p /run/fmc/logLHCbA.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'SSSRecover',\
                  'utgid': 'SSSRecover',\
                  'command': '/home/beat/ssscheck.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p -15 -n beat',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"Infrastructure", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('next_level_server', '<TOP_LOGGER>'), ('gaudilog', 1), ('group', 'Infrastructure')]
  server.assignTask("LogDefaultSrv", "Infrastructure")
  server.assignTask("LogGaudiSrv", "Infrastructure")
  server.assignTask("LogLHCb1Srv", "Infrastructure")
  server.assignTask("LogLHCb2Srv", "Infrastructure")
  server.assignTask("LogLHCbASrv", "Infrastructure")
  server.assignTask("TANServ", "Infrastructure")
  server.assignTask("pingSrv_u", "Infrastructure")
  server.assignTask("ROcollect", "Infrastructure")
  server.assignTask("SSSRecover", "Infrastructure")
  server.addClass({"node_class": "CheckPoint", "description": ""})
  server.assignSet("Infrastructure", "CheckPoint")
  server.addNode({"regex": "checkpoint01", "description": ""})
  server.assignClass("CheckPoint","checkpoint01")
  server.addTask({'task': 'ipmi_farm_slow',\
                  'utgid': 'ipmi_farm_slow',\
                  'command': '/home/beat/ipmistart.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': "-c /home/beat/ipmi_slow_nodes.config -n ipmi_farm_slow -p /FMC", 'description': ''})

  server.addTask({'task': 'ipmi_farm_medium',\
                  'utgid': 'ipmi_farm_medium',\
                  'command': '/home/beat/ipmistart.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': "-c /home/beat/ipmi_medium_nodes.config -n ipmi_farm_medium -p /FMC", 'description': ''})

  server.addTask({'task': 'ipmi_farm_fast',\
                  'utgid': 'ipmi_farm_fast',\
                  'command': '/home/beat/ipmistart.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': "-c /home/beat/ipmi_fast_nodes.config -n ipmi_farm_fast -p /FMC", 'description': ''})

  server.addTask({'task': 'ipmi_farm_faster',\
                  'utgid': 'ipmi_farm_faster',\
                  'command': '/home/beat/ipmistart.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': "-c /home/beat/ipmi_faster_nodes.config -n ipmi_faster_nodes -p /FMC", 'description': ''})

  server.addTask({'task': 'ipmi_rest',\
                  'utgid': 'ipmi_rest',\
                  'command': '/home/beat/ipmistart.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 15 -X 20 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': "-c /home/beat/ipmi_rest.conf -n ipmi_rest -p /FMC", 'description': ''})

  server.addTask({'task': 'ipmi_mona',\
                  'utgid': 'ipmi_mona',\
                  'command': '/home/beat/ipmistart.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 15 -X 20 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': "-c /home/beat/ipmi_MONA.conf -n ipmi_mona -p /FMC", 'description': ''})

  server.addTask({'task': 'webDID',\
                  'utgid': 'webDID',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"FMC01Tasks", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("ipmi_farm_slow", "FMC01Tasks")
  server.assignTask("ipmi_farm_medium", "FMC01Tasks")
  server.assignTask("ipmi_farm_fast", "FMC01Tasks")
  server.assignTask("ipmi_farm_faster", "FMC01Tasks")
  server.assignTask("ipmi_rest", "FMC01Tasks")
  server.assignTask("ipmi_mona", "FMC01Tasks")
  server.assignTask("webDID", "FMC01Tasks")
  server.addClass({"node_class": "FMC01", "description": ""})
  server.assignSet("FMC01Tasks", "FMC01")
  server.addNode({"regex": "fmc01", "description": ""})
  server.assignClass("FMC01","fmc01")
  server.addTask({'task': 'webDID',\
                  'utgid': 'webDID',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"DIMTasks", "description": ""})    ### [('host', '<NODE>')]
  server.assignTask("webDID", "DIMTasks")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'ROpublish_ECS03',\
                  'utgid': 'ROpublish',\
                  'command': '<SCRIPTS>/MONPublish.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <DIM_DNS> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<DIM_DNS> -statDelay=8000 -mbmDelay=1500", 'description': ''})

  server.addTask({'task': 'ROcollect_ECS03',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <DIM_DNS> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROBridge_0',\
                  'utgid': 'ROBridge_0',\
                  'command': '<SCRIPTS>/ROBridge.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROLogger_0',\
                  'utgid': 'ROLogger_0',\
                  'command': '<SCRIPTS>/ROLogger.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'TaskSupervisor',\
                  'utgid': 'TaskSupervisor',\
                  'command': '<SCRIPTS>/TaskSupervisor.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'FarmStatSrv_0',\
                  'utgid': 'FarmStatSrv_0',\
                  'command': '<SCRIPTS>/FarmStatSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '-target=fmc01 -mbm', 'description': ''})

  server.addTask({'task': 'FarmStatSrv_1',\
                  'utgid': 'FarmStatSrv_1',\
                  'command': '<SCRIPTS>/FarmStatSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '-target=ecs03 -mbm -cpu', 'description': ''})

  server.addTask({'task': 'ActiveMQ_0',\
                  'utgid': 'ActiveMQ_0',\
                  'command': '<SCRIPTS>/ActiveMQ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 1 -X 5 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'ecs3', 'description': ''})

  server.addTask({'task': 'StatusStomp_0',\
                  'utgid': 'StatusStomp_0',\
                  'command': '<SCRIPTS>/StompSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 1 -X 5 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'ecs04 -topic=/topic/status -protocol=AMQ', 'description': ''})

  server.addTask({'task': 'StatusMQTT_0',\
                  'utgid': 'StatusMQTT_0',\
                  'command': '<SCRIPTS>/StompSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 1 -X 5 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'ecs04 -topic=/topic/item -protocol=MQTT', 'description': ''})

  server.addTask({'task': 'OrbitedSrv_0',\
                  'utgid': 'OrbitedSrv_0',\
                  'command': '<SCRIPTS>/OrbitedSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'orbited/orbited.activeMQ.0.cfg', 'description': ''})

  server.addTask({'task': 'ElogExtract',\
                  'utgid': 'ElogExtract',\
                  'command': '<SCRIPTS>/elog_extract.py',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ElogPublish',\
                  'utgid': 'ElogPublish',\
                  'command': '<SCRIPTS>/elog_publish.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'ecs04', 'description': ''})

  server.addTask({'task': 'BootMonSrv_0',\
                  'utgid': 'BootMonSrv_0',\
                  'command': '<SCRIPTS>/BootMonSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'AMQWatch_0',\
                  'utgid': 'AMQWatch_0',\
                  'command': '<SCRIPTS>/AMQWatch.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'Hlt_DeferSrv',\
                  'utgid': 'Hlt_DeferSrv',\
                  'command': '<SCRIPTS>/HltDeferSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '-listen=/ROpublish/HLTDefer -publish=/HLT/Deferred', 'description': ''})

  server.addTask({'task': 'Hlt_Hlt1Srv',\
                  'utgid': 'Hlt_Hlt1Srv',\
                  'command': '<SCRIPTS>/HltDeferSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '-listen=/ROpublish/HLT1 -publish=/HLT/HLT1', 'description': ''})

  server.addTask({'task': 'Hlt_Hlt1DQSrv',\
                  'utgid': 'Hlt_Hlt1DQSrv',\
                  'command': '<SCRIPTS>/HltDeferSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '-listen=/ROpublish/HLT1 -publish=/HLT/HLT1 -dis_dns=mona10', 'description': ''})

  server.addTask({'task': 'Hlt1Srv2fmc01',\
                  'utgid': 'Hlt1Srv2fmc01',\
                  'command': '<SCRIPTS>/HltDeferSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '-listen=/ROpublish/HLT1 -publish=/HLT/HLT1 -dis_dns=fmc01 -dic_dns=ecs03', 'description': ''})

  server.addTask({'task': 'FileStatistics',\
                  'utgid': 'FileStatistics',\
                  'command': '<SCRIPTS>/FileStatistics.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'HLTFileEqualizer',\
                  'utgid': 'HLTFileEqualizer',\
                  'command': '<SCRIPTS>/HLTFileEqualizer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'FarmStatusTrans',\
                  'utgid': 'FarmStatusTrans',\
                  'command': '<SCRIPTS>/FarmStatusTrans.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'SwDataScan',\
                  'utgid': 'SwDataScan',\
                  'command': '<SCRIPTS>/SwDataScan.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'CleanOptions',\
                  'utgid': 'CleanOptions',\
                  'command': '<SCRIPTS>/clean.nfs.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'PropertyServer',\
                  'utgid': 'PropertyServer',\
                  'command': '<SCRIPTS>/PropertyServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'PropertyInfo',\
                  'utgid': 'PropertyInfo',\
                  'command': '<SCRIPTS>/PropertyInfo.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'DidServer',\
                  'utgid': 'DidServer',\
                  'command': '<SCRIPTS>/DidServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'webDID_ecs01',\
                  'utgid': 'webDID_ecs01',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '-DDIM_DNS_NODE=ecs01 -DDIM_DID_PORT=2701', 'description': ''})

  server.addTask({'task': 'webDID_ecs05',\
                  'utgid': 'webDID_ecs05',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '-DDIM_DNS_NODE=ecs05 -DDIM_DID_PORT=2702', 'description': ''})

  server.addTask({'task': 'webDID_echvc01',\
                  'utgid': 'webDID_echvc01',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '-DDIM_DNS_NODE=echvc01 -DDIM_DID_PORT=2703', 'description': ''})

  server.addTask({'task': 'webDID_tmudaq01',\
                  'utgid': 'webDID_tmudaq01',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '-DDIM_DNS_NODE=tmudaq01 -DDIM_DID_PORT=2704', 'description': ''})

  server.addTask({'task': 'webDID_vedaqa01',\
                  'utgid': 'webDID_vedaqa01',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '-DDIM_DNS_NODE=vedaqa01 -DDIM_DID_PORT=2705', 'description': ''})

  server.addTask({'task': 'webDID_itecs01',\
                  'utgid': 'webDID_itecs01',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '-DDIM_DNS_NODE=itecs01 -DDIM_DID_PORT=2708', 'description': ''})

  server.addTask({'task': 'webDID_ttecs01',\
                  'utgid': 'webDID_ttecs01',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '-DDIM_DNS_NODE=ttecs01 -DDIM_DID_PORT=2709', 'description': ''})

  server.addTask({'task': 'webDID_otecs01',\
                  'utgid': 'webDID_otecs01',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '-DDIM_DNS_NODE=otecs01 -DDIM_DID_PORT=2710', 'description': ''})

  server.addTask({'task': 'webDID_riecs01',\
                  'utgid': 'webDID_riecs01',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '-DDIM_DNS_NODE=riecs01 -DDIM_DID_PORT=2711', 'description': ''})

  server.addTask({'task': 'webDID_ececs01',\
                  'utgid': 'webDID_ececs01',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '-DDIM_DNS_NODE=caecs01 -DDIM_DID_PORT=2712', 'description': ''})

  server.addTask({'task': 'webDID_muecs01',\
                  'utgid': 'webDID_muecs01',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '-DDIM_DNS_NODE=muecs01 -DDIM_DID_PORT=2713', 'description': ''})

  server.addTask({'task': 'webDID_muhva01',\
                  'utgid': 'webDID_muhva01',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '-DDIM_DNS_NODE=muhva01 -DDIM_DID_PORT=2714', 'description': ''})

  server.addTask({'task': 'webDID_muhvc01',\
                  'utgid': 'webDID_muhvc01',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '-DDIM_DNS_NODE=muhvc01 -DDIM_DID_PORT=2715', 'description': ''})

  server.addTask({'task': 'webDID_othv01',\
                  'utgid': 'webDID_othv01',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '-DDIM_DNS_NODE=othv01 -DDIM_DID_PORT=2716', 'description': ''})

  server.addTask({'task': 'webDID_otdaqa01',\
                  'utgid': 'webDID_otdaqa01',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '-DDIM_DNS_NODE=otdaqa01 -DDIM_DID_PORT=2717', 'description': ''})

  server.addTask({'task': 'webDID_otdaqc01',\
                  'utgid': 'webDID_otdaqc01',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '-DDIM_DNS_NODE=otdaqc01 -DDIM_DID_PORT=2718', 'description': ''})

  server.addTask({'task': 'webDID_infdai01',\
                  'utgid': 'webDID_infdai01',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '-DDIM_DNS_NODE=infdai01 -DDIM_DID_PORT=2730', 'description': ''})

  server.addTask({'task': 'webDID_infmag01',\
                  'utgid': 'webDID_infmag01',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '-DDIM_DNS_NODE=infmag01 -DDIM_DID_PORT=2732', 'description': ''})

  server.addSet({"task_set":"ECS03Tasks", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("LogDefaultSrv", "ECS03Tasks")
  server.assignTask("ROpublish_ECS03", "ECS03Tasks")
  server.assignTask("ROcollect_ECS03", "ECS03Tasks")
  server.assignTask("TANServ", "ECS03Tasks")
  server.assignTask("ROBridge_0", "ECS03Tasks")
  server.assignTask("ROLogger_0", "ECS03Tasks")
  server.assignTask("TaskSupervisor", "ECS03Tasks")
  server.assignTask("FarmStatSrv_0", "ECS03Tasks")
  server.assignTask("FarmStatSrv_1", "ECS03Tasks")
  server.assignTask("ActiveMQ_0", "ECS03Tasks")
  server.assignTask("StatusStomp_0", "ECS03Tasks")
  server.assignTask("StatusMQTT_0", "ECS03Tasks")
  server.assignTask("OrbitedSrv_0", "ECS03Tasks")
  server.assignTask("ElogExtract", "ECS03Tasks")
  server.assignTask("ElogPublish", "ECS03Tasks")
  server.assignTask("BootMonSrv_0", "ECS03Tasks")
  server.assignTask("AMQWatch_0", "ECS03Tasks")
  server.assignTask("Hlt_DeferSrv", "ECS03Tasks")
  server.assignTask("Hlt_Hlt1Srv", "ECS03Tasks")
  server.assignTask("Hlt_Hlt1DQSrv", "ECS03Tasks")
  server.assignTask("Hlt1Srv2fmc01", "ECS03Tasks")
  server.assignTask("FileStatistics", "ECS03Tasks")
  server.assignTask("HLTFileEqualizer", "ECS03Tasks")
  server.assignTask("FarmStatusTrans", "ECS03Tasks")
  server.assignTask("SwDataScan", "ECS03Tasks")
  server.assignTask("CleanOptions", "ECS03Tasks")
  server.assignTask("PropertyServer", "ECS03Tasks")
  server.assignTask("PropertyInfo", "ECS03Tasks")
  server.assignTask("DidServer", "ECS03Tasks")
  server.assignTask("webDID_ecs01", "ECS03Tasks")
  server.assignTask("webDID_ecs05", "ECS03Tasks")
  server.assignTask("webDID_echvc01", "ECS03Tasks")
  server.assignTask("webDID_tmudaq01", "ECS03Tasks")
  server.assignTask("webDID_vedaqa01", "ECS03Tasks")
  server.assignTask("webDID_itecs01", "ECS03Tasks")
  server.assignTask("webDID_ttecs01", "ECS03Tasks")
  server.assignTask("webDID_otecs01", "ECS03Tasks")
  server.assignTask("webDID_riecs01", "ECS03Tasks")
  server.assignTask("webDID_ececs01", "ECS03Tasks")
  server.assignTask("webDID_muecs01", "ECS03Tasks")
  server.assignTask("webDID_muhva01", "ECS03Tasks")
  server.assignTask("webDID_muhvc01", "ECS03Tasks")
  server.assignTask("webDID_othv01", "ECS03Tasks")
  server.assignTask("webDID_otdaqa01", "ECS03Tasks")
  server.assignTask("webDID_otdaqc01", "ECS03Tasks")
  server.assignTask("webDID_infdai01", "ECS03Tasks")
  server.assignTask("webDID_infmag01", "ECS03Tasks")
  server.addClass({"node_class": "ECS03", "description": ""})
  server.assignSet("ECS03Tasks", "ECS03")
  server.assignSet("DIMTasks", "ECS03")
  server.addNode({"regex": "ecs03", "description": ""})
  server.assignClass("ECS03","ecs03")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'LogGaudiSrv',\
                  'utgid': 'LogGaudiSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s gaudi -p /run/fmc/logGaudi.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb1Srv',\
                  'utgid': 'LogLHCb1Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb1 -p /run/fmc/logLHCb1.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCb2Srv',\
                  'utgid': 'LogLHCb2Srv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcb2 -p /run/fmc/logLHCb2.fifo', 'description': ''})

  server.addTask({'task': 'LogLHCbASrv',\
                  'utgid': 'LogLHCbASrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s lhcba -p /run/fmc/logLHCbA.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'SSSRecover',\
                  'utgid': 'SSSRecover',\
                  'command': '/home/beat/ssscheck.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p -15 -n beat',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"Infrastructure", "description": ""})    ### [('host', '<NODE>'), ('controlsPC', '<DIM_DNS>'), ('next_level_server', '<TOP_LOGGER>'), ('gaudilog', 1), ('group', 'Infrastructure')]
  server.assignTask("LogDefaultSrv", "Infrastructure")
  server.assignTask("LogGaudiSrv", "Infrastructure")
  server.assignTask("LogLHCb1Srv", "Infrastructure")
  server.assignTask("LogLHCb2Srv", "Infrastructure")
  server.assignTask("LogLHCbASrv", "Infrastructure")
  server.assignTask("TANServ", "Infrastructure")
  server.assignTask("pingSrv_u", "Infrastructure")
  server.assignTask("ROcollect", "Infrastructure")
  server.assignTask("SSSRecover", "Infrastructure")
  server.addTask({'task': 'LogPVSSSrv_ECS04',\
                  'utgid': 'LogPVSSSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': "<TOP_LOGGER> -l 1 -S 100 -s pvss -p /run/fmc/logPVSS.fifo", 'description': ''})

  server.addTask({'task': 'LogPVSS_ECS04',\
                  'utgid': 'LogPVSS',\
                  'command': '<SCRIPTS>/PVSSlogger.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logPVSS.fifo -O /run/fmc/logPVSS.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'ECS04', 'description': ''})

  server.addSet({"task_set":"WinCCOALogs_ECS04", "description": ""})    ### [('host', '<NODE>'), ('project', 'ECS04'), ('next_level_server', '<TOP_LOGGER>'), ('group', 'ECS04')]
  server.assignTask("LogPVSSSrv_ECS04", "WinCCOALogs_ECS04")
  server.assignTask("LogPVSS_ECS04", "WinCCOALogs_ECS04")
  server.addTask({'task': 'webDID',\
                  'utgid': 'webDID',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"DIMTasks", "description": ""})    ### [('host', '<NODE>')]
  server.assignTask("webDID", "DIMTasks")
  server.addClass({"node_class": "ECS04", "description": ""})
  server.assignSet("DIMTasks", "ECS04")
  server.assignSet("Infrastructure", "ECS04")
  server.assignSet("WinCCOALogs_ECS04", "ECS04")
  server.addNode({"regex": "ecs04", "description": ""})
  server.assignClass("ECS04","ecs04")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"LogSrvInfrastructure", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("LogDefaultSrv", "LogSrvInfrastructure")
  server.assignTask("TANServ", "LogSrvInfrastructure")
  server.assignTask("pingSrv_u", "LogSrvInfrastructure")
  server.assignTask("ROcollect", "LogSrvInfrastructure")
  server.addTask({'task': 'ROpublishLOGSRV',\
                  'utgid': 'ROpublish',\
                  'command': '<SCRIPTS>/MONPublish.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <DIM_DNS> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'ecs03 -statDelay=8000 -mbmDelay=1376"', 'description': ''})

  server.addTask({'task': 'TaskSupervisor',\
                  'utgid': 'TaskSupervisor',\
                  'command': '<SCRIPTS>/TaskSupervisor.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'webDID',\
                  'utgid': 'webDID',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"DIMTasks", "description": ""})    ### [('host', '<NODE>')]
  server.assignTask("webDID", "DIMTasks")
  server.addSet({"task_set":"LogSrvTasks", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("ROpublishLOGSRV", "LogSrvTasks")
  server.assignTask("TaskSupervisor", "LogSrvTasks")
  server.addClass({"node_class": "LogServerControl", "description": ""})
  server.assignSet("LogSrvInfrastructure", "LogServerControl")
  server.assignSet("DIMTasks", "LogServerControl")
  server.assignSet("LogSrvTasks", "LogServerControl")
  server.addNode({"regex": "logsrv01", "description": ""})
  server.assignClass("LogServerControl","logsrv01")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"LogSrvInfrastructure", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("LogDefaultSrv", "LogSrvInfrastructure")
  server.assignTask("TANServ", "LogSrvInfrastructure")
  server.assignTask("pingSrv_u", "LogSrvInfrastructure")
  server.assignTask("ROcollect", "LogSrvInfrastructure")
  server.addTask({'task': 'PartitionLog_LHCb',\
                  'utgid': 'PartitionLog_LHCb',\
                  'command': '<SCRIPTS>/PartitionLog.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'LHCb', 'description': ''})

  server.addSet({"task_set":"LHCb_LoggerTasks", "description": ""})    ### [('node', '<NODE>'), ('host', '<DIM_DNS>'), ('partition', 'LHCb'), ('match', 'logspartlhcb01')]
  server.assignTask("PartitionLog_LHCb", "LHCb_LoggerTasks")
  server.addClass({"node_class": "LHCb_LogServer", "description": ""})
  server.assignSet("LogSrvInfrastructure", "LHCb_LogServer")
  server.assignSet("LHCb_LoggerTasks", "LHCb_LogServer")
  server.addNode({"regex": "logspartlhcb01", "description": ""})
  server.assignClass("LHCb_LogServer","logspartlhcb01")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"LogSrvInfrastructure", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("LogDefaultSrv", "LogSrvInfrastructure")
  server.assignTask("TANServ", "LogSrvInfrastructure")
  server.assignTask("pingSrv_u", "LogSrvInfrastructure")
  server.assignTask("ROcollect", "LogSrvInfrastructure")
  server.addTask({'task': 'PartitionLog_LHCb1',\
                  'utgid': 'PartitionLog_LHCb1',\
                  'command': '<SCRIPTS>/PartitionLog.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'LHCb1', 'description': ''})

  server.addSet({"task_set":"LHCb1_LoggerTasks", "description": ""})    ### [('node', '<NODE>'), ('host', '<DIM_DNS>'), ('partition', 'LHCb1'), ('match', 'logspartlhcb101')]
  server.assignTask("PartitionLog_LHCb1", "LHCb1_LoggerTasks")
  server.addClass({"node_class": "LHCb1_LogServer", "description": ""})
  server.assignSet("LogSrvInfrastructure", "LHCb1_LogServer")
  server.assignSet("LHCb1_LoggerTasks", "LHCb1_LogServer")
  server.addNode({"regex": "logspartlhcb101", "description": ""})
  server.assignClass("LHCb1_LogServer","logspartlhcb101")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"LogSrvInfrastructure", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("LogDefaultSrv", "LogSrvInfrastructure")
  server.assignTask("TANServ", "LogSrvInfrastructure")
  server.assignTask("pingSrv_u", "LogSrvInfrastructure")
  server.assignTask("ROcollect", "LogSrvInfrastructure")
  server.addTask({'task': 'PartitionLog_LHCbA',\
                  'utgid': 'PartitionLog_LHCbA',\
                  'command': '<SCRIPTS>/PartitionLog.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'LHCbA', 'description': ''})

  server.addSet({"task_set":"LHCbA_LoggerTasks", "description": ""})    ### [('node', '<NODE>'), ('host', '<DIM_DNS>'), ('partition', 'LHCbA'), ('match', 'logspartlhcba01')]
  server.assignTask("PartitionLog_LHCbA", "LHCbA_LoggerTasks")
  server.addClass({"node_class": "LHCbA_LogServer", "description": ""})
  server.assignSet("LogSrvInfrastructure", "LHCbA_LogServer")
  server.assignSet("LHCbA_LoggerTasks", "LHCbA_LogServer")
  server.addNode({"regex": "logspartlhcba01", "description": ""})
  server.assignClass("LHCbA_LogServer","logspartlhcba01")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"LogSrvInfrastructure", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("LogDefaultSrv", "LogSrvInfrastructure")
  server.assignTask("TANServ", "LogSrvInfrastructure")
  server.assignTask("pingSrv_u", "LogSrvInfrastructure")
  server.assignTask("ROcollect", "LogSrvInfrastructure")
  server.addTask({'task': 'PartitionLog_FEST',\
                  'utgid': 'PartitionLog_FEST',\
                  'command': '<SCRIPTS>/PartitionLog.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'FEST', 'description': ''})

  server.addSet({"task_set":"FEST_LoggerTasks", "description": ""})    ### [('node', '<NODE>'), ('host', '<DIM_DNS>'), ('partition', 'FEST'), ('match', 'logspartfest01')]
  server.assignTask("PartitionLog_FEST", "FEST_LoggerTasks")
  server.addClass({"node_class": "FEST_LogServer", "description": ""})
  server.assignSet("LogSrvInfrastructure", "FEST_LogServer")
  server.assignSet("FEST_LoggerTasks", "FEST_LogServer")
  server.addNode({"regex": "logspartfest01", "description": ""})
  server.assignClass("FEST_LogServer","logspartfest01")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"LogSrvInfrastructure", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("LogDefaultSrv", "LogSrvInfrastructure")
  server.assignTask("TANServ", "LogSrvInfrastructure")
  server.assignTask("pingSrv_u", "LogSrvInfrastructure")
  server.assignTask("ROcollect", "LogSrvInfrastructure")
  server.addTask({'task': 'PartitionLog_LHCb2',\
                  'utgid': 'PartitionLog_LHCb2',\
                  'command': '<SCRIPTS>/PartitionLog.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'LHCb2', 'description': ''})

  server.addSet({"task_set":"LHCb2_LoggerTasks", "description": ""})    ### [('node', '<NODE>'), ('host', '<DIM_DNS>'), ('partition', 'LHCb2'), ('match', 'logspartfest01')]
  server.assignTask("PartitionLog_LHCb2", "LHCb2_LoggerTasks")
  server.addClass({"node_class": "LHCb2_LogServer", "description": ""})
  server.assignSet("LogSrvInfrastructure", "LHCb2_LogServer")
  server.assignSet("LHCb2_LoggerTasks", "LHCb2_LogServer")
  server.addNode({"regex": "logspartfest01", "description": ""})
  server.assignClass("LHCb2_LogServer","logspartfest01")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"LogSrvInfrastructure", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("LogDefaultSrv", "LogSrvInfrastructure")
  server.assignTask("TANServ", "LogSrvInfrastructure")
  server.assignTask("pingSrv_u", "LogSrvInfrastructure")
  server.assignTask("ROcollect", "LogSrvInfrastructure")
  server.addTask({'task': 'PartitionLog_VELO',\
                  'utgid': 'PartitionLog_VELO',\
                  'command': '<SCRIPTS>/PartitionLog.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'VELO', 'description': ''})

  server.addSet({"task_set":"VELO_LoggerTasks", "description": ""})    ### [('node', '<NODE>'), ('host', '<DIM_DNS>'), ('partition', 'VELO'), ('match', 'logspartvelo01')]
  server.assignTask("PartitionLog_VELO", "VELO_LoggerTasks")
  server.addClass({"node_class": "VELO_LogServer", "description": ""})
  server.assignSet("LogSrvInfrastructure", "VELO_LogServer")
  server.assignSet("VELO_LoggerTasks", "VELO_LogServer")
  server.addNode({"regex": "logspartvelo01", "description": ""})
  server.assignClass("VELO_LogServer","logspartvelo01")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"LogSrvInfrastructure", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("LogDefaultSrv", "LogSrvInfrastructure")
  server.assignTask("TANServ", "LogSrvInfrastructure")
  server.assignTask("pingSrv_u", "LogSrvInfrastructure")
  server.assignTask("ROcollect", "LogSrvInfrastructure")
  server.addTask({'task': 'PartitionLog_IT',\
                  'utgid': 'PartitionLog_IT',\
                  'command': '<SCRIPTS>/PartitionLog.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'IT', 'description': ''})

  server.addSet({"task_set":"IT_LoggerTasks", "description": ""})    ### [('node', '<NODE>'), ('host', '<DIM_DNS>'), ('partition', 'IT'), ('match', 'logspartit01')]
  server.assignTask("PartitionLog_IT", "IT_LoggerTasks")
  server.addClass({"node_class": "IT_LogServer", "description": ""})
  server.assignSet("LogSrvInfrastructure", "IT_LogServer")
  server.assignSet("IT_LoggerTasks", "IT_LogServer")
  server.addNode({"regex": "logspartit01", "description": ""})
  server.assignClass("IT_LogServer","logspartit01")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"LogSrvInfrastructure", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("LogDefaultSrv", "LogSrvInfrastructure")
  server.assignTask("TANServ", "LogSrvInfrastructure")
  server.assignTask("pingSrv_u", "LogSrvInfrastructure")
  server.assignTask("ROcollect", "LogSrvInfrastructure")
  server.addTask({'task': 'PartitionLog_TT',\
                  'utgid': 'PartitionLog_TT',\
                  'command': '<SCRIPTS>/PartitionLog.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'TT', 'description': ''})

  server.addSet({"task_set":"TT_LoggerTasks", "description": ""})    ### [('node', '<NODE>'), ('host', '<DIM_DNS>'), ('partition', 'TT'), ('match', 'logsparttt01')]
  server.assignTask("PartitionLog_TT", "TT_LoggerTasks")
  server.addClass({"node_class": "TT_LogServer", "description": ""})
  server.assignSet("LogSrvInfrastructure", "TT_LogServer")
  server.assignSet("TT_LoggerTasks", "TT_LogServer")
  server.addNode({"regex": "logsparttt01", "description": ""})
  server.assignClass("TT_LogServer","logsparttt01")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"LogSrvInfrastructure", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("LogDefaultSrv", "LogSrvInfrastructure")
  server.assignTask("TANServ", "LogSrvInfrastructure")
  server.assignTask("pingSrv_u", "LogSrvInfrastructure")
  server.assignTask("ROcollect", "LogSrvInfrastructure")
  server.addTask({'task': 'PartitionLog_OT',\
                  'utgid': 'PartitionLog_OT',\
                  'command': '<SCRIPTS>/PartitionLog.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'OT', 'description': ''})

  server.addSet({"task_set":"OT_LoggerTasks", "description": ""})    ### [('node', '<NODE>'), ('host', '<DIM_DNS>'), ('partition', 'OT'), ('match', 'logspartot01')]
  server.assignTask("PartitionLog_OT", "OT_LoggerTasks")
  server.addClass({"node_class": "OT_LogServer", "description": ""})
  server.assignSet("LogSrvInfrastructure", "OT_LogServer")
  server.assignSet("OT_LoggerTasks", "OT_LogServer")
  server.addNode({"regex": "logspartot01", "description": ""})
  server.assignClass("OT_LogServer","logspartot01")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"LogSrvInfrastructure", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("LogDefaultSrv", "LogSrvInfrastructure")
  server.assignTask("TANServ", "LogSrvInfrastructure")
  server.assignTask("pingSrv_u", "LogSrvInfrastructure")
  server.assignTask("ROcollect", "LogSrvInfrastructure")
  server.addTask({'task': 'PartitionLog_TDET',\
                  'utgid': 'PartitionLog_TDET',\
                  'command': '<SCRIPTS>/PartitionLog.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'TDET', 'description': ''})

  server.addSet({"task_set":"TDET_LoggerTasks", "description": ""})    ### [('node', '<NODE>'), ('host', '<DIM_DNS>'), ('partition', 'TDET'), ('match', 'logsparttdet01')]
  server.assignTask("PartitionLog_TDET", "TDET_LoggerTasks")
  server.addClass({"node_class": "TDET_LogServer", "description": ""})
  server.assignSet("LogSrvInfrastructure", "TDET_LogServer")
  server.assignSet("TDET_LoggerTasks", "TDET_LogServer")
  server.addNode({"regex": "logsparttdet01", "description": ""})
  server.assignClass("TDET_LogServer","logsparttdet01")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"LogSrvInfrastructure", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("LogDefaultSrv", "LogSrvInfrastructure")
  server.assignTask("TANServ", "LogSrvInfrastructure")
  server.assignTask("pingSrv_u", "LogSrvInfrastructure")
  server.assignTask("ROcollect", "LogSrvInfrastructure")
  server.addTask({'task': 'PartitionLog_CALO',\
                  'utgid': 'PartitionLog_CALO',\
                  'command': '<SCRIPTS>/PartitionLog.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'CALO', 'description': ''})

  server.addSet({"task_set":"CALO_LoggerTasks", "description": ""})    ### [('node', '<NODE>'), ('host', '<DIM_DNS>'), ('partition', 'CALO'), ('match', 'logspartcalo01')]
  server.assignTask("PartitionLog_CALO", "CALO_LoggerTasks")
  server.addClass({"node_class": "CALO_LogServer", "description": ""})
  server.assignSet("LogSrvInfrastructure", "CALO_LogServer")
  server.assignSet("CALO_LoggerTasks", "CALO_LogServer")
  server.addNode({"regex": "logspartcalo01", "description": ""})
  server.assignClass("CALO_LogServer","logspartcalo01")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"LogSrvInfrastructure", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("LogDefaultSrv", "LogSrvInfrastructure")
  server.assignTask("TANServ", "LogSrvInfrastructure")
  server.assignTask("pingSrv_u", "LogSrvInfrastructure")
  server.assignTask("ROcollect", "LogSrvInfrastructure")
  server.addTask({'task': 'PartitionLog_RICH',\
                  'utgid': 'PartitionLog_RICH',\
                  'command': '<SCRIPTS>/PartitionLog.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'RICH', 'description': ''})

  server.addSet({"task_set":"RICH_LoggerTasks", "description": ""})    ### [('node', '<NODE>'), ('host', '<DIM_DNS>'), ('partition', 'RICH'), ('match', 'logspartrich01')]
  server.assignTask("PartitionLog_RICH", "RICH_LoggerTasks")
  server.addClass({"node_class": "RICH_LogServer", "description": ""})
  server.assignSet("LogSrvInfrastructure", "RICH_LogServer")
  server.assignSet("RICH_LoggerTasks", "RICH_LogServer")
  server.addNode({"regex": "logspartrich01", "description": ""})
  server.assignClass("RICH_LogServer","logspartrich01")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"LogSrvInfrastructure", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("LogDefaultSrv", "LogSrvInfrastructure")
  server.assignTask("TANServ", "LogSrvInfrastructure")
  server.assignTask("pingSrv_u", "LogSrvInfrastructure")
  server.assignTask("ROcollect", "LogSrvInfrastructure")
  server.addTask({'task': 'PartitionLog_RICH1',\
                  'utgid': 'PartitionLog_RICH1',\
                  'command': '<SCRIPTS>/PartitionLog.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'RICH1', 'description': ''})

  server.addSet({"task_set":"RICH1_LoggerTasks", "description": ""})    ### [('node', '<NODE>'), ('host', '<DIM_DNS>'), ('partition', 'RICH1'), ('match', 'logspartrich101')]
  server.assignTask("PartitionLog_RICH1", "RICH1_LoggerTasks")
  server.addClass({"node_class": "RICH1_LogServer", "description": ""})
  server.assignSet("LogSrvInfrastructure", "RICH1_LogServer")
  server.assignSet("RICH1_LoggerTasks", "RICH1_LogServer")
  server.addNode({"regex": "logspartrich101", "description": ""})
  server.assignClass("RICH1_LogServer","logspartrich101")
  server.addTask({'task': 'LogDefaultSrv',\
                  'utgid': 'LogDefaultSrv',\
                  'command': '<SCRIPTS>/LogServer.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '<DIM_DNS> -S 100 -l 1 -s default -p /tmp/logSrv.fifo', 'description': ''})

  server.addTask({'task': 'TANServ',\
                  'utgid': 'TANServ',\
                  'command': '<SCRIPTS>/TanServ.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'pingSrv_u',\
                  'utgid': 'pingSrv_u',\
                  'command': '<SCRIPTS>/PingSrv.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addTask({'task': 'ROcollect',\
                  'utgid': 'ROcollect',\
                  'command': '<SCRIPTS>/ROMONCollect.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g root -p 0 -n root',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"LogSrvInfrastructure", "description": ""})    ### [('node', '<NODE>'), ('control', '<DIM_DNS>')]
  server.assignTask("LogDefaultSrv", "LogSrvInfrastructure")
  server.assignTask("TANServ", "LogSrvInfrastructure")
  server.assignTask("pingSrv_u", "LogSrvInfrastructure")
  server.assignTask("ROcollect", "LogSrvInfrastructure")
  server.addTask({'task': 'PartitionLog_RICH2',\
                  'utgid': 'PartitionLog_RICH2',\
                  'command': '<SCRIPTS>/PartitionLog.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': 'RICH2', 'description': ''})

  server.addSet({"task_set":"RICH2_LoggerTasks", "description": ""})    ### [('node', '<NODE>'), ('host', '<DIM_DNS>'), ('partition', 'RICH2'), ('match', 'logspartrich201')]
  server.assignTask("PartitionLog_RICH2", "RICH2_LoggerTasks")
  server.addClass({"node_class": "RICH2_LogServer", "description": ""})
  server.assignSet("LogSrvInfrastructure", "RICH2_LogServer")
  server.assignSet("RICH2_LoggerTasks", "RICH2_LogServer")
  server.addNode({"regex": "logspartrich201", "description": ""})
  server.assignClass("RICH2_LogServer","logspartrich201")
  server.addTask({'task': 'webDID',\
                  'utgid': 'webDID',\
                  'command': '<SCRIPTS>/webDID.sh',\
                  'task_parameters': '--no-authentication -K 120 -M 5 -X 300 -m <NODE> -E /run/fmc/logSrv.fifo -O /run/fmc/logSrv.fifo -g onliners -p 0 -n online',\
                  'command_parameters': '', 'description': ''})

  server.addSet({"task_set":"DIMTasks", "description": ""})    ### [('host', '<NODE>')]
  server.assignTask("webDID", "DIMTasks")
  server.addClass({"node_class": "HistoAnalysisNode", "description": ""})
  server.assignSet("DIMTasks", "HistoAnalysisNode")
  server.addNode({"regex": "hist0?", "description": ""})
  server.assignClass("HistoAnalysisNode","hist0?")
