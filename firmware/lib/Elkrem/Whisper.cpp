#include "Whisper.h"

void WhisperClass::subscribeAsym(char * privateKey,char * topic, char * pubKey, bool isAscii){
  WhisperSubscribe ws = WhisperSubscribe_init_zero;
  ws.has_private_key = true;
  strcpy(ws.private_key,privateKey);
  ws.has_topic = true;
  strcpy(ws.topic,topic);
  if(pubKey!=0){
    ws.has_pubkey = true;
    strcpy(ws.pubkey,pubKey);
  }
  ws.has_is_ascii = true;
  ws.is_ascii = isAscii;
  pb_ostream_t ostream = pb_ostream_from_buffer(ElkremClass::protobufBuffer, sizeof(ElkremClass::protobufBuffer));
  bool status = pb_encode(&ostream, WhisperSubscribe_fields, &ws);
  if(status){
    Elkrem.sendMessage(MessageType::MessageType_whisper_subscribe,ElkremClass::protobufBuffer,ostream.bytes_written);
  }
}

void WhisperClass::subscribeSym(char * symKeyPassword,char * topic, char * pubKey, bool isAscii){
  WhisperSubscribe ws = WhisperSubscribe_init_zero;
  ws.has_sym_key_password = true;
  strcpy(ws.sym_key_password,symKeyPassword);
  ws.has_topic = true;
  strcpy(ws.topic,topic);
  if(pubKey!=0){
    ws.has_pubkey = true;
    strcpy(ws.pubkey,pubKey);
  }
  ws.has_is_ascii = true;
  ws.is_ascii = isAscii;
  pb_ostream_t ostream = pb_ostream_from_buffer(ElkremClass::protobufBuffer, sizeof(ElkremClass::protobufBuffer));
  bool status = pb_encode(&ostream, WhisperSubscribe_fields, &ws);
  if(status){
    Elkrem.sendMessage(MessageType::MessageType_whisper_subscribe,ElkremClass::protobufBuffer,ostream.bytes_written);
  }
}

void WhisperClass::subscribe(char * privateKey,char * topic, char * pubKey, bool isAscii){
  subscribeAsym(privateKey,topic,pubKey,isAscii);
}

void WhisperClass::unsubscribeAll(){
  Elkrem.sendMessage(MessageType::MessageType_whisper_unsubscribe_all,0,0);
}

void WhisperClass::post(char * pubKey, char *signaturePrivateKey, char * topic,char * payload){
  postAsym(pubKey,signaturePrivateKey,topic,payload);
}

void WhisperClass::postSym(char * symKeyPassword, char *signaturePrivateKey, char * topic,char * payload){
  TransmittedWhisperMessage tws = TransmittedWhisperMessage_init_zero;
  tws.has_sym_key_password = true;
  strcpy(tws.sym_key_password,symKeyPassword);
  tws.has_signature_private_key = true;
  strcpy(tws.signature_private_key,signaturePrivateKey);
  tws.has_topic = true;
  strcpy(tws.topic,topic);
  tws.has_payload = true;
  strcpy(tws.payload,payload);
  tws.has_is_ascii = true;
  tws.is_ascii = true;
  pb_ostream_t ostream = pb_ostream_from_buffer(ElkremClass::protobufBuffer, sizeof(ElkremClass::protobufBuffer));
  bool status = pb_encode(&ostream, TransmittedWhisperMessage_fields, &tws);
  if(status){
    Elkrem.sendMessage(MessageType::MessageType_whisper_transmitted_message,ElkremClass::protobufBuffer,ostream.bytes_written);
  }
}

void WhisperClass::postAsym(char * pubKey, char *signaturePrivateKey, char * topic,char * payload){
  TransmittedWhisperMessage tws = TransmittedWhisperMessage_init_zero;
  tws.has_pub_key = true;
  strcpy(tws.pub_key,pubKey);
  tws.has_signature_private_key = true;
  strcpy(tws.signature_private_key,signaturePrivateKey);
  tws.has_topic = true;
  strcpy(tws.topic,topic);
  tws.has_payload = true;
  strcpy(tws.payload,payload);
  tws.has_is_ascii = true;
  tws.is_ascii = true;
  pb_ostream_t ostream = pb_ostream_from_buffer(ElkremClass::protobufBuffer, sizeof(ElkremClass::protobufBuffer));
  bool status = pb_encode(&ostream, TransmittedWhisperMessage_fields, &tws);
  if(status){
    Elkrem.sendMessage(MessageType::MessageType_whisper_transmitted_message,ElkremClass::protobufBuffer,ostream.bytes_written);
  }
}

void WhisperClass::post(char * pubKey, char *signaturePrivateKey, char * topic,uint8_t * payload,int size){
  postAsym(pubKey,signaturePrivateKey,topic,payload,size);
}

void WhisperClass::postSym(char * symKeyPassword, char *signaturePrivateKey, char * topic,uint8_t * payload,int size){
  TransmittedWhisperMessage tws = TransmittedWhisperMessage_init_zero;
  tws.has_sym_key_password = true;
  strcpy(tws.sym_key_password,symKeyPassword);
  tws.has_signature_private_key = true;
  strcpy(tws.signature_private_key,signaturePrivateKey);
  tws.has_topic = true;
  strcpy(tws.topic,topic);
  tws.has_payload = true;
  memcpy(tws.payload,payload,size);
  tws.has_is_ascii = true;
  tws.is_ascii = false;
  pb_ostream_t ostream = pb_ostream_from_buffer(ElkremClass::protobufBuffer, sizeof(ElkremClass::protobufBuffer));
  bool status = pb_encode(&ostream, TransmittedWhisperMessage_fields, &tws);
  if(status){
    Elkrem.sendMessage(MessageType::MessageType_whisper_transmitted_message,ElkremClass::protobufBuffer,ostream.bytes_written);
  }
}

void WhisperClass::postAsym(char * pubKey, char *signaturePrivateKey, char * topic,uint8_t * payload,int size){
  TransmittedWhisperMessage tws = TransmittedWhisperMessage_init_zero;
  tws.has_pub_key = true;
  strcpy(tws.pub_key,pubKey);
  tws.has_signature_private_key = true;
  strcpy(tws.signature_private_key,signaturePrivateKey);
  tws.has_topic = true;
  strcpy(tws.topic,topic);
  tws.has_payload = true;
  memcpy(tws.payload,payload,size);
  tws.has_is_ascii = true;
  tws.is_ascii = false;
  pb_ostream_t ostream = pb_ostream_from_buffer(ElkremClass::protobufBuffer, sizeof(ElkremClass::protobufBuffer));
  bool status = pb_encode(&ostream, TransmittedWhisperMessage_fields, &tws);
  if(status){
    Elkrem.sendMessage(MessageType::MessageType_whisper_transmitted_message,ElkremClass::protobufBuffer,ostream.bytes_written);
  }
}

void WhisperClass::onNewMessage(void (*_newWhisperMessageCallBack)(WhisperMessage*)){
  newWhisperMessageCallBack = _newWhisperMessageCallBack;
}

void WhisperClass::processData(MessageType type,uint8_t *data, uint16_t length){
  if(type == MessageType::MessageType_whisper_received_message){
    ReceivedWhisperMessage rws = ReceivedWhisperMessage_init_zero;
    pb_istream_t istream = pb_istream_from_buffer(data, length);
    bool status = pb_decode(&istream, ReceivedWhisperMessage_fields, &rws);
    // Serial.println(PB_GET_ERROR(&stream));
    if(status){
      // Serial.println(eas.from_address);
      WhisperMessage whisperMessage(rws.from,rws.topic,rws.payload,rws.timestamp);
      if(Status.newStatusMessageCallBack && !strcmp(rws.topic,"0xaabb11ee"))
        Status.newStatusMessageCallBack(&whisperMessage);
      else if(newWhisperMessageCallBack)
        newWhisperMessageCallBack(&whisperMessage);
    }
  }
}

void StatusClass::onNewMessage(void (*_newStatusMessageCallBack)(WhisperMessage*)){
  newStatusMessageCallBack = _newStatusMessageCallBack;
}

void StatusClass::subscribe(char * privateKey){
  Whisper.subscribe(privateKey,"0xaabb11ee");
}


WhisperClass Whisper;
StatusClass Status;
