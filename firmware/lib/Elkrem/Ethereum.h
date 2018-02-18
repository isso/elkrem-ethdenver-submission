#ifndef Ethereum_h
#define Ethereum_h
#include "Elkrem.h"

// bool blockCallback(pb_istream_t *stream, const pb_field_t *field, void **arg)
// {
//     EthereumBlock ethereumBlock = EthereumBlock_init_zero;
//     if (!pb_decode(stream, EthereumBlock_fields, &ethereumBlock))
//         return false;
//     return true;
// }

class EthereumTransaction{
public:
  EthereumTransaction(char * from,char * to,float value,char * hash,uint32_t blockNumber, char * blockHash){
    strcpy(this->from,from);
    strcpy(this->to,to);
    this->value=value;
    strcpy(this->hash,hash);
    this->blockNumber=blockNumber;
    strcpy(this->blockHash,blockHash);
  }

  void getFromAddress(char *from){
    strcpy(from,this->from);
  }

  void getToAddress(char *to){
    strcpy(to,this->to);
  }

  void getHash(char *hash){
    strcpy(hash,this->hash);
  }

  void getBlockHash(char *blockHash){
    strcpy(blockHash,this->blockHash);
  }

  float getValue(){
    return value;
  }

  uint32_t getBlockNumber(){
    return blockNumber;
  }

private:
  char from[43]={0};
  char to[43]={0};
  float value;
  char hash[67]={0};
  uint32_t blockNumber;
  char blockHash[67]={0};
};

class EthereumClass{
  friend class ElkremClass;
public:
  void track(char *);
  void untrack(char *);
  void untrackAll();
  void requestBalance(char *);
  void requestCurrentBlock();
  void onNewTransaction(void (*)(EthereumTransaction *));
  void onBalanceResponse(void (*)(char * , float));
	void onCurrentBlockResponse(void (*)(uint32_t, char * ));

private:
	void processData(MessageType,uint8_t *, uint16_t);
	void (*newTransactionCallBack)(EthereumTransaction *);
  void (*balanceResponseCallBack)(char * , float);
	void (*currentBlockResponseCallBack)(uint32_t, char * );
};

extern EthereumClass Ethereum;
#endif
