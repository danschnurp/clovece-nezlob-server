//
// Created by daniel on 17.11.2020.
//


#include "game.h"

Game::Game(std::string &name) : name(name) {
    runs = false;
    playerAway = false;
    playerTurnId = -1;
}

const std::string &Game::getName() const {
    return name;
}

bool Game::isRuns() const {
    return runs;
}

std::list<Player *> &Game::getPlayers() {
    return players;
}

void Game::setName(const std::string &n) {
    Game::name = n;
}

void Game::setRuns(bool r) {
    Game::runs = r;
}

void Game::setPlayers(std::list<Player *> &p) {
    Game::players = p;
}

void Game::appendPlayer(Player *p) {
    players.push_back(p);
}

std::ostream &operator<<(std::ostream &os, const Game *game) {
    os << game->name << " " << game->runs << " " << game->players.size();
    for (Player *player: game->players) {
        os << " " << player->getColor();
    }

    return os;
}

void Game::removePlayer(Player *p) {
    p->setInGame(false);
    players.remove(p);
}

bool Game::isPlayerAway() const {
    return playerAway;
}

void Game::setPlayerAway(bool pAway) {
    Game::playerAway = pAway;
}

Player *Game::turn() {
    playerTurnId++;
    if (playerTurnId >= players.size()) //todo ????
        playerTurnId = 0;
    auto it = players.begin();
    std::advance(it, playerTurnId);
    it.operator*()->setOnTurn(true);
    it.operator*()->setRolledDice(false);
    return it.operator*();
}


void Game::turnEnd() {
    auto it = players.begin();
    std::advance(it, playerTurnId);
    it.operator*()->setOnTurn(false);
}

void Game::enableTurn() {
    auto it = players.begin();
    std::advance(it, playerTurnId);
    it.operator*()->setRolledDice(true);
}








