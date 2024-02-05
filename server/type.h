//
// Created by daniel on 10.11.20.
//

#ifndef UPSCLOVECESERVER_TYPE_H
#define UPSCLOVECESERVER_TYPE_H

/**
 * typy zpráv
 */
enum EMsgType {

    // MSG_TYPE             // SENDER // poznámky

    LOGIN_REQUEST,          // client
    LOGIN_ANSWER,           // server

    GAMES_LIST_REQUEST,     // client
    GAMES_LIST_ANSWER,      // server

    JOIN_REQUEST,           // client
    JOIN_ANSWER,            // server

    CREATE_GAME_REQUEST,    // client
    CREATE_GAME_ANSWER,     // server

    GAME_INIT,              // server

    MY_DICE_TURN,                // server
    OTHER_DICE_TURN,             // server

    MY_ROLL,                // client
    OTHER_ROLL,             // server // neposílat klientovi co posílal MY_ROLL

    MY_TURN,                // client // bere se i vyhození figurky
    OTHER_TURN,             // server // bere se i vyhození figurky, neposílat klientovi co posílal MY_TURN

    WINNER_NAME,            // server

    OTHER_DISCONNECTED,     // server // hra se pozastaví

    DISCONNECTED,           // client

    KEEP_ALIVE,              // client + server

    AWAY,                   // server

    RECONNECT_REQUEST,      // client

    RECONNECT_ANSWER,       //server

    // zabezpečení

    CLOV2UPS,

    // neautorizovaný typ
    UNAUTHORIZED




};

#endif //UPSCLOVECESERVER_TYPE_H
