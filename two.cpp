//
// Created by Mateusz Kubaszek on 11/03/2018.
//

#include <iostream>
#include <omp.h>
#include <chrono>
#include <functional>

class Integral {
private:
    double _a{};
    double _b{};
    double _c{};
    double _d{}; // Step size
    double _n{}; // Steps
    double _range_begin{};
    double _range_end{};

    double f(double x) {
        return _a * (x * x) + _b * x + _c;
    }

public:
    Integral(double a, double b, double c, double range_begin, double range_end, double d) {};

    Integral() = default;

    void set_func(double a, double b, double c) {
        _a = a;
        _b = b;
        _c = c;
    }

    void set_range(double range_begin, double range_end) {
        _range_begin = range_begin;
        _range_end = range_end;
    }

    void set_integral_step(double d) {
        _d = d;
        _n = (_range_end - _range_begin) / _d;
    }

    void set_steps(double n) {
        _n = n;
        _d = (_range_end - _range_begin) / _n;
    }

    double count_by_quadratic() {
        double result = 0.0;
        for(long i = 1; i <= _n; i++) {
            result += f(_range_begin + i*_d) * _d;
        }
        return result;
    }

    double count_by_simpson() { return 1.0; }
};


class Stopwatch {
private:
    std::chrono::high_resolution_clock::time_point _begin{};
    std::chrono::high_resolution_clock::time_point _end{};
    std::chrono::duration<double> _elapsed{};

public:
    void start() {
        _begin = std::chrono::high_resolution_clock::now();
    }

    void stop() {
        _end = std::chrono::high_resolution_clock::now();
        _elapsed = _end - _begin;
    }

    void count_parallel(const std::function<void ()> &foo, long count = 1) {
        start();
        #pragma omp paraller for
        for(long i = 0; i < count; i++) {
            foo();
        }
        stop();
    }

    std::chrono::duration<double> result() {
        return _elapsed;
    }

    void print_result() {
        print_result(_elapsed.count());
    }

    void print_result(double elapsed) {
        const int minutes = static_cast<int>(elapsed) / 60;
        const int seconds = static_cast<int>(elapsed) % 60;
        const auto milliseconds =
                (elapsed - static_cast<int>(elapsed));

        std::cout << minutes << " minutes "
                  << seconds << " seconds "
                  << milliseconds << " milliseconds"
                  << std::endl;
    }

    void compare(Stopwatch& stopwatch) {
        if(result().count() > stopwatch.result().count()) {
            std::cout << "Is slower." << std::endl;
            print_result(result().count() - stopwatch.result().count());
        }
        else if(result().count()  == stopwatch.result().count()) {
            std::cout << "Is this same, value is: " << std::endl;
            print_result(result().count());
        }
        else {
            std::cout << "Is faster."  << std::endl;
            print_result(stopwatch.result().count() - result().count());
        }
    }

    void print_progressbar() {
    }
};

int main(const int argc, const char *argv[]) {
    Integral integral;
    Stopwatch stopwatchQ;
    Stopwatch stopwatchS;
    integral.set_func(-1, -1, 10);
    integral.set_range( -2, 1);
    integral.set_steps(3);

    stopwatchQ.count_parallel([&]{ integral.count_by_quadratic(); }, 9999);
    stopwatchQ.print_result();
    stopwatchS.count_parallel([&]{ integral.count_by_quadratic(); }, 9999999);
    stopwatchS.print_result();
    stopwatchQ.compare(stopwatchS);

    return EXIT_SUCCESS;
}