/*
 * Copyright (c) 2002 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.2 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
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

#ifndef _APPLEPS2SENTILICSFSP_H
#define _APPLEPS2SENTILICSFSP_H

#include "ApplePS2MouseDevice.h"
#include <IOKit/hidsystem/IOHIPointing.h>




#define kPacketLengthMax          4
#define kPacketLengthStandard     3
#define kPacketLengthLarge        4




template <class T, int N>
class SimpleAverage2
{
private:
    T m_buffer[N];
    int m_count;
    int m_sum;
    int m_index;
    
public:
    inline SimpleAverage2() { reset(); }
    T filter(T data)
    {
        // add new entry to sum
        m_sum += data;
        // if full buffer, then we are overwriting, so subtract old from sum
        if (m_count == N)
            m_sum -= m_buffer[m_index];
        // new entry into buffer
        m_buffer[m_index] = data;
        // move index to next position with wrap around
        if (++m_index >= N)
            m_index = 0;
        // keep count moving until buffer is full
        if (m_count < N)
            ++m_count;
        // return average of current items
        return m_sum / m_count;
    }
    inline void reset()
    {
        m_count = 0;
        m_sum = 0;
        m_index = 0;
    }
    inline int count() { return m_count; }
    inline int sum() { return m_sum; }
    T oldest()
    {
        // undefined if nothing in here, return zero
        if (m_count == 0)
            return 0;
        // if it is not full, oldest is at index 0
        // if full, it is right where the next one goes
        if (m_count < N)
            return m_buffer[0];
        else
            return m_buffer[m_index];
    }
    T newest()
    {
        // undefined if nothing in here, return zero
        if (m_count == 0)
            return 0;
        // newest is index - 1, with wrap
        int index = m_index;
        if (--index < 0)
            index = m_count-1;
        return m_buffer[index];
    }
    T average()
    {
        if (m_count == 0)
            return 0;
        return m_sum / m_count;
    }
};


#define SCROLL_DIR_UP 1
#define SCROLL_DIR_DOWN 2
#define SCROLL_DIR_LEFT 3
#define SCROLL_DIR_RIGHT 4

#define SCROLL_DELTA_EVEN 0
#define SCROLL_DELTA_INSCREASE 1
#define SCROLL_DELTA_DESCREASE 2


#define SCROLL_DELTA_FACTOR 32

class ScrollSmoother
{
private:
    uint64_t lastinputTime;
    uint64_t lastspeedCalcTime;
    int  direction; //0 up, 1 down, 2 , left, 3 right
    int  history[2];
    int  lastDelta;
    int  cur_history_index;
    int  deltaDir; // 0- verage 1->increase  2- decress
    int  inputCount;
    int  stopDelta;
    int  decDelta;
    int  maxDelta;
public:
    ScrollSmoother();
    ~ScrollSmoother();
    double getSpeed();
    int   getDelta();
    int   getDir();
    void  setDir(int dir);
    int   filter(int dz);
    int   stop();
    int   getFlingDelta();
    
};





// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// ApplePS2SentelicFSP Class Declaration
//

class EXPORT ApplePS2SentelicFSP : public IOHIPointing
{
    typedef IOHIPointing super;
    OSDeclareDefaultStructors( ApplePS2SentelicFSP );
    
private:
    ApplePS2MouseDevice * _device;
    bool                  _interruptHandlerInstalled;
    bool                  _powerControlHandlerInstalled;
    bool                _messageHandlerInstalled;
    RingBuffer<UInt8, kPacketLengthMax*32> _ringBuffer;
    UInt32                _packetByteCount;
    UInt8                 _packetSize;
    IOFixed               _resolution;
    IOFixed                 _scrollresolution;
    UInt16                _touchPadVersion;
    UInt8                 _touchPadModeByte;
    UInt8                 _buttons;
    
    
    int _modifierdown; // state of left+right control keys
    int                     _last_abs_x;
    int                     _last_abs_y;
    int                     _last_abs_z;
    bool                    _isInGesture;
    
    
    SimpleAverage2<int, 3> x_avg;
    SimpleAverage2<int, 3> y_avg;
    ScrollSmoother           z_avg;
    uint64_t keytime;
    uint64_t gestureStopTime;
    uint64_t momentumscrollInteval;
    uint64_t maxaftertyping ;
    uint64_t  scrolllockTime;
    IOTimerEventSource* scrollTimer;
    virtual void   dispatchRelativePointerEventWithPacket( UInt8 * packet, UInt32  packetSize ); 
    
    virtual void   setTouchPadEnable( bool enable );
    virtual UInt32 getTouchPadData( UInt8 dataSelector );
    virtual bool   setTouchPadModeByte( UInt8 modeByteValue,
                                       bool  enableStreamMode = false );
    
    virtual PS2InterruptResult interruptOccurred(UInt8 data);
    virtual void packetReady();
    virtual void   setDevicePowerState(UInt32 whatToDo);
    virtual void   receiveMessage(int message, void* data);
protected:
    virtual IOItemCount buttonCount();
    virtual IOFixed     resolution();
    
    inline void dispatchRelativePointerEventX(int dx, int dy, UInt32 buttonState, uint64_t now)
        { dispatchRelativePointerEvent(dx, dy, buttonState, *(AbsoluteTime*)&now); }
    inline void dispatchScrollWheelEventX(short deltaAxis1, short deltaAxis2, short deltaAxis3, uint64_t now)
        { dispatchScrollWheelEvent(deltaAxis1, deltaAxis2, deltaAxis3, *(AbsoluteTime*)&now); }
    inline void setTimerTimeout(IOTimerEventSource* timer, uint64_t time)
    { timer->setTimeout(*(AbsoluteTime*)&time); }
    
public:
    virtual bool init( OSDictionary * properties );
    virtual ApplePS2SentelicFSP * probe( IOService * provider,
                                              SInt32 *    score );
    
    virtual bool start( IOService * provider );
    virtual void stop( IOService * provider );
    
    virtual UInt32 deviceType();
    virtual UInt32 interfaceID();
    void onScrollTimer(void);
    virtual IOReturn setParamProperties( OSDictionary * dict );
    
    
};

#endif /* _APPLEPS2SENTILICSFSP_H */
