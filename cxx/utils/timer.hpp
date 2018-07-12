#pragma once
/**
   Measure the duration of code fragments.
   Similar to the Python variant in meta.py.
*/
#include <chrono>
#include <cstdio>
#include <cstring>      // std::strlen


inline bool
endsnewline(const char *s)
{
    const auto n = std::strlen(s);
    if (n < 1)
        return false;
    return s[n-1] == '\n';
}


class Timer
{
public:
    Timer() {}

    Timer(const char *msg,
          int indent = 20,
          const char *fmt = "%8.3fms\n") {
        start(msg, indent, fmt);
    }

    void start(const char *msg,
               int indent = 20,
               const char *fmt = "%8.3fms\n") {
        this->fmt = fmt;
        this->indent = indent;
        this->newline = endsnewline(msg);
        if (!_Timer_disable()) {
            if (newline) {
                fprintf(Timer::stream(), "%s", msg);
            } else {
                fprintf(Timer::stream(), "%-*s", indent, msg);
            }
            fflush(Timer::stream());
            time0 = std::chrono::high_resolution_clock::now();
        }
        running = true;
    }

    static Timer& startit(const char *msg,
                          int indent = 20,
                          const char *fmt = "%8.3fms\n") {
        Timer &d = _default();
        d.start(msg, indent, fmt);
        return d;
    }

    static Timer& stopit(Timer &d = _default()) {
        d.stop();
        return d;
    }

    void stop() {
        if (!_Timer_disable() && running) {
            auto time1 = std::chrono::high_resolution_clock::now();
            auto sec = std::chrono::duration_cast<std::chrono::duration<double>>
                (time1 - time0);
            if (newline)
                fprintf(Timer::stream(), "%-*s", indent, " --> ");
            fprintf(Timer::stream(), fmt, sec.count() * 1000.0);
            fflush(Timer::stream());
        }
        running = false;
    }

    ~Timer() { stop(); }

    static FILE*& stream() {
        static FILE *out = stdout;
        return out;
    }

    static int log(const char *format) {
        if (!_Timer_disable()) {
            return fprintf(Timer::stream(), "%s", format);
        }
        return 0;
    }

    template<typename... Args>
    static int log(const char *format, Args... args) {
        if (!_Timer_disable()) {
            return fprintf(Timer::stream(), format, args...);
        }
        return 0;
    }

private:
    // https://stackoverflow.com/a/11711082
    static bool& _Timer_disable() {
        static bool _impl = false;
        return _impl;
    }

    inline void static set_disable(bool disable) {
        _Timer_disable() = disable;
    }

    inline bool static get_disable() {
        return _Timer_disable();
    }

    static Timer& _default() {
        static Timer t;
        return t;
    }

    // FILE *out;
    const char *fmt = "";
    int indent = 20;
    bool newline = false;
    bool running = false;
    std::chrono::high_resolution_clock::time_point time0;
    friend struct TimerQuiet;
};


/** As long as this object exists, shut off the Timer */
struct TimerQuiet
{
    bool old_state;

    TimerQuiet(bool verbose = false) : old_state(Timer::get_disable()) {
        Timer::set_disable(!verbose);
    }

    ~TimerQuiet() {
        if (Timer::get_disable() && Timer::_default().running) {
            Timer::stopit();
            // std::cout << "stopit" << std::endl;
        }
        Timer::set_disable(old_state);
    }
};
