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
```



### リセットについて

GMシステムオン、XGシステムオン、GSリセットすべてに対応しています。



## 未対応機能

上に書いてあるもの以外は未実装です。

ランニングステータスルールには対応しているので現状でも使い物にはなります。

割り込み機能は使っていないのでwhileループに余計なものを書くとリアルタイム性が失われます。