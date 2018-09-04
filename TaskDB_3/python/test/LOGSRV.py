def func(server):
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
