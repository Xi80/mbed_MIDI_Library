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
    buffer.clear();
    serial.attach(this,&midi::getData,Serial::RxIrq);
}

void midi::getData(void){
    uint8_t data = serial.getc();
    buffer.push_back(data);
    midiParse();
}

void midi::setCallbackNoteOn(void (*func)(int,int,int)){
    noteOnFunc = func;
}

void midi::setCallbackNoteOff(void (*func)(int,int)){
    noteOffFunc = func;
}

void midi::setCallbackControlChange(void (*func)(int,int,int)){
    controlChangeFunc = func;
}

void midi::setCallbackProgramChange(void (*func)(int,int)){
    programChangeFunc = func;
}

void midi::setCallbackReset(void (*func)(void)){
    resetFunc = func;
}


void midi::midiParse(void){
    if(!buffer.size())return;
    decodeByte = buffer.pull();
    if(isSysEx){
        if(decodeByte == endSysEx){
            isSysEx = false;
            decodeSysEx();
        } else {
            sysExBuffer.push_back(decodeByte);
        }
    } else {
        if(decodeByte == beginSysEx){
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
    }
}

void midi::decodeSysEx(void){
    if(decodeResets() == 0){
        //other commands
    } else {
        if(resetFunc != NULL)resetFunc();
    }
}

uint8_t midi::decodeResets(void){
    uint8_t tmp = 0x00;

    if(sysExBuffer.size() == 4){
        for(int i = 0;i < 4;i++){
            tmp = sysExBuffer.pull();
            if(tmp != gmSystemOn[i])return 0;
        }
    }
    if(sysExBuffer.size() == 7){
        for(int i = 0;i < 7;i++){
            tmp = sysExBuffer.pull();
            if(tmp != gmSystemOn[i])return 0;
        }
    }
    if(sysExBuffer.size() == 9){
        for(int i = 0;i < 9;i++){
            tmp = sysExBuffer.pull();
            if(tmp != gmSystemOn[i])return 0;
        }
    }
    sysExBuffer.clear();
    return 1;
}