#pragma once

namespace Gorilla { namespace Graphics
{
    enum class ErrorCode
    {
        NONE = 0,
        FAIL,
    };

    template <typename T>
    struct Result
    {
        Result() = delete;

        Result(ErrorCode code)
            : Code(code)
        {

        }

        Result(T& value)
            : Code(ErrorCode::NONE)
            , Value(value)
        {

        }

        ErrorCode Code;
        T Value;

        operator bool() const
        {
            return Code == ErrorCode::NONE;
        }
    };
}}
