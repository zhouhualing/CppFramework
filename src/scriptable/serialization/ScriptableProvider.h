#ifndef __SCRITABLEPROVIDER_H__
#define __SCRITABLEPROVIDER_H__

#include "ImportableProvider.h"

#include <string>

using namespace std;

namespace stc {
namespace framework {

class ScriptableProvider : public ImportableProvider
{
  public:
    virtual ~ScriptableProvider() {}
    static const ScriptableProvider& getInstance();

    ImportableExportable* create(string strType, ImportableExportable* parent);
  private:
    ScriptableProvider();
};

}
}

#endif

