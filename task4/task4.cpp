#include <cassert>
#include <iostream>

template<typename Derived>
class less_than_comparable {
public:
    friend bool operator>(const Derived& a, const Derived& b) noexcept {
        return b < a;
    }
    
    friend bool operator<=(const Derived& a, const Derived& b) noexcept {
        return !(b < a);
    }
    
    friend bool operator>=(const Derived& a, const Derived& b) noexcept {
        return !(a < b);
    }
    
    friend bool operator==(const Derived& a, const Derived& b) noexcept {
        return !(a < b) && !(b < a);
    }
    
    friend bool operator!=(const Derived& a, const Derived& b) noexcept {
        return !(a == b);
    }
};

template<typename T>
class counter {
private:
    inline static size_t count_ = 0;

protected:
    counter() noexcept {
        ++count_;
    }
    
    counter(const counter&) noexcept {
        ++count_;
    }
    
    counter(counter&&) noexcept {
        ++count_;
    }
    
    ~counter() noexcept {
        --count_;
    }

public:
    static size_t count() noexcept {
        return count_;
    }
};

class Number : public less_than_comparable<Number>, public counter<Number> {
public:
    Number(int value) : m_value{value} {}

    int value() const {
        return m_value;
    }

    bool operator<(const Number& other) const {
        return m_value < other.m_value;
    }

private:
    int m_value;
};

int main() {
    Number one{1};
    Number two{2};
    Number three{3};
    Number four{4};
    
    assert(one >= one);
    assert(three <= four);
    assert(two == two);
    assert(three > two);
    assert(one < two);
    
    std::cout << "Count: " << counter<Number>::count() << std::endl; 
    
    return 0;
}
