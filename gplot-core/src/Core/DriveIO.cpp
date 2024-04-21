#include <Core/DriveIO.hpp>

#include <fstream>
#include <iostream>

using namespace gplot::core;

DriveIO::file_data DriveIO::Read(std::string_view path)
{
    return ReadAsync(path).get();
}

std::future<DriveIO::file_data> DriveIO::ReadAsync(std::string_view path)
{
    return std::async(std::launch::async, [path]()
    {
        return DriveIO::ReadFile(path);
    });
}

bool DriveIO::Write(std::string_view path, const file_data& data)
{
    return WriteAsync(path, data).get();
}

std::future<bool> DriveIO::WriteAsync(std::string_view path, const gplot::core::DriveIO::file_data& data)
{
    return std::async(std::launch::async, [path, data]()
    {
        return DriveIO::WriteFile(path, data);
    });
}

DriveIO::file_data DriveIO::ReadFile(std::string_view path)
{
    std::ifstream file(path.data(), std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        std::cerr << __FILE__ << __LINE__ << "Failed to open file: " << path << std::endl;
        return { };
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size))
    {
        buffer.emplace_back('\0');
        return buffer;
    }
    else
    {
        std::cerr << __FILE__ << __LINE__ << "Failed to read file: " << path << std::endl;
        return { };
    }
}

bool DriveIO::WriteFile(std::string_view path, const gplot::core::DriveIO::file_data& data)
{
    std::ofstream file(path.data(), std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << __FILE__ << __LINE__ << "Failed to open file: " << path << std::endl;
        return false;
    }

    if (file.write(data.data(), static_cast<std::streamsize>(data.size())))
    {
        return true;
    }

    std::cerr << __FILE__ << __LINE__ << "Failed to write to file: " << path << std::endl;
    return false;
}
