def func(server):
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
