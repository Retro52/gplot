#pragma once

#include <vector>
#include <future>

namespace gplot::core
{
    class DriveIO
    {
    public:

        using file_data = std::vector<char>;

        file_data Read(std::string_view path);

        bool Write(std::string_view path, const file_data& data);

        std::future<file_data> ReadAsync(std::string_view path);

        std::future<bool> WriteAsync(std::string_view path, const file_data& data);

    private:

        static file_data ReadFile(std::string_view path);

        static bool WriteFile(std::string_view path, const file_data& data);

    };
}