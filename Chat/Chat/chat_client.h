#pragma once
#include "socket.h"

class Chat_client{
private:
    Socket_client sock;

public:
    Chat_client() = default;
    virtual ~Chat_client() = default;
    Chat_client(Chat_client& _arr) = delete;
    Chat_client& operator=(Chat_client& _list) = delete;

    void runclient();
};
