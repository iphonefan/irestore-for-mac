#include "irestore.h"
#include <getopt.h>

int norm = 0;
int dfu = 0;
int recovery = 0;

static struct __AMDevice *normDev;
static struct __AMRecoveryModeDevice *rdev;
static struct __AMDFUModeDevice *ddev;

void normalCallback(AMDeviceNotificationRef notif) {
	norm = 1;
	normDev = notif->device;
}
void dfuCallback(AMDFUModeDeviceRef dev) {
	dfu = 1;
	ddev = dev;
}
void disdfuCallback(AMDFUModeDeviceRef dev) {
	ddev = dev;
}
void recoveryCallback(AMRecoveryModeDeviceRef dev) {
	recovery = 1;
	rdev = dev;
}
void disRecoveryCallback(AMRecoveryModeDeviceRef dev) {
	rdev = dev;
}

void usage() {
	printf("irestore - utility to restore iDevices without iTunes ");
	printf("by iSn0wra1n\n");
	printf("Usage: irestore [Options]\n");	
	printf("Options:\n");
	printf("\t-i <file> Specify an IPSW file\n");
	printf("\t-r        Restore device\n");
	printf("\t-u        Update device\n\n");
	
	printf("\t-s <url>  Signing server URL (default: http://gs.apple.com)\n");
	printf("\t-b        Prevent baseband update\n");
	printf("\t-v        Be verbose\n");
}
int main (int argc, const char **argv[]) {
    if (argc < 2) {
		usage();
		return 0;
	}
	int c;
	
	while ((c = getopt(argc, argv, "i:rus:bv")) != EOF) {
		switch (c) {
			case 'i':
				ipsw = optarg;
				break;
			case 'r':
				MODE = RESTORE;
				break;
			case 'u':
				MODE = UPDATE;
				break;
			case 's':
				signingServer = optarg;
				break;
			case 'b':
				updateBB = false;
				break;
			case 'v':
				verbose = 1;
				break;
		}
	}
	
	if (!ipsw || MODE == 0) {
		usage();
		return 0;
	}
	
	if(verbose) {
        AMDSetLogLevel(INT_MAX);
        AMDAddLogFileDescriptor(1);
        AMRestoreSetLogLevel(INT_MAX);
        AMRestoreEnableFileLogging("/dev/stderr");
	}
	else {
		AMDSetLogLevel(0);
		AMRestoreSetLogLevel(0);
	}
	
	int ret = AMDeviceNotificationSubscribe(normalCallback, 0, 0, 0, NULL);
	if (ret != 0) {
		printf("AMDeviceNotificationSubscribe failed with status %d\n",ret);
		exit(ret);
	}
	ret = AMRestoreRegisterForDeviceNotifications(dfuCallback, recoveryCallback, disdfuCallback, disRecoveryCallback, 0, NULL);
	if (ret != 0) {
		printf("AMRestoreRegisterForDeviceNotifications failed with status %d\n", ret);
		exit(ret);
	}
	
	
	if (dfu) {		
		puts("Found device in DFU Mode");		
	}
	else if (recovery) {		
		puts("Found device in Recovery Mode");
		
	}
	else if (norm) {
		puts("Found device in Normal Mode");
	}	
	else {
		puts("Could not determine device mode. Please make sure a device is connected");
	}	

}