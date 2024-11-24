#!/usr/bin/pypy3

import sys

input = lambda: sys.stdin.buffer.readline().rstrip()


class DynamicLazySegmentTree:

    def __init__(self, u, op, e, mapping, composition, id_, pow_, init):
        self.u = 1 << (u - 1).bit_length()
        self.e = e
        self.op = op
        self.mapping = mapping
        self.composition = composition
        self.id_ = id_
        self.pow_ = pow_
        self.root = self.Node(0, self.u, init, pow_(init, self.u))

    class Node:

        def __init__(self, l, r, key, data):
            self.left = None
            self.right = None
            self.l = l
            self.r = r
            self.key = key
            self.data = data
            self.lazy = None

        def is_leaf(self):
            return self.r - self.l == 1

        def apply(self, f, mapping, composition, id_):
            if f == id_():
                return
            self.key = mapping(f, self.key)
            self.data = mapping(f, self.data)
            if not self.is_leaf():
                if self.lazy is None:
                    self.lazy = f
                else:
                    self.lazy = composition(f, self.lazy)

        def propagate(self, mapping, composition, id_, pow_):
            if self.is_leaf():
                return
            if self.left is None:
                self.left = DynamicLazySegmentTree.Node(
                    self.l, self.mid(), self.key, pow_(self.key, self.mid() - self.l)
                )
            elif self.lazy is not None:
                self.left.apply(self.lazy, mapping, composition, id_)
            if self.right is None:
                self.right = DynamicLazySegmentTree.Node(
                    self.mid(), self.r, self.key, pow_(self.key, self.r - self.mid())
                )
            elif self.lazy is not None:
                self.right.apply(self.lazy, mapping, composition, id_)
            self.lazy = None

        def update(self, op, e):
            self.data = e()
            if self.left:
                self.data = op(self.data, self.left.data)
            if self.right:
                self.data = op(self.data, self.right.data)

        def mid(self):
            return (self.l + self.r) // 2

    def prod(self, l, r):
        assert 0 <= l <= r <= self.u
        return self._inner_prod(self.root, l, r)

    def _inner_prod(self, node: Node, l, r):
        if not node or l >= r or r <= node.l or node.r <= l:
            return self.e()
        if l <= node.l and node.r <= r:
            return node.data
        node.propagate(self.mapping, self.composition, self.id_, self.pow_)
        return self.op(
            self._inner_prod(node.left, l, min(r, node.mid())),
            self._inner_prod(node.right, max(l, node.mid()), r),
        )

    def apply(self, l, r, f):
        assert 0 <= l <= r <= self.u
        self._inner_apply(self.root, l, r, f)

    def _inner_apply(self, node: Node, l, r, f):
        if not node or l >= r or r <= node.l or node.r <= l:
            return
        if l <= node.l and node.r <= r:
            node.apply(f, self.mapping, self.composition, self.id_)
            return
        node.propagate(self.mapping, self.composition, self.id_, self.pow_)
        self._inner_apply(node.left, l, min(r, node.mid()), f)
        self._inner_apply(node.right, max(l, node.mid()), r, f)
        node.update(self.op, self.e)


mod = 998244353

bit = 31
msk = (1 << bit) - 1


def op(a, b):
    a0, a1 = a >> bit, a & msk
    b0, b1 = b >> bit, b & msk
    return ((a0 + b0) % mod) << bit | (a1 + b1)


def e():
    return 0


def mapping(f, x):
    x0, x1 = x >> bit, x & msk
    return ((x0 + f * x1) % mod) << bit | x1


def composition(f, g):
    return (f + g) % mod


def id_():
    return 0


def pow_(s, k):
    s0, s1 = s >> bit, s & msk
    assert (s1 * k) <= msk
    return (s0 * k % mod) << bit | (s1 * k)


INT_MAX = 2**31

#  -----------------------  #

n, q = map(int, input().split())

seg: DynamicLazySegmentTree = DynamicLazySegmentTree(
    n + 1, op, e, mapping, composition, id_, pow_, 1
)
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

        l -= 1
        assert 0 <= l <= r <= n
        seg.apply(l, r, x)

    elif com == 2:
        l, r = qu
        assert 0 <= l < INT_MAX
        assert 0 <= r < INT_MAX

        l ^= acc
        r ^= acc

        l -= 1

        assert 0 <= l <= r <= n

        s = seg.prod(l, r) >> bit
        acc = (acc + s) % mod
        print(s)
