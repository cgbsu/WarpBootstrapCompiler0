#include <Warp/Common.hpp>

#ifndef WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_FILE_IO_HPP
#define WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_FILE_IO_HPP

namespace Warp::Utilities {
    std::optional< std::string > read_file( std::string_view path );
}

#endif // WARP_BOOTSTRAP_COMPILER_HEADER_UTILITIES_FILE_IO_HPP

