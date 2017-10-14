//
// Created by danonofficial on 14.10.17.
//

#ifndef WAV_IOEXCEPTION_HPP
#define WAV_IOEXCEPTION_HPP

#include <exception>
#include <string>

namespace error {
    class IOException : public std::exception {
    public:

        explicit IOException(std::string msg) noexcept : msg(std::move(msg)) {}

        ~IOException()  noexcept override = default;

        const char *what() const noexcept override {
            return msg.c_str();
        }

    private:
        std::string msg;
    };
}
#endif //WAV_IOEXCEPTION_HPP
