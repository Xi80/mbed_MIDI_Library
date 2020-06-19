#include "midi.h"



midi::midi(PinName tx,PinName rx) : serial(tx,rx){
    #ifdef USBDEBUG
        serial.baud(38400);
    #else
        serial.baud(31500);
    #endif
    noteOnFunc = NULL;
    noteOffFunc = NULL;
    controlChangeFunc = NULL;
    programChangeFunc = NULL;
    resetFunc = NULL;
    sysExFunc = NULL;
    buffer.clear();
    serial.attach(this,&midi::getData,Serial::RxIrq);
}

void midi::init(void){
    buffer.clear();
    sysExBufferPos = 0;
}

void midi::getData(void){
    uint8_t data = serial.getc();
    if(buffer.size() >= 1023)buffer.clear();
    buffer.push_back(data);
}

void midi::setCallbackNoteOn(void (*func)(uint8_t,uint8_t,uint8_t)){
    noteOnFunc = func;
}

void midi::setCallbackNoteOff(void (*func)(uint8_t,uint8_t)){
    noteOffFunc = func;
}

void midi::setCallbackControlChange(void (*func)(uint8_t,uint8_t,uint8_t)){
    controlChangeFunc = func;
}

void midi::setCallbackProgramChange(void (*func)(uint8_t,uint8_t)){
    programChangeFunc = func;
}

void midi::setCallbackReset(void (*func)(void)){
    resetFunc = func;
}

void midi::setCallbackPitchBend(void (*func)(uint8_t,unsigned short)){
    pitchBendFunc = func;
}

void midi::setCallbackSystemExclusive(void (*func)(uint8_t*,uint8_t)){
    sysExFunc = func;
}

void midi::midiParse(void){
    if(!buffer.size())return;
    decodeByte = buffer.pull();
    if(isSysEx){
        if(decodeByte == endSysEx){
            isSysEx = false;
            decodeSysEx();
        } else {
            if(sysExBufferPos > 127)sysExBufferPos = 0;
            if(decodeByte == 0xFE)return;
            sysExBuffer[sysExBufferPos++] = decodeByte;
        }
    } else {
        if(decodeByte == beginSysEx){
            sysExBufferPos = 0;
            isSysEx = true;
        } else {
            if(decodeByte >> 7){
                if(!(decodeByte >= 0xF8)){
                    runningStatusCommand = decodeByte & 0xF0;
                    runningStatusChannel = decodeByte & 0x0F;
                    threeBytesFlag = false;
                }
            } else {
                if(threeBytesFlag){
                    threeBytesFlag = false;
                    thirdByte = decodeByte;
                    decodeCommand();
                } else {
                    if(runningStatusCommand < 0xC0){
                        threeBytesFlag = true;
                        secondByte = decodeByte;
                    } else {
                        if(runningStatusCommand < 0xE0){
                            secondByte = decodeByte;
                            decodeCommand();
                        } else {
                            if(runningStatusCommand < 0xF0){
                                threeBytesFlag = true;
                                secondByte = decodeByte;
                            }
                        }
                    }
                }
            }
        }
    }
}

void midi::decodeCommand(void){
    switch(runningStatusCommand){
        case noteOn:
            if(thirdByte != 0){
                if(noteOnFunc != NULL)noteOnFunc(runningStatusChannel,secondByte,thirdByte);
            } else {
                if(noteOffFunc != NULL)noteOffFunc(runningStatusChannel,secondByte);
            }
            break;
        case noteOff:
            if(noteOffFunc != NULL)noteOffFunc(runningStatusChannel,secondByte);
            break;
        case controlChange:
            if(controlChangeFunc != NULL)controlChangeFunc(runningStatusChannel,secondByte,thirdByte);
            break;
        case programChange:
            if(programChangeFunc != NULL)programChangeFunc(runningStatusChannel,secondByte);
            break;
        case pitchBend:
            if(pitchBendFunc != NULL)pitchBendFunc(runningStatusChannel,(unsigned short)(thirdByte << 7 | secondByte));
            break;
    }
}

void midi::decodeSysEx(void){
    if(sysExBufferPos == 4){
        for(int i = 0;i < 4;i++){
            if(sysExBuffer[i] != gmSystemOn[i]){
                if(sysExFunc != NULL)sysExFunc(sysExBuffer,sysExBufferPos);
            }
        }
        if(resetFunc != NULL)resetFunc();
    } else {
        if(sysExFunc != NULL)sysExFunc(sysExBuffer,sysExBufferPos);
    }
    sysExBufferPos = 0;
    return;
}