#pragma once

#include "Core/Core.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <memory>

namespace Estun
{

class Log
{
public:
	static void Init();

	static std::shared_ptr<spdlog::logger> &GetCoreLogger() { return coreLogger; }
	static std::shared_ptr<spdlog::logger> &GetClientLogger() { return clientLogger; }

private:
	static std::shared_ptr<spdlog::logger> coreLogger;
	static std::shared_ptr<spdlog::logger> clientLogger;
};

} // namespace Estun

// Core log macros
#define ES_CORE_TRACE(...) ::Estun::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ES_CORE_INFO(...) ::Estun::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ES_CORE_WARN(...) ::Estun::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ES_CORE_ERROR(...) ::Estun::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ES_CORE_CRITICAL(...) ::Estun::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define ES_TRACE(...) ::Estun::Log::GetClientLogger()->trace(__VA_ARGS__)
#define ES_INFO(...) ::Estun::Log::GetClientLogger()->info(__VA_ARGS__)
#define ES_WARN(...) ::Estun::Log::GetClientLogger()->warn(__VA_ARGS__)
#define ES_ERROR(...) ::Estun::Log::GetClientLogger()->error(__VA_ARGS__)
#define ES_CRITICAL(...) ::Estun::Log::GetClientLogger()->critical(__VA_ARGS__)