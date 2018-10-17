#ifndef __LOADER_H__
#define __LOADER_H__

#include <string>

#include "Scriptable.h"

using namespace std;
using namespace stc::framework;

namespace stc {
namespace framework {

/**
 * A Loader is a facade for the import/export classes that
 * provides simple methods to load from XML.
 *
 * @author <a href="mailto:Andrew.Oliver@spirentcom.com">Andy Oliver</a>
 */
class Loader {
  public:
    STC_PUBLIC static CScriptable* loadFromXml(string strFilename);
	STC_PUBLIC static CScriptable* loadFromXmlString(string strConfig);

	STC_PUBLIC static std::vector<CScriptable*> loadPartialFromXml(string strFilename, CScriptable* baseConfig);
	STC_PUBLIC static std::vector<CScriptable*> loadPartialFromXmlString(string strConfig, CScriptable* baseConfig);
};

}
}

#endif

