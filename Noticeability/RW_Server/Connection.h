#ifndef RW_SERVER_CONNECTION_H
#define RW_SERVER_CONNECTION_H
#include <winsock2.h>
#include <cstdio>
#include "Game.h"
#include "utils/utils.h"
#include "config.h"
#include "utils/Logger.h"

#pragma comment(lib,"ws2_32.lib")
#define BUFFLEN 512

class Connection {
public:
    Connection(Game*, Logger*, Logger*);
    ~Connection();
    /**
     * Bind to UDP socket.
     */
    void bindSocket();
    /**
     * Listen for incoming UDP packages.
     */
    void listen();
    /**
     * Parse received messages and store them.
     * @return True if positional coordinates were received indicating a need for redirection, false otherwise.
     */
    bool parse(const char*);
private:
    SOCKET s;
    struct sockaddr_in server{}, si_other{};
    int slen , recv_len{};
    char buf[BUFFLEN]{};
    WSADATA wsa{};
    Game* game;
    Logger* env_logger;
    Logger* user_logger;
};


#endif //RW_SERVER_CONNECTION_H
