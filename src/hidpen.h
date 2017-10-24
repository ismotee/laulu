#pragma once

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <hidapi/hidapi.h>
#include <algorithm>

//if HIDPEN_DEBUG is defined, prints all kinds of information to stderr:
#define HIDPEN_DEBUG

namespace hidpen{

    /*** device data type ***/
    struct deviceInfo{
        std::string path;
        unsigned short vendor_id;
        unsigned short product_id;
        deviceInfo(unsigned short vid, unsigned short pid, std::string path) {
            this->path = path;
            this->vendor_id = vid;
            this->product_id = pid;
        }
    };    
    
    /*** constants ***/
    
    //buffer size
    const unsigned int BUF_SIZE = 256;
    
    //location of pressure data in packet. Pressure data is 2 bytes.
    //NOTE: This information is from linuxwacom wiki, and might not be correct for all pens on all systems!
    const unsigned int PRESSURE_BYTE = 6;                                                       


    
    /*** variable declarations ***/
    /*     defined in hidpen.cpp */    
    //maximum value of pressure, used to scale it to [0...1]. NOTE: this also may or may not vary between devices and systems
    extern unsigned int pressureScale;               
    extern std::vector<deviceInfo> deviceList;  //vector containing info of all the devices
    extern hid_device* device;          //the open device
    extern bool isOpen;
    extern int currentDevice_i;  //which device is open         
    extern unsigned char buffer[BUF_SIZE];     //buffer for reading and writing
    extern float pressure;                     //pressure value is updated in readPressure(). This is the latest value of pressure    
    
    /*** function declarations ***/
    /*      defined in hidpen.cpp*/

    //this opens a device. Returns false if no devices can be opened
    bool setup(std::string setting_file);
    bool setup(unsigned int device_i = 0);    
    //populate the deviceList vector with your devices. Return false on error, true otherwise. Used by setup():
    bool getDevices();
    //open a device listed in deviceList by index:
    bool openDevice(int i);  
    //close the currently open device. Return true if closed, false if device was not open:
    bool closeDevice();
    //switch to next or previous device:
    bool nextDevice();        
    bool prevDevice();
    //update the pressure value. Return true if something was read
    bool update();
    //print out device info, return false on error:
    bool printDevice(unsigned int i);    
    //print all devices. Return false if there were no devices:
    bool printDevices();    
    //close hidapi
    bool exit();

    //read a pressure from pen. Used by update(). Return true if read, false otherwise. This only reads one value; use update() to read all packets
    bool readPressure();            
    //other functions use this:
    void clearBuffer();
            
};
