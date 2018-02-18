# Elkrem: Development board for IOT & Ethereum

Elkrem makes it easy for blockchain developers and hardware engineers to build Ethereum connected IOT products.

# How?

We built a hardware development board that allows Ethereum developers to integrate Dapps with hardware IOT products in an easy way without having deep knowledge in hardware development, it also allows electrical engineers and hardware developers to connect Ethereum blockchain to their IOT products without having deep knowledge of how the Ethereum blockchain works. So they both can trigger physical actuators through smart contracts and log sensors data to smart contracts as well. We also integrated Whisper to it, so our users can monitor and control hardware products and projects using small whisper messages.

# Sample Code

Here is a sample code for our demo. It lets Status app users control phyisical hardware (i.e. light bulbs) using Ethereum transactions and Whisper messages right from the app.

```
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

    // Subscribe to new Ethereum transaction callback to a specific address.
    Ethereum.onNewTransaction(&paymentCallback);
    Ethereum.track("0x03FC1b1F0A72f43c526a27DbF51fB9c7a98666e6");

	// Subscribe to new Whisper messages callback to using a specific private key.
    Status.onNewMessage(&statusCallback);
    Status.subscribe("0x61e2225acef5ecfa81652896fb17a29835aa13e6cc0f89c640d2e60e9255ea6e");
}

void loop() {
  Elkrem.run();
}

```

# Project Status
The PoC we did is fully functional, we were able to control a 110v light bulb using 'on' and 'off' messages from Status app, and even trigger actions when an Ethereum payment is sent from the chat window.

# Demo Video

[![Elkrem Video](https://i.imgur.com/ksRyffR.png)](https://www.youtube.com/watch?v=qR6AP10LGts)

# System Diagram

![Elkrem Video](https://i.imgur.com/Y6vFU2o.png)

# Hardware Used

We used a combination of NanoPi Duo and ESP8266 to simulate our final development kit

# Technologies Used

NodeJS, Web3js, C++, Protobuf, Nanopb, HDLC, Platformio, Arduino and Python.



