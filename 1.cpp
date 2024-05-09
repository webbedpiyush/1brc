#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <string.h>

struct Result {
  std::string city;
  int count;
  double sum, min, max;
};

int getCity(const std::string& city, std::vector<Result>& results) {
  for (size_t i = 0; i < results.size(); i++) {
    if (results[i].city == city) {
      return i;
    }
  }
  return -1;
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
  std::string line;
  while (std::getline(fh, line)) {
    size_t pos = line.find(';');
    if (pos == std::string::npos) {
      std::cerr << "Skipping line: " << line << std::endl;
      continue;
    }

    std::string city = line.substr(0, pos);
    std::string measurementStr = line.substr(pos + 1);

    if (city.empty() || measurementStr.empty()) {
      std::cerr << "Skipping line: " << line << std::endl;
      continue;
    }

    try {
      double measurement = std::stod(measurementStr);
      int c = getCity(city, results);
      if (c < 0) {
        Result result;
        result.city = city;
        result.sum = measurement;
        result.max = measurement;
        result.min = measurement;
        result.count = 1;
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
      std::cerr << "Skipping line: " << line << std::endl;
    }
  }

  std::sort(results.begin(), results.end(), cmp);

  for (size_t i = 0; i < results.size(); i++) {
    std::cout << results[i].city << "=%.1f/%.1f/%.1f" << (i < results.size() - 1? ", " : "") << std::endl;
  }

  fh.close();
  return 0;
}