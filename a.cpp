#include <bits/stdc++.h>
using namespace std;

int main() {
  cin.tie(0)->sync_with_stdio(0);
  int n;
  cin >> n;
  array<int, 20> b{};
  for (int i = 0, y = 0; i < n; i++) {
    int tmp;
    cin >> tmp;
    int x = tmp ^ y;
    y ^= tmp;
    for (int bit = 19; bit >= 0; bit--) {
      if (x & (1 << bit)) {
        x ^= b[bit];
      }
    }
    if (x > 0) {
      b[__lg(x)] = x;
    }
  }
  vector<int> sums(1 << 20);
  for (int mask = 1; mask < (1 << 20); mask++) {
    for (int i = 0; i < 20; i++) {
      if (mask & (1 << i)) {
        sums[mask] ^= b[i];
      }
    }
  }
  ranges::sort(sums);
  sums.erase(unique(begin(sums), end(sums)), end(sums));
  cout << ssize(sums) << '\n';
  for (int x : sums) {
    cout << x << ' ';
  }
  cout << '\n';
  return 0;
}
