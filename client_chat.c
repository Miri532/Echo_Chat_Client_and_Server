#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <string.h> 
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


#define PORT 5050
#define IP "127.0.0.1"
#define MAX_BUFF 100 
#define USER_NAME_LEN 20
#define IP_LEN 15



int main(int argc, char* argv[]) 
{ 
	int sock = 0; 
	struct sockaddr_in server_addr; 
	//char *hello = "Hello Friend"; 
	char buff[MAX_BUFF];

	// Creating Socket:
	// int socket(int domain, int type, int protocol);
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("***Error in creating the socket*** \n"); 
		return -1; 
	} 

	// default values
	uint port = PORT;
	char ip[IP_LEN] = IP;
	char user_name[USER_NAME_LEN] = "anonymous";


	// usage: ip port user_name
	if (argc != 4){
		// use the default values
	}
	
	else {
		memset(ip, 0, IP_LEN);
		strncpy(ip, argv[1], IP_LEN);
		port = atoi(argv[2]);
		strncpy(user_name, argv[3], USER_NAME_LEN);
	}
	

	// Creating Adress:
	server_addr.sin_family = AF_INET; 
	server_addr.sin_port = htons(port); 
	
	// convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_aton(ip, &server_addr.sin_addr) <= 0)
	{ 
		printf("***Invalid Address***\n"); 
		return -1; 
	} 

	// Connect the socket to the address
	if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
	{ 
		printf("**Connection Failed**\n"); 
		return -1; 
	}

	// after successfull connection - send to the server user name
	send(sock , user_name , USER_NAME_LEN , 0); 

	fd_set read_fds;
	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	int max_sd = 0;

	while(1){
		// building read set:
		FD_ZERO(&read_fds);
		FD_SET(sock, &read_fds);
		FD_SET(STDIN_FILENO, &read_fds);

		max_sd = sock;

		int ready = select(max_sd + 1, &read_fds, NULL, NULL, &timeout);	

		// user typed msg we need to send to server
		if (FD_ISSET(STDIN_FILENO, &read_fds))
		{
			fgets (buff, MAX_BUFF, stdin);
			if (strncmp(buff, "q", 1) == 0){
				send(sock , buff , MAX_BUFF , 0); 
				break;
			}

			send(sock , buff , MAX_BUFF , 0); 
			memset(buff, 0 , MAX_BUFF);
		}

		// server sent us a msg - print it
		if (FD_ISSET(sock, &read_fds)){
			int res = read(sock, buff, MAX_BUFF); 
			if (res < 0){
				printf("read error\n");
			}
			printf("%s\n", buff); 
			memset(buff, 0 , MAX_BUFF);
		}
	}
	
	printf("closing client socket\n");
	close(sock);
	return 0; 
} 
