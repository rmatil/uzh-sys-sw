//
//  client.cpp
//  ex-06
//
//  Created by Raphael Matile on 04/12/13.
//  Copyright (c) 2013 Raphael Matile. All rights reserved.
//

#include "client.h"

client::client(std::string pPath) {
    initalize_playfield();
    
    // create socket
    if ((client_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(errno);
    }
    
    std::cout << "Establish connection to server..." << std::endl;
    
    // connect to server
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, (pPath.c_str()));
    addr_size = (uint) strlen(remote.sun_path + '\0') + sizeof(remote.sun_family)+1;
    if (connect(client_sock, (struct sockaddr*)&remote, addr_size) == -1) {
        perror("connect");
        exit(errno);
    }
    
    std::cout << "Connected." << std::endl;
}

void client::initalize_playfield() {
    // fill vector with zeros
    std::vector<int> vec;
	vec.assign(5, 0);
	local_playfield.assign(5, vec);
    enemy_playfield.assign(5, vec);
    
    std::cout << "Type in the position where the boat should be placed. [Range: 1-5]:" << std::endl;
    for (int i=0; i<5; i++) {
        std::cout << "Boat no " << i+1 << std::endl;
        std::cout << "xPos > ";
        int xPos;
        int yPos;
        std::cin >> xPos;
        std::cout << "yPos > ";
        std::cin >> yPos;
        local_playfield.at(yPos-1).at(xPos-1) = 1;
    }
}

void client::send_msg(std::string pMessage) {
   // std::cout << "Sent> ";
    if (write(client_sock, pMessage.c_str()+'\0', strlen(pMessage.c_str())+1) == -1) {
        perror("write");
        close(client_sock);
        exit(errno);
    }
    //std::cout << pMessage << std::endl;
}

std::string client::recv_msg() {
    char buffer[100];
    if ((recv_msg_length = read(client_sock, buffer, 100)) == -1) {
        perror("read");
        close(client_sock);
        exit(errno);
    } else if (recv_msg_length > 0) {
     //   std::cout << "received msg: " << buffer << std::endl;
    }
    return std::string(buffer);
}

bool client::checkPlayfield(std::string pCoordinates) {
    //std::cout << "'" << pCoordinates << "'" << std::endl;

    int xPos = std::atoi(pCoordinates.substr(0, 1).c_str()) - 1;
    int yPos = std::atoi(pCoordinates.substr(2, 1).c_str()) - 1;
    
    //std::cout << "x: " << xPos << ", " << "y: " << yPos << std::endl;

    if (local_playfield.at(yPos).at(xPos) == 1) {
        // hit
        local_playfield.at(yPos).at(xPos) = 2;
        return true;
    }
    local_playfield.at(yPos).at(xPos) = 0;
    return false;
}

void client::drawPlayfield() {
    for (int i=0; i<enemy_playfield.size(); i++) {
		std::cout << "      | ";
		for (int j=0; j<enemy_playfield.at(i).size(); j++) {
			if (enemy_playfield.at(i).at(j) == 0) {
                std::cout << "  ";
            }
            if (enemy_playfield.at(i).at(j) == 1) {
                std::cout << "O ";
            }
            if (enemy_playfield.at(i).at(j) == 2) {
                std::cout << "X ";
            }
		}
		std::cout << "|" << std::endl;
	}
	std::cout << "      -------------" << std::endl;
    std::cout << "      | 1|2|3|4|5 |" << std::endl;
    std::cout << "      -------------" << std::endl;
}


void client::run() {
    std::cout << "######################################" << std::endl;
    std::cout << "############# GAME STARTS ############" << std::endl;
    std::cout << "######################################" << std::endl;
    int ctr = 0;
    while (!hasWon) {
        // check if has won
        if (recv_msg() == "won") {
            std::cout << "######################################" << std::endl;
            std::cout << "########### ENEMY HAS WON ############" << std::endl;
            std::cout << "######################################" << std::endl;
            break;
        }
        // send current win status
        if (has_hit_ctr == 5) {
            std::cout << "######################################" << std::endl;
            std::cout << "############### YOU WIN ##############" << std::endl;
            std::cout << "######################################" << std::endl;
            send_msg("won");
            hasWon = true;
            break;
        }
        send_msg("notWon");
        
        // check if is full
        if (recv_msg() == "full") {
            std::cout << "######################################" << std::endl;
            std::cout << "##########     GAME END     ##########" << std::endl;
            std::cout << "##########  NO ONE HAS WON  ##########" << std::endl;
            std::cout << "######################################" << std::endl;
            break;
        }
        if (ctr == 24) {
            std::cout << "######################################" << std::endl;
            std::cout << "##########     GAME END     ##########" << std::endl;
            std::cout << "##########  NO ONE HAS WON  ##########" << std::endl;
            std::cout << "######################################" << std::endl;
            send_msg("full");
            break;
        }
        send_msg("notFull");
              
        
        // get enemy coordinates
        // check if hit
        std::cout << "Waiting for enemy player..." << std::endl;
        bool isHit = checkPlayfield(recv_msg());
        
        // send true/false
        send_msg(boolToString(isHit));
        
        std::cout << "Type in the coordinates where to shoot:" << std::endl;
        std::string xPos;
        std::string yPos;
        std::cout << "xPos > ";
        std::cin >> xPos;
        std::cout << "yPos > ";
        std::cin >> yPos;
        send_msg(xPos + "/" + yPos);
        
        // receive if hit
        bool isHitEnemy = stringToBool(recv_msg());
        if (isHitEnemy) {
            enemy_playfield.at(atoi(yPos.c_str())-1).at(atoi(xPos.c_str())-1) = 2;
            ++has_hit_ctr;
        } else {
            enemy_playfield.at(atoi(yPos.c_str())-1).at(atoi(xPos.c_str())-1) = 1;
        }
        drawPlayfield();
        ++ctr;
    }
    close(client_sock);
    
}

int main(int argc, const char *pArgv[]) {
    // TODO process
    if (argc < 2) {
        std::cerr << "usage: client [absolute path to socket file]" << std::endl;
        return -1;
    }
    client c = client(pArgv[1]);
    c.run();

}