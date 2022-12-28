// Kate Radford
// init.c
// program of INIT process
// INIT Program Algorithm referenced from Design and Implementation pg362
// also Embedded and Real-Time pg321
// also KC WANG's base code found in sdimage

#include "ucode.c"
int console, s0, s1;

// NOTES:
// Special files /dev/ttyS0, /dev/ttyS1, etc. are for serial terminals.
// P2 : /dev/tty0
// P1 fork -> P3 : /dev/ttyS0 and P4 : /dev/ttyS1

// MODIFY to fork several login processes
int main(int argc, char* argv[]) {
	int in, out;						// file descriptors for terminal I/O

	// open /dev/tty0 as 0 (READ) and 1 (WRTIE) in order to display messages
	in = open("/dev/tty0", 0);
	out = open("/dev/tty0", 1);	// for display to console

	printf("You are in Kate's INIT\n");
	printf("INIT : fork a login proc on console\n");

	// FORK: On success, the PID of the child process is returned in the parent’s thread of execution, 
	// and a 0 is returned in the child’s thread of execution.
	console = fork();
	if (console) { // parent
		s0 = fork();
		if (s0) {
			s1 = fork();
			if (s1) {
				parent();
			}
			else { //P4
				exec("login /dev/ttyS1");
			}
		}
		else { //P3
			exec("login /dev/ttyS0");
		}
	}
	else  // child exec to login on tty0 -> P2
		exec("login /dev/tty0");		
}

// P1 plays the role of the INIT process(P1) in Unix.
int parent() {		// PI's code
	int pid, status;

	while (1) {
		printf("INIT : wait for ZOMBIE child\n");
		pid = wait(&status);

		if (pid == console) {
			printf("INIT : forks a new console login\n");
			console = fork();
			if (console)
				continue;
			else
				exec("login /dev/tty0");
		}

		if (pid == s0) {
			printf("INIT : forks a new s0 login\n");
			s0 = fork();
			if (s0)
				continue;
			else
				exec("login /dev/ttyS0");
		}

		if (pid == s1) {
			printf("INIT : forks a new s1 login\n");
			s1 = fork();
			if (s1)
				continue;
			else
				exec("login /dev/ttyS1");
		}

		printf("INIT : I just buried an orphan child proc %d\n", pid);
	}
}

//*************************************************************************
//                      Logic of init.c 
// NOTE: this init.c creates only ONE login process on console=/dev/tty0
// YOUR init.c must also create login processes on serial ports /dev/ttyS0
// and /dev/ttyS1.. 
//************************************************************************
//int pid, child, status;
//int stdin, stdout;
//
//#include "ucode.c"
//
//main(argc, argv) int argc; char* argv[];
//{
//	// open /dev/tty0 as 0 (READ) and 1 (WRTIE) in order to display messages
//	stdin = open("/dev/tty0", 0);
//	stdout = open("/dev/tty0", 1);
//
//	// Now we can use printf, which calls putc(), which writes to stdout
//	printf("KCINIT : fork a login task on console\n");
//	child = fork();
//
//	if (child)
//		parent();
//	else  /* login task */
//		login();
//}
//
//int login()
//{
//	exec("login /dev/tty0");
//}
//
//int parent()
//{
//	while (1) {
//		printf("KCINIT : waiting .....\n");
//
//		pid = wait(&status);
//
//		if (pid == child)
//			// fork another login child
//		else
//			printf("INIT : buried an orphan child %d\n", pid);
//
//	}
//}

//int main(int argc, char* argv[]) {
//	int in, out;						// file descriptors for terminal I/O
//	in = open("/dev/tty0", O_RDONLY);
//	out = open("/dev/tty0", O_WRONLY);	// for display to console
//	printf("INIT : fork a login proc on console\n");
//	console = fork();
//	if (console) { // parent
//		continue;
//	}
//	else  // child exec to login on tty0
//		exec("login /dev/tty0");
//}
//
//int parent() {		// PI's code
//	int pid, status;
//	while (1) {
//		printf("INIT : wait for ZOMBIE child\n");
//		pid = wait(&status);
//		if (pid == console) {
//			printf("INIT : forks a new console login\n");
//			console = fork();
//			if (console)
//				continue;
//			else
//				exec("login /dev/tty0");
//		}
//		printf("INIT : I just buried an orphan child proc %d\n", pid);
//	}
//}