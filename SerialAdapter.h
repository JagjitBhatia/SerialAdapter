#ifndef SERIAL_H
#define SERIAL_H

// C library headers
#include <iostream>
#include <string.h>

// Linux headers
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/file.h>

class SerialAdapter {
	private:
		char* path;
		int flags;
		int conn;
		bool connected;
		int baud;
		

		speed_t toBaud(int rate);

	public:
		SerialAdapter();
		SerialAdapter(char* path, int flags);
		SerialAdapter(char* path, int flags, int rate);
		~SerialAdapter();
		void setPath(char* path);
		void setFlags(int flags);
		void addFlag(int flag);
		void setRate(int rate);
		bool connect();
		bool connect(int rate);
		void disconnect();
		bool SerialWrite(char* buf, int size);
		bool SerialRead(int size, char* buf);
		bool SerialRead(char* &buf);
		char* SerialRead(int size);
		char* SerialRead();
};

#endif