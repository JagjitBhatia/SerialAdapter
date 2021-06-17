#include "SerialAdapter.h"

// Example code

int main() {
	char* buf = new char[128];
	
	SerialAdapter serial = SerialAdapter((char*) "/dev/ttys0", O_RDWR, 9600);
	serial.SerialRead(128, buf);

	std::cout << "Buffer: " << buf << std::endl;

	buf = (char*) "Let's write to this!";
	serial.SerialWrite(buf, sizeof(buf));
}