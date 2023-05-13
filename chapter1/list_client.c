#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

/*Port to send a status packet to on the C2 server*/
#define PORT 12345
/*C2 Host*/
#define HOST "localhost"
/*Debug mode*/
#define DEBUG 1
#define PORT_MAX_LENGTH 6

int sendSocketReal(char *data, size_t data_len, char *hostname, int port) {
	int sock;
	struct addrinfo hints;
	struct addrinfo *result;
	
	memset(&hints, 0, sizeof(hints));
	
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	
	char port_str[PORT_MAX_LENGTH];
	sprintf(port_str, "%d", port);

	if (getaddrinfo(hostname, port_str, &hints, &result)) {
		perror("getaddrinfo");
	}

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		
		return 1;
	}

	if ((connect(sock, (struct sockaddr *)result->ai_addr, result->ai_addrlen)) < 0) {
		perror("connect");

		return 1;
	}
	
	send(sock, data, data_len, 0);

	close(sock);
	
	return 0;
}

int main(int argc, char **argv) {
	sendSocketReal("client connected", strlen("hello"), "localhost", PORT);

	return 0;
}
