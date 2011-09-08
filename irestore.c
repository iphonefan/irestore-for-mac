#include "irestore.h"

int ret;
void normalCallback(AMDeviceNotificationRef notif) {
	puts("Found device in Normal Mode");	
	ret = AMDeviceEnterRecovery(notif->device);
	if (ret != 0) {
		puts("Device could not enter recovery mode!\n");
		exit(ret);
	}	
}
																								 
void restoreCallback(void* d) {
	printf("Restoring...");	
}
void dfuCallback(AMDFUModeDeviceRef dev) {
	puts("Found device in DFU Mode");	
	AMRestorePerformDFURestore(dev, createOptions(),restoreCallback, NULL);
}
void disdfuCallback(AMDFUModeDeviceRef dev) {
	printf("Device exited DFU Mode\n");
}
void recoveryCallback(AMRecoveryModeDeviceRef dev) {
	puts("Found device in Recovery Mode");	
	AMRestorePerformRecoveryModeRestore(dev, createOptions(), restoreCallback, NULL);
}
void disRecoveryCallback(AMRecoveryModeDeviceRef dev) {
	printf("Device exited Recovery Mode\n");
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
	
	ret = AMDeviceNotificationSubscribe(normalCallback, 0, 0, 0, NULL);
	if (ret != 0) {
		printf("AMDeviceNotificationSubscribe failed with status %d\n",ret);
		exit(ret);
	}
	ret = AMRestoreRegisterForDeviceNotifications(dfuCallback, recoveryCallback, disdfuCallback, disRecoveryCallback, 0, NULL);
	if (ret != 0) {
		printf("AMRestoreRegisterForDeviceNotifications failed with status %d\n", ret);
		exit(ret);
	}
	
	CFRunLoopRun();
	return 0;
}

CFMutableDictionaryRef createOptions() {
	CFMutableDictionaryRef dOptions = AMRestoreCreateDefaultOptions(kCFAllocatorDefault);
	CFMutableDictionaryRef cOptions = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	if (MODE == RESTORE) {
		CFDictionarySetValue(dOptions, CFSTR("AuthInstallVariant"), CFSTR("Customer Erase Install (IPSW)"));
	}
	else {
		CFDictionarySetValue(dOptions, CFSTR("AuthInstallVariant"), CFSTR("Customer Upgrade Install (IPSW)"));
	}

	CFDictionarySetValue(dOptions, CFSTR("AuthInstallSigningServerURL"), cs(signingServer));
	CFDictionarySetValue(dOptions, CFSTR("SourceRestoreBundlePath"), cs(ipsw));
	
	if (updateBB == false) {
		CFDictionarySetValue(dOptions, CFSTR("UpdateBaseband"), kCFBooleanFalse);
	}
	
	CFDictionarySetValue(cOptions, CFSTR("BootOptions"), dOptions);
	CFDictionarySetValue(cOptions, CFSTR("RestoreOptions"), dOptions);
	
	return cOptions;
}