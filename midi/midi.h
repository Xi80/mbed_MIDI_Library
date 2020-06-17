#ifndef H_MIDI
#define H_MIDI
#include "mbed.h"
#include "DKS_CircularBuffer.h"

#define USBDEBUG

/**
 * @file midi.h
 * @brief MIDI library for mbed
 * @author Luna Tsukiyono
 * @date 2020/06/16
 */


const static uint8_t gmSystemOn[] = {0x7E,0x7F,0x09,0x01};
const static uint8_t xgSystemOn[] = {0x43,0x10,0x4C,0x00,0x00,0x7E,0x00};
const static uint8_t gsReset[] = {0x41,0x10,0x42,0x12,0x40,0x00,0x7F,0x00,0x41};


class midi{
    public:
        /** Create a MIDI instance connected to specified UART pins
        *
        * @param tx UART TX Pin
        * @param rx UART RX Pin
        */
        midi(PinName tx,PinName rx);

    
        void setCallbackNoteOn(void (*func)(int,int,int));
        /**
         * @fn
         * set callback function which is called when the message is note on
         * @brief set callback function
         * @param function pointer which arguments is (int channel,int noteNumber,int velocity)
         * @return void
         */
        void setCallbackNoteOff(void (*func)(int,int));
        /**
         * @fn
         * set callback function which is called when the message is note off
         * @brief set callback function
         * @param function pointer which arguments is (int channel,int noteNumber)
         * @return void
         */
        void setCallbackControlChange(void (*func)(int,int,int));
        /**
         * @fn
         * set callback function which is called when the message is controlchange
         * @brief set callback function
         * @param function pointer which arguments is (int channel,int controlChangeNumber,int value)
         * @return void
         */
        void setCallbackProgramChange(void (*func)(int,int));
        /**
         * @fn
         * set callback function which is called when the message is program change
         * @brief set callback function
         * @param function pointer which arguments is (int channel,int programNumber)
         * @return void
         */
        void setCallbackReset(void (*func)(void));
        /**
         * @fn
         * set callback function which is called when the message is reset
         * @brief set callback function
         * @param void
         * @return void
         */
    private:

        Serial serial;

        DKS::CircularBuffer<uint8_t,512> buffer;

        DKS::CircularBuffer<uint8_t,64> sysExBuffer;

        void (*noteOnFunc)(int,int,int);
        void (*noteOffFunc)(int,int);
        void (*controlChangeFunc)(int,int,int);
        void (*programChangeFunc)(int,int);
        void (*resetFunc)(void);


        void midiParse(void);
        void getData(void);
        void decodeCommand(void);
        void decodeSysEx(void);
        
        uint8_t decodeResets(void);

        uint8_t runningStatusCommand = 0x00;
        uint8_t runningStatusChannel = 0x00;

        bool isSysEx = false;
        
        bool threeBytesFlag = false;

        uint8_t decodeByte = 0x00;
        uint8_t secondByte = 0x00;
        uint8_t thirdByte = 0x00;

        const static uint8_t noteOn = 0x90;
        const static uint8_t noteOff = 0x80;
        const static uint8_t controlChange = 0xB0;
        const static uint8_t programChange = 0xC0;

        const static uint8_t beginSysEx = 0xF0;
        const static uint8_t endSysEx = 0xF7;
};

#endif