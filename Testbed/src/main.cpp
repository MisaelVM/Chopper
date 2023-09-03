#include <Chopper.h>

class App : public Chopper::Application {
public:
	App() {}
	~App() {}
};

Chopper::Application* Chopper::CreateApplication() {
	return new App();
}
