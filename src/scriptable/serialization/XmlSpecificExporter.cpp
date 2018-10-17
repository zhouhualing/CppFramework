#include "StdAfx.h"

#include "XmlSpecificExporter.h"
#include "StcSystem.h"

#include <ostream>

using namespace stc::framework;

XmlSpecificExporter::XmlSpecificExporter(string strFilename) : XmlExporter(strFilename) {
}

XmlSpecificExporter::XmlSpecificExporter(ostream* posOutputDestination) : XmlExporter(posOutputDestination) {
}

void XmlSpecificExporter::writeHeader(Exportable& exp) {

    const char* encoding = GetEncoding();
    *pout << "<?xml version=\"1.0\"";

    if(encoding)
        *pout << " encoding=\"" << encoding << "\"?>";
    else
        *pout << "?>";

    *pout << endl << endl;
    *pout << "<!-- This is a machine-generated file. -->" << endl;
    *pout << "<!-- Please do not edit it. -->" << endl;
    *pout << "<!-- Generated by Framework ver. " << CStcSystem::Instance().GetVersion() << " -->" << endl;
    *pout << endl;
}

void XmlSpecificExporter::startObject(Exportable& exp, Exportable& expParent, const string& strPrefix) {
    ostream& out = *pout;

    // Write PC header (name, type)
    out << strPrefix;
    out << "<";
    out << exp.getType();
    out << " ";
    if (exp.hasNonParentChildNonDefaultRelationships()) {
        out << "id=\"" << exp.getId() << "\" ";
    }
}

void XmlSpecificExporter::finishObjectProperties(Exportable& exp,
                                                 const string& strPrefix) {
    ostream& out = *pout;

    out << ">" << endl;
}

void XmlSpecificExporter::finishObject(Exportable& exp, const string& strPrefix) {
    ostream& out = *pout;

    // Close it out
    out << strPrefix;
    out << '<';
    out << '/';
    out << exp.getType();
    out << '>';
    out << endl;
}

void XmlSpecificExporter::startProperty(const string& strName,
                                        const string& strValue,
                                        const string& strPrefix) {
    ostream& out = *pout;
    size_t preLineLength = strPrefix.size() + strName.size() + 3;

    out << endl;
    out << strPrefix;
    out << ' ';
    out << strName;
    out << '=';
    out << '"';
    out << StringUtil::WordWrap(sanitizeString(strValue), 79, preLineLength);
    out << '"';
}

void XmlSpecificExporter::handleRelationship(Exportable& exp,
                                             const string& strType,
                                             bool bSourceToTarget,
                                             bool last,
                                             const string& strPrefix) {
    ostream& out = *pout;

    // Extra indent level?
    out << incrementPrefix(strPrefix);
    out << "<Relation type=\"";
    out << strType;
    out << "\" ";
    if (bSourceToTarget) {
        out << "target";
    } else {
        out << "source";
    }
    out << "=\"";
    out << exp.getId();
    out << "\"/>";
    out << endl;
}

void XmlSpecificExporter::finishProperty(const string& strPrefix) {
}

void XmlSpecificExporter::writeTrailer() {
}

const char* XmlSpecificExporter::GetEncoding()
{
#ifdef WIN32
    // We need to call setlocale and pass NULL as a second parameter, to get current locate.
    // You would think that runtime would have getlocale method for that.
    // Anyway, do not get confused. setlocate(0, NULL) returns current locale.
    const char* szLocale = ::setlocale(0, NULL);

    const char* szCodePage = strrchr(szLocale, '.');

    if(szCodePage == NULL)
        return NULL;

    int nCodePage = atoi(szCodePage+1);

    // see http://www.microsoft.com/globaldev/reference/WinCP.mspx
    // and take a look at common\tools\xercesc\src\xercesc\util\Transcoders\Uniconv390\ccsid.h
    switch(nCodePage)
    {
    case 1250:
        return "windows-1250"; // Central Europe
    case 1251:
        return "windows-1251"; // Cyrillic
    case 1252:
        return "windows-1252"; // Latin I
    case 1253:
        return "windows-1253"; // Greek
    case 1254:
        return "windows-1254"; // Turkish
    case 1255:
        return "windows-1255"; // Hebrew
    case 1256:
        return "windows-1256"; // Arabic
    case 1257:
        return "windows-1257"; // Baltic
    case 1258:
        return "windows-1258"; // Vietnam
    case 874:
        return "iso-8859-11";  // Thai
    case 932:
        return "Shift-JIS";    // Japanese
    case 936:
        return "gbk"; // Simplified Chinese
    case 949:
        return "korean";  // Korean
    case 950:
        return "big5"; //Traditional Chinese Big5
    default:
        return NULL;
    }
#else
    return NULL;
#endif
}
