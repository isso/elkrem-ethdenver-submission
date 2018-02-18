#include "Elkrem.h"
#include "ESP8266WiFi.h"

bool ElkremClass::_isInit = false;
uint8_t ElkremClass::hdlcBuffer[MAX_HDLC_FRAME_LENGTH] = {0};
uint8_t ElkremClass::protobufBuffer[MAX_PROTOBUF_LENGTH] = {0};

ElkremClass::ElkremClass():hdlc(&ElkremClass::write, &ElkremClass::processFrame, MAX_HDLC_FRAME_LENGTH)
{
      _isHostConnected = false;
      isHostConnectedCallBack = 0;
      _isInit = false;
}

void ElkremClass::init()
{
  WiFi.forceSleepBegin();
  Serial.begin(115200);
  _isInit = true;
  sendMessage(MessageType::MessageType_init_request,0,0);
  sendMessage(MessageType::MessageType_ping,0,0);
}

void ElkremClass::waitForHostConnection()
{
  _isHostConnected = false;
  while(!_isHostConnected)
  {
    Elkrem.run();
    ::delay(0);
  }
}

void ElkremClass::write(uint8_t data)
{
  if(_isInit)Serial.write(data);
}

void ElkremClass::sendMessage(MessageType type,uint8_t* data, uint16_t dataLength)
{
  hdlcBuffer[0]=static_cast<uint8_t>(type);
  if(dataLength>0)
    memcpy(hdlcBuffer+1,data,dataLength);
  hdlc.frameDecode(hdlcBuffer,dataLength+1);
}
bool ElkremClass::isHostConnected()
{
  return _isHostConnected;
}
void ElkremClass::onHostConnection(void (*userFunction)(bool))
{
  isHostConnectedCallBack = userFunction;
}

void ElkremClass::run()
{
  while(Serial.available())
  {
    hdlc.charReceiver((uint8_t)Serial.read());
    ::delay(0);
  }
  // TODO send ping every 2 seconds
}

void ElkremClass::processFrame(uint8_t *data, uint16_t length){
  if(length>0){
      MessageType type = static_cast<MessageType>(data[0]);
      if(type == MessageType::MessageType_pong){
        Elkrem._isHostConnected = true;
        Elkrem.sendMessage(MessageType::MessageType_ping,0,0);
      } else if(type == MessageType::MessageType_init_response){

      } else if(type == MessageType::MessageType_ethereum_address_subscription_response){
        // Serial.println('received');
        Ethereum.processData(type,data+1,length-1);
      } else if(type == MessageType::MessageType_ethereum_address_balance_response){
        Ethereum.processData(type,data+1,length-1);
      } else if(type == MessageType::MessageType_current_block_number_response){
        Ethereum.processData(type,data+1,length-1);
      } else if(type == MessageType::MessageType_error){

      }else if(type == MessageType::MessageType_whisper_received_message){
        Whisper.processData(type,data+1,length-1);
      }
  }
}

void ElkremClass::delay(unsigned long time)
{
  unsigned long now=millis();
  while(millis()<(now+time))
  {
    run();
    ::delay(0);
  }
}
ElkremClass Elkrem;
