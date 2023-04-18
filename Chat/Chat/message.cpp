#include "message.h"
#include <string>
#include <ctime>
#include <chrono>

std::fstream& operator >>(std::fstream& is, Message& _msg)
{
    std::string timestamp_string;
    
    getline(is, _msg._from, ';');
    getline(is, _msg._to, ';');
    getline(is, timestamp_string, ';');
    getline(is, _msg._text);
    std::chrono::system_clock::time_point timestamp(std::chrono::seconds(std::stoll(timestamp_string)));
    _msg._timestamp = std::chrono::system_clock::to_time_t(timestamp);
    return is;
}

std::ostream& operator <<(std::ostream& os, const Message& _msg)
{
    os << _msg.getFrom();
    os << ';';
    os << _msg.getTo();
    os << ';';
    os << _msg.getTime();
    os << ';';
    os << _msg.getText();
    return os;
}

const std::string Message::getFrom() const 
{ 
    return _from; 
}

const std::string Message::getTo() const 
{ 
    return _to; 
}

const std::string Message::getText() const 
{ 
    return _text; 
}

const std::time_t Message::getTime() const 
{ 
    return _timestamp; 
}