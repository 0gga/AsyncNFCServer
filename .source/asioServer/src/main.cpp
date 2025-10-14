#include "Reader.h"

int main() {
	Reader reader1(3, 9000);

	while (true) {
		reader1.tick();
	}
}
