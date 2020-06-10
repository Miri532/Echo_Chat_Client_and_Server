#include <stdio.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>

#define MAX_BUFF 100 
#define PORT 5050
#define MAX_LISTEN_QUEUE_LEN 5
#define MAX_CLIENTS 30
#define USER_NAME_LEN 20


typedef struct client{
	int sd;
	char user_name[USER_NAME_LEN];
} client_t;


int main(int argc, char** argv) 
{ 
	int master = 0, conn = 0, len = 0, i = 0; 
	struct sockaddr_in server_addr = {0};
	struct sockaddr_in client_addr = {0};
	char buff[MAX_BUFF] = {0};  // buffer for receiving msgs
	char send_buff[MAX_BUFF] = {0}; // buffer for sending msgs
	uint port = 0;
	int ready = 0;
	fd_set read_fds;
	client_t client_sockets[MAX_CLIENTS] = {0};
	int max_sd = 0;

	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	if (argc != 2){
		port = PORT;
	}
	
	else {
		port = atoi(argv[1]);
	}
	
	// Creating master listener Socket:
	if ((master = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("***Error in creating the socket*** \n"); 
		return -1; 
	} 

	// Creating Adress:
	server_addr.sin_family = AF_INET; 
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
	server_addr.sin_port = htons(port); 

	// Binding socket to IP:
	if ((bind(master, (struct sockaddr*)&server_addr, sizeof(server_addr))) != 0) { 
		printf("***Error in binding socket to address***\n"); 
		return -1; 
	} 

	// Listening for connection:
	if ((listen(master, MAX_LISTEN_QUEUE_LEN)) != 0) { 
		printf("***Error in listening for connection***\n"); 
		return -1; 
	} 

	len = sizeof(client_addr); 

	while(1){
		// building read set:
		FD_ZERO(&read_fds);
		FD_SET(master, &read_fds);
		max_sd = master;
				
		for (i = 0; i < MAX_CLIENTS; i++){
			// set connected clients
			if (client_sockets[i].sd > 0){
				FD_SET(client_sockets[i].sd, &read_fds);
			}			
		}
		
		if (conn > max_sd){
			max_sd = conn;
		}

		ready = select(max_sd + 1, &read_fds, NULL, NULL, &timeout);	

		// if master socket is ready, means someone wants to connect
		if (FD_ISSET(master, &read_fds))
		{
			// Accept the connection and create a new socket
			conn = accept(master, (struct sockaddr*)&client_addr, &len); 
			if (conn < 0) { 
				printf("***Error in accepting connection***\n"); 
				continue; 
			} 

			//add the new socket to array of sockets  
            for (i = 0; i < MAX_CLIENTS; i++) {   
                if(client_sockets[i].sd == 0) {   
                    client_sockets[i].sd = conn;  
                    break;   
                }   
            } 

            // no room for more clients
			if (i == MAX_CLIENTS){
				printf("exceeded max amount of clients\n");
			}					            
		}

		// read from all clients connections
		for(int i = 0; i < MAX_CLIENTS; i++){
			if (FD_ISSET(client_sockets[i].sd, &read_fds)){
				int num_read = read(client_sockets[i].sd, buff, MAX_BUFF); 
				if (num_read < 0)
				{
					printf("***Error in reading from client socket***\n"); 
				}
				
				// handle 3 different scenarios: 
				// 1 - we got the user name 
				if (client_sockets[i].user_name[0]== 0){
					strncpy(client_sockets[i].user_name, buff, USER_NAME_LEN);
					// create the msg format: example - maya: hi here is my msg
					strncat(client_sockets[i].user_name, ": ", 2);
					memset(buff, 0 , MAX_BUFF);	
					break;				
				}
				// 2 - we got the q
				else if (strncmp(buff, "q", 1) == 0) {
					printf("got q from client number %d , closing clients socket\n", i);
					close(client_sockets[i].sd);
					memset(&client_sockets[i], 0, sizeof(client_sockets[i]));
					memset(buff, 0 , MAX_BUFF);
					break;
				}
				// 3 - we got the msg - send to all clients				
				else {
					strncpy(send_buff, client_sockets[i].user_name, strlen(client_sockets[i].user_name));
					strcat(send_buff, buff);
					memset(buff, 0 , MAX_BUFF);
					// sent to all the clients
					for(int i = 0; i < MAX_CLIENTS; i++){
						if(client_sockets[i].sd != 0 && strlen(send_buff) != 0){
							send(client_sockets[i].sd, send_buff, MAX_BUFF, 0); 
						}						
					}					 				
					memset(send_buff, 0 , MAX_BUFF);	
				}	
			}
		} // end of read from all clients connections

	}  // end of while(1)
} 
