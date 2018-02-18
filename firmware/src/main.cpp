void paymentCallback(EthereumTransaction* transaction)
{
    float value = transaction->getValue();
    if(value>=0.1){
      digitalWrite(4,HIGH);
      Elkrem.delay(2000);
      digitalWrite(4,LOW);
  }
}

void statusCallback(WhisperMessage* whisperMessage)
{
    char payload[3]= "";
    whisperMessage->getPayload(payload);
    if(!strcmp(payload,"On"))
      digitalWrite(4,HIGH);
    else if(!strcmp(payload,"Off"))
      digitalWrite(4,LOW);
}

void setup() {
    Elkrem.init();
    pinMode(4,OUTPUT);

    Ethereum.onNewTransaction(&paymentCallback);
    Ethereum.track("0x03FC1b1F0A72f43c526a27DbF51fB9c7a98666e6");

    Status.onNewMessage(&statusCallback);
    Status.subscribe("0x61e2225acef5ecfa81652896fb17a29835aa13e6cc0f89c640d2e60e9255ea6e");
}

void loop() {
  Elkrem.run();
}
