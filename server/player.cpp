//
// Created by daniel on 17.11.2020.
//

#include "player.h"

Player::Player(std::string &name, int fd) : name(name), fd(fd) {
    figs = {0, 1, 2, 3};
    inGame = false;
    away = false;
    rolledDice = true;
    onTurn = false;
    time(&lastMsg);
}

const std::string &Player::getName() const {
    return name;
}

int Player::getFd() const {
    return fd;
}

bool Player::isInGame() const {
    return inGame;
}

const std::string &Player::getColor() const {
    return color;
}

const std::vector<int> &Player::getFigs() const {
    return figs;
}

void Player::setInGame(bool in) {
    Player::inGame = in;
}

void Player::setColor(const std::string &col) {
    Player::color = col;
}

bool Player::operator==(const Player &rhs) const {
    return fd == rhs.fd;
}

bool Player::operator!=(const Player &rhs) const {
    return !(rhs == *this);
}

bool Player::isAway() const {
    return away;
}

void Player::setAway(bool a) {
    Player::away = a;
}

time_t Player::getLastMsg() const {
    return lastMsg;
}

void Player::setLastMsg(time_t last) {
    Player::lastMsg = last;
}

const std::string &Player::getGameId() const {
    return gameId;
}

void Player::setGameId(const std::string &Id) {
    Player::gameId = Id;
}

bool Player::isOnTurn() const {
    return onTurn;
}

void Player::setOnTurn(bool turn) {
    Player::onTurn = turn;
}

bool Player::isRolledDice() const {
    return rolledDice;
}

void Player::setRolledDice(bool rolledice) {
    Player::rolledDice = rolledice;
}

