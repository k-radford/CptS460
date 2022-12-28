// Kate Radford
// more.c
// display file one page/line at a time
//  <enter> : show one more line
//  <space> : show one more screen(of 25 lines) : 1024 bytes

#include "ucode.c"

int main(int argc, char* argv[]) {

	int stdin = 0, j, n, i, fd = 0;;
	char buf[1024], c[1], cr = '\r';

	printf("------------- KATE'S MORE -------------\n");

	// get proc.tty name
	gettty(mytty);

	// int stat(const char *path, struct stat *buf);
	// stats the file pointed to by path and fills in buf.
	stat(mytty, &myst);
	fstat(0, &st0);
	fstat(1, &st1);
	// int fstat(int filedes, struct stat *buf);
	// identical to stat(), except that the file to be stat-ed is specified by the file descriptor filedes.

	if (myst.st_ino != st0.st_ino) {
		printf("MORE: redirected\n");
		stdin = 1;
	}
	// now what?

	if (argc > 1) { // read from file
		fd = open(argv[1], O_RDONLY);
		if (fd < 0) {
			printf("MORE: Error! cannot open file\n");
			exit(1);
		}

		while (1) {
			read(0, c, 1);
			//gets(c);

			// enter: one line
			if (c[0] == '\n' || c[0] == '\r') {
				while (1) {
					n = read(fd, buf, 1);

					write(1, &buf[0], 1);

					if (buf[0] == '\n') {
						write(2, &cr, 1);
						break; // break at end of line
					}
				}
			}

			// space: 25 lines
			if (c[0] == ' ') {
				for (int i = 0; i < 25; i++) {
					while (1) {
						n = read(fd, buf, 1);

						write(1, &buf[0], 1);

						if (buf[0] == '\n') {
							write(2, &cr, 1);
							break;
						}
					}
				}
			}

		}

	}

	else { // no filename: read from stdin to stdout same as cat
		while (read(0, c, 1)) {
			write(1, c, 1);
		}
	}




}