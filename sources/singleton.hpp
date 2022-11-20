#pragma once

template <typename T> class SingletonBase {
public:
    SingletonBase(const SingletonBase&) = delete;
    SingletonBase operator=(const SingletonBase&) = delete;
    SingletonBase(SingletonBase&&) = delete;
    SingletonBase& operator=(SingletonBase&&) = delete;

    static T& get_instance() {
        // Потокобезопасный magic-static
        static T unique_instance;
        return unique_instance;
    }

protected:
    SingletonBase() {}
};