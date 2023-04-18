#include "user.h"
#include <iostream>

std::fstream& operator >>(std::fstream& is, User& _usr)
{
    std::string _login;
    unsigned int* _pw = new unsigned int[5];
    is >> _usr._login;
    for (int hash_block = 0; hash_block < 5; hash_block++)
    {
        is >> _pw[hash_block];
    }
    _usr._password = _pw;
    return is;
}

std::ostream& operator <<(std::ostream& os, const User& _usr)
{
    os << _usr.getLogin();
    os << ' ';
    const unsigned int* _pw = _usr.getPassword();
    for (int hash_block = 0; hash_block < 4; hash_block++)
        os << _pw[hash_block] << ' ';
    os << _pw[4];
    return os;
}

const std::string User::getLogin() const
{
    return _login;
}

const unsigned int* User::getPassword() const
{
    return _password;
}

const bool User::Auth(std::string password) const
{
    return true;
}
