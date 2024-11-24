#include <cstdio>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <stack>
#include <vector>
#include <utility>
#include <map>
#include <set>
#include <random>
#include "../../common/xrand.h"
#include "constraints.hpp"

#include "titan_cpplib/data_structures/dynamic_lazy_segment_tree.cpp"

const int mod = 998244353;
const int MAX_X = mod - 1;

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

const int num_of_type = 2;
std::string case_type[num_of_type] = {
    "10_random_small",
    "20_random_large"
};
int num_of_case[num_of_type] = {20, 20};
int min_n[num_of_type] = {1, 10000};
int max_n[num_of_type] = {100, 10000};
int min_q[num_of_type] = {1, 100000};
int max_q[num_of_type] = {10000, 100000};

XRand Rnd(334);

int main() {
    for(int typenum=0; typenum<num_of_type; ++typenum) {
        for(int casenum=0;casenum<num_of_case[typenum];++casenum){

            std::string file_name=case_type[typenum]+std::to_string(casenum)+".in";
            std::ofstream output(file_name);

            int n = Rnd.NextInt(min_n[typenum], max_n[typenum]);
            int q = Rnd.NextInt(min_q[typenum], max_q[typenum]);

            output << n << " " << q << "\n";

            int acc = 0;

            S init = {0, 1};
            titan23::DynamicLazySegmentTree<IndexType, S, op, e, F, mapping, composition, id, pow> seg(n+1, init);

            for (int qidx = 0; qidx < q; ++qidx) {
                int com = Rnd.NextInt(1, 2);
                if (com == 1) {
                    int l = Rnd.NextInt(1, n);
                    int r = Rnd.NextInt(1, n);
                    if (l > r) std::swap(l, r);
                    int x = Rnd.NextInt(1, MAX_X);

                    seg.apply(l-1, r, x);

                    int a = l ^ acc;
                    int b = r ^ acc;
                    int c = x ^ acc;
                    output << com << " " << a << " " << b << " " << c << "\n";
                } else if (com == 2) {
                    int l = Rnd.NextInt(1, n);
                    int r = Rnd.NextInt(1, n);
                    if (l > r) std::swap(l, r);

                    int a = l ^ acc;
                    int b = r ^ acc;

                    int res = seg.prod(l-1, r).value;
                    acc = (acc + res) % mod;

                    output << com << " " << a << " " << b << "\n";
                }
            }

            output.close();
        }
    }
    return 0;
}
