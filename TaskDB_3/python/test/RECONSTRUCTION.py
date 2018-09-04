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
