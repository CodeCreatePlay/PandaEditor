#include <iostream> 
#include "demon.h" 
#include "xevent.hpp"


// Reference to the global event manager instance
auto& eventManager = XEventManager::Instance();

// Define a custom event class inheriting from XEvent
class TestEvent : public XEvent {
public:
    TestEvent(unsigned int value_a, unsigned int value_b)
        : value_a(value_a), value_b(value_b) {}

    // Unique identifier for this event type
    std::type_index GetEventType() const override { return typeid(TestEvent); }

    unsigned int value_a { 0 };  // Event payload: value_a
    unsigned int value_b { 0 };  // Event payload: value_b
};

// Test class that subscribes to and handles events
class TestObj {
public:
    TestObj() {
        // Subscribe to TestEvent with a lambda handler
        resizeHandler_ = [this](const TestEvent& e) { OnEvent(e); };
        eventManager.Subscribe<TestEvent>(resizeHandler_);
        std::cout << "[TestObj] Subscribed to TestEvent.\n";
    }

    ~TestObj() {
        // Unsubscribe from TestEvent when the object is destroyed
        eventManager.Unsubscribe<TestEvent>(resizeHandler_);
        std::cout << "[TestObj] Unsubscribed from TestEvent.\n";
    }

    // Callback function to handle TestEvent
    void OnEvent(const TestEvent& e) {
        std::cout << "[TestObj] OnEvent: " 
                  << "value_a: " << e.value_a << "value_b: " << e.value_b << "\n";
    }

private:
    XEventHandler<TestEvent> resizeHandler_; // Handler for TestEvent
};

// Demonstrates the functionality of the event system
class EventSysDemo : public Demon {
public:
    EventSysDemo() {
        // Create a test object that subscribes to events
        TestObj testObj;

        // Trigger an immediate event
        TestEvent resizeEvent(1024, 768);
        eventManager.TriggerEvent(resizeEvent); // TestObj handles this instantly

        // Queue multiple events for later processing
        eventManager.QueueEvent(std::make_unique<TestEvent>(800, 600));
        eventManager.QueueEvent(std::make_unique<TestEvent>(1280, 720));

        // Dispatch all queued events
        eventManager.DispatchEvents(); // TestObj handles the queued events here
    }
};

int main(int argc, char* argv[]) {
    EventSysDemo demo;
    demo.start();
    return 0;
}
