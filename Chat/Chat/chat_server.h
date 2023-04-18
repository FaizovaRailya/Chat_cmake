#pragma once
#include "User.h"
#include "message.h"
#include <vector>
#include <map>
#include <fstream>
#include <filesystem>
#include "socket.h"

class Chat_server{
private:
    std::vector<Message> _messages;
    std::vector<User> _users;
    std::string currentUser;
    Socket_server sock;
    
public:
    std::fstream user_file = std::fstream("users.txt", std::ios::in | std::ios::out);
    std::fstream msg_file = std::fstream("messages.txt", std::ios::in | std::ios::out);
    Chat_server();
    virtual ~Chat_server() = default;
    Chat_server(Chat_server& _arr) = delete;
    Chat_server& operator=(Chat_server& _list) = delete;

    void runChat();
    void userMenu();
    void registerUser();
    void loginOperation();
    void logoutOperation();
    int showUsersByLogin();
    void setCurrentUser(const std::string& user);
    void createMessage(bool toAll = false);
    void showMessages(bool toAll = false);
    void showAllMessagesWith();
    void sentMessages();
    std::string get_login() const;
    const bool findUser(const std::string& login);
    const bool check_perms(const std::filesystem::perms& p);
};
