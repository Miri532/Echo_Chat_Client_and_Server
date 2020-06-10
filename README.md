# Echo_Chat_Client_and_Server
using sockets and select function - runs on Linux.

build with gcc:
gcc EchoServer_chat.c -o server  
gcc client_chat.c -o client  

run the server:  
./server <port_num (optional)>  
the default port number for the server to listen on is 5050.  

run the client(s):  
./client <server_ip> <server_port> <client's_username  (max 20 chars)>   

if all 3 parameters are not passed, use the default ones:  
server_ip: 127.0.0.1  
server_port: 5050  
client's_username: anonymous  

send messages between clients.  

send 'q' to disconnect from the server.  




