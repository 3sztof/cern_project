def func(server):
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
