[![cern](https://upload.wikimedia.org/wikipedia/en/2/22/Logo_of_CERN.png)](https://home.cern/)
# CERN LHCb Online Summer Student Programme 2018
> Revision 1.1
> Author: [Krzysztof Wilczynski](https://www.linkedin.com/in/3sztof/)
> Supervisor: [Markus Frank]()

## Contents
- [Resources](https://github.com/3sztof/cern_project#resources)
- [Repository structure](https://github.com/3sztof/cern_project#repository-structure)
- [Quickstart guide](https://github.com/3sztof/cern_project#quickstart-guide)
    - [Windows](#windows)
    - [Linux](#linux)
    - [Docker (coming sometime in the future)](#docker)

## Resources
| Resource | Description |
| ------ | ------ |
|[Report](http://cds.cern.ch/record/2639717/files/Krzysztof_Wilczynski_2018_CERN_Summer_Student_Report.pdf) | The Summer Student Programme final project report. |
| [Dillinger](https://www.dillinger.io) | Online markdown markup editor. |

## Repository structure
| Directory | Description |
| --------- | ----------- |
| TaskDB_2 | Python 2.7 backend version, discontinued. |
| TaskDB_3 | Python 3.6 backend version (main). |
| Sencha   | Root for sencha app development, then LHCb/build dir is meant to be copied to TaskDB folder to be hosted on the same port as APIs (a Python file server - alternative to Xampp) |
| Libs | Ready to copy to Sencha root ext and .sencha dirs needed to build sencha app with 'sencha app watch' and 'sencha app build' (for SDK ver. 6.2.0). |
| Docs |	Project documentation, instructions, database and gui diagrams. |
| Legacy |	My old scripts (prototypes for later developments by Markus Frank. I leave them here as some contain clever methods and are easier to understand / use than production version that is encapsulated to run as single instance. |
| FMC |	LHCb Online Farm Monitoring and Control system - a base for the project. |

## Quickstart guide

First, clone the repository (Windows / Linux):
```sh
$ git clone https://bitbucket.org/3sztof/lhcb_online
$ cd lhcb_online
$ cd TaskDB_3
```

Depending on the host operating system:

- **Windows** <a name="windows"></a>
    - Set up the Python environment to the local Python dependencies (while in \lhcb_online\TaskDB_3):
        ```sh
        > python\TaskDB\Setup.bat
        ```
    - Run the Python dev server (make sure to use Python 3 and a free port):
        ```sh
        > python python\TaskDB\Server.py -d -p 8081 -s sqlite:///LHCbOnline.db --run -t json
        ```
        Might be useful in dealing with "permission denied" or "port in use" errors:
          - Locate the process using port 8081: 
            ```sh 
            > netstat -ano | find ":8081" 
            ```
        - Kill a process by the PID found in the previous step:
            ```sh
            > taskkill /F /PID <process_pid>
            ```
        - Check the Python verison (determined by the first Python distribution in the PATH environment variable):
            ```sh
            > python --version
            ```
    - The web GUI should now be served under: http://localhost:8081/LHCb
- **Linux** <a name="linux"></a>
    - Set up the Python environment to the local Python dependencies (while in /lhcb_online/TaskDB_3):
        ```sh
        $ export PYTHONPATH=$(pwd)/python
        ```
    - Run the Python dev server (make sure to use Python 3 and a free port note that sometimes the command is "python3"):
        ```sh
        $ python python/TaskDB/Server.py -d -p 8081 -s sqlite:///LHCbOnline.db --run -t json
        ```
        Might be useful in dealing with "permission denied" or "port in use" errors:
          - Locate the process using port 8081 (might be needed to install net-tools via apt): 
            ```sh 
            $ netstat -ltnp | grep ':8081'
            ```
        - Kill a process by the PID found in the previous step:
            ```sh
            $ kill <process_pid>
            ```
        - Check the Python verison (not needed if installed as "python3", obviously):
            ```sh
            $ python --version
            ```
        
    - The web GUI should now be served under: http://localhost:8081/LHCb
- **Docker** <a name="docker"></a> 
    not yet done, too busy



