#ifndef SHARP_SHARP_BENCHMARK_H_
#define SHARP_SHARP_BENCHMARK_H_

#include <sharp/global>

#include <iostream>
#include <string>
#include <mutex>
#include <sharp/ISharpOutput.hpp>

namespace sharp
{
	class SHARP_API Benchmark
	{
	public:
		static void registerTimestamp(const std::string &name);
		static void printBenchmarks();
	    static void interrupt() { exit = true; }
	    static bool isInterrupt() { return exit; }
        static void registerOutput(std::shared_ptr<ISharpOutput> output) { output_ = output;};
        static std::shared_ptr<ISharpOutput> output();

	private:
		Benchmark();
		static bool exit;
        static std::mutex lock;
        static std::shared_ptr<ISharpOutput> output_;
    };

} // namespace sharp

#endif // SHARP_SHARP_BENCHMARK_H_
