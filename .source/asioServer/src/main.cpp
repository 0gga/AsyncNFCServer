#include "Reader.h"

int main() {
	Reader reader(3, 9000, 9001);

	while (reader.isRunning()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	std::cout << "Server Shutting Down" << std::endl;

	return 0;
}