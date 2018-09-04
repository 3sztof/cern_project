def func(server):
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
