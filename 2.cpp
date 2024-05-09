#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <string.h>

#define HCAP 4096

struct Result {
  std::string city;
  int count;
  double sum, min, max;
};

// hash returns a simple (but fast) hash for the first n bytes of data
static unsigned int hash(const unsigned char *data, int n) {
  unsigned int hash = 0;

  for (int i = 0; i < n; i++) {
    hash = (hash * 31) + data[i];
  }

  return hash;
}

bool cmp(const Result& a, const Result& b) {
  return a.city < b.city;
}

int main(int argc, char* argv[]) {
  const char* file = "measurements.txt";
  if (argc > 1) {
    file = argv[1];
  }

  std::ifstream fh(file);
  if (!fh) {
    std::cerr << "Error opening file: " << strerror(errno) << std::endl;
    return EXIT_FAILURE;
  }

  std::vector<Result> results;
  int map[HCAP];
  std::fill_n(map, HCAP, -1);

  std::string buf;
  while (std::getline(fh, buf, ';')) {
    if (buf.empty()) {
      std::cerr << "Skipping empty line" << std::endl;
      continue;
    }

    std::string measurementStr = buf.substr(buf.find(';') + 1);
    if (measurementStr.empty()) {
      std::cerr << "Skipping line without measurement: " << buf << std::endl;
      continue;
    }

    try {
      double measurement = std::stod(measurementStr);

      // find index of group by key through hash with linear probing
      int h = hash((unsigned char *)buf.c_str(), buf.size()) & (HCAP - 1);
      while (map[h]!= -1 && results[map[h]].city!= buf) {
        h = (h + 1) & (HCAP - 1);
      }
      int c = map[h];

      if (c < 0) {
        Result result;
        result.city = buf;
        result.sum = measurement;
        result.max = measurement;
        result.min = measurement;
        result.count = 1;
        map[h] = results.size();
        results.push_back(result);
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
    } catch (const std::invalid_argument& ia) {
      std::cerr << "Invalid argument: " << ia.what() << std::endl;
      std::cerr << "Skipping line: " << buf << std::endl;
    }
  }

  std::sort(results.begin(), results.end(), cmp);

  for (size_t i = 0; i < results.size(); i++) {
    std::cout << results[i].city << "=%.1f/%.1f/%.1f" << (i < results.size() - 1? ", " : "") << std::endl;
  }

  fh.close();
  return 0;
}