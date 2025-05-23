#include "timer.h"
#include "game.h"

// Static member definition
std::vector<Timer::TimerEntry> Timer::timers;

void Timer::setTimeout(Callback callback, int delay_ms) {
    TimerEntry entry;
    entry.callback = callback;
    entry.trigger_time = game_time + (delay_ms / 1000.0f);
    entry.active = true;
    
    timers.push_back(entry);
}

void Timer::update(float current_time) {
    // Process all timers
    for (auto& timer : timers) {
        if (timer.active && current_time >= timer.trigger_time) {
            timer.callback();
            timer.active = false;
        }
    }
    
    // Remove inactive timers
    timers.erase(
        std::remove_if(timers.begin(), timers.end(),
            [](const TimerEntry& t) { return !t.active; }),
        timers.end()
    );
}

void Timer::clear() {
    timers.clear();
}