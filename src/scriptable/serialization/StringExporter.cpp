#include "StdAfx.h"

#include "StringExporter.h"

#include <ostream>
#include <fstream>

#include "XmlSpecificExporter.h"

using namespace stc::framework;

StringExporter::StringExporter(string strFilename) : AbstractRecursiveExporter() {
    setNested(true);
}

void StringExporter::writeHeader(Exportable& exp) {
}

void StringExporter::startObject(Exportable& exp, Exportable& expParent, const string& strPrefix) {
    if (&exp != &expParent) {
        out << " ";
    }
    out << "(" << exp.getType() << "";
}

void StringExporter::finishObjectProperties(Exportable& exp, const string& strPrefix) {
}

void StringExporter::finishObject(Exportable& exp, const string& strPrefix) {
    out << ")";
}

void StringExporter::writeSerializationBaseProperty() {
    // TODO: P2S3: decide on final output style/flag
    out << "[base]";
}

void StringExporter::startProperty(const std::string& strName,
                                   const std::string& strValue,
                                   const string& strPrefix) {
    out << ", " << strName << "=" << strValue;
}

void StringExporter::finishProperty(const string& strPrefix) {
}

void StringExporter::writeTrailer() {
}

void StringExporter::handleRelationship(const string& strId, 
                                        const string& strType,
                                        const bool bSourceToTarget,
                                        const string& strPrefix) {
    // TODO: P2S3: Write the relationship to the string
}

string StringExporter::getString() {
    return out.str();
}

