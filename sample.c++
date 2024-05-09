#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

struct Result {
    std::string city;
    int count;
    double sum, min, max;
};

static int getCity(const std::string& city, const std::vector<Result>& results) {
    for (int i = 0; i < results.size(); i++) {
        if (results[i].city == city) {
            return i;
        }
    }
    return -1;
}

static bool cmp(const Result& a, const Result& b) {
    return a.city < b.city;
}

int main(int argc, char* argv[]) {
    std::string filename = "measurements.txt";
    if (argc > 1) {
        filename = argv[1];
    }

    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error opening file" << std::endl;
        return 1;
    }

    std::vector<Result> results;
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find(';');
        std::string city = line.substr(0, pos);
        double measurement = std::stod(line.substr(pos + 1));

        int c = getCity(city, results);
        if (c < 0) {
            results.push_back({city, 1, measurement, measurement, measurement});
        } else {
            results[c].sum += measurement;
            results[c].count += 1;
            results[c].min = std::min(results[c].min, measurement);
            results[c].max = std::max(results[c].max, measurement);
        }
    }

    std::sort(results.begin(), results.end(), cmp);

    for (const auto& result : results) {
        std::cout << result.city << "=" << result.min << "/" << result.sum / result.count << "/" << result.max << ", ";
    }
    std::cout << std::endl;

    file.close();
    return 0;
}