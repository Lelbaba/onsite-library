#include <bits/stdc++.h>

using namespace std;

/*
 *   Source: AlphaQ
 *   O(|S| + |alphabet|) Suffix Array
 *   LIM := max{s[i]} + 2
*/
/*....................................................................*/
void inducedSort(const vector<int> &vec, int val_range, vector<int> &SA,
                 const vector<int> &sl, const vector<int> &lms_idx) {
    vector<int> l(val_range, 0), r(val_range, 0);
    for (int c : vec) {
        ++r[c];
        if (c + 1 < val_range) ++l[c + 1];
    }
    partial_sum(l.begin(), l.end(), l.begin());
    partial_sum(r.begin(), r.end(), r.begin());
    fill(SA.begin(), SA.end(), -1);
    for (int i = lms_idx.size() - 1; i >= 0; --i)
        SA[--r[vec[lms_idx[i]]]] = lms_idx[i];
    for (int i : SA)
        if (i > 0 and sl[i - 1]) SA[l[vec[i - 1]]++] = i - 1;
    fill(r.begin(), r.end(), 0);
    for (int c : vec) ++r[c];
    partial_sum(r.begin(), r.end(), r.begin());
    for (int k = SA.size() - 1, i = SA[k]; k; --k, i = SA[k]) {
        if (i and !sl[i - 1]) SA[--r[vec[i - 1]]] = i - 1;
    }
}

vector<int> suffixArray(const vector<int> &vec, int val_range) {
    const int n = vec.size();
    vector<int> sl(n), SA(n), lms_idx;
    for (int i = n - 2; i >= 0; --i) {
        sl[i] = vec[i] > vec[i + 1] or (vec[i] == vec[i + 1] and sl[i + 1]);
        if (sl[i] and !sl[i + 1]) lms_idx.emplace_back(i + 1);
    }
    reverse(lms_idx.begin(), lms_idx.end());
    inducedSort(vec, val_range, SA, sl, lms_idx);
    vector<int> new_lms_idx(lms_idx.size()), lms_vec(lms_idx.size());
    for (int i = 0, k = 0; i < n; ++i) {
        if (SA[i] > 0 and !sl[SA[i]] and sl[SA[i] - 1])
            new_lms_idx[k++] = SA[i];
    }
    int cur = 0;
    SA[n - 1] = 0;
    for (int k = 1; k < new_lms_idx.size(); ++k) {
        int i = new_lms_idx[k - 1], j = new_lms_idx[k];
        if (vec[i] ^ vec[j]) {
            SA[j] = ++cur;
            continue;
        }
        bool flag = 0;
        for (int a = i + 1, b = j + 1;; ++a, ++b) {
            if (vec[a] ^ vec[b]) {
                flag = 1;
                break;
            }
            if ((!sl[a] and sl[a - 1]) or (!sl[b] and sl[b - 1])) {
                flag = !(!sl[a] and sl[a - 1] and !sl[b] and sl[b - 1]);
                break;
            }
        }
        SA[j] = flag ? ++cur : cur;
    }
    for (int i = 0; i < lms_idx.size(); ++i) lms_vec[i] = SA[lms_idx[i]];
    if (cur + 1 < lms_idx.size()) {
        auto lms_SA = suffixArray(lms_vec, cur + 1);
        for (int i = 0; i < lms_idx.size(); ++i)
            new_lms_idx[i] = lms_idx[lms_SA[i]];
    }
    inducedSort(vec, val_range, SA, sl, new_lms_idx);
    return SA;
}

vector<int> getSuffixArray(const string &s, const int LIM = 128) {
    vector<int> vec(s.size() + 1);
    copy(begin(s), end(s), begin(vec));
    vec.back() = '$';
    auto ret = suffixArray(vec, LIM);
    ret.erase(ret.begin());
    return ret;
}

// build RMQ on it to get LCP of any two suffix
vector<int> getLCParray(const string &s, const vector<int> &SA) {
    int n = s.size(), k = 0;
    vector<int> lcp(n), rank(n);
    for (int i = 0; i < n; ++i) rank[SA[i]] = i;
    for (int i = 0; i < n; ++i, k ? --k : 0) {
        if (rank[i] == n - 1) {
            k = 0;
            continue;
        }
        int j = SA[rank[i] + 1];
        while (i + k < n and j + k < n and s[i + k] == s[j + k]) ++k;
        lcp[rank[i]] = k;
    }
    lcp[n - 1] = 0;
    return lcp;
}

/*....................................................................*/
const int N = (1 << 20) + 5 ;
int logs[N];

void computeLogs(){
    logs[1] = 0;
    for(int i = 2; i < (1 << 20); i++){
        logs[i] = logs[i / 2]+1;
    }
}

template <typename T> struct SparseTable {
    int n;
    vector <vector<T>> table; 
    function <T(T,T)> func;

    SparseTable(vector <T> &v, function <T(T,T)> func): n(v.size()), func(func), table(logs[n] + 1, v) {
        for(int h = 1; h <= logs[n]; h++) {
            for(int i = 0; i + (1 << h) <= n; i++) {
                table[h][i] = func(table[h - 1][i], 
                       table[h - 1][i + (1 << (h - 1))]);
            }
        }
    }

    T query(int l, int r){
        assert(r >= l);
        int pow = logs[r - l + 1];
        return func(table[pow][l], table[pow][r - (1 << pow) + 1]);
    }
};


pair <vector <int>, vector <int>> reversedLCPArray(string s) {
    int n = s.size();

    auto sa = getSuffixArray(s);
    auto lcp = getLCParray(s, sa);


    vector <int> rank(n);
    
    for(int i = 0; i < n; i++) {
        rank[n - sa[i] - 1] = i;
    }

    return make_pair(rank, lcp);
}

string appendReverse(string s) {
    string ans = s;
    reverse(s.begin(), s.end());
    ans += s;
    return ans;
}

int main() {
    cin.tie(0)->sync_with_stdio(0);
    computeLogs();

    string s;
    cin >> s;
    int n = s.size();

    string t = appendReverse(s);

    auto [rank, lcs] = reversedLCPArray(t);

    SparseTable <int> tbl(lcs, [](int x, int y) { return min(x, y); });

    auto query = [&](int i, int j) {
        int l = rank[i], r = rank[j];
        if(l > r) swap(l, r);
        return tbl.query(l, r - 1);
    };

    vector <int> min_prefix(n + 1, n + 1);
    min_prefix[n] = -1;

    for(int len = 1, i = 0; len <= n; len++) {
        while(i <= n and query(n + len - 1, i) < len) i++;
        min_prefix[n - len] = i;
    }

    vector<int> d1(n);
    for (int i = 0, l = 0, r = -1; i < n; i++) {
        int k = (i > r) ? 1 : min(d1[l + r - i], r - i + 1);
        while (0 <= i - k && i + k < n && s[i - k] == s[i + k]) k++;
        d1[i] = k--;  
        if (i + k > r) l = i - k, r = i + k;
    }

    auto getPrefixSuffix = [&](int m) {
        int lo = m + d1[m], hi = n, ans = n;
        while(lo <= hi) {
            int mid = lo + hi >> 1;
            if(min_prefix[mid] <= m - d1[m]) ans = mid, hi = mid - 1;
            else lo = mid + 1;
        }
        return ans;
    };

    int idx = 0, max_len = 0;

    for(int i = 1; i < n; i++) {
        int suf = getPrefixSuffix(i);
        if(d1[i] + (n - suf) > max_len) {
            idx = i, max_len = d1[i] + n - suf;
        }
    }

    vector <pair <int, int>> ans;

    auto push = [&](int l, int r) {
        if(l > r) return;
        else ans.emplace_back(l, r);
    };

    int suf = getPrefixSuffix(idx);
    push(min_prefix[suf] - (n - suf - 1) + 1, min_prefix[suf] + 1);
    push(idx - d1[idx] + 2, idx + d1[idx]);
    push(suf + 1, n);

    cout << ans.size() << '\n';
    for(auto [x, y] : ans) {
        cout << x << ' ' << (y - x + 1) << '\n';
    }
}
