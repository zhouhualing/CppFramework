#ifndef __IMPORTABLEPROVIDER_H__
#define __IMPORTABLEPROVIDER_H__

#include "ImportableExportable.h"

using namespace std;

namespace stc {
namespace framework {

/**
 * An ImportableProvider can create certain types of Importable objects.
 *
 * @author <a href="mailto:Andrew.Oliver@spirentcom.com">Andy Oliver</a>
 */
class ImportableProvider {
  public:
    virtual ~ImportableProvider() {}
    virtual ImportableExportable* create(string strType, ImportableExportable* parent) = 0;
};

}
}

#endif

