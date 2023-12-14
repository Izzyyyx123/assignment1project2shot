#pragma once
#include <iostream>
#include <chrono>
#include <vector>
#include <fstream>
#include <cstdio>
#include <iomanip>
class Timer
{
protected:

	std::chrono::time_point<std::chrono::steady_clock>timerbegin, timerend;
public:

	void start()
	{
		timerbegin = std::chrono::steady_clock::now();
	}
	void stop() {
		timerend = std::chrono::steady_clock::now();
	}

	float get_elapsed_s() {
		std::chrono::duration<float>dif = timerend - timerbegin;
		return dif.count();
	}

	float get_elapsed_ms() {
		std::chrono::duration<float, std::milli>dif = timerend - timerbegin;
		return dif.count();
	}
};

class timer_average :public Timer {
public:

	int const NUM_LOOPS = 100;
	std::vector <std::chrono::duration<float, std::milli>> times;
	std::string filename = "Frame Times";
	bool hasprinted = false;

	timer_average() {
		times.reserve(NUM_LOOPS);
	}

	void start()
	{
		Timer::start();
	}
	void stop() {
		Timer::stop();
		if (times.size() < NUM_LOOPS) {
			times.push_back(timerend - timerbegin);
		}
	}

	bool get_are_all_times_collected() {
		return(times.size() == NUM_LOOPS);
	}
	float  get_average_time_s() {
		float running = 0;
		for (int i = 0; i < NUM_LOOPS; ++i)
			running += times[i].count();

		running /= NUM_LOOPS;

		running /= 1000.0f;

		return running;

	}

	float get_max_time() {

		float largestETime = 0;

		for (int i = 0; i < NUM_LOOPS; ++i) {
			if (largestETime < times[i].count())
				largestETime = times[i].count();
		}

		return largestETime;

	}

	float get_min_time() {

		float smallestETime = times[0].count();

		for (int i = 1; i < NUM_LOOPS; ++i) {
			if (smallestETime > times[i].count())
				smallestETime = times[i].count();
		}
		return smallestETime;

	}

	void print_to_file()
	{
		if (!get_are_all_times_collected())
		{
			std::cerr << "Cannot print to file. Not all times have been collected." << std::endl;
			return;
		}

		if (!hasprinted)
		{

			std::ofstream outfile(filename + ".csv");
			if (!outfile.is_open())
			{
				std::cerr << "Unable to open file for writing." << std::endl;
				return;
			}

			outfile << "frame,time(ms)" << std::endl;
			for (int i = 0; i < NUM_LOOPS; ++i)
			{
				outfile << i << "," << std::fixed << std::setprecision(2) << times[i].count() << std::endl;
			}

			outfile.close();
			std::cout << "MS written to" << filename << ".csv" << std::endl;
			hasprinted = true;
		}

	}
};