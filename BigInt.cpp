#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>

class BigInt {
private:
    std::vector<int> digits;
    bool positive;
public:

    BigInt() : positive(true) {
    }

    void print() const {
        if (!positive) std::cout << "-";
        for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
            std::cout << *it;
        }
    }

    BigInt(const std::string& numStr) {
        positive = (numStr[0] != '-');
        digits.clear();
        for (int i = (numStr[0] == '-' ? 1 : 0); i < numStr.size(); ++i) {
            digits.push_back(numStr[i] - '0');
        }
        std::reverse(digits.begin(), digits.end());
        removeLeadingZeros();
    }
    BigInt operator&(const BigInt& other) const {
        BigInt result;
        BigInt a = *this;
        BigInt b = other;
        int maxlen = std::max(other.digits.size(), digits.size());
        while (a.digits.size() != maxlen) {
            a.digits.push_back(0);
        }
        while (b.digits.size() != maxlen) {
            b.digits.push_back(0);
        }
        for (int i = 0; i < maxlen; i++) {
            result.digits.push_back(a.digits[i] & b.digits[i]);
        }
        result.digits = this->digits;
        result.positive = this->positive && other.positive;

        return result;
    }
    bool operator==(const BigInt& other) const {
        return digits == other.digits && positive == other.positive;
    }
    bool operator!=(const BigInt& other) const {
        return !(*this == other);
    }
    bool operator<=(const BigInt& other) const {
        if (positive && !other.positive) return false;
        if (!positive && other.positive) return true;
        if (*this == other) return true;
        if (positive) return !absoluteIsGreaterThan(other);
        else return absoluteIsGreaterThan(other);
    }
    bool operator>=(const BigInt& other) const {
        bool t2 = *this <= other;
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
        BigInt x = *this;
        BigInt y = divisor;
        x.positive = true;
        y.positive = true;
        std::pair<BigInt,BigInt> result = x.divideWithRemainder(y);
        result.first.positive = this->positive == divisor.positive;
        result.second.positive = this->positive == divisor.positive;
        if (result.first == BigInt("-0") or result.first == BigInt("0")) {
            result.first.positive = true;
        }
        if (result.second == BigInt("-0") or result.second == BigInt("0")) {
            result.second.positive = true;
        }
        return result;
    }

    BigInt operator%(const BigInt& divisor) const {
        BigInt x = *this;
        BigInt y = divisor;
        x.positive = true;
        y.positive = true;
        BigInt result = x.divideWithRemainder(y).second;
        result.positive = this->positive == divisor.positive;
        return result;
    }

    BigInt operator<<(int shift) const {
        if (shift < 0) {
            throw std::runtime_error("Negative shift");
        }
        if (shift == 0) {
            return *this;
        }
        BigInt multiplier("2");
        for (int i = 0; i < shift-1; i++) {
            multiplier = multiplier * BigInt("2");
        }
        BigInt result;
        BigInt carry ("0");
        for(size_t i = 0; i < digits.size(); i++) {
            BigInt temp = BigInt(std::to_string(digits[i]));
            temp = temp * multiplier;
            BigInt sum = carry + BigInt(std::to_string(digits[i])) * multiplier;
            BigInt add = sum % BigInt("10");
            result.digits.push_back(add.digits[0]);
            if (sum >= BigInt("10")) {
                carry = (sum/BigInt("10")).first;
            }
            else {
                carry = BigInt("0");
            }
        }
        if (carry != BigInt("0")) {
            for (int i = 0; i < carry.digits.size(); i++) {
                result.digits.push_back(carry.digits[i]);
            }
        }
        result.positive = positive;
        return result;
    }
    
    BigInt operator>>(int shift) const {
        if (shift < 0) {
            throw std::runtime_error("Negative shift");
        }
        BigInt result;
        result.positive = positive;
        int divisor = pow(2, shift);
        int carry = 0;
        int quotient = 0;
        for (int i = digits.size() - 1; i >= 0; i--) {
            if (i != 0) {
                int temp1 = digits[i];
                int temp2 = carry * 10;
                quotient = quotient * 10 + digits[i] + carry * 10;
                if (quotient < divisor) {
                    result.digits.push_back(0);
                    continue;
                }
                result.digits.push_back(quotient / divisor);
                carry = quotient % divisor;
                quotient = 0;
            }
            else {
                quotient = (quotient * 10 + digits[i] + carry * 10) / divisor;
                result.digits.push_back(quotient);
            }
        }
        std::reverse(result.digits.begin(), result.digits.end());
        result.removeLeadingZeros();
        return result;
    }

    BigInt operator*(const BigInt& other) const {
        BigInt result;
        result.digits.resize(digits.size() + other.digits.size(), 0);

        for (size_t i = 0; i < digits.size(); ++i) {
            int carry = 0;
            for (size_t j = 0; j < other.digits.size() || carry; ++j) {
                long long cur = result.digits[i + j] + (digits[i]) * (j < other.digits.size() ? other.digits[j] : 0) + carry;
                result.digits[i + j] = cur % 10;
                carry = cur / 10;
            }
        }
        result.removeLeadingZeros();
        result.positive = positive == other.positive;
        return result;
    }

    BigInt& operator=(const BigInt& other) {
        if (this != &other) {
            digits.clear();
            digits = other.digits;
            positive = other.positive;
        }
        return *this;
    }

    bool operator<(const BigInt& other) const {
        return !(*this > other);
    }

    bool operator>(const BigInt& other) const {
        if (positive != other.positive) {
            return positive;
        }
        if (positive) {
            return absoluteIsGreaterThan(other);
        }
        else {
            return other.absoluteIsGreaterThan(*this);
        }
    }

    BigInt(const std::vector<int>& numVec) : positive(true) {
        digits = numVec;
        removeLeadingZeros();
    }

    BigInt binaryPower(const BigInt& exponent) const {
        if (exponent != BigInt("0") && exponent <= BigInt("0")) {
            throw std::runtime_error("Negative exponent not supported");
        }

        BigInt result("1");
        BigInt base(*this);
        BigInt exp(exponent);

        while (exp != BigInt("0") and exp >= BigInt("0")) {
            if (exp % BigInt("2") == BigInt("1")) {
                result = result * base;
            }
            base = base * base;
            exp = exp >> 1;
        }

        return result;
    }

    std::vector<int> powerExpansion(int x, int y) const {
        std::vector<int> powers;
        while (x > 0) {
            int power = 0;
            while (pow(y, power) <= x) {
                power++;
            }
            power--;
            powers.push_back(power);
            x -= pow(y, power);
        }
        return powers;
    }

    BigInt quaryPower(const int& power, const int& q) const {
        std::vector<int> ps = powerExpansion(power, q);
        std::vector<BigInt> bi;
        BigInt result("1");
        for (int i = 0; i < ps.size(); i++) {
            int p = pow(q, ps[i]);
            result = result * this->binaryPower(BigInt(std::to_string(p)));
        }
        return result;
    }
    bool operator>(const int& other) const {
        BigInt num(std::to_string(other));
        return *this > num;
    }

    // https://scienceland.info/algebra8/euclid-algorithm
    BigInt GCD(const BigInt& other) const {
        BigInt a(*this);
        a.positive = true;
        BigInt b(other);
        b.positive = true;
        while (b != BigInt("0")) {
            BigInt temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }
    std::pair<BigInt, std::pair<BigInt, BigInt>> extendedGCD(const BigInt& a, const BigInt& b) {
        BigInt x("1");
        BigInt y("1");
        return extGCD(a, b, x, y);
    }

    BigInt(std::vector<int>::const_iterator begin, std::vector<int>::const_iterator end) : positive(true) {
        digits.assign(begin, end);
    }
    BigInt increasing_Discharge(BigInt& num, size_t n) {
        for (size_t i = 0; i < n; ++i) {
            num.digits.insert(num.digits.begin(), 0);
        }
        return num;
    }
    BigInt karatsuba(BigInt& X, BigInt& Y) {

        if (X.digits[0] < 4 && Y.digits[0] < 4)
            return X * Y;

        long long size = fmax(X.digits.size(), Y.digits.size());
        if (size < 10)
            return X * Y;

        size = (size / 2) + (size % 2);
        BigInt Size(std::to_string(size));
        BigInt multiplier = increasing_Discharge(multiplier, size);
        BigInt b = (X / multiplier).first;
        BigInt a = X - (b * multiplier);
        BigInt d = (Y / multiplier).first;
        BigInt c = Y - (d * Size);
        BigInt temp1 = a + b;
        BigInt temp2 = c + d;
        BigInt u = karatsuba(a, c);
        BigInt z = karatsuba(temp1, temp2);
        BigInt v = karatsuba(b, d);
        return u + ((z - u - v) * multiplier) + increasing_Discharge(v, 2 * size);
    }

    BigInt montgomeryReduce(const BigInt& ab, const BigInt& P, const BigInt& Pinv, const BigInt& r, const BigInt& r2) const {
        BigInt m = ab * Pinv % r;
        return ((ab + m * P) / r).first;
    }

    BigInt montgomeryTransform(const BigInt& a, const BigInt& r2, const BigInt& Pinv, const BigInt& r) const {
        return montgomeryReduce(a * r2, *this, Pinv, r, r2);
    }

    BigInt montgomeryMultiply(const BigInt& a, const BigInt& b, const BigInt& r2, const BigInt& Pinv, const BigInt& r) const {
        BigInt aPrim = montgomeryTransform(a, r2, Pinv, r);
        BigInt bPrim = montgomeryTransform(b, r2, Pinv, r);
        BigInt prodPrim = montgomeryReduce(aPrim * bPrim, *this, Pinv, r, r2);
        return montgomeryReduce(prodPrim, *this, Pinv, r, r2);
    }

    BigInt powMod(BigInt a, BigInt e, BigInt m) {
        BigInt r("1");
        while (e > 0) {
            if ((e & BigInt("1")) == BigInt("1")) {
                r = (r * a) % m;
            }
            e = e >> 1;
            a = (a * a) % m;
        }
        return r;
    }
private:
    std::pair<BigInt, std::pair<BigInt, BigInt>> extGCD(const BigInt& a, const BigInt& b, BigInt& x, BigInt& y) {
        if (a == BigInt("0")) {
            x = BigInt("0");
            y = BigInt("1");
            return std::make_pair(b, std::make_pair(x, y));
        }
        BigInt x1;
        BigInt y1;
        BigInt d = extGCD((b / a).second, a, x1, y1).first;
        x = y1 - (b / a).first * x1;
        y = x1;
        return std::make_pair(d, std::make_pair(x, y));
    }
    std::pair<BigInt, BigInt> divideWithRemainder(const BigInt& divisor) const {
        if (divisor == BigInt("0")) {
            throw std::runtime_error("Division by zero");
        }

        BigInt quotient("0");
        BigInt remainder(*this);

        while (remainder >= divisor) {
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

