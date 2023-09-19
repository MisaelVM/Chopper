#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <common/definitions.h>
#include <common/includes.h>

namespace Chopper {
	
	class CHOPPER_API Logger {
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }

	private:
		static std::shared_ptr<spdlog::logger> s_Logger;
	};
}

#define CHOPPER_LOG_CRIT(...) ::Chopper::Logger::GetLogger()->critical(##__VA_ARGS__)
#define CHOPPER_LOG_ERROR(...) ::Chopper::Logger::GetLogger()->error(##__VA_ARGS__)
#define CHOPPER_LOG_WARN(...)  ::Chopper::Logger::GetLogger()->warn(##__VA_ARGS__)
#define CHOPPER_LOG_INFO(...)  ::Chopper::Logger::GetLogger()->info(##__VA_ARGS__)

#ifdef DEBUG_BUILD
#define CHOPPER_LOG_DEBUG(...) ::Chopper::Logger::GetLogger()->debug(##__VA_ARGS__)
#define CHOPPER_LOG_TRACE(...) ::Chopper::Logger::GetLogger()->trace(##__VA_ARGS__)
#else
#define CHOPPER_LOG_DEBUG(...)
#define CHOPPER_LOG_TRACE(...)
#endif
