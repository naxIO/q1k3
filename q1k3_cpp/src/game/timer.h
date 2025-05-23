#ifndef TIMER_H
#define TIMER_H

#include <functional>
#include <vector>
#include <chrono>

// Timer system to replace JavaScript setTimeout
class Timer {
public:
    using Callback = std::function<void()>;
    
private:
    struct TimerEntry {
        Callback callback;
        float trigger_time;
        bool active;
    };
    
    static std::vector<TimerEntry> timers;
    
public:
    // Schedule a callback after delay_ms milliseconds
    static void setTimeout(Callback callback, int delay_ms);
    
    // Update all timers (called each frame)
    static void update(float current_time);
    
    // Clear all timers
    static void clear();
};

// Global helper function matching JavaScript API
inline void setTimeout(std::function<void()> callback, int delay_ms) {
    Timer::setTimeout(callback, delay_ms);
}

#endif // TIMER_H