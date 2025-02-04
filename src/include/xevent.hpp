#ifndef X_EVENT_SYSTEM_H
#define X_EVENT_SYSTEM_H

#include <functional>
#include <unordered_map>
#include <vector>
#include <queue>
#include <memory>
#include <mutex>
#include <string>
#include <algorithm>


// Base Event Class
class XEvent {
public:
    virtual ~XEvent() = default;
    virtual std::string GetEventKey() const = 0; // Unique string key for the event type
    void StopPropagation() { propagationStopped = true; }
    bool IsPropagationStopped() const { return propagationStopped; }

private:
    bool propagationStopped = false;
};

using XEventHandler = std::function<void(const XEvent&)>;

class XEventManager {
public:
    static XEventManager& Instance() {
        static XEventManager instance;
        return instance;
    }

    ~XEventManager() = default;

    // Prevent copying
    XEventManager(const XEventManager&) = delete;
    XEventManager& operator=(const XEventManager&) = delete;

    // Prevent moving
    XEventManager(XEventManager&&) = delete;
    XEventManager& operator=(XEventManager&&) = delete;

    void Subscribe(const std::string& eventKey, const XEventHandler& handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        subscribers_[eventKey].emplace_back(handler);
    }

    void Unsubscribe(const std::string& eventKey, const XEventHandler& handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto& handlers = subscribers_[eventKey];
        handlers.erase(std::remove_if(handlers.begin(), handlers.end(),
            [&handler](const XEventHandler& registeredHandler) {
                return handler.target<void(const XEvent&)>() ==
                       registeredHandler.target<void(const XEvent&)>();
            }),
            handlers.end());
    }

    void TriggerEvent(const XEvent& event) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = subscribers_.find(event.GetEventKey());
        if (it != subscribers_.end()) {
            for (auto& handler : it->second) {
                handler(event);
                if (event.IsPropagationStopped()) break;
            }
        }
    }

    void QueueEvent(std::unique_ptr<XEvent>&& event) {
        std::lock_guard<std::mutex> lock(queueMutex_);
        eventQueue_.emplace(std::move(event));
    }

    void DispatchEvents() {
        std::lock_guard<std::mutex> lock(queueMutex_);
        while (!eventQueue_.empty()) {
            auto event = std::move(eventQueue_.front());
            eventQueue_.pop();
            TriggerEvent(*event);
        }
    }

private:
    XEventManager() = default;

    std::unordered_map<std::string, std::vector<XEventHandler>> subscribers_;
    std::queue<std::unique_ptr<XEvent>> eventQueue_;
    mutable std::mutex mutex_;
    mutable std::mutex queueMutex_;
};

#endif // X_EVENT_SYSTEM_H
