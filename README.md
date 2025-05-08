# DaemonForge
An application to receives executable software and have it operate as a daemon in Linux.


Installation: 

After cloning the repo, go to the root dir and do both make and make install.

Uninstallation:

In the root directory, make uninstall and make clean.

Usage:

Basic usage:

daemonforge ./myDaemon

For custom name in Systemd:

daemonforge ./myDaemon Daemon_name

For classic Unix Daemons (that involve forking):

daemonforge -f ./myDaemon Daemon_name

For python script Daemons that work with a venv:

daemonforge ./myDaemon.py -pv Daemon_name

