# Network-Simulator---C++
Shows a simulation of network of routers and machines.
Uses a command line interface to input commands. There is a list of specific commands it understands.
A command parser checks the commands entered.
The network is set up using a csv file which specifies where the machines and routers are and what their connections are.
Linear lists or splay trees are used for this purpose and user is provided with choice to choose.
If message is to be sent, Djisktra is used to find shortest path from machine to machine and that path of routers used to convey the message.
Path is written to a txt file and messages are also read from txt file.
If user wishes to view the shortest paths of all machines, they are displayed.
Threading is used so that even if a process is running, one can interrupt it and run another process on another thread and continue that thread when needed.
Upon entering exit, program ends.
