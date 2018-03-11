#include <iostream>
#include <cmath>

bool is_prime(const long number)
{
    if(number == 1)
        return false;
    if(number == 2)
        return true;
    if(number % 2 == 0)
        return false;
    auto sq = static_cast<long>(sqrt(number));
    for(long i = 2; i <= sq; i++){
        if(number % i == 0) {
            return false;
        }
    }
    return true;
}

void execute_with_parallel(bool parallel = false)
{
    const long BIG_NUMBER = 99999;
    const int EXECUTE_NUMBER = 4;

    for(long i = 0; i < EXECUTE_NUMBER; i++)
    {
        auto start = std::chrono::high_resolution_clock::now();

        if(parallel)
        {
            #pragma omp parallel for
            for(long j = 0; j < BIG_NUMBER; j++)
                is_prime(j);
        } else {
            for(long j = 0; j < BIG_NUMBER; j++)
                is_prime(j);
        }

        auto finish = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> elapsed = finish - start;
        const int minutes = static_cast<int>(elapsed.count()) / 60;
        const int seconds = static_cast<int>(elapsed.count()) % 60;
        const int milliseconds = static_cast<int>(
                                    (elapsed.count() - static_cast<int>(elapsed.count())) * 100
                                );
        std::cout << (parallel ? "Parallel: " : "Without parallel: ")
                  << minutes << " minutes "
                  << seconds << " seconds "
                  << milliseconds << " miliseconds"
                  << std::endl;
    }
}

int main() {
    execute_with_parallel(false);
    execute_with_parallel(true);
    return EXIT_SUCCESS;
}