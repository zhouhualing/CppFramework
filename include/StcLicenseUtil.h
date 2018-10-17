#ifndef INCLUDED_STC_LICENSE_UTIL_H
#define INCLUDED_STC_LICENSE_UTIL_H

// Framework
#include "StcTypes.h"
#include "StcExport.h"

#include <string>
#include <vector>

namespace stc {
namespace framework {

class StcLicenseUtil
{
public:
	/// \brief Get license date
	static STC_PUBLIC std::string GetLicenseDate();

	/// \brief Check out license
	static STC_PUBLIC void CheckoutLicense(const std::string& feature,
		bool checkDbv = true /* Check that date based version(DBV) is valid */);

	/// \brief Indicates if STC is running in a virtual environment
	static STC_PUBLIC bool IsVirtual();
};

}
}

#endif  //INCLUDED_STC_LICENSE_UTIL_H

