#ifndef __EXPORTER_H__
#define __EXPORTER_H__

#include "Exportable.h"

namespace stc {
namespace framework {

/**
 * An Exporter is a class than can write a ScriptableObject into some
 * format on permanent storage, like XML, HTML, Tcl, an SQL database,
 * etc.
 *
 * Subtypes are expected to have constructors that specify the
 * format-specific destination specification (filename, database
 * connection string, etc.)
 *
 * @author <a href="mailto:Andrew.Oliver@spirentcom.com">Andy Oliver</a>
 */
class Exporter {
  public:
    virtual ~Exporter() {}
    virtual int save(Exportable& exportableToStore) = 0;
    virtual int save(Exportable& expRoot,
                     set<Exportable*> setExportablesToSave) = 0;
};

}
}

#endif

