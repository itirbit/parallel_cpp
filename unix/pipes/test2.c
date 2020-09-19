#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
	const char* ipath = "/tmp/stepik_in.fifo";
	const char* opath = "/tmp/st_out.fifo";
	//int ipipe = mkfifo(ipath, 0666);
	//int opipe = mkfifo(opath, 0666);

	char data[1024];
	
	int ifd = open(ipath, O_CREAT | O_RDONLY | O_NONBLOCK);
	if (ifd < 0)
		return 1;
	int nbytes = read(ifd, data, 1024);
	close(ifd);

	int ofd = open(opath, O_CREAT | O_RDWR | O_TRUNC);
	if (ofd < 0)
		return 1;
	write(ofd, data, nbytes);
	close(ofd);
	return 0;
}
