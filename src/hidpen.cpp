#include <fstream>

#include "hidpen.h"
#include <iostream>
#include <sstream>

/*** variable definitions ***/

std::vector<hidpen::deviceInfo> hidpen::deviceList; //vector containing info of all the devices
hid_device* hidpen::device = NULL;          //the open device
bool hidpen::isOpen = false;
int hidpen::currentDevice_i = -1;  //which device is open 

unsigned char hidpen::buffer[hidpen::BUF_SIZE];     //buffer for reading and writing
float hidpen::pressure;                     //pressure value is updated in readPressure(). This is the latest value of pressure
unsigned int hidpen::pressureScale = 2048;

/*** function definitions ***/

bool hidpen::setup(std::string setting_file) {
    std::ifstream is(setting_file);
    
    
    if(!is.is_open()) return false;
    
    int device_id = -1;
    
    std::stringstream ss;
    
    ss << is.rdbuf();
    ss >> device_id;
    ss >> pressureScale;
#ifdef HIDPEN_DEBUG
    std::cout << "pressureScale set to: " << pressureScale << "\n";
#endif

    setup(device_id);
}


bool hidpen::setup(unsigned int device_i) {
    //list devices & see if there are any
    hidpen::getDevices();
    if(hidpen::deviceList.empty() ) {
        #ifdef HIDPEN_DEBUG
            std::cerr << "DEBUG:setup: No devices!\n";
        #endif
        return false;
    }

    if(device_i >= hidpen::deviceList.size() ) {
        #ifdef HIDPEN_DEBUG
            std::cerr << "DEBUG:setup: Bad device index: " << device_i << " / " << hidpen::deviceList.size() << "\n";
        #endif            
        return false;
    }

    //try to open device
    if(!hidpen::openDevice(device_i)) return false;

    return true;
}


bool hidpen::getDevices() { 
    //temporary linked list of devices and an iterator to it:
    struct hid_device_info *devs, *cur_dev; 

    //check init
    if (hid_init() == -1) 
        return false;

    deviceList.clear();

    //get the devices to the linked list:
    devs = hid_enumerate(0x0, 0x0); 

    //copy info from the temporary linked list to the deviceList vector:
    cur_dev = devs;	
    while (cur_dev) {
        deviceInfo newDeviceInfo(cur_dev->vendor_id, cur_dev->product_id, cur_dev->path);
        deviceList.push_back(newDeviceInfo);
        cur_dev = cur_dev->next;
    }

    //free the temporary list:
    hid_free_enumeration(devs);

    #ifdef HIDPEN_DEBUG
        std::cerr << "DEBUG:getDevices: Found " << deviceList.size() << " devices.\n";
    #endif        
    return true;
}


//open a device listed in deviceList by index:
bool hidpen::openDevice(int i) {

    //if device i is already open:
    if(i == currentDevice_i && isOpen == true)
        return false;

    //check init
    if (hid_init() == -1) 
        return false;

    //check if there is such index
    if(i >= deviceList.size() ) {
        #ifdef HIDPEN_DEBUG
            std::cerr << "DEBUG:openDevice: Bad device index! " << i << " / " << deviceList.size() << "\n";
        #endif  
        return false;
    }

    //close if open
    if(isOpen)
        closeDevice();

    //open device by path
    device = hid_open_path(deviceList[i].path.c_str() );
    if (!device) {
        #ifdef HIDPEN_DEBUG
            std::cerr << "DEBUG:openDevice: Could not open device" << i << "!\n";
        #endif
        return false;
    }
    else {
        hid_set_nonblocking(device, 1);
        #ifdef HIDPEN_DEBUG
            std::cerr << "DEBUG:openDevice: Opened device " << i << "\n";
        #endif
        isOpen = true;
        currentDevice_i = i;
        return true;
    }
}  


bool hidpen::closeDevice() {
    //check init
    if (hid_init() == -1) 
        return false;

    if(isOpen) {
        hid_close(device);
        isOpen = false;
        #ifdef HIDPEN_DEBUG
            std::cerr << "DEBUG:closeDevice: Closed device " << currentDevice_i << "\n";
        #endif
        return true;
    }
    else {
        #ifdef HIDPEN_DEBUG
            std::cerr << "DEBUG:closeDevice: No open device!\n";
        #endif
        return false;
    }
}


bool hidpen::nextDevice() {
    if(deviceList.size() <= 1) return false;

    unsigned int next_i = currentDevice_i + 1;
    if(next_i >= deviceList.size() ) next_i = 0; 
    openDevice(next_i);
    return true;
}


bool hidpen::prevDevice() {
    if(deviceList.size() <= 1) return false;

    int prev_i = currentDevice_i - 1;
    if(prev_i < 0) prev_i = deviceList.size()-1;
    openDevice(prev_i);
    return true;
}


bool hidpen::update() {
    //read all packets & update pressure
    int n;
    //count number of packages read (n)
    for(n = 0; readPressure() == true; n++)
        ;
    //if there were packages, return true
    if(n > 0) return true;
    else return false;
}


bool hidpen::readPressure() {
    //check init
    if (hid_init() == -1) 
        return false;

    if(!isOpen) {
        #ifdef HIDPEN_DEBUG
            std::cerr << "DEBUG:readPressure: device not open!\n";
        #endif            
        return false;
    }

    int err = 0;
    err = hid_read(device, buffer, sizeof(buffer));

    if(err >= 0) {
        
        #ifdef HIDPEN_DEBUG
            if(err != 0) std::cerr << "DEBUG:readPressure: Read " << err << " bytes. ";
        #endif            
            
        //If packet was too small to contain pen pressure data:
        if(err < PRESSURE_BYTE + 1) {
            #ifdef HIDPEN_DEBUG
                if(err != 0) std::cerr << "No pressure data!\n";
            #endif            
            return false;
        }

        //get pressure
        unsigned short pressure_s = (unsigned short) buffer[PRESSURE_BYTE + 1] << 8 | (unsigned short) buffer[PRESSURE_BYTE];
        pressure = (float)pressure_s / pressureScale;
        #ifdef HIDPEN_DEBUG
            std::cerr << "Pressure: " << pressure << " (" << pressure_s << ")\n";
        #endif
        return true;
    }
    else {
        #ifdef HIDPEN_DEBUG
            std::cerr << "DEBUG:readPressure: Could not read! " << hid_error(device) << "\n";
        #endif
        return false;
    }
}


bool hidpen::printDevice(unsigned int i) {
    //check init
    if (hid_init() == -1) 
        return false;

    if(i >= deviceList.size() )
        return false;
    printf("Device %i: \nType: %04hx:%04hx \nPath: %s \n\n", i, deviceList[i].vendor_id, deviceList[i].product_id, deviceList[i].path.c_str() );
    return true;
}


bool hidpen::printDevices() {
    //check init
    if (hid_init() == -1) 
        return false;

    if (deviceList.empty() ) {
        printf("No devices!\n");
        return false;
    }        
    for(int i=0; i<deviceList.size(); i++)
        printDevice(i);
    return true;
}   


bool hidpen::exit() {
    if(isOpen) {
        if(!closeDevice() ) {
            #ifdef HIDPEN_DEBUG
                std::cerr << "DEBUG:exit: Couldn't close device! " << hid_error(device) << "\n";
                std::cerr << "DEBUG:exit: Try to exit anyway... " << "\n";
            #endif
        }
    }    
    if(hid_exit() < 0) {
        #ifdef HIDPEN_DEBUG
            std::cerr << "DEBUG:exit: Couldn't exit! "<< "\n";
        #endif
        return false;                        
    }
    else {
        #ifdef HIDPEN_DEBUG
            std::cerr << "DEBUG:exit: Exit." << "\n";
        #endif 
        return true;
    }            
}


void hidpen::clearBuffer() {
    std::fill(buffer, buffer + BUF_SIZE, 0);
}

