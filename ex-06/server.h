//
//  server.h
//  ex-06
//
//  Created by Raphael Matile on 04/12/13.
//  Copyright (c) 2013 Raphael Matile. All rights reserved.
//

#ifndef __ex_06__server__
#define __ex_06__server__

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <functional>
#include <errno.h>
#include <stdlib.h>

typedef unsigned int uint;

class server {
public:
    server(std::string pPath);
    ~server() {}
    
    void run();
    
private:
    int server_sock;
    int client_sock;
    size_t recv_msg_length;
    sockaddr_un local;
    sockaddr_un remote;
    socklen_t addr_size;
        
    bool hasWon = false;
    int has_hit_ctr = 0;
    
    void accept_connect();
    std::string recv_msg();
    void send_msg(std::string pMessage);
    
    void initalize_playfield();
    bool checkPlayfield(std::string pCoordinates);
    void drawPlayfield();
    
    std::string const boolToString(bool b) {
        return b ? "true" : "false";
    }
    
    bool stringToBool(std::string const& s) {
        return s=="false" ? false : true;
    }
    
    // row(column)
    // 0 represents hit which didn't hit
    // 1 represents boat
    // 2 represents hit
    std::vector<std::vector<int> > enemy_playfield;
    // 0 represents none
    // 1 represents hit, which didn't hit
    // 2 represents hit, which hit
    std::vector<std::vector<int> > local_playfield;
};

#endif /* defined(__ex_06__server__) */
