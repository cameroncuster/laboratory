use std::io::{self, prelude::*};
use std::{str, vec};

struct Scanner<R> {
    reader: R,
    buf_str: Vec<u8>,
    buf_iter: str::SplitWhitespace<'static>,
}
impl<R: BufRead> Scanner<R> {
    fn new(reader: R) -> Self {
        Self {
            reader,
            buf_str: vec![],
            buf_iter: "".split_whitespace(),
        }
    }
    fn token<T: str::FromStr>(&mut self) -> T {
        loop {
            if let Some(token) = self.buf_iter.next() {
                return token.parse().ok().expect("Failed parse");
            }
            self.buf_str.clear();
            self.reader
                .read_until(b'\n', &mut self.buf_str)
                .expect("Failed read");
            self.buf_iter = unsafe {
                let slice = str::from_utf8_unchecked(&self.buf_str);
                std::mem::transmute(slice.split_whitespace())
            }
        }
    }
}

fn main() {
    let (stdin, stdout) = (io::stdin(), io::stdout());
    let mut scan = Scanner::new(stdin.lock());
    let mut out = io::BufWriter::new(stdout.lock());

    let t = scan.token::<usize>();
    for _ in 0..t {
        let n = scan.token::<usize>();
        let eds = (0..n - 1)
            .map(|_| (scan.token::<usize>() - 1, scan.token::<usize>() - 1))
            .collect::<Vec<_>>();
        let mut adj = vec![vec![]; n];
        for i in 0..n - 1 {
            let (u, v) = eds[i];
            adj[u].push(i);
            adj[v].push(i);
        }
        fn dfs(
            u: usize,
            p_e: usize,
            adj: &Vec<Vec<usize>>,
            eds: &Vec<(usize, usize)>,
            alive: &mut Vec<bool>,
        ) {
            let mut chs = 0;
            for &i in &adj[u] {
                if i == p_e {
                    continue;
                }
                let v = if eds[i].0 == u { eds[i].1 } else { eds[i].0 };
                dfs(v, i, adj, eds, alive);
                if alive[i] {
                    chs += 1;
                }
            }
            if chs >= 2 {
                alive[p_e] = false;
                if chs > 2 {
                    for &i in &adj[u] {
                        if i == p_e || !alive[i] {
                            continue;
                        }
                        alive[i] = false;
                        chs -= 1;
                        if chs == 2 {
                            break;
                        }
                    }
                }
            }
        }
        let mut alive = vec![true; n - 1];
        let root = (0..n).find(|&i| adj[i].len() == 1).unwrap();
        dfs(root, n, &adj, &eds, &mut alive);
        fn get_other(
            u: usize,
            adj: &Vec<Vec<usize>>,
            eds: &Vec<(usize, usize)>,
            alive: &Vec<bool>,
            seen: &mut Vec<bool>,
        ) -> usize {
            assert!(!seen[u]);
            seen[u] = true;
            for &i in adj[u].iter() {
                if !alive[i] {
                    continue;
                }
                let v = if eds[i].0 == u { eds[i].1 } else { eds[i].0 };
                if seen[v] {
                    continue;
                }
                return get_other(v, adj, eds, alive, seen);
            }
            return u;
        }
        fn alive_deg(u: usize, adj: &Vec<Vec<usize>>, alive: &Vec<bool>) -> usize {
            adj[u]
                .iter()
                .filter(|&&i| alive[i])
                .map(|&i| if alive[i] { 1 } else { 0 })
                .sum()
        }
        let mut seen = vec![false; n];
        let mut ropes = vec![];
        for i in 0..n {
            if !seen[i] && alive_deg(i, &adj, &alive) <= 1 {
                let j = get_other(i, &adj, &eds, &alive, &mut seen);
                ropes.push((i, j));
            }
        }
        let mut rems = vec![];
        for i in 0..n - 1 {
            if !alive[i] {
                rems.push(eds[i]);
            }
        }
        let mut adds = vec![];
        for i in 1..ropes.len() {
            adds.push((ropes[i - 1].1, ropes[i].0));
        }
        assert_eq!(rems.len(), adds.len());
        writeln!(out, "{}", rems.len()).ok();
        for i in 0..rems.len() {
            writeln!(
                out,
                "{} {} {} {}",
                rems[i].0 + 1,
                rems[i].1 + 1,
                adds[i].0 + 1,
                adds[i].1 + 1
            )
            .ok();
        }
    }
}
