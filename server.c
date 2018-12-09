#include <stdio.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#define MAX 256 
#define PORT 12421
#define SA struct sockaddr 

//basic boolean definition
typedef enum { false, true } bool;

typedef struct _account{
	char name[256];
	double balance;
	bool flag;
}account;


//want to remove the command and the whitespace
char* trimcommand(char * input, int a){
	
	int i=0;
	int k=1;
	//remove first a letters
	for(i=0; i<a;i++){
		for(k=1;k<strlen(input);k++){
			input[k-1]= input[k];
			//printf("%s",input);
			}
		input[strlen(input)-1] = '\0';
	}
	
	//printf("first letters removed: %s\n",input);
	//remove leading whitespace
	while(input[0]==' '){
		for(k=1;k<strlen(input);k++){
			input[k-1] = input[k];
			//printf("%s",input);
		}
	
		input[strlen(input)-1] = '\0';
	}
	//printf("leading whitespace removed: %s\n",input);
	//remove trailing whitespace
	while(input[strlen(input)-1]==' '||input[strlen(input)-1]=='\n'){
		input[strlen(input)-1]= '\0';
		//printf("removed 1",input);
	}
	
	printf("%s\n",input);
	return input;
}
int isNumeric(char* data){
	int i = 0;
	int isNum = 1; //set to 0 if not numeric
	int period = 0; //shouldnt be more than one period
	int length = (int)(strlen(data));
	for(i=0; i<length;i++){
		char c = data[i];
		if(c < '0' || c > '9'){
			if(c == '.')
				period++;
			else
				isNum = 0;
			if(period > 1)
				isNum = 0;
		}
	}
	return isNum;
}
// Function designed for chat between client and server. 
void func(int sockfd) 
{ 
    char buff[MAX]; 
    int n; 
    // infinite loop for chat 
    for (;;) { 
        bzero(buff, MAX); 
  
        // read the message from client and copy it in buffer 
        read(sockfd, buff, sizeof(buff)); 
        
        //My shitty code
        
        //example account created
		  account * example = malloc(sizeof(account));
        char * inputcopy = malloc(strlen(buff)*sizeof(char)*2);
        strcpy(inputcopy,buff);
        example->balance = 100;
        example->flag = false;
        
        
     //CREATE ACCOUNT   
        if(strncmp("create",buff,6)==0){
        printf("create\n");
     }
     
     //SERVICE
        else if(strncmp("serve",buff,5)==0){
        	strcpy(inputcopy,trimcommand(inputcopy,5));
        	if(example->flag)
        		printf("Error: Cannot service an account that is already in service.\n");
        	else{
        		example->flag = true;
       		 printf("Account with name: %s :is now IN service\n",inputcopy);
       	}
     }
     
     //DEPOSIT
        else if(strncmp("deposit",buff,7)==0){
        		if(example->flag){
        			strcpy(inputcopy,trimcommand(inputcopy,7));
        			if(isNumeric(inputcopy)){
       		 		example->balance = example->balance + atof(inputcopy);
        				printf("Balance after deposit: %f\n",example->balance);
 				}
 					else 
 						printf("Input is not Numeric.\n");
 			}
 				else 
 					printf("Cannot Deposit, account is not in service.\n");
     }
     
     //WITHDRAW
        else if(strncmp("withdraw",buff,8)==0){
        		if(example->flag){
        			strcpy(inputcopy,trimcommand(inputcopy,8));
        			if(isNumeric(inputcopy)){
        				
        				if(example->balance<atof(inputcopy))
        					printf("Withdraw request is invalid. ");
        				else
        					example->balance = example->balance - atof(inputcopy);
        		
        				printf("Balance after withdraw: %f\n",example->balance);
        		}
      		  else 
 						printf("Input is not Numeric.\n");
 			}
 				else
 					printf("Cannot Withdraw, account is not in service.\n");
     }	
     
     //QUERY
        else if(strncmp("query",buff,5)==0){
        	if(example->flag){
       		 printf("Current account Balance is: %f\n",example->balance);
       	}
       	else 
       		printf("Cannot perform Query, account is not in service.\n");
     }
     
     //END SERVICE
        else if(strncmp("end",buff,3)==0){
         strcpy(inputcopy,trimcommand(inputcopy,3));
       if(!example->flag)
        		printf("Error: The account service session has already been ended.\n");
        	else{
        		example->flag = false;
       		 printf("Account with name: %s :is now OUT OF service\n",inputcopy);
       	}
     }
     
     //QUIT
        else if(strncmp("quit",buff,4)==0){
        printf("quit\n");
     }
        else
        printf("error: %s does not contain a valid command\n",buff);
        
        
        
        
        
        
        
        
        //end shit code
        
        // print buffer which contains the client contents 
        printf("From client: %s\t To client : ", buff); 
        bzero(buff, MAX); 
        n = 0; 
        // copy server message in the buffer 
        while ((buff[n++] = getchar()) != '\n') 
            ; 
  
        // and send that buffer to client 
        write(sockfd, buff, sizeof(buff)); 
  
        // if msg contains "Exit" then server exit and chat ended. 
        if (strncmp("exit", buff, 4) == 0) { 
            printf("Server Exit...\n"); 
            break; 
        } 
    } 
} 
  
// Driver function 
int main() 
{ 
		/*
		//example account created
		  account * example = malloc(sizeof(account));
        char * inputcopy = malloc(strlen(buff)*sizeof(char)*2);
        strcpy(inputcopy,buff);
        example->balance = 100;
        */
        
    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and verification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 
  
    // Now server is ready to listen and verification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    len = sizeof(cli); 
  
    // Accept the data packet from client and verification 
    connfd = accept(sockfd, (SA*)&cli, &len); 
    if (connfd < 0) { 
        printf("server acccept failed...\n"); 
        exit(0); 
    } 
    else
        printf("server acccept the client...\n"); 
  
    // Function for chatting between client and server 
    func(connfd); 
  
    // After chatting close the socket 
    close(sockfd); 
} 
