// data transfe via shared memory

#include <stdio.h>
#include <string.h>
#if defined WIN
  #include <lusb0_usb.h>    // this is libusb, see http://libusb.sourceforge.net/
#else
  #include <usb.h>        // this is libusb, see http://libusb.sourceforge.net/
#endif
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define  KEY1 12345 //shared mry
#define  KEY2 6789  //semaphore
#define MAX_SAMPLES 1   // no.of xy samples to collect
//semaphore
union semun {
	int val;
	unsigned short *array;
};

int main (int argc, char **argv)
{

int shmid,index=0,j=0;
int sem_id1,sem_ret;
int xy_count=0,ret;
char *shm_addr = NULL;
char ch;
shmid = shmget(KEY1,getpagesize(),IPC_CREAT|0666);
shm_addr = (char*)shmat(shmid,NULL,IPC_CREAT|0666);
printf("shm_addr=0x%x\tshmid=%d\n",shm_addr,shmid);
bzero(shm_addr,getpagesize());

struct sembuf sb1;
union semun u1;
sem_id1 = semget(KEY2,1,IPC_CREAT | 0666);
if (sem_id1 < 0) {
	perror("semget error");
	exit(2);
}
u1.val = 1; //semaphore value
sem_ret = semctl(sem_id1,0,SETVAL,u1); //sem value is set
if(sem_ret < 0) {
	perror("Error in semctl");
	exit(3);
}

bool debug = true;
bool readToNewline = false;
int arg_pointer = 1;
int charsToRead = 0;
char thechar = ' ';

FILE *fd;
struct usb_bus *bus = NULL;
struct usb_device *digiSpark = NULL;
struct usb_device *device = NULL;

if(debug) printf("Detecting USB devices...\n");

// Initialize the USB library
usb_init();

// Enumerate the USB device tree
usb_find_busses();
usb_find_devices();

// Iterate through attached busses and devices
bus = usb_get_busses();
while(bus != NULL)
{
	device = bus->devices;
	while(device != NULL)
	{
		// Check to see if each USB device matches the DigiSpark Vendor and Product IDs
		if((device->descriptor.idVendor == 0x16c0) && (device->descriptor.idProduct == 0x05df))
       		{
			if(debug) printf("Detected DigiSpark... \n");
			digiSpark = device;
		}

		device = device->next;
	}

	bus = bus->next;
}

// If a digiSpark was found
if(digiSpark != NULL)
{
	int result = 0;
	int i = 0;
	int numInterfaces = 0;
	struct usb_dev_handle *devHandle = NULL;
	struct usb_interface_descriptor *interface = NULL;
	devHandle = usb_open(digiSpark);
	
	if(devHandle != NULL) {
		numInterfaces = digiSpark->config->bNumInterfaces;
		interface = &(digiSpark->config->interface[0].altsetting[0]);
		if(debug) printf("Found %i interfaces, using interface %i\n", numInterfaces, interface->bInterfaceNumber);
   
		// Try to read from the digispark
		if(debug) printf("Read from Digispark: ", thechar);
		while(1){
		//while(thechar != 4) {
			thechar = 4;
			result = usb_control_msg(devHandle, (0x01 << 5) | 0x80, 0x01, 0, 0, &thechar, 1, 2000);
			if(result < 0) {
				if(debug) printf("Error %i reading from USB device\n", result);
				break;
			}
			if(thechar!=4){	
				//sem_dec
				sb1.sem_num = 0;
				sb1.sem_op = -1;
				sb1.sem_flg = 0;
				sem_ret = semop(sem_id1,&sb1,1);
				if(sem_ret < 0) {perror("semop-dec error");exit(4);}
			
				shm_addr[index] = thechar;
				//printf("%c",shm_addr[index]);
				index++;	
				//sem_inc
				sb1.sem_num = 0;
				sb1.sem_op = +1;
				sb1.sem_flg = 0;
				sem_ret = semop(sem_id1,&sb1,1);
				if(sem_ret < 0) {perror("semop-dec error");exit(5);}
				
				if(thechar=='\n') {
				xy_count++;
				if(xy_count== MAX_SAMPLES) {xy_count=0;index=0;}
				}
	       		}
			/*else {
				if(readToNewline){
					if(thechar == '\n')
					break;
				}
				if(thechar != 4) {
				//	printf("%c", thechar);
				}  
				i++;
				if(i>=charsToRead && charsToRead>0)
				break;
			}*/
		}
//		if(debug) printf("\n", thechar);
		printf("exiting while loop\n");
		//}
		result = usb_release_interface(devHandle, interface->bInterfaceNumber);
		if(result < 0) {if(debug) printf("Error %i releasing Interface 0\n", result); return 1;}
		usb_close(devHandle);
	}
     
}
else {
	printf("No Digispark Found");
	return 1;
}     
printf("exiting\n") ;
return 0;
}
