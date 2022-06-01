// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include<arpa/inet.h>
#include <stdlib.h>
#define MAX 80

struct packet //Incoming packet from client
{
	int a;
	int b;
	char operator[4];
};

struct result_packet //Outgoing packet to client
{
	int successOrFail;
	double resultOrError;
};

int main(int argv, char *afgv[]){ 

int socket_desc, c,read_size;
long unsigned int client_socket ;
struct sockaddr_in server, client;

// get a socket
socket_desc = socket(AF_INET, SOCK_STREAM,0); 
if (socket_desc==-1){ 
	 printf("Could not create socket.");
}
puts("Socket created");

//fill the fields 
server.sin_addr.s_addr = inet_addr("127.0.0.1");
server.sin_family =AF_INET; 
server.sin_port = htons( 8888 ); 


//bind the socket to the port
if(bind(socket_desc,(struct sockaddr *)&server, sizeof(server))<0){ 
	perror("Bind failed. Error");
	return 1;
}

//start listening for incoming connections
puts("Bind done.");
listen(socket_desc,3);

puts("Waiting for incoming connections...");
c = sizeof(struct sockaddr_in);
client_socket= accept(socket_desc,(struct sockaddr *)&client, (socklen_t*)&c);

if (client_socket == -1) {
      perror("Accept failed.");
      return 1;
    }
puts("Connection accepted.");

while(1){
	struct packet packet1;
	struct result_packet packet2;
	char final[MAX];
	char a[MAX];
	char b[MAX];
	double total;
	recv(client_socket, &packet1, sizeof(packet1),0); //Recieves packet from client
	if(strcmp(packet1.operator,"add")==0) //Determines and carries out operation if it's addition
	{
		total = (double)packet1.a + (double)packet1.b;
		snprintf(final,sizeof(final),"%f",total);
		packet2.successOrFail=1;
		packet2.resultOrError=total;
	}
	else if(strcmp(packet1.operator,"sub")==0) //Determines and carries out operation if it's subtraction
	{
		total = (double)packet1.a - (double)packet1.b;
		snprintf(final,sizeof(final),"%f",total);
		packet2.successOrFail=1;
		packet2.resultOrError=total;
	}
	else if(strcmp(packet1.operator,"div")==0) //Determines and carries out operation if it's division
	{
		total = (double)packet1.a / (double)packet1.b;
		if(packet1.b==0) // Means the operation was a failure
		{
			packet2.successOrFail=0;
			packet2.resultOrError=1; //error code representing "divide by zero"
		}
		else
		{
			packet2.successOrFail=1;
			packet2.resultOrError=total;
			snprintf(final,sizeof(final),"%f",total);
		}
		
	}
	else if(strcmp(packet1.operator,"mul")==0) //Determines and carries out operation if it's multiplication
	{
		total = (double)packet1.a * (double)packet1.b;
		snprintf(final,sizeof(final),"%f",total);
		packet2.successOrFail=1;
		packet2.resultOrError=total;
	}
	
	char server_reply[MAX];
	
	if(packet2.successOrFail==0) //If operation failed:
	{
		if(packet2.resultOrError==1) //Determines error code and reason
			strcpy(server_reply,"Fail - (Return code: 0). Can't divide by zero - (Error code: 1)");
	}
	else
	{
		snprintf(server_reply,sizeof(server_reply),"Success - (Return code: 1). Result is %f",packet2.resultOrError);
	}
	
	puts("\nSending message to client:");
	puts(server_reply); //Sends result_packet converted into a ready string called "server_reply"
	write(client_socket,server_reply,strlen(server_reply));
}
 


if(read_size==0){ 
  puts("Client Disconnected.");
  fflush(stdout);
}
else if(read_size==-1){
 perror("Recv failed.");
}
 

return 0;
}
