#include "StdAfx.h"

#include "XmlGenericExporter.h"

#include <ostream>

using namespace stc::framework;

XmlGenericExporter::XmlGenericExporter(string strFilename) : XmlExporter(strFilename) {
}

XmlGenericExporter::XmlGenericExporter(ostream* posOutputDestination) : XmlExporter(posOutputDestination) {
}

void XmlGenericExporter::writeHeader(Exportable& exp) {
    *pout << "<?xml version=\"1.0\"?>" << endl;
    /*
    *pout << "<!DOCTYPE " <<
        "Object" <<
        " SYSTEM \""  << "__" <<
        "PhoenixGeneric" <<
        ".dtd\">" << endl;
    */
}

void XmlGenericExporter::startObject(Exportable& exp, Exportable& expParent, const string& strPrefix) {
    ostream& out = *pout;

    // Write PC header (name, type)
    out << strPrefix;
    out << "<Object type=\"";
    out << exp.getType();
    out << "\">" << endl;
}

void XmlGenericExporter::writeSerializationBaseProperty() {
    *pout << "  <SerializationBase />" << endl;
}

void XmlGenericExporter::finishObjectProperties(Exportable& exp,
                                                const string& strPrefix) {
}

void XmlGenericExporter::finishObject(Exportable& exp, const string& strPrefix) {
    ostream& out = *pout;

    // Close it out
    out << strPrefix;
    out << "</Object>";
    out << endl;
}

void XmlGenericExporter::startProperty(const string& strName,
                                        const string& strValue,
                                        const string& strPrefix) {
    ostream& out = *pout;

    out << strPrefix;
    out << "  <Property name=\"";
    out << strName;
    out << "\" value=\"";
    out << sanitizeString(strValue);
    out << "\" />";
    out << endl;
}

void XmlGenericExporter::finishProperty(const string& strPrefix) {
}

void XmlGenericExporter::handleRelationship(Exportable& exp, 
                                            const string& strType,
                                            bool bSourceToTarget,
                                            bool last,
                                            const string& strPrefix) {
    // TODO: Write out relationship
}

void XmlGenericExporter::writeTrailer() {
}

