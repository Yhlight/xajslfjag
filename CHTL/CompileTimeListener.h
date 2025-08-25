#pragma once

#include <chrono>
#include <thread>
#include <atomic>

namespace CHTL {

    class CompileTimeListener {
    public:
        CompileTimeListener();
        ~CompileTimeListener();

        // Starts the watchdog timer in a separate thread.
        void Start(std::chrono::milliseconds timeout);

        // Stops the watchdog timer, preventing termination.
        void Stop();

    private:
        std::atomic<bool> m_Stopped;
        std::thread m_TimerThread;
    };

}
