#ifndef MESSAGE_PARSER_H
#define	MESSAGE_PARSER_H

typedef struct {
	unsigned char identification[2];
	unsigned char protocol[2];
	unsigned char messageType;
	unsigned char messageNumber;
	unsigned char componentType[2];
	unsigned char version[2];
} discoverSend;

typedef struct {
	unsigned char identification[2];
	unsigned char messageType;
	unsigned char messageNumber;
	unsigned char id[2];
	unsigned char standby[2];
	unsigned char timeout[2];
	unsigned char retries;
} discoverReceived;

typedef struct {
	unsigned char identification[2];
	unsigned char protocol[2];
	unsigned char messageType;
	unsigned char messageNumber;
	unsigned char id[2];
	unsigned char data[2];
	unsigned char battery;
} statementSend;

typedef struct {
	unsigned char identification[2];
	unsigned char messageType;
	unsigned char messageNumber;
	unsigned char id[2];
	unsigned char state;
	unsigned char standby[2];
	unsigned char timeout[2];
	unsigned char retries;
	unsigned char listParams;
} statementReceived;

const discoverSend askForId();
const discoverReceived parseDiscoverMessage(unsigned char m[]);
const statementSend sendData(unsigned int id, unsigned char messageNumber, unsigned int data, unsigned char battery);
const statementReceived parseStatementMessage(unsigned char m[]);

#endif	/* MESSAGE_PARSER_H */
