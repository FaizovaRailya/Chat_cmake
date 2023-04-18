#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "chat_server.h"
//#include "chat_server.cpp"
#include "chat_client.h"
//#include "chat_client.cpp"
#if defined(__linux__)
#include <unistd.h>
#include <sys/utsname.h> // Для uname()
#endif

int main(){
#if defined(__linux__)
    struct utsname utsname; // объект для структуры типа utsname

    uname(&utsname); // передаем объект по ссылке

    // распечатаем на экране информацию об операционной системе
    std::cout << "OS name: " << utsname.sysname << std::endl;
    std::cout << "Host name: " << utsname.nodename << std::endl;
    std::cout << "OS release: " << utsname.release << std::endl;
    std::cout << "OS version: " << utsname.version << std::endl;
    std::cout << "Machine name: " << utsname.machine << std::endl;
#elif (defined(_WIN32) || defined(_WIN64))
    system("systeminfo | find \"Название ОС\"");
    system("systeminfo | find \"Версия ОС\"");
#endif

    try {
        char user_choise;
        std::cout << "Press 1 for server. Press 2 for client. Press 0 for exit." << std::endl;
        std::cin >> user_choise;

        switch (user_choise){
        case '1':{
            Chat_server chatsrv;
            chatsrv.runChat();
            break; }
        case '2': {
            Chat_client chat;
            chat.runclient();
            break; }
        case '0':
            std::cout << "Exit" << std::endl;
            break;
        default:
            std::cout << "Wrong input. Exit" << std::endl;
            break;
        }
    }
    catch (std::exception& e){
        std::cout << e.what() << std::endl;
    }
    return 0;
}
