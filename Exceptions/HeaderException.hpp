//
// Created by danonofficial on 14.10.17.
//

#ifndef WAV_HEADEREXCEPTION_HPP
#define WAV_HEADEREXCEPTION_HPP

#include <exception>
#include <string>

namespace error {
    class HeaderException : public std::exception {
    public:

        explicit HeaderException(std::string msg) noexcept : msg(std::move(msg)) {}

        ~HeaderException()  noexcept override = default;

        const char *what() const noexcept override {
            return msg.c_str();
        }

    private:
        std::string msg;
    };
}
#endif //WAV_HEADEREXCEPTION_HPP
