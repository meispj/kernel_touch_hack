#include <dirent.h>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <paths.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/types.h>
#include <termios.h>
#include <time.h>
#include <pthread.h>
#include <thread>
#include <unistd.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <signal.h>
int GetEventCount()
{
	DIR *dir = opendir("/dev/input/");
	dirent *ptr = NULL;
	int count = 0;
	while ((ptr = readdir(dir)) != NULL)
	{
		if (strstr(ptr->d_name, "event"))
			count++;
	}
	return count;
}
char* get_device_name(int id) {
    char path[256];
    snprintf(path, sizeof(path), "/sys/class/input/event%d/device/name", id);

    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror("Unable to open file");
        return NULL;
    }

    static char device_name[256];
    if (fgets(device_name, sizeof(device_name), file) != NULL) {
        // Remove newline character if present
        size_t len = strlen(device_name);
        if (len > 0 && device_name[len - 1] == '\n') {
            device_name[len - 1] = '\0';
        }
    } else {
        perror("Unable to read device name");
        fclose(file);
        return NULL;
    }

    fclose(file);
    return device_name;
}


int GetEventId()
{
	
	int EventCount = GetEventCount();
	int *fdArray = (int *)malloc(EventCount * 4 + 4);
	int result;

	for (int i = 0; i < EventCount; i++)
	{
		char temp[128];
		sprintf(temp, "/dev/input/event%d", i);
		fdArray[i] = open(temp, O_RDWR | O_NONBLOCK);
	}

	int k = 0;
	input_event ev;
	puts("\033[31m请点击屏幕\033[0m");
	while (1)
	{
		for (int i = 0; i < EventCount; i++)
		{
			memset(&ev, 0, sizeof(ev));
			read(fdArray[i], &ev, sizeof(ev));
			if (ev.type == EV_ABS)
			{
				// printf("id:%d\n", i);
				free(fdArray);
				return i;
			}
		}
		//usleep(100);
	}
}