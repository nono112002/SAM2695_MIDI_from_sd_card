#include <M5Stack.h>  // M5Stackライブラリをインクルード
#include "M5UnitSynth.h"
#include <SD.h>
#include <SPI.h>

// MIDIの送信設定
M5UnitSynth synth;

// SDカードの設定
File midiFile;

void setup() {
  // M5Stackの初期化
  M5.begin(true, false);  // SDカードと関連の初期化を有効にする

  Serial.begin(115200);  // デバッグ用にシリアル通信を開始
  Serial.println("Unit Synth MIDI Player");

  // Unit Synthの初期化
  synth.begin(&Serial2, UNIT_SYNTH_BAUD, 16, 17);  // シリアル通信設定 (ボーレートなど)
  synth.setInstrument(0, 0, GrandPiano_1);  // GrandPiano sound

  // SDカードの初期化
  if (!SD.begin(GPIO_NUM_4, SPI, 15000000)) {  // CSピンGPIO4に接続されている場合、クロック周波数を15MHzに設定
    Serial.println("SDカードの初期化失敗！");
    return;
  }
  Serial.println("SDカードの初期化成功");

  // MIDIファイルのオープン
  midiFile = SD.open("/test.mid");  // SDカードに保存したMIDIファイル
  if (!midiFile) {
    Serial.println("MIDIファイルが見つかりません！");
    return;
  }
  Serial.println("MIDIファイルを再生開始");
}

void loop() {

  if (midiFile.available()) {
    byte midiByte = midiFile.read();  // 1バイトずつ読み込む
    // MIDIノートオン/ノートオフのコマンド
    if (midiByte == 0x90 || midiByte == 0x80) {  // ノートオン（0x90）またはノートオフ（0x80）
      byte note = midiFile.read();  // ノート番号
      byte velocity = midiFile.read();  // ベロシティ
      if (midiByte == 0x90) {
        synth.setNoteOn(0, note, velocity);  // ノートオン
      } else {
        synth.setNoteOff(0, note, 0);  // ノートオフ
      }
      delay(200);
    }
  } else {
    Serial.println("再生終了");
    midiFile.close();
    while (1);  // 再生終了後は停止
  }
}
