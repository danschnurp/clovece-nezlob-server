//
// Created by daniel on 17.11.2020.
//

#ifndef UPSCLOVECESERVER_PLAYER_H
#define UPSCLOVECESERVER_PLAYER_H

#include <iostream>
#include <vector>
#include <ctime>

/**
 * hráč
 */
class Player {
private:
    /**
     * jméno
     */
    std::string name;

    /**
     * id filedeskriptoru
     */
    int fd;

    /**
     * je ve hře
     */
    bool inGame;

    /**
     * je na tahu?
     */
    bool onTurn;

    /**
     * hodil už hráč kostkou?
     */
    bool rolledDice;

    /**
     * barva figurek
     */
    std::string color;

    /**
     * figurky
     */
    std::vector<int> figs;

    /**
     * je hráč aktivní?
     */
    bool away;

    /**
     * čas od poslední zprávy
     */
    time_t lastMsg{};

    /**
     * id hry, ve ktré je hráč
     */
     std::string gameId;

public:

    /**
     * konstruktor
     * @param name jméno
     * @param fd id
     */
    Player(std::string &name, int fd);

    /**
     * getr
     * @return jmeno
     */
    const std::string &getName() const;

    /**
 * getr
 * @return id
 */
    int getFd() const;

    /**
 * getr
 * @return je ve hře?
 */
    bool isInGame() const;

    /**
 * getr
 * @return barva
 */
    const std::string &getColor() const;

    /**
 * getr
 * @return figurky
 */
    const std::vector<int> &getFigs() const;

    /**
     * setr
     * @param inGame bool
     */
    void setInGame(bool in);

    /**
     * setr
     * @param color barva
     */
    void setColor(const std::string &col);

    /**
     * getr
     * @return aktivnost hráče
     */
    bool isAway() const;

    /**
     * setr
     * @param a aktivnost hráče
     */
    void setAway(bool a);

    /**
     * getr
     * @return čas od poslední zprávy
     */
    time_t getLastMsg() const;

    /**
     * setr
     * @param last čas od poslední zprávy
     */
    void setLastMsg(time_t last);

    const std::string &getGameId() const;

    void setGameId(const std::string &Id);

    bool isOnTurn() const;

    void setOnTurn(bool turn);

    bool isRolledDice() const;

    void setRolledDice(bool rolledice);

    bool operator==(const Player &rhs) const;

    bool operator!=(const Player &rhs) const;


};


#endif //UPSCLOVECESERVER_PLAYER_H
