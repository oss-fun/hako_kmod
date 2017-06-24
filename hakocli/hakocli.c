#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

#define BUFLEN	(8)

int main(int argc, char *argv[])
{
	int fd, it;
	unsigned char buf[BUFLEN];

	if ((argc < 4) || (argc > 4)) {
		fprintf(stderr, "Usage: %s file n_th n_iterate\n",
			argv[0]);
		return -1;
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		perror("open");
		return -1;
	}

	it = strtoul(argv[3], NULL, 0);
	do {
		unsigned char *pc;
		unsigned int nth;
		size_t len, l;

		for (len = 0; len < BUFLEN; len += l) {
			l = read(fd, &buf[len], BUFLEN - len);
			if (l < 0) {
				perror("read");
				close(fd);
				return -1;
			}
		}

		if (it <= 0) {
			buf[BUFLEN] = '\0';
			puts((const char *)buf);
			break;
		}

		nth = strtoul(argv[2], NULL, 0);
		if (nth > BUFLEN) {
			fprintf(stderr, "invalid n_th %d\n", nth);
			close(fd);
			return -1;
		}
		pc = &buf[nth - 1];
		*pc = islower(*pc) ? toupper(*pc) : tolower(*pc);

		for (len = 0; len < BUFLEN; len += l) {
			l = write(fd, &buf[len], BUFLEN - len);
			if (l < 0) {
				perror("write");
				close(fd);
				return -1;
			}
		}
	} while (--it > 0);

	close(fd);
	return 0;
}