#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include "ImportableExportable.h"

namespace stc {
namespace framework {

/**
 * An Importer is a class than can create a ScriptableObject from some
 * format on permanent storage, like XML, HTML, Tcl, an SQL database,
 * etc.
 *
 * Subtypes are impected to have constructors that specify the
 * format-specific source specification (filename, database
 * connection string, etc.)
 *
 * @author <a href="mailto:Andrew.Oliver@spirentcom.com">Andy Oliver</a>
 */
class STC_PUBLIC Importer {
  public:
    virtual ~Importer() {}
    // TODO: Talk with Andy see if we can change these and just have load return a vector of "roots"?
    //       Also should use uri to specify import source instead of subtype ctor?
    virtual ImportableExportable* load(void) = 0;
	virtual vector<ImportableExportable*> loadPartial(ImportableExportable*) = 0;
};

}
}

#endif

