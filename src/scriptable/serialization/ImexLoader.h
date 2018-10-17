#ifndef __IMEXLOADER_H__
#define __IMEXLOADER_H__

#include <string>

#include "ImportableExportable.h"
#include "StcExport.h"

using namespace std;

namespace stc {
namespace framework {

/**
 * An ImexLoader is a facade for the import/export classes that
 * provides simple methods to load from XML.
 *
 * @author <a href="mailto:Andrew.Oliver@spirentcom.com">Andy Oliver</a>
 */
class ImexLoader {
  public:
    STC_PUBLIC static ImportableExportable* loadFromXml(string strFilename);
	STC_PUBLIC static ImportableExportable* loadFromXmlString(string config);

    STC_PUBLIC static vector<ImportableExportable*> loadPartialFromXml(string strFilename, ImportableExportable* parent);
    STC_PUBLIC static vector<ImportableExportable*> loadPartialFromXmlString(string config, ImportableExportable* parent);
};

}
}

#endif

