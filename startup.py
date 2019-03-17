#!/usr/bin/env python
import string
import os
import sys
import resource
from time import gmtime, strftime

pid = str(os.getpid())
pidfile = "/tmp/startup.pid"

if os.path.isfile(pidfile):
#    print "%s already exists, exiting" % pidfile
    sys.exit()
else:
    open(pidfile, 'w').write(pid)

if (len(sys.argv) > 1):
    argument = sys.argv[1]

try:
    port = int(argument)
except:
    port = 2000
#
os.chdir('/home/havenlive/area')

if (os.path.exists('../data/shutdown.txt')):
    os.unlink(pidfile)
    exit()


while True:
    #Run the MUD, logging to /home/haven/haven/log/current/system.log.
    resource.setrlimit(
            resource.RLIMIT_CORE,
            (resource.RLIM_INFINITY, resource.RLIM_INFINITY))
    os.system('(/home/havenlive/src/rom {0} 2>&1) > /home/havenlive/log/current/system.log'.format(port))
    #Get the date, turn it into a filename for the daily logs.
    #filename = date.log ie: 14-jun-2011.log
    date = strftime("%Y-%b-%d", gmtime())
    filename = date+'.log'
    #Create the file, if it doesn't exist.
    os.system('touch /home/havenlive/log/{0}'.format(filename))
    #appent system.log to daily log.
    os.system('cat /home/havenlive/log/current/system.log >> /home/havenlive/log/{0}'.format(filename))


    if (os.path.exists('../data/shutdown.txt')):
       # os.system('rm -f ../data/shutdown.txt')
        os.unlink(pidfile)
        exit()

    os.system('sleep 5s')


