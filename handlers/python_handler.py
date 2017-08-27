# -*- coding: utf-8 -*-
import sys
import os
import time
import calendar
from datetime import date
from datetime import datetime
import argparse

# Parse args
parser = argparse.ArgumentParser()
action = parser.add_mutually_exclusive_group(required=True)
action.add_argument('-s', action='store', dest='get_since', help='Parse tasks since certain date (format dd-mm-yy)')
action.add_argument('-y', action='store_true', dest='get_yesterday', help='Parse yesterday tasks')
action.add_argument('-d', action='store_true', dest='get_today', help='Parse current day tasks')
action.add_argument('-w', action='store_true', dest='get_week', help='Parse current week tasks')
action.add_argument('-m', action='store_true', dest='get_month', help='Parse current month tasks')

parser.add_argument('-t', action='store', dest='filter_task', help='Filter by task name')
parser.add_argument('-f', action='store_true', dest='want_format', help='Task times formated as H:M:S')
args = parser.parse_args()
"""
print(args.get_since)
print(args.get_yesterday)
print(args.get_today)
print(args.get_week)
print(args.get_month)
print(args.filter_task)
print(args.want_format)
"""

def get_prev_day(day):
    """Returns previous day as <class 'datetime.date'>"""
    try:
        return(date(day.year, day.month, day.day-1))
    except ValueError:
        try:
            _,d = calendar.monthrange(day.year, day.month-1)
            return(date(day.year, day.month-1, d))
        except ValueError:
            return(date(day.year-1, 12, 31))

def get_weekdays(day):
    """Returns all weekdays of certains day week"""
    retval = []
    if day.weekday() == 0: # Current day is already first weekday
        retval.append(day)
        return(retval)

    retval.append(day)
    d = get_prev_day(day)
    while d.weekday() >= 0:
        retval.append(d)
        d = get_prev_day(d)

    return(retval)
 
def build_path(day):
    """Builds full path to time file"""
    return(os.path.join(datadir, day.strftime("%d-%m-%y")))

def get_times_by_day(day, task=None):
    """Get times for each task at a certain day"""
    path = build_path(day)
    try:
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
    except FileNotFoundError:
        print("File {} not found, ignoring...".format(path))
    

def format_output():
    """Format output to H:M:S"""
    for k,v in tasks.items():
        print('{} : {}:{}:{}'.format(k, int(v/3600%3600), int(v/60%60), v%60))

def print_output():
    """Prints times information"""
    if args.want_format:
        format_output()
    else:
        print(tasks)

# General
datadir = os.path.join(os.path.expanduser('~'), '.task-timer')
tasks = {}

# Since date
if args.get_since is not None:
    since_date = datetime.strptime(args.get_since, "%d-%m-%y").date()
    today = date.today()
    
    if today == since_date:
        get_times_by_day(today, args.filter_task)
        print_output()
    else:
        get_times_by_day(today, args.filter_task)
        d = get_prev_day(today)
        while d != since_date:
            get_times_by_day(d, args.filter_task)
            d = get_prev_day(d)
        print_output()

# Yesterday
if args.get_yesterday:
    yesterday = get_prev_day(date.today())
    get_times_by_day(yesterday, args.filter_task)
    print_output()

# Today
if args.get_today:
    today = date.today()
    get_times_by_day(today, args.filter_task)
    print_output()

# This week
if args.get_week:
    today = date.today()
    weekdays = get_weekdays(today)

    for w in weekdays:
        get_times_by_day(w, args.filter_task)
    print_output()

# This month
if args.get_month:
    today = date.today()
    month = today.strftime("%m")
    files = os.listdir(datadir)
    filtered = list(filter(lambda x: x[3:5] == month , files))
    for f in filtered:
        as_date = datetime.strptime(f, "%d-%m-%y")
        get_times_by_day(as_date, args.filter_task)
    print_output()
            

