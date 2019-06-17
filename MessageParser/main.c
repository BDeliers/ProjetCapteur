#include <stdio.h>
#include "messageParser.h"

int main() {

	// Create an id asking message
	const discoverSend ds = askForId();

	unsigned char discover[10] = {ds.identification[0], ds.identification[1], ds.protocol[0], ds.protocol[1], ds.messageType, ds.messageNumber, ds.componentType[0], ds.componentType[1], ds.version[0], ds.version[1]};

	printf("Send this to get the id \n");
	for (int i = 0; i < 10; i++) {
		printf("%#02x\n", discover[i]);
	}

	// Create a data sending message
	const statementSend ss = sendData(0x0001, 3, 0x1234, 80);

	unsigned char statement[11] = {ss.identification[0], ss.identification[1], ss.protocol[0], ss.protocol[1], ss.messageType, ss.messageNumber, ss.id[0], ss.id[1], ss.data[0], ss.data[1], ss.battery};

	printf("\nThen a statement sent\n");
	for (int i = 0; i < 11; i++) {
		printf("%#02x\n", statement[i]);
	}

	// Parse a discover response message
	unsigned char tmp[] = {0x26, 0x42, 0x02, 3, 0x00, 0x01, 0x00, 0x05, 0x00, 0x1e, 0x05};
	const discoverReceived discovered = parseDiscoverMessage(tmp);

	printf("\nThen a discover received\n");

	printf("%#02x\n", discovered.identification[0]);
	printf("%#02x\n", discovered.identification[1]);

	printf("%#02x\n", discovered.messageType);

	printf("%#02x\n", discovered.messageNumber);

	printf("%#02x\n", discovered.id[0]);
	printf("%#02x\n", discovered.id[1]);

	printf("%#02x\n", discovered.standby[0]);
	printf("%#02x\n", discovered.standby[1]);

	printf("%#02x\n", discovered.timeout[0]);
	printf("%#02x\n", discovered.timeout[1]);

	printf("%#02x\n", discovered.retries);

	// Parse a statmeent response message
	unsigned char tmp1[] = {0x26, 0x42, 0x02, 3, 0x00, 0x01, 0x00, 0x00, 0x05, 0x00, 0x1e, 0x05};
	const statementReceived received = parseStatementMessage(tmp1);

	printf("\nThen a statement received\n");

	printf("%#02x\n", received.identification[0]);
	printf("%#02x\n", received.identification[1]);

	printf("%#02x\n", received.messageType);

	printf("%#02x\n", received.messageNumber);

	printf("%#02x\n", received.id[0]);
	printf("%#02x\n", received.id[1]);

	printf("%#02x\n", received.state);

	printf("%#02x\n", received.standby[0]);
	printf("%#02x\n", received.standby[1]);

	printf("%#02x\n", received.timeout[0]);
	printf("%#02x\n", received.timeout[1]);

	printf("%#02x\n", received.retries);

	return 1;
}
