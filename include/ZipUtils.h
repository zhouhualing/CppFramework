#ifndef INCLUDED_ZIP_UTILS_H
#define INCLUDED_ZIP_UTILS_H

#include "StcTypes.h"


namespace stc {
namespace framework {

/*
	Utility class that helps inflate/deflate and/or archive data/files complying with zlib 1.2.3 format.
*/

class ZipUtils
{
public:

/// \brief Compress and Archive one more files into ZIP format.
    ///     Compress Level - 0  Store only
    ///     Compress Level - 1  Compress faster
    ///     Compress Level - 6  default
    ///     Compress Level - 9  Compress better

    static STC_PUBLIC bool ZipFiles(const std::string &archiveFile, const std::vector<std::string> &inputFiles, int compressLevel = DEFAULT_COMPRESSION_LEVEL);

private:
    static const uint32_t WRITE_BUFFER_SIZE;
    static const uint8_t DEFAULT_COMPRESSION_LEVEL;

};

}
}

#endif  

