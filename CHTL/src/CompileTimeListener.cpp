#include "../CompileTimeListener.h"
#include <iostream>
#include <csignal>

namespace CHTL {

CompileTimeListener::CompileTimeListener() : m_Stopped(false) {}

CompileTimeListener::~CompileTimeListener() {
    // Ensure the thread is joined upon destruction
    if (m_TimerThread.joinable()) {
        m_TimerThread.join();
    }
}

void CompileTimeListener::Start(std::chrono::milliseconds timeout) {
    m_TimerThread = std::thread([this, timeout]() {
        // Wait for the specified duration
        std::this_thread::sleep_for(timeout);

        // If Stop() has not been called by the main thread, terminate.
        if (!m_Stopped.load()) {
            std::cerr << "\nError: Compilation timed out after " << timeout.count() << "ms. Terminating to prevent infinite loop." << std::endl;
            std::terminate(); // A more graceful exit might use signals
        }
    });
}

void CompileTimeListener::Stop() {
    m_Stopped.store(true);
    // Detach or join the thread. Joining is safer.
    if (m_TimerThread.joinable()) {
        m_TimerThread.join();
    }
}

}