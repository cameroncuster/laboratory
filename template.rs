#[allow(unused_imports)]
use std::cmp::{Ordering, Reverse, max, min};
#[allow(unused_imports)]
use std::collections::*;
use std::io::{self, prelude::*};

// proconio-style input! Slurps all of stdin on first use (fastest); use
// input_interactive! for interactive problems (reads lazily line by line).
//
//   input! {
//       n: usize,
//       mut k: i64,
//       s: Chars,                 // Vec<char>; Bytes -> Vec<u8>
//       a: [i64; n],              // Vec<i64>
//       g: [[i32; n]; m],         // Vec<Vec<i32>>, m rows x n cols
//       j: [[i32]; n],            // jagged: each row starts with its length
//       e: [(Usize1, Usize1); m], // 1-indexed -> 0-indexed
//   }
#[allow(unused_macros)]
macro_rules! input {
  ($($rest:tt)*) => { input_inner!(read_token, $($rest)*); };
}

#[allow(unused_macros)]
macro_rules! input_interactive {
  ($($rest:tt)*) => { input_inner!(read_token_line, $($rest)*); };
}

#[allow(unused_macros)]
macro_rules! input_inner {
  ($rd:ident $(,)?) => {};
  ($rd:ident, mut $var:ident: $t:tt $($rest:tt)*) => {
    let mut $var = read_value!($rd, $t);
    input_inner!($rd $($rest)*);
  };
  ($rd:ident, $var:ident: $t:tt $($rest:tt)*) => {
    let $var = read_value!($rd, $t);
    input_inner!($rd $($rest)*);
  };
}

#[allow(unused_macros)]
macro_rules! read_value {
  ($rd:ident, ($($t:tt),*)) => { ($(read_value!($rd, $t),)*) };
  ($rd:ident, [$t:tt; $len:expr]) => { (0..$len).map(|_| read_value!($rd, $t)).collect::<Vec<_>>() };
  ($rd:ident, [$t:tt]) => {{
    let len = read_value!($rd, usize);
    (0..len).map(|_| read_value!($rd, $t)).collect::<Vec<_>>()
  }};
  ($rd:ident, Chars) => { $rd().chars().collect::<Vec<char>>() };
  ($rd:ident, Bytes) => { $rd().bytes().collect::<Vec<u8>>() };
  ($rd:ident, Usize1) => { read_value!($rd, usize) - 1 };
  ($rd:ident, Isize1) => { read_value!($rd, isize) - 1 };
  ($rd:ident, $t:ty) => { $rd().parse::<$t>().expect("parse failed") };
}

#[allow(dead_code)]
fn read_token() -> &'static str {
  use std::sync::Mutex;
  static TOKENS: Mutex<Option<std::str::SplitAsciiWhitespace<'static>>> = Mutex::new(None);
  TOKENS
    .lock()
    .unwrap()
    .get_or_insert_with(|| {
      let mut buf = String::new();
      io::stdin().read_to_string(&mut buf).expect("read failed");
      Box::leak(buf.into_boxed_str()).split_ascii_whitespace()
    })
    .next()
    .expect("input exhausted")
}

#[allow(dead_code)]
fn read_token_line() -> &'static str {
  use std::sync::Mutex;
  static TOKENS: Mutex<Vec<&'static str>> = Mutex::new(Vec::new());
  let mut tokens = TOKENS.lock().unwrap();
  while tokens.is_empty() {
    let mut line = String::new();
    if io::stdin().read_line(&mut line).expect("read failed") == 0 {
      panic!("input exhausted");
    }
    *tokens = Box::leak(line.into_boxed_str())
      .split_ascii_whitespace()
      .rev()
      .collect();
  }
  tokens.pop().unwrap()
}

fn main() {
  let mut writer = io::BufWriter::new(io::stdout().lock());
}
