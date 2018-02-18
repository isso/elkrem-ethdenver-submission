#ifndef Whisper_h
#define Whisper_h
#include "Elkrem.h"

class WhisperMessage{
public:
  WhisperMessage(char * from,char * topic,char * payload,uint32_t timestamp){
    strcpy(this->from,from);
    strcpy(this->topic,topic);
    strcpy(this->payload,payload);
    this->timestamp=timestamp;
  }

  void getFrom(char *from){
    strcpy(from,this->from);
  }

  void getTopic(char *topic){
    strcpy(topic,this->topic);
  }

  void getPayload(char *payload){
    strcpy(payload,this->payload);
  }

  uint32_t getTimeStamp(){
    return timestamp;
  }

private:
  char from[133]={0};
  char topic[11]={0};
  char payload[512]={0};
  uint32_t timestamp;
};

class WhisperClass{
  friend class ElkremClass;
public:
  void subscribeAsym(char *,char *, char * =NULL, bool=true);
  void subscribeSym(char *,char *, char * =NULL, bool=true);
  void subscribe(char *,char *, char * =NULL, bool=true);
  void unsubscribeAll();
  void post(char *, char *, char *,char *);
  void postSym(char *, char *, char *,char *);
  void postAsym(char *, char *, char *,char *);
  void post(char *, char *, char *,uint8_t *, int);
  void postSym(char *, char *, char *,uint8_t *, int);
  void postAsym(char *, char *, char *,uint8_t *, int);
  void onNewMessage(void (*)(WhisperMessage*));

private:
	void processData(MessageType,uint8_t *, uint16_t);
	void (*newWhisperMessageCallBack)(WhisperMessage *);
};

class StatusClass{
  friend class WhisperClass;
public:
  void subscribe(char *);
  void onNewMessage(void (*)(WhisperMessage*));

private:
	void (*newStatusMessageCallBack)(WhisperMessage *);
};

extern WhisperClass Whisper;
extern StatusClass Status;

#endif
