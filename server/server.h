//
// Created by daniel on 06.11.20.
//

#ifndef UPSCLOVECESERVER_SERVER_H
#define UPSCLOVECESERVER_SERVER_H


#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <list>
#include <mutex>
#include <memory>
#include <sys/ioctl.h>
#include <ctime>

#include "message.h"
#include "player.h"
#include "game.h"

/**
 * nápověda
 * @param argv parametry příkazové řádky
 */
void help(char *argv[]);

/**
 * server
 */
class Server {

    /**
     * socket serveru
     */
    int serverSock{};

    /**
     * souborový descriptor posledního klienta
     */
    int fd{};

    /**
     * socket klienta
     */
    int clientSock{};

    /**
     * délka adresy klienta
     */
    socklen_t cliAddrLen{};

    /**
     * adresa klienta
     */
    struct sockaddr_in clientAddr{};

    /**
     * adresa serveru
     */
    struct sockaddr_in serverAddr{};

    /**
     * set socketů klientů
     */
    fd_set clientsockets{};

    /**
 * řídí nekonečnou smyčku serveru
 */
    bool runningS = false;

    /**
     * seznam připojených hráčů
     */
    std::list<Player *> clients;

    /**
     * seznam volných her
    */
    std::list<Game *> games;


    /**
 * zamkne klienta... slouží k paralelizaci
 */
    std::mutex cliMutex;

    /**
     * přidá hráče do hry
     * @param id id hráče
     * @param gameId hra
     * @param color barva
     */
    void addToGame(int id,  const std::string& gameId, const std::string& color) ;

    /**
     * kontroluje vstup od uživatele
     * @param address localhost nebo veřejná IP adresa
     * @param port
     * @return signál úspěchu/neúspěchu
     */
    bool prepareToServ(const std::string& address, uint16_t port);



    /**
     * odpojí klienta
     * @param cliFd
     * @param err odstřihne klienta kvůli nevalidním zprávám
     */
    void cutClient(int cliFd, bool err);

    /**
     * zpracuje zprávu podle typu
     * @param msg přijatá zpráva
     */
    void processMsg(TcpMessage msg);

    /**
     * vybere hráče ze senzamu klientů
     * @param id id
     * @return instance hráče
     */
    Player * getPlayerByID(int id);


    /**
     * vybere hru
     * @param id jméno (unikátní)
     * @return hra
     */
    Game *getGameById(const std::string& id);


    /**
     * začne hru
     * @param gameId jméno hry
     */
    void initGame(const std::string& gameId);

    /**
     * resetuje tiky u hráče
     * @param sender hráčův filedescriptor
     */
    void keepAlive(int sender);

    /**
     * odpojí neaktivní hráče
     */
    void checkticks();

    /**
     * odstraní hráče ze hry
     * @param p hráč
     */
    void deletePinGame(Player *p);

    /**
     * smaže prázdnou hru
     */
    void deleteGame();


public:

    /**
     * staruje server
     * @param address
     * @param port
     * @return signál úspěchu/neúspěchu
     */
    int startServ(const std::string& address, uint16_t port);


};


#endif //UPSCLOVECESERVER_SERVER_H
