#pragma once

#if defined(CHOPPER_WINDOWS_PLATFORM) || defined(CHOPPER_LINUX_PLATFORM)

extern Chopper::Application* Chopper::CreateApplication();

int main(int argc, char* argv[])
{
	Chopper::Logger::Init();
	CHOPPER_LOG_CRIT("Tis a message");
	CHOPPER_LOG_ERROR("Tis a message");
	CHOPPER_LOG_WARN("Tis a message");
	CHOPPER_LOG_INFO("Tis a message");
	CHOPPER_LOG_DEBUG("Tis a message");
	CHOPPER_LOG_TRACE("Tis a message");

	auto app = Chopper::CreateApplication();
	app->Run();
	delete app;

	return 0;
}

#endif
