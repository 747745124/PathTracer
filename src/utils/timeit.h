#pragma once
#include <chrono>
#include <functional>
#include <iostream>

template <class>
struct Timeit;

template <class Fn, class... Args>
struct Timeit<Fn(Args...)>
{
public:
    Timeit(std::function<Fn(Args...)> func) : f_(func) {}
    std::function<Fn(Args...)> f_;
    void operator()(Args... args)
    {
        std::chrono::time_point<std::chrono::system_clock> start, end;
        std::chrono::duration<double> duration;

        start = std::chrono::system_clock::now();
        f_(args...);
        end = std::chrono::system_clock::now();
        duration = end - start;
        std::cout << duration.count() << " seconds" << std::endl;
    }
};

template <class Fn, class... Args>
Timeit<Fn(Args...)> make_decorator(Fn (*f)(Args...))
{
    return Timeit<Fn(Args...)>(std::function<Fn(Args...)>(f));
}



// class Timer {
// private:
// 	LARGE_INTEGER freq;
// 	LARGE_INTEGER t_start, t_end;
// 	LARGE_INTEGER total_time;
// public:
// 	Timer() {
// 		 QueryPerformanceFrequency(&freq);
// 	}
	
// 	void resetTimer(void) {
// 		total_time.QuadPart = 0;
// 	}

// 	void unpauseTimer(void) {
// 		QueryPerformanceCounter(&t_start);
// 	}

// 	void pauseTimer(void) {
// 		QueryPerformanceCounter(&t_end);
// 		total_time.QuadPart += (t_end.QuadPart - t_start.QuadPart);
// 	}

// 	void startTimer(void) {
// 		QueryPerformanceCounter(&t_start);
// 	}

// 	void stopTimer(void) {
// 		QueryPerformanceCounter(&t_end);
// 		total_time.QuadPart = (t_end.QuadPart - t_start.QuadPart);
// 	}

// 	void printTime(void) {
// 		fprintf(stderr,"%lf\n",((double) total_time.QuadPart) /((double) freq.QuadPart));
// 		fflush(stderr);
// 	}

// 	double getTime(void) const{
// 		return ((double) total_time.QuadPart) /((double) freq.QuadPart);
// 	}

// };
