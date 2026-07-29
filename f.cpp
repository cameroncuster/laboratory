#include <bits/stdc++.h>
using namespace std;

#ifdef LOCAL
#include "debug.hpp"
#else
#define dbg(...) 42
#endif

int main() {
  cin.tie(0)->sync_with_stdio(0);
  int n, q;
  cin >> n >> q;
  vector<vector<int>> tree(n);
  for (int i = 0; i < n - 1; i++) {
    int u, v;
    cin >> u >> v, --u, --v;
    tree[u].push_back(v);
    tree[v].push_back(u);
  }

  vector<vector<array<int, 2>>> cons(n);
  for (int i = 0; i < q; i++) {
    int u, v, x;
    cin >> u >> v >> x, --u, --v;
    cons[u].push_back({v, x});
    cons[v].push_back({u, x});
  }

  vector<int> p(n);
  vector<int> sz(n);
  vector<bool> vis(n);

  for (int s = 0; s < n; s++) {
    if (vis[s])
      continue;
    [&](this auto &&dfs, int u) -> void {
      sz[s]++;
      vis[u] = 1;
      for (auto [v, x] : cons[u]) {
        if (!vis[v]) {
          p[v] = p[u] ^ x;
          dfs(v);
        } else if (p[v] != (p[u] ^ x)) {
          cout << "NO" << '\n';
          exit(0);
        }
      }
    }(s);
  }

  cout << "YES" << '\n';

  int s = ranges::fold_left(p, 0, bit_xor());
  for (int v = 0; v < n; v++) {
    if (sz[v] & 1) {
    }
  }

  return 0;
}
