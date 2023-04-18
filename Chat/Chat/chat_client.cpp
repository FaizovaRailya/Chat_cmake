#include "chat_client.h"
#include <string>

void Chat_client::runclient(){
    std::string message;
    while (1)    {
        while (1){
            message = sock.receive_data();
            if (message.length() > 7 && (message.substr(message.length() - 8, message.length()) == "end_chat")){
                std::cout << message.substr(0, message.length() - 8) << std::endl;
                break;
            }
            else if (message.length() > 10 && (message.substr(message.length() - 11, message.length()) == "end_receive")){
                std::cout << message.substr(0, message.length() - 11) << std::endl;
                break;
            }
            else{
                std::cout << message << std::endl;
            }
        }
        if (message.length() > 7 && (message.substr(message.length() - 8, message.length()) == "end_chat")){
            break;
        }
        while (getline(std::cin, message)){
            if (message != "") break;
        }
        sock.send_data(message.data());
    }
    std::cout << "end of cycle" << std::endl;
}