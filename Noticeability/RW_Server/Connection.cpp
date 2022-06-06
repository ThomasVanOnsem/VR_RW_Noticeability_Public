#include <iostream>
#include "Connection.h"


Connection::Connection(Game* game1, Logger* env, Logger* user) {
    game = game1;
    env_logger = env;
    user_logger = user;
    slen = sizeof(si_other);

    //Initialise winsock
    printf("\nInitialising Winsock...");
    if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Initialised.\n");

    if((s = socket(AF_INET, SOCK_DGRAM, 0 )) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d" , WSAGetLastError());
    }
    printf("Socket created.\n");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(config::port);
}

Connection::~Connection() {
    closesocket(s);
    WSACleanup();
}

void Connection::bindSocket() {
    if(bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d\n" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Bind done on port %i.\n", config::port);
}

void Connection::listen() {
    while(true)
    {
        //Clear the buffer by filling null, it might have previously received data
        memset(buf,'\0', BUFFLEN);

        //Try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
        {
            printf("recvfrom() failed with error code : %d\n" , WSAGetLastError());
            exit(EXIT_FAILURE);
        }

        //Parse the received data
        bool send_pos = parse(inet_ntoa(si_other.sin_addr));  //TODO does not handle IPv6?
        //If a user's position was sent, calculate the redirection coordinates.
        if (send_pos){
            std::pair<bool, double> next_pos = game->getRWAlgorithm()->predictRotation(game, inet_ntoa(si_other.sin_addr));
            std::string return_data = std::to_string(next_pos.second);
            if(next_pos.first){
                return_data = "RESET:" + return_data;
            }
            std::cout << "Recommend rotation 3: " << return_data << std::endl;
            user_logger->log("Recommend Rotation: " + return_data, inet_ntoa(si_other.sin_addr));

            //Reply to user with new coordinates
            int n = return_data.size();
            char data[n+1];
            strcpy(data, return_data.c_str());
            if (sendto(s, data, n+1, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
            {
                printf("sendto() failed with error code : %d\n" , WSAGetLastError());
                exit(EXIT_FAILURE);
            }
        }
    }
}


bool Connection::parse(const char* client_id) {
    bool pos_received = false;
    //Split possible pos and rot data
    std::vector<std::string> received = custom_split(buf, '|');
    for(const std::string& str: received){
        //Split header from data
        std::vector<std::string> data = custom_split(str, ':');
        if(data.size() < 2){
            std::cout << "ERROR: Could not parse received data!" << std::endl;
            return false;
        }

        //Parse coordinates and add to correct data storage
        Coordinate coordinate = getCoordinateFromString(data[1]);
        if(data[0] == config::indicator_border){
            if(!config::use_guardian){
                continue;
            }
            game->addToBorder(coordinate);
            env_logger->log("Border point: " + coordinate.asString());
        }
        else if(data[0] == config::indicator_player_pos){
            // Mark game as setup because all border data had to have been sent before user position data.
            // Also avoid resetting the environment as second user when it has already been set by the first one.
            if(!game->getEnvAdded()){
                game->setEnvAdded();
            }
            std::cout << "Receiving pos: " << '(' << coordinate.x << ',' << coordinate.y << ',' << coordinate.z << ')' << std::endl;
            game->updatePlayerPos(client_id, coordinate);
            pos_received = true;
            user_logger->log("Received virt pos: " + coordinate.asString(), client_id);
            user_logger->log("Received real pos: " + game->getPlayer(client_id)->getLocation().asString(), client_id);
        }
        else if(data[0] == config::indicator_player_rot){
            //std::cout << "Receiving rot: " << '(' << coordinate.x << ',' << coordinate.y << ',' << coordinate.z << ')' << std::endl;
            game->updatePlayerRot(client_id, coordinate);
            //user_logger->log("Received rot: " + coordinate.asString(), client_id);
        }
        else{
            std::cout << "ERROR: Unrecognized message format, unable to parse!" << std::endl;
            std::cout << buf << std::endl;
        }
    }
    return pos_received;
}