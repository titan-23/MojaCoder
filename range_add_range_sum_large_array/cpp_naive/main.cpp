#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

#include <bits/stdc++.h>
using namespace std;

const long long mod = 998244353;

void solve() {
    int n, q;
    cin >> n >> q;
    vector<long long> A(n, 0);
    long long s = 0;
    for (int qidx = 0; qidx < q; ++qidx) {
        int com;
        cin >> com;
        if (com == 1) { // apply
            long long l, r;
            long long x;
            cin >> l >> r >> x;
            l ^= s;
            r ^= s;
            x ^= s;
            --l;
            for (int i = l; i < r; ++i) {
                A[i] = (A[i] + x) % mod;
            }
        } else {
            long long l, r;
            cin >> l >> r;
            l ^= s;
            r ^= s;
            --l;
            long long ans = 0;
            for (int i = l; i < r; ++i) {
                ans += A[i];
                ans %= mod;
            }
            s = (s + ans) % mod;
            cout << ans << "\n";
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    solve();
    return 0;
}
