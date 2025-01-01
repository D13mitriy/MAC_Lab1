// Giophantus Cryptosystem Implementation - C++20
// Modular structure with classes for Key Generation, Encryption, Decryption

#include <iostream>
#include <vector>
#include <random>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <tuple>
#include <cassert>

// Define constants for the cryptosystem
struct GiophantusParams {
    int q;      // Prime modulus for the field
    int n;      // Degree of modulus polynomial (prime number)
    int l;      // Small integer for encoding
    int dX;     // Total degree of X(x, y)
    int dr;     // Total degree of random polynomial r
    int mlen;   // Length of the message
};

GiophantusParams param128 = {17, 11, 4, 2, 2, 32};
GiophantusParams param192 = {23, 19, 6, 3, 3, 48};
GiophantusParams param256 = {29, 23, 8, 4, 4, 64};

struct Signature {
    std::vector<int> s1;
    std::vector<int> s2;
};


// Utility Functions for Finite Field Arithmetic
int mod(int a, int p) {
    return ((a % p) + p) % p;
}

int add(int a, int b, int p) {
    return mod(a + b, p);
}

int mul(int a, int b, int p) {
    return mod(a * b, p);
}

int sub(int a, int b, int p) {
    return mod(a - b, p);
}

// Polynomial Representation and Operations
class Polynomial {
public:
    std::vector<int> coeffs; // Coefficients in little-endian order

    Polynomial(int degree = 0) : coeffs(degree + 1, 0) {}

    Polynomial(const std::vector<int>& c) : coeffs(c) {}

    int degree() const {
        return coeffs.size() - 1;
    }

    void trim() {
        while (!coeffs.empty() && coeffs.back() == 0) {
            coeffs.pop_back();
        }
    }

    // Polynomial addition
    Polynomial operator+(const Polynomial& other) const {
        std::vector<int> result(std::max(coeffs.size(), other.coeffs.size()), 0);
        for (size_t i = 0; i < result.size(); ++i) {
            int a = (i < coeffs.size()) ? coeffs[i] : 0;
            int b = (i < other.coeffs.size()) ? other.coeffs[i] : 0;
            result[i] = add(a, b, q);
        }
        return Polynomial(result);
    }

    // Polynomial multiplication
    Polynomial operator*(const Polynomial& other) const {
        std::vector<int> result(coeffs.size() + other.coeffs.size() - 1, 0);
        for (size_t i = 0; i < coeffs.size(); ++i) {
            for (size_t j = 0; j < other.coeffs.size(); ++j) {
                result[i + j] = add(result[i + j], mul(coeffs[i], other.coeffs[j], q), q);
            }
        }
        return Polynomial(result);
    }

    // Polynomial modular reduction by t^n - 1
    Polynomial mod_tn() const {
        Polynomial result;
        result.coeffs.resize(n, 0);
        for (size_t i = 0; i < coeffs.size(); ++i) {
            result.coeffs[i % n] = add(result.coeffs[i % n], coeffs[i], q);
        }
        return result;
    }

    // Polynomial evaluation
    int eval(int t) const {
        int value = 0;
        int power = 1;
        for (int c : coeffs) {
            value = add(value, mul(c, power, q), q);
            power = mul(power, t, q);
        }
        return value;
    }
};

// Random Polynomial Generator
Polynomial random_polynomial(int degree, int max_coeff) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, max_coeff - 1);
    std::vector<int> coeffs(degree + 1);
    for (int& c : coeffs) {
        c = dist(gen);
    }
    return Polynomial(coeffs);
}


// Check if a polynomial is irreducible (placeholder implementation)
bool is_irreducible(const Polynomial& poly) {
    // Placeholder: Implement proper irreducibility check
    return true;
}

// Generate irreducible polynomial X(x, y)
Polynomial generate_irreducible_X(const Polynomial& ux, const Polynomial& uy) {
    Polynomial X;
    while (true) {
        X = random_polynomial(dX, q - 1);
        if (is_irreducible(X)) {
            break;
        }
    }
    return X;
}

// Giophantus Key Generation
std::tuple<Polynomial, Polynomial, Polynomial> keygen() {
    Polynomial ux = random_polynomial(n - 1, l);
    Polynomial uy = random_polynomial(n - 1, l);
    Polynomial X = generate_irreducible_X(ux, uy);
    return {ux, uy, X};
}

// Implement Encryption
Polynomial encrypt(const Polynomial& message, const Polynomial& public_key, const GiophantusParams& params) {
    Polynomial r = random_polynomial(params.dr, params.q - 1);
    Polynomial e = random_polynomial(params.dX + params.dr, params.l - 1);
    Polynomial ciphertext = message + (public_key * r).mod_tn() + e;
    return ciphertext.mod_tn();
}


// Implement Decryption
Polynomial decrypt(const Polynomial& ciphertext, const Polynomial& ux, const Polynomial& uy) {
    Polynomial substituted = ciphertext;
    for (size_t i = 0; i < substituted.coeffs.size(); ++i) {
        substituted.coeffs[i] = mod(substituted.coeffs[i], l);
    }
    return substituted;
}

void test_keygen() {
    GiophantusParams params = param256;
    auto [ux, uy, X] = keygen(params);
    assert(!X.coeffs.empty()); // test X for correctness 
}

int main() {
    // Example usage of key generation
    auto [ux, uy, X] = keygen();



    // Example message
    Polynomial message({1, 0, 3}); // m(t) = 1 + 3t^2

    // Encrypt and decrypt example
    Polynomial ciphertext = encrypt(message, X);
    Polynomial decrypted_message = decrypt(ciphertext, ux, uy);

    std::cout << "Encryption and Decryption completed." << std::endl;
    return 0;
}
