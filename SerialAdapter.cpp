#include "SerialAdapter.h"

speed_t SerialAdapter::toBaud(int rate) {
	switch (rate) {
		case 9600:
			return B9600;
		case 19200:
			return B19200;
		case 38400:
			return B38400;
		default: 
			return -1;
    }
}

SerialAdapter::SerialAdapter() {
	path = (char*) "";
	flags = -1;
	conn = -1;
	connected = false;
	baud = -1;
}

SerialAdapter::SerialAdapter(char* path, int flags) {
	this->path = path;
	this->flags = flags;
	conn = -1;
	connected = false;
	baud = -1;
}

SerialAdapter::SerialAdapter(char* path, int flags, int rate) {
	this->path = path;
	this->flags = flags;
	conn = -1;
	connected = false;
	baud = toBaud(rate);
	connect();
}

SerialAdapter::~SerialAdapter() {
	disconnect();
}

void SerialAdapter::setPath(char* path) {
	this->path = path;
}

void SerialAdapter::setFlags(int flags) {
	this->flags = flags;
}

void SerialAdapter::addFlag(int flag) {
	if(flags > -1) flags |= flag;
	else flags = flag;
}

void SerialAdapter::setRate(int rate) {
	baud = toBaud(rate);
}

bool SerialAdapter::connect() {
	if(connected) {
		std::cout << "Serial already connected!" << std::endl;
		return false;
	}

	bool passed = true;

	if(strlen(path) == 0) {
		std::cout << "Serial path not set!" << std::endl;
		passed = false;
	}

	if(flags < 0) {
		std::cout << "Access flags not set!" << std::endl;
		passed = false;
	}

	if(baud < 0) {
		std::cout << "Baud rate not set!" << std::endl;
		passed = false;
	}
	
	if(!passed) return false;

	if((conn = open(path, flags)) < 0) {
		std::cout << "Serial device not connected!" << std::endl;
	};

	if(flock(conn, LOCK_EX | LOCK_NB) == -1) {
		std::cout << "ERROR: Serial port with path '" << path << "' is already locked by another process." << std::endl;
        return false;
	}

	struct termios tty;

	if(tcgetattr(conn, &tty) != 0) {
		std::cout << "ERROR " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
		return false;
	}

    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size 
    tty.c_cflag |= CS8; // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
    // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

    tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    cfsetispeed(&tty, baud);
    cfsetospeed(&tty, baud);

	// Save tty settings, also checking for error
    if (tcsetattr(conn, TCSANOW, &tty) != 0) {
        std::cout << "ERROR " << errno << " from tcsetattr: " << strerror(errno) << std::endl;
        return false;
    }

    connected = true;
	return true;
}

bool SerialAdapter::connect(int rate) {
	setRate(rate);
	return connect();
}

void SerialAdapter::disconnect() {
	if(connected) {
		close(conn);
		connected = false;
	}
}

bool SerialAdapter::SerialWrite(char* buf, int size) {
	return (write(conn, buf, size) == size);
}

bool SerialAdapter::SerialRead(int size, char* buf) {
	if(read(conn, buf, size) > 0 ) return true;

	return false;
}

bool SerialAdapter::SerialRead(char* &buf) {
	buf = new char[1028];
	if(read(conn, buf, 1028) > 0) return true;

	delete[] buf;
	return false;
}

char* SerialAdapter::SerialRead(int size) {
	char* buf = new char[size];
	SerialRead(size, buf);
	return buf;
}

char* SerialAdapter::SerialRead() {
	return SerialRead(1028);
}