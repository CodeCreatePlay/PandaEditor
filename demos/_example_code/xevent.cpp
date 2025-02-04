#include <iostream> 
#include <sstream>
#include "xevent.hpp"


// -------------------------------------------------------------------
// Demonstrates basic example usage of XEvent, a robust alternative to
// Panda3D's default event system, recommended for custom events.
// -------------------------------------------------------------------

/*
	Step 01: Define a custom event class (or a struct) inheriting from XEvent.
*/
class TestEvent : public XEvent {
public:
    TestEvent(unsigned int value_a, unsigned int value_b)
        : value_a(value_a), value_b(value_b) {}

    // Unique identifier for this event type
    std::string GetEventKey() const override { return "TestEvent"; }

    unsigned int value_a { 0 };  // Event payload: value_a
    unsigned int value_b { 0 };  // Event payload: value_b
};

/*
	Step 02: Create a Test class that subscribes to and handles events, TestEvent in this case.
*/
class TestObj {
public:
    TestObj() {
		
        // Subscribe to TestEvent with a lambda handler
        eventHandler_ = [this](const XEvent& e) { OnEvent(e); };
        XEventManager::Instance().Subscribe("TestEvent", eventHandler_);

        std::cout << "[TestObj] Subscribed to TestEvent.\n";
    }

    ~TestObj() {
		
        // Unsubscribe from TestEvent when the object is destroyed
        XEventManager::Instance().Unsubscribe("TestEvent", eventHandler_);

        std::cout << "\n[TestObj] Unsubscribed from TestEvent.\n";
    }

    // Callback function to handle TestEvent
    void OnEvent(const XEvent& e) {
        const auto& event = static_cast<const TestEvent&>(e);

        std::ostringstream oss;
        oss << "  [TestObj] OnEvent: "
            << "value_a: (" << event.value_a << ") "
            << "value_b: (" << event.value_b << ")";
        std::cout << oss.str() << "\n";
    }

private:
    XEventHandler eventHandler_; // Generic event handler
};


// ------------------------------------------- Main ------------------------------------------- //
int main(int argc, char* argv[]) {
    std::cout << "----XEvent demo----\n";

    auto& eventManager = XEventManager::Instance();

    // Create event
    TestEvent resizeEvent(1024, 768);

    // A TestClass that subscribes to handle 'TestEvent'
    TestObj testObj;

    // Trigger an event immediately
    std::cout << "\n Dispatching events!\n";
    eventManager.TriggerEvent(resizeEvent);

    // Queue multiple events for later processing
    eventManager.QueueEvent(std::make_unique<TestEvent>(800, 600));

    // Dispatch all queued events
    std::cout << "\n Dispatching queued events!\n";
    eventManager.DispatchEvents(); // TestObj handles the queued events here
	
	
	// Other ways to create event handlers.
	/*
	// ------------------------------------------------------------------
	// 1. Free function
    eventManager.Subscribe("TestEvent", FreeFunctionHandler);
	
	// -- free function in this case would be --
	void FreeFunctionHandler(const XEvent& event) {
		const auto& event_ = static_cast<const TestEvent&>(event);
		std::cout << "  FreeFunctionHandler: value_a: " << event_.value_a << " value_b: " << event_.value_b << "\n";
	}	
	
	// ------------------------------------------------------------------
    // 2. Lambda event handler
    eventManager.Subscribe (
		"TestEvent", 
		[](const XEvent& event) 
		{
			const auto& event_ = static_cast<const TestEvent&>(event);
			std::cout << "  LambdaFunctionsHandler: value_a: " << event_.value_a << " value_b: " << event_.value_b << "\n";
		}
	);
	
	// 3. Trigger events as usual
	std::cout << "\nOther ways to create event handlers\n";
	eventManager.TriggerEvent(TestEvent(300, 700));
	*/
	
    return 0;
}
