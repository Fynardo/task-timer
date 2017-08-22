# -*- coding: utf-8 -*-

import sys
import os
import serial
import time

# Usage
# recorder.py <task>

if len(sys.argv) == 1:
    print('Usage: recorder.py <task>')
    exit(1)

# Config
port = '/dev/ttyACM0'
baud = 9600
destdir = os.path.join(os.path.expanduser('~'), '.task-timer')
destfile = os.path.join(destdir, time.strftime('%d-%m-%y'))

if not os.path.exists(destdir):
    os.mkdir(destdir)

# Serial set up
conn = serial.Serial(port, baud)

# Send task to Arduino
conn.write(sys.argv[1]+'\n')

# Record task info
task = conn.readline()
with open(destfile, 'a') as f:
    f.write(task)
