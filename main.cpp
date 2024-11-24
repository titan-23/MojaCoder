#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")

#include <bits/stdc++.h>
using namespace std;

#include <iostream>
#include <cassert>
#include <memory>

using namespace std;

// DynamicLazySegmentTree
namespace titan23 {

    /**
     * @brief `[0, u)` の列を管理する、必要なところだけ作る遅延セグ木
     *
     * - apply / set : `O(logu)` time, `O(logu)` space
     *
     * - prod  / get : `O(logu)` time, `O(1)` space
     *
     * @tparam IndexType 添え字を表すインデックス long long を推奨 和がオーバーフローしないことが条件かな
     * @tparam T モノイドの型
     * @tparam (*op)(T, T) モノイドの2項演算
     * @tparam (*e)() モノイドの単位元
     * @tparam F 作用素の型
     * @tparam (*mapping)(F, T) 遅延セグ木のアレ
     * @tparam (*composition)(F, F) 遅延セグ木のアレ
     * @tparam (*id)() 遅延セグ木のアレ
     * @tparam (*pow)(T, IndexType) T=op(T,T)をk回繰り返した結果を返す関数
     */
    template <class IndexType,
            class T,
            T (*op)(T, T),
            T (*e)(),
            class F,
            T (*mapping)(F, T),
            F (*composition)(F, F),
            F (*id)(),
            T (*pow)(T, IndexType)>
    class DynamicLazySegmentTree {
      private:
        static int bit_length(long long x) {
            return x == 0 ? 0 : 64 - __builtin_clzll(x);
        }

        struct Node;
        using NodePtr = Node*;
        // using NodePtr = shared_ptr<Node>;
        NodePtr root;
        IndexType u;

        struct Node {
            NodePtr left, right;
            IndexType l, r;
            T key, data; // 子がないとき、data=pow(key, r-l)が成り立つ
            F lazy;

            Node() {}

            Node(IndexType l, IndexType r, T key) :
                    left(nullptr), right(nullptr),
                    l(l), r(r),
                    key(key), data(pow(key, r-l)) {
                    // key(key), data(r-l == 1 ? key : pow(key, r-l)) {
                assert(l < r);
            }

            bool is_leaf() const {
                return this->r - this->l == 1;
            }

            void apply(const F f) {
                if (f == id()) return;
                this->key = mapping(f, this->key);
                this->data = mapping(f, this->data);
                if (!is_leaf()) this->lazy = composition(f, this->lazy);
            }

            void propagate() {
                if (is_leaf()) return;
                if (this->left) {
                    this->left->apply(this->lazy);
                } else {
                    this->left = new Node(l, mid(), this->key);
                }
                if (this->right) {
                    this->right->apply(this->lazy);
                } else {
                    this->right = new Node(mid(), r, this->key);
                }
                this->lazy = id();
            }

            void update() {
                if (this->left) {
                    this->data = this->left->data;
                } else {
                    this->data = e();
                }
                if (this->right) {
                    this->data = op(this->data, this->right->data);
                }
            }

            IndexType mid() const {
                return (this->l + this->r) / 2;
            }
        };

      private:
        T inner_prod(NodePtr node, const IndexType l, const IndexType r) {
            if (!node || l >= r || r <= node->l || node->r <= l) return e();
            if (l <= node->l && node->r <= r) return node->data;
            if ((!node->left) && (!node->right)) return pow(node->key, r - l);
            node->propagate();
            return op(
                inner_prod(node->left, l, min(r, node->mid())),
                inner_prod(node->right, max(l, node->mid()), r)
            );
        }

        void inner_apply(NodePtr node, IndexType l, IndexType r, F f) {
            if (!node || l >= r || r <= node->l || node->r <= l) return;
            if (l <= node->l && node->r <= r) {
                node->apply(f);
                return;
            }
            node->propagate();
            inner_apply(node->left, l, min(r, node->mid()), f);
            inner_apply(node->right, max(l, node->mid()), r, f);
            node->update();
        }

        void inner_set(NodePtr node, IndexType k, T val) {
            assert(node);
            if (node->is_leaf()) {
                assert(node->l == k && node->r == k+1);
                node->key = val;
                node->data = val;
                return;
            }
            node->propagate();
            if (k < node->mid()) {
                inner_set(node->left, k, val);
            } else {
                inner_set(node->right, k, val);
            }
            node->update();
        }

        T inner_get(NodePtr node, IndexType k) {
            assert(node);
            while (true) {
                if (node->is_leaf()) {
                    assert(node->l == k && node->r == k+1);
                    return node->key;
                }
                if (k < node->mid()) {
                    if (!node->left) return node->key;
                    node->propagate();
                    node = node->left;
                } else {
                    if (!node->right) return node->key;
                    node->propagate();
                    node = node->right;
                }
            }
        }

      public:

        DynamicLazySegmentTree() : root(nullptr), u(0) {}

        //! 初期値 `e()` , `[0, u)` の区間を管理する `DynamicLazySegmentTree` を作成する
        DynamicLazySegmentTree(const IndexType u_) {
            assert(u_ > 0);
            this->u = (IndexType)1 << bit_length(u_);
            this->root = new Node(0, this->u, e());
        }

        //! 初期値 `init` , `[0, u)` の区間を管理する `DynamicLazySegmentTree` を作成する
        DynamicLazySegmentTree(const IndexType u_, const T init) {
            assert(u_ > 0);
            this->u = (IndexType)1 << bit_length(u_);
            this->root = new Node(0, this->u, init);
        }

        //! `[l, r)` の集約値を返す / `O(logu)` time, `O(logu)` space
        T prod(IndexType l, IndexType r) {
            assert(0 <= l && l <= r && r <= u);
            return inner_prod(this->root, l, r);
        }

        //! `[l, r)` に `f` を作用させる / `O(logu)` time, `O(logu)` space
        void apply(IndexType l, IndexType r, F f) {
            assert(0 <= l && l <= r && r <= u);
            inner_apply(this->root, l, r, f);
        }

        //! `k` 番目の値を取得する / `O(logu)` time, `O(1)` space
        T get(IndexType k) {
            return inner_get(this->root, k);
        }

        //! `k` 番目の値を `val` に更新する / `O(logu)` time, `O(logu)` space
        void set(IndexType k, T val) {
            assert(0 <= k && k < u);
            inner_set(this->root, k, val);
        }

        //! 適当に表示する
        void print() {
            for (IndexType i = 0; i < u; ++id) {
                cout << get(i) << ", ";
            }
            cout << endl;
        }
    };
}  // namespace titan23
#include <cassert>
#include <unordered_set>
#include <vector>
using namespace std;

// Random
namespace titan23 {

    /**
     * @brief (疑似)乱数生成クラス(XOR shift)
     */
    class Random {
      private:
        unsigned int _x, _y, _z, _w;

        unsigned int _xor128() {
            const unsigned int t = _x ^ (_x << 11);
            _x = _y;
            _y = _z;
            _z = _w;
            _w = (_w ^ (_w >> 19)) ^ (t ^ (t >> 8));
            return _w;
        }

      public:
        Random() : _x(123456789),
                   _y(362436069),
                   _z(521288629),
                   _w(88675123) {}

        //! `[0, 1]` の乱数を返す(実数)
        double random() { return (double)(_xor128()) / 0xFFFFFFFF; }

        //! `[0, end]` の乱数を返す(整数)
        int randint(const int end) {
            assert(0 <= end);
            return (((unsigned long long)_xor128() * (end+1)) >> 32);
        }

        //! `[begin, end]` の乱数を返す(整数)
        int randint(const int begin, const int end) {
            assert(begin <= end);
            return begin + (((unsigned long long)_xor128() * (end-begin+1)) >> 32);
        }

        //! `[0, end)` の乱数を返す(整数)
        int randrange(const int end) {
            assert(0 < end);
            return (((unsigned long long)_xor128() * end) >> 32);
        }

        //! `[begin, end)` の乱数を返す(整数)
        int randrange(const int begin, const int end) {
            assert(begin < end);
            return begin + (((unsigned long long)_xor128() * (end-begin)) >> 32);
        }

        //! `[0, u64_MAX)` の乱数を返す / zobrist hash等の使用を想定
        unsigned long long rand_u64() {
            return ((unsigned long long)_xor128() << 32) | _xor128();
        }

        //! `[0, end)` の異なる乱数を2つ返す
        pair<int, int> rand_pair(const int end) {
            assert(end >= 2);
            int u = randrange(end);
            int v = u + randrange(1, end);
            if (v >= end) v -= end;
            if (u > v) swap(u, v);
            return {u, v};
        }

        //! `[begin, end)` の異なる乱数を2つ返す
        pair<int, int> rand_pair(const int begin, const int end) {
            assert(end - begin >= 2);
            int u = randrange(begin, end);
            int v = (u + randrange(1, end-begin));
            if (v >= end) v -= (end-begin);
            if (u > v) swap(u, v);
            return {u, v};
        }

        //! Note `a`は非const
        vector<int> rand_vec(const int cnt, vector<int> &a) {
            int n = a.size();
            for (int i = 0; i < cnt; ++i) {
                int j = randrange(i, n);
                swap(a[i], a[j]);
            }
            vector<int> res(a.begin(), a.begin()+cnt);
            return res;
        }

        //! `[begin, end)` の乱数を返す(実数)
        double randdouble(const double begin, const double end) {
            assert(begin < end);
            return begin + random() * (end-begin);
        }

        //! `vector` をインプレースにシャッフルする / `O(n)`
        template <typename T>
        void shuffle(vector<T> &a) {
            int n = a.size();
            for (int i = 0; i < n-1; ++i) {
                int j = randrange(i, n);
                swap(a[i], a[j]);
            }
        }

        template <typename T>
        vector<T> choices(const vector<T> &a, const int k) {
            assert(a.size() >= k);
            vector<T> r(k);
            unordered_set<int> seen;
            for (int i = 0; i < k; ++i) {
                int x = randrange(a.size());
                while (seen.find(x) != seen.end()) x = randrange(a.size());
                seen.insert(x);
                r[i] = a[x];
            }
            return r;
        }

        template <typename T>
        T choice(const vector<T> &a) {
            return a[randrange(a.size())];
        }

        template <typename T>
        T choice(const vector<T> &a, const vector<int> &w, bool normal) {
            assert(normal == false);
            assert(a.size() == w.size());
            double sum = 0.0;
            for (const int &x: w) sum += x;
            assert(sum > 0);
            vector<double> x(w.size());
            for (int i = 0; i < x.size(); ++i) {
                x[i] = (double)w[i] / sum;
                if (i-1 >= 0) x[i] += x[i-1];
            }
            return choice(a, x);
        }

        template <typename T>
        T choice(const vector<T> &a, const vector<double> &w) {
            double i = random();
            int l = -1, r = a.size()-1;
            while (r - l > 1) {
                int mid = (l + r) / 2;
                if (w[mid] <= i) l = mid;
                else r = mid;
            }
            return a[r];
        }
    };
} // namespace titan23

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

void solve() {
    int n, q;
    n = 1e9;
    q = 2e5;
    titan23::Random rnd;

    S init = {0, 1};
    long long acc = 0;
    titan23::DynamicLazySegmentTree<IndexType, S, op, e, F, mapping, composition, id, pow> seg(n+1, init);

    for (int qidx = 0; qidx < q; ++qidx) {
        int com = rnd.randint(1, 2);
        if (com == 1) { // apply
            int l = rnd.randint(1, n), r = rnd.randint(1, n);
            int x = rnd.randrange(1, mod);
            if (l > r) swap(l, r);
            --l;
            seg.apply(l, r, x);
        } else {
            int l = rnd.randint(1, n), r = rnd.randint(1, n);
            if (l > r) swap(l, r);
            --l;
            S res = seg.prod(l, r);
            acc += res.value;
            cout << res.value << "\n";
        }
    }
    cout << acc << endl;
}

int main() {
    solve();
    return 0;
}
