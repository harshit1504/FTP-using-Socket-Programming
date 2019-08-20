Names and roll numbers: Harshit Singh(160123010), Kshitij Nayar(160123016) and Kuldeep Sharma(160123017)
   
-------------------------------------------------------------------------
 File Transfer Protocol (FTP) using Client-Server socket programming
-------------------------------------------------------------------------

These functionalities have been implemented in the the program

PUT : Transfer a file from client to server
GET : Transfer a file from server to client
MGET : Transfer all files of a given extension from server to client
MPUT : Transfer all files of a given extension from client to server


File overwrite feature has been implemented along with it i.e. if thefile is already present at the destination, then the client will be asked whether to overwrite the file or not.

----------------------
compiling the code
----------------------

On Server’s terminal
$ gcc server.c -o server

On Client’s terminal
$ gcc client.c -o client

------------------------

To run the code, First on the Server’s terminal
$ ./server <Server Port number>

Then on Client’s terminal run
$ ./client <Server IP Address> <Server Port number>

---------------------
Commands
---------------------
Enter a choice:
put <filename>
get <filename>
mput <extention>
mget <extention>
quit


------------------------------
Closing Client and Server
------------------------------
Use command “quit” to quit client program. This will disconnect the client. You can exit the server by simply using Ctrl + C on the server terminal.


---------------------
Assumptions
---------------------
->The user shall give the input in legal format. Incomplete command or illegal characters in the command might lead to runtime crash and has not been handled assuming user shall provide the command in legal format.
->Filename must be a string of ASCII characters without space.
->The folders 'client_files' and 'server_files' act as the client’s and server’s disk. All the files are transferred within them.
-> FTP transfer may not work for large sized files (since library functions used don't support large files)
