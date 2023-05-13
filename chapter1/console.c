#include <stdio.h>
#include <string.h>

#define BANNER "######################\n"\
	"#        BOTNET       #\n" \
	"#                     #\n" \
	"#                     #\n" \
	"#######################\n"

#define TRUE 1
#define COMMAND_SIZE 100

int main(int argc, char **argv) {
	/*Prints the banner*/
	printf(BANNER);

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
			printf("Calling list_hosts()... (This command isn't implemented yet.)\n");
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
