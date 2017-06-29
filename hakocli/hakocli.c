#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

#define BUFLEN	(8)
#define PATHLEN	(256)

int main(int argc, char *argv[])
{
	int fd, it;
	unsigned char buf[BUFLEN];
	int exclusive, nth;
	char devfile[PATHLEN];

	if ((argc < 4) || (argc > 5)) {
		fprintf(stderr, "Usage: %s file n_th n_iterate [-x]\n",
			argv[0]);
		return -1;
	}

	if ((argc == 5) && (argv[4][0] = '-') && (argv[4][1] = 'x'))
		exclusive = 1;
	else
		exclusive = 0;

	nth = strtol(argv[2], NULL, 0);
	snprintf(devfile, PATHLEN, "%s%d", argv[1], nth);
	fd = open(devfile, O_RDWR);
	if (fd < 0) {
		perror(devfile);
		return -1;
	}

	it = strtoul(argv[3], NULL, 0);
	do {
		size_t len, l;
		const size_t unit_len = (nth == 0) ? BUFLEN : 1;

		if (exclusive)
			flock(fd, LOCK_EX);
		for (len = 0; len < unit_len; len += l) {
			l = read(fd, &buf[len], unit_len - len);
			if (l < 0) {
				perror("read");
				if (exclusive)
					flock(fd, LOCK_UN);
				close(fd);
				return -1;
			}
		}

		if (it <= 0) {
			buf[len] = '\0';
			puts((const char *)buf);
			if (exclusive)
				flock(fd, LOCK_UN);
			break;
		}

		if (nth > 0)
			buf[0] = islower(buf[0]) ?
				toupper(buf[0]) : tolower(buf[0]);

		for (len = 0; len < unit_len; len += l) {
			l = write(fd, &buf[len], unit_len - len);
			if (l < 0) {
				perror("write");
				if (exclusive)
					flock(fd, LOCK_UN);
				close(fd);
				return -1;
			}
		}
		if (exclusive)
			flock(fd, LOCK_UN);
	} while (--it > 0);

	close(fd);
	return 0;
}
