// Kate Radford
// cat.c
// display file contents to screen
// cat [filename]: if no filename:  cat from stdin

#include "ucode.c"

// NOTES:
// we may determine whether stdin/stdout has been redirected or not by:
// (1) stat(gettty(), &st_tty); fstat(0, &st0); fstat(1, &st1);
// (2) 0 has been redirected if st_tty.(dev, ino) != st0.(dev, ino)
// (3) 1 has been redirected if st_tty.(dev, ino) != st1.(dev, ino)

int main(int argc, char* argv[]) {

	char buf[1024];
	int fd = 0, n, stdout = 0;
	char r = '\r', nn = '\n';

	printf("------------- KATE's CAT -------------\n");

	// get proc.tty name
	gettty(mytty);
	
	// int stat(const char *path, struct stat *buf);
	// stats the file pointed to by path and fills in buf.
	stat(mytty, &myst); 
	fstat(0, &st0); 
	fstat(1, &st1);
	// int fstat(int filedes, struct stat *buf);
	// identical to stat(), except that the file to be stat-ed is specified by the file descriptor filedes.

	if (myst.st_ino != st1.st_ino) {
		stdout = 1;
	}

	
	// if stdout (fd=1) is redirected to a file, as in
	// cat f1 > f2 it would add extra \r chars to the file, which is INCORRECT.
	// REQUIREMENT: re-write uio.c to satisfy BOTH conditions:
	// (1).cat f1 : correct visual effect on screen
	// (2).cat f1 > f2 : sise of f2 must be SAME as f1
	
	if (argc > 1) { // filename provided
		// display contents of file
		printf("Opening %s...\n", argv[1]);
		fd = open(argv[1], O_RDONLY);
		while (n = read(fd, buf, 1024)) {
			int i = 0;
			buf[n] = '\0'; 

			if (!stdout) { 
				while (i < n) {
					// print a \r for each \n in order to produce the right visual effects
					write(1, &buf[i], 1);
					if (buf[i] == '\n') {
						write(2, &r, 1);
					}
					i++;
				}
			}
			else { // if stdout has been redirected, it must not add a \r to each line
				write(1, buf, n);
			}
		}
		close(fd);
	}
	else { // no filename:  cat from stdin
		while (gets(buf) > 0) {
			// echo inputs
			write(1, buf, sizeof(char)*strlen(buf));
		}
	}
}