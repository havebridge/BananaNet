#pragma once

#include <memory>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Core
{
	class Log
	{
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	public:
		Log() = default;
		~Log() = default;

	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() {return  s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return  s_ClientLogger; }
	};

#define HN_CORE_TRACE(...) ::Core::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define HN_CORE_INFO(...) ::Core::Log::GetCoreLogger()->info(__VA_ARGS__)
#define HN_CORE_WARN(...) ::Core::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define HN_CORE_ERROR(...) ::Core::Log::GetCoreLogger()->error(__VA_ARGS__)
#define HN_CORE_FATAL(...) ::Core::Log::GetCoreLogger()->fatal(__VA_ARGS__)

#define HN_TRACE(...) ::Core::Log::GetClientLogger()->trace(__VA_ARGS__)
#define HN_INFO(...) ::Core::Log::GetClientLogger()->info(__VA_ARGS__)
#define HN_WARN(...) ::Core::Log::GetClientLogger()->warn(__VA_ARGS__)
#define HN_ERROR(...) ::Core::Log::GetClientLogger()->error(__VA_ARGS__)
#define HN_FATAL(...) ::Core::Log::GetClientLogger()->fatal(__VA_ARGS__)
}