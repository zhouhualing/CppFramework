#include "StdAfx.h"

#include <map>
#include <algorithm>

#include "ResourceManager.h"

#include "StcException.h"
#include "BLLLoggerFactory.h"
#include "StringUtil.h"

using namespace stc::framework;


// development category/logger
DEFINE_STATIC_LOGGER("fmwk.bll.base.resourceManager", devLogger);


CResourceManager& CResourceManager::Instance()
{
	static CResourceManager resourceMgr;
	return resourceMgr;
}
	
CResourceManager::CResourceManager() 
{
}

CResourceManager::~CResourceManager()
{
}





/**
*	Adds the string resource using the key; throws on if there are duplicates
*
*	@param key This must be a unique string across the solution
*	@param resource a string to be stored </param>
*
*	@returns none
*/
void CResourceManager::AddStringResource(const ResourceKey& key, const std::string& resource)
{
	LOG_DEBUG(devLogger(), "Trying to insert resource \'" << resource 
				<< "\' using key: " << key);

	if (m_resourceMap.find(key) != m_resourceMap.end())
	{
		stc::framework::CResourceManager::Instance().PrintResources();
		throw CStcException("Cannot add duplicate resource: " + resource);
	}

	if (!m_resourceMap.insert(std::make_pair(key, resource)).second)
	{
		stc::framework::CResourceManager::Instance().PrintResources();
		throw CStcException("Cannot add resource: \'" + resource + "\'. Insertion failed.");
	}
}



/**
*	Returns the string resource using the key; throws if not found
*
*	@param key This must be a unique string across the solution
*
*	@returns a string associated with the given key
*/
std::string CResourceManager::GetStringResource(const ResourceKey& key) const
{
	const ResourceMap::const_iterator pos = m_resourceMap.find(key);
	if (pos != m_resourceMap.end())
	{
		return pos->second;
	} 
	else 
	{
		stc::framework::CResourceManager::Instance().PrintResources();
		std::string strKey;
        ConvertToString(strKey, (int64_t)key);
		throw CStcException("Cannot find resource: " + strKey);
	}
}






/**
*	ctor of the format object
*
*	@param key the msg to log
*	@returns none
*/ResourceFormatter::ResourceFormatter(ResourceKey key)
{
	mMinWidth = 0;
    mNextReplace = 0;
	mPadding = ' ';
    SetKey(key);
}

/**
*	private ctor for unit testing
*
*	@param msg the msg to log
*	@returns none
*/
ResourceFormatter::ResourceFormatter(const std::string& message)
{
	mMinWidth = 0;
    mNextReplace = 0;
	mPadding = ' ';

	//look up string resource string
	//and initialize string
    mKey = -1;
	mMessage = message;
}

ResourceFormatter::~ResourceFormatter()
{
}

/**
*	put together the msg and its tokens.
*
*	@returns string - formatted msg
*/
std::string ResourceFormatter::ToString()
{
	//subsitute resource string {0} with
	//return the final string;
	for (; mNextReplace < mParameters.size(); mNextReplace++) 
	{
		std::string prm = mParameters[mNextReplace];
		Replace(prm, (int)(mNextReplace+1));
	}

	return mMessage;
}



/**
*	replace substitutes in string with tokens.
*
*	@param prm	a token string
*	@param id	token count
*	@returns none
*/
void ResourceFormatter::Replace(std::string prm, int id)
{
	LOG_DEBUG(devLogger(), "Processing prm=" << prm << ", id=" << id);

	size_t sIndex = 0;
	std::string msg = mMessage;
	bool replaced = false;
	while (sIndex < msg.size())
	{
		// search string for {#}
		switch (msg[sIndex])
		{ 
			//the first letter of string
		case '{':
			{
				// find the end of replacement
				int fmCount = 1;                //this indicates how many '{' and'}'
				size_t siz;
				for (siz = sIndex + 1; fmCount && (siz != msg.size()); ++siz)
				{
					if (msg[siz] == '{') ++fmCount;
					if (msg[siz] == '}') --fmCount;
				}

				if (fmCount)
				{
					//error: mismatched parentheses, too many '{'
					LOG_ERROR(devLogger(), "String format contains invalid substitute - key: " << mKey);
					throw stc::framework::CStcInvalidArgument("mismatched parentheses, too many '{'");
				}

				std::ostringstream idStr;
				idStr << id;
				std::string prmCount = msg.substr(sIndex+1, siz - 1 - sIndex - 1);
				if (idStr.str() == prmCount)
				{
					replaced = true;
					msg.erase(sIndex, siz - sIndex);
					msg.insert(sIndex, prm);

					// process next char
					sIndex += prm.size();
				}
				else
				{
					// process next char
					sIndex += siz - sIndex + 1;
				}
			}
			break;

		default:
			// process next char
			sIndex++;
			break;
		}
	}

	if (!replaced)
	{
        // This is a << with no matching {#}; append it to the end
        msg += prm;
    }

	mMessage = msg;
}



/** test function */
void CResourceManager::PrintResources()
{
	ResourceMap::iterator pos;
	for (pos = m_resourceMap.begin(); pos != m_resourceMap.end(); ++pos) 
	{
       LOG_DEBUG(devLogger(), "key: " << pos->first << ", value: " << 
								pos->second);
	}
}

std::ostream& stc::framework::operator << (std::ostream& os, ResourceFormatter& res)
{
	return os << res.ToString();
}

