#ifndef SHARP_SHARP_BENCHMARK_H_
#define SHARP_SHARP_BENCHMARK_H_

#include <sharp/global>

#include <iostream>
#include <string>
#include <mutex>

namespace sharp
{
	class SHARP_API Benchmark
	{
	public:
		static void registerTimestamp(const std::string &name);
		static void printBenchmarks(std::ostream &out, bool csv);
		static void printBenchmarksJSON(std::ostream &out, bool csv);
	    static void interrupt() { exit = true; }
		static bool isInterrupt() { return exit; }
	private:
		Benchmark();
		static bool exit;
		static std::mutex lock;
	};

} // namespace sharp

#endif // SHARP_SHARP_BENCHMARK_H_
