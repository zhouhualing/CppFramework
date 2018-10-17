#ifndef INCLUDED_SQLITE_UTIL_H
#define INCLUDED_SQLITE_UTIL_H

#include "StcTypes.h"
#include "StcExport.h"

#include <string>
#include <vector>

namespace stc {
namespace framework {

class CppSQLite3Query;

class SqliteUtil
{
public:
    /// Returns the column names as a vector<string>
    static STC_PUBLIC std::vector<std::string> ExtractColumnNames(CppSQLite3Query& query, bool removeSpaces = true);

    /// Returns the rows as a vector<string> space-delimited columns, {} for empty columns, and {} around columns containing spaces 
    static STC_PUBLIC std::vector<std::string> ExtractRows(CppSQLite3Query& query);
};

}
}

#endif  //INCLUDED_SQLITE_UTIL_H

