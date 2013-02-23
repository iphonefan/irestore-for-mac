#include "irestore.h"

void restoreCallback(void* d) {
	printf("Restoring: arg=%p\n", d);
}

void normalCallback(AMDeviceNotificationRef notif) {
	printf("normalCallback: device=%p message=%i subscription=%s\n", notif->device, notif->message, notif->subscription);
    if (notif->message == kAMDeviceNotificationMessageConnected) {
        if(AMDeviceConnect(notif->device) == -402653144) { //kAMDWrongDroidError
            printf("Should be in restore mode.\n");
            
            CFMutableDictionaryRef dOptions = AMRestoreCreateDefaultOptions(kCFAllocatorDefault);
            CFDictionarySetValue(dOptions, CFSTR("RestoreBundlePath"), cs(ipsw));
            CFDictionarySetValue(dOptions, CFSTR("AuthInstallRestoreBehavior"), CFSTR("Update"));
            
            if(verbose)
                NSLog(CFSTR("%@"), dOptions);
            
            AMRestorePerformRestoreModeRestore(AMRestoreModeDeviceCreate(0, AMDeviceGetConnectionID(notif->device), 0), dOptions, restoreCallback, NULL);
        }
    }
}
																								 
void dfuConnectCallback(AMDFUModeDeviceRef dev) {
	printf("Found device in DFU Mode: device=%p\n", dev);
    
    CFMutableDictionaryRef cOptions = AMRestoreCreateDefaultOptions(kCFAllocatorDefault);
	if (MODE == RESTORE)
        CFDictionarySetValue(cOptions, CFSTR("AuthInstallRestoreBehavior"), CFSTR("Erase"));
	else
        CFDictionarySetValue(cOptions, CFSTR("AuthInstallRestoreBehavior"), CFSTR("Update"));
    CFDictionarySetValue(cOptions, CFSTR("AuthInstallSigningServerURL"), cs(signingServer));
    CFDictionarySetValue(cOptions, CFSTR("SourceRestoreBundlePath"), cs(ipsw));
	if (updateBB == false)
		CFDictionarySetValue(cOptions, CFSTR("UpdateBaseband"), kCFBooleanFalse);

    if(verbose)
        NSLog(CFSTR("%@"), cOptions);
    
	AMRestorePerformDFURestore(dev, cOptions, restoreCallback, NULL);
}

void dfuDisconnectCallback(AMDFUModeDeviceRef dev) {
	printf("Device exited DFU Mode: device=%p\n", dev);
}

void recoveryConnectCallback(AMRecoveryModeDeviceRef dev) {
	printf("Found device in Recovery Mode: device=%p\n", dev);
    
    CFMutableDictionaryRef cOptions = AMRestoreCreateDefaultOptions(kCFAllocatorDefault);
	if (MODE == RESTORE)
        CFDictionarySetValue(cOptions, CFSTR("AuthInstallRestoreBehavior"), CFSTR("Erase"));
	else
        CFDictionarySetValue(cOptions, CFSTR("AuthInstallRestoreBehavior"), CFSTR("Update"));
    CFDictionarySetValue(cOptions, CFSTR("AuthInstallSigningServerURL"), cs(signingServer));
    CFDictionarySetValue(cOptions, CFSTR("SourceRestoreBundlePath"), cs(ipsw));
	if (updateBB == false)
		CFDictionarySetValue(cOptions, CFSTR("UpdateBaseband"), kCFBooleanFalse);
	CFMutableDictionaryRef dOptions = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    AMRecoveryModeDeviceCopyAuthInstallPreflightOptions(dev, cOptions, &dOptions);
    
    if(verbose)
        NSLog(CFSTR("%@"), dOptions);

	AMRestorePerformRecoveryModeRestore(dev, dOptions, restoreCallback, NULL);
}

void recoveryDisconnectCallback(AMRecoveryModeDeviceRef dev) {
	printf("Device exited Recovery Mode: device=%p\n", dev);
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

int main (int argc, char **argv) {
	int c, ret;
    
	if (argc < 2) {
		usage();
		return 0;
	}    
	
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
	
    AMDeviceSubscriptionRef subscription;
	ret = AMDeviceNotificationSubscribe(normalCallback, 0, 0, 0, &subscription);
	if (ret != 0) {
		printf("AMDeviceNotificationSubscribe failed with status %d\n",ret);
		exit(ret);
	}
	ret = AMRestoreRegisterForDeviceNotifications(dfuConnectCallback, recoveryConnectCallback, dfuDisconnectCallback, recoveryDisconnectCallback, 0, NULL);
	if (ret != 0) {
		printf("AMRestoreRegisterForDeviceNotifications failed with status %d\n", ret);
		exit(ret);
	}
	
	CFRunLoopRun();
	return 0;
}
