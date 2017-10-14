//
// Created by danonofficial on 14.10.17.
//

#ifndef WAV_UNSUPPORTEDFORMATEXCEPTION_HPP
#define WAV_UNSUPPORTEDFORMATEXCEPTION_HPP

#include <exception>
#include <string>

namespace error {
    class UnsupportedFormatException : public std::exception {
    public:

        explicit UnsupportedFormatException(std::string msg) noexcept : msg(std::move(msg)) {}

        ~UnsupportedFormatException()  noexcept override = default;

        const char *what() const noexcept override {
            return msg.c_str();
        }

    private:
        std::string msg;
    };
}
#endif //WAV_UNSUPPORTEDFORMATEXCEPTION_HPP
