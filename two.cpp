//
// Created by Mateusz Kubaszek on 11/03/2018.
//

#include <iostream>
#include <omp.h>
#include <chrono>
#include <functional>
#include <thread>
#include <iomanip>

class Progress_bar {
private:
    static const auto overhead = sizeof " [100%]";

    std::ostream &os;
    const std::size_t bar_width;
    std::string message;
    const std::string full_bar;

public:
    Progress_bar(std::ostream &os, std::size_t line_width,
                 std::string message_, const char symbol = '.')
            : os{os},
              bar_width{line_width - overhead},
              message{std::move(message_)},
              full_bar{std::string(bar_width, symbol) + std::string(bar_width, ' ')} {
        if (message.size() + 1 >= bar_width || message.find('\n') != message.npos) {
            os << message << '\n';
            message.clear();
        } else {
            message += ' ';
        }
        write(0.0);
    }

    Progress_bar(const Progress_bar &) = delete;

    Progress_bar &operator=(const Progress_bar &) = delete;

    ~Progress_bar() {
        write(1.0);
        os << '\n';
    }

    void write(double fraction) {
        if (fraction < 0)
            fraction = 0;
        else if (fraction > 1)
            fraction = 1;

        auto width = bar_width - message.size();
        auto offset = bar_width - static_cast<unsigned>(width * fraction);

        os << '\r' << message;
        os.write(full_bar.data() + offset, width);
        os << " [" << std::setw(3) << static_cast<int>(100 * fraction) << "%] " << std::flush;
    }
};


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

    Integral &set_func(double a, double b, double c) {
        _a = a;
        _b = b;
        _c = c;
        return *this;
    }

    Integral &set_range(double range_begin, double range_end) {
        _range_begin = range_begin;
        _range_end = range_end;
        return *this;
    }

    Integral &set_integral_step(double d) {
        _d = d;
        _n = (_range_end - _range_begin) / _d;
        return *this;
    }

    Integral &set_steps(double n) {
        _n = n;
        _d = (_range_end - _range_begin) / _n;
        return *this;
    }

    double count_by_quadratic() {
        double result = 0.0;
        for (long i = 1; i <= _n; i++) {
            result += f(_range_begin + i * _d) * _d;
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
    long _counter{};

public:
    void start() {
        _begin = std::chrono::high_resolution_clock::now();
    }

    void stop() {
        _end = std::chrono::high_resolution_clock::now();
        _elapsed = _end - _begin;
    }

    Stopwatch &count_parallel(const std::function<void()> &foo, long count = 1) {
        start();
        Progress_bar progress{std::clog, 70u, "Working"};

        for (auto j = 0.0; j <= 100; j += 3.65) {
            progress.write(j / 100.0);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            #pragma omp paraller for
            for (long i = 0; i < count; i++) {
                foo();
            }
        }
        stop();
        return *this;
    }

    std::chrono::duration<double> result() {
        return _elapsed;
    }

    Stopwatch &print_result() {
        return print_result(_elapsed.count());
    }

    Stopwatch &print_result(double elapsed) {
        const int minutes = static_cast<int>(elapsed) / 60;
        const int seconds = static_cast<int>(elapsed) % 60;
        const auto milliseconds =
                (elapsed - static_cast<int>(elapsed));

        std::cout << minutes << " minutes "
                  << seconds << " seconds "
                  << milliseconds << " milliseconds"
                  << std::endl;
        return *this;
    }

    Stopwatch &compare(Stopwatch &stopwatch) {
        if (result().count() > stopwatch.result().count()) {
            std::cout << "Is slower." << std::endl;
            print_result(result().count() - stopwatch.result().count());
        } else if (result().count() == stopwatch.result().count()) {
            std::cout << "Is this same, value is: " << std::endl;
            print_result(result().count());
        } else {
            std::cout << "Is faster." << std::endl;
            print_result(stopwatch.result().count() - result().count());
        }
        return *this;
    }
};

int main(const int argc, const char *argv[]) {
    Integral integral;
    Stopwatch stopwatchQ;
    Stopwatch stopwatchS;
    integral.set_func(-1, -1, 10)
            .set_range(-2, 1)
            .set_steps(3);

    stopwatchQ.count_parallel([&] { integral.count_by_quadratic(); }, 9999);
    stopwatchS.count_parallel([&] { integral.count_by_quadratic(); }, 999999);
    stopwatchQ.compare(stopwatchS);
    return EXIT_SUCCESS;
}