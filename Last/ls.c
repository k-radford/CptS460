// Kate Radford
// LS.
// list cwd or filename *NO time field*
// format : ls [filename]
// pg299 in Embedded and Real-Time

#include "ucode.c"


char* t1 = "xwrxwrxwr-------";
char* t2 = "----------------";
struct stat mystat, * sp;

int main(int argc, char* argv[]) {

	struct stat mystat, * sp;
	int r;
	char* s;
	char filename[1024], cwd[1024];

	s = argv[1];
	if (argc == 1)		// no parameter : ls CWD
		s = "./"
	sp = &mystat;
	r = stat(s, sp);	// stat() syscall
	if (r < 0) {		
		perror("ls");
		exit(1);
	}
	strcpy(filename, s);
	if (s[0] != '/') {			// filename is relative to CWD
		getcwd(cwd, 1024);
		strcpy(filename, cwd);
		strcat(filename, "/");
		strcat(filename, s);	// construct $CWD/filename
	}
	if (S_ISDIR(sp->st_mode))
		ls_dir(filename);
	else
		ls_file(filename);

}

// list a single file
int ls_file(char* fname) {

	int r, i;
	char sbuf[4096];
	r = lstat(fname, sp);		// lstat the file
	if (S_ISDIR(sp->st_mode))	// test for DIR file
		printf("%c", 'd');		
	if (S_ISREG(sp->st_mode))	// test for REG file
		printf("%c", '-');
	if (S_ISLINK(sp->st_mode))	// test for symbolic file
		printf("%c", '1');
	for (i = 8; i >= 0; i--) {
		if (sp->st_mode & (1 << i))
			printf("%c", t1[i]); // print permission bit as r w x
		else
			printf("%c", t2[i]); // print permission bit as -
	}
	printf("%4d ", sp->st_nlink);		// link count
	printf("%4d ", sp->st_uid);			// uid
	printf("%8d ", sp->st_size);		// file size
	strcpy(ftime, ctime(&sp->st_ctime));
	ftime[strlen(ftime) - 1] = 0;		// kill \n at end
	printf("%s ", ftime);				// time in calendar form
	printf("%s ", basename(fname));		// file basename
	if (S_ISLINK(sp->st_mode)) {
		r = readlink(fname, sbuf, 4096);
		printf(" -> %s", sbuf);			// -> linked pathname
	}
	printf("\n");

}

// list a directory
int ls_dir(char* dname) {

	char name[256]; // EXT2 filename: 1-255 chars
	DIR* dp;
	struct dirent* ep;
	// open DIR to read names
	dp = opendir(dname);			// opendir() syscall
	while (ep = readdir(dp)) {		// readdir() syscall
		strcpy(name, ep->d_name);
		if (!strcmp(name, ".") || !strcmp(name, "..")) {
			continue; // skip over . and .. entries
		}
		strcpy(name, dname);
		strcat(name, "/");
		strcat(name, ep->d_name);
		ls_file(name);
	}
}