include(FetchContent)

#Fetches the version 1.8.5 for spdlog
FetchContent_Declare(spdlog
        URL https://github.com/gabime/spdlog/archive/refs/tags/v1.8.5.zip
        )

# Disable stuff we don't need
option(SPDLOG_BUILD_EXAMPLE "" OFF)
option(SPDLOG_BUILD_TESTS "" OFF)
option(SPDLOG_INSTALL "" OFF)

FetchContent_MakeAvailable(spdlog)

# Disable warnings from the library target
target_compile_options(spdlog PRIVATE -w)
# Disable warnings from included headers
get_target_property(propval spdlog INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(spdlog SYSTEM PUBLIC "${propval}")
