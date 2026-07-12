// https://codeforces.com/problemset/problem/4/C
use std::collections::HashMap;
use std::io::{self, prelude::*};

fn main() {
    let mut input = Scanner::from(io::stdin().lock());
    let mut writer = io::BufWriter::new(io::stdout().lock());

    let n: usize = input.token();
    let mut cnt: HashMap<String, u32> = HashMap::new();
    for _ in 0..n {
        let name: String = input.token();
        let c = cnt.entry(name.clone()).or_insert(0);
        if *c == 0 {
            writeln!(writer, "OK").unwrap();
        } else {
            writeln!(writer, "{}{}", name, c).unwrap();
        }
        *c += 1;
    }
}

pub struct Scanner<B> {
    reader: B,
    buffer: Vec<String>,
}

impl<R: BufRead> Scanner<R> {
    pub fn token<T: std::str::FromStr>(&mut self) -> T {
        loop {
            if let Some(token) = self.buffer.pop() {
                return token.parse().ok().expect("Failed parse");
            }
            let mut input = String::new();
            self.reader.read_line(&mut input).expect("Failed read");
            self.buffer = input.split_whitespace().rev().map(String::from).collect();
        }
    }
}

impl<R: BufRead> From<R> for Scanner<R> {
    fn from(reader: R) -> Self {
        Self {
            reader,
            buffer: Vec::new(),
        }
    }
}
