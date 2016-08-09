#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sharp/Benchmark.hpp>

#include <deque>

//FIXME: use config.h info here, build check into autoconf
#include <sys/times.h>

//TODO:
//#ifdef HAVE_UNISTD_H //FIXME: what if not def'd?
#include <unistd.h>
//#endif

namespace sharp {
    bool Benchmark::exit = false;
    std::shared_ptr<ISharpOutput> Benchmark::output_;

//    ISharpOutput *Benchmark::output_;

    std::mutex Benchmark::lock;

    using std::deque;
    using std::string;
    using std::endl;
    using std::ios;

    namespace {
        deque<clock_t> wallClock_;
        deque<struct tms> cpuClock_;
        deque<string> names_;

    } // namespace

    void Benchmark::registerTimestamp(const std::string &name) {
        lock.lock();
        struct tms cpu;
        clock_t wall = times(&cpu);

        wallClock_.push_front(wall);
        cpuClock_.push_front(cpu);
        names_.push_front(name);
        lock.unlock();
    }

    /*void Benchmark::registerTimestamp(const std::string &name, const std::string &baseTimestamp)
    {
        struct tms cpu;
        clock_t wall = times(&cpu);

        wallClock_.push_front(wall);
        cpuClock_.push_front(cpu);
        names_.push_front(name);
    }*/


    void Benchmark::printBenchmarks(std::ostream &out, bool csv) {
        lock.lock();
        if (names_.empty()) return;

        long tcksec = 0;
        if ((tcksec = sysconf(_SC_CLK_TCK)) < 0) return;

        out.setf(ios::fixed, ios::floatfield);
        out.precision(2);

        clock_t lastWall = wallClock_.back();
        struct tms lastCpu = cpuClock_.back();

        if (!csv)
            out << "0.00s (usr),\t0.00s (sys),\t0.00s (wall) - "
                << names_.back() << endl;
        else
            out << "usr,sys,cpu,wall,description" << endl
                << "0.00,0.00,0.00,0.00,"
                << names_.back() << endl;


        names_.pop_back();
        wallClock_.pop_back();
        cpuClock_.pop_back();

        while (!names_.empty()) {
            clock_t wall = wallClock_.back();
            struct tms cpu = cpuClock_.back();

            if (!csv)
                out << ((cpu.tms_utime - lastCpu.tms_utime) / (double) tcksec)
                    << "s (usr),\t"
                    << ((cpu.tms_stime - lastCpu.tms_stime) / (double) tcksec)
                    << "s (sys),\t"
                    << ((wall - lastWall) / (double) tcksec)
                    << "s (wall) - "
                    << names_.back() << endl;
            else
                out << ((cpu.tms_utime - lastCpu.tms_utime) / (double) tcksec)
                    << ","
                    << ((cpu.tms_stime - lastCpu.tms_stime) / (double) tcksec)
                    << ","
                    << ((cpu.tms_utime - lastCpu.tms_utime) / (double) tcksec) +
                       ((cpu.tms_stime - lastCpu.tms_stime) / (double) tcksec)
                    << ","
                    << ((wall - lastWall) / (double) tcksec)
                    << ","
                    << names_.back() << endl;

            lastWall = wall;
            lastCpu = cpu;

            names_.pop_back();
            wallClock_.pop_back();
            cpuClock_.pop_back();
        }
        lock.unlock();
    }

    std::shared_ptr<ISharpOutput> Benchmark::output() {
        if (!output_){
            throw "An output handler needs to be registered using Benchmark::registerOutput(std::shared_ptr<ISharpOutput>).";
        }
        return output_;
    }


    /*void Benchmark::registerTimestamp(const std::string &name, const std::string &baseTimestamp)
{
    struct tms cpu;
    clock_t wall = times(&cpu);

    wallClock_.push_front(wall);
    cpuClock_.push_front(cpu);
    names_.push_front(name);
}*/

} // namespace sharp
