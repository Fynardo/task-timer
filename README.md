# task-timer
Arduino powered task timer with work time / free time alert


## Structure

* **task-timer**. Arduino code, reads tasks from serial, updates LCD and counts time.
* **recorders**. Some scripts in different languages to communicate with Arduino via serial and store task times.
* **handlers**. Calculate total time grouping by task/week/etc. Different languages too.


## Usage

Upload code to Arduino and execute the recorder script of your choice passing the task name as argument. These recorders will create a file for each day of work under *$HOME/.task-timer* directory



