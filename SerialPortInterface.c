#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h> 
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <string.h>

#define BAUD_RATE B9600
#define TIMEOUT_WRITE 10000 //in uSec
#define TIMEOUT_READ 10000
#define MAX_PACKET_SIZE 20 //in bytes
#define SERIAL_PORT_DEVICE0 "/dev/ttyUSB0"
#define SERIAL_PORT_DEVICE1 "/dev/ttyUSB1"
#define SERIAL_PORT_DEVICE2 "/dev/ttyUSB2"
#define SERIAL_PORT_DEVICE3 "/dev/ttyUSB3"
#define NUM_DEC 1000
#define CALC_OP 'C'

unsigned char rx_packet[MAX_PACKET_SIZE];
unsigned char tx_packet[MAX_PACKET_SIZE];

int serialport_read(int fd, unsigned char* packet)
{
	int n = -1;
	unsigned char bytes_remaining[1];
	int i = 1; 
	int j, k;
	int packet_index = 0;
	unsigned char packet_aux[MAX_PACKET_SIZE] = "00000000000000000000";
	
	while(n<=0) {
		n = read(fd, &bytes_remaining, 1);
		i = atoi(bytes_remaining);
		if(n==1)
			break;
		if(n<=0) {
			usleep(10000);
			continue;
		}
	}
	n=-1;
	while(n<=0 && i > 0) {
	n = read(fd, &packet_aux[packet_index], 1);
		if(n<=0) {
			usleep(10000);
			continue;
		} else {
			packet_index++;
			n = -1;
	      i--;
		}
	}
			 
	memcpy(packet, packet_aux, MAX_PACKET_SIZE);
	return 0;
}

int serialport_write(int fd, unsigned char* msg) {
	int l = sizeof(msg)/sizeof(unsigned char);
	int n = -1;
	while(n<=0) {
		n = write(fd, msg, l);
     	if( n==0 ) {
         usleep(10000);
      continue;
     }
	}
	return 0;
}

int serialport_init(const char* serialport, int baud)
{
	struct termios toptions;
	int fd;

	fd = open(serialport, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1)  {
		return -1;
	}

	if (tcgetattr(fd, &toptions) < 0) {
		perror("Couldn't get term attributes of serial port. Program will now exit. Bye bye!");
		close(fd);
	return -1;
	}
	speed_t brate = baud;
	switch(baud) {
		case 4800:   
			brate=B4800;   
			break;
		case 9600:   
			brate=B9600;   
			break;
		#ifdef B14400
		case 14400:  
			brate=B14400;  
			break;
		#endif
		case 19200:  
			brate=B19200;  
			break;
		#ifdef B28800
		case 28800:  
			brate=B28800;  
			break;
		#endif
		case 38400:  
			brate=B38400;  
			break;
		case 57600:  
			brate=B57600;  
			break;
		case 115200: 
			brate=B115200; 
			break;
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
		printf("Couldn't set term attributes. Program will now exit. Bye bye!");
		close(fd);
		return -1;
	}
	return fd;
}


void get_user_message(){
	printf("Please, enter a command:\n");
	scanf("%s", &tx_packet);
}

double get_double(char* bytes, int dec_num) {
		double value = 0;
		int p_int=0, p_dec=0;
		char* p = (char*) &p_int;
		p[0] = bytes[0];
		p[1] = bytes[3];
		p = (char*) &p_dec;
		p[0] = bytes[4];
		p[1] = bytes[5];
		value = p_int + (1.0*p_dec/dec_num);
		return value;
}

void create_calc_packet(char id){
	tx_packet[0] = '2';
	tx_packet[1] = id ;
	tx_packet[2] = CALC_OP;
}

int main(int argc, char *argv[])
{
	int fd = -1;
	int option = 0;
	int err;
	unsigned char op_code;
	unsigned char target_id;
	char err_msg[200];	

	unsigned char ack_message[] = "40111";
	unsigned int state=0;	
	int msgs = 0;
	int id;
	
	fd = serialport_init(SERIAL_PORT_DEVICE0, BAUD_RATE);
	if(fd==-1) {
		fd = serialport_init(SERIAL_PORT_DEVICE1, BAUD_RATE);
	}
	if(fd==-1) {
		fd = serialport_init(SERIAL_PORT_DEVICE2, BAUD_RATE);
	}
	if(fd==-1) {
		fd = serialport_init(SERIAL_PORT_DEVICE3, BAUD_RATE);
	}
	if(fd==-1){
		perror("Unable to open serial port. Program will now exit. Bye bye!");
		return -1;
	}

	while (1) {
		switch(state) {
			case -1: {
				printf(err_msg);
				state = 0;
				break;
			}
	
		   case 0: {
				err = 0;
				memset(tx_packet, 0, sizeof(tx_packet));
				memset(rx_packet, 0, sizeof(rx_packet));
				printf("\n");
				printf("What would you like to do?\n");
				printf("1-Send single message\n");
				printf("2-Send raw message\n");
				scanf("%d", &state);
				if(state < 1 || state > 2) {
					strcpy(err_msg, "Error: Invalid option\n\0");
					state = -1;
				}

				if(state == 2)
					state = 6;
				break;
			}

			case 1: {
				printf("Please, insert the id of the target module\n");
				scanf("%d", &id);
				create_calc_packet(id +'0');
				state = 2;
				break;
			}

			case 2: {
				serialport_write(fd, tx_packet);
				state = 5;
				break;
			}

			case 3: {
				serialport_read(fd, rx_packet);
				double value = 0;
				int p_int=0, p_dec=0;
				char* p = (char*) &p_int;
				p[0] = rx_packet[2];
				p[1] = rx_packet[3];
				p = (char*) &p_dec;
				p[0] = rx_packet[4];
				p[1] = rx_packet[5];
				value = p_int + (1.0*p_dec/1000);
				printf("Power value: %f\n", value);
				state = 4;
				break;

			}

			case 4: {
				sleep(2);
				serialport_write(fd, ack_message);
				state = 0;
				break;
			}

			case 5: {
				serialport_read(fd, rx_packet);
				if(rx_packet[0] == '0') {
					printf("Error in identificer\n");	
					err = 1;
				}
				if(rx_packet[1] == '0') {
					printf("Error in opcode\n");	
					err = 1;
				}
				if(rx_packet[2] == '0') {
					printf("Error in data\n");	
					err = 1;
				}
				if(err)
					state = 0;
				else
					state = 3;
				break;
			}
			case 6: {
				printf("A raw message has the following: [N_BITS][MOD_ID][OP_COD][DATA]\n");
				scanf("%s", &tx_packet);
				state = 2;
				break;
			}
	
		}


	}
}
