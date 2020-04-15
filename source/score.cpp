#include <nds.h>
#include "globals.h"
#include "score.h"

using namespace std;

Score::Score() {

}

Score::~Score() {

}

void Score::loop() {
	while (1) {
		render();
		swiWaitForVBlank();
	}
}

void Score::render() {

}