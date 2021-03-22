/*
    The sieve of Eratosthenes.
    Generates every prime number less than or equal to N.
    Time: O(N)
    Source: https://www.geeksforgeeks.org/sieve-of-eratosthenes/
*/
vector<int> primes;
vector<int> spf; // smallest prime factor
vector<bool> is_prime;
void sieve(int N) {
    is_prime.resize(N + 1, true);
    spf.resize(N + 1);
    is_prime[0] = is_prime[1] = false;
    for (int i = 2; i <= N; i++) {
        if (is_prime[i]) {
            spf[i] = i;
            primes.push_back(i);
        }
        for (int j = 0; j < (int) primes.size() && primes[j] <= spf[i] && (long long) i * primes[j] <= N; j++) {
            is_prime[i * primes[j]] = false;
            spf[i * primes[j]] = primes[j];
        }
    }
}