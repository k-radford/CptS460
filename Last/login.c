// Kate Radford
// login.c
// program of login processes
// Upon entry; argv[0] = login, argv[1] = /dev/ttyX
// Login Program algorithm referenced from Design and Implementation pg363
// also from Embedded and Real-Time pg322
// also KC WANG's base code found in sdimage

#include "ucode.c"
int in, out, err;
char Accounts[64][128];

// NOTES:
// Each line of /etc/passwd has the format:
// username:password:gid:uid:fullname:HOMEDIR:program
// root : xxxxxxx : 1000 : 0 : superuser : / root : sh

int main(int argc, char* argv[]) { // invoked by exec("login /dev/ttyxx")
	
	char buf[1024], username[128], password[128];
	int fd, n, accounts;

	// (1). close file descriptors 0, 1 inherited from INIT.
	close(0);
	close(1);

	// (2). open argv[1] 3 times as in(0), out(1), err(2).
	in = open(argv[1], 0);
	out = open(argv[1], 1);
	err = open(argv[1], 2);

	// (3). set the tty name string in PROC.tty
	settty(argv[1]);

	// NOW we can use printf, which calls putc() to our tty
	printf("Welcome to Kate's LOGIN\n");
	printf("LOGIN : open %s as stdin, stdout, stderr\n", argv[1]);

	// syscall(51, 2, 1); ignore Control-C interrupts so that Control-C KILLs 
	// other procs on this tty but not the main sh
	signal(2, 1);  

	// (4). open /etc/passwd file for READ.
	fd = open("/etc/passwd", 0);
	n = read(fd, buf, 1024);

	while (1) {

		// 1. show login : to stdout
		printf("login: ");
		// 2. read user name from stdin
		gets(username);
		// 3. show passwd :
		printf("password: ");
		// 4. read user passwd
		gets(password);

		// tokenize account lines
		accounts = tokenize(&buf[0]);

		// 5. verify user name and passwd from / etc / passwd file
		printf("verifying username and password...\n");
		// parse password file lines
		for (int i = 0; i < accounts; i++) {
			// 6. if (user account valid)
			if (strcmp(Accounts[i], username) == 0 && strcmp(Accounts[i+1], password) == 0) {
				printf("Welcome %s !\n", username);
				// change uid, gid to user's uid, gid
				chuid(atoi(Accounts[i+2]), atoi(Accounts[i+3]));
				// change cwd to user's home DIR
				chdir(Accounts[i+5]);
				// close opened /etc/passwd file
				close(fd);
				// exec to program in user account
				exec(Accounts[i+6]);
			}
		}
	}
	printf("login failed, try again\n");
}

int tokenize(char* line)
{
	int len = strlen(line);

	int j = 0, k = 0;
	int numAccounts = 0;
	char buf[128];
	
	for (int i = 0; i < len; i++) {
		if (line[i] != ':' && line[i] != '\r' && line[i] != '\n' && line[i] != 0 && line[i] != EOF) {
			buf[j] = line[i];
			j++;
		}
		else {
			buf[j] = '\0';
			strcpy(Accounts[k], buf);
			k++;
			j = 0;
			if (line[i] != '\n' || line[i] != EOF) {
				numAccounts++;
			}
		}
	}
	
	return numAccounts;
}

//***********************************************************************
//                   LOGIC of login.c file
//***********************************************************************
//char* tty;
//
//main(argc, argv) int argc; char* argv[];   // invoked by exec("login /dev/ttyxx")
//{
//	tty = argv[1];
//
//	// close 0, 1, 2 in case INIT opened them before
//	close(0); close(1); close(2);
//
//	// open tty (passed in by INIT) as stdin, stdout, stderr
//	stdin = open(tty, 0);
//	stdout = open(tty, 1);
//	stderr = open(tty, 1);
//
//	settty(tty);   // store tty string in PROC.tty[] for putc()
//
//	// NOW we can use printf, which calls putc() to our tty
//	printf("KCLOGIN : open %s as stdin, stdout, stderr\n", tty);
//
//	signal(2, 1);  // syscall(51, 2, 1); ignore Control-C interrupts so that 
//				  // Control-C KILLs other procs on this tty but not the main sh
//
//	while (1) {
//		// 1. show login : to stdout
//		// 2. read user name from stdin
//		// 3. show passwd :
//		// 4. read user passwd
//
//		// 5. verify user name and passwd from / etc / passwd file
//
//		// 6. if (user account valid) {
//			// setuid to user uid.
//				// chdir to user HOME directory.
//				// exec to the program in users's account
//		// }
//		printf("login failed, try again\n");
//	}
//}