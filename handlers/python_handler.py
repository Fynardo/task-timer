# -*- coding: utf-8 -*-
import sys
import os
import time
import argparse

# Parse args
parser = argparse.ArgumentParser()
action = parser.add_mutually_exclusive_group(required=True)
action.add_argument('-s', action='store', dest='get_since', help='Parse tasks since certain date (format dd-mm-yy)')
action.add_argument('-d', action='store_true', dest='get_today', help='Parse current day tasks')
action.add_argument('-w', action='store_true', dest='get_week', help='Parse current week tasks')
action.add_argument('-m', action='store_true', dest='get_month', help='Parse current month tasks')

parser.add_argument('-t', action='store', dest='filter_task', help='Filter by task name')
parser.add_argument('-f', action='store_true', dest='want_format', help='Task times formated as H:M:S')
args = parser.parse_args()
"""
print(args.get_since)
print(args.get_today)
print(args.get_week)
print(args.get_month)
print(args.filter_task)
print(args.want_format)
"""

def get_times_by_day(path, task=None):
    """Get times for each task at a certain day"""
    with open(path, 'r') as f:
        lines = f.readlines()
    for l in lines:
        k,v = l[:-1].split('-')
        if task is not None and task != k:
            continue
        try:
            tasks[k] += int(v)
        except KeyError:
            tasks[k] = int(v)

def format_output():
    """Format output to H:M:S"""
    for k,v in tasks.items():
        print('{} : {}:{}:{}'.format(k, int(v/3600%3600), int(v/60%60), v%60))

# General
datadir = os.path.join(os.path.expanduser('~'), '.task-timer')
tasks = {}

# Since date
if args.get_since is not None:
    #TODO
    pass

# Today
if args.get_today:
    today = time.strftime("%d-%m-%y")
    path = os.path.join(datadir, today)
    get_times_by_day(path, args.filter_task)
    if args.want_format:
        format_output()
    else:
        print(tasks)

# This week
if args.get_week:
    #TODO
    pass

# This month
if args.get_month:
    month = time.strftime("%m")
    files = os.listdir(datadir)
    filtered = list(filter(lambda x: x[3:5] == month , files))
    for f in filtered:
        fpath = os.path.join(datadir, f)
        get_times_by_day(fpath, args.filter_task)
    if args.want_format:
        format_output()
    else:
        print(tasks)
            

