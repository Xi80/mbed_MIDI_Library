#ifndef H_MIDI
#define H_MIDI
#include "mbed.h"
#include "circularBuffer.h"
#define USBDEBUG

/**
 * @file midi.h
 * @brief MIDI library for mbed
 * @author Luna Tsukiyono
 * @date 2020/06/19
 */

const  uint8_t gmSystemOn[] = {0x7E,0x7F,0x09,0x01};
const  uint8_t xgSystemOn[] = {0x43,0x10,0x4C,0x00,0x00,0x7E,0x00};
const  uint8_t gsReset[]    = {0x41,0x10,0x42,0x12,0x40,0x00,0x7F,0x00,0x41};

class midi{
    public:
        midi(PinName tx,PinName rx);
		
		void init(void);

        void setCallbackNoteOn(void (*func)(uint8_t,uint8_t,uint8_t));
        void setCallbackNoteOff(void (*func)(uint8_t,uint8_t));
        void setCallbackControlChange(void (*func)(uint8_t,uint8_t,uint8_t));
        void setCallbackProgramChange(void (*func)(uint8_t,uint8_t));
        void setCallbackReset(void (*func)(void));
        void setCallbackPitchBend(void (*func)(uint8_t,unsigned short));
		void setCallbackSystemExclusive(void (*func)(uint8_t*,uint8_t));

        void midiParse(void);
    private:
		Serial serial;

        lunaLib::CircularBuffer<uint8_t,1024> buffer;

        uint8_t sysExBuffer[128];
		uint8_t sysExBufferPos = 0;

        void (*noteOnFunc)(uint8_t,uint8_t,uint8_t);
        void (*noteOffFunc)(uint8_t,uint8_t);
        void (*controlChangeFunc)(uint8_t,uint8_t,uint8_t);
        void (*pitchBendFunc)(uint8_t,unsigned short);
        void (*programChangeFunc)(uint8_t,uint8_t);
        void (*resetFunc)(void);
		void (*sysExFunc)(uint8_t*,uint8_t);

        void getData(void);
        void decodeCommand(void);
        void decodeSysEx(void);
        
        uint8_t decodeResets(void);

        uint8_t runningStatusCommand = 0x00;
        uint8_t runningStatusChannel = 0x00;

        bool isSysEx = false;
        
        bool threeBytesFlag = false;

        uint8_t decodeByte  = 0x00;
        uint8_t secondByte  = 0x00;
        uint8_t thirdByte   = 0x00;

        const static uint8_t noteOn         = 0x90;
        const static uint8_t noteOff        = 0x80;
        const static uint8_t controlChange  = 0xB0;
        const static uint8_t programChange  = 0xC0;
        const static uint8_t pitchBend      = 0xE0;
        const static uint8_t beginSysEx     = 0xF0;
        const static uint8_t endSysEx       = 0xF7;
};

#endif