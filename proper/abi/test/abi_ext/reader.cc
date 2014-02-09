#include <abi/ext/reader.h>
#include <cstdio>

constexpr int N = 26;
constexpr char BASE = 'a';
char s[N + 1];

int test_reader_rotatedown() {
  using abi::ext::align_t;
  using abi::ext::ALIGN;
  using abi::ext::rotate_down_addr;
  static_assert(sizeof(align_t) == ALIGN, "sizeof(align_t) != ALIGN");

  union tt {
    abi::ext::align_t v;
    char c[ALIGN];
  };

  for (unsigned int rot = 0; rot <= ALIGN; ++rot) {
    tt v;
    for (unsigned char i = 0; i < ALIGN; ++i) v.c[i] = BASE + i;
    tt w;
    w.v = rotate_down_addr(v.v, rot);

    fprintf(stderr, "v: %.*s, rotated %d bytes down, is w: %.*s\n",
        int(ALIGN), &v.c[0],
        rot,
        int(ALIGN), &w.c[0]);
    for (int i = 0; i < ALIGN; ++i) {
      if (w.c[i] != v.c[(i + rot) % ALIGN]) return 1;
    }
  }
  return 0;
}

int test_reader_rotateup() {
  using abi::ext::align_t;
  using abi::ext::ALIGN;
  using abi::ext::rotate_up_addr;
  static_assert(sizeof(align_t) == ALIGN, "sizeof(align_t) != ALIGN");

  union tt {
    abi::ext::align_t v;
    char c[ALIGN];
  };

  for (unsigned int rot = 0; rot <= ALIGN; ++rot) {
    tt v;
    for (unsigned char i = 0; i < ALIGN; ++i) v.c[i] = BASE + i;
    tt w;
    w.v = rotate_up_addr(v.v, rot);

    fprintf(stderr, "v: %.*s, rotated %d bytes up, is w: %.*s\n",
        int(ALIGN), &v.c[0],
        rot,
        int(ALIGN), &w.c[0]);
    for (int i = 0; i < ALIGN; ++i) {
      if (w.c[(i + rot) % ALIGN] != v.c[i]) return 1;
    }
  }
  return 0;
}

int test_reader_lowmask() {
  using abi::ext::ALIGN;
  using abi::ext::mask_lowaddr_bytes;

  union tt {
    abi::ext::align_t v;
    char c[ALIGN];
  };

  tt t;
  for (int b = 0; b < 2 * ALIGN; ++b) {
    t.v = mask_lowaddr_bytes<abi::ext::align_t>(b);

    fprintf(stderr, "Masking %2d low bytes:", b);
    for (int i = 0; i < ALIGN; ++i) fprintf(stderr, " %#4.2hhx", t.c[i]);

    bool same = true;
    for (int i = 0; i < ALIGN; ++i) {
      char expect;
      if (i < b)
        expect = 0xff;
      else
        expect = 0x00;
      if (t.c[i] != expect) same = false;
    }
    if (same)
      fprintf(stderr, "   \\o/\n");
    else
      return 1;
  }
  return 0;
}

int test_reader_highmask() {
  using abi::ext::ALIGN;
  using abi::ext::mask_highaddr_bytes;

  union tt {
    abi::ext::align_t v;
    char c[ALIGN];
  };

  tt t;
  for (int b = 0; b < 2 * ALIGN; ++b) {
    t.v = mask_highaddr_bytes<abi::ext::align_t>(b);

    fprintf(stderr, "Mask, starting at byte %2d:", b);
    for (int i = 0; i < ALIGN; ++i) fprintf(stderr, " %#4.2hhx", t.c[i]);

    bool same = true;
    for (int i = 0; i < ALIGN; ++i) {
      char expect;
      if (i < b)
        expect = 0x00;
      else
        expect = 0xff;
      if (t.c[i] != expect) same = false;
    }
    if (same)
      fprintf(stderr, "   \\o/\n");
    else {
      fprintf(stderr, "\n");
      return 1;
    }
  }
  return 0;
}

int test_reader_merge() {
  using abi::ext::ALIGN;
  using abi::ext::merge_addr;

  union tt {
    abi::ext::align_t v;
    char c[ALIGN];
  };

  tt a, b;

  for (int i = 0; i < ALIGN; ++i) {
    a.c[i] = BASE + i;
    b.c[i] = BASE + ALIGN + i;
  }

  for (int i = 0; i <= ALIGN; ++i) {
    tt x;
    x.v = merge_addr(a.v, b.v, i);

    fprintf(stderr, "Merging %.*s %.*s, off = %d, "
                    "yields: \"%.*s\", expected \"%.*s%.*s\"\n",
            int(ALIGN), &a.c[0], int(ALIGN), &b.c[0], i,
            int(ALIGN), &x.c[0], int(ALIGN - i), &a.c[i], i, &b.c[0]);
    for (int j = 0; j < ALIGN; ++j) {
      if (x.c[j] != BASE + i + j) return 1;
    }
  }
  return 0;
}

int test_reader_read8() {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;

  for (unsigned int i = 0; i < N; ++i) s[i] = BASE + i;
  s[N] = '\0';

  for (unsigned int i = 0; i < N; ++i) {
    auto r = reader<DIR_FORWARD>(&s[i]);
    for (unsigned int j = i; j < N; ++j) {
      int c = r.read8();
      fprintf(stderr, "Read '%c', expected '%c'%s\n", c, j + BASE,
              (c == j + BASE ? "    \\o/" : ""));
      if (c != j + BASE) return 1;
    }
  }
  return 0;
}

int test_reader_readN() {
  using abi::ext::reader;
  using abi::ext::DIR_FORWARD;
  using abi::ext::ALIGN;

  for (unsigned int i = 0; i < N; ++i) s[i] = BASE + i;
  s[N] = '\0';

  union tt {
    abi::ext::align_t v;
    char c[ALIGN];
  };

  for (unsigned int off = 0; off < ALIGN; ++off) {
    auto r = reader<DIR_FORWARD>(&s[off]);
    char* v = &s[off];
    tt t;

    t.v = r.readN();
    fprintf(stderr, "Read \"%-*.*s\", expected \"%-*.*s\"\n",
        int(ALIGN), int(ALIGN), &t.c[0],
        int(ALIGN), int(ALIGN), v);
    for (int i = 0; i < ALIGN; ++i) if (t.c[i] != v[i]) return 1;

    v += ALIGN;
    t.v = r.readN();
    fprintf(stderr, "Read \"%-*.*s\", expected \"%-*.*s\"\n",
        int(ALIGN), int(ALIGN), &t.c[0],
        int(ALIGN), int(ALIGN), v);
    for (int i = 0; i < ALIGN; ++i) if (t.c[i] != v[i]) return 1;
  }
  return 0;
}

int test_reader_read8_backwards() {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;

  for (unsigned int i = 0; i < N; ++i) s[i] = BASE + i;
  s[N] = '\0';

  for (int i = N - 1; i >= 0; --i) {
    auto r = reader<DIR_BACKWARD>(&s[i + 1]);
    for (int j = i; j >= 0; --j) {
      int c = r.read8();
      fprintf(stderr, "Read '%c', expected '%c'%s\n", c, j + BASE,
              (c == j + BASE ? "    \\o/" : ""));
      if (c != j + BASE) return 1;
    }
  }
  return 0;
}

int test_reader_readN_backwards() {
  using abi::ext::reader;
  using abi::ext::DIR_BACKWARD;
  using abi::ext::ALIGN;

  for (unsigned int i = 0; i < N; ++i) s[i] = BASE + i;
  s[N] = '\0';

  union tt {
    abi::ext::align_t v;
    char c[ALIGN];
  };

  for (int off = N - 1; off >= N - ALIGN; --off) {
    auto r = reader<DIR_BACKWARD>(&s[off + 1]);
    char* v = &s[off + 1 - ALIGN];
    tt t;

    t.v = r.readN();
    fprintf(stderr, "Read \"%-*.*s\", expected \"%-*.*s\"\n",
        int(ALIGN), int(ALIGN), &t.c[0],
        int(ALIGN), int(ALIGN), v);
    for (int i = 0; i < ALIGN; ++i) if (t.c[i] != v[i]) return 1;

    v -= ALIGN;
    t.v = r.readN();
    fprintf(stderr, "Read \"%-*.*s\", expected \"%-*.*s\"\n",
        int(ALIGN), int(ALIGN), &t.c[0],
        int(ALIGN), int(ALIGN), v);
    for (int i = 0; i < ALIGN; ++i) if (t.c[i] != v[i]) return 1;
  }
  return 0;
}

int test_reader_consume_bytes() {
  using abi::ext::consume_bytes;
  using abi::ext::ALIGN;

  union tt {
    abi::ext::align_t v;
    char c[ALIGN];
  };

  tt t;
  for (int i = 0; i < ALIGN; ++i) t.c[i] = BASE + i;

  for (int i = 0; i < 2 * ALIGN; ++i) {
    auto rv = consume_bytes(&t.v, 1);
    fprintf(stderr, "Consumed byte '%c', expected '%c'\n",
            int(rv), int(BASE + (i % ALIGN)));
    if (rv != BASE + (i % ALIGN)) return 1;
  }
  return 0;
}

int test_maybe_contains() {
  using abi::ext::collapse;
  using abi::ext::repeat;
  using abi::ext::maybe_contains;
  using abi::ext::ALIGN;

  static_assert(CHAR_BIT == 8, "Kinda assumed CHAR_BIT = 8...");
  static_assert(repeat<uint32_t>(0xff, 8) == 0xffffffff, "Broken repeat.");
  static_assert(repeat<uint32_t>(0xf0, 8) == 0xf0f0f0f0, "Broken repeat.");
  static_assert(repeat<uint32_t>(0x42, 8) == 0x42424242, "Broken repeat.");
  static_assert(collapse(0xf00f, 8) == 0xff, "Broken collapse.");
  static_assert(collapse(0x7010, 8) == 0x70, "Broken collapse.");
  static_assert(collapse(0x021d, 8) == 0x1f, "Broken collapse.");
  static_assert(collapse(0xffffffff, 8) == 0xff, "Broken collapse.");
  static_assert((0xffffffff ^ 0xf0f0f0f0) == 0x0f0f0f0f,
                "Broken math in programmer, or broken xor operation...");
  static_assert(~0xf0f0f0f0 == 0x0f0f0f0f, "Broken bitwise not operation..?");
  static_assert((0xffffffff ^ repeat<uint32_t>(0xf0, 8)) == 0x0f0f0f0f,
                "Ehm... something is very wrong here...");
  static_assert(collapse(~(0x1f2f004f ^ repeat<uint32_t>(0x00, 8)), 8) == 0xff,
                "Logic in maybe_contains is flaky.");
  static_assert(!maybe_contains<uint32_t, uint8_t>(0x00000000, 0xff),
                "Trigger happy maybe_contains.");
  static_assert(maybe_contains<uint32_t, uint8_t>(0xffffffff, 0xff),
                "Broken maybe_contains.");

  union tt {
    abi::ext::align_t v;
    char c[ALIGN];
  };

  tt v;
  for (int i = 0; i < ALIGN; ++i) v.c[i] = BASE + i;

  for (int i = 0; i < ALIGN; ++i) {
    if (!maybe_contains(v.v, uint8_t(BASE + i))) {
      fprintf(stderr, "maybe_contains says v does not contain '%c'\n",
              int(BASE + i));
      return 1;
    }
  }
  return 0;
}


int main() {
  int err = 0;

  err = (err ? err : test_reader_rotatedown());
  err = (err ? err : test_reader_rotateup());
  err = (err ? err : test_reader_lowmask());
  err = (err ? err : test_reader_highmask());
  err = (err ? err : test_reader_merge());
  err = (err ? err : test_reader_read8());
  err = (err ? err : test_reader_readN());
  err = (err ? err : test_reader_read8_backwards());
  err = (err ? err : test_reader_readN_backwards());
  err = (err ? err : test_reader_consume_bytes());
  err = (err ? err : test_maybe_contains());

  return err;
}
