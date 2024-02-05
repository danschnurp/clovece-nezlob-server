//
// Created by daniel on 10.11.20.
//


#include "message.h"

TcpMessage::TcpMessage() {
    type = UNAUTHORIZED;
    message = "";
}

TcpMessage::TcpMessage(EMsgType type, const std::string &message) : type(type), message(message) {
    lenMsg = message.length();
}

std::string &TcpMessage::getMessage() {
    return message;
}

int TcpMessage::getLenMsg() {
    return lenMsg;
}

EMsgType TcpMessage::getType() {
    return type;
}

std::string TcpMessage::getSECURITY() {
    return "CLOV2UPS";
}

void TcpMessage::putMessage(int cliFd) {
    std::string data{getSECURITY() + ";" + std::to_string(getType()) + ";"
                                            + std::to_string(getLenMsg()) + ";" + getMessage() + "\n"};

    send(cliFd, data.c_str(), data.length(), 0);

    std::cout << "Posílám: " << cliFd << std::endl << data << std::endl;
}

int TcpMessage::recvMessage(int cliFd) {
    std::ostringstream line("", std::ios::app);

    for (;;) {
        char c;
        if (recv(cliFd, &c, 1, MSG_DONTWAIT | MSG_PEEK) <= 0) {
            std::cout << "Neautorizovaná zpráva." << std::endl;
            type = UNAUTHORIZED;
            return 1;
        }
        recv(cliFd, &c, 1, 0);
        if (c == '\n') {
            break;
        }
        else if (c == '\r') continue;


        line << c;

    }

    std::string msg = line.str();

    if (msg.length() == 0) return 0;

    std::string_view sv(msg);
    if (sv.substr(0, getSECURITY().length()) != getSECURITY()) {
        std::cout << "Neautorizovaná zpráva" << std::endl;
        type = UNAUTHORIZED;
        return 2;
    }

    try {

        // jedná se o jednociferný typ zprávy
        if (sv.substr(getSECURITY().length() + 2, 1) == ";") {
            try {
                type = static_cast<EMsgType>(std::stoi(msg.substr(getSECURITY().length() + 1, 1), nullptr, 10));
            }
            catch (std::exception &e) {
                std::cout << "chyba formátu zprávy" << std::endl;
                type = UNAUTHORIZED;
                return 4;
            }
        }
            // dvojciferný typ zprávy
        else {
            try {
                type = static_cast<EMsgType>(std::stoi(msg.substr(getSECURITY().length() + 1, 2), nullptr, 10));
            }
            catch (std::exception &e) {
                std::cout << "chyba formátu zprávy" << std::endl;
                type = UNAUTHORIZED;
                return 4;
            }
        }

        int offset;
        if (type < 10) {
            offset = 2;
        } else offset = 3;

        //  jedná se o jednocifernou délku zprávy
        if (sv.substr(getSECURITY().length() + offset + 2, 1) == ";") {
            try {
                lenMsg = static_cast<int>(std::stoi(msg.substr(getSECURITY().length() + offset + 1, 1), nullptr, 10));
            }
            catch (std::exception &e) {
                std::cout << "chyba formátu zprávy" << std::endl;
                type = UNAUTHORIZED;
                return 4;
            }
        } else  {// dvojciferná délka
            try {
            lenMsg = static_cast<int>(std::stoi(msg.substr(getSECURITY().length() + offset + 1, 2), nullptr, 10));
            }
            catch (std::exception &e) {
                std::cout << "chyba formátu zprávy" << std::endl;
                type = UNAUTHORIZED;
                return 4;
            }
    }
        if (lenMsg < 10) {
            offset += 2;
        } else offset += 3;

        message = msg.substr(getSECURITY().length() + offset + 1, msg.length());

        if ((lenMsg - 1) != static_cast<int>(message.length())) {
            std::cout << "data mají špatnou délku" << std::endl;
            type = UNAUTHORIZED;

            return 3;
        }


    }
    catch (std::exception &e) {
        std::cout << "chyba formátu zprávy" << std::endl;
        type = UNAUTHORIZED;
        return 4;

    }
    sender = cliFd;
    return 0;

}

void TcpMessage::setType(EMsgType t) {
    TcpMessage::type = t;
}

void TcpMessage::setMessage(const std::string &msg) {
    TcpMessage::lenMsg = msg.length();
    TcpMessage::message = msg;
}






