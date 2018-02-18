#ifndef Elkrem_h
#define Elkrem_h
#include "Arduino.h"
#include "Arduhdlc.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "messages.pb.h"
#include "Ethereum.h"
#include "ElkremSerial.h"
#include "Whisper.h"

#define MAX_HDLC_FRAME_LENGTH 2048
#define MAX_PROTOBUF_LENGTH 1024

class ElkremClass{

public:

	ElkremClass();
	void waitForHostConnection();
	bool isHostConnected();
	void onHostConnection(void (*)(bool));
	void run();
	void init();
	void delay(unsigned long);
  bool isInit();
  void sendMessage(MessageType,uint8_t*, uint16_t);
	static uint8_t protobufBuffer[MAX_PROTOBUF_LENGTH];
private:
  static bool _isInit;
	bool _isHostConnected;
	static void processFrame(uint8_t *data, uint16_t length);
	static uint8_t hdlcBuffer[MAX_HDLC_FRAME_LENGTH];
	void (*isHostConnectedCallBack)(bool);
	static void write(uint8_t data);
  Arduhdlc hdlc;
};

extern ElkremClass Elkrem;
#endif
