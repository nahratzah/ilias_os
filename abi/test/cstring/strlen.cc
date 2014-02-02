#include <cstring>
#include <cstdio>

struct test_t {
  size_t len;
  const char* s;
};

const test_t test[] = {
  { 0, "" },
  { 1, "a" },
  { 2, "ab" },
  { 3, "abc" },
  { 16, "xxxxxxxxxxxxxxxx" },
  { 512, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
         "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
         "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
         "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
         "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
         "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
         "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
         "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" }
};

int main() {
  const test_t*const b = &test[0];
  const test_t*const e = b + sizeof(test) / sizeof(test[0]);

  for (const test_t* i = b; i != e; ++i) {
    for (size_t off = 0; off <= i->len; ++off) {
      const size_t rv = ::test_std::strlen(i->s + off);
      fprintf(stderr, "strlen(\"%s\") = %zu, expecting %zu\n",
              i->s + off, rv, i->len - off);
      if (rv != i->len - off) return 1;
    }
  }
}
