// https://codeforces.com/problemset/problem/4/C
fun main() {
  val n = readInt()
  val cnt = HashMap<String, Int>()
  val sb = StringBuilder()
  repeat(n) {
    val name = readln()
    val c = cnt.getOrDefault(name, 0)
    if (c == 0) {
      sb.appendLine("OK")
    } else {
      sb.appendLine("$name$c")
    }
    cnt[name] = c + 1
  }
  print(sb)
}

private fun readInt() = readln().toInt()

private fun readLong() = readln().toLong()

private fun readDouble() = readln().toDouble()

private fun readStrings() = readln().split(" ")

private fun readInts() = readStrings().map { it.toInt() }

private fun readLongs() = readStrings().map { it.toLong() }

private fun readDoubles() = readStrings().map { it.toDouble() }
