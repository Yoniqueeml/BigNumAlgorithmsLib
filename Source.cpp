#include "Test.cpp"
unsigned long long powMod(unsigned long long a, unsigned long long e, unsigned long long m) {
    unsigned long long r = 1;
    while (e > 0) {
        if (e & 1)
            r = (r * a) % m;
        e >>= 1;
        a = (unsigned long long)(a * a) % m;
    }
    std::cout << "A=" << a << ", E=" << e << ", R=" << r << std::endl;
    return r;
}

BigInt m_reduce(BigInt ab, BigInt Pinv, BigInt r, BigInt P) {
    BigInt m = ab * Pinv % r;
    return ((ab + m * P) / r).first;
}

BigInt m_transform(BigInt a, BigInt r2, BigInt Pinv, BigInt r, BigInt P) {
    return m_reduce((a * r2), Pinv, r, P);
}
int main() {
    startTests();

    BigInt P("533"); // modulus
    BigInt r("4294967296"); // 2^32
    BigInt r2;
    BigInt two("2");
    BigInt degree("64");

    //BigInt Pinv("1071727299");
    r2 = r2.powMod(two, degree, P);

    unsigned long long p = 0, a1 = -533, e1 = -1, m = 4294967296;
    p = powMod(a1, e1, m);

    BigInt Pinv(std::to_string(p));

    BigInt a("36363");
    BigInt b("139393");

    BigInt a_prim = m_transform(a, r2, Pinv, r, P); // mult a by 2 ^ 32
    BigInt b_prim = m_transform(b, r2, Pinv, r, P); // mult b by 2 ^ 32
    BigInt prod_prim = m_reduce(a_prim * b_prim, Pinv, r, P); // divide a' * b' by 2 ^ 32
    BigInt prod = m_reduce(prod_prim, Pinv, r, P); // divide prod' by 2^32
    //std::cout << a << " * " << b << " = " << prod << std::endl;// prints 123456789 * 35 % 1000000007 = 320987587
    a.print();
    b.print();
    prod.print();
    return 0;
}