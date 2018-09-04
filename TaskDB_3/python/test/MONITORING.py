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
