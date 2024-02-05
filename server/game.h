//
// Created by daniel on 17.11.2020.
//

#ifndef UPSCLOVECESERVER_GAME_H
#define UPSCLOVECESERVER_GAME_H

#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include "player.h"

class Game {
private:

    /**
     * jméno hry (je to i ID)
     */
    std::string name;

    /**
     * jede hra?
     */
    bool runs;

    /**
     * je hráč pryč?
     */
    bool playerAway;

    /**
     * hráči ve hře
     */
    std::list<Player *> players;

    /**
     * id hráče  co je na tahu
     */
    int playerTurnId;

public:

    /**
     * konstruktor
     * @param name jméno
     */
    explicit Game(std::string &name);

    /**
     * getr
     * @return jméno
     */
    const std::string &getName() const;
    /**
     * getr
     * @return je aktivní?
     */
    bool isRuns() const;

    /**,
     * getr
     * @return players
     */
    std::list<Player *> &getPlayers();

    /**
     * setr
     * @param n jméno
     */
    void setName(const std::string &n);

    /**
     * setr
     * @param r je hra aktivní?
     */
    void setRuns(bool r);

    /**
     * setr
     * @param p hráči
     */
    void setPlayers(std::list<Player *> &p);

    bool isPlayerAway() const;

    void setPlayerAway(bool pAway);

    /**
     * přidá hráče
     * @param p hráč
     */
    void appendPlayer(Player *p);

    /**
     * tah (kostka)
     */
    Player* turn();

    /**
     * konec tahu
     */
    void turnEnd();

    /**
     * umožní tah podle kostky
     */
    void enableTurn();

    /**
     * odstraní hráče
     * @param p hráč
     */
    void removePlayer(Player *p);

    friend std::ostream &operator<<(std::ostream &os, const Game *game);

};


#endif //UPSCLOVECESERVER_GAME_H
