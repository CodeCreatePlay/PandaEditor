#ifndef X_EVENT_SYSTEM_H
#define X_EVENT_SYSTEM_H

#include <functional>
#include <unordered_map>
#include <vector>
#include <queue>
#include <memory>
#include <mutex>
#include <typeindex>
#include <typeinfo>

// Base Event Class
class XEvent {
public:
    virtual ~XEvent() = default;
    virtual std::type_index GetEventType() const = 0;
    void StopPropagation() { propagationStopped = true; }
    bool IsPropagationStopped() const { return propagationStopped; }

private:
    bool propagationStopped = false;
};

template <typename EventType>
using XEventHandler = std::function<void(const EventType&)>;

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

    template <typename EventType>
    void Subscribe(const XEventHandler<EventType>& handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto& handlers = subscribers_[typeid(EventType)];
        handlers.emplace_back(std::make_unique<XEventHandlerWrapper<EventType>>(handler));
    }

    template <typename EventType>
    void Unsubscribe(const XEventHandler<EventType>& handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto& handlers = subscribers_[typeid(EventType)];
        auto it = std::remove_if(handlers.begin(), handlers.end(),
            [&handler](const std::unique_ptr<XEventHandlerWrapperBase>& wrapper) {
                return wrapper->GetHandlerType() == typeid(handler).name();
            });
        handlers.erase(it, handlers.end());
    }

    void TriggerEvent(const XEvent& event) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = subscribers_.find(event.GetEventType());
        if (it != subscribers_.end()) {
            for (auto& handler : it->second) {
                handler->Execute(event);
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

    class XEventHandlerWrapperBase {
    public:
        virtual ~XEventHandlerWrapperBase() = default;
        virtual void Execute(const XEvent& e) = 0;
        virtual std::string GetHandlerType() const = 0;
    };

    template <typename EventType>
    class XEventHandlerWrapper : public XEventHandlerWrapperBase {
    public:
        explicit XEventHandlerWrapper(const XEventHandler<EventType>& handler)
            : handler_(handler), handlerType_(typeid(handler).name()) {}

        void Execute(const XEvent& e) override {
            handler_(static_cast<const EventType&>(e));
        }

        std::string GetHandlerType() const override { return handlerType_; }

    private:
        XEventHandler<EventType> handler_;
        std::string handlerType_;
    };

    std::unordered_map<std::type_index, std::vector<std::unique_ptr<XEventHandlerWrapperBase>>> subscribers_;
    std::queue<std::unique_ptr<XEvent>> eventQueue_;
    mutable std::mutex mutex_;
    mutable std::mutex queueMutex_;
};

#endif // X_EVENT_SYSTEM_H
