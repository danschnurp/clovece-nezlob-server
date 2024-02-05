//
// Created by daniel on 06.11.20.
//

#include "server.h"


/**
 * hlavní spouštěcí metoda
 * @param argc počet parametrů
 * @param argv parametry
 * @return signál
 */
int main(int argc, char *argv[]) {


	if (argc != 3) {
		help(argv);
		return 1;
	}
	uint16_t port;
	try {
		port = (uint16_t) std::stoi(argv[2], NULL, 10);
	} catch (std::out_of_range &e) {
	    std::cout << "Špatný formát portu" << std::endl;
        return 6;
	}

	Server s{};
    return s.startServ(argv[1], port);

}
