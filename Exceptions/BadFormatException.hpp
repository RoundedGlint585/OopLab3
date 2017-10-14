//
// Created by danonofficial on 14.10.17.
//

#ifndef WAV_BADFORMATEXCEPTION_HPP
#define WAV_BADFORMATEXCEPTION_HPP

#include <exception>
#include <string>

namespace error {
    class BadFormatException : public std::exception {
    public:

        explicit BadFormatException(std::string msg) noexcept : msg(std::move(msg)) {}

        ~BadFormatException()  noexcept override = default;

        const char *what() const noexcept override {
            return msg.c_str();
        }

    private:
        std::string msg;
    };
}
#endif //WAV_BADFORMATEXCEPTION_HPP
