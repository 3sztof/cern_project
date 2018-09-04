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
