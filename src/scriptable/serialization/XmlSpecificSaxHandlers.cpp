#include "StdAfx.h"

#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include "XmlSpecificSaxHandlers.h"

#include <iostream>
#include <memory>
#include <algorithm>

#include "ImportableFactory.h"
#include "StcException.h"
#include "BLLLoggerFactory.h"

using namespace std;

using namespace stc::framework;

// development category/logger
DEFINE_STATIC_LOGGER("user.load", userLogger);


// ---------------------------------------------------------------------------
//  XmlSpecificSaxHandlers: Constructors and Destructor
// ---------------------------------------------------------------------------
XmlSpecificSaxHandlers::XmlSpecificSaxHandlers():
	importer(0),
	icElementsSeen(0),
	pimexLoaded(NULL),
	usePartialLoad(false),
	useDefaultName(false),
	partialLoadParent(0),
	isInSerializationBaseBranch(false),
	overwriteParentOnPartialLoad(false) {
}

XmlSpecificSaxHandlers::~XmlSpecificSaxHandlers() {
}

void XmlSpecificSaxHandlers::SetImporter(AbstractImporter* imp) {
	importer = imp;
}

void XmlSpecificSaxHandlers::SetUsePartialLoad(bool partialLoad)
{
	usePartialLoad = partialLoad;
}

void XmlSpecificSaxHandlers::SetUseDefaultName(bool defaultName)
{
	useDefaultName = defaultName;
}

void XmlSpecificSaxHandlers::SetPartialLoadParent(ImportableExportable* parent, bool overwriteParent)
{
	partialLoadParent = parent;
	overwriteParentOnPartialLoad = overwriteParent;
}


// ---------------------------------------------------------------------------
//  XmlSpecificSaxHandlers: Overrides of the SAX ErrorHandler interface
// ---------------------------------------------------------------------------
void XmlSpecificSaxHandlers::error(const SAXParseException& e) {
    char* pszFilename = XMLString::transcode(e.getSystemId());
    string strFilename(pszFilename);
    XMLString::release(&pszFilename);
    char* pszMessage = XMLString::transcode(e.getMessage());
    string strMessage(pszMessage);
    XMLString::release(&pszMessage);

    LOG_ERROR(userLogger(), strFilename <<
        ", line " << e.getLineNumber() <<
        ", char " << e.getColumnNumber() <<
        " " << strMessage << endl);
}

void XmlSpecificSaxHandlers::fatalError(const SAXParseException& e) {
    char* pszFilename = XMLString::transcode(e.getSystemId());
    string strFilename(pszFilename);
    XMLString::release(&pszFilename);
    char* pszMessage = XMLString::transcode(e.getMessage());
    string strMessage(pszMessage);
    XMLString::release(&pszMessage);

	LOG_ERROR(userLogger(), 
		strFilename <<
        ", line " << e.getLineNumber() <<
        ", char " << e.getColumnNumber() <<
		" " << strMessage << endl);
}

void XmlSpecificSaxHandlers::warning(const SAXParseException& e) {
    char* pszFilename = XMLString::transcode(e.getSystemId());
    string strFilename(pszFilename);
    XMLString::release(&pszFilename);
    char* pszMessage = XMLString::transcode(e.getMessage());
    string strMessage(pszMessage);
    XMLString::release(&pszMessage);
    // TODO: Log the following
    /*
    XERCES_STD_QUALIFIER cerr << "\nWarning at file " << strFilename <<
        ", line " << e.getLineNumber() <<
        ", char " << e.getColumnNumber() <<
        "\n  Message: " << strMessage << XERCES_STD_QUALIFIER endl;
    */
}

// ---------------------------------------------------------------------------
//  XmlSpecificSaxHandlers: Overrides of the SAX DTDHandler interface
// ---------------------------------------------------------------------------
void XmlSpecificSaxHandlers::unparsedEntityDecl(const XMLCh* const name,
                                                const XMLCh* const publicId,
                                                const XMLCh* const systemId,
                                                const XMLCh* const notationName) {
    // Not used at this time
}


void XmlSpecificSaxHandlers::notationDecl(const XMLCh* const name,
                                          const XMLCh* const publicId,
                                          const XMLCh* const systemId) {
    // Not used at this time
}

void XmlSpecificSaxHandlers::endDocument() {
}

void XmlSpecificSaxHandlers::endElement(const XMLCh* const name) {
    char* pszName = XMLString::transcode(name);
    string strName(pszName);
    XMLString::release(&pszName);

    if ("Relation" != strName) {

		if (deqParents.empty() ||
			strName == serializationBaseName)
		{
			isInSerializationBaseBranch = false;
		}

		if (!deqParents.empty())
		{
			deqParents.pop_back();
		}
    }

	
}

void XmlSpecificSaxHandlers::startDocument() {
}

void XmlSpecificSaxHandlers::handleRelationship(AttributeList& attributes) {
    if (!deqParents.empty() && (NULL == deqParents.back())) {
        return;
    }

	if (deqParents.empty())
	{
        // TODO: Log
        // cout << "FAIL: Parents deque empty" << endl;
		return;
	}

    // Get the type
    char* pszType = XMLString::transcode(attributes.getValue("type"));
    string strType(pszType);
    XMLString::release(&pszType);

    // Get the source or target ID
    char* pszTarget = XMLString::transcode(attributes.getValue("target"));
    if (NULL != pszTarget) {
        const string strTarget(pszTarget);
        XMLString::release(&pszTarget);

        int iType = deqParents.back()->getRelationshipId(
			true, strType);

        // Record relation to target
		importer->recordRelationToTarget(deqParents.back(), iType, strTarget);

    } else {
        // Must be a source attribute
        char* pszSource = XMLString::transcode(attributes.getValue("source"));
        if (NULL != pszSource) {
            const string strSource(pszSource);
            XMLString::release(&pszSource);

            int iType = deqParents.back()->getRelationshipId(
				false, strType);

			// Record relation to source
			importer->recordRelationToSource(deqParents.back(), iType, strSource);
        } else {
            // TODO: Log
            // cout << "FAIL: neither source nor target in relation" << endl;
        }
    }
}

ImportableExportable* XmlSpecificSaxHandlers::GetCurrentChild(
	const std::string& name, 
	const AttributeList& attributes,
	ImportableExportable* parent) {

    ImportableExportable* pimexNewChild = 0;
        
	if (usePartialLoad)
	{
		bool isSerializationBase = false;
		const XMLCh* serlBase = attributes.getValue("serializationBase");
		if (serlBase)
		{
			char* pszAttrName = XMLString::transcode(serlBase);
			string strAttrName(pszAttrName);
			XMLString::release(&pszAttrName);
			if (strAttrName == "true")
			{
				isSerializationBase = true;
				isInSerializationBaseBranch = true;
				serializationBaseName = name;
			}
		}

		// SerializationBase is the attach point for the loaded config
		// If doing an overwrite, we will overwrite the parent node that was passed in
		// else we will attach the SerializationBase under the parent
		if (isSerializationBase)
		{
			STC_ASSERT(partialLoadParent, "partialLoadParent must be set first in partial load");

			if (overwriteParentOnPartialLoad)
			{
				// This should only occur once
				if (!vecimexLoaded.empty())
				{
					throw CStcInvalidArgument("Invalid PartialLoad Parent");
				}

				pimexNewChild = partialLoadParent;
			}
			else
			{
				// check for pseudo parent (affiliate port relation)
				// TODO: remove imex facad or try to expose subtype checking functions..
				bool affiliationPortAdded = false;
				if ((name == "EmulatedDevice" || name == "Router" || name == "Host") && 
					partialLoadParent->getType() == "Port")
				{
					pimexNewChild = 
						ImportableFactory::getInstance().create(
							name, (ImportableExportable*)(partialLoadParent->getParent()));

					pimexNewChild->addRelationship(partialLoadParent, 
						pimexNewChild->getRelationshipId(true, "AffiliationPort"));

					affiliationPortAdded = true;
				}
				else
				{
					pimexNewChild = 
						ImportableFactory::getInstance().create(name, partialLoadParent);
				}

				// Setup external relation reference
				const XMLCh* xmlAttr = attributes.getValue("id");
				if (xmlAttr)
				{
					char* handleStr = XMLString::transcode(xmlAttr);
					assert(handleStr);
					string strHandle(handleStr);
					XMLString::release(&handleStr);

					std::vector<Exportable*> siblings = partialLoadParent->getChildren();
					for (std::vector<Exportable*>::const_iterator sit = siblings.begin();
						 sit != siblings.end();
						 ++sit)
					{
						if ((*sit)->getId() == strHandle)
						{
                            Exportable *pExportable = *sit;

                            static const char * relationNames[] = 
                            {
                                "AffiliationPort",
                                "SrcBinding",
                                "DstBinding",
								"ExpectedRx",
                                "JoinedGroup",
                                "AffiliatedRouter"
                            };

                            for (int i = 0; i < sizeof(relationNames)/sizeof(relationNames[0]); ++i)
                            {
							    const int relId = pExportable->getRelationshipId(true, relationNames[i]);

								if (relId <= 0 ||
									(affiliationPortAdded && (strcmp(relationNames[i], "AffiliationPort") == 0)))
									continue;

							    std::vector<Exportable*> affiliatedObjs = (*sit)->getRelatives(relId, true);
							    for (std::vector<Exportable*>::const_iterator ait = affiliatedObjs.begin();
								     ait != affiliatedObjs.end();
								     ++ait)
							    {
								    ImportableExportable* imexAffiliated = 
									    dynamic_cast<ImportableExportable*>(*ait);
								    if (imexAffiliated)
								    {
									    pimexNewChild->addRelationship(imexAffiliated, relId);
								    }
							    }
                            }
							break;
						}
					}
				}
			}

			vecimexLoaded.push_back(pimexNewChild);
		}
		// Elements that are not inside the serialization base are ignored when doing a partial load.
		else if (isInSerializationBaseBranch)
		{
			pimexNewChild = ImportableFactory::getInstance().create(name, parent);
		}
	}
	else
	{
		pimexNewChild = ImportableFactory::getInstance().create(name, parent);
	}

	return pimexNewChild;
}

#define ELEMENT_YIELD_INTERVAL 50
void XmlSpecificSaxHandlers::startElement(const XMLCh* const name,
                                          AttributeList& attributes) {
	assert(importer);

    // Yield if needed
    icElementsSeen++;
    if (0 == (icElementsSeen % ELEMENT_YIELD_INTERVAL)) {
        if (!deqParents.empty() && (NULL != deqParents.back())) {
            deqParents.back()->yield();
        }
    }

    if (!deqParents.empty() && (NULL == deqParents.back())) {
        deqParents.push_back(NULL);
        return;
    }

    char* pszName = XMLString::transcode(name);
    string strName(pszName);
    XMLString::release(&pszName);

    if (strName == "Relation") {
        // Special-case handling for Relationships
        handleRelationship(attributes);
    } else {

        // Get parent
		ImportableExportable* parent = NULL;
        if (!deqParents.empty()) {
			parent = deqParents.back();
        }

        // Standard child
        ImportableExportable* pimexNewChild = NULL;
		try
		{
			pimexNewChild =	GetCurrentChild(strName, attributes, parent);
		}
		catch (CStcException&)
		{
			// TODO: Log unable to load
			pimexNewChild =	NULL;
		}


        if (NULL != pimexNewChild) {
            if (NULL == pimexLoaded) {
                pimexLoaded = pimexNewChild;
            }


            // Push onto stack
            deqParents.push_back(pimexNewChild);

            unsigned int len = attributes.getLength();
            for (unsigned int index = 0; index < len; index++) {
                char* pszAttrName = XMLString::transcode(attributes.getName(index));
                string strAttrName(pszAttrName);
                XMLString::release(&pszAttrName);
                char* pszAttrValue = XMLString::transcode(attributes.getValue(index));
                string strAttrValue(pszAttrValue);
                XMLString::release(&pszAttrValue);

                if (strAttrName == "id") {
                    // Handle 'id' attribute by storing in map
					importer->recordImportedObject(pimexNewChild, strAttrValue);
                } else if (strAttrName == "serializationBase") {  
					// Skip internal attribute introduced by partial save
                    continue;
				} else if (useDefaultName && strAttrName == "Name") {
					continue;
                } else { 

					// Normal attributes are saved in the imex normally
                    int iPropId = pimexNewChild->getPropertyIdForName(strAttrName);
					// TODO: Assert instead?
					if (iPropId != 0)
					{
						pimexNewChild->setPropertyValue(iPropId, strAttrValue);

						// Check if the property is a handle; if it is, it will 
						// need to be translated to the deserialized object's new handle.
						if (pimexNewChild->getPropertyTypeNameForId(iPropId) == "handle") {
							// Record the property info for endDocument processing
							importer->recordHandleProperty(pimexNewChild, iPropId);
						}
					}
					
                }

                // Remove this section when global class name uniqueness confirmed
                // 12 Aug 2005: Discussed with Raymond, names will be globally unique
                // This section can go
                /*
                  if (strAttrName != "schema") {
                */
                //cout << "Set attribute '" << strAttrName;
                //cout << "' to '" << strAttrValue << "'" << " for the object on top of the stack" << endl;
                /*
                  }
                */
            }
        } else {
            // Unable to create new child, perhaps a provider wasn't available
			if (!usePartialLoad || isInSerializationBaseBranch)
				deqParents.push_back(NULL);
        }
    }
}

ImportableExportable* XmlSpecificSaxHandlers::getImportable() {
    return pimexLoaded;
}

vector<ImportableExportable*> XmlSpecificSaxHandlers::getImportableBranches() {
    return vecimexLoaded;
}

