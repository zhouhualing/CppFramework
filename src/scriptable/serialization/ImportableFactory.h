#ifndef __IMPORTABLEFACTORY_H__
#define __IMPORTABLEFACTORY_H__

#include <map>
#include <string>
#include <vector>

#include "ImportableProvider.h"
#include "StcExport.h"

using namespace std;

namespace stc {
namespace framework {

class ImportableFactory;

/**
 * An ImportableFactory allows new Importable objects to be created.
 * It delegates its work to other objects in the scriptable hierarchy.
 *
 * @author <a href="mailto:Andrew.Oliver@spirentcom.com">Andy Oliver</a>
 */
class ImportableFactory {
  public:
    STC_PUBLIC static ImportableFactory& getInstance();

    STC_PUBLIC void registerProvider(ImportableProvider* pprovider,
                          vector<string> vecProvidedTypes);

    STC_PUBLIC void unregisterProvider(ImportableProvider* pprovider);

    ImportableExportable* create(string strType, ImportableExportable* parent = 0);
  private:
    map<string, ImportableProvider*> mapTypeToProvider;

    ImportableFactory();
    ImportableProvider* getProviderForType(string strType);
};

}
}

#endif

