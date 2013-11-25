#include <stdio.h>    /* Standard input/output definitions */
#include <stdlib.h>
#include <stdint.h>   /* Standard types */
#include <string.h>   /* String function definitions */
#include <unistd.h>   /* UNIX standard function definitions */
#include <fcntl.h>    /* File control definitions */
#include <errno.h>    /* Error number definitions */
#include <termios.h>  /* POSIX terminal control definitions */
#include <sys/ioctl.h>
//#include <getopt.h>

#define BAUD_RATE B9600
#define STRING_SERIAL_PORT_LENGTH 256
#define STRING_BUF_LENGTH 256

#define SERIAL_PORT_DEVICE "/dev/ttyUSB1"


int serialport_read(int fd, char* packet)
{
  //printf("Now reading...\n");
  int n = -1;
  char byte_aux[20] = "00000000000000000000";
  int bytes_remaining; 
  int packet_index = 0;
	while(n<=0) {
     n = read(fd, &byte_aux, 20);  // read a char at a time
     if( n==-1) return -1;    // couldn't read*/
     if( n<=0 ) {
         usleep( 10 * 1000 ); // wait 10 msec try again
         continue;
     }
	} 
	memcpy(packet, byte_aux, 20);
	//printf("Packet:%c\n",packet[0]);
	
    return 0;
}

int serialport_write(int fd, unsigned char* msg) {
	int l = sizeof(msg)/sizeof(unsigned char);
	int n = -1;
	while(n<=0) {
		n = write(fd, "2AB", 3);
	   if( n==-1) return -1;    // couldn't read
     	if( n==0 ) {
         usleep( 10 * 1000 ); // wait 10 msec try again
      continue;
     }
	}
	printf("Write successfull!\n");
 return 0;
}

// takes the string name of the serial port (e.g. "/dev/tty.usbserial","COM1")
// and a baud rate (bps) and connects to that port at that speed and 8N1.
// opens the port in fully raw mode so you can send binary data.
// returns valid fd, or -1 on error
int serialport_init(const char* serialport, int baud)
{
    struct termios toptions;
    int fd;
   
    //fprintf(stderr,"init_serialport: opening port %s @ %d bps\n",
    //        serialport,baud);

    fd = open(serialport, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)  {
        perror("init_serialport: Unable to open port ");
        return -1;
    }
   
    if (tcgetattr(fd, &toptions) < 0) {
        perror("init_serialport: Couldn't get term attributes");
        return -1;
    }
    speed_t brate = baud; // let you override switch below if needed
    switch(baud) {
    case 4800:   brate=B4800;   break;
    case 9600:   brate=B9600;   break;
#ifdef B14400
    case 14400:  brate=B14400;  break;
#endif
    case 19200:  brate=B19200;  break;
#ifdef B28800
    case 28800:  brate=B28800;  break;
#endif
    case 38400:  brate=B38400;  break;
    case 57600:  brate=B57600;  break;
    case 115200: brate=B115200; break;
    }
    cfsetispeed(&toptions, brate);
    cfsetospeed(&toptions, brate);

    // 8N1
    toptions.c_cflag &= ~PARENB;
    toptions.c_cflag &= ~CSTOPB;
    toptions.c_cflag &= ~CSIZE;
    toptions.c_cflag |= CS8;
    // no flow control
    toptions.c_cflag &= ~CRTSCTS;

    toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
    toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

    toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    toptions.c_oflag &= ~OPOST; // make raw

    toptions.c_cc[VMIN]  = 0;
    toptions.c_cc[VTIME] = 20;
   
    if( tcsetattr(fd, TCSANOW, &toptions) < 0) {
        perror("init_serialport: Couldn't set term attributes");
        return -1;
    }

    return fd;
}



int main(int argc, char *argv[])
{
    int fd = 0;
    char serial_port[STRING_SERIAL_PORT_LENGTH];
    int baudrate = BAUD_RATE;  // default
    char c[20];
    unsigned char buf[20];
    int rc,n;

    for (n=0;n<STRING_SERIAL_PORT_LENGTH;n++) serial_port[n]='0';

 
    strcpy(serial_port,SERIAL_PORT_DEVICE);
    fd = serialport_init(serial_port, baudrate);
    if(fd==-1) {printf("Unable to open the device"); return -1;}
   
	 serialport_write(fd, "1");
    while (1) {
	/*
	 scanf("%s", &buf);
    serialport_write(fd, "1");*/
    serialport_read(fd, c);
		 printf("Packet:%c\n",c[0]);
       fflush(NULL);
    }
} // end main
