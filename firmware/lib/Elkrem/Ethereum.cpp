#include "Ethereum.h"

void EthereumClass::track(char * toAddress){
  EthereumAddressSubscription eas = EthereumAddressSubscription_init_zero;
  eas.has_to_address = true;
  strcpy(eas.to_address,toAddress);
  // uint8_t buffer[128];
  pb_ostream_t ostream = pb_ostream_from_buffer(ElkremClass::protobufBuffer, sizeof(ElkremClass::protobufBuffer));
  bool status = pb_encode(&ostream, EthereumAddressSubscription_fields, &eas);
  if(status){
    Elkrem.sendMessage(MessageType::MessageType_ethereum_address_subscription_request,ElkremClass::protobufBuffer,ostream.bytes_written);
  }
}

void EthereumClass::untrack(char * toAddress){
  EthereumAddressSubscription eas = EthereumAddressSubscription_init_zero;
  eas.has_to_address = true;
  strcpy(eas.to_address,toAddress);
  // uint8_t buffer[128];
  pb_ostream_t ostream = pb_ostream_from_buffer(ElkremClass::protobufBuffer, sizeof(ElkremClass::protobufBuffer));
  bool status = pb_encode(&ostream, EthereumAddressSubscription_fields, &eas);
  if(status){
    Elkrem.sendMessage(MessageType::MessageType_ethereum_address_unsubscription_request,ElkremClass::protobufBuffer,ostream.bytes_written);
  }
}

void EthereumClass::untrackAll(){
  Elkrem.sendMessage(MessageType::MessageType_ethereum_all_addresses_unsubscription_request,0,0);
}

void EthereumClass::requestBalance(char * address){
  EthereumAddressBalance eab = EthereumAddressBalance_init_zero;
  eab.has_address = true;
  strcpy(eab.address,address);
  // uint8_t buffer[128];
  pb_ostream_t ostream = pb_ostream_from_buffer(ElkremClass::protobufBuffer, sizeof(ElkremClass::protobufBuffer));
  bool status = pb_encode(&ostream, EthereumAddressBalance_fields, &eab);
  if(status){
    Elkrem.sendMessage(MessageType::MessageType_ethereum_address_balance_request,ElkremClass::protobufBuffer,ostream.bytes_written);
  }
}

void EthereumClass::requestCurrentBlock(){
  Elkrem.sendMessage(MessageType::MessageType_current_block_number_request,0,0);
}

void EthereumClass::onNewTransaction(void (*_newTransactionCallBack)(EthereumTransaction *)){
  newTransactionCallBack = _newTransactionCallBack;
}

void EthereumClass::onBalanceResponse(void (*_balanceResponseCallBack)(char * , float)){
  balanceResponseCallBack = _balanceResponseCallBack;
}

void EthereumClass::onCurrentBlockResponse(void (*_currentBlockResponseCallBack)(uint32_t, char * )){
  currentBlockResponseCallBack = _currentBlockResponseCallBack;
}

void EthereumClass::processData(MessageType type,uint8_t *data, uint16_t length){
  if(type == MessageType::MessageType_ethereum_address_subscription_response){
    // Serial.println('subreceived');
    EthereumAddressSubscription eas = EthereumAddressSubscription_init_zero;
    // eas.block.funcs.decode = &blockCallback;
    pb_istream_t istream = pb_istream_from_buffer(data, length);
    bool status = pb_decode(&istream, EthereumAddressSubscription_fields, &eas);
    // Serial.println(PB_GET_ERROR(&stream));
    if(status && newTransactionCallBack){
      // Serial.println(eas.from_address);
      EthereumTransaction transaction(eas.from_address,eas.to_address,eas.value,eas.hash,eas.block.blockNumber,eas.block.blockHash);
      newTransactionCallBack(&transaction);
    }
  } else if(type == MessageType::MessageType_current_block_number_response){
    EthereumAddressBalance eab = EthereumAddressBalance_init_zero;
    pb_istream_t istream = pb_istream_from_buffer(data, length);
    bool status = pb_decode(&istream, EthereumAddressBalance_fields, &eab);
    if(status && balanceResponseCallBack){
      balanceResponseCallBack(eab.address,eab.value);
    }
  } else if(type == MessageType::MessageType_current_block_number_response){
    EthereumBlock eb = EthereumBlock_init_zero;
    pb_istream_t istream = pb_istream_from_buffer(data, length);
    bool status = pb_decode(&istream, EthereumBlock_fields, &eb);
    if(status && currentBlockResponseCallBack){
      currentBlockResponseCallBack(eb.blockNumber, eb.blockHash);
    }
  }
}

EthereumClass Ethereum;
