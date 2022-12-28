// Kate Radford
// sh.c

char cmdLine[64], *head[1024], *tail[1024];

#include "ucode.c"       /* contains utility functions */

// Algorithm base from pg 365 of Design and Implementation
// also KC WANG's base code found in sdimage
int main(int argc, char* argv[]) {
	int stdin, stdout, stderr, i, j, pid, cid, me, status;
	
	printf("-------------- Kate's SH ----------------\n");
	printf("Enter ? for help menu\n");

	me = getpid();
	while (1) {

		// get a command line; e.g. cmdLine = cmd | cam2 | .... | cmdn &
		printf("SH %d (input command): ", getpid());
		gets(cmdLine);
		if (cmdLine[0] == 0)
			continue;
		//token(cmdLine);   /* break up cmdLine into name[0], name[1],.... */

		if (strcmp(cmdLine, "?") == 0 || strcmp(cmdLine, "help") == 0) {
			menu(); continue;
		}
		
		if (strcmp(cmdLine, "logout") == 0) {
			printf("logging out...\ngoodbye %d\n", getpid());
			exit(0);
		}

		if (strcmp(cmdLine, "exit") == 0) {
			exit(0);
		}

		/***** fork a child to execute the binary command ******/
		// sh forks a child proc to execute the command line and waits for child to terminate
		pid = fork();				// fork a child sh process
		if (pid) { // parent sh					
			printf("parent sh waits for child to die\n");
			pid = wait(&status); // wait for process state change
		}
		else { // child sh
			printf("child task %d\n", getpid());
			do_pipe(cmdLine, 0);
		}
	}
}

int menu()
{
	printf("##################### KATES's HELP MENU #####################\n");
	printf("#  ls       cd     pwd    cat   more    cp     mv   >   >>  #\n");
	printf("#  mkdir    rmdir  creat  rm    chmod   chown  lpr  <   |   #\n");
	printf("#############################################################\n");
}

int do_pipe(char* cmdLine, int* pd) {

	int hasPipe, lpd[2], verify;

	printf("DO_PIPE: cmdLine=%s\n", cmdLine);

	// NOTES:
	// the syscall int r = pipe(int pd[]);
	// returns two fd's in pd[2] where pd[0]=READ, pd[1]=WRITE
	// the syscall dup2(oldfd, newfd) copies oldfd to newfd, closing newfd first if necessary
	// stdin's file descriptor (fd) is 0, stdout is 1
	
	// if has a pipe passed in, as WRITER on pipe pd:
	if (pd) {
		printf("DO_PIPE: pipe writer passed in\n");
		close(pd[0]);		// close pipe READ end
		dup2(pd[1], 1);		// redirect stdout to pipe WRITE end
		//close(pd[1]);		// close pipe WRITE end
	}
	
	// divide cmdLine into head, tail by rightmost pipe symbol
	hasPipe = scan(cmdLine, head, tail);
	
	if (hasPipe) {
		// create a pipe lpd
		// pipe() creates a pair of file descriptors, pointing to a pipe inode, and places them in the 
		// array pointed to by filedes. filedes[0] is for reading, filedes[1] is for writing.
		verify = pipe(lpd);
		if (verify != 0) {
			printf("DO_PIPE: failed to create pipe\n");
			exit(1);
		}

		// after creating pipe, the proc forks a child proc to share the pipe (both parent/child have same pd[0]
		// and pd[1]. HOWEVER, on the same pipe each proc must be EITHER a reader or writer.
		pid = fork();
		if (pid) { // parent
			// as READER on lpd
			// must close pd[1] & redirect stdin (fd=0) to pd[0] so stdin is connected to the read end of pipe.
			close(lpd[1]);
			dup2(lpd[0], 0);	// redirect stdin to pipe READ end
			//close(lpd[0]);
			do_command(tail);
		}
		else {
			do_pipe(head, lpd);
		}
	}
	else
		do_command(cmdLine);
}

int do_command(char* cmdLine) {

	int nk, fd = 0, i = 0, verify;
	char line[1024], *head, * name[16];

	printf("DO_COMMAND: cmdLine=%s\n", cmdLine);

	strcpy(line, cmdLine);
	nk = eatpath(line, name);

	// scan cmdLine for I/O redirection symbols
	for (i = 0; i < nk; i++) {

		// do I/O redirections
		if (strcmp(name[i], ">") == 0) {
			printf("DO_COMMAND: > detected, opening %s\n", name[i+1]);
			// redirect the stdout of cmd to a file
			close(1);
			// O_CREAT: If pathname does not exist, create it as a regular file.
			fd = open(name[i + 1], O_WRONLY | O_CREAT);
		}

		if (strcmp(name[i], ">>") == 0) {
			printf("DO_COMMAND: >> detected, opening %s\n", name[i + 1]);
			// append stdout to a file
			close(1);
			fd = open(name[i + 1], O_WRONLY | O_CREAT | O_APPEND);
		}

		if (strcmp(name[i], "<") == 0) {
			printf("DO_COMMAND: < detected, opening %s\n", name[i + 1]);
			// redirects the contents of the file to the cmd
			close(0);
			fd = open(name[i + 1], O_RDONLY);
		}
	}

	if (fd < 0) {
		printf("DO_COMMAND: file failed to open\n");
		exit(2);
	}
	
	// head = cmdLine BEFORE redirections
	// strcpy(head, cmdLine);
	strcpy(head, name[0]);
	int j;
	for (j = 1; j < nk; j++) {
		strcat(head, " ");
		strcat(head, name[j]);
	}

	// exec(head);
	printf("DO_COMMAND: exec(head), head=%s\n", head);
	verify = exec(head);
	if (verify < 0) {
		printf("DO_COMMAND: exec failed\n");
		exit(1);
	}
}

// divide cmdLind into head and tail by rightmost | symbol
int scan(char* cmdLine, char* head, char* tail) {

	char temp[128];

	// cmdLine = cmd1 | cmd2 | ... | cmdn-1 | cmdn
	//           | <-------- head --------> | tail |
	// cmdLine = cmd1 ==> head = cmd1, tail = null
	printf("scanning for pipes in [%s]...\n", cmdLine);
	int len = strlen(cmdLine), i = 0;
	for (i = len - 1; i >= 0; i--) {
		if (cmdLine[i] == '|') {
			printf("SCAN: pipe found\n");
			strcpy(tail, cmdLine + i + 1);
			cmdLine[i] = 0;
			cmdLine[i + 1] = 0;
			strcpy(head, cmdLine);
			printf("head: %s", head);
			printf(" tail: %s\n", tail);
			return 1;
		}
	}
	strcpy(head, cmdLine);
	return 0;
}

//main(argc, argv) int argc; char* argv[];
//{
//	int stdin, stdout, stderr, i, j, pid, cid, me, status;
//
//	me = getpid();
//	printf("Enter ? for help menu\n");
//	while (1) {
//		printf("input command : ");
//		gets(cmdLine);
//		if (cmdLine[0] == 0)
//			continue;
//
//		token(cmdLine);   /* break up cmdLine into name[0], name[1],.... */
//
//		if (strcmp(name[0], "?") == 0 || strcmp(name[0], "help") == 0) {
//			menu(); continue;
//		}
//
//		if (strcmp(name[0], "logout") == 0)
//			exit(0);
//
//		/****************************************************************
//		   NOTE: some commands, e.g. cd, MUST be done by sh itself.
//				 OTHER commands will be done as shown below:
//		 ****************************************************************/
//
//		 /***** fork a child to execute the command ******/
//		pid = fork();
//		if (pid) {
//			printf("parent sh waits for child to die\n");
//			pid = wait(&status);
//		}
//		else {
//			printf("child task %d exec to %s\n", getpid(), name[0]);
//			exec(cmdLine);
//			printf("exec failed\n");
//		}
//	}
//}

// Algorithm base from pg 365 of Design and Implementation
//int main(int argc, char* argv[]) {
//
//	while (1)
//	{
//		// get a command line; e.g. cmdLine = cmd | cam2 | .... | cmdn &
//		// get cmd token from command line
//		if (cmd == cd || logout || su) {	// built-in commands
//			// do command directly
//			continue;
//		}
//		// for binary executable command
//		pid = fork();				// fork a child sh process
//		if (pid) // parent sh
//		{
//			// assume at most one & for main sh
//			if (/*(no & symbol)*/)
//				pid = wait(&status);
//			continue;
//		}
//		else // child sh
//			do_pipe(cmdLine, 0);
//
//	}
//
//
//}

//int do_pipe(char* cmdLine, int* pd) {
//
//	// if has a pipe passed in, as WRITER on pipe pd:
//	if (pd) {
//		close(pd[0]);		// close pipe READ end
//		dup2(pd[1], 1);		// redirect stdout to pipe WRITE end
//		close(pd[1]);		// close pipe WRITE end
//	}
//
//	// divide cmdLine into head, tail by rightmost pipe symbol
//	hasPipe = scan(cmdLine, head, tail);
//	if (hasPipe) {
//		// create a pipe lpd
//		pipe(lpd);
//		pid = fork();
//		if (pid) { // parent
//			// as READER on lpd
//			close(lpd[1]);
//			dup2(lpd[0], 0);	// redirect stdin to pipe READ end
//			close(lpd[0]);
//			do_command(tail);
//		}
//		else {
//			do_pipe(head, lpd);
//		}
//	}
//	else
//		do_command(cmdLine);
//}
//
//int do_command(char* cmdLine) {
//
//	// scan cmdLine for I/O redirection symbols
//	// do I/O redirections
//	// head = cmdLine BEFORE redirections
//	// exec(head);
//}