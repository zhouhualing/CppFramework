#include "StdAfx.h"

#include "AbstractRecursiveFileExporter.h"

#include <fstream>
#include <iostream>
#include <list>

using namespace std;

using namespace stc::framework;

AbstractRecursiveFileExporter::AbstractRecursiveFileExporter(string strFilename) {
    this->strFilename = strFilename;
    pout = new ofstream(strFilename.c_str());
    if (pout) {
    } else {
        // TODO: P2S3: Report errors to caller
        cout << "-=-=- FAIL: Error opening file: " << strFilename.c_str() << endl;
    }
}

AbstractRecursiveFileExporter::AbstractRecursiveFileExporter(ostream* posOutputDestination) {
    this->strFilename = "";
    pout = posOutputDestination;
}

AbstractRecursiveFileExporter::~AbstractRecursiveFileExporter() {
}

void AbstractRecursiveFileExporter::openOutput() {
}

void AbstractRecursiveFileExporter::saveSpecialCaseObjects(Exportable& exp) {
}

void AbstractRecursiveFileExporter::closeOutput() {
    if (strFilename != "") {
        if (NULL != pout) {
            if (ofstream* pofs = dynamic_cast<ofstream*>(pout)) {
                pofs->close();
                delete pout;
                pout = NULL;
            }
        }
    }
}

string AbstractRecursiveFileExporter::fixEscapeInString(string strUnescaped, 
                                                        string strNeedsEscaping, 
                                                        string strEscapedString) {
    string strEscaped = strUnescaped;

    // Generate list of positions to replace, starting with the end
    list<string::size_type> listUnescapedStringLocations;
    string::size_type index = strEscaped.size();
    while (true) {
        index = strEscaped.rfind(strNeedsEscaping, index - 1);
        if (string::npos == index) {
            break;
        }

        listUnescapedStringLocations.push_back(index);

        if (0 == index) {
            break;
        }
    }
    
    // Replace all indicated locations
    list<string::size_type>::iterator iterLocations = 
        listUnescapedStringLocations.begin();
    while (iterLocations != listUnescapedStringLocations.end()) {
        string::size_type index = *iterLocations;

        strEscaped.replace(index, strNeedsEscaping.size(), strEscapedString);

        iterLocations++;
    }

    return strEscaped;
}

