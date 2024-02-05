//
// Created by daniel on 06.11.20.
//


#include "server.h"

void help(char *argv[]) {
    std::cout << "Zadal jsi: " << argv[0] << std::endl;
    std::cout << "Zadej adresu a port." << std::endl;
    std::cout << " Například: " << std::endl;
    std::cout << " local 12333 " << std::endl;
    std::cout << " any 12333 " << std::endl;
}

bool Server::prepareToServ(const std::string& address, uint16_t port) {
    int retVal;

    serverSock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serverAddr, 0, sizeof(struct sockaddr_in));
    serverAddr.sin_family = AF_INET;

    if (address == "any") {
        serverAddr.sin_addr.s_addr = htons(INADDR_ANY);
    }
    else if (address == ("local")) {
        serverAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    }
    else if (1 == inet_pton(serverAddr.sin_family, address.c_str(), &serverAddr.sin_addr)){
        std::cout << "zadali jste " << address << std::endl;
    }
    else {
        std::cout << "Vadná IP adresa" << std::endl;
        return false;
    }

    if (port < 40000 && port > 1024) {
        serverAddr.sin_port = htons(port);
    } else {
        std::cout << "Vadný port" << std::endl;
        std::cout << "Rozsah portů je 1 025 - 39 999" << std::endl;
        return false;
    }

    retVal = bind(serverSock, (struct sockaddr *) &serverAddr, sizeof(struct sockaddr_in));
    if (retVal == 0) {
        std::cout << "bind v pořádku" << std::endl;
    } else {
        std::cout << "Server bind ERR" << std::endl;
        return false;
    }

    retVal = listen(serverSock, 10);
    if (retVal == 0) {
        std::cout << "Server naslochá" << std::endl;
    } else {
        std::cout << "Server je neslyší" << std::endl;
        return false;
    }

    FD_ZERO(&clientsockets);
    FD_SET(serverSock, &clientsockets);


    return true;
}


int Server::startServ(const std::string& address, uint16_t port) {
    if (!prepareToServ(address, port)) {
        return 1;
    }
    runningS = true;
    int readable = 0;
    auto time = std::make_unique<timeval>();
    time->tv_usec = 10000;

    while (runningS) {
        fd_set temp{};

        { std::lock_guard<std::mutex> lock(cliMutex);
        temp = clientsockets; }



        if (select(FD_SETSIZE, &temp, (fd_set *)0, (fd_set *)0, time.get()) < 0) {
            std::cout << "select error" << std::endl;
            runningS = false;
            break;
        }
        /** vynechavam stdin, stdout, stderr */
        for (fd = 3; fd < FD_SETSIZE; fd++) {
            if (FD_ISSET(fd, &temp)) {
                if (fd == serverSock) {
                std::lock_guard<std::mutex> lock(cliMutex);
                clientSock = accept(serverSock, (struct sockaddr *) &clientAddr, &cliAddrLen);
                FD_SET(clientSock, &clientsockets);
                std::cout << "Někdo se připojil..." << std::endl;
            }

            else {
                ioctl(fd, FIONREAD, &readable);
                if (readable > 0) {

                    TcpMessage m{};
                     if(m.recvMessage(fd) > 0) cutClient(fd, true);
                     else {
                         processMsg(m);

                     }

                }
                else {
                    cutClient(fd, false);

                }
            }
        }
        }
        checkticks();
    }


    return 0;

}

void Server::cutClient(int cliFd, bool err) {

    std::lock_guard<std::mutex> lock(cliMutex);
    close(cliFd);
    FD_CLR(cliFd, &clientsockets);

    Player *p = getPlayerByID(cliFd);

    if (err) {
                std::cout << "server odpojil klienta " << " kvůli nevalidním zprávám anebo nečinnosti" << std::endl;
            }
    else {
                std::cout << "někdo"  << " se odpojil" << std::endl;
            }
    deletePinGame(p);

    clients.remove(p);
    delete p;

}



Player *Server::getPlayerByID(int id) {
    for (Player *p: clients) {
        if (p->getFd() == id) {
            return p;
        }
    }
    return nullptr;
}


Game *Server::getGameById(const std::string& id) {
    for (Game *game: games) {
        if (game->getName() == id) {
            return game;
        }
    }
    return nullptr;
}

void Server::addToGame(int id,  const std::string& gameId, const std::string& color) {
    Game *game = getGameById(gameId);
    Player *p = getPlayerByID(id);
    p->setGameId(gameId);
    p->setColor(color);
    game->appendPlayer(p);

}

void Server::initGame(const std::string& gameId) {
    TcpMessage r{UNAUTHORIZED, "0"};
    Game *game = getGameById(gameId);
    if (!game) {
        std::cout << "hra neexistuje" << std::endl;
        return;
    }
    if (game->getPlayers().size() >= 2) {
        game->setRuns(true);
        std::string others;

        r.setType(GAME_INIT);
        for (Player* player: game->getPlayers()) {
            for (Player *player2: game->getPlayers()) {
                others += +"-" + player2->getName() + " " + player2->getColor();
            }
            player->setInGame(true);
            r.setMessage(others);
            r.putMessage(player->getFd());
            others = "";
        }
        Player *p = game->turn();
        r.setType(MY_DICE_TURN);
        r.setMessage("0");
        r.putMessage(p->getFd());
        for (Player *player: game->getPlayers()) {
            if (p != player) {
                r.setType(OTHER_DICE_TURN);
                r.setMessage(p->getColor());
                r.putMessage(player->getFd());
            }
        }

    }
}

void Server::keepAlive(int sender) {
    Player *player = getPlayerByID(sender);
    if (player == nullptr)
        return;
    time_t current;
    time(&current);
    player->setLastMsg(current);
    if (player->isAway()) {
        player->setAway(false);

    }

}

void Server::checkticks() {
    std::lock_guard<std::mutex> lock(cliMutex);
    if (clients.empty())
        return;
    time_t current;
    time(&current);
    long timeout = 20;

    for (Player *player: clients) {

        if ((current - player->getLastMsg()) > (2 * timeout) && player->isAway()) {
            cutClient(player->getFd(), true);
            if (player->isInGame()) {
                for (Player *playerInGame: getGameById(player->getGameId())->getPlayers()) {
                    TcpMessage r{OTHER_DISCONNECTED, player->getColor()};
                    r.putMessage(playerInGame->getFd());
                }
                games.remove(getGameById(player->getGameId()));
            }
            return;
        }


        else if ((current - player->getLastMsg()) > timeout && !player->isAway()) {
                player->setAway(true);
                if (player->isInGame()) {
                    getGameById(player->getGameId())->setPlayerAway(true);
                    for (Player *playerInGame: getGameById(player->getGameId())->getPlayers()) {
                        TcpMessage r{AWAY, player->getColor()};
                        r.putMessage(playerInGame->getFd());
                    }
                }
            }





    }
}


void Server::deletePinGame(Player *p) {
    for (Game *game: games) {
        for (Player *player: game->getPlayers()) {
            if (p == player) {
                game->removePlayer(p);
                break;
            }
        }
    }

    int size = games.size();
    for (int i = 0; i < size; i++)
        deleteGame();

}

void Server::deleteGame(){
    auto itr = games.begin();
    int size = games.size();
    for (int i = 0; i < size; i++) {
        std::advance( itr , i);
        if ((itr.operator*()->getPlayers().empty())) {
            std::cout << "mazu zaznam hry " << itr.operator*()->getName() << std::endl;
            games.remove(itr.operator*());
        }
    }
}

void Server::processMsg(TcpMessage msg) {
    TcpMessage r{UNAUTHORIZED, "0"};
    std::string color;
    std::string gameId;
    try {
    switch (msg.getType()) {

            case KEEP_ALIVE: {
                std::lock_guard<std::mutex> lock(cliMutex);
                std::cout << "keep alive " << msg.sender << std::endl;
                keepAlive(msg.sender);
            }
            break;

            case LOGIN_REQUEST:
                std::cout << "LOGIN_REQUEST " << msg.sender << std::endl;
            r.setType(LOGIN_ANSWER);
            {
                auto *p = new Player(msg.getMessage(), msg.sender);
                clients.push_back(p);
            }
            std::cout << "Nově připojený klient se jménem " << msg.getMessage() << std::endl;
            r.putMessage(fd);
            break;

            case GAMES_LIST_REQUEST:
                std::cout << "GAMES_LIST_REQUEST " << msg.sender << std::endl;
            r.setType(GAMES_LIST_ANSWER);
            {
                std::lock_guard<std::mutex> lock(cliMutex);
                keepAlive(msg.sender);
                std::ostringstream m("", std::ios::app);
                for (const Game *game: games) {
                    m << game;
                    m << "-";
                }
                r.setMessage(m.str());
                r.putMessage(fd);
            }
            break;

            case JOIN_REQUEST: {
                std::cout << "JOIN_REQUEST " << msg.sender << std::endl;
                std::lock_guard<std::mutex> lock(cliMutex);
                keepAlive(msg.sender);
                gameId = msg.getMessage().substr(0, msg.getMessage().find(' '));
                color = msg.getMessage().substr(gameId.length() + 1, msg.getMessage().length() - gameId.length());
                if (games.empty()) {
                    r.setMessage("3");
                    r.setType(JOIN_ANSWER);
                    r.putMessage(msg.sender);
                    return;
                }
                else {
                    bool found = false;
                    for (auto &game: games) {
                        if (game->getName() == gameId) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        r.setMessage("3");
                        r.setType(JOIN_ANSWER);
                        r.putMessage(msg.sender);
                        return;
                    }
                }

                if (getGameById(gameId)->getPlayers().size() > 4) {
                    r.setMessage("3");
                    r.setType(JOIN_ANSWER);
                    r.putMessage(msg.sender);
                    return;
                }

                for (Player *player: getGameById(gameId)->getPlayers()) {

                    if (player->getColor() == color) {
                        r.setMessage("1");
                        r.setType(JOIN_ANSWER);
                        r.putMessage(msg.sender);
                        return;

                    }
                    if (player->getFd() == fd) {
                        r.setMessage("2");
                        r.setType(JOIN_ANSWER);
                        r.putMessage(msg.sender);
                        return;
                    }
                }
                addToGame(msg.sender, gameId, color);

                r.setMessage(msg.getMessage() + " " +
                             std::to_string(getGameById(gameId)->getPlayers().size()));

                r.setType(JOIN_ANSWER);
                r.putMessage(msg.sender);
            }
            break;

        case GAME_INIT:
        {
            if (msg.getMessage().empty())
                return;
            std::cout << "GAME_INIT " << msg.sender << std::endl;

            std::lock_guard<std::mutex> lock(cliMutex);
            gameId = msg.getMessage().substr(0, msg.getMessage().find(' '));
            initGame(gameId);
        }
            break;

            case CREATE_GAME_REQUEST:
                std::cout << "CREATE_GAME_REQUEST " << msg.sender << std::endl;

            deleteGame();

            r.setType(CREATE_GAME_ANSWER);
            gameId = msg.getMessage().substr(0, msg.getMessage().find(' '));
            color = msg.getMessage().substr(gameId.length() + 1, msg.getMessage().length() - gameId.length());
            if (!games.empty()) {
                for (const Game *game: games) {
                    if (gameId == game->getName()) {
                        r.setMessage("1");
                        r.putMessage(fd);
                        return;

                    }
                }

            }
            {
                std::lock_guard<std::mutex> lock(cliMutex);
                keepAlive(msg.sender);
                auto *g = new Game(gameId);
                games.push_back(g);
                addToGame(fd, gameId, color);
                gameId += " " + color;
                r.setMessage(gameId);
                r.putMessage(fd);
            }
            break;
            case MY_ROLL: {
                std::cout << "MY_ROLL " << msg.sender << std::endl;

                std::lock_guard<std::mutex> lock(cliMutex);

                // hra skončila
                if (!getGameById(getPlayerByID(msg.sender)->getGameId())) {
                    r.setType(OTHER_DISCONNECTED);
                    r.setMessage(getPlayerByID(msg.sender)->getColor());
                    r.putMessage(msg.sender);
                    break;
                }
                int diceVal = std::stoi(msg.getMessage());
                if (diceVal > 0 && diceVal < 7 && getPlayerByID(msg.sender)->isOnTurn() && !getPlayerByID(msg.sender)->isRolledDice()) {
                    for (Player *player: getGameById(getPlayerByID(msg.sender)->getGameId())->getPlayers()) {
                        if (player != getPlayerByID(msg.sender)) {
                            r.setType(OTHER_ROLL);
                            r.setMessage(msg.getMessage());
                            r.putMessage(player->getFd());
                        }
                    }
                    getGameById(getPlayerByID(msg.sender)->getGameId())->enableTurn();
                }


                keepAlive(msg.sender);
            }
            break;
            case MY_TURN: {
                std::cout << "MY_TURN " << msg.sender << std::endl;

                std::lock_guard<std::mutex> lock(cliMutex);

                if (!getGameById(getPlayerByID(msg.sender)->getGameId())) {
                    r.setType(OTHER_DISCONNECTED);
                    r.setMessage(getPlayerByID(msg.sender)->getColor());
                    r.putMessage(msg.sender);
                    break;
                }


                int pid = std::stoi(msg.getMessage().substr(0, 1));
                int fid = std::stoi(msg.getMessage().substr(2, 1));
                if ( pid >= 0 && pid < 5 && fid < 5 && fid >= 0) {
                    if (getPlayerByID(msg.sender)->isOnTurn()) {
                        for (Player *player: getGameById(getPlayerByID(msg.sender)->getGameId())->getPlayers()) {
                            if (player != getPlayerByID(msg.sender)) {
                                r.setType(OTHER_TURN);
                                r.setMessage(msg.getMessage());
                                r.putMessage(player->getFd());
                            }
                        }
                        getGameById(getPlayerByID(msg.sender)->getGameId())->turnEnd();
                        Player *nowTurn = getGameById(getPlayerByID(msg.sender)->getGameId())->turn();
                        r.setType(MY_DICE_TURN);
                        r.setMessage("0");
                        r.putMessage(nowTurn->getFd());

                        for (Player *player: getGameById(getPlayerByID(msg.sender)->getGameId())->getPlayers()) {
                            if (player != nowTurn) {
                                r.setType(OTHER_DICE_TURN);
                                r.setMessage(nowTurn->getColor());
                                r.putMessage(player->getFd());
                            }
                        }

                    }
                }
                keepAlive(msg.sender);
            }
            break;
            case WINNER_NAME: {
                std::cout << "WINNER_NAME " << msg.sender << std::endl;
                std::lock_guard<std::mutex> lock(cliMutex);
                keepAlive(msg.sender);
            }
            break;

            case RECONNECT_REQUEST: {
            std::cout << "RECONNECT_REQUEST " << msg.sender << std::endl;
            std::lock_guard<std::mutex> lock(cliMutex);
            if (msg.getMessage() != getGameById(msg.getMessage())->getName()){
                std::cout << "reconnect není možný" << std::endl;
                cutClient(msg.sender, true);
            }
            else {
                Player *p = getPlayerByID(msg.sender);
                if (p->isInGame() && p->isAway() && p->getGameId() == msg.getMessage()){
                    p->setAway(false);
                    r.setType(RECONNECT_ANSWER);
                    r.setMessage("0");
                    r.putMessage(msg.sender);

                }
                else{
                    std::cout << "reconnect není možný" << std::endl;
                    r.setType(RECONNECT_ANSWER);
                    r.setMessage("1");
                    r.putMessage(msg.sender);
                }

            }


                keepAlive(msg.sender);
            }
            break;

            case DISCONNECTED: {
                std::cout << "DISCONNECTED " << msg.sender << std::endl;

                std::lock_guard<std::mutex> lock(cliMutex);


                if (!getPlayerByID(msg.sender)->isInGame())
                    break;

                std::list<Player *> players = getGameById(getPlayerByID(msg.sender)->getGameId())->getPlayers();
                for (Player *player: players) {
                    r.setType(OTHER_DISCONNECTED);
                    r.setMessage(getPlayerByID(msg.sender)->getColor());
                    r.putMessage(player->getFd());
                    deletePinGame(getPlayerByID(player->getFd()));
                }
            }
            break;

        }

    }
    catch (std::exception &e) {
        std::cout << "chyba dat zprávy" << std::endl;
       cutClient(msg.sender, true);
        return;
    }
}


