/*test machine: CSEL-KH4250-18
* date: 12/6/19
* name: Peyton Nash, Taylor O'Neill
* x500: nashx282, oneil569
*/

The purpose of this program is to implement a map-reduce algorithm to mimic a lettercount of words within a given directory that contains only subfolders and .txt files that have one word per line. The program implements this using a sockets and a client server model. The client program will take in the directory, number of mapper processes, server IP and sercer port and the server program will specify the server port number. The client produces a log file with with various statements indicating the functionality of the program including the final counts of the beginning letter of each word.

The each program is compiled by running "make" in the directory where "src" and "include" are visible (like what was given to us as a template). "make" will generate an executable called "client" and "server" for the client and server programs respectively. Ensure "commands.txt" is located in the same directory as the executable.

The client and server programs are executed with the following syntaxes: 
	Client syntax: "$ .client/ <Test_Folder> <#ofMappers> <Server IP> <Server Port>
	Server syntax: "$ .server/ <Server Port>

Where the <Test_Folder> is the directory location to be tested, <#ofMappers> is between 0-32, and the <Server IP> is the IP Adress of the machine where the server program is executing. The <Server Port> is determined when executing the server executable. 

Assumptions:
  1) For "commands.txt" it is assumed that the line ends in "\n" character.
  2) Incorrect command codes get caught by client because there is no open/close connection because there is no indication of a connection on a bad command code in the example outputs folder.
  3) Any assumptions stated in section 7 of the project description.
  4) Server errors (like double Check-In) will result in the the thread sending an error message (RSP_NOK) and terminating the socket and itself.
  5) There will be no consequtive client programs called as explained in the discussion posts.
  6) The client only prints "close connection" after successful Check-Out.
  7) The server does not delete entries from UpdateStatus table after any thread terminates.
  8) The server is able to listen on INADDR_ANY.

For project contributions, all work on the project was done side-by-side in the lab.

Extra credit is attempted.
