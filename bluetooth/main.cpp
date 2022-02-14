#include <exception>
#include <iostream>
#include <stdexcept>
#include "bluetooth_functions.hpp"

using std::cout, std::cin, std::endl;

int main() {
	cout << "File Transfer: \n"
	     << "-----------------\n"
	     << "1. Send file\n"
	     << "2. Receive file\n"
	     << "\nAudio Transmission: \n"
	     << "3. Transmit audio to device\n"
	     << "4. Receive and play audio from device\n";

	int ch;
	cout << "\nApna option chose kare :) -> ";
	cin >> ch;

	try {
		switch( ch ) {
			case 1:
				send_file();
				break;
			case 2:
				receive_file();
				break;
			case 3:
				trasmit_audio();
				break;
			case 4: receive_audio();
				break;
				// For default, we can have while loop, lekin rehne de isme
			default: throw std::logic_error("Invalid option");
		}
	} catch (std::exception e) {
		std::cerr << "ERROR: " << e.what() << endl;
		return 1;
	}
}

