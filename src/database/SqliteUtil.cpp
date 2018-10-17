#include "StdAfx.h"

#include "StringUtil.h"
#include "CppSQLite3.h"

#include "SqliteUtil.h"

using namespace stc::framework;


/// Returns the column names as a vector<string>
STC_PUBLIC std::vector<std::string> SqliteUtil::ExtractColumnNames(CppSQLite3Query& query, bool removeSpaces)
{
    std::vector<std::string> result;
    int numFields = query.numFields();

    for(int i = 0; i < numFields; i++)
    {
        result.push_back(removeSpaces ? StringUtil::RemoveChars(query.fieldName(i), " ") : query.fieldName(i));
    }

    return result;
}

STC_PUBLIC std::vector<std::string> SqliteUtil::ExtractRows(CppSQLite3Query& query)
{
    std::vector<std::string> result;
    int numFields = query.numFields();

    while (!query.eof())
    {
        std::string row;

        for(int i = 0; i < numFields; ++i)
        {
            if (i != 0)
                row += " ";

            const char * dataPtr = query.fieldValue(i);

            if (dataPtr == NULL || *dataPtr == '\0')
            {
                row += "{}";
            }
            else
            {
                std::string data = dataPtr;
                if (data.find_first_of(" \t\n") != std::string::npos)
                {
                    row += "{";
                    row += data;
                    row += "}";
                }
                else
                {
                    row += data;
                }
            }
        }
        result.push_back(row);
        query.nextRow();
    }

    return result;

}

