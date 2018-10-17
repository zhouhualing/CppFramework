#ifndef INCLUDED_USERDEFAULT_UTIL_H
#define INCLUDED_USERDEFAULT_UTIL_H

#include "StcTypes.h"
#include "StcExport.h"
#include "MetaPropIdValueCache.h"

namespace stc {
namespace framework {

class STC_PUBLIC UserDefaultUtils
{
public:
   /// \brief Load meta info from objects to cache
	static void LoadCacheFromObjects(MetaPropIdValueCache& cache);

	/// \brief Update/Add  objects
	static void UpdateObejcts(MetaPropIdValueCache& cache);

	/// \brief Remove objects
	static void RemoveObejcts(const MetaPropIdValueCache& cache);

	/// \brief Get class and property from string
	static bool SplitProperty(const std::string& classPropId,
		std::string& cls, std::string& prop);

} ;
}
}


#endif  //INCLUDED_USERDEFAULT_UTIL_H

