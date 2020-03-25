#include <nds.h>
#include <cstring>
#include "debug.h"
void debugMessage(const char* msg) {
	fifoSendDatamsg(FIFO_USER_08, strlen(msg) + 1, (u8*)msg);
}