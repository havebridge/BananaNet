#pragma once

#include "spdlog/spdlog.h"


namespace Core
{
    void test()
    {
         spdlog::info("Welcome to spdlog version {}.{}.{}  !", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR,
                 SPDLOG_VER_PATCH);
    }
}