#include "Elkrem.h"

bool writeBytesFieldCallback(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
  uint8_t *data = (uint8_t*) *arg;
  if (!pb_encode_tag_for_field(stream, field))
      return false;
  return pb_encode_string(stream, data, sizeof(data));
}

char ElkremSerialClass::read()
{
	if(buffer.remain()<=0)return -1;
	return buffer.pop();
}

void ElkremSerialClass::flush()
{
	while(read()!=-1);
}

int ElkremSerialClass::available()
{
	return buffer.remain();
}

int ElkremSerialClass::readBytes(char *arr, int length)
{
	int count = 0;
	 	while (count < length) {
	    int c = read();
	    if (c < 0) break;
	    *arr++ = (char)c;
	    count++;
 	}
  return count;
}

void ElkremSerialClass::processData(MessageType type,uint8_t * data, uint16_t size)
{
		// buffer.push(getOneSheeldInstance().getArgumentData(0)[j]);
}

void ElkremSerialClass::sendByteArray(uint8_t * data, uint16_t size)
{
  SerialMessage serialData = SerialMessage_init_zero;
  serialData.data.funcs.encode = &writeBytesFieldCallback;
  serialData.data.arg = data;
  pb_ostream_t ostream = pb_ostream_from_buffer(ElkremClass::protobufBuffer, sizeof(ElkremClass::protobufBuffer));
  bool status = pb_encode(&ostream, SerialMessage_fields, &serialData);
  if(status){
    Elkrem.sendMessage(MessageType::MessageType_serial_request,ElkremClass::protobufBuffer,ostream.bytes_written);
  }
  // Create new empty data msg
  // Set its encode method
  // Set its data pointer
  // Encode it and return result
}

void ElkremSerialClass::write(char data)
{
  sendByteArray((uint8_t *)&data,1);
}

void ElkremSerialClass::print(char data)
{
  sendByteArray((uint8_t *)&data,1);
}

void ElkremSerialClass::print(int data)
{
	char stringPointer[7];
	snprintf(stringPointer,7,"%d",data);
  sendByteArray((uint8_t *)&stringPointer,strlen(stringPointer));
}

void ElkremSerialClass::print(unsigned int data)
{
	char stringPointer[6];
	snprintf(stringPointer,6,"%d",data);
  sendByteArray((uint8_t *)&stringPointer,strlen(stringPointer));
}

void ElkremSerialClass::print(long data)
{
	char stringPointer[12];
	snprintf(stringPointer,12,"%ld",data);
  sendByteArray((uint8_t *)&stringPointer,strlen(stringPointer));
}

void ElkremSerialClass::print(unsigned long data)
{
	char stringPointer[11];
	snprintf(stringPointer,11,"%lu",data);
  sendByteArray((uint8_t *)&stringPointer,strlen(stringPointer));
}

void ElkremSerialClass::print(const char * stringData)
{
	//Check length of string
	int stringDataLength = strlen(stringData);
	if(!stringDataLength) return;
  sendByteArray((uint8_t *)&stringData,stringDataLength);
}

void ElkremSerialClass::print(String stringData)
{
	print(&stringData[0]);
}

void ElkremSerialClass::print(double data , int precesion)
{
	char buffer[32]={'\0'};
	dtostrf(data,1,precesion,buffer);
  sendByteArray((uint8_t *)&buffer,strlen(buffer));
}

void ElkremSerialClass::println()
{
	char buffer[3];
	buffer[0]='\r';
	buffer[1]='\n';
	buffer[2]='\0';
	print(buffer);
}

void ElkremSerialClass::println(char data)
{
	char buffer[4];
	buffer[0]=data;
	buffer[1]='\r';
	buffer[2]='\n';
	buffer[3]='\0';
	print(buffer);
}

void ElkremSerialClass::println(int data)
{
	char stringPointer[10];
	snprintf(stringPointer,9,"%d",data);
	strcat(stringPointer,"\r\n");
	print(stringPointer);
}

void ElkremSerialClass::println(unsigned int data)
{
	char stringPointer[9];
	snprintf(stringPointer,8,"%d",data);
	strcat(stringPointer,"\r\n");
	print(stringPointer);
}

void ElkremSerialClass::println(long data)
{
	char stringPointer[15];
	snprintf(stringPointer,14,"%ld",data);
	strcat(stringPointer,"\r\n");
	print(stringPointer);
}

void ElkremSerialClass::println(unsigned long data)
{
	char stringPointer[14];
	snprintf(stringPointer,13,"%lu",data);
	strcat(stringPointer,"\r\n");
	print(stringPointer);
}

void ElkremSerialClass::println(const char * stringData)
{
	char stringNewLine[strlen(stringData)+3];
	stringNewLine[0]='\0';
	strcat(stringNewLine,stringData);
	strcat(stringNewLine,"\r\n");
	print(stringNewLine);
}

void ElkremSerialClass::println(String stringData)
{
	println(&stringData[0]);
}

void ElkremSerialClass::println(double data, int precesion)
{
	int i=0;
	char buffer[32]={'\0'};
	dtostrf(data,1,precesion,buffer);

	while(buffer[i]!='\0' && i<32)i++;

	if(i+2>32)return;
	buffer[i]='\r';
	buffer[i+1]='\n';
	buffer[i+2]='\0';

	print(buffer);
}

ElkremSerialClass Serial2;
