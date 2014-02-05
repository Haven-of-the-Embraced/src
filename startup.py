#!/usr/bin/env python
import string
import os
import sys
from time import gmtime, strftime

#set port
# port = 2000
#Make If you enter a port, use that instead.

pid = str(os.getpid())
pidfile = "/tmp/startup.pid"

if os.path.isfile(pidfile):
#    print "%s already exists, exiting" % pidfile
    sys.exit()
else:
    file(pidfile, 'w').write(pid)

if (len(sys.argv) > 1):
    argument = sys.argv[1]

try:
    port = int(argument)
except:
    port = 2000
#
os.chdir('/home/havenlive/area')

if (os.path.exists('shutdown.txt')):
    os.system('rm -f shutdown.txt')


while True:
    #set limits.
    os.system('ulimit -c unlimited')
    os.system('ulimit -s unlimited')
    #Run the MUD, logging to /home/haven/haven/log/current/system.log.
    os.system('(/home/havenlive/src/rom {0} 2>&1) > /home/havenlive/log/current/system.log'.format(port))
    #Get the date, turn it into a filename for the daily logs.
    #filename = date.log ie: 14-jun-2011.log
    date = strftime("%d-%b-%Y", gmtime())
    filename = date+'.log'
    #Create the file, if it doesn't exist.
    os.system('touch /home/havenlive/log/{0}'.format(filename))
    #appent system.log to daily log.
    os.system('cat /home/havenlive/log/current/system.log >> /home/havenlive/log/{0}'.format(filename))


    if (os.path.exists('shutdown.txt')):
       # os.system('rm -f shutdown.txt')
        os.unlink(pidfile)
        exit()

    os.system('sleep 5s')


