#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

#include <bits/stdc++.h>
using namespace std;

#include "titan_cpplib/data_structures/dynamic_lazy_segment_tree.cpp"

const int mod = 998244353;

using IndexType = int;

struct S {
    long long value;
    int size;
};

using F = long long;

S op(S a, S b) {
    return {(a.value+b.value)%mod, a.size+b.size};
}

S e() {
    return {0, 0};
}

S mapping(F f, S x) {
    return {(x.value + f)%mod, x.size};
}

F composition(F f, F g) {
    return (f+g)%mod;
}

F id() {
    return 0;
}

S pow(S s, IndexType k) {
    return {(s.value*k)%mod, s.size*k};
}

void solve() {
    int n, q;
    cin >> n >> q;

    int acc = 0;
    S init = {0, 1};
    titan23::DynamicLazySegmentTree<IndexType, S, op, e, F, mapping, composition, id, pow> seg(n+1, init);

    for (int qidx = 0; qidx < q; ++qidx) {
        int com;
        cin >> com;
        if (com == 1) { // apply
            IndexType l, r;
            int x;
            cin >> l >> r >> x;
            l ^= acc;
            r ^= acc;
            x ^= acc;
            --l;
            seg.apply(l, r, x);
        } else {
            IndexType l, r;
            cin >> l >> r;
            l ^= acc;
            r ^= acc;
            --l;
            S res = seg.prod(l, r);
            acc = (acc + res.value) % mod;
            cout << res.value << "\n";
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    solve();
    return 0;
}
