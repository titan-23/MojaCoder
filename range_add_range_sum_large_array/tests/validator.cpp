#include <iostream>
#include <string>
#include <cassert>
#include "../../common/testlib.h"
#include "constraints.hpp"
using namespace std;

const int mod = 998244353;
const long long MAX_V = (1ll << 31)-1;

#include "titan_cpplib/data_structures/dynamic_lazy_segment_tree.cpp"

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
    return {(x.value + f*x.size%mod)%mod, x.size};
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

int main() {
    registerValidation();

    int n = inf.readInt(MIN_N, MAX_N);
    inf.readSpace();
    int q = inf.readInt(MIN_Q, MAX_Q);
    inf.readEoln();

    long long acc = 0;
    S init = {0, 1};
    titan23::DynamicLazySegmentTree<IndexType, S, op, e, F, mapping, composition, id, pow> seg(n+1, init);

    for (int qidx = 0; qidx < q; ++qidx) {
        int com = inf.readInt(1, 2);
        inf.readSpace();
        if (com == 1) {
            long long l = inf.readLong(0, MAX_V);
            inf.readSpace();
            long long r = inf.readLong(0, MAX_V);
            inf.readSpace();
            long long x = inf.readLong(0, MAX_V);
            inf.readEoln();
            l ^= acc;
            r ^= acc;
            x ^= acc;
            
            l--;
            ensure(0 <= l && l <= r && r <= n);
            ensure(0 <= x && x < mod);
            seg.apply(l, r, x);
        } else {
            long long l = inf.readLong(0, MAX_V);
            inf.readSpace();
            long long r = inf.readLong(0, MAX_V);
            inf.readEoln();
            l ^= acc;
            r ^= acc;
            l--;
            ensure(0 <= l && l <= r && r <= n);
            int res = seg.prod(l, r).value;
            acc = (acc + res) % mod;
            ensure(0 <= acc && acc < mod);
            ensure(0 <= res && res < mod);
        }
    }
    inf.readEof();
    return 0;
}
