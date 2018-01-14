#include <xinu.h>
#include <string.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  * xsh_hello - obtain and print the string
 *   *------------------------------------------------------------------------
 *    */
shellcmd xsh_hello(int nargs, char *args[]) {

        int32   retval;                 /* return value                 */
        uint32  now;                    /* current local time           */
        char    datestr[64];            /* printable date in ascii      */

        /* Output info for '--help' argument */

        if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
                printf("Usage: %s\n\n", args[0]);
                printf("Description:\n");
                printf("\tprint the string.\n");
                printf("Options (one per invocation):\n");
               	printf("\t--help\tdisplay this help and exit\n");
                return 0;
        }

        /* Check argument count */

        if (nargs > 2) {
                fprintf(stderr, "%s: too many arguments\n", args[0]);
                fprintf(stderr, "Try '%s --help' for more information\n",
                        args[0]);
                return 1;
        }
	if(nargs <2){
		fprintf(stderr, "%s: too few arguments\n", args[0]);
                fprintf(stderr, "Try '%s --help' for more information\n",
                        args[0]);
                return 1;

	}
        if (nargs == 2) {
               printf("Hello %s, Welcome to the world of Xinu!!",args[1]);
        }
}
