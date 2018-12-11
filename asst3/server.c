#include "banking.h"

account * global= NULL;
client * clientlist= NULL;
bool terminate = false;

//want to remove the command and the whitespace
bool nameAlreadyExists(char * input){
	if(global==NULL){
		return false;
	}
	account * pointer = malloc(sizeof(account));
	pointer = global;
	while(pointer!=NULL){
		if(strcmp(input,pointer->name)==0)
			return true;

		pointer = pointer->next;
		}
	return false;
}
void createAccount(char * input)
{
	account * newAccount = malloc(sizeof(account));
	account * pointer = malloc(sizeof(account));
	newAccount->name = input;
	newAccount->balance = 0;
	newAccount->flag = false;
	newAccount->next = NULL;
	if(global==NULL){
		global= newAccount;
	}
	else
	{
		//Attach account at end of GLOBAL LL
		pointer = global;
		while(pointer->next!=NULL){
			pointer=pointer->next;
			printf("%s->%s\n",pointer->name,pointer->flag);
		}

		pointer->next =  newAccount;

	}
	//free(pointer);
	return;
}
void createClient(pthread_t new, int clientfd)
{
	client * pointer = malloc(sizeof(client));
	client newclient = malloc( sizeof(client));
	newclient->tid = new;
	newclient->sock = clientfd;
	newclient->next = NULL;
	if(clientlist==NULL){
		clientlist= newAccount;
	}
	else
	{
		//Attach account at end of clientlist LL
		pointer = clientlist;
		while(pointer->next!=NULL){
			pointer=pointer->next;
		}
		pointer->next =  newclient;
	}
	//free(pointer);
	return;
}


char* trimcommand(char * input, int a){
	int i=0;
	int k=1;
	//remove first a letters
	for(i=0; i<a;i++){
		for(k=1;k<strlen(input);k++){
			input[k-1]= input[k];
			}
		input[strlen(input)-1] = '\0';
	}
	//remove leading whitespace
	while(input[0]==' '){
		for(k=1;k<strlen(input);k++){
			input[k-1] = input[k];
		}
		input[strlen(input)-1] = '\0';
	}
	//remove trailing whitespace
	while(input[strlen(input)-1]==' '||input[strlen(input)-1]=='\n'){
		input[strlen(input)-1]= '\0';
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

void metadata()
{
   	//sem_wait(&mutex);
   	printf("Beginning Metadata Dump\n");
    account* ptr =(account*) malloc(sizeof(account));
	ptr = global;
	int count =0;
	while(ptr != NULL){
		count++;
        	char* accountname= ptr->name;
        	double accountbalance= ptr->balance;
        	if(ptr->flag == true){
          		 printf("%s\t%s\tIN SERVICE\n", accountname,accountbalance);
        	}else{
            		printf("%s\t%f\n", accountname,accountbalance);

        	}
		ptr=ptr->next;
    }
    //sem_post(&mutex);
    return;
}
void signal_handler(int signum)
{
    if(signum== SIGINT){
		fprintf(stderr, "got control C\n",signum);
        terminate = true;
		ender();
    }/*else if (signum == SIGALARM) {
        print = true;
    }*/
	else{
        fprintf(stderr, "Recieved a bad signum, got: %d closing anyways\n",signum);
        terminate = true;
    }
}
void ender()
{
	client * pointer = malloc(sizeof(client));
	pointer = clientlist;
	while(pointer!=NULL){
		pthread_t id = pointer->tid;
		int fd = pointer->sock;
		close(fd);
		pthread_cancel(id);
		// maybe join instead
		// pthread_join(id,NULL);
	}

}


// Function designed for chat between client and server.
void banking(void * args)
{
	// type cast args to int
    int* client_args = (int*) args;
	int sockfd = *client_args;

	//Initialize vars
	bool isServiceSession = false;
	char buff[MAX];
	int n;
	// infinite loop for chat
	while (terminate == false) {
		bzero(buff, MAX);

		// read the message from client and copy it in buffer
		read(sockfd, buff, sizeof(buff));

		//My shitty code

		//example account created set to flagged account if not last node
		account * example;
		example = global;
		bool flagFound = false;

		while(example!=NULL){
			if(example->flag){
				flagFound=true;
				break;
			}
			if(example->next==NULL)
				break;
			example= example->next;
		}
		//printf("after example\n");
		char * inputcopy = malloc(strlen(buff)*sizeof(char)*2);
		strcpy(inputcopy,buff);


		//CREATE ACCOUNT
		if(strncmp("create",buff,6)==0){
			strcpy(inputcopy,trimcommand(inputcopy,6));
			if(isServiceSession)
				sprintf(buff,"Cannot create new account during a service session.\n");
			else{
				if(nameAlreadyExists(inputcopy))
					sprintf(buff,"The account name you seek to create already exists.\n");
				else{
					if(strlen(inputcopy)>255)
						sprintf(buff,"The account name cannot exceed 255 characters.\n");
					else {
						//CREATE NEW ACCOUNT
						createAccount(inputcopy);
						sprintf(buff,"Account: --%s-- has been successfully created.\n",inputcopy);
					}
				}
			}
			//metadata();
		}

		//SERVICE
		else if(strncmp("serve",buff,5)==0){
			strcpy(inputcopy,trimcommand(inputcopy,5));

			//FIND ACCOUNT TO SERVICE BY NAME
			bool nameMatch = false;
			example = global;
			while(example!=NULL){
				if(strcmp(inputcopy,example->name)==0){
					nameMatch = true;
					break;
				}
				example = example->next;
			}
			//if Name NOT FOUND
			if(!nameMatch)
				sprintf(buff,"Account name to be serviced cannot be found.\n");
			else{
				if(example->flag||isServiceSession)		//Might need to differentiate between single service on client, and no concurrent serve on same account across multiple clients.
					sprintf(buff,"Error: Cannot service. An account is already in a service session.\n");
				else{
					example->flag = true;
					isServiceSession = true;
					sprintf(buff,"Account with name: --%s-- :is now IN service\n",inputcopy);
				}
			}
		}

		//DEPOSIT
		else if(strncmp("deposit",buff,7)==0){
			if(example->flag){
				strcpy(inputcopy,trimcommand(inputcopy,7));
				if(isNumeric(inputcopy)){
					example->balance = example->balance + atof(inputcopy);
					sprintf(buff,"Balance of account --%s-- after deposit: %f\n",example->name,example->balance);
				}
				else
					sprintf(buff,"Input is not Numeric.\n");
			}
			else
				sprintf(buff,"Cannot Deposit, account is not in service.\n");
		}

		//WITHDRAW
		else if(strncmp("withdraw",buff,8)==0){
			if(example->flag){
				strcpy(inputcopy,trimcommand(inputcopy,8));
				if(isNumeric(inputcopy)){
					if(example->balance<atof(inputcopy))
						sprintf(buff,"Withdraw request is invalid.\n");
					else{
 						example->balance = example->balance - atof(inputcopy);
						sprintf(buff,"Balance of account --%s-- after withdraw: %f\n",example->name,example->balance);
					}
        		}
				else
					sprintf(buff,"Input is not Numeric.\n");
			}
				else
					sprintf(buff,"Cannot Withdraw, account is not in service.\n");
		}

		//QUERY
		else if(strncmp("query",buff,5)==0){
			if(example->flag){
				sprintf(buff,"Current account balance is: %f\n",example->balance);
			}
			else
				sprintf(buff,"Cannot perform Query, account is not in service.\n");
		}

		//END SERVICE
		else if(strncmp("end",buff,3)==0){
			strcpy(inputcopy,trimcommand(inputcopy,3));
			if(!example->flag)
				sprintf(buff,"Error: The account service session has already been ended.\n");
			else{
				example->flag = false;
				isServiceSession = false;
				sprintf(buff,"Account with name: --%s-- :is now OUT OF service\n",inputcopy);
			}
		}

		//QUIT
		else if(strncmp("quit",buff,4)==0){
			sprintf(buff,"quit\n");
		}
		else{
			bzero(buff, MAX);
			sprintf(buff,"error: %s does not contain a valid command\n");
			write(sockfd, buff, sizeof(buff));
		}
	}
	bzero(buff, MAX);
	sprintf(buff,"Server is terminating program, DISCONNECTING\n");
	write(sockfd, buff, sizeof(buff));
	return;
}

// Driver function
int main(int argc, char const *argv[])
{
	//Initialize vars
	global = malloc(sizeof(account));
	global = NULL;
	clientlist = malloc(sizeof(client));
	clientlist = NULL;
	terminate = false;
	if( argc != 2){
		fprintf(stderr, "%s\n", "wrong number of input args");
	}
	// for control C
	//signal(SIGINT,signal_handler);

	//Initialize sephamore
    //sem_init(&pmutex, 0, 1);

	int p = atoi(argv[1]);
	PORT = &p;
	printf("port is: %d ", *PORT);

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
	servaddr.sin_port = htons(*PORT);

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

	// Accepts clients and makes threads
	while(terminate == false){
		clientfd = accept(sockfd, (SA*)&cli, &len);
		if (clientfd < 0) {
			printf("server acccept failed...\n");
			continue;
		}
		else{
			printf("server acccept the client...\n");
		}
		// pthread prep
		pthread_t new;
		// idk if attr is necessary
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		int * thread_args;
		thread_args=&clientfd;
		// idk if attr is necessary
		if( pthread_create( &new , &attr ,  (void*)&banking , (void*) thread_args ) < 0)
		{
			perror("could not create send thread");
			return -1;
		}
		createClient(new, clientfd);
	}
	ender();
	return 0;
}
