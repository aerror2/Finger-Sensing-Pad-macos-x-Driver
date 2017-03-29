/*
 * Copyright (c) 2002 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.2 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License ater
 * http://www.apple.com/publicsource and read it before using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

#include <IOKit/IOLib.h>
#include <IOKit/hidsystem/IOHIDParameter.h>
#include <IOKit/IOWorkLoop.h>
#include <IOKit/IOTimerEventSource.h>

#include "VoodooPS2Controller.h"
#include "VoodooPS2SentelicFSP.h"

enum {
    kModeByteValueGesturesEnabled  = 0x00,
    kModeByteValueGesturesDisabled = 0x04
};




/* Finger-sensing Pad information registers */
#define	FSP_REG_DEVICE_ID	0x00
#define	FSP_REG_VERSION		0x01
#define	FSP_REG_REVISION	0x04
#define	FSP_REG_TMOD_STATUS1	0x0B
#define	FSP_BIT_NO_ROTATION	0x08
#define	FSP_REG_PAGE_CTRL	0x0F

/* Finger-sensing Pad control registers */
#define	FSP_REG_SYSCTL1		0x10
#define	FSP_BIT_EN_REG_CLK	0x20
#define	FSP_REG_OPC_QDOWN	0x31
#define	FSP_BIT_EN_OPC_TAG	0x80
#define	FSP_REG_OPTZ_XLO	0x34
#define	FSP_REG_OPTZ_XHI	0x35
#define	FSP_REG_OPTZ_YLO	0x36
#define	FSP_REG_OPTZ_YHI	0x37
#define	FSP_REG_SYSCTL5		0x40
#define	FSP_REG_SWREG1		0x90


#define	FSP_BIT_90_DEGREE	0x01
#define	FSP_BIT_EN_MSID6	0x02
#define	FSP_BIT_EN_MSID7	0x04
#define	FSP_BIT_EN_MSID8	0x08
#define	FSP_BIT_EN_AUTO_MSID8	0x20
#define	FSP_BIT_EN_PKT_G0	0x40

#define	FSP_REG_ONPAD_CTL	0x43
#define	FSP_BIT_ONPAD_ENABLE	0x01
#define	FSP_BIT_ONPAD_FBBB	0x02
#define	FSP_BIT_FIX_VSCR	0x03
#define	FSP_BIT_FIX_HSCR	0x20
#define	FSP_BIT_DRAG_LOCK	0x40


/* Finger-sensing Pad packet formating related definitions */

/* absolute packet type */
#define	FSP_PKT_TYPE_NORMAL	(0x00)
#define	FSP_PKT_TYPE_ABS	(0x01)
#define	FSP_PKT_TYPE_NOTIFY	(0x02)
#define	FSP_PKT_TYPE_NORMAL_OPC	(0x03)
#define	FSP_PKT_TYPE_SHIFT	(6)


#define FSP_DEVICE_MAGIC		0x01


/* swreg1 values, supported in Cx hardware */
#define FSP_CX_ABSOLUTE_MODE     0x01
#define FSP_CX_GESTURE_OUTPUT	0x02
#define FSP_CX_2FINGERS_OUTPUT	0x04
#define FSP_CX_FINGER_UP_OUTPUT	0x08
#define FSP_CX_CONTINUOUS_MODE	0x10
#define FSP_CX_GUEST_GROUP_BIT1  0x20
#define FSP_CX_GUEST_GROUP_BIT2  0x40
#define FSP_CX_COMPATIBLE_MODE   0x80

#define FSP_CX_NOTIFY_MSG_TYPE_GUESTURE         0xba
#define FSP_CX_NOTIFY_MSG_TYPE_ONE_FINGER_HOLD  0xC0

//byte0
#define MFMT_LEFT_BTN_DOWN  0x01
#define MFMT_RIGHT_BTN_DOWN 0x02
#define MFMT_FINGER_INDEX   0x04
#define MFMT_PS2_SPECIFY    0x08
#define MFMT_LEFT_BTN_OPC   0x10
#define MFMT_COORD_MODE     0x20

//byte3
#define MFMT_SCROLL_RIGHT_BTN 0x80
#define MFMT_SCROLL_LEFT_BTN  0x40
#define MFMT_5TH_BTN          0x20
#define MFMT_4TH_BTN          0x10

#define MFMT_MID_BTN_DOWN   0x04





// =============================================================================
// ApplePS2SentelicFSP Class Implementation
//

OSDefineMetaClassAndStructors(ApplePS2SentelicFSP, IOHIPointing);




const char * fsp_get_guesture_name_by_id(int gestureId)
{
    /*
     ID	Description
     0x86	2 finger straight up
     0x82	2 finger straight down
     0x80	2 finger straight right
     0x84	2 finger straight left
     0x8f	2 finger zoom in
     0x8b	2 finger zoom out
     0xc0	2 finger curve, counter clockwise
     0xc4	2 finger curve, clockwise
     0x2e	3 finger straight up
     0x2a	3 finger straight down
     0x28	3 finger straight right
     0x2c	3 finger straight left
     0x38	palm
     */
    
    switch (gestureId) {
        case	0x86:	return ("	2 finger straight up             ");break;
        case	0x82:	return ("	2 finger straight down           ");break;
        case	0x80:	return ("	2 finger straight right          ");break;
        case	0x84:	return ("	2 finger straight left           ");break;
        case	0x8f:	return ("	2 finger zoom in                 ");break;
        case	0x8b:	return ("	2 finger zoom out                ");break;
        case	0xc0:	return ("	2 finger curve, counter clockwise");break;
        case	0xc4:	return ("	2 finger curve, clockwise        ");break;
        case	0x2e:	return ("	3 finger straight up             ");break;
        case	0x2a:	return ("	3 finger straight down           ");break;
        case	0x28:	return ("	3 finger straight right          ");break;
        case	0x2c:	return ("	3 finger straight left           ");break;
        case	0x38:	return ("	palm                             ");break;
        default:
            return         ("unkonw Guesture id               ");
            break;
    }
    
    
}


void fsp_packet_debug(unsigned char packet[],UInt32 packetSize)
{
	static unsigned int ps2_packet_cnt;
	//static unsigned int ps2_last_second;
//	unsigned int jiffies_msec;
	const char *packet_type = "UNKNOWN";
	int abs_x = 0, abs_y = 0;
    int lb=0,rb=0,mb=0,fi=0,opc=0,cm=0;
    int sl=0,sr=0,b4=0,b5=0;
	/* Interpret & dump the packet data. */
	switch (packet[0] >> FSP_PKT_TYPE_SHIFT) {
        case FSP_PKT_TYPE_ABS:
            packet_type = "Absolute";
            abs_x = (packet[1] << 2) | ((packet[3] >> 2) & 0x03);
            abs_y = (packet[2] << 2) | (packet[3] & 0x03);
          
            if(MFMT_LEFT_BTN_DOWN & packet[0]) lb=1;
            if(MFMT_RIGHT_BTN_DOWN &packet[0]) rb =1;
            if(MFMT_FINGER_INDEX&packet[0]) fi=1;
            if(MFMT_LEFT_BTN_OPC&packet[0]) opc=1;
            if(MFMT_COORD_MODE&packet[0]) cm=1;
            
            if(MFMT_4TH_BTN&packet[3])b4=1;
            if(MFMT_5TH_BTN&packet[3])b5=1;
            if(MFMT_SCROLL_LEFT_BTN&packet[3])sl=1;
            if(MFMT_SCROLL_RIGHT_BTN&packet[3]) sr =1;
            
            IOLog(
                  "%08d: %s packet: %02x, %02x, %02x, %02x;"
                  "abs_x: %d, abs_y: %d,"
                  "lb:%d,rb:%d,fi:%d,opc:%d,cm:%d,b4:%d,b5:%d,sl:%d,sr:%d\n",
                  ps2_packet_cnt, packet_type,
                  packet[0], packet[1], packet[2], packet[3],
                  abs_x, abs_y,
                  lb,rb,fi,opc,cm,
                  b4,b5,sl,sr
                  );
            
            break;
        case FSP_PKT_TYPE_NORMAL:
            packet_type = "Normal";
            IOLog(
                  "%08d: %s packet: %02x, %02x, %02x, %02x;",
                  ps2_packet_cnt, packet_type,
                  packet[0], packet[1], packet[2], packet[3]);
            
            break;
        case FSP_PKT_TYPE_NOTIFY:
            if(MFMT_LEFT_BTN_DOWN & packet[0]) lb=1;
            if(MFMT_RIGHT_BTN_DOWN &packet[0]) rb =1;
            if(MFMT_MID_BTN_DOWN&packet[0]) mb=1;
            if(MFMT_LEFT_BTN_OPC&packet[0]) opc=1;
            
            packet_type = "Notify";
            
            IOLog(
                  "%08d: %s: %02x, %02x, %02x, %02x;"
                  "lb:%d,rb:%d,mb:%d,opc:%d\n",
                  ps2_packet_cnt, packet[1]==0xba?fsp_get_guesture_name_by_id(packet[2]):packet_type,
                  packet[0], packet[1], packet[2], packet[3],
                  lb, rb,mb,opc);
            
            break;
        case FSP_PKT_TYPE_NORMAL_OPC:
            packet_type = "Normal-OPC";
            IOLog(
                  "%08d: %s packet: %02x, %02x, %02x, %02x;",
                  ps2_packet_cnt, packet_type,
                  packet[0], packet[1], packet[2], packet[3]);
            
            break;
	}
    
	ps2_packet_cnt++;
	//jiffies_msec = jiffies_to_msecs(jiffies);

//	if (jiffies_msec - ps2_last_second > 1000) {
//		psmouse_dbg(psmouse, "PS/2 packets/sec = %d\n", ps2_packet_cnt);
//		ps2_packet_cnt = 0;
//		ps2_last_second = jiffies_msec;
//	}
}


UInt32 ApplePS2SentelicFSP::deviceType()
{ return NX_EVS_DEVICE_TYPE_MOUSE; };

UInt32 ApplePS2SentelicFSP::interfaceID()
{ return NX_EVS_DEVICE_INTERFACE_BUS_ACE; };

IOItemCount ApplePS2SentelicFSP::buttonCount() { return 2; };
IOFixed     ApplePS2SentelicFSP::resolution()  { return _resolution; };

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool ApplePS2SentelicFSP::init(OSDictionary* dict)
{
    //
    // Initialize this object's minimal state. This is invoked right after this
    // object is instantiated.
    //
    
    if (!super::init(dict))
        return false;
	
    // find config specific to Platform Profile
    OSDictionary* list = OSDynamicCast(OSDictionary, dict->getObject(kPlatformProfile));
    OSDictionary* config = ApplePS2Controller::makeConfigurationNode(list);
    if (config)
    {
        // if DisableDevice is Yes, then do not load at all...
        OSBoolean* disable = OSDynamicCast(OSBoolean, config->getObject(kDisableDevice));
        if (disable && disable->isTrue())
        {
            config->release();
            return false;
        }
#ifdef DEBUG
        // save configuration for later/diagnostics...
        setProperty(kMergedConfiguration, config);
#endif
    }
    
    // initialize state
    _device                    = 0;
    _interruptHandlerInstalled = false;
    _packetByteCount           = 0;
    _resolution                = (250) << 16; // (100 dpi, 4 counts/mm)
    _scrollresolution             = (250) <<16;
    _touchPadModeByte          =  kModeByteValueGesturesEnabled;//kModeByteValueGesturesDisabled;
    
    _last_abs_x = 0;
    _last_abs_y = 0;
    _last_abs_z  = 0;
    
    _isInGesture = false;
    
    keytime =0;
    _modifierdown=0;
    gestureStopTime=0;
    
     maxaftertyping = 500000000;
    scrolllockTime =    500000000;
    
    momentumscrollInteval = 20000000;
    OSSafeRelease(config);
	
    return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


int fsp_ps2_command(ApplePS2MouseDevice * device, PS2Request * request, int cmd)
{
    request->commands[0].command  = kPS2C_WriteCommandPort;
    request->commands[0].inOrOut  = kCP_TransmitToMouse;
    request->commands[1].command  = kPS2C_WriteDataPort;
    request->commands[1].inOrOut  = cmd;
    request->commands[2].command  = kPS2C_ReadDataPort;
    request->commands[2].inOrOut  = 0;

    request->commandsCount = 3;
    device->submitRequestAndBlock(request);

    //IOLog("ApplePS2Trackpad: Sentelic FSP: fsp_ps2_command(cmd = %0x) => %0x\n", cmd, request->commands[2].inOrOut);

    return (request->commandsCount == 3) ? request->commands[2].inOrOut : -1;
}

int fsp_reg_read(ApplePS2MouseDevice * device, PS2Request * request, int reg)
{
    int register_select = 0x66;
    int register_value = reg;

    // mangle reg to avoid collision with reserved values
    if (reg == 10 || reg == 20 || reg == 40 || reg == 60 || reg == 80 || reg == 100 || reg == 200) {
        register_value = (reg >> 4) | (reg << 4);
        register_select = 0xCC;
    } else if (reg == 0xe9 || reg == 0xee || reg == 0xf2 || reg == 0xff) {
        register_value = ~reg;
        register_select = 0x68;
    }

    fsp_ps2_command(device, request, 0xf3);
    fsp_ps2_command(device, request, 0x66);
    fsp_ps2_command(device, request, 0x88);
    fsp_ps2_command(device, request, 0xf3);
    fsp_ps2_command(device, request, register_select);
    fsp_ps2_command(device, request, register_value);

    request->commands[0].command  = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[0].inOrOut  = kDP_GetMouseInformation;
    request->commands[1].command  = kPS2C_ReadDataPort;
    request->commands[1].inOrOut  = 0;
    request->commands[2].command  = kPS2C_ReadDataPort;
    request->commands[2].inOrOut  = 0;
    request->commands[3].command  = kPS2C_ReadDataPort;
    request->commands[3].inOrOut  = 0;

    request->commandsCount = 4;
    device->submitRequestAndBlock(request);

  //  IOLog("ApplePS2Trackpad: Sentelic FSP: fsp_reg_read(reg = %0x) => %0x\n", reg, request->commands[3].inOrOut);

    return (request->commandsCount == 4) ? request->commands[3].inOrOut : -1;
}

void fsp_reg_write(ApplePS2MouseDevice * device, PS2Request * request, int reg, int val)
{
    int register_select = 0x55;
    int register_value = reg;

    // mangle reg to avoid collision with reserved values
    if (reg == 10 || reg == 20 || reg == 40 || reg == 60 || reg == 80 || reg == 100 || reg == 200) {
        register_value = (reg >> 4) | (reg << 4);
        register_select = 0x77;
    } else if (reg == 0xe9 || reg == 0xee || reg == 0xf2 || reg == 0xff) {
        register_value = ~reg;
        register_select = 0x74;
    }

    fsp_ps2_command(device, request, 0xf3);
    fsp_ps2_command(device, request, register_select);
    fsp_ps2_command(device, request, register_value);

    register_select = 0x33;
    register_value = val;

    // mangle val to avoid collision with reserved values
    if (val == 10 || val == 20 || val == 40 || val == 60 || val == 80 || val == 100 || val == 200) {
        register_value = (val >> 4) | (val << 4);
        register_select = 0x44;
    } else if (val == 0xe9 || val == 0xee || val == 0xf2 || val == 0xff) {
        register_value = ~val;
        register_select = 0x47;
    }

    fsp_ps2_command(device, request, 0xf3);
    fsp_ps2_command(device, request, register_select);
    fsp_ps2_command(device, request, register_value);

//    IOLog("ApplePS2Trackpad: Sentelic FSP: fsp_reg_write(reg = %0x, val = %0x)\n", reg, val);
}

void fsp_write_enable(ApplePS2MouseDevice * device, PS2Request * request, int enable)
{
    int wen = fsp_reg_read(device, request, FSP_REG_SYSCTL1);

    if (enable)
        wen |= FSP_BIT_EN_REG_CLK; 
    else
        wen &= ~FSP_BIT_EN_REG_CLK;

    fsp_reg_write(device, request, FSP_REG_SYSCTL1, wen);
}

void fsp_opctag_enable(ApplePS2MouseDevice * device, PS2Request * request, int enable)
{
    fsp_write_enable(device, request, true);

    int opc = fsp_reg_read(device, request, FSP_REG_OPC_QDOWN);

    if (enable)
        opc |= FSP_BIT_EN_OPC_TAG; 
    else
        opc &= ~FSP_BIT_EN_OPC_TAG;

    fsp_reg_write(device, request, FSP_REG_OPC_QDOWN, opc);

    fsp_write_enable(device, request, false);
}

/*
 测试一下读取REG
 */
int fsp_get_buttons(ApplePS2MouseDevice * device,PS2Request * request)
{
	static const int buttons[] = {
		0x16, /* Left/Middle/Right/Forward/Backward & Scroll Up/Down */
		0x06, /* Left/Middle/Right & Scroll Up/Down/Right/Left */
		0x04, /* Left/Middle/Right & Scroll Up/Down */
		0x02, /* Left/Middle/Right */
	};
	int val=fsp_reg_read(device, request,FSP_REG_TMOD_STATUS1) ;
    
	if (val== -1)
		return -1;
    
	return buttons[(val & 0x30) >> 4];
}
/*
 打开vscr?
 */
 int fsp_onpad_vscr(ApplePS2MouseDevice * device, PS2Request * request ,int enable)
{
  //  TPS2Request<> req;
//    PS2Request * request = &req;
    int val = fsp_reg_read(device,request,FSP_REG_ONPAD_CTL);
	
    
	
	if (enable)
		val |= (FSP_BIT_FIX_VSCR | FSP_BIT_ONPAD_ENABLE);
	else
		val &= (0xff ^ FSP_BIT_FIX_VSCR);
    
	fsp_reg_write(device, request,FSP_REG_ONPAD_CTL, val);
	
	return 0;
}


int fsp_onpad_hscr(ApplePS2MouseDevice *psmouse,PS2Request * request , bool enable)
{
    //TPS2Request<> req;
	//struct fsp_data *pad = psmouse->private;
	int val, v2;
    
	val=fsp_reg_read(psmouse, request, FSP_REG_ONPAD_CTL);
    if(val==-1)
		return -1;
    
	v2 = fsp_reg_read(psmouse,request,FSP_REG_SYSCTL5);
	if(v2)
        return -1;
    
//	pad->hscroll = enable;
    
	if (enable) {
		val |= (FSP_BIT_FIX_HSCR | FSP_BIT_ONPAD_ENABLE);
		v2 |= FSP_BIT_EN_MSID6;
	} else {
		val &= ~FSP_BIT_FIX_HSCR;
		v2 &= ~(FSP_BIT_EN_MSID6 | FSP_BIT_EN_MSID7 | FSP_BIT_EN_MSID8);
	}
    
	fsp_reg_write(psmouse, request,FSP_REG_ONPAD_CTL, val);
	
    
	/* reconfigure horizontal scrolling packet output */
     fsp_reg_write(psmouse,request, FSP_REG_SYSCTL5, v2);
	
    
	return 0;
}

int fsp_onpad_icon(ApplePS2MouseDevice * psmouse,PS2Request * request,int enable)
{
	int val;
  
    /* enable icon switch button and absolute packet */
	val = fsp_reg_read(psmouse, request,FSP_REG_SYSCTL5);
    
	val &= ~(FSP_BIT_EN_MSID7 | FSP_BIT_EN_MSID8 | FSP_BIT_EN_AUTO_MSID8);
    
	if (enable) {
		val |= (FSP_BIT_EN_MSID8 | FSP_BIT_EN_PKT_G0);
		
    }
    
    
    
    
	fsp_reg_write(psmouse,request,FSP_REG_SYSCTL5, val);
	return 0;
}


int fsp_intellimouse_mode(ApplePS2MouseDevice * device, PS2Request * request)
{
    request->commands[0].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[0].inOrOut = kDP_SetMouseSampleRate;
    request->commands[1].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[1].inOrOut = 200;

    request->commands[2].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[2].inOrOut = kDP_SetMouseSampleRate;
    request->commands[3].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[3].inOrOut = 200;

    request->commands[4].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[4].inOrOut = kDP_SetMouseSampleRate;
    request->commands[5].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[5].inOrOut = 80;

    request->commands[6].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[6].inOrOut = kDP_GetId;
    request->commands[7].command = kPS2C_ReadDataPort;
    request->commands[7].inOrOut = 0;

    request->commandsCount = 8;
    device->submitRequestAndBlock(request);

    IOLog("ApplePS2Trackpad: Sentelic FSP: fsp_intellimouse_mode() => %0x\n", request->commands[7].inOrOut);

    return request->commands[7].inOrOut;
}




ScrollSmoother::ScrollSmoother(){
    
}

ScrollSmoother::~ScrollSmoother(){
    
}

double ScrollSmoother::getSpeed(){
    return 0.0;
}

int ScrollSmoother::getDelta(){
    
    return 0;
}

int ScrollSmoother::getDir()
{
    return this->direction;
}


int  ScrollSmoother::filter(int z)
{
    
    if(z>0&& stopDelta>0)
    {
        stopDelta = 0;
    }

    if(cur_history_index>= sizeof(history)/sizeof(int))
    {
        cur_history_index=0;
    }
    history[cur_history_index++] = z;
    
    clock_get_uptime(&lastinputTime);
    
    inputCount ++;
    
    
    
    if(inputCount%2==1)
    {
        if(inputCount>2)
        {
            
            return lastDelta * SCROLL_DELTA_FACTOR;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        int dz = (history[1] + history[0])/2;
        
        if(lastDelta > dz)
            deltaDir = SCROLL_DELTA_DESCREASE;
        else if(lastDelta<dz)
            deltaDir = SCROLL_DELTA_INSCREASE;
        else
            deltaDir = SCROLL_DELTA_EVEN;
        
        if(dz>maxDelta)
        {
            maxDelta  =z;
        }
        
        return dz* SCROLL_DELTA_FACTOR;
    }
    
    
    
    return 0;
}

int  ScrollSmoother::stop(){
    if(lastinputTime!=0)
    {
     //   IOLog("stop delta = %d\n",lastDelta);
        if(inputCount<10)
        {
            stopDelta = maxDelta*SCROLL_DELTA_FACTOR;
        }
        
        decDelta =1;
        lastDelta = 0;
        cur_history_index = 0 ;
        lastinputTime = 0;
        lastspeedCalcTime = 0;
        inputCount = 0;
        maxDelta = 0;
        memset(history, 0, sizeof(history));
        return stopDelta;
    }
    else
    {
        return 0;
    }
    
}

void ScrollSmoother::setDir(int dir)
{
    this->direction  = dir;
}


int  ScrollSmoother::getFlingDelta()
{
    if(stopDelta>0)
    {
        stopDelta = stopDelta*0.6;
        return stopDelta;
      
    }
    return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ApplePS2SentelicFSP* ApplePS2SentelicFSP::probe( IOService * provider, SInt32 * score )
{
    DEBUG_LOG("ApplePS2SentelicFSP::probe entered...\n");
    
    //
    // The driver has been instructed to verify the presence of the actual
    // hardware we represent. We are guaranteed by the controller that the
    // mouse clock is enabled and the mouse itself is disabled (thus it
    // won't send any asynchronous mouse data that may mess up the
    // responses expected by the commands we send it).
    //
    
    ApplePS2MouseDevice* device  = (ApplePS2MouseDevice*)provider;
    
    if (!super::probe(provider, score))
        return 0;
        
    bool success = false;
    TPS2Request<> request;
    if (fsp_reg_read(device, &request, FSP_REG_DEVICE_ID) == FSP_DEVICE_MAGIC)
    {
        _touchPadVersion =
		(fsp_reg_read(device, &request, FSP_REG_VERSION) << 8) |
		(fsp_reg_read(device, &request, FSP_REG_REVISION));
		
        _buttons = fsp_get_buttons(device,&request);
        
        success = true;
    }
	
    DEBUG_LOG("ApplePS2SentelicFSP::probe leaving.\n");
    return (success) ? this : 0;
}


void ApplePS2SentelicFSP::onScrollTimer(void)
{
    int dz = z_avg.getFlingDelta();
    if(dz>0)
    {
        
        uint64_t    now_abs;
        clock_get_uptime(&now_abs);
        switch (z_avg.getDir()) {
            case SCROLL_DIR_UP:
                dispatchScrollWheelEventX(
                                          dz,
                                          0,
                                          0,
                                          now_abs);
                break;
            case SCROLL_DIR_DOWN:
                dispatchScrollWheelEventX(
                                          -dz,
                                          0,
                                          0,
                                          now_abs);
                break;
            case SCROLL_DIR_LEFT:
                dispatchScrollWheelEventX(
                                          0,
                                          dz,
                                          0,
                                          now_abs);
                break;
            case SCROLL_DIR_RIGHT:
                dispatchScrollWheelEventX(
                                          0,
                                          -dz,
                                          0,
                                          now_abs);
                break;
                
            default:
                break;
        }
        
        setTimerTimeout(scrollTimer,momentumscrollInteval);
       
    }
    
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool ApplePS2SentelicFSP::start( IOService * provider )
{ 
    UInt64 gesturesEnabled;
	
    //
    // The driver has been instructed to start. This is called after a
    // successful probe and match.
    //
	
    if (!super::start(provider))
        return false;
	
    //
    // Maintain a pointer to and retain the provider object.
    //
	
    _device = (ApplePS2MouseDevice *) provider;
    _device->retain();
    
    //
    // Enable the mouse clock and disable the mouse IRQ line.
    //

    //
    // Announce hardware properties.
    //
	
    IOLog("ApplePS2Trackpad: Sentelic FSP %d.%d.%d buttons %x\n",
		(_touchPadVersion >> 12) & 0x0F,
		(_touchPadVersion >> 8) & 0x0F,
		(_touchPadVersion) & 0x0F
          ,_buttons);
	
    //
    // Default to 3-byte packets, will try and enable 4-byte packets later
    //

    _packetSize = 3;

    //
    // Advertise the current state of the tapping feature.
    //
	
    gesturesEnabled = (_touchPadModeByte == kModeByteValueGesturesEnabled) ? 1 : 0;
    setProperty("Clicking", gesturesEnabled, sizeof(gesturesEnabled)*8);
	
    //
    // Must add this property to let our superclass know that it should handle
    // trackpad acceleration settings from user space.  Without this, tracking
    // speed adjustments from the mouse prefs panel have no effect.
    //
	
    setProperty(kIOHIDPointerAccelerationTypeKey, kIOHIDTrackpadAccelerationType);
    setProperty(kIOHIDScrollAccelerationTypeKey, kIOHIDTrackpadScrollAccelerationKey);
	setProperty(kIOHIDScrollResolutionKey, _scrollresolution, 32);
    
    
    IOWorkLoop* pWorkLoop = getWorkLoop();
    if (!pWorkLoop )
    {
        _device->release();
        return false;
    }
    scrollTimer = IOTimerEventSource::timerEventSource(this, OSMemberFunctionCast(IOTimerEventSource::Action, this,
                                                                                  
                                                                                  &ApplePS2SentelicFSP::onScrollTimer));
    if (scrollTimer)
        pWorkLoop->addEventSource(scrollTimer);
    
    
    
	
    //
    // Lock the controller during initialization
    //
    
    _device->lock();
    
    
    /* Ensure we are not in absolute mode */
   // fsp_onpad_icon(_device, 0);
    
    //
    // Finally, we enable the trackpad itself, so that it may start reporting
    // asynchronous events.
    //
	
    setTouchPadEnable(true);
	
   
    
    //
    // Install our driver's interrupt handler, for asynchronous data delivery.
    //
	
    _device->installInterruptAction(this,
                                    OSMemberFunctionCast(PS2InterruptAction, this, &ApplePS2SentelicFSP::interruptOccurred),
                                    OSMemberFunctionCast(PS2PacketAction, this, &ApplePS2SentelicFSP::packetReady));
    _interruptHandlerInstalled = true;
	
    // now safe to allow other threads
    _device->unlock();
    
    //
    // Install our power control handler.
    //
	
    _device->installPowerControlAction( this, OSMemberFunctionCast(PS2PowerControlAction, this, &ApplePS2SentelicFSP::setDevicePowerState));
    _powerControlHandlerInstalled = true;
    
    IOLog("ApplePS2Trackpad: Sentelic gesture enbaled %lld\n",
        gesturesEnabled);
    _device->installMessageAction( this,
                                  OSMemberFunctionCast(PS2MessageAction, this, &ApplePS2SentelicFSP::receiveMessage));
    _messageHandlerInstalled = true;
    
    return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ApplePS2SentelicFSP::stop( IOService * provider )
{
    //
    // The driver has been instructed to stop.  Note that we must break all
    // connections to other service objects now (ie. no registered actions,
    // no pointers and retains to objects, etc), if any.
    //
	
    assert(_device == provider);
	
    //
    // Disable the mouse itself, so that it may stop reporting mouse events.
    //
	
    setTouchPadEnable(false);
	
    //
    // Uninstall the interrupt handler.
    //
	
    if ( _interruptHandlerInstalled )  _device->uninstallInterruptAction();
    _interruptHandlerInstalled = false;
	
    //
    // Uninstall the power control handler.
    //
	
    if ( _powerControlHandlerInstalled ) _device->uninstallPowerControlAction();
    _powerControlHandlerInstalled = false;
	
    if(_messageHandlerInstalled)
        _device->uninstallMessageAction();
    _messageHandlerInstalled = false;
    
    IOWorkLoop* pWorkLoop = getWorkLoop();
    if (pWorkLoop)
    {
        if (scrollTimer)
        {
            pWorkLoop->removeEventSource(scrollTimer);
            scrollTimer->release();
            scrollTimer = 0;
        }
    }
    
    //
    // Release the pointer to the provider object.
    //
	
    OSSafeReleaseNULL(_device);
	
	super::stop(provider);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

PS2InterruptResult ApplePS2SentelicFSP::interruptOccurred( UInt8 data )
{
    //
    // This will be invoked automatically from our device when asynchronous
    // events need to be delivered. Process the trackpad data. Do NOT issue
    // any BLOCKING commands to our device in this context.
    //
    // Ignore all bytes until we see the start of a packet, otherwise the
    // packets may get out of sequence and things will get very confusing.
    //
    if (_packetByteCount == 0 && ((data == kSC_Acknowledge) || !(data & 0x08)))
    {
        DEBUG_LOG("%s: Unexpected byte0 data (%02x) from PS/2 controller\n", getName(), data);
        return kPS2IR_packetBuffering;
    }
	
    //
    // Add this byte to the packet buffer. If the packet is complete, that is,
    // we have the three bytes, dispatch this packet for processing.
    //
	
    UInt8* packet = _ringBuffer.head();
    packet[_packetByteCount++] = data;
    if (_packetByteCount == _packetSize)
    {
        _ringBuffer.advanceHead(kPacketLengthMax);
        _packetByteCount = 0;
        return kPS2IR_packetReady;
    }
    return kPS2IR_packetBuffering;
}

void ApplePS2SentelicFSP::packetReady()
{
    // empty the ring buffer, dispatching each packet...
    while (_ringBuffer.count() >= kPacketLengthMax)
    {
        dispatchRelativePointerEventWithPacket(_ringBuffer.tail(), _packetSize);
        _ringBuffer.advanceTail(kPacketLengthMax);
    }
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ApplePS2SentelicFSP::dispatchRelativePointerEventWithPacket(UInt8* packet, UInt32 packetSize)
{
      
    UInt32      buttons = 0;
    SInt32      dx, dy, dz;
    uint64_t    now_abs;
    clock_get_uptime(&now_abs);
    uint64_t now_ns;
    absolutetime_to_nanoseconds(now_abs, &now_ns);
    
    int abs_x = 0, abs_y = 0;
    int lb=0,rb=0,mb=0,fi=0,opc=0,cm=0;
    int sl=0,sr=0,b4=0,b5=0;

    
    
    if(1)
    {
    
        //fsp_packet_debug(packet,packetSize);
//        return;
        
        
        if(now_ns-keytime < maxaftertyping)
            return;
        
        switch (packet[0] >> FSP_PKT_TYPE_SHIFT) {
            case FSP_PKT_TYPE_NOTIFY:
//                dev_warn(&psmouse->ps2dev.serio->dev,
//                         "Unexpected gesture packet, ignored.\n");
               
                if(MFMT_LEFT_BTN_DOWN & packet[0]) lb=1;
                if(MFMT_RIGHT_BTN_DOWN &packet[0]) rb =1;
                if(MFMT_MID_BTN_DOWN&packet[0]) mb=1;
                if(MFMT_LEFT_BTN_OPC&packet[0]) opc=1;
                
                
                if(packet[1]==FSP_CX_NOTIFY_MSG_TYPE_GUESTURE)
                {
                    
                    _last_abs_x = 0;
                    _last_abs_y = 0;
                  
                    
                   
                    if(_isInGesture)
                    {
                        dz  =  packet[3] - _last_abs_z;
                    }                    
                    else
                    {
                        dz  =  packet[3] ;
                    }
                    _last_abs_z  = packet[3] ;
                    
                    
                    switch (packet[2]) {
                        case	0x86:
                              _isInGesture=true;
                            z_avg.setDir(SCROLL_DIR_UP);
                            dz = z_avg.filter(dz);
                            //IOLog("	2 finger straight up             \n");
                            if(dz>0)
                                dispatchScrollWheelEventX(
                                                      dz,
                                                      0,
                                                      0,
                                                      now_abs);
                            break;
                        case	0x82:
                            
                            _isInGesture=true;
                            z_avg.setDir(SCROLL_DIR_DOWN);
                            dz = z_avg.filter(dz);
                                //IOLog("	2 finger straight down           \n");
                            if(dz>0)
                                dispatchScrollWheelEventX(
                                                      -dz,
                                                      0,
                                                      0,
                                                      now_abs);
                            break;
                        case	0x80:
                              _isInGesture=true;
                            z_avg.setDir(SCROLL_DIR_RIGHT);
                            dz = z_avg.filter(dz);
                            //IOLog("	2 finger straight right          \n");break;
                              //_device->dispatchKeyboardMessage(kPS2M_swipeUp, &now_abs);
                            if(dz>0)
                                dispatchScrollWheelEventX(
                                                      0,
                                                      dz,
                                                      0,
                                                      now_abs);
                            break;
                        case	0x84:
                              _isInGesture=true;
                            z_avg.setDir(SCROLL_DIR_LEFT);
                            dz = z_avg.filter(dz);
                            //IOLog("	2 finger straight left           \n");break;
                              //_device->dispatchKeyboardMessage(kPS2M_swipeDown, &now_abs);
                            if(dz>0)
                                dispatchScrollWheelEventX(
                                                      0,
                                                      -dz,
                                                      0,
                                                      now_abs);
                            break;
                        case	0x8f:
                            //IOLog("	2 finger zoom in                 \n");break;
                            if(!_isInGesture)
                            {
                                _isInGesture=true;
                                _device->dispatchKeyboardMessage(kPS2M_zoomIn, &now_abs);
                            }
                        case	0x8b:
                            //IOLog("	2 finger zoom out                \n");break;
                            if(!_isInGesture)
                            {
                                _isInGesture=true;
                                _device->dispatchKeyboardMessage(kPS2M_zoomOut, &now_abs);
                            }
                        case	0xc0:
                            //IOLog("	2 finger curve, counter clockwise");break;
                            if(!_isInGesture)
                            {
                                _isInGesture=true;
                                _device->dispatchKeyboardMessage(kPS2M_rotateL, &now_abs);
                            }
                        case	0xc4:
                            //IOLog("	2 finger curve, clockwise        \n");break;
                            if(!_isInGesture)
                            {
                                _isInGesture=true;
                                _device->dispatchKeyboardMessage(kPS2M_rotateR, &now_abs);
                            }
                        case	0x2e:
                            //IOLog("	3 finger straight up             \n");break;
                            if(!_isInGesture)
                            {
                                _isInGesture=true;
                                _device->dispatchKeyboardMessage(kPS2M_swipeUp, &now_abs);
                            }
                        case	0x2a:
                            if(!_isInGesture)
                            {
                                //IOLog("	3 finger straight down           \n");break;
                                 _isInGesture=true;
                                _device->dispatchKeyboardMessage(kPS2M_swipeDown, &now_abs);
                            }
                        case	0x28:
                            //IOLog("	3 finger straight right          \n");break;
                            if(!_isInGesture)
                            {
                                _isInGesture=true;
                               _device->dispatchKeyboardMessage(kPS2M_swipeRight, &now_abs);
                            }
                        case	0x2c:
                            //IOLog("	3 finger straight left           \n");break;
                            if(!_isInGesture)
                            {
                               _isInGesture=true;
                               _device->dispatchKeyboardMessage(kPS2M_swipeLeft, &now_abs);
                            }
                        case 0x18:
                            //3 fingers quick click ?
                            break;
                        case 0x19:
                            //3 fingers quick click

                            break;
                            
                        case 0x11:
                            //2 finger click
                            dispatchRelativePointerEventX(0, 0, 0x2
                                                          // right button  (bit 1 in packet)
                                                          , now_abs);
                            dispatchRelativePointerEventX(0, 0, 0
                                                          // right button  (bit 1 in packet)
                                                          , now_abs+1);
                            break;
                            
                        case 0x1a:
                            //3 fingers double click
                            if(!_isInGesture)
                            {
                                 _isInGesture=true;
                                 _device->dispatchKeyboardMessage(kPS2M_lauchPad, &now_abs);
                            }
                            break;
                        case	0x38:
                            //IOLog("	palm                             \n");
                           
                            break;
                        case 0:
                            _isInGesture = false;
                            
                            if(z_avg.stop()>0)
                            {
                                setTimerTimeout(scrollTimer,momentumscrollInteval);
                            }
                            gestureStopTime =now_ns;
                            break;
                        default:
                            //IOLog("	unkonw Guesture id :%x                            ",gestureId);
                           // guesting = false;
                         
                            break;
                    }
                    

                }
                else if(packet[1]==FSP_CX_NOTIFY_MSG_TYPE_ONE_FINGER_HOLD)
                {
                     _isInGesture = false;
                    gestureStopTime =now_ns;
                    IOLog("FSP_CX_NOTIFY_MSG_TYPE_ONE_FINGER_HOLD\n");
                }
                else
                {
                    _isInGesture = false;
                    gestureStopTime =now_ns;
                    IOLog("Unexpected gesture packet, ignored.\n");
                }
                break;
                
            case FSP_PKT_TYPE_ABS:
                /* Absolute packets are sent with version Cx and newer
                    * touchpads if register 0x90 bit 0 is set.
                    */
                abs_x = (packet[1] << 2) | ((packet[3] >> 2) & 0x03);
                abs_y = (packet[2] << 2) | (packet[3] & 0x03);
                
                if(MFMT_LEFT_BTN_DOWN & packet[0]) lb=1;
                if(MFMT_RIGHT_BTN_DOWN &packet[0]) rb =1;
                if(MFMT_FINGER_INDEX&packet[0]) fi=1;
                if(MFMT_LEFT_BTN_OPC&packet[0]) opc=1;
                if(MFMT_COORD_MODE&packet[0]) cm=1;
                
                if(MFMT_4TH_BTN&packet[3])b4=1;
                if(MFMT_5TH_BTN&packet[3])b5=1;
                if(MFMT_SCROLL_LEFT_BTN&packet[3])sl=1;
                if(MFMT_SCROLL_RIGHT_BTN&packet[3]) sr =1;
                
                if(_isInGesture)
                {
                    if(abs_y==0 && abs_x==0)
                    {
                        _isInGesture = false;
                        gestureStopTime =now_ns;
                        _last_abs_x = 0;
                        _last_abs_y = 0;
                        x_avg.reset();
                        y_avg.reset();
                        if(z_avg.stop()>0)
                        {
                            setTimerTimeout(scrollTimer,momentumscrollInteval);
                        }
                    }
                    return;
                }
                 
                if(now_ns-gestureStopTime < scrolllockTime)
                {
                    return;
                }
                   
                if ( lb) buttons |= 0x1;  // left button   (bit 0 in packet)
                if ( rb ) buttons |= 0x2;  // right button  (bit 1 in packet)
                if(!lb && !opc)
                {
                    buttons |= 0x1;  // onpad click
                }
                
                if(abs_y==0 && abs_x==0)
                {
                    dx = 0;
                    dy = 0;
                    x_avg.reset();
                    y_avg.reset();
                    
                }
                else if(_last_abs_x==0 && _last_abs_y==0)
                {
                    dx = 0;
                    dy = 0;
                }
                else
                {
                    
                    abs_x = x_avg.filter(abs_x);
                    abs_y = y_avg.filter(abs_y);
                    
                    dx = abs_x - _last_abs_x;
                    dy = abs_y - _last_abs_y;
                }
                
                dispatchRelativePointerEventX(dx, dy, buttons, now_abs);
                
                
                _last_abs_x = abs_x;
                _last_abs_y = abs_y;
                
                break;
                
            case FSP_PKT_TYPE_NORMAL_OPC:
                /* on-pad click, filter it if necessary */
//                if ((ad->flags & FSPDRV_FLAG_EN_OPC) != FSPDRV_FLAG_EN_OPC)
//                    packet[0] &= ~0x01;
                /* fall through */
                
            case FSP_PKT_TYPE_NORMAL:
                /* normal packet */
                
                
                if ((_touchPadModeByte == kModeByteValueGesturesEnabled) ||         // pad clicking enabled
                    (packet[0] >> FSP_PKT_TYPE_SHIFT) != FSP_PKT_TYPE_NORMAL_OPC)   // real button
                {
                    if ( (packet[0] & 0x1) ) buttons |= 0x1;  // left button   (bit 0 in packet)
                    if ( (packet[0] & 0x2) ) buttons |= 0x2;  // right button  (bit 1 in packet)
                    if ( (packet[0] & 0x4) ) buttons |= 0x4;  // middle button (bit 2 in packet)
                }
                
                dx = ((packet[0] & 0x10) ? 0xffffff00 : 0 ) | packet[1];
                dy = -(((packet[0] & 0x20) ? 0xffffff00 : 0 ) | packet[2]);
                
                
                dispatchRelativePointerEventX(dx, dy, buttons, now_abs);
                
                break;
        }
        return;
    }
    
    //
    // Process the three byte relative format packet that was retreived from the
    // trackpad. The format of the bytes is as follows:
    //
    //  7  6  5  4  3  2  1  0
    // -----------------------
    // YO XO YS XS  1  M  R  L
    // X7 X6 X5 X4 X3 X3 X1 X0  (X delta)
    // Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0  (Y delta)
    // Z7 Z6 Z5 Z4 Z3 Z2 Z1 Z0  (Z delta) (iff 4 byte packets)
    //
	
   
	
//    if ((_touchPadModeByte == kModeByteValueGesturesEnabled) ||         // pad clicking enabled
//        (packet[0] >> FSP_PKT_TYPE_SHIFT) != FSP_PKT_TYPE_NORMAL_OPC)   // real button
//    {
//		if ( (packet[0] & 0x1) ) buttons |= 0x1;  // left button   (bit 0 in packet)
//		if ( (packet[0] & 0x2) ) buttons |= 0x2;  // right button  (bit 1 in packet)
//		if ( (packet[0] & 0x4) ) buttons |= 0x4;  // middle button (bit 2 in packet)
//    }
//    
//    dx = ((packet[0] & 0x10) ? 0xffffff00 : 0 ) | packet[1];
//    dy = -(((packet[0] & 0x20) ? 0xffffff00 : 0 ) | packet[2]);
//    
//   
//    dispatchRelativePointerEventX(dx, dy, buttons, now_abs);
//
//    if (packetSize == 4)
//    {
//        
//        dz = (int)(packet[3] & 8) - (int)(packet[3] & 7);
//      //  dispatchScrollWheelEventX(dz, 0, 0, now_abs);
//    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ApplePS2SentelicFSP::setTouchPadEnable( bool enable )
{
    //
    // Instructs the trackpad to start or stop the reporting of data packets.
    // It is safe to issue this request from the interrupt/completion context.
    //
	
    // (mouse enable/disable command)
    TPS2Request<> request;
    request.commands[0].command = kPS2C_SendMouseCommandAndCompareAck;
    request.commands[0].inOrOut =  enable ? kDP_Enable : kDP_SetDefaultsAndDisable;
    request.commandsCount = 1;
    assert(request.commandsCount <= countof(request.commands));
    _device->submitRequestAndBlock(&request);
	
    // enable one-pad-click tagging, so we can filter them out!
    fsp_opctag_enable(_device, &request, true);
    fsp_onpad_icon(_device, &request, false);
    int val = FSP_CX_ABSOLUTE_MODE |
    FSP_CX_GESTURE_OUTPUT|
    FSP_CX_2FINGERS_OUTPUT |
    FSP_CX_FINGER_UP_OUTPUT |
   FSP_CX_CONTINUOUS_MODE|
    FSP_CX_GUEST_GROUP_BIT1|
    FSP_CX_GUEST_GROUP_BIT2;
    
    fsp_reg_write(_device, &request, FSP_REG_SWREG1, val);
    
    int fsprt = fsp_intellimouse_mode(_device, &request);
    IOLog("fsp_intellimouse_mode retrun %d\n",fsprt);
    // turn on intellimouse mode (4 bytes per packet)
    if (fsprt== 4) 
    {
        
        /* Enable absolute positioning, two finger mode and continuous output
         * on Cx and newer pads (version ID 0xE0+)
         */
      // if (pad->ver >= 0xE0) {
       // fsp_onpad_icon
       
        
        _packetSize = 4;
    }
    
  
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UInt32 ApplePS2SentelicFSP::getTouchPadData( UInt8 dataSelector )
{
    TPS2Request<13> request;
    
    // Disable stream mode before the command sequence.
    request.commands[0].command  = kPS2C_SendMouseCommandAndCompareAck;
    request.commands[0].inOrOut  = kDP_SetDefaultsAndDisable;
	
    // 4 set resolution commands, each encode 2 data bits.
    request.commands[1].command  = kPS2C_SendMouseCommandAndCompareAck;
    request.commands[1].inOrOut  = kDP_SetMouseResolution;
    request.commands[2].command  = kPS2C_SendMouseCommandAndCompareAck;
    request.commands[2].inOrOut  = (dataSelector >> 6) & 0x3;
	
    request.commands[3].command  = kPS2C_SendMouseCommandAndCompareAck;
    request.commands[3].inOrOut  = kDP_SetMouseResolution;
    request.commands[4].command  = kPS2C_SendMouseCommandAndCompareAck;
    request.commands[4].inOrOut  = (dataSelector >> 4) & 0x3;
	
    request.commands[5].command  = kPS2C_SendMouseCommandAndCompareAck;
    request.commands[5].inOrOut  = kDP_SetMouseResolution;
    request.commands[6].command  = kPS2C_SendMouseCommandAndCompareAck;
    request.commands[6].inOrOut  = (dataSelector >> 2) & 0x3;
	
    request.commands[7].command  = kPS2C_SendMouseCommandAndCompareAck;
    request.commands[7].inOrOut  = kDP_SetMouseResolution;
    request.commands[8].command  = kPS2C_SendMouseCommandAndCompareAck;
    request.commands[8].inOrOut  = (dataSelector >> 0) & 0x3;
	
    // Read response bytes.
    request.commands[9].command  = kPS2C_SendMouseCommandAndCompareAck;
    request.commands[9].inOrOut  = kDP_GetMouseInformation;
    request.commands[10].command = kPS2C_ReadDataPort;
    request.commands[10].inOrOut = 0;
    request.commands[11].command = kPS2C_ReadDataPort;
    request.commands[11].inOrOut = 0;
    request.commands[12].command = kPS2C_ReadDataPort;
    request.commands[12].inOrOut = 0;
    request.commandsCount = 13;
    assert(request.commandsCount <= countof(request.commands));
    _device->submitRequestAndBlock(&request);
	
    UInt32 returnValue = (UInt32)(-1);
    if (request.commandsCount == 13) // success?
    {
        returnValue = ((UInt32)request.commands[10].inOrOut << 16) |
		((UInt32)request.commands[11].inOrOut <<  8) |
		((UInt32)request.commands[12].inOrOut);
    }
    return returnValue;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

IOReturn ApplePS2SentelicFSP::setParamProperties( OSDictionary * dict )
{
    
    OSNumber * clicking = OSDynamicCast( OSNumber, dict->getObject("Clicking") );
	
    if ( clicking )
    {
        clicking->setValue(1l);
        UInt8  newModeByteValue = clicking->unsigned32BitValue() & 0x1 ?
		kModeByteValueGesturesEnabled :
		kModeByteValueGesturesDisabled;
		
        if (_touchPadModeByte != newModeByteValue)
        {
            _touchPadModeByte = newModeByteValue;
			
            //
            // Advertise the current state of the tapping feature.
            //
			
            setProperty("Clicking", clicking);
        }
        IOLog("ApplePS2Trackpad: Sentelic FSP: setParamProperties newModeByteValue %0x\n", newModeByteValue);

    }
	
    return super::setParamProperties(dict);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ApplePS2SentelicFSP::setDevicePowerState( UInt32 whatToDo )
{
    switch ( whatToDo )
    {
        case kPS2C_DisableDevice:
            
            //
            // Disable touchpad (synchronous).
            //
			
            setTouchPadEnable( false );
            break;
			
        case kPS2C_EnableDevice:
			
            //
            // Must not issue any commands before the device has
            // completed its power-on self-test and calibration.
            //
			
            IOSleep(1000);
			
            //
            // Clear packet buffer pointer to avoid issues caused by
            // stale packet fragments.
            //
			
            _packetByteCount = 0;
            _ringBuffer.reset();
			
            //
            // Finally, we enable the trackpad itself, so that it may
            // start reporting asynchronous events.
            //
			
            setTouchPadEnable( true );
            break;
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool ApplePS2SentelicFSP::setTouchPadModeByte(UInt8 modeByteValue, bool enableStreamMode)
{
    TPS2Request<12> request;
    
    // Disable stream mode before the command sequence.
    request.commands[0].command  = kPS2C_SendMouseCommandAndCompareAck;
    request.commands[0].inOrOut  = kDP_SetDefaultsAndDisable;
	
    // 4 set resolution commands, each encode 2 data bits.
    request.commands[1].command  = kPS2C_SendMouseCommandAndCompareAck;
    request.commands[1].inOrOut  = kDP_SetMouseResolution;
    request.commands[2].command  = kPS2C_SendMouseCommandAndCompareAck;
    request.commands[2].inOrOut  = (modeByteValue >> 6) & 0x3;
	
    request.commands[3].command  = kPS2C_SendMouseCommandAndCompareAck;
    request.commands[3].inOrOut  = kDP_SetMouseResolution;
    request.commands[4].command  = kPS2C_SendMouseCommandAndCompareAck;
    request.commands[4].inOrOut  = (modeByteValue >> 4) & 0x3;
	
    request.commands[5].command  = kPS2C_SendMouseCommandAndCompareAck;
    request.commands[5].inOrOut  = kDP_SetMouseResolution;
    request.commands[6].command  = kPS2C_SendMouseCommandAndCompareAck;
    request.commands[6].inOrOut  = (modeByteValue >> 2) & 0x3;
	
    request.commands[7].command  = kPS2C_SendMouseCommandAndCompareAck;
    request.commands[7].inOrOut  = kDP_SetMouseResolution;
    request.commands[8].command  = kPS2C_SendMouseCommandAndCompareAck;
    request.commands[8].inOrOut  = (modeByteValue >> 0) & 0x3;
	
    // Set sample rate 20 to set mode byte 2. Older pads have 4 mode
    // bytes (0,1,2,3), but only mode byte 2 remain in modern pads.
    request.commands[9].command  = kPS2C_SendMouseCommandAndCompareAck;
    request.commands[9].inOrOut  = kDP_SetMouseSampleRate;
    request.commands[10].command = kPS2C_SendMouseCommandAndCompareAck;
    request.commands[10].inOrOut = 20;
	
    request.commands[11].command  = kPS2C_SendMouseCommandAndCompareAck;
    request.commands[11].inOrOut  = enableStreamMode ? kDP_Enable : kDP_SetMouseScaling1To1;
    request.commandsCount = 12;
    assert(request.commandsCount <= countof(request.commands));
    
    _device->submitRequestAndBlock(&request);
    return 12 == request.commandsCount;
}




void ApplePS2SentelicFSP::receiveMessage(int message, void* data)
{
    //
    // Here is where we receive messages from the keyboard driver
    //
    // This allows for the keyboard driver to enable/disable the trackpad
    // when a certain keycode is pressed.
    //
    // It also allows the trackpad driver to learn the last time a key
    //  has been pressed, so it can implement various "ignore trackpad
    //  input while typing" options.
    //
    switch (message)
    {
        case kPS2M_getDisableTouchpad:
        {
//            bool* pResult = (bool*)data;
//            *pResult = !ignoreall;
            break;
        }
            
        case kPS2M_setDisableTouchpad:
        {
//            bool enable = *((bool*)data);
//            // ignoreall is true when trackpad has been disabled
//            if (enable == ignoreall)
//            {
//                // save state, and update LED
//                ignoreall = !enable;
//                updateTouchpadLED();
//            }
            break;
        }
            
        case kPS2M_notifyKeyPressed:
        {
            // just remember last time key pressed... this can be used in
            // interrupt handler to detect unintended input while typing
            PS2KeyInfo* pInfo = (PS2KeyInfo*)data;
            static const int masks[] =
            {
                0x10,       // 0x36
                0x100000,   // 0x37
                0,          // 0x38
                0,          // 0x39
                0x080000,   // 0x3a
                0x040000,   // 0x3b
                0,          // 0x3c
                0x08,       // 0x3d
                0x04,       // 0x3e
                0x200000,   // 0x3f
            };

            switch (pInfo->adbKeyCode)
            {
                    // don't store key time for modifier keys going down
                    // track modifiers for scrollzoom feature...
                    // (note: it turns out we didn't need to do this, but leaving this code in for now in case it is useful)
                case 0x38:  // left shift
                case 0x3c:  // right shift
                case 0x3b:  // left control
                case 0x3e:  // right control
                case 0x3a:  // left windows (option)
                case 0x3d:  // right windows
                case 0x37:  // left alt (command)
                case 0x36:  // right alt
                case 0x3f:  // osx fn (function)
                    if (pInfo->goingDown)
                    {
                        _modifierdown |= masks[pInfo->adbKeyCode-0x36];
                        break;
                        
                    }
                    _modifierdown &= ~masks[pInfo->adbKeyCode-0x36];
                    keytime = pInfo->time;
                    break;
                    
                default:
                    gestureStopTime = 0;  // keys cancel momentum scroll
                    keytime = pInfo->time;
            }
            break;
        }
    }
}
