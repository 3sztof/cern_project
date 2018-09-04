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
