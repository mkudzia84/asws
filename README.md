# asws
Advanced Symphony Workload Simulator for Platform Symphony/IBM Spektrum Computing
Formely known as Platform Independend Symphony Simulation Oriented Failure Framework (p*ssoff)

Multithreaded client used to simulate various types of workloads submission patterns to Platform Symphony/IBM Spektrum Computing 
SOA middleware.

Written in C++ tested on Windows and Linux (both 32bit and 64bit) configurations incluing mixed configurations (Windows Client, Linux services, Linux client, Windows services etc)

Current features include:
- Workload pattern configuration in xml scripts
- Multithreaded submission mode
- Options to choose synchronous and asynchronous data retrieval mode
- Options to use blocking and message-overlapp asynchronous submission mode
- Options to configure common data, task payload (input and output), randomized task input and output payload
- Robust options to define workload per each submission thread (including session composision, number of repetitions, etc)
- idle-workload - sleep tasks 
- linpack workload to test the utilization
- Statistics gathering including
  * end-2-end runtime
  * workload submission time/cluster flooding time
  * workload submission rate (tasks/s)
  * LINPACK statstics (see LINPACK docs)

TODO:
- JSON format for batch definition
- CUDA workload
- XML batch definition docs (check the examples)

