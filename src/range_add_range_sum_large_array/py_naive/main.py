#!/usr/bin/pypy3

import sys
input = lambda: sys.stdin.buffer.readline().rstrip()

mod = 998244353
INT_MAX = 2147483647

#  -----------------------  #

n, q = map(int, input().split())

A = [0] * n
acc = 0

for _ in range(q):
    com, *qu = map(int, input().split())

    if com == 1:
        l, r, x = qu
        assert 0 <= l < INT_MAX
        assert 0 <= r < INT_MAX
        assert 0 <= x < INT_MAX
        l ^= acc
        r ^= acc
        x ^= acc
        if l > r:
            l, r = r, l
        l -= 1
        assert 0 <= l <= r <= n
        for i in range(l, r):
            A[i] = (A[i] + x) % mod
    elif com == 2:
        l, r = qu
        l ^= acc
        r ^= acc
        assert 0 <= l < INT_MAX
        assert 0 <= r < INT_MAX

        l -= 1
        s = sum(A[l:r]) % mod
        assert 0 <= l <= r <= n
        acc = (acc + s) % mod
        print(s)
