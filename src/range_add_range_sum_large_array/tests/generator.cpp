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

const int num_of_type = 4;
std::string case_type[num_of_type] = {
    "10_random_small",
    "20_random_large",
    "30_random_max",
    "40_small_width",
};
int num_of_case[num_of_type] = {10, 10, 5, 5};
int min_n[num_of_type] = {1,          (int)(1e2), (int)(1e8), (int)(1e8)};
int max_n[num_of_type] = {100,        (int)(1e6), (int)(1e9), (int)(1e9)};
int min_q[num_of_type] = {(int)(1e1), (int)(1e5), (int)(1e5), (int)(1e5)};
int max_q[num_of_type] = {(int)(2e2), (int)(2e5), (int)(2e5), (int)(2e5)};

XRand Rnd(334);

int main() {
    for (int typenum = 0; typenum < num_of_type; ++typenum) {
        for (int casenum = 0; casenum < num_of_case[typenum]; ++casenum){

            std::string file_name = case_type[typenum] + std::to_string(casenum) + ".in";
            std::ofstream output(file_name);

            int n = Rnd.NextInt(min_n[typenum], max_n[typenum]);
            int q = Rnd.NextInt(min_q[typenum], max_q[typenum]);

            if (casenum == num_of_case[typenum]-1) {
                n = max_n[typenum];
                q = max_q[typenum];
            }

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

                    if (case_type[typenum] == "40_small_width") {
                        int d = Rnd.NextInt(1, 10);
                        int l = Rnd.NextInt(1, n-d-1);
                        int r = l + d;
                        int x = Rnd.NextInt(1, MAX_X);
                    } else if (case_type[typenum] == "30_random_max") {
                        int d = Rnd.NextInt(1, 1000);
                        int l = d;
                        int r = n - d;
                        int x = Rnd.NextInt(MAX_X-d, MAX_X);
                    }

                    seg.apply(l-1, r, x);

                    int a = l ^ acc;
                    int b = r ^ acc;
                    int c = x ^ acc;
                    output << com << " " << a << " " << b << " " << c << "\n";
                } else if (com == 2) {
                    int l = Rnd.NextInt(1, n);
                    int r = Rnd.NextInt(1, n);
                    if (l > r) std::swap(l, r);

                    if (case_type[typenum] == "40_small_width") {
                        int d = Rnd.NextInt(1, 10);
                        l = Rnd.NextInt(1, n-d-1);
                        r = l + d;
                    } else if (case_type[typenum] == "30_random_max") {
                        int d = Rnd.NextInt(1, 1000);
                        l = d;
                        r = n - d;
                    }

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
