# mbed MIDI Library

Copyright(c) 2020 Xi80



## 概要

良い感じのMIDI受信ライブラリがmbedになかったので一から書きました。

受信しかできません、僕は受信しか使わないので。



## 使い方

```cpp
#include "mbed.h"
#include "midi/midi.h"

midi MIDI(USBTX,USBRX);

int noteOn(int ch,int note,int vel){
    //ここにノートオン時の処理を書く
}
int main(void){
    MIDI.setCallbackNoteOn(noteOn);
    while(1){
        MIDI.parse();
    }
}
```

USBシリアルではなくMIDIケーブルで受信する場合はmidi.hの#define USBDEBUGを消してください。



### コールバックについて

```cpp
void setCallbackNoteOn(void (*func)(int,int,int));

void setCallbackNoteOff(void (*func)(int,int));

void setCallbackControlChange(void (*func)(int,int,int));

void setCallbackProgramChange(void (*func)(int,int));

void setCallbackReset(void (*func)(void));

void setCallbackPitchBend(void (*func)(int,unsigned short));
```

コードを見ればわかります。

### リセットについて

GMシステムオン、XGシステムオン、GSリセットすべてに対応しています。



## 未対応機能

上に書いてあるもの以外は未実装です。

ランニングステータスルールには対応しているので現状でも使い物にはなります。

~~割り込み機能は使っていないのでwhileループに余計なものを書くとリアルタイム性が失われます。~~

-->割り込みに対応しました。



## サンプルコード

```cpp
#include <mbed.h>
#include "midi/midi.h"


DigitalOut LEDs[] = {D2,D3,D4,D5};

void noteOn(int ch,int note,int vel){
  LEDs[0] = !LEDs[0];
}

void noteOff(int ch,int note){
  LEDs[1] = !LEDs[1];
}

void controlChange(int ch,int num,int val){
  LEDs[2] = !LEDs[2];
}

void programChange(int ch,int num){
  LEDs[3] = !LEDs[3];
}

void reset(void){
  for(int i = 0;i < 4;i++){
    LEDs[i] = 0;
    wait(0.1);
  }
  for(int i = 0;i < 4;i++){
    LEDs[i] = 1;
    wait(0.1);
  }
  for(int i = 3;i >= 0;i--){
    LEDs[i] = 0;
    wait(0.1);
  }
}

midi MIDI(USBTX,USBRX);

int main() {
  MIDI.setCallbackNoteOn(noteOn);
  MIDI.setCallbackNoteOff(noteOff);
  MIDI.setCallbackControlChange(controlChange);
  MIDI.setCallbackProgramChange(programChange);
  MIDI.setCallbackReset(reset);
    
  while(1){
      MIDI.parse();
  }
}
```

D2~D5にLEDを繋ぐとMIDIメッセージに合わせて光ります。