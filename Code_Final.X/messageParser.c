#include <stdio.h>
#include <stdlib.h>

#include "messageParser.h"

const discoverSend askForId() {
	discoverSend ds;

	ds.identification[0] = 0x42;
	ds.identification[1] = 0x26;

	ds.protocol[0] = 0x00;
	ds.protocol[1] = 0x01;

	ds.messageType = 0x01;

	ds.messageNumber = 0xc0;

	ds.componentType[0] = 0x00;
	ds.componentType[1] = 0x01;

	ds.version[0] = 0x00;
	ds.version[1] = 0x01;

	return ds;
}

const discoverReceived parseDiscoverMessage(unsigned char m[]) {
	discoverReceived dr;

	dr.identification[0] = m[0];
	dr.identification[1] = m[1];

	dr.messageType = m[2];

	dr.messageNumber = m[3];

	dr.id[0] = m[4];
	dr.id[1] = m[5];

	dr.standby[0] = m[6];
	dr.standby[1] = m[7];

	dr.timeout[0] = m[8];
	dr.timeout[1] = m[9];

	dr.retries = m[10];

	return dr;
}

const statementSend sendData(unsigned int id, unsigned char messageNumber, unsigned int data, unsigned char battery) {
	statementSend ss;

	ss.identification[0] = 0x42;
	ss.identification[1] = 0x26;

	ss.protocol[0] = 0x00;
	ss.protocol[1] = 0x01;

	ss.messageType = 0x02;

	ss.messageNumber = messageNumber;

	ss.id[0] = (id >> 8) & 0xff;
	ss.id[1] = id & 0xff;

	ss.data[0] = (data >> 8) & 0xff;
	ss.data[1] = data & 0xff;

	ss.battery = battery;

	return ss;
}

const statementReceived parseStatementMessage(unsigned char m[]) {
	statementReceived sr;

	sr.identification[0] = m[0];
	sr.identification[1] = m[1];

	sr.messageType = m[2];

	sr.messageNumber = m[3];

	sr.id[0] = m[4];
	sr.id[1] = m[5];

	sr.state = m[6];

	sr.standby[0] = m[7];
	sr.standby[1] = m[8];

	sr.timeout[0] = m[9];
	sr.timeout[1] = m[10];

	sr.retries = m[11];

	return sr;
}
