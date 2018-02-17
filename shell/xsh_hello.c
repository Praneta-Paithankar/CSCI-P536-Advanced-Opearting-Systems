#include <xinu.h>
#include <string.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  * xsh_hello - obtain and print the string
 *   *------------------------------------------------------------------------
 *    */
shellcmd xsh_hello(int nargs, char *args[]) {

        /* Output info for '--help' argument */

        if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
                printf("Usage: %s\n\n", args[0]);
                printf("Description:\n");
                printf("\tDisplay the message.\n");
                printf("Options (one per invocation):\n");
               	printf("\t--help\tdisplay this help and exit\n");
                return 0;
        }

        /* Check argument count */
	/*Number of arguments are greater than 2,print the error*/
        if (nargs > 2) {
                fprintf(stderr, "%s: too many arguments\n", args[0]);
                fprintf(stderr, "Try '%s --help' for more information\n",
                        args[0]);
                return 1;
        }
	/*Number of arguments are less than 2.Print the error */
	if(nargs <2){
		fprintf(stderr, "%s: too few arguments\n", args[0]);
                fprintf(stderr, "Try '%s --help' for more information\n",
                        args[0]);
                return 1;

	}
	/*Print the message.*/
        if (nargs == 2) {
               printf("Hello %s, Welcome to the world of Xinu!!\n",args[1]);
        }
	return 0;
}
