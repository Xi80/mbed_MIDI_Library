#ifndef H_MIDI
#define H_MIDI
#include "mbed.h"
#include "string.h"
#define USBDEBUG

/**
 * @file midi.h
 * @brief MIDI library for mbed
 * @author Luna Tsukiyono
 * @date 2020/06/16
 */

const  uint8_t gmSystemOn[] = {0x7E,0x7F,0x09,0x01};
const  uint8_t xgSystemOn[] = {0x43,0x10,0x4C,0x00,0x00,0x7E,0x00};
const  uint8_t gsReset[]    = {0x41,0x10,0x42,0x12,0x40,0x00,0x7F,0x00,0x41};

class midi{
    public:
        midi(PinName tx,PinName rx);

        void setCallbackNoteOn(void (*func)(int,int,int));
        void setCallbackNoteOff(void (*func)(int,int));
        void setCallbackControlChange(void (*func)(int,int,int));
        void setCallbackProgramChange(void (*func)(int,int));
        void setCallbackReset(void (*func)(void));
        void setCallbackPitchBend(void (*func)(int,unsigned short));

        void midiParse(void);

    private:
        Serial serial;
        lunaLib::CircularBuffer<uint8_t,2048> buffer;

        lunaLib::CircularBuffer<uint8_t,128> sysExBuffer;

        void (*noteOnFunc)(int,int,int);
        void (*noteOffFunc)(int,int);
        void (*controlChangeFunc)(int,int,int);
        void (*pitchBendFunc)(int,unsigned short);
        void (*programChangeFunc)(int,int);
        void (*resetFunc)(void);

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

namespace lunaLib{
	template<typename T, uint16_t Capacity>
	class CircularBuffer{
	private:
		T buffer[Capacity];
		const uint16_t m_mask;
		uint16_t m_head, m_tail;
		uint16_t m_size;
		uint16_t m_capacity;

		inline void MovePointer(uint16_t &pointer, const int16_t &step){
			pointer = (pointer + step) & m_mask;
		}
		inline uint16_t emptySize() const{
			return Capacity - m_size;
		}

	public:
		volatile bool isLocked;
		explicit CircularBuffer() :m_mask(Capacity - 1), m_head(0), m_tail(0), m_size(0), m_capacity(Capacity), isLocked(false){
			if ((Capacity & (Capacity - 1)) != 0) while (true);
		}

		virtual ~CircularBuffer(){
		}

		void push_back(const T &value){
			if (isLocked) while (true);
			isLocked = true;
			buffer[m_tail] = value;
			MovePointer(m_tail, 1);
			if (m_size == Capacity)
				MovePointer(m_head, 1);
			else m_size++;
			isLocked = false;
		}
		T pull(){
			if (isLocked) while (true);
			isLocked = true;
			const T res = buffer[m_head];
			MovePointer(m_head, 1);
			m_size--;
			isLocked = false;
			return res;
		}

		uint16_t size() const{
			return m_size;
		}

		void clear(){
			if (isLocked) while (true)
				;
			isLocked = true;

			m_head = 0;
			m_tail = 0;
			m_size = 0;

			isLocked = false;
		}

		T &operator[ ](const uint16_t &n){
			return buffer[(m_head + n) & m_mask];
		}
	};

};

#endif