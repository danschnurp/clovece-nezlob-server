//
// Created by daniel on 10.11.20.
//

#ifndef UPSCLOVECESERVER_MESSAGE_H
#define UPSCLOVECESERVER_MESSAGE_H

#include <string>
#include <sys/socket.h>
#include <iostream>
#include <sstream>

#include "type.h"

/**
 * zpráva
 */
class TcpMessage {

private:

    /**
     * typ zprávy
     */
    EMsgType type{};

    /**
     * data zprávy
     */
    std::string message;

    /**
     * délka dat
     */
    int lenMsg{};

public:

    /**
     * fd odesílatele
     */
    int sender;

    /**
     * konstruktor prázdné zprávy
     */
    TcpMessage();

    /**
     * konstruktor zprávy
     * @param type
     * @param message
     */
    TcpMessage(EMsgType type, const std::string &message);

    /**
     * getr zprávy
     * @return zpráva
     */
    std::string &getMessage();

    /**
     * getr délky zprávy
     * @return délka zprávy
     */
    int getLenMsg();

    /**
     * getr typu zprávy
     * @return typ zprávy
     */
    EMsgType getType();

    /**
     * nastaví typ zprávy
     * @param type nový typ
     */
    void setType(EMsgType type);

    /**
     * nastaví novou zprávu
     * @param message nová zpráva
     */
    void setMessage(const std::string &message);

    /**
     * getr zabezpečovací hlavičky
     * @return zabezpečovací hlavička zprávy
     */
    std::string getSECURITY();

    /**
     * pošle zprávu danému klientovi
     * @param cliFd
     */
    void putMessage(int cliFd);

    /**
     * přijme zprávu od daného klienta
     * @param cliFd
     * @return 0 když ok, jinak chyby
     */
    int recvMessage(int cliFd);



};


#endif //UPSCLOVECESERVER_MESSAGE_H
