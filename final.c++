#include <fcntl.h>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include <sys/mman.h>
#include <sys/stat.h>
#include <cstring> // for memchr, memcpy
#include <unistd.h>
#include <limits.h>

using namespace std;

// constexpr means constant expression they are dealt in compile time
constexpr int cityLength = 100;
constexpr int HASHMAP_CAPACITY = 16384;

struct info
{
    unsigned int count = 0;
    int min = INT_MAX;
    int max = INT_MIN;
    long sum = 0;
    char key[cityLength];
};

struct Result
{
    unordered_map<string, info> groups;
};

// static for internal linkage , inline for better efficiency
static inline const char *parse_number(int *dest, const char *s)
{
    // to check for negative num
    int mod = 1;
    if (*s == '-')
    {
        mod = -1;
        s++;
    }

    // for floating point
    if (s[1] == '.')
    {
        *dest = ((s[0] * 10) + s[2] - ('0' * 11)) * mod;
        return s + 4;
    }

    // for integer
    *dest = (s[0] * 100 + s[1] * 10 + s[3] - ('0' * 111)) * mod;
    return s + 5;
}

static inline int min(int a, int b) { return min(a, b); }
static inline int max(int a, int b) { return max(a, b); }

static void process_chunk(const char *data, size_t chunk_start, size_t chunk_end, Result &result, std::mutex &mtx)
{
    const char *s = chunk_start > 0 ? (char *)memchr(&data[chunk_start], '\n', chunk_end - chunk_start) + 1 : &data[chunk_start];
    const char *end = (char *)memchr(&data[chunk_end], '\n', chunk_end - chunk_start) + 1;

    while (s != end)
    {
        const char *linestart = s;

        unsigned int len = 0;
        while (s[len] != ';')
        {
            len++;
        }

        int temperature;
        s = parse_number(&temperature, linestart + len + 1);

        std::string key(linestart, len);
        std::lock_guard<std::mutex> lock(mtx);
        auto &group = result.groups[key];
        group.count++;
        group.min = min(group.min, temperature);
        group.max = max(group.max, temperature);
        group.sum += temperature;
        memcpy(group.key, key.c_str(), len);
        group.key[len] = '\0';
    }
}

int main(int argc, char **argv)
{
    std::string file = "measurements.txt";
    if (argc > 1)
    {
        file = argv[1];
    }

    int fd = open(file.c_str(), O_RDONLY);
    if (!fd)
    {
        std::cerr << "Error opening file" << std::endl;
        return 1;
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1)
    {
        std::cerr << "Error getting file size" << std::endl;
        return 1;
    }

    size_t sz = (size_t)sb.st_size;
    char *data = (char *)mmap(NULL, sz, PROT_READ, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED)
    {
        std::cerr << "Error mmapping file" << std::endl;
        return 1;
    }

    constexpr unsigned int NTHREADS = 10;
    size_t chunk_size = (sz / (2 * NTHREADS));
    size_t chunk_count = (sz / chunk_size - 1) + 1;

    Result result;
    std::mutex mtx;
    std::vector<std::thread> threads;

    for (unsigned int i = 0; i < chunk_count; i++)
    {
        size_t chunk_start = i * chunk_size;
        size_t chunk_end = std::min(chunk_start + chunk_size, sz);
        threads.emplace_back(process_chunk, data, chunk_start, chunk_end, std::ref(result), std::ref(mtx));
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    for (const auto &[key, group] : result.groups)
    {
        std::cout << key << "=" << static_cast<double>(group.min) / 10.0 << "/" << static_cast<double>(group.sum) / (group.count * 10.0) << "/" << static_cast<double>(group.max) / 10.0 << ", ";
    }
    std::cout << std::endl;

    munmap((void *)data, sz);
    close(fd);

    return 0;
}