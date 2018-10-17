#ifndef INCLUDED_STDAFX_H
#define INCLUDED_STDAFX_H

#include "ace/config.h"
#include "ace/OS.h"
#include "ace/INET_Addr.h"
#include "ace/Acceptor.h"

#ifdef WIN32

#include <list>
#include <vector>
#include <map>
#include <string>
#include <sstream>

#include "boost/intrusive/slist.hpp"
#include "boost/foreach.hpp"
#include "boost/range/rend.hpp"
#include "boost/iterator/reverse_iterator.hpp"
#include "boost/foreach.hpp"
#include "boost/mpl/apply.hpp"
#include "boost/utility.hpp"
#include "boost/multi_index_container.hpp"
#include "boost/function.hpp"
#include "boost/multi_index_container_fwd.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/function.hpp"
#include "boost/bind.hpp"

#include "xercesc/framework/XMLAttr.hpp"
#include "xercesc/parsers/SAXParser.hpp"

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#include "MetaClass.h"

#endif // #ifdef WIN32
#endif

