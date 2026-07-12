use std::io::{self, prelude::*};

// proconio-style input! Tokens are read lazily line by line, so it also
// works for interactive problems (unlike proconio's read-everything input!).
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
    ($(,)?) => {};
    (, $($rest:tt)*) => { input!($($rest)*); };
    (mut $var:ident: $t:tt $($rest:tt)*) => {
        let mut $var = read_value!($t);
        input!($($rest)*);
    };
    ($var:ident: $t:tt $($rest:tt)*) => {
        let $var = read_value!($t);
        input!($($rest)*);
    };
}

#[allow(unused_macros)]
macro_rules! read_value {
    (($($t:tt),*)) => { ($(read_value!($t),)*) };
    ([$t:tt; $len:expr]) => { (0..$len).map(|_| read_value!($t)).collect::<Vec<_>>() };
    ([$t:tt]) => {{
        let len = read_value!(usize);
        (0..len).map(|_| read_value!($t)).collect::<Vec<_>>()
    }};
    (Chars) => { read_token().chars().collect::<Vec<char>>() };
    (Bytes) => { read_token().bytes().collect::<Vec<u8>>() };
    (Usize1) => { read_value!(usize) - 1 };
    (Isize1) => { read_value!(isize) - 1 };
    ($t:ty) => { read_token().parse::<$t>().expect("parse failed") };
}

#[allow(dead_code)]
fn read_token() -> &'static str {
    use std::sync::Mutex;
    static TOKENS: Mutex<Vec<&'static str>> = Mutex::new(Vec::new());
    let mut tokens = TOKENS.lock().unwrap();
    while tokens.is_empty() {
        let mut line = String::new();
        if io::stdin().read_line(&mut line).expect("read failed") == 0 {
            panic!("input exhausted");
        }
        *tokens = Box::leak(line.into_boxed_str()).split_ascii_whitespace().rev().collect();
    }
    tokens.pop().unwrap()
}

fn main() {
    let mut writer = io::BufWriter::new(io::stdout().lock());
}
