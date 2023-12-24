﻿#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

class BigInt {
private:
    std::vector<int> digits;
    bool positive;

public:
    BigInt() : positive(true) {
        digits.push_back(0);
    }
    void print() const {
        if (!positive) std::cout << "-";
        for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
            std::cout << *it;
        }
    }

    BigInt(const std::string& numStr) {
        positive = (numStr[0] != '-');
        for (int i = (numStr[0] == '-' ? 1 : 0); i < numStr.size(); ++i) {
            digits.push_back(numStr[i] - '0');
        }
        std::reverse(digits.begin(), digits.end());
        removeLeadingZeros();
    }
    bool operator==(const BigInt& other) const {
        return digits == other.digits && positive == other.positive;
    }
    bool operator<=(const BigInt& other) const {
        return !absoluteIsGreaterThan(other);
    }
    bool operator>=(const BigInt& other) const {
        return !(*this <= other);
    }
    BigInt operator+(const BigInt& other) const {
        if (positive == other.positive) {
            BigInt result = addAbsolute(other);
            result.positive = positive;
            return result;
        }
        else {
            if (absoluteIsGreaterThan(other)) {
                BigInt result = subtractAbsolute(other);
                result.positive = positive;
                return result;
            }
            else {
                BigInt result = other.subtractAbsolute(*this);
                result.positive = other.positive;
                return result;
            }
        }
    }

    BigInt operator-(const BigInt& other) const {
        if (positive == other.positive) {
            if (absoluteIsGreaterThan(other)) {
                BigInt result = subtractAbsolute(other);
                result.positive = positive;
                return result;
            }
            else {
                BigInt result = other.subtractAbsolute(*this);
                result.positive = !positive;
                return result;
            }
        }
        else {
            BigInt result = addAbsolute(other);
            result.positive = positive;
            return result;
        }
    }

    std::pair<BigInt, BigInt> operator/(const BigInt& divisor) const {
        return divideWithRemainder(divisor);
    }

    BigInt operator%(const BigInt& divisor) const {
        return getRemainder(divisor);
    }

    BigInt operator<<(int shift) const {
        if (shift < 0) {
            throw std::runtime_error("Negative shift");
        }
        int multiplier = pow(2, shift);
        BigInt result;
        int carry = 0;
        for (size_t i = 0; i < digits.size(); ++i) {
            int sum = carry + digits[i] * multiplier;
            result.digits.push_back(sum % 10);
            if (sum >= 10) {
                carry = 1;
            }
            else {
                carry = 0;
            }
        }
        if (carry != 0) {
            result.digits.push_back(carry);
        }
        while (result.digits[0] == 0) {
            result.digits.erase(result.digits.begin());
        }
        return result;
    }

    BigInt operator>>(int shift) const {
        if (shift < 0) {
            throw std::runtime_error("Negative shift");
        }
        BigInt result;
        int divisor = pow(2, shift);
        int carry = 0;
        int quotient = 0;
        for (int i = digits.size() - 1; i >= 0; i--) {
            if (i != 0) {
                quotient = quotient * 10 + digits[i] + carry * 10 / divisor;
                if (quotient < divisor) {
                    continue;
                }
                result.digits.push_back(quotient / divisor);
                carry = quotient % divisor;
                quotient = 0;
            }
            else {
                quotient = quotient * 10 + (digits[i] + carry * 10) / divisor;
                result.digits.push_back(quotient);
            }
        }
        std::reverse(result.digits.begin(), result.digits.end());
        result.removeLeadingZeros();
        return result;
    }

private:
    std::pair<BigInt, BigInt> divideWithRemainder(const BigInt& divisor) const {
        if (divisor == BigInt("0")) {
            throw std::runtime_error("Division by zero");
        }

        BigInt quotient;
        BigInt remainder(*this);

        while (remainder.absoluteIsGreaterThanOrEqual(divisor)) {
            BigInt temp = divisor;
            BigInt multiple("1");
            while (temp <= remainder) {
               temp = temp + divisor;
               multiple = multiple + BigInt("1");
            }
            remainder = remainder - (temp - divisor);
            quotient = quotient + (multiple - BigInt("1"));
        }

        quotient.positive = positive == divisor.positive;
        remainder.positive = positive;

        return std::make_pair(quotient, remainder);
    }

    BigInt getRemainder(const BigInt& divisor) const {
        auto result = divideWithRemainder(divisor);
        return result.second;
    }

    bool absoluteIsGreaterThanOrEqual(const BigInt& other) const {
        return absoluteIsGreaterThan(other) || *this == other;
    }
    BigInt addAbsolute(const BigInt& other) const {
        BigInt result;
        result.digits.resize(std::max(digits.size(), other.digits.size()), 0);

        int carry = 0;
        for (size_t i = 0; i < result.digits.size(); ++i) {
            int sum = carry;
            if (i < digits.size()) sum += digits[i];
            if (i < other.digits.size()) sum += other.digits[i];

            result.digits[i] = sum % 10;
            carry = sum / 10;
        }

        if (carry > 0) {
            result.digits.push_back(carry);
        }

        return result;
    }

    BigInt subtractAbsolute(const BigInt& other) const {
        BigInt result;
        result.digits.resize(std::max(digits.size(), other.digits.size()), 0);

        int borrow = 0;
        for (size_t i = 0; i < result.digits.size(); ++i) {
            int diff = borrow;
            if (i < digits.size()) diff += digits[i];
            if (i < other.digits.size()) diff -= other.digits[i];

            if (diff < 0) {
                diff += 10;
                borrow = -1;
            }
            else {
                borrow = 0;
            }

            result.digits[i] = diff;
        }

        result.removeLeadingZeros();
        return result;
    }

    bool absoluteIsGreaterThan(const BigInt& other) const {
        if (digits.size() > other.digits.size()) return true;
        if (digits.size() < other.digits.size()) return false;
        for (int i = digits.size() - 1; i >= 0; --i) {
            if (digits[i] > other.digits[i]) return true;
            if (digits[i] < other.digits[i]) return false;
        }
        return false;
    }

    void removeLeadingZeros() {
        while (digits.size() > 1 && digits.back() == 0) {
            digits.pop_back();
        }
    }
};


int main() {
    BigInt num1("-15");
    BigInt num2("-15");
    

    BigInt sum = num1 + num2;
    BigInt diff = num1 - num2;

    std::cout << "sum: ";
    sum.print();
    std::cout << '\n';

    std::cout << "diff: ";
    diff.print();
    std::cout << '\n';
    BigInt num3("15");
    BigInt num4("4");

    std::pair<BigInt, BigInt> div =  num3 / num4;
    std::cout << "div quotient: ";
    div.first.print();
    std::cout << '\n';
    std::cout << "div: ";
    div.second.print();
    std::cout << '\n';
    BigInt num5 = num3 % num4;
    std::cout << "operator%: ";
    num5.print();
    std::cout << '\n';

    BigInt num6 = num4 << 2;
    std::cout << "operator << : ";
    num6.print();
    std::cout << '\n';
    
    BigInt num8("4444");
    BigInt num7 = num8 >> 2;
    std::cout << "operator >> : ";
    num7.print();
    std::cout << '\n';
    return 0;
}