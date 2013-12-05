//
//  client.h
//  ex-06
//
//  Created by Raphael Matile on 04/12/13.
//  Copyright (c) 2013 Raphael Matile. All rights reserved.
//

#ifndef __ex_06__client__
#define __ex_06__client__

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <functional>
#include <errno.h>

typedef unsigned int uint;

class client {
public:
    client(std::string pPath);
    ~client() {};
    
    void run();
    
private:
    int client_sock;
    size_t recv_msg_length;
    socklen_t addr_size;
    struct sockaddr_un remote;
    
    bool hasWon = false;
    
    int has_hit_ctr = 0;
    
    void send_msg(std::string pMessage);
    std::string recv_msg();
    
    std::string const boolToString(bool b) {
        return b ? "true" : "false";
    }
    
    bool stringToBool(std::string const& s) {
        return s=="false" ? false : true;
    }
    
    bool checkPlayfield(std::string pCoordinates);
    
    void drawPlayfield();
    
    void initalize_playfield();
    // row(column)
    // 0 represents shot which didn't hit
    // 1 represents boat
    // 2 represents hit
    std::vector<std::vector<int> > enemy_playfield;
    // 0 represents none
    // 1 represents shot, which didn't hit
    // 2 represents shot, which hit
    std::vector<std::vector<int> > local_playfield;

};

#endif /* defined(__ex_06__client__) */
