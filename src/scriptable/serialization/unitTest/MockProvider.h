#ifndef __MOCKPROVIDER_H__
#define __MOCKPROVIDER_H__

#include "../ImportableProvider.h"

#include <string>

using namespace std;

using namespace stc::framework;

class MockProvider : public ImportableProvider
{
  public:
    MockProvider();
    virtual ~MockProvider();

    ImportableExportable* create(string strType, ImportableExportable* parent);

  private:
    void registerProvider();
    void unregisterProvider();

};

#endif

