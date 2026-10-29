
#pragma once
#include <exception>
#include <string>

class library_exception : public std::exception {
private:
    std::string msg;

public:
    explicit library_exception(const std::string& message) : msg(message) {}
    virtual ~library_exception() noexcept {}

    const char* what() const noexcept override {
        return msg.c_str();
    }
};
