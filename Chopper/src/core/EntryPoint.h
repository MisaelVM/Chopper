#pragma once

extern Chopper::Application* Chopper::CreateApplication();

int main(int argc, char* argv[])
{
	auto app = Chopper::CreateApplication();
	app->Run();
	delete app;

	return 0;
}
