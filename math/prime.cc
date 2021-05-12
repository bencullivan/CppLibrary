/**
 * Author: chilli, Ramchandra Apte, Noam527, Simon Lindholm
 * Date: 2019-04-24
 * License: CC0
 * Source: https://github.com/RamchandraApte/OmniTemplate/blob/master/modulo.hpp
 * Description: Calculate $a\cdot b\bmod c$ (or $a^b \bmod c$) for $0 \le a, b \le c \le 7.2\cdot 10^{18}$.
 * Time: O(1) for \texttt{modmul}, O(\log b) for \texttt{modpow}
 * Status: stress-tested, proven correct
 * Details:
 * This runs ~2x faster than the naive (__int128_t)a * b % M.
 * A proof of correctness is in doc/modmul-proof.tex. An earlier version of the proof,
 * from when the code used a * b / (long double)M, is in doc/modmul-proof.md.
 * The proof assumes that long doubles are implemented as x87 80-bit floats; if they
 * are 64-bit, as on e.g. MSVC, the implementation is only valid for
 * $0 \le a, b \le c < 2^{52} \approx 4.5 \cdot 10^{15}$.
 */
ull modmul(ull a, ull b, ull M) {
	//ll ret = a * b - M * ull(1.L / M * a * b);
	//return ret + M * (ret < 0) - M * (ret >= (ll)M);
	return (__uint128_t)a * b % M;
}
ull modpow(ull b, ull e, ull mod) {
	ull ans = 1;
	for (; e; b = modmul(b, b, mod), e /= 2)
		if (e & 1) ans = modmul(ans, b, mod);
	return ans;
}

/**
 * Author: chilli, c1729, Simon Lindholm
 * Date: 2019-03-28
 * License: CC0
 * Source: Wikipedia, https://miller-rabin.appspot.com/
 * Description: Deterministic Miller-Rabin primality test.
 * Guaranteed to work for numbers up to $7 \cdot 10^{18}$; for larger numbers, use Python and extend A randomly.
 * Time: 7 times the complexity of $a^b \mod c$.
 * Status: Stress-tested
 */
bool miller_rabin(ull n) {
	if (n < 2 || n % 6 % 4 != 1) return (n | 1) == 3;
	ull A[] = {2, 325, 9375, 28178, 450775, 9780504, 1795265022},
		s = __builtin_ctzll(n-1), d = n >> s;
	for (ull a : A) {   // ^ count trailing zeroes
		ull p = modpow(a%n, d, n), i = s;
		while (p != 1 && p != n - 1 && a % n && i--)
			p = modmul(p, p, n);
		if (p != n-1 && i != s) return 0;
	}
	return 1;
}

/**
 * Author: chilli, SJTU, pajenegod
 * Date: 2020-03-04
 * License: CC0
 * Source: own
 * Description: Pollard-rho randomized factorization algorithm. Returns prime
 * factors of a number, in arbitrary order (e.g. 2299 -> \{11, 19, 11\}).
 * Time: $O(n^{1/4})$, less for numbers with small factors.
 * Status: stress-tested
 *
 * Details: This implementation uses the improvement described here
 * (https://en.wikipedia.org/wiki/Pollard%27s_rho_algorithm#Variants), where
 * one can accumulate gcd calls by some factor (40 chosen here through
 * exhaustive testing). This improves performance by approximately 6-10x
 * depending on the inputs and speed of gcd. Benchmark found here:
 * (https://ideone.com/nGGD9T)
 *
 * GCD can be improved by a factor of 1.75x using Binary GCD
 * (https://lemire.me/blog/2013/12/26/fastest-way-to-compute-the-greatest-common-divisor/).
 * However, with the gcd accumulation the bottleneck moves from the gcd calls
 * to the modmul. As GCD only constitutes ~12% of runtime, speeding it up
 * doesn't matter so much.
 *
 * This code can probably be sped up by using a faster mod mul - potentially
 * montgomery reduction on 128 bit integers.
 * Alternatively, one can use a quadratic sieve for an asymptotic improvement,
 * which starts being faster in practice around 1e13.
 *
 * Brent's cycle finding algorithm was tested, but doesn't reduce modmul calls
 * significantly.
 *
 * Subtle implementation notes:
 * - we operate on residues in [1, n]; modmul can be proven to work for those
 * - prd starts off as 2 to handle the case n = 4; it's harmless for other n
 *   since we're guaranteed that n > 2. (Pollard rho has problems with prime
 *   powers in general, but all larger ones happen to work.)
 * - t starts off as 30 to make the first gcd check come earlier, as an
 *   optimization for small numbers.
 */
ull pollard(ull n) {
	auto f = [n](ull x) { return modmul(x, x, n) + 1; };
	ull x = 0, y = 0, t = 30, prd = 2, i = 1, q;
	while (t++ % 40 || __gcd(prd, n) == 1) {
		if (x == y) x = ++i, y = f(x);
		if ((q = modmul(prd, max(x,y) - min(x,y), n))) prd = q;
		x = f(x), y = f(f(y));
	}
	return __gcd(prd, n);
}
vector<ull> factor(ull n) {
	if (n == 1) return {};
	if (miller_rabin(n)) return {n};
	ull x = pollard(n);
	auto l = factor(x), r = factor(n / x);
	l.insert(l.end(), r.begin(), r.end());
	return l;
}

vector<ll> primes;
vector<ll> spf;
vector<bool> is_prime;

/*
	The sieve of Eratosthenes.
	Generates every prime number less than or equal to N.
	Almost twice as fast as spf_sieve.
	Time: O(N*log(log(N)))
	Source: https://cp-algorithms.com/algebra/sieve-of-eratosthenes.html
*/
void sieve(ll N) {
	primes.clear();
	is_prime = vector<bool>(N + 1, true);
	is_prime[0] = is_prime[1] = false;
	for (ll i = 2; i <= N; i++) {
		if (is_prime[i]) {
			primes.push_back(i);
			for (ll j = i * i; j <= N; j += i) {
				is_prime[j] = false;
			}
		}
	}
}

/*
	Generates all prime numbers in the range [L, R] (inclusive)
	Time: O((R-L+1)*log(log(R)) + sqrt(R)*log(log(sqrt(R))))
	Source: https://cp-algorithms.com/algebra/sieve-of-eratosthenes.html
*/
void range(ll L, ll R) {
	ll root = sqrt(R);
	sieve(root);
	is_prime = vector<bool>(R - L + 1, true);
	for (ll i:primes) {
		for (ll j = max(i * i, (L + i - 1) / i * i); j <= R; j += i) {
			is_prime[j - L] = false;
		}
	}
	if (L == 1) {
		is_prime[0] = false;
	}
	primes.clear();
	for (ll i = 0; i < R - L + 1; i++) {
		if (is_prime[i]) {
			primes.push_back(i + L);
		}
	}
}

/*
	Generates every prime number less than or equal to N.
	Generates the smallest prime factor of every number from 2 to N.
	Time: O(N)
	Source: https://cp-algorithms.com/algebra/prime-sieve-linear.html
*/
void spf_sieve(ll N) {
	primes.clear();
	spf = vector<ll>(N + 1);
	for (ll i = 2; i <= N; i++) {
		if (spf[i] == 0) {
			spf[i] = i;
			primes.push_back(i);
		}
		for (ll j = 0; j < (ll) primes.size() && primes[j] <= spf[i] && i * primes[j] <= N; j++) {
			spf[i * primes[j]] = primes[j];
		}
	}
}

/*
	Finds the prime factorization of x using precomputed spfs.
	Must be used with spf_sieve.
	Time: O(log(x))
*/
vector<pair<ll, ll>> pf_precomp(ll x) {
	ll cur = spf[x];
	int ct = 0;
	vector<pair<ll, ll>> prime_factors;
	while (x > 1) {
		if (cur != spf[x]) {
			prime_factors.emplace_back(cur, ct);
			cur = spf[x];
			ct = 0;
		}
		ct++;
		x /= spf[x];
	}
	if (ct) {
		prime_factors.emplace_back(cur, ct);
	}
	return prime_factors;
}