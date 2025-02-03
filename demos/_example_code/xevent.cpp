#include <iostream> 
#include <sstream>
#include "xevent.hpp"


// -------------------------------------------------------------------
// Demonstrates basic example usage of XEvent, a robust alternative to
// Panda3D's default event_system, recommended for custom events.
// -------------------------------------------------------------------


/*
	Step 01: Define a custom event class (or a struct) inheriting from XEvent.
*/
class TestEvent : public XEvent {
public:
    TestEvent(unsigned int value_a, unsigned int value_b)
        : value_a(value_a), value_b(value_b) {}

    // Unique identifier for this event type
    std::type_index GetEventType() const override { return typeid(TestEvent); }

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
        resizeHandler_ = [this](const TestEvent& e) { OnEvent(e); };
        XEventManager::Instance().Subscribe<TestEvent>(resizeHandler_);
		
        std::cout << "[TestObj] Subscribed to TestEvent.\n";
    }

    ~TestObj() {
		
        // Unsubscribe from TestEvent when the object is destroyed
        XEventManager::Instance().Unsubscribe<TestEvent>(resizeHandler_);
		
        std::cout << "\n[TestObj] Unsubscribed from TestEvent.\n";
    }

    // Callback function to handle TestEvent
    void OnEvent(const TestEvent& e) {

		std::ostringstream oss;
		oss << "  [TestObj] OnEvent: "
			<< "value_a: (" << e.value_a << ") "
			<< "value_b: (" << e.value_b << ")";
		std::cout << oss.str() << "\n";
    }

private:
    XEventHandler<TestEvent> resizeHandler_; // Handler for TestEvent
};



// -------------------------------------------Main function------------------------------------------- //
int main(int argc, char* argv[]) {
	
	std::cout << "----XEvent demo----\n";
	
	auto& eventManager = XEventManager::Instance();
	
	// Instances of TestObj and TestEvent
	TestObj testObj;
	TestEvent resizeEvent(1024, 768); 

	// Trigger an event immediately
	std::cout << "\n Dispatching events!\n";
	eventManager.TriggerEvent(resizeEvent);

	// Queue multiple events for later processing
	eventManager.QueueEvent(std::make_unique<TestEvent>(800, 600));

	// Dispatch all queued events
	std::cout << "\n Dispatching queued events!\n";
	eventManager.DispatchEvents(); // TestObj handles the queued events here

    return 0;
}
