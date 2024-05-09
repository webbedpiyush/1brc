#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <string.h>
#include <iomanip>

using namespace std;

struct Result {
  string city;
  int count;
  double sum, min, max;
};

static const char* parseDouble(double* dest, const char* s) {
  // parse sign
  double mod;
  if (*s == '-') {
    mod = -1.0;
    s++;
  } else {
    mod = 1.0;
  }

  if (s[1] == '.') {
    *dest = (((double)s[0] + (double)s[2] / 10.0) - 1.1 * '0') * mod;
    return s + 4;
  }

  *dest = ((double)((s[0]) * 10 + s[1]) + (double)s[3] / 10.0 - 11.1 * '0') * mod;
  return s + 5;
}

static unsigned int hash1(const unsigned char* data, int n) {
  unsigned int hash = 0;

  for (int i = 0; i < n; i++) {
    hash = (hash * 31) + data[i];
  }

  return hash;
}

static bool cmp(const Result& a, const Result& b) {
  return a.city < b.city;
}

int main(int argc, const char** argv) {
  const char* file = "measurements.txt";
  if (argc > 1) {
    file = argv[1];
  }

  ifstream fh(file);
  if (!fh) {
    cerr << "error opening file" << endl;
    return EXIT_FAILURE;
  }

  vector<Result> results;
  char buf[1 << 10];
  int map[4096];
  memset(map, -1, 4096 * sizeof(int));

  while (fh.getline(buf, 1 << 10)) {
    char* pos = strchr(buf, ';');
    *pos = 0x0;
    double measurement;
    parseDouble(&measurement, pos + 1);

    // find index of group by key through hash with linear probing
    int h = hash1((unsigned char*)buf, pos - buf) & (4095);
    while (map[h]!= -1 && results[map[h]].city!= buf) {
      h = (h + 1) & (4095);
    }
    int c = map[h];

    if (c < 0) {
      Result r;
      r.city = buf;
      r.sum = measurement;
      r.max = measurement;
      r.min = measurement;
      r.count = 1;
      results.push_back(r);
      map[h] = results.size() - 1;
    } else {
      results[c].sum += measurement;
      results[c].count += 1;
      if (results[c].min > measurement) {
        results[c].min = measurement;
      }
      if (results[c].max < measurement) {
        results[c].max = measurement;
      }
    }
  }

  sort(results.begin(), results.end(), cmp);

  for (size_t i = 0; i < results.size(); i++) {
    cout << results[i].city << "=" << fixed << setprecision(1) << results[i].min << "/" << results[i].sum / results[i].count << "/" << results[i].max;
    if (i < results.size() - 1) {
      cout << ", ";
    }
  }
  cout << endl;

  fh.close();
  return 0;
}