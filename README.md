# Assignment6OS
Assignment 6 OS


SOURCES:
guard.cpp from the model

Singleton:  https://refactoring.guru/design-patterns/singleton/cpp/example

Files in the repo:

Question 1+2: main1.c , How to run : gcc main1.c -pthread -o "name", then ./name
Question 3: server.c+client.cpp+makefile : make all , then ./server and for client: ./client 127.0.0.1
Question 4: guard.cpp,  How to run : clang main1.c -pthread -o "name", then ./name
Question 5: singleton.cpp , clang main1.c -pthread -o "name", then ./name

Question 3: The client continuously sends strings to the server, those string are stored in a queue , until the user sends the "FINISH" command
which then the server create 3 new active objects, each one serves it's purpose  as requested in the assignment.
To stop the client from stoping sending strings , and signaling the server to create the AO as requested, the client mus enter the "FINISH" command.

disclaimer: after inputing the FINISH command , the user will receive exactly 3*(n) string from the server , when n is the number of strings sent to the server.
3 strings are the strings encrypted in ceaser cipher , 3 strings converted to from small to big letters and vice versa, and the 3 original strings sent by the user.
In this question, I created 2 threads, on which sends the strings, and second one which always listens for data from the server.

When sending small letters to the servers the ceaser will not work on them , since we were requested to do it only on big letters.

Example  Question 3:


![Assignment6](https://user-images.githubusercontent.com/54214707/172061735-8ee64a51-97bf-4509-a334-90e3ea952672.PNG)


