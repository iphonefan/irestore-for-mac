/*
 MobileDevice.h
 Header for the MobileDevice framework used by iTunes + some extras
 
 Created by John Heaton(Gojohnnyboi) on Sunday, April 3, 2011
 Contact: gojohnnyboi@me.com
 
 Contributions are welcome.
 */

#if !defined(MOBILEDEVICE_H) && defined(__APPLE__)
#define MOBILEDEVICE_H
#include <CoreFoundation/CoreFoundation.h>

typedef struct {
    const char *name;
    uint16_t productID;
    const char *model;
    const char *board;
    uint32_t dfuDeviceID;
    uint32_t recoveryDeviceID;
} APPLE_MOBILE_DEVICE;

#define NUM_APPLE_MOBILE_DEVICES 13

static APPLE_MOBILE_DEVICE APPLE_MOBILE_DEVICES[NUM_APPLE_MOBILE_DEVICES] = {
    { "iPhone",         0x1290, "iPhone1,1",   "m68ap", 304222464,  310382848 },
    { "iPhone 3G",      0x1292, "iPhone1,2",   "n82ap", 304230656,  310391040 },
    { "iPhone 3G[s]",   0x1294, "iPhone2,1",   "n88ap", 35104,      35104 },
    { "iPhone 4(GSM)",  0x1297, "iPhone3,1",   "n90ap", 35120,      35120 },
    { "iPhone 4(CDMA)", 0x129c, "iPhone3,3",   "n92ap", 100698416,  100698416 },
    { "iPod touch 1G",  0x1291, "iPod1,1",     "n45ap", 304226560,  310386944 },
    { "iPod touch 2G",  0x1293, "iPod2,1",     "n72ap", 34592,      34592 },
    { "iPod touch 3G",  0x1299, "iPod3,1",     "n18ap", 33589538,   33589538 },
    { "iPod touch 4G",  0x129e, "iPod4,1",     "n81ap", 134252848,  134252848 },
    { "iPad",           0x129a, "iPad1,1",     "k48ap", 33589552,   33589552 },
    { "iPad 2(WiFi)",   0x129f, "iPad2,1",     "k93ap", 67144000,   67144000 },
    { "iPad 2(GSM)",    0x12a2, "iPad2,2",     "k94ap", 100698432,  100698432 },
    { "iPad 2(CDMA)",   0x12a3, "iPad2,3",     "k95ap", 33589568,   33589568 }
};

typedef struct {
    const char *name;
    uint16_t productID;
} APPLE_USB_INTERFACE_TYPE;

#define NUM_APPLE_USB_INTERFACES 6

static APPLE_USB_INTERFACE_TYPE APPLE_USB_INTERFACES[NUM_APPLE_USB_INTERFACES] = {
    { "Recovery Mode v1",   0x1280 },
    { "Recovery Mode v2",   0x1281 },
    { "Recovery Mode v3",   0x1282 },
    { "Recovery Mode v4",   0x1283 },
    { "DFU/WTF v1",         0x1222 },
    { "DFU/WTF v2",         0x1227 }
};

#define AMDeviceInterfaceIsRecovery(productID) \
((productID == APPLE_USB_INTERFACES[0].productID) || \
(productID == APPLE_USB_INTERFACES[1].productID) || \
(productID == APPLE_USB_INTERFACES[2].productID) || \
(productID == APPLE_USB_INTERFACES[3].productID))

#define AMDeviceInterfaceIsDFU(productID) \
((productID == APPLE_USB_INTERFACES[4].productID) || \
(productID == APPLE_USB_INTERFACES[5].productID))

enum {
    kAMDeviceNormalMode = 0,
    kAMDeviceRestoreMode = 1,
    kAMDeviceRecoveryMode = 2,
    kAMDeviceDFUMode = 3,
    kAMDeviceNoMode = 4
};
typedef int AMDeviceMode;

// Statuses for functions -- more will be added
typedef enum {
    kAMStatusSuccess = 0,
    kAMStatusFailure = !kAMStatusSuccess
} AMStatus;

// Possible messages in the AMDeviceNotificationRef sent to a AMDeviceConnectionCallback function
typedef enum {
    kAMDeviceNotificationMessageConnected = 1,
    kAMDeviceNotificationMessageDisconnected = 2,
    kAMDeviceNotificationMessageUnsubscribed = 3
} AMDeviceNotificationMessage;

// List of known services on the device

// MobileDevice types. Struct names based off of CoreFoundation
// NOTE: You may cast from dfu to a recovery device 
typedef struct __AMDevice *AMDeviceRef;
typedef struct __AMRecoveryModeDevice *AMRecoveryModeDeviceRef;
typedef struct __AMDFUModeDevice *AMDFUModeDeviceRef;
typedef struct __AMRestoreModeDevice *AMRestoreModeDeviceRef;

typedef unsigned char *AMDeviceSubscriptionRef;

// It is necessary for this object's struct to be public. There are no functions to properly obtain its elements
typedef struct {
    AMDeviceRef                     device; // can be cast to AMRestoreModeDeviceRef if notification is restore device
    AMDeviceNotificationMessage     message;
    AMDeviceSubscriptionRef         subscription;
} *AMDeviceNotificationRef;

// Callback declarations
typedef void (* AMRecoveryModeDeviceConnectionCallback)(AMRecoveryModeDeviceRef device);
typedef void (* AMDFUModeDeviceConnectionCallback)(AMDFUModeDeviceRef device);
typedef void (* AMDeviceConnectionCallback)(AMDeviceNotificationRef notification);

typedef void (* AMRestoreCallback)(void* a);

#ifdef __cplusplus
extern "C" {
#endif
	
	// Connection subscription functions. These will allow you to set up callback functions that are called upon connection/disconnection of a device.
	AMStatus AMRestoreRegisterForDeviceNotifications(AMDFUModeDeviceConnectionCallback DFUConnectCallback, AMRecoveryModeDeviceConnectionCallback recoveryConnectCallback, AMDFUModeDeviceConnectionCallback DFUDisconnectCallback, AMRecoveryModeDeviceConnectionCallback recoveryDisconnectCallback, unsigned int alwaysZero, void *userInfo);
	
	AMStatus AMDeviceNotificationSubscribe(AMDeviceConnectionCallback callback, int alwaysZero_1, int alwaysZero_2, int alwaysZero_3, AMDeviceSubscriptionRef *subscription);
	
	
	// Functions for use with AMDeviceRef objects(normal interface)
	AMStatus AMDeviceConnect(AMDeviceRef device);
	AMStatus AMDeviceDisconnect(AMDeviceRef device);
	int AMDeviceGetConnectionID(AMDeviceRef device);
	
	Boolean AMDeviceIsPaired(AMDeviceRef device);
	AMStatus AMDevicePair(AMDeviceRef device);
	AMStatus AMDeviceValidatePairing(AMDeviceRef device);
	
	CFStringRef AMDeviceCopyDeviceIdentifier(AMDeviceRef device);
	
	AMStatus AMDeviceEnterRecovery(AMDeviceRef device);
	
	AMStatus AMDeviceStartSession(AMDeviceRef device);
	AMStatus AMDeviceStopSession(AMDeviceRef device);
	
	CFTypeRef AMDeviceCopyValue(AMDeviceRef device, CFStringRef domain, CFStringRef valueName);
	
	AMStatus AMDeviceStartService(AMDeviceRef device, CFStringRef serviceName, CFSocketNativeHandle *socketDescriptor);
	
	AMStatus AMDPostNotification(CFSocketNativeHandle socket, CFStringRef notification, CFStringRef userinfo);
	
	uint16_t AMDeviceUSBProductID(AMDeviceRef device);
	
	void AMDeviceRelease(AMDeviceRef device);
	void AMDeviceRetain(AMDeviceRef device);
	
	// Functions for use with AMRecoveryModeDeviceRef objects(recovery interface)
	CFStringRef AMRecoveryModeDeviceCopyEnvironmentVariableFromDevice(AMRecoveryModeDeviceRef device, CFStringRef variable);
	
	AMStatus AMRecoveryModeDeviceSendCommandToDevice(AMRecoveryModeDeviceRef device, CFStringRef command);
	AMStatus AMRecoveryModeDeviceSendBlindCommandToDevice(AMRecoveryModeDeviceRef device, CFStringRef command);
	
	AMStatus AMRecoveryModeDeviceSendFileToDevice(AMRecoveryModeDeviceRef device, CFStringRef filename);
	
	AMStatus AMRecoveryModeDeviceSetAutoBoot(AMRecoveryModeDeviceRef device, Boolean autoBoot);
	AMStatus AMRecoveryModeDeviceReboot(AMRecoveryModeDeviceRef device);
	
	uint16_t AMRecoveryModeDeviceGetProductID(AMRecoveryModeDeviceRef device);
	uint32_t AMRecoveryModeDeviceGetProductType(AMRecoveryModeDeviceRef device);
	
	CFTypeID AMRecoveryModeDeviceGetTypeID(AMRecoveryModeDeviceRef device);
	
	AMStatus AMRecoveryModeDeviceCopyAuthInstallPreflightOptions(AMRecoveryModeDeviceRef device, CFDictionaryRef inputOptions, CFDictionaryRef *newRestoreOptions);
	AMStatus AMRestorePerformRecoveryModeRestore(AMRecoveryModeDeviceRef device, CFDictionaryRef restoreOptions, AMRestoreCallback callback, void *userInfo);
	
	// Functions for use with AMRestoreModeDeviceRef objects(restore interface)
	
	AMRestoreModeDeviceRef AMRestoreModeDeviceCreate(int alwaysZero_1, int connectionID, int alwaysZero_2);
	
	CFMutableDictionaryRef AMRestoreCreateDefaultOptions(CFAllocatorRef allocator); // may also be used for recovery/dfu restores
	AMStatus AMRestorePerformRestoreModeRestore(AMRecoveryModeDeviceRef device, CFDictionaryRef restoreOptions, void *callback, void *userInfo);
	
	// Functions for use with AMDFUModeDeviceRef objects(DFU/WTF interface)
	uint16_t AMDFUModeDeviceGetProductID(AMDFUModeDeviceRef device);
	uint32_t AMDFUModeDeviceGetProductType(AMDFUModeDeviceRef device);
	uint64_t AMDFUModeDeviceGetECID(AMDFUModeDeviceRef device);
	AMStatus AMRestorePerformDFURestore(AMDFUModeDeviceRef device, CFDictionaryRef restoreOptions, AMRestoreCallback callback, void *userInfo);
    
	AMStatus USBMuxConnectByPort(int connectionID, int phonePort, CFSocketNativeHandle *outHandle);
	// Log functions (iSn0wra1n)
	void AMRestoreSetLogLevel(int level);
	int AMRestoreEnableFileLogging(const char *filename);
	void AMDSetLogLevel(int level);
	void AMDAddLogFileDescriptor(int fd);
	
	// custom functions
	static APPLE_MOBILE_DEVICE *iOSGetDeviceType(uint16_t productID, uint32_t deviceID) {
		for(int i=0;i<NUM_APPLE_USB_INTERFACES;++i) {
			if(APPLE_USB_INTERFACES[i].productID == productID) {
				if(i < 4) {
					for(int i=0;i<NUM_APPLE_MOBILE_DEVICES;++i) {
						if(APPLE_MOBILE_DEVICES[i].recoveryDeviceID == deviceID) {
							return &APPLE_MOBILE_DEVICES[i];
						}
					}
				} else {
					for(int i=0;i<NUM_APPLE_MOBILE_DEVICES;++i) {
						if(APPLE_MOBILE_DEVICES[i].dfuDeviceID == deviceID) {
							return &APPLE_MOBILE_DEVICES[i];
						}
					}
				}
			}
		}
		
		for(int i=0;i<NUM_APPLE_MOBILE_DEVICES;++i) {
			if(APPLE_MOBILE_DEVICES[i].productID == productID) {
				return &APPLE_MOBILE_DEVICES[i];
			}
		}
		
		return NULL;
	}
    
#ifdef __OBJC__
	
	static NSString *iOSGetDeviceConnectionType(uint16_t productID, uint32_t deviceID, BOOL isRestoreMode) {
		NSString *modeName = (isRestoreMode ? @"Restore" : @"Normal");
		NSString *deviceName = @"Unknown Device";
		
		APPLE_MOBILE_DEVICE *device = iOSGetDeviceType(productID, deviceID);
		deviceName = [NSString stringWithUTF8String:device->name];
		
		for(int i=0;i<NUM_APPLE_USB_INTERFACES;++i) {
			if(APPLE_USB_INTERFACES[i].productID == productID) {
				if(i < 4)
					modeName = @"Recovery";
				else
					modeName = @"DFU";
			}
		}
		
		return [NSString stringWithFormat:@"%@: %@ Mode", deviceName, modeName];
	}
    
#endif /* __OBJC__ */
	
	static CFStringRef iOSGetDeviceClassName(APPLE_MOBILE_DEVICE *deviceType) {
		if(strstr(deviceType->model, "iPod") != NULL) {
			return CFSTR("iPod");
		} else if(strstr(deviceType->model, "iPhone") != NULL) {
			return CFSTR("iPhone");
		} else if(strstr(deviceType->model, "iPad") != NULL) {
			return CFSTR("iPad");
		}
		
		return CFSTR("Unknown");
	}
	
#ifdef __cplusplus 
}
#endif

#endif /* MOBILEDEVICE_H */