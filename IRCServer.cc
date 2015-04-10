
const char * usage =
"                                                               \n"
"IRCServer:                                                   \n"
"                                                               \n"
"Simple server program used to communicate multiple users       \n"
"                                                               \n"
"To use it in one window type:                                  \n"
"                                                               \n"
"   IRCServer <port>                                          \n"
"                                                               \n"
"Where 1024 < port < 65536.                                     \n"
"                                                               \n"
"In another window type:                                        \n"
"                                                               \n"
"   telnet <host> <port>                                        \n"
"                                                               \n"
"where <host> is the name of the machine where talk-server      \n"
"is running. <port> is the port number you used when you run    \n"
"daytime-server.                                                \n"
"                                                               \n";

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "IRCServer.h"
#include "HashTableVoid.h"
using namespace std;

int QueueLength = 5;
fstream passFile;
fstream userFile;
int
IRCServer::open_server_socket(int port) {

	// Set the IP address and port for this server
	struct sockaddr_in serverIPAddress; 
	memset( &serverIPAddress, 0, sizeof(serverIPAddress) );
	serverIPAddress.sin_family = AF_INET;
	serverIPAddress.sin_addr.s_addr = INADDR_ANY;
	serverIPAddress.sin_port = htons((u_short) port);
  
	// Allocate a socket
	int masterSocket =  socket(PF_INET, SOCK_STREAM, 0);
	if ( masterSocket < 0) {
		perror("socket");
		exit( -1 );
	}

	// Set socket options to reuse port. Otherwise we will
	// have to wait about 2 minutes before reusing the sae port number
	int optval = 1; 
	int err = setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, 
			     (char *) &optval, sizeof( int ) );
	
	// Bind the socket to the IP address and port
	int error = bind( masterSocket,
			  (struct sockaddr *)&serverIPAddress,
			  sizeof(serverIPAddress) );
	if ( error ) {
		perror("bind");
		exit( -1 );
	}
	
	// Put socket in listening mode and set the 
	// size of the queue of unprocessed connections
	error = listen( masterSocket, QueueLength);
	if ( error ) {
		perror("listen");
		exit( -1 );
	}

	return masterSocket;
}

void
IRCServer::runServer(int port)
{
	int masterSocket = open_server_socket(port);

	initialize();
	
	while ( 1 ) {
		
		// Accept incoming connections
		struct sockaddr_in clientIPAddress;
		int alen = sizeof( clientIPAddress );
		int slaveSocket = accept( masterSocket,
					  (struct sockaddr *)&clientIPAddress,
					  (socklen_t*)&alen);
		
		if ( slaveSocket < 0 ) {
			perror( "accept" );
			exit( -1 );
		}
		
		// Process request.
		processRequest( slaveSocket );		
	}
}

int
main( int argc, char ** argv )
{
	// Print usage if not enough arguments
	if ( argc < 2 ) {
		fprintf( stderr, "%s", usage );
		exit( -1 );
	}
	
	// Get the port from the arguments
	int port = atoi( argv[1] );

	IRCServer ircServer;

	// It will never return
	ircServer.runServer(port);
    ircServer.initialize();
	
}

//
// Commands:
//   Commands are started y the client.
//
//   Request: ADD-USER <USER> <PASSWD>\r\n
//   Answer: OK\r\n or DENIED\r\n
//
//   REQUEST: GET-ALL-USERS <USER> <PASSWD>\r\n
//   Answer: USER1\r\n
//            USER2\r\n
//            ...
//            \r\n
//
//   REQUEST: CREATE-ROOM <USER> <PASSWD> <ROOM>\r\n
//   Answer: OK\n or DENIED\r\n
//
//   Request: LIST-ROOMS <USER> <PASSWD>\r\n
//   Answer: room1\r\n
//           room2\r\n
//           ...
//           \r\n
//
//   Request: ENTER-ROOM <USER> <PASSWD> <ROOM>\r\n
//   Answer: OK\n or DENIED\r\n
//
//   Request: LEAVE-ROOM <USER> <PASSWD>\r\n
//   Answer: OK\n or DENIED\r\n
//
//   Request: SEND-MESSAGE <USER> <PASSWD> <MESSAGE> <ROOM>\n
//   Answer: OK\n or DENIED\n
//
//   Request: GET-MESSAGES <USER> <PASSWD> <LAST-MESSAGE-NUM> <ROOM>\r\n
//   Answer: MSGNUM1 USER1 MESSAGE1\r\n
//           MSGNUM2 USER2 MESSAGE2\r\n
//           MSGNUM3 USER2 MESSAGE2\r\n
//           ...\r\n
//           \r\n
//
//    REQUEST: GET-USERS-IN-ROOM <USER> <PASSWD> <ROOM>\r\n
//    Answer: USER1\r\n
//            USER2\r\n
//            ...
//            \r\n
//

void
IRCServer::processRequest( int fd )
{
	// Buffer used to store the command received from the client
	const int MaxCommandLine = 1024;
	char commandLine[ MaxCommandLine + 1 ];
	int commandLineLength = 0;
	int n;
	
	// Currently character read
	unsigned char prevChar = 0;
	unsigned char newChar = 0;
	
	//
	// The client should send COMMAND-LINE\n
	// Read the name of the client character by character until a
	// \n is found.
	//

	// Read character by character until a \n is found or the command string is full.
	while ( commandLineLength < MaxCommandLine &&
		read( fd, &newChar, 1) > 0 ) { // read from client

		if (newChar == '\n' && prevChar == '\r') {
			break;
		} 
		
		commandLine[ commandLineLength ] = newChar;
		commandLineLength++;

		prevChar = newChar;
	}
	
	// Add null character at the end of the string
	// Eliminate last \r
	commandLineLength--;
        commandLine[ commandLineLength ] = 0;
    
	printf("RECEIVED: %s\n", commandLine);
    // breakup comandLine using substr and delimiter && s.find-- then store into an array

	printf("The commandLine has the following format:\n");
	printf("COMMAND <user> <password> <arguments>. See below.\n");
	printf("You need to separate the commandLine into those components\n");
	printf("For now, command, user, and password are hardwired.\n");
    string s(commandLine);
	istringstream iss(s);
	string token;
	string strings[100];
	int d = 0; // also number of arguments 
	while(getline(iss, token, ' ')) // parsing
	{
          strings[d] = token;
	      //cout << strings[d] << endl;
	      d++;
	}
	
	const char * command = strings[0].c_str();
	const char * user = strings[1].c_str(); //strings[1]
	const char * password = strings[2].c_str();
	const char * args = strings[3].c_str();

	printf("command=%s\n", command);
	printf("user=%s\n", user);
	printf( "password=%s\n", password );
	printf("args=%s\n", args);

	if (!strcmp(command, "ADD-USER")) {
		addUser(fd, user, password, args);
	}
	else if (!strcmp(command, "ENTER-ROOM")) {
		enterRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "LEAVE-ROOM")) {
		leaveRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "SEND-MESSAGE")) {
		sendMessage(fd, user, password, args);
	}
	else if (!strcmp(command, "GET-MESSAGES")) {
		getMessages(fd, user, password, args);
	}
	else if (!strcmp(command, "GET-USERS-IN-ROOM")) {
		getUsersInRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "GET-ALL-USERS")) {
		getAllUsers(fd, user, password, args);
	}
	else if (!strcmp(command, "CREATE-ROOM")) {
		createRoom(fd, user, password, args);
	}
	else if (!strcmp(command, "LIST-ROOMS")) {
		listRoom(fd, user, password, args);
	}
	else {
		const char * msg =  "UNKNOWN COMMAND\r\n";
		write(fd, msg, strlen(msg));
	}

	// Send OK answer
	//const char * msg =  "OK\n";
	//write(fd, msg, strlen(msg));

	close(fd);	
}

void
IRCServer::initialize()
{
	// FILE *fp = fopen(PASSWORD_FILE,"a+"); // Open password file-- fopen PASSWORD_FILE
    printf("Initialize\n");
    //fstream passFile;
    passFile.open(PASSWORD_FILE);
    string line;
    string pass[10000];
    int a = 0;
    if (passFile.is_open())
    {
      while ( getline (passFile,line) ) // separated by \n
        {
           pass[a] = line;
           cout << pass[a] << '\n';
           a++;
        }
      passFile.close();
    } else {
      cout << "Can't read file\n";
    }
	// Initialize users in room

	// Initalize message list

}

bool
IRCServer::checkPassword(int fd, const char * user, const char * password) {
	// Here check the password--find user and see if correct password 
    int countUser = 1;
    int countPass = 1;
    // find user
    userFile.open(USER_FILE, std::fstream::in | std::fstream::out | std::fstream::app); 
    if (userFile.is_open()) // check users
     {
        string line;
		while (getline(userFile, line)) // separated by \n
		{
            string str13(user);
            if(line.compare(str13) == 0) { 
               break;
            }
            countUser++;
		}
		userFile.close();
    } 
    // get countUser and iterate thru countPass-passfile 
    passFile.open(PASSWORD_FILE, std::fstream::in | std::fstream::out | std::fstream::app); 
    if (passFile.is_open()) // check users
     {
        string line;
		while (getline(passFile, line)) 
		{
            if(countPass == countUser) {
               string str13(password);
               if(line.compare(str13) == 0) { 
                  return true;
               } else {
                  return false;
               }
            }
            countPass++;
		}
		passFile.close();
    } 
	return false;
}

void
IRCServer::addUser(int fd, const char * user, const char * password, const char * args)
{
    userFile.open(USER_FILE, std::fstream::in | std::fstream::out | std::fstream::app); 
    if (userFile.is_open()) // check users
     {
        string line;
        int count = 0;
		while (getline(userFile, line)) // separated by \n
		{
            string str13(user);
            if(line.compare(str13) == 0) { 
               count++;
            }
		}
        if(count > 0) {
               const char * msg =  "DENIED\r\n";
	           write(fd, msg, strlen(msg));
        } else {
	     	   const char * msg = "OK\r\n";
			   write(fd, msg, strlen(msg));
        }
		userFile.close();
    } 
    const char * msg1 = "THATS ALL!\r\n";
	write(fd, msg1, strlen(msg1));
    passFile.open(PASSWORD_FILE, std::fstream::in | std::fstream::out | std::fstream::app);
    if (passFile.is_open())
    {
      passFile<< password << '\n';
      passFile.close();
    } else {
      cout << "Can't read file\n";
    } // adding pass to file
    userFile.open(USER_FILE, std::fstream::in | std::fstream::out | std::fstream::app);
    if (userFile.is_open())
    {
      userFile<< user << '\n';
      userFile.close();
    } else {
      cout << "Can't read file\n";
    }
	return;		
}

void
IRCServer::createRoom(int fd, const char * user, const char * password, const char * args)
{
    HashTableVoid h; // room 1
    void * gradev;
    //char * grade;
    bool e; 
    h.insertItem(user,(void *)password);
    e = h.find(user,&gradev);
    //grade = (char *)gradev;
    assert(e);
    assert(strcmp((char *)gradev,password));
    void * pass;
    if((!h.insertItem(user,(void **)""))) { 
      //h.insertItem(user,(void *)""); // not found- add
	  const char * msg =  "OK FROM HASHTABLE\r\n";
	  write(fd, msg, strlen(msg)); // to telnet
      cout << "HASHTABLE ADDED!" << user ;
    } else {
      cout << "DENY!";
      const char * msg =  "DENIED2 FROM HASHTABLE found but wrong pass\r\n";
	  write(fd, msg, strlen(msg));
      if(strcmp((char *) pass, password) == 0) { // user found check pass -- still deny add
        const char * msg =  "DENIED FROM HASHTABLE found but wrong pass\r\n";
	    write(fd, msg, strlen(msg));
      } else {
        const char * msg =  "DENIED FROM HASHTABLE\r\n";
	    write(fd, msg, strlen(msg));
      }
    }
}

void
IRCServer::listRoom(int fd, const char * user, const char * password, const char * args)
{
  if(checkPassword(fd, user, password)) {
  } else {
        const char * msg =  "DENIED\r\n";
	    write(fd, msg, strlen(msg));
  }
}

void
IRCServer::enterRoom(int fd, const char * user, const char * password, const char * args)
{
  if(checkPassword(fd, user, password)) {
  } else {
        const char * msg =  "DENIED\r\n";
	    write(fd, msg, strlen(msg));
  } 
}

void
IRCServer::leaveRoom(int fd, const char * user, const char * password, const char * args)
{ 
   if(checkPassword(fd, user, password)) {
  } else {
        const char * msg =  "DENIED\r\n";
	    write(fd, msg, strlen(msg));
  }
}

void
IRCServer::sendMessage(int fd, const char * user, const char * password, const char * args)
{
   if(checkPassword(fd, user, password)) {
  } else {
        const char * msg =  "DENIED\r\n";
	    write(fd, msg, strlen(msg));
  }
}

void
IRCServer::getMessages(int fd, const char * user, const char * password, const char * args)
{
   if(checkPassword(fd, user, password)) {
  } else {
        const char * msg =  "DENIED\r\n";
	    write(fd, msg, strlen(msg));
  }
}

void
IRCServer::getUsersInRoom(int fd, const char * user, const char * password, const char * args)
{
   if(checkPassword(fd, user, password)) {
  } else {
        const char * msg =  "DENIED\r\n";
	    write(fd, msg, strlen(msg));
  }
}

void
IRCServer::getAllUsers(int fd, const char * user, const char * password,const  char * args)
{
  if(checkPassword(fd, user, password)) {
	userFile.open(USER_FILE);
	string line;
	string pass[10000];
    int n;
	if (userFile.is_open())
     {
		while (getline(userFile, line)) // separated by \n
		{
            stringstream ss;
            ss << line << "\r\n";
            string s = ss.str();
			const char * msg = s.c_str();
			write(fd, msg, strlen(msg));
		}
		userFile.close();
    }
	else {
	 	cout << "Can't read file\n";
	} 
  } else {
        const char * msg =  "DENIED\r\n";
	    write(fd, msg, strlen(msg));
  }
}

