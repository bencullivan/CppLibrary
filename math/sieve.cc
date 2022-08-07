vector<int> primes;
vector<int> spf;
// https://cp-algorithms.com/algebra/sieve-of-eratosthenes.html
void sieve(long long N) {
	primes.clear();
	spf.assign(N+1, -1);
	for (long long i = 2; i <= N; i++) {
		if (spf[i] == -1) {
			spf[i] = i;
			primes.push_back(i);
			for (long long j = i*i; j <= N; j += i) {
				if (spf[j] == -1) spf[j] = i;
			}
		}
	}
}
vector<pair<int, int>> get_pfs(int x) {
	int cur = spf[x];
	int ct = 0;
	vector<pair<int, int>> pfs;
	while (x > 1) {
		if (cur != spf[x]) {
			pfs.emplace_back(cur, ct);
			cur = spf[x];
			ct = 0;
		}
		ct++;
		x /= spf[x];
	}
	if (ct) pfs.emplace_back(cur, ct);
	return pfs;
}