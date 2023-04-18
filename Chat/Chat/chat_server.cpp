#include "chat_server.h"
#include "sha1.h"
#include <string>
#include <chrono>
#include <ctime>
#include <exception>
#include <filesystem>

#if defined(__linux__)
#include <sys/socket.h>
#elif (defined(_WIN32)||defined(_WIN64))
#include <winsock2.h>
#endif

namespace fs = std::filesystem;

using std::chrono::system_clock;

class bad_login : public std::exception{
public:
    const char* what() const noexcept override{
        return "Too many failed login attempts!";
    }
};

class bad_password : public std::exception{
public:
    const char* what() const noexcept override{
        return "too many bad password attempts!";
    }
};

const bool Chat_server::check_perms(const fs::perms& p)
{
    if (p == (fs::perms::owner_read | fs::perms::owner_write)){
        return true;
    }
    return false;
}

Chat_server::Chat_server(){
    if (!msg_file){
        msg_file = std::fstream("messages.txt", std::ios::in | std::ios::out | std::ios::trunc);
    }
#if defined(__linux__)
    if (!msg_file){
        std::cout << "Cannot create or open file 'messages.txt'. Trying create it and another chat files at user's home dir" << std::endl;
        std::string homepath = getenv("HOME");
        fs::current_path(getenv("HOME"));
        msg_file = std::fstream("messages.txt", std::ios::in | std::ios::out | std::ios::trunc);
    }
#endif

    if (msg_file){
#if defined(__linux__)
            fs::permissions("messages.txt", fs::perms::owner_write | fs::perms::owner_read, fs::perm_options::replace);
            if (!check_perms(fs::status("messages.txt").permissions())){
                std::cout << "Permissions of file 'messages.txt' are not applied! Maybe you are using Windows?" << std::endl;
            }
#endif
        msg_file.seekg(0, std::ios_base::end);
        int end_line = msg_file.tellg();
        msg_file.seekg(0, std::ios_base::beg);
        int loaded_message_num = 0;
        while (msg_file.tellg() < end_line && msg_file.tellg() >= 0){
            Message msg;
            msg_file >> msg;
            loaded_message_num++;
            _messages.emplace_back(msg);
        }
        std::cout << "Loaded " << loaded_message_num << " message record(s)" << std::endl;
    }

    else{
        std::cout << "Could not create or open file messages.txt!" << std::endl;
    }

    if (!user_file){
        user_file = std::fstream("users.txt", std::ios::in | std::ios::out | std::ios::trunc);
    }
#if defined(__linux__)
    if (!user_file){
        std::cout << "Cannot create or open file 'users.txt'. Trying create it at user's home dir" << std::endl;
	std::string homepath = getenv("HOME");
	fs::current_path(getenv("HOME"));
        msg_file = std::fstream("users.txt", std::ios::in | std::ios::out | std::ios::trunc);
    }
#endif
    if (user_file) {
#if defined(__linux__)
            fs::permissions("users.txt", fs::perms::owner_write | fs::perms::owner_read, fs::perm_options::replace);
            if (!check_perms(fs::status("users.txt").permissions())){
                std::cout << "Permissions of file 'users.txt' are not applied! Maybe you are using Windows?" << std::endl;
            }
#endif
        std::cout << fs::current_path() << std::endl;
        user_file.seekg(0, std::ios_base::end);
        int end_line = user_file.tellg();
        user_file.seekg(0, std::ios_base::beg);
        int loaded_user_num = 0;
        while (user_file.tellg() < end_line && user_file.tellg() >= 0){
            User usr;
            user_file >> usr;
            loaded_user_num++;
            _users.emplace_back(usr);
        }
        std::cout << "Loaded " << loaded_user_num << " user record(s)" << std::endl;
    }
    else std::cout << "Could not open file users.txt!" << std::endl;
}

void Chat_server::setCurrentUser(const std::string& user){
    currentUser = user;
}

void Chat_server::loginOperation(){
    std::string login, password;
    for (size_t try_num = 0; try_num < 6; ++try_num){
        sock.send_data((char*)"Sign-in. Enter login:");
        sock.send_data((char*)"end_receive");
        login = sock.receive_data();
        if (!login.empty()) {
            break;
        }
        if (login.empty() && try_num < 5) {
            char buffer[MESSAGE_LENGTH];
            sprintf(buffer, "Empty login! Try again. (%d/5)", (int)try_num + 1);
            sock.send_data(buffer);
        }
        else throw bad_login();
    }
    User* temp_user = nullptr;
    for (auto& user : _users){
        if (user.getLogin() == login){
            temp_user = &user;
            break;
        }
    }

    if (temp_user) {
        for (size_t try_num = 0; try_num < 6; ++try_num){
            sock.send_data((char*)"Enter password:");
            sock.send_data((char*)"end_receive");
            password = sock.receive_data();
            if (!memcmp(temp_user->getPassword(), sha1((char*)password.data(), password.length()), SHA1HASHLENGTHBYTES)) {
                break;
            }
            if (try_num < 5) {
                char buffer[MESSAGE_LENGTH];
                sprintf(buffer, "Incorrect Password! Try again. (%d/5)", (int)try_num + 1);
                sock.send_data(buffer);
            }
            else throw bad_password();
        }
        sock.send_data((char*)"         Welcome!");
        this->setCurrentUser(temp_user->getLogin());
        
        this->userMenu();
        std::cout << "end user menu" << std::endl;
    }
    else{
        sock.send_data((char*)"No such user!");
    }
}

void Chat_server::logoutOperation(){
    this->setCurrentUser("");
}

void Chat_server::registerUser(){
    std::string login, password;
    sock.send_data((char*)"Register new user. Enter login:");
    sock.send_data((char*)"end_receive");
    login = sock.receive_data();
    std::cout << login << std::endl;
    
    bool error = true;
    if (login == "")
        sock.send_data((char*)"You didn't enter the data!");
    else if
        (login == "all") sock.send_data((char*)"The user 'all' is forbidden to use");
    else{
        error = false;
        if (findUser(login)){
            sock.send_data((char*)"A user with this name is already registered!");
            error = true;
        }
    }
    if (!error){
        sock.send_data((char*)"Enter password:");
        sock.send_data((char*)"end_receive");
        password = sock.receive_data();
        User usr(login, sha1((char*)(password.data()), password.length()));
        _users.emplace_back(usr);
        user_file.seekp(0, std::ios_base::end);
        user_file << usr << std::endl;
        sock.send_data((char*)"User added.");
    }
}

const bool Chat_server::findUser(const std::string& login){
    for (const auto& user : _users){
        if (user.getLogin() == login){
            return true;
        }
    }
    return false;
}

int Chat_server::showUsersByLogin(){
    sock.send_data((char*)"\nUsers list:");
    int num_users = 0;
    for (const auto& user : _users){
        if (user.getLogin() != currentUser){
            char buffer[MESSAGE_LENGTH];
            sprintf(buffer, ">>> %s", user.getLogin().data());
            sock.send_data(buffer);
            num_users++;
        }
    }
    sock.send_data((char*)"\n");
    if (!num_users) sock.send_data((char*)"No another users");
    return num_users;
}

void Chat_server::createMessage(bool toAll){
    std::string from, to, text;
    system_clock::time_point value_t = system_clock::now();
    time_t timestamp = system_clock::to_time_t(value_t);
    if (toAll) {
        to = "all";
    }
    else if (showUsersByLogin()){
        while (1){
            sock.send_data((char*)"Enter recipient login: ");
            sock.send_data((char*)"end_receive");
            to = sock.receive_data();
            if (to == currentUser){
                sock.send_data((char*)"You can't send a message to yourself");
            }
            else if (!findUser(to)){
                sock.send_data((char*)"No such user.\nEnter recipient login: ");
            }
            else break;
        }
    }
    if (!to.empty()){
        from = currentUser;
        sock.send_data((char*)"Write your message, press enter to send: ");
        sock.send_data((char*)"end_receive");
        text = sock.receive_data();
        Message msg(from, to, text, timestamp);
        _messages.emplace_back(msg);
        msg_file.seekp(0, std::ios_base::end);
        msg_file << msg << std::endl;
        sock.send_data((char*)"Message sent.");
        userMenu();
    }
}

void Chat_server::showMessages(bool toAll){
    sock.send_data((char*)"\nNew messages: ");
    size_t message_num = 0;
    for (auto& text : _messages)    {
        std::string From = text.getFrom();
        std::string Text = text.getText();
        char buffer[MESSAGE_LENGTH];
        char *time_str;
        time_t x = text.getTime();
        time_str = ctime(&x);
        if (toAll){
            if (text.getTo() == "all"){
                sprintf(buffer, "\n%sBroadcast from: %s\n%s\n", time_str, From.data(), Text.data());
                sock.send_data(buffer);
                message_num++;
            }
        }
        else{
            if (currentUser == text.getTo() && currentUser != text.getFrom()){
                sprintf(buffer, "\n%sFrom: %s\n%s\n", time_str, From.data(), Text.data());
                sock.send_data(buffer);
                message_num++;
            }
        }
    }

    if (!message_num){
        if (toAll) sock.send_data((char*)"No broadcast messages\n");
        else sock.send_data((char*)"No messages\n");
    }   
}

void Chat_server::showAllMessagesWith(){
    std::string with;
    size_t message_num = 0;
    if (showUsersByLogin()){
        sock.send_data((char*)"\nEnter addressee login:");
        sock.send_data((char*)"end_receive");
        with = sock.receive_data();
        if (currentUser == with){
            sock.send_data((char*)"You enter your own login");
        }
        else{
            for (auto& text : _messages){
                std::string From = text.getFrom();
                std::string To = text.getTo();
                std::string Text = text.getText();
                char *time_str;
                char buffer[MESSAGE_LENGTH];
                time_t x = text.getTime();
                time_str = ctime(&x);
                if (with == text.getTo() && currentUser == text.getFrom()){
                    sprintf(buffer, "\n<<< %sFrom: %s <<< To: %s \n%s\n<<<\n", time_str, From.data(), To.data(), Text.data());
                    sock.send_data(buffer);
                    message_num++;
                }
                else if (with == text.getFrom() && currentUser == text.getTo()){
                    sprintf(buffer, "\n>>> %sFrom: %s >>> To: %s \n%s\n>>>\n", time_str, From.data(), To.data(), Text.data());
                    sock.send_data(buffer);
                    message_num++;
                }
            }
        }
        if (!message_num) sock.send_data((char*)"\nNo messages with this user\n");
    }
}

void Chat_server::sentMessages(){
    sock.send_data((char*)"\nSent messages:");
    int num_messages = 0;
    for (auto& text : _messages){
        if (currentUser == text.getFrom()){
            std::string To = text.getTo();
            std::string Text = text.getText();
            char *time_str;
            char buffer[MESSAGE_LENGTH];
            time_t x = text.getTime();
            time_str = ctime(&x);
            sprintf(buffer, "\n%sTo: %s\n%s\n", time_str, To.data(), Text.data());
            sock.send_data(buffer);
            num_messages++;
        }
    }
    if (!num_messages) sock.send_data((char*)"No sent messages");
}

void Chat_server::userMenu(){
    while (!currentUser.empty()){
        char buffer[MESSAGE_LENGTH];
        sprintf(buffer, "\n************** User Menu: Choose an option: ***************\n1 - Read new messages | 2 - Read messages with... | 3 - Read broadcast messages\n4 - Send a message | 5 - Send a broadcast message | 6 - View sent messages\n0 - Logout / Return to start");
        sock.send_data(buffer);
        sock.send_data((char*)"end_receive");
        char* user_choice = sock.receive_data();
        switch (user_choice[0]){
        case '1':
            showMessages(false);
            break;
        case '2':
            showAllMessagesWith();
            break;
        case '3':
            showMessages(true);
            break;
        case '4':
            createMessage(false);
            break;
        case '5':
            createMessage(true);
            break;
        case '6':
            sentMessages();
            break;
        case '0':
            logoutOperation();
            break;
        default:
            logoutOperation();
            sock.send_data((char*)"Wrong input. Return to start");
            break;
        }
    }
}

void Chat_server::runChat(){
    bool chat_enable = true;
    while (chat_enable){
        time_t tt = system_clock::to_time_t(system_clock::now());
        char* today_str;
        today_str = ctime(&tt);
        char buffer[MESSAGE_LENGTH];
        sprintf(buffer, "Today is: %s**************Initial Menu: Choose an option:**************\n1 - new user sign-up | 2 - sign-in | 0 - exit\n", today_str);
        sock.send_data(buffer);
        sock.send_data((char*)"end_receive");
        char* user_choise = sock.receive_data();
        switch (user_choise[0]){
        case '1':
            registerUser();
            break;
        case '2':
            loginOperation();
            break;
        case '0':
            std::cout << "Exit" << std::endl;
            sock.send_data((char*)"end_chat");
            chat_enable = sock.listen_socket();
            break;
        default:
            std::cout << "Wrong input. Exit" << std::endl;
            sock.send_data((char*)"end_chat");
            chat_enable = sock.listen_socket();
            break;
        }
    }
    std::cout << "Chat session terminated! See you again!" << std::endl;
}

std::string Chat_server::get_login() const{
    return currentUser; 
}

