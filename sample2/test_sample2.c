/*DriverTest.c*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_LEN 80


int main(int argc, char *argv[])
{
	int testdev;
	int i;
	char buf[BUF_LEN];
	char *test_buf = "Test for sample2 read/write.";

	testdev = open("/dev/sample2_cdev0", O_RDWR);
	
	if (testdev == -1)
	{
		printf("Cann't open file \n");
		exit(0);
	}

	read(testdev, buf, BUF_LEN);
	printf("%s\n", buf);

	for (i = 0; i < BUF_LEN; i++)
	{
		buf[i] = '*';
	}

	write(testdev, buf, BUF_LEN);

	read(testdev, buf, BUF_LEN);
	printf("%s\n", buf);

	write(testdev, test_buf, strlen(test_buf));

	read(testdev, buf, BUF_LEN);
	printf("%s\n", buf);

	close(testdev);
}
