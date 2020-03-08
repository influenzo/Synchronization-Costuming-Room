#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#

Matthew Selva and Lorenzo DeSimone
Project 3: Synchronization

#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#=#

|=|=|=|=|=|=| Overview |=|=|=|=|=|=|

Compiles the DressingRoom.c file into an executable called DRoom, which can be run with ./DRoom in terminal.

The program takes in 7 integers as arguments:
2-4 costuming teams
10-50 pirates
10-50 ninjas
The average costuming time for a pirate
The average costuming time for a ninja
The average arrival time for a pirate
The average arrival time for a ninja

This project is centered around the balancing of critcal space with the two types of threads, represented by a costuming room, and pirates and ninjas, respectively.
The problemexplanation.txt file goes more in depth with the inner mechanics of the program and how our program performed on an example run. 


|=|=|=|=|=|=| Testing |=|=|=|=|=|=|

Testing is done through a single line of input in the terminal. The program will then run, printing the current state of the changing room as the program runs, including:
The ninja or pirate number
how long they waited
how long they were in the room
the total visits for that specific person
how much money they owe

and ouputting basic statistics at the end, including: 
Expenses for employing the costume team
How long each team was busy/free
Average queue length
Gross revenue
Average gold per visit
Total Revenue


Ultimately, we compiled our tests into the file test.txt with the accompanying output from that test in output.txt.



