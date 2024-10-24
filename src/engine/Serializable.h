#pragma once

template<typename T>
class Serializable {
public:
    virtual T serialize() const = 0;
    virtual void deserialize(const T&) = 0;
};
