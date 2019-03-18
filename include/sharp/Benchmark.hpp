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
		static void interrupt() { exit = true; }
		static bool isInterrupt() { return exit; }
		static int id() { return Benchmark::idv; }
		static void setId(int vid) { Benchmark::idv = vid; }
		static void useOrdering(unsigned o) { Benchmark::ord = o; }
		static unsigned isOrdering() { return Benchmark::ord; }

		static std::string& folder() { return Benchmark::fold; }
		static void setFolder(const std::string& flder) { Benchmark::fold = flder; }
	private:
		Benchmark();
		static bool exit;
		static std::mutex lock;
		static int idv;
		static unsigned ord;
		static std::string fold;

	};

} // namespace sharp

#endif // SHARP_SHARP_BENCHMARK_H_
