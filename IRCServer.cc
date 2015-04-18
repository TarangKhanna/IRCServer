    
const char * usage =
"                                                               \n"
"IRCServer:                                                   \n"
"                                                               \n"
"Simple server parogram used to communicate multiple users       \n"
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
#include <vector>
#include <algorithm> 
#include "IRCServer.h"
#include "HashTableVoid.h"
using namespace std;


int QueueLength = 5;
fstream passFile;
fstream userFile;
fstream roomFile;
HashTableVoid h; // USER and PASS in room - bucket 0 is room 0
HashTableVoid h2; // MESSAGE and USER in room - bucket 0 is room 0
vector<string> userVec;
vector<string> passVec;
vector<string> roomVec;
int hTableCount = 0; // keep track of how many rooms and when to realloc
int hTableMax = 10;
int bucketCount = 0;
int numRooms = 0;
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
    char command1[1025];
  char user2[1025];
    char password3[1025]; 
    char args4[1025];
    memset(command1, 0, 1025);
    memset(user2, 0, 1025);
    memset(password3, 0, 1025);
    memset(args4, 0, 1025);
  int nRead = sscanf(commandLine, "%s %s %s %[^\n]", command1, user2, password3, args4);
  const char * command = command1;
  const char * user = user2;
  const char * password = password3;
  const char * args = args4;
    
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
    printf("Initialize\n");
    passFile.open(PASSWORD_FILE);
    string line;
    int n = 1;
    if (passFile.is_open())
    {
      while(getline (passFile,line)) {
        if(n % 2 == 1) {
            userVec.push_back(line); // user\npassword\n\nuser(2)
            getline (passFile,line);
            passVec.push_back(line);
            //cout << line << '\n';
            n++;
         } else {
            getline (passFile,line); // space
            n++;
          }
      }
      passFile.close();
    }
  // Initialize users in room
    ofstream resetRoom; // reset room file
    resetRoom.open(ROOM_FILE, std::ofstream::out | std::ofstream::trunc);
    resetRoom.close();
  // Initalize message list
}

bool
IRCServer::checkPassword(int fd, const char * user, const char * password) {
  // Here check the password--find user and see if correct password 
    for(int i = 0; i < passVec.size(); i++) {
       if((passVec[i].compare(password) == 0) && (userVec[i].compare(user) == 0)) {
         return true;
       }
    }
  return false;
}

bool
IRCServer::userExists(const char * user) {
    for(int i = 0; i < userVec.size(); i++) {
       if((userVec[i].compare(user) == 0)) {
          return true;
       }
    }
    return false;
} 

void
IRCServer::addUser(int fd, const char * user, const char * password, const char * args)
{
    if(userExists(user)) {
       const char * msg =  "DENIED\r\n";
       write(fd, msg, strlen(msg));
    } else {
       passFile.open(PASSWORD_FILE, std::fstream::in | std::fstream::out | std::fstream::app);
       passFile << user << '\n' << password << "\n\n";
       passFile.close(); 
       passVec.push_back(password);
       userVec.push_back(user);
       const char * msg =  "OK\r\n";
       write(fd, msg, strlen(msg));
    }
  return;   
}

bool
IRCServer::roomExists(const char * args) {
    for(int i = 0; i < roomVec.size(); i++) {
       if((roomVec[i].compare(args) == 0)) {
          return true;
       }
    }
    return false;
} 

void
IRCServer::createRoom(int fd, const char * user, const char * password, const char * args)
{
   if(checkPassword(fd, user, password)) {
       if(!roomExists(args)){
            roomVec.push_back(args);
            numRooms++;
            const char * msg = "OK\r\n";
            write(fd, msg, strlen(msg));
       } else {
            const char * msg =  "DENIED\r\n";
          write(fd, msg, strlen(msg));
       }
    } else {
        const char * msg =  "ERROR (Wrong password)\r\n";
      write(fd, msg, strlen(msg));
    }
  return;
}

void
IRCServer::listRoom(int fd, const char * user, const char * password, const char * args)
{
  if(checkPassword(fd, user, password)) {
    for(int i = 0; i < roomVec.size(); i++) {
          const char * msg = roomVec[i].c_str();
        write(fd, msg, strlen(msg));
        const char * msg2 = "\r\n";
        write(fd, msg2, strlen(msg2));
      }
  } else {
        const char * msg =  "ERROR (Wrong password)\r\n";
      write(fd, msg, strlen(msg));
  }
}

void
IRCServer::enterRoom(int fd, const char * user, const char * password, const char * args)
{ 
  if(checkPassword(fd, user, password)) {
    if(!roomExists(args)){
            const char * msg = "ERROR (No room)\r\n";
            write(fd, msg, strlen(msg));
    } else { 
    int roomCount = 0; 
    for(int i = 0; i < roomVec.size(); i++) {
       if((roomVec[i].compare(args) == 0)) {
          break;
       }
       roomCount++;
    }
    h.insertItem3(fd, user,(void *)password, roomCount); 
    const char * msg =  "OK\r\n";
    write(fd, msg, strlen(msg));
   } 
  } else {
        const char * msg =  "ERROR (Wrong password)\r\n";
      write(fd, msg, strlen(msg));
  } 
}

void
IRCServer::leaveRoom(int fd, const char * user, const char * password, const char * args)
{ 
  if(checkPassword(fd, user, password)) {
    int roomCount = 0; 
    for(int i = 0; i < roomVec.size(); i++) {
       if((roomVec[i].compare(args) == 0)) {
          break;
       }
       roomCount++;
    }
    HashTableVoidIterator iterator(&h);
    const char * key5;
    void * gradev;
    if(!iterator.userInRoomExists(fd,user,roomCount)) {
       const char * msg2 =  "ERROR (User not in room)\r\n";
       write(fd, msg2, strlen(msg2));
    } else {
        h.removeElement2(user, roomCount);
        const char * msg2 =  "OK\r\n";
        write(fd, msg2, strlen(msg2));
    }
  } else {
    const char * msg =  "ERROR (Wrong password)\r\n";
  write(fd, msg, strlen(msg));
  }
}

void
IRCServer::sendMessage(int fd, const char * user, const char * password, const char * args)
{
   char pRoom[100];
   char message[1025];
   int nRead = sscanf(args,"%s %[^\n]", pRoom, message);
   if(checkPassword(fd, user, password)) {
    int roomCount = 0; 
    for(int i = 0; i < roomVec.size(); i++) {
       if((roomVec[i].compare(args) == 0)) {
          break;
       }
       roomCount++;
    }
    const char * key5;
    void * gradev;
    HashTableVoidIterator iterator(&h);
    if(!iterator.userInRoomExists(fd,user,roomCount)) {
       const char * msg2 =  "ERROR (user not in room)\r\n";
       write(fd, msg2, strlen(msg2));
    } else {
        h2.insertItem2(fd , message,(void *)user, roomCount); // for message and pass
        const char * msg2 =  "OK\r\n";
        write(fd, msg2, strlen(msg2));
    }
  } else {
      const char * msg =  "ERROR (Wrong password)\r\n";
      write(fd, msg, strlen(msg));
  } 
}

void 
IRCServer::getMessages(int fd, const char * user, const char * password, const char * args)
{
   int from;
   char pRoom[100];
   int nRead = sscanf(args,"%d %[^\n]", &from, pRoom);
   if(checkPassword(fd, user, password)) {
    int roomCount = 0; 
    for(int i = 0; i < roomVec.size(); i++) {
       if((roomVec[i].compare(args) == 0)) {
          break;
       }
       roomCount++;
    }
    const char * key5;
    void * gradev;
    HashTableVoidIterator iterator(&h);
    if(!iterator.userInRoomExists(fd,user,roomCount)) {
       const char * msg2 =  "ERROR (User not in room)\r\n";
       write(fd, msg2, strlen(msg2));
    } else {
        const char * msg;
        HashTableVoidIterator iterator2(&h2); 
        iterator2.next3(fd,msg, gradev, roomCount, from);
    }
  } else {
      const char * msg =  "ERROR (Wrong password)\r\n";
      write(fd, msg, strlen(msg));
  } 
}


void
IRCServer::getUsersInRoom(int fd, const char * user, const char * password, const char * args)
{
   if(checkPassword(fd, user, password)) {
    int roomCount = 0; // this is the number h[roomCount]
    roomFile.open(ROOM_FILE, std::fstream::in | std::fstream::out | std::fstream::app); 
    if (roomFile.is_open()) // check room
     {
        string line;
        while (getline(roomFile, line)) // separated by \n
        {
            string str13(args);
            if(line.compare(str13) == 0) { 
               break;
            }
            roomCount++; 
     }
    roomFile.close();
    } 
    HashTableVoidIterator iterator(&h); // users and pass table
    const char * msg;
    void * gradev;
    iterator.next2(fd,msg, gradev, roomCount);
    //iterator.next(msg, gradev);
  //write(fd, msg, strlen(msg));// print key which is user for h2
   }
 else {
        const char * msg =  "ERROR (Wrong password)\r\n";
      write(fd, msg, strlen(msg));
  } 
}

void
IRCServer::getAllUsers(int fd, const char * user, const char * password,const  char * args)
{
  if(checkPassword(fd, user, password)) {
    vector<string> new_(userVec);
    sort(new_.begin(), new_.end());
    for(int i = 0; i < new_.size(); i++) {
       const char * msg = new_[i].c_str();
       write(fd, msg, strlen(msg));
       const char * msg3 = "\r\n";
       write(fd, msg3, strlen(msg3));
    }
    const char * msg4 = "\r\n";
    write(fd, msg4, strlen(msg4));
  } else {
      const char * msg =  "ERROR (Wrong password)\r\n";
      write(fd, msg, strlen(msg));
  }
}

