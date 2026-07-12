// https://codeforces.com/problemset/problem/4/C
#include <bits/stdc++.h>
using namespace std;

#ifdef LOCAL
#include "debug.hpp"
#else
#define debug(...) 42
#endif

int main() {
  cin.tie(0)->sync_with_stdio(0);
  int n;
  cin >> n;
  map<string, int> cnt;
  while (n--) {
    string name;
    cin >> name;
    int &c = cnt[name];
    if (c == 0)
      cout << "OK\n";
    else
      cout << name << c << "\n";
    c++;
  }
  return 0;
}
