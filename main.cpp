#include <iostream>

[[nodiscard]] bool IsPrime(int n) {
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

[[nodiscard]] bool FindPQ(int N, int& p, int& q) {
    for (int i = 2; i < N / 2; ++i) {
        if (N % i == 0 && IsPrime(i) && IsPrime(N / i)) {
            p = i;
            q = N / i;
            return true;
        }
    }
    std::cout << "N may have everything but two prime divisors (excluding 1 and N)\n";
    return false;
}

int EEA(int a, int b, int& x, int& y) {
    if (a == 0) {
        x = 0;
        y = 1;
        return b;
    }
    int x1, y1;
    int d = EEA(b % a, a, x1, y1);
    x = y1 - (b / a) * x1;
    y = x1;
    return d;
}

[[nodiscard]] int ExpPower(int a, int p, int mod) {
    if (p == 0) {
        return 1;
    } else if (p == 1) {
        return a;
    }

    if (p % 2 != 0) {
        return (a * ExpPower((a * a) % mod, (p - 1) >> 1, mod)) % mod;
    } else {
        return ExpPower((a * a) % mod, p >> 1, mod) % mod;
    }
}

int main() {
    int e = 53;
    int N = 299;
    int enc = 171;

    std::cout << "Enter N: ";
    std::cin >> N;
    std::cout << "Enter e: ";
    std::cin >> e;
    std::cout << "Enter enc: ";
    std::cin >> enc;

    int p = 0;
    int q = 0;
    if (!FindPQ(N, p, q)) {
        return 1;
    }
    std::cout << "p = " << p << ", q = " << q << '\n';

    int phiN = (p - 1) * (q - 1);

    int d = 0;
    int k = 0;
    EEA(e, phiN, d, k);
    d = (phiN + (phiN + d % phiN)) % phiN;
    std::cout << "N = " << N << " d = " << d << '\n';

    std::cout << "dec = " << ExpPower(enc, d, N) << '\n';
    return 0;
}
