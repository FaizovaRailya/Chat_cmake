#pragma once
#include <string>
#include <ctime>
#include <fstream>
#include <iostream>

class Message{
private:
    std::string _from;
    std::string _to;
    std::string _text;
    std::time_t _timestamp;

public:
    Message() = default;
    ~Message() = default;
    Message(std::string& from, std::string& to, std::string& text, std::time_t& timestamp) 
        : _from(from), _to(to), _text(text), _timestamp(timestamp) {}
    
    friend std::fstream& operator >>(std::fstream& is, Message& _msg);
    friend std::ostream& operator <<(std::ostream& os, const Message& _msg);

    const std::string getFrom() const;
    const std::string getTo() const;
    const std::string getText() const;
    const std::time_t getTime() const;
};

