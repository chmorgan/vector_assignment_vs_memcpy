
#include <stdint.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <chrono>


class ITest
{
public:
    virtual void Run(unsigned int iterations) = 0;
};

class VectorAssignmentTest
{
public:
    VectorAssignmentTest(unsigned int size)
    {
        source.resize(size, 0);
    }

    void Run(unsigned int iterations)
    {
        while(iterations)
        {
            destination = source;
            iterations--;
        }
    }
private:
    std::vector<uint8_t> source;
    std::vector<uint8_t> destination; 
};

class MemcpyTest
{
public:
    MemcpyTest(unsigned int size) : size(size)
    {
        source = (uint8_t*)malloc(size);
        destination = (uint8_t*)malloc(size);
    }

    ~MemcpyTest()
    {
        free(source);
        free(destination);
    }

    void Run(unsigned int iterations)
    {
        while(iterations)
        {
            memcpy(source, destination, size);
            iterations--;
        }  
    }

private:
    unsigned int size;
    uint8_t* source;
    uint8_t* destination;
};

struct result
{
    unsigned int buffer_length;
    unsigned int iterations;
    std::chrono::microseconds duration;
};

template<class T>
std::vector<result>
    RunTest(unsigned int starting_power, unsigned int ending_power,
            unsigned int iterations)
{
    std::vector<result> results;

    for(auto x = starting_power; x < ending_power; x++)
    {
        const unsigned int length = 2 << x;
        T t(length);
        auto start = std::chrono::system_clock::now();
        t.Run(iterations);
        auto duration = std::chrono::duration_cast<std::chrono::microseconds> 
                            (std::chrono::system_clock::now() - start);
        results.push_back(result { .buffer_length = length, .iterations = iterations, .duration = duration });
    }

    return results;
}

int main()
{
    const auto starting_power = 1;
    const auto ending_power = 16;
    const auto iterations = 100000;

    auto vat = RunTest<VectorAssignmentTest>(starting_power, ending_power, iterations);
    auto mct = RunTest<MemcpyTest>(starting_power, ending_power, iterations);

    std::cout << "Buffer length, Vector assignment (us), Memcpy (us)" << std::endl;
    for(auto x = 0; x < mct.size(); x++)
    {
        std::cout << mct[x].buffer_length << "," << vat[x].duration.count() << ","
            << mct[x].duration.count() << std::endl;
    }

    return 0;
}