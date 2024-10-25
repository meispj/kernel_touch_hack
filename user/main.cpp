#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include "touch.h"
#include "NativeSurfaceUtils.h"
android::ANativeWindowCreator::DisplayInfo displayInfo;
float x_proportion, y_proportion;
int fd;
int input_id;
struct input_absinfo absX;
struct input_absinfo absY;
typedef struct _UpTouch
{
    int x;
    int y;
    bool isdown;
  
} UpTouch, *PUpTouch;
	
void upthouch (bool isdown,int x,int y)
{
    float Width = absX.maximum + 1;
	float Height = absY.maximum + 1;
	
	int scr_x = displayInfo.width;
	int scr_y = displayInfo.height;
	
	if(scr_x > scr_y){
	    int t = scr_y;
	    scr_y = scr_x;
	    scr_x = t;
	}
    
    x_proportion =abs(Width / scr_x);
    y_proportion = abs(Height / scr_y);
	UpTouch uptc;
	uptc.isdown=isdown;

	if (displayInfo.orientation == 0)
    {
    uptc.x=x*x_proportion;
	uptc.y=y*y_proportion;
    }else if(displayInfo.orientation == 1)
    {
    uptc.x=y*y_proportion;
	uptc.y=scr_x+x * x_proportion;
    }else
    {
    uptc.x= scr_y+y * y_proportion;
	uptc.y=x*x_proportion;
    }

	ioctl(fd,1,&uptc);
}
void initthouch(char* name)
{
char buf[0x100];
strcpy(buf, name);
ioctl(fd,-1,name);
}
int main ()
{
    fd = open("/dev/PeiJue", O_RDWR);
    
	if (fd <0)
	{
	printf ("驱动链接失败");
	return 0;
	}
	input_id = GetEventId(); // 替换为实际的事件编号
	char *device_name = get_device_name(input_id);
	initthouch(device_name);
	printf ("Input_device_name :%s\n",device_name);
	char devicepath[64];
	sprintf(devicepath, "/dev/input/event%d", input_id);
	printf("Eventpath: %s \n", devicepath);
	
    int fb = open(devicepath, O_RDONLY | O_NONBLOCK);
	ioctl(fb, EVIOCGABS(ABS_MT_POSITION_X), &absX);
	ioctl(fb, EVIOCGABS(ABS_MT_POSITION_Y), &absY);
	

    displayInfo = android::ANativeWindowCreator::GetDisplayInfo();
  
    

	for (int i=0;i<500;i++)
	{
		
		upthouch(true,500+i,500+i);
		
		usleep (1000);
		
	}
	
	upthouch(false,0,0);
	return 0;
}