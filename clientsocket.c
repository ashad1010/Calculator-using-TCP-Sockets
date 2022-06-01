#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>  
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#define MAX 80

char operation[4];
int sizeOfFirstOperand;
int sizeOfSecondOperand;
int firstNum;
int secondNum;

const char* getFirstOperand(char *str) //Parses the requested operation string until the operator is found, and returns a string of the first operand
{
    char * firstInteger;
    
    for (int i = 0; str[i]; i++)
    {
    	if (str[i] == '+') //Once the operator is hit, the length of the first operand can be determined and the operator is stored to be sent to server later. We can now break the loop.
        {
        	strcpy(operation,"add");
        	sizeOfFirstOperand = i;
        	break;
        }
        if (str[i] == '-')
        {
        	strcpy(operation,"sub");
        	sizeOfFirstOperand = i;
        	break;
        }
        if (str[i] == '/')
        {
        	strcpy(operation,"div");
        	sizeOfFirstOperand = i;
        	break;
        }
        if (str[i] == '*')
        {
        	strcpy(operation,"mul");
        	sizeOfFirstOperand = i;
        	break;
        }
    }
    
    firstInteger = malloc(sizeof(char)*sizeOfFirstOperand);
    strncpy(firstInteger,&str[0],sizeOfFirstOperand);
    
    return firstInteger;
}

const char* getSecondOperand(char *str) //Based on getFirstOperand, we can determine the second operand using this information and return it as a string
{
	char * secondInteger;
	sizeOfSecondOperand = strlen(str)-(sizeOfFirstOperand+1);
	secondInteger = malloc(sizeof(char)*sizeOfSecondOperand);
    	strncpy(secondInteger,&str[sizeOfFirstOperand+1],sizeOfSecondOperand);
    	return secondInteger;
}

struct packet //Packet from client to server
{
	int a;
	int b;
	char operator[4];
};

struct result_packet //Packet from server to client
{
	int successOrFail;
	double resultOrError;
};


int main(int argc, char *argv[])
{
    //Initializing sockets...
    
    int sock;

    struct sockaddr_in server;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) 
        printf("ERROR opening socket."); 
    puts("Socket created.");	

    server.sin_addr.s_addr=inet_addr("127.0.0.1");   
    server.sin_family = AF_INET;
    server.sin_port = htons(8888);

    if (connect(sock,(struct sockaddr *)&server,sizeof(server)) < 0){ 
        printf("ERROR connecting.");
	return 1;
}
    puts("Connected.");

struct packet packet1;
struct result_packet packet2;

while(1){
    
    char message[MAX], server_reply[MAX];
    printf("\nPlease enter the operation WITHOUT ANY SPACES: ");
    scanf("%s",message);
    puts(message);
    puts("");
    char firstInteger[strlen(getFirstOperand(message))];
    strcpy(firstInteger,getFirstOperand(message));
    char secondInteger[strlen(message)-strlen(firstInteger)-1];
    strcpy(secondInteger,getSecondOperand(message));
    firstNum = atoi(firstInteger); // converts string of first operand to an int
    secondNum = atoi(secondInteger); // converts string of second operand to an int
    
    //Sets packet details after recieving input
    packet1.a=firstNum;
    packet1.b=secondNum;
    strcpy(packet1.operator,operation);
    
    //Sends packet over
    if (send(sock,&packet1,sizeof(packet1),0)< 0){ 
         printf("ERROR writing to socket.");
	return 1;
	}
    
   //Recives a result packet turned into a string called "server_reply"
   memset(server_reply,0,strlen(server_reply)); //Clears server_reply string from previous forms
   if (recv(sock,server_reply,MAX,0)<0){
	puts("Recv failed.");
	break;
	}

puts("Server Reply:");
puts(server_reply);

}
close(sock);
    return 0;
}
