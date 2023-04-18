#pragma once
#include <string>
#include <fstream>

class User
{
private:
    std::string _login;
    unsigned int* _password;

public:
    User() = default;
    ~User() = default;
    User(std::string& login, unsigned int* password)
        : _login(login), _password(password) {}

    friend std::fstream& operator >>(std::fstream& is, User& _usr);
    friend std::ostream& operator <<(std::ostream& os, const User& _usr);

    const std::string getLogin() const;
    const unsigned int* getPassword() const;
    const bool Auth(std::string password) const;
};


