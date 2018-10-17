#include "StdAfx.h"

#include "XmlExporter.h"
#include "StringUtil.h"
#include <ostream>
#include <fstream>

using namespace stc::framework;

XmlExporter::XmlExporter(string strFilename) : AbstractRecursiveFileExporter(strFilename) {
    setNested(true);
    setExcludeUnsuitable(true);
}

XmlExporter::XmlExporter(ostream* posOutputDestination) : AbstractRecursiveFileExporter(posOutputDestination) {
    setNested(true);
    setExcludeUnsuitable(true);
}

void XmlExporter::writeSerializationBaseProperty() {
    *pout << "serializationBase=\"true\" ";
}

string XmlExporter::sanitizeString(string strUnclean) {
    string strClean = strUnclean;

    // TODO: P2S3: Change this code to do less string copying
    strClean = fixEscapeInString(strClean, "&", "&amp;");
    strClean = fixEscapeInString(strClean, "\"", "&quot;");
    strClean = fixEscapeInString(strClean, "\'", "&apos;");
    strClean = fixEscapeInString(strClean, "<", "&lt;");
    strClean = fixEscapeInString(strClean, ">", "&gt;");
    strClean = fixEscapeInString(strClean, "\n", "&#xD;&#xA;");

	char trimChars[] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x0B,0x0C,0x0E,0x0F,0x10,
		0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x00};
	strClean = StringUtil::RemoveChars(strClean,std::string(trimChars));

    return strClean;
}

