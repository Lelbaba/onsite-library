#include <bits/stdc++.h>
using namespace std;
using LL = long long;

/*
 * Solves SPOJ-ahocur
 * source: AlphaQ
 */

/*....................................................................*/

const int N = 2000, L = 105;

struct AhoCorasick {
    int N, P;
    const int A = 256;
    vector<vector<int>> next;
    vector<int> link, out_link, end_in_pattern;
    vector<vector<int>> out;

    AhoCorasick() : N(0), P(0) { node(); }

    int node() {
        next.emplace_back(A, 0);
        link.emplace_back(0);
        out_link.emplace_back(0);
        out.emplace_back(0);
        end_in_pattern.emplace_back(0);
        return N++;
    }

    inline int get(char c) { return c; }

    int addPattern(const string T) {
        int u = 0;
        for (auto c : T) {
            if (!next[u][get(c)]) next[u][get(c)] = node();
            u = next[u][get(c)];
        }
        out[u].push_back(P);
        end_in_pattern[u] = 1;
        return P++;
    }

    void pushLinks() {
        queue<int> q;
        for (q.push(0); !q.empty();) {
            int u = q.front();
            q.pop();
            for (int c = 0; c < A; ++c) {
                int v = next[u][c];
                if (!v) next[u][c] = next[link[u]][c];
                else {
                    link[v] = u ? next[link[u]][c] : 0;
                    out_link[v] = out[link[v]].empty() ? out_link[link[v]] : link[v];
                    q.push(v);
                }
                end_in_pattern[v] |= end_in_pattern[out_link[v]];
            }
        }
    }

    int advance(int u, char c) {
        while (u && !next[u][get(c)]) u = link[u];
        u = next[u][get(c)];
        return u;
    }
};
/*....................................................................*/
double dp[L][N];
int vis[L][N], tc;

double prob[256];

int len;

double call(int idx, int u, AhoCorasick& Aho) {
    if (Aho.end_in_pattern[u]) return 0.0;
    if (idx == len) return 1.0;
    if (vis[idx][u] == tc) return dp[idx][u];

    vis[idx][u] = tc;
    dp[idx][u] = 0;
    for (char c = '0'; c <= 'z'; c++) {
        dp[idx][u] += prob[c] * call(idx + 1, Aho.advance(u, c), Aho);
    }

    return dp[idx][u];
}

int main() {
    cin.tie(0)->sync_with_stdio(0);

    int T;
    cin >> T;

    for (tc = 1; tc <= T; tc++) {
        AhoCorasick Aho;
        int k;
        cin >> k;
        while (k--) {
            string pat;
            cin >> pat;
            Aho.addPattern(pat);
        }
        Aho.pushLinks();

        int n;
        cin >> n;
        for (char c = '0'; c <= 'z'; c++) prob[c] = 0.0;
        for (int i = 0; i < n; i++) {
            char c;
            double p;
            cin >> c >> p;
            prob[c] = p;
        }

        cin >> len;

        cout << "Case #" << tc << ": " << fixed << setprecision(6) << call(0, 0, Aho) << '\n';
    }
}
