#include <filesystem>

#include "Resource.h"

namespace Engine::Resource
{
    void Import(std::string const directory, void (* const function)(std::string const & file))
    {
        if(std::filesystem::exists(directory))
        {
            for(std::filesystem::path const & path : std::filesystem::recursive_directory_iterator(directory))
            {
                if(std::filesystem::is_regular_file(path))
                {
                    std::string file = path.string();

                    std::replace(file.begin(), file.end(), '\\', '/');

                    function(file);
                }
            }
        }
        else
        {
            std::filesystem::create_directory(directory);
        }
    }
}