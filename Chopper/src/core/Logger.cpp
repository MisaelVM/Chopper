#include "Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Chopper {

	std::shared_ptr<spdlog::logger> Logger::s_Logger;

	void Logger::Init() {
		spdlog::set_pattern("%^[%m-%d-%C %T.%e] %n (%l): %v%$");
		s_Logger = spdlog::stdout_color_mt("CHOPPER");
		s_Logger->set_level(spdlog::level::trace);
	}

}
