#include <Warp/Utilities/FileIO.hpp>

namespace Warp::Utilities
{
    std::optional< std::string > read_file( std::string_view path )
    {
        std::filesystem::path file_path{ path };
        if( std::filesystem::exists( file_path ) == true )
        {
            std::ifstream file_stream;
            file_stream.open( file_path );
            if( file_stream.is_open() == false )
                return std::nullopt;
            std::stringstream media_buffer;
            while( file_stream.good() == true && file_stream.eof() == false )
            {
                std::string line;
                std::getline( file_stream, line );
                media_buffer << line;
            }
            return std::optional{ media_buffer.str() };
        }
        return std::nullopt;
    }
}
