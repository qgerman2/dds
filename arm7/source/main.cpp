#include <nds.h>
#include <stdio.h>
#include <dswifi7.h>
#include <maxmod7.h>
#include "main.h"
#include "debug.h"

using namespace std;

void VblankHandler(void) {
	Wifi_Update();
}

void VcountHandler() {
	inputGetAndSend();
}

volatile bool exitflag = false;

void powerButtonCB() {
	exitflag = true;
}

int main() {
	// clear sound registers
	dmaFillWords(0, (void*)0x04000400, 0x100);
	REG_SOUNDCNT |= SOUND_ENABLE;
	writePowerManagement(PM_CONTROL_REG, ( readPowerManagement(PM_CONTROL_REG) & ~PM_SOUND_MUTE ) | PM_SOUND_AMP );
	powerOn(POWER_SOUND);
	readUserSettings();
	ledBlink(0);
	irqInit();
	initClockIRQ();
	fifoInit();
	touchInit();
	mmInstall(FIFO_MAXMOD);
	SetYtrigger(80);
	installWifiFIFO();
	installSoundFIFO();
	installSystemFIFO();
	irqSet(IRQ_VCOUNT, VcountHandler);
	irqSet(IRQ_VBLANK, VblankHandler);
	irqEnable( IRQ_VBLANK | IRQ_VCOUNT | IRQ_NETWORK);
	setPowerButtonCB(powerButtonCB);
	debugMessage("hola");
	while (!exitflag) {
		if ( 0 == (REG_KEYINPUT & (KEY_SELECT | KEY_START | KEY_L | KEY_R))) {
			exitflag = true;
		}
		swiWaitForVBlank();
	}
	return 0;
}