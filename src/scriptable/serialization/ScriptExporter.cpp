#include "StdAfx.h"

#include "ScriptExporter.h"
#include "AutomationOptions.h"
#include "Path.h"
#include "StcSystem.h"
#include "ScriptableImexAdapter.h"
#include "Saver.h"
#include "PerlCodeGen.h"
#include "PythonCodeGen.h"
#include "Project.h"

#include "ace/ACE.h"

#include <ostream>
#include <fstream>
#include <sstream>
#include <cassert>

using namespace stc::framework;

ScriptExporter::ScriptExporter(string strFilename) 
: AbstractRecursiveFileExporter(strFilename)
, iiPortLocationCurrent(-1)
, pexpCurrent(NULL)
, poutLauncher(NULL)
{
    setNested(false);    
    setExcludeUnsuitable(true);
    mScriptGen = NULL;

    // Open Logic file
    openLogic();

    initScriptCodeGen();
}

ScriptExporter::ScriptExporter(ostream* posOutputDestination) 
: AbstractRecursiveFileExporter(posOutputDestination)
, iiPortLocationCurrent(-1)
, pexpCurrent(NULL)
, poutLauncher(NULL)
{
    setNested(false);    
    setExcludeUnsuitable(true);
    mScriptGen = NULL;
    initScriptCodeGen();
}

ScriptExporter::~ScriptExporter()
{
    if(mScriptGen)
    {
        delete mScriptGen;
        mScriptGen = NULL;
    }
}

void ScriptExporter::writeIntroComment()
{
	const string time = CTimer::GetCurrentSystemDateTime();
    const string filename = Path::GetFileName(strLogicFilename);
    const string code = mScriptGen->IntroComment(time, filename);
    *pout << code;
}

/// \brief Writes out the initialization tcl logic
void ScriptExporter::writeInitCode()
{
    std::string logLevel;
    std::string logTo;
    CScriptable& autoOptions(CAutomationOptions::Instance());
    autoOptions.Get(FRAMEWORK_AutomationOptions_ucLogLevel, logLevel);
    autoOptions.Get(FRAMEWORK_AutomationOptions_szLogTo, logTo);

    string code = mScriptGen->Init(logTo, logLevel, "STOP_ON_ERROR");
    *pout << code;
}

/// \brief Writes out the connect and reserve tcl logic (not handled by apply)
void ScriptExporter::writeConnectReserveCode()
{
    string code = mScriptGen->ConnectReserveApply();
    *pout << code;

    list<Exportable*>::iterator iterResultQuery = listResultQueryForRDS.begin();
    int iterCount = 1;
    while (iterResultQuery != listResultQueryForRDS.end())
    {
        Exportable* pexp = *iterResultQuery;

        // Get Parent
        string strParent;
        createCodeToGetHandleForObject(*(pexp->getParent()->getParent()), strParent);

        // Get Result Root(s)
        int ciResultParent = 0;
        string strResultParent("");
        string strResultRootCurrent;
        ostringstream osResultParent;
        osResultParent << "\"" << strResultParent;

        // Get Result Root property
        int idResultParent = getPropertyIdForName(*pexp, string("ResultRootList"));
        string strResultRootList = pexp->getPropertyValueForId(idResultParent);

        // Loop over list items
        const string strDelimiters(" \t\n");
        string::size_type posStart;
        string::size_type posEnd;

        posStart = strResultRootList.find_first_not_of(strDelimiters);

        while (string::npos != posStart)
        {
            posEnd = strResultRootList.find_first_of(strDelimiters, posStart);

            if (string::npos == posEnd)
            {
                posEnd = strResultRootList.length();
            }

            string strResultRootHandleCurrent =
                strResultRootList.substr(posStart, (posEnd - posStart));
            // (posEnd - posStart));

            // Convert Handle to Exportable
            Exportable* pexpCurrent =
                mapIdToExportable[strResultRootHandleCurrent];

            if (NULL != pexpCurrent)
            {
                // Convert exportable to Tcl lookup code
                createCodeToGetHandleForObject(*pexpCurrent, strResultRootCurrent);

                // Append to list
                osResultParent << " " << strResultRootCurrent;

                // Remember we saw another good root
                ciResultParent++;
            }

            posStart = strResultRootList.find_first_not_of(strDelimiters, posEnd);
        }
        osResultParent << " \"";
        if (0 != ciResultParent)
        {
            strResultParent = osResultParent.str();
        }
        else
        {
            // Use project if no valid roots available
            strResultParent = mScriptGen->HandleForChildObject("system1", "Project", 0, STRING_LITERAL);
        }

        // Get Types
        int idConfig = getPropertyIdForName(*pexp, string("ConfigClassId"));
        int idResult = getPropertyIdForName(*pexp, string("ResultClassId"));
        string strConfigType = pexp->getPropertyValueForId(idConfig);
        string strResultType = pexp->getPropertyValueForId(idResult);

        std::ostringstream strFilterList;
        const int relId = pexp->getRelationshipId(true, "ResultFilters");
        vector<Exportable*> filterVec = pexp->getRelatives(relId, true);

        for(vector<Exportable*>::iterator fIte = filterVec.begin(); fIte != filterVec.end(); ++fIte)
        {
            string strFilterHandle;
            createCodeToGetHandleForObject(**fIte, strFilterHandle);
            strFilterList << strFilterHandle << " ";
        }

        // Prefix
        string strFilenamePrefix = getCsvFileName(strResultType, iterCount);

        // ViewAttributeList
        const int idPropIds = getPropertyIdForName(*pexp, "PropertyIdArray");
        const string strPropIds = pexp->getPropertyValueForId(idPropIds);

        // We only write the non-qualified form of property id for
        // backwards compatibility. ResultsSubscribeCommand will convert the strings
        // to real PropIds via a lookup in the Result class' MetaClass.
        vector<string> ids;
        StringUtil::Tokenize(strPropIds, " ", ids);
        ostringstream viewAttributeList;
        vector<string>::iterator it = ids.begin();
        for(; it != ids.end(); ++it)
        {
            vector<string> tokens;
            StringUtil::Tokenize((*it), ".", tokens);
            viewAttributeList << tokens[1] << " ";
        }

        // Subscribe once for each ResultQuery
        string subscribeCode = mScriptGen->Subscribe(strFilenamePrefix, strParent,
                                                     strResultParent, strConfigType,
                                                     strResultType, strFilterList.str(),
                                                     viewAttributeList.str());
        *pout << subscribeCode;

        iterResultQuery++;
        iterCount++;
    }

    if(listResultQueryForCharts.size() > 0)
    {
        string code = mScriptGen->ChartSubscribe();
        *pout << code;
    }

    code = mScriptGen->Disconnect();
    *pout << code;
}

void ScriptExporter::writeSerializationBaseProperty()
{
    // Ignore the request.  Tcl scripts have no need to keep track of
    // which objects were the base objects
}

void ScriptExporter::writeHeader(Exportable& exp)
{
    iiPortLocationCurrent = -1;
    writeIntroComment();
    writeInitCode();

    // Loading from configuration...
    Properties props;
    vector<string> values;
    values.push_back("true");
    props["IsLoadingFromConfiguration"] = values;
    string code = mScriptGen->Config("system1", props, STRING_LITERAL, STRING_LITERAL);
    *pout << code << endl << endl;
}

/// Find ports no matter how deep -- recurse down up to 3 levels
/// (since StcSystem is grandparent of Port)
void ScriptExporter::writePortsAndAncestors(Exportable& exp, Exportable& expParent, std::string& strPrefix, int depth = 0)
{
    if ((exp.getType() == "StcSystem") ||
        (exp.getType() == "Project") ||
        (exp.getType() == "Port"))
    {
        rememberAlreadyExported(exp);
        writeOneObject(exp, expParent, strPrefix, false);

        // If we haven't reached grandchildren, recurse
        if (depth < 2)
        {
            vector<Exportable*> vecChildren = exp.getChildren();
            vector<Exportable*>::iterator iterChildren = vecChildren.begin();
            while (iterChildren != vecChildren.end())
            {
                if ((*iterChildren)->getType() == "Port")
                {
                    writePortsAndAncestors(**iterChildren, exp, strPrefix, depth + 1);
                }
                else
                {
                    writePortsAndAncestors(**iterChildren, **iterChildren, strPrefix, depth + 1);
                }

                iterChildren++;
            }
        }
    }
}

void ScriptExporter::saveSpecialCaseObjects(Exportable& exp)
{
    // Write out all ports and their ancestors too
    string strPrefix("");
    writePortsAndAncestors(exp, exp, strPrefix);
}

string ScriptExporter::sanitizeType(string strType)
{
    string type = StringUtil::Replace(strType, ":", "_");
    return StringUtil::Replace(type, ".", "_");    
}

string ScriptExporter::sanitizeString(string strUnclean)
{
    string strClean = strUnclean;

    strClean = fixEscapeInString(strClean, "\\", "\\\\");
    strClean = fixEscapeInString(strClean, "\"", "\\\"");
    strClean = fixEscapeInString(strClean, "$", "\\$");

    return strClean;
}

string ScriptExporter::getNameForObject(Exportable& exp)
{
    string strName("");

    if ("StcSystem" == exp.getType())
    {
        strName = "system1";
    }
    else
    {
        string strId = exp.getId();
        if (mapExportableToName.find(strId) != mapExportableToName.end())
        {
            strName = mapExportableToName[strId];
        }
        else
        {
            int id = 0;

            string strType = sanitizeType(exp.getType());
            if (mapTypeToLargestId.find(strType) != mapTypeToLargestId.end())
            {
                id = mapTypeToLargestId[strType];
            }

            id++;
            mapTypeToLargestId[strType] = id;

            strName = mScriptGen->CreateVarName(strType, id);
            mapExportableToName[strId] = strName;


        }
    }

    return strName;
}

// Determine if the object is manually created or automatically created
bool ScriptExporter::isManuallyCreated(Exportable& exp)
{
    return exp.isManuallyCreated(getPositionInParent(exp));
}

bool ScriptExporter::canRemove(Exportable& exp)
{
    bool remove = false;

    if(getOptions().useDefaultsForRarelyUsedSettings)
    {
        const bool autoCreated = !isManuallyCreated(exp);
        const bool relEnd = isSourceOrDestForRelationship(exp);
        if(isCandidateForRemoval(exp))
        {
            if(autoCreated && !relEnd)
            {
                remove = true;
            }
            else if(!autoCreated)
            {
                remove = true;
                string parentType = exp.getParent()->getType();
                if(relEnd && parentType != "StcSystem" && parentType != "Project")
                {
                    // We won't remove it if it's more complicated than this.
                    // One parent is pretty intuitive to the user.
                    // We may need to rethink how this should work if it ever
                    // gets more complicated. For now, this is adequate.
                    remove = false;
                }
            }
        }
        else if(isChildOfDefaultHost(exp))
        {
            remove = true;
        }
        // It can be removed if it's an auto-created object
        // and the handle is not needed for relationship connections
        // and there is no additional configuration on the object needed
        // and it's children ( if any ) can also be safely removed. whew!
        else if(autoCreated &&
                !relEnd &&
                !exp.isOnePropertyVisible())
        {
            remove = true;
        }
        // Removed if it's part of Result*Set which is not subscribed.
        else if(exp.getType() == "ResultDataSet" || exp.getType() == "ResultChartQuerySet")
        {
            remove = !isResultSetSubscribed(exp);
        }
        else if(exp.getParent() != NULL && ( exp.getParent()->getType() == "ResultDataSet" ||
                                             exp.getParent()->getType() == "ResultChartQuerySet"))
        {
            remove = !isResultSetSubscribed(*exp.getParent());
        }
        else if(exp.getParent() != NULL &&
                exp.getParent()->getType() == "ResultQuery" &&
                exp.getParent()->getParent() != NULL)
        {
            remove = !isResultSetSubscribed(*exp.getParent()->getParent());
        }
    }

    if(remove)
    {
        vector<Exportable*> children = exp.getChildren();
        vector<Exportable*>::iterator it = children.begin();
        for(; it != children.end(); ++it)
        {
            bool removeChild = canRemove(**it);
            if(!removeChild)
            {
                remove = false;
                break;
            }
        }

        if(remove)
        {
            removedIds.push_back(exp.getId());
        }
    }
    return remove;
}

bool ScriptExporter::isVirtualPort(Exportable &exp)
{
    string isVirtual = exp.getPropertyValueForId(getPropertyIdForName(exp, "IsVirtual"));
    return (isVirtual == "TRUE");
}

bool ScriptExporter::isChildOfDefaultHost(Exportable& exp)
{
    Exportable* parent = exp.getParent();
    if(parent != NULL && parent->getType() == "Host")
    {
        return parent->getParent() != NULL && parent->getParent()->getType() == "Port";
    }

    return false;
}

bool ScriptExporter::wasRemoved(Exportable& exp)
{
    return std::find(removedIds.begin(), removedIds.end(), exp.getId()) != removedIds.end();
}

void ScriptExporter::startObject(Exportable& exp, Exportable& expParent, const string& strPrefix)
{
    saveResultQuery(exp, expParent);

    writeFilePathSubstitution(exp);

    pexpCurrent = &exp;

    // TODO: Move special-case handling to plug-in from provider
    // TODO: Re-enable when port type information available in data model

    if (exp.getType() == "Port")
    {
        if (!isVirtualPort(exp))
        {
            string strLocation = exp.getPropertyValueForId(getPropertyIdForName(exp, "Location"));
            listPortLocations.push_back(strLocation);
            iiPortLocationCurrent++;
        }
    }

    string strObjName = getNameForObject(exp);
    if ("StcSystem" == exp.getType())
    {
        string code = mScriptGen->Assignment("system1", "system1", "string", STRING_LITERAL);
        *pout << code;

        Properties props;
        code = mScriptGen->Config("system1", props, STRING_LITERAL, STRING_LITERAL, false);
        *pout << code;
    }
    else
    {
        const bool manuallyCreated = isManuallyCreated(exp);
        if(canRemove(exp))
        {
            //*pout << "#" << getNameForObject(exp) << " of type " << exp.getType() << " is removed." << endl;
        }
        else
        {
            if (exp.isFratricidal())
            {
                assert(manuallyCreated); // doesn't make sense otherwise

                string deleteChildren = mScriptGen->DeleteChildren(getNameForObject(expParent));
                *pout << deleteChildren;
            }

            if (manuallyCreated)
            {
                string parentName = (&exp == &expParent) ? "" : getNameForObject(expParent);
                string createObjCode = mScriptGen->Create(strObjName, exp.getType(), parentName, VARIABLE);
                *pout << createObjCode;
            }
            else
            {   // Automatically-created Object
                autoCreatedIds.push_back(exp.getId());

                int iPosition = getPositionInParent(exp);

                string rightSide = mScriptGen->HandleForChildObject(
                    getNameForObject(expParent), exp.getType(), iPosition, VARIABLE);
                string autoCreated = mScriptGen->Assignment(strObjName, rightSide, "string", LITERAL);
                *pout << autoCreated;

                if(exp.isOnePropertyVisible())
                {
                    Properties props;
                    string configCode = mScriptGen->Config(strObjName, props, VARIABLE, VARIABLE, false);
                    *pout << configCode;
                }
            }
        }
    }
}

void ScriptExporter::saveResultQuery(Exportable& exp, Exportable& expParent)
{
    if ("ResultQuery" == exp.getType() &&
        "ResultDataSet" == expParent.getType() &&
        isResultSetSubscribed(expParent))
    {
        listResultQueryForRDS.push_back(&exp);
    }
    else if("ResultQuery" == exp.getType() &&
            "ResultChartQuerySet" == expParent.getType() &&
            isResultSetSubscribed(expParent))
    {
        listResultQueryForCharts.push_back(&exp);
    }

    // Save ID for later use when writing relationships
    mapIdToExportable[exp.getId()] = &exp;
}

void ScriptExporter::finishObjectProperties(Exportable& exp,
                                            const string& strPrefix)
{
    // If it's auto-created object and there were properties to configure,
    // close the config statement in some languages.
    if((exp.getType() == "StcSystem") ||
       (std::find(autoCreatedIds.begin(), autoCreatedIds.end(), exp.getId()) != autoCreatedIds.end() &&
        exp.isOnePropertyVisible()))
    {
        ostream& out = *pout;
        string closeConfig = mScriptGen->CloseConfig();
        out << closeConfig;
    }
}

void ScriptExporter::finishObject(Exportable& exp, const string& strPrefix)
{
    ostream& out = *pout;

    const bool removed = wasRemoved(exp);
    if (!removed && isManuallyCreated(exp))
    {
        string close = mScriptGen->CloseCreate();
        out << close;
    }

    if(!removed)
    {
        out << endl << endl;
    }

    pexpCurrent = NULL;
}

void ScriptExporter::startProperty(const string& strName,
                                   const string& strValue,
                                   const string& strPrefix)
{

    assert(pexpCurrent);
    const int iPropId = getPropertyIdForName(*pexpCurrent, strName);
    const string type = pexpCurrent->getPropertyTypeNameForId(iPropId);

    if(wasRemoved(*pexpCurrent))
    {
        return;
    }

    // Check if the property is a handle; if it is, it will
    // need to be translated to the deserialized object's new handle.
    if (type == "handle")
    {
        mapHandleProps[pexpCurrent].push_back(make_pair(iPropId, strValue));
    }
    else
    {
        string code;
        if (strName == "Location" && pexpCurrent->getType() == "Port")
        {
            // Don't load location for virtual ports
            if (isVirtualPort(*pexpCurrent))
                return;

            code = mScriptGen->PropertyLocation(iiPortLocationCurrent);
        }
        else
        {
            string myValue = strValue;
            ValueType_t vt;
            if(mScriptGen->GetVariableSigil() + pexpCurrent->getPropertyNameForId(iPropId) == strValue)
            {
                vt = VARIABLE;
            }
            else if(type == "string" || type == "inputFilePath" || type == "outputFilePath")
            {
                vt = STRING_LITERAL;                
            }
            else
            {
                vt = LITERAL;
                // I'm only keeping this here to satisfy unit tests. We don't need to escape
                // special chars any more, but we don't understand MockScriptable types.
                myValue = sanitizeString(myValue);
            }

            code = mScriptGen->Property(strName, myValue, vt);
        }

        *pout << code;
    }
}

void ScriptExporter::finishProperty(const string& strPrefix) {
}

void ScriptExporter::handleRelationship(Exportable& exp,
                                        const string& strType,
                                        bool bSourceToTarget,
                                        bool last,
                                        const string& strPrefix)
{
    // Save the relationship for output when all target handles are complete.
    Properties props;
    string relName = strType + "-";
    relName += ( bSourceToTarget ) ? "targets" : "sources";
    string strBaseAndType = mScriptGen->ConfigRelationship(getNameForObject(*pexpCurrent), relName);

    if (mapRelationships.find(strBaseAndType) == mapRelationships.end())
    {
        list<string> listEmpty;
        mapRelationships[strBaseAndType] = listEmpty;
        listRelationshipsOrdered.push_back(strBaseAndType);
    }

    // Add to map
    mapRelationships[strBaseAndType].push_back(exp.getId());
}

void ScriptExporter::writeTrailer()
{
    // Write out relationship-generating Tcl code
    if (listRelationshipsOrdered.size() > 0)
    {
        string comment = mScriptGen->Comment("Set up relationships");
        *pout << comment;

        list<string>::iterator iterRelationships;
        for (iterRelationships = listRelationshipsOrdered.begin();
             iterRelationships != listRelationshipsOrdered.end();
             iterRelationships++)
        {
            string strBaseAndType = *iterRelationships;
            vector<string> targets;
            list<string> listRelatives = mapRelationships[strBaseAndType];
            int ciRelatives = 0;
            list<string>::iterator iterRelatives;
            for (iterRelatives = listRelatives.begin();
                 iterRelatives != listRelatives.end();
                 iterRelatives++)
            {
                string strRelative = *iterRelatives;
                Exportable* expTarget = mapIdToExportable[strRelative];
                if (NULL != expTarget)
                {
                    string strSymbolicTarget = getNameForObject(*expTarget);
                    // If the object handle was removed, get the "get" handle code.
                    if(wasRemoved(*expTarget) &&
                       std::find(handlesCreatedIds.begin(),
                                 handlesCreatedIds.end(),
                                 expTarget->getId()) == handlesCreatedIds.end())
                    {
                        string handleGetCode = createHandleGetCodeForRemovedExportable(*expTarget);
                        *pout << handleGetCode;
                        handlesCreatedIds.push_back(expTarget->getId());
                    }

                    targets.push_back(strSymbolicTarget);
                    ciRelatives++;
                }
                else
                {
                    // TODO: Change to a log message
                    //cout << "-=-=- FAIL: Lookup of ID failed: " << strId << endl;
                }
            }

            // If any relatives remain after culling unserializable objects, write out the string
            if (ciRelatives > 0)
            {
                string relationCode = mScriptGen->ConnectRelationshipTargets(strBaseAndType, targets, VARIABLE);
                *pout << relationCode << endl;
            }
        }

        *pout << endl;
    }

    // Write out handle translation code
    writeHandleTranslation();

    // End loading from config.
    Properties props;
    vector<string> values;
    values.push_back("false");
    props["IsLoadingFromConfiguration"] = values;
    string endLoading = mScriptGen->Config("system1", props, STRING_LITERAL, STRING_LITERAL);
    *pout << endLoading << endl << endl;

    // Connect, reserve
    writeConnectReserveCode();

    // Write Launcher Script into separate file
    writeLauncherScript();

    std::vector<std::string> files;
    files.push_back(strFilename);
    files.push_back(strLogicFilename);

    Saver::setSavedFiles(files);
}

void ScriptExporter::writeHandleTranslation()
{
    if (mapHandleProps.empty())
        return;

    string comment = mScriptGen->Comment("Set up handles");
    *pout << comment;

    map<Exportable*, deque< pair<int, string> > >::const_iterator it;
    for (it = mapHandleProps.begin(); it != mapHandleProps.end(); ++it)
    {
        Exportable* obj = it->first;
        const string objName = getNameForObject(*obj);
        const deque< pair<int, string> >& props = it->second;
        deque< pair<int, string> >::const_iterator pit;
        for (pit = props.begin(); pit != props.end(); ++pit)
        {
            const int iPropId = pit->first;
            const std::string strVal = pit->second;
            const bool isCollection =  obj->isPropertyCollection(iPropId);

            vector<string> targets;

            const char DELIM = ' ';
            std::string::size_type curPos = 0;
            std::string::size_type prePos = 0;
            while (curPos != std::string::npos)
            {
                prePos = strVal.find_first_not_of(DELIM, curPos);
                if (prePos == std::string::npos)
                    break;

                curPos = strVal.find_first_of(DELIM, prePos);
                const std::string curId =
                    strVal.substr(prePos, curPos - prePos);

                // Lookup the mapped object
                map<std::string, Exportable*>::iterator iit =
                    mapIdToExportable.find(curId);
                if (iit != mapIdToExportable.end())
                {
                    Exportable& expTarget = *iit->second;
                    // If the object handle was removed, get the "get" handle code.
                    if(wasRemoved(expTarget) &&
                       std::find(handlesCreatedIds.begin(),
                                 handlesCreatedIds.end(),
                                 expTarget.getId()) == handlesCreatedIds.end())
                    {
                        string handleGetCode = createHandleGetCodeForRemovedExportable(expTarget);
                        *pout << handleGetCode;
                        handlesCreatedIds.push_back(expTarget.getId());
                    }

                    targets.push_back(getNameForObject(expTarget));
                }
                else
                {
                    // TODO: P2S3: Log error message
                }
            }

            // skip null for non-collection since it's invalid
            if (!isCollection && targets.empty())
            {
                continue;
            }

            ValueType_t valueType = VARIABLE;
            if (targets.empty())
            {
                assert(isCollection);
                targets.push_back("\"\""); //TODO Do we really need the statement at all?
                valueType = LITERAL;
            }
            Properties props;
            props[obj->getPropertyNameForId(iPropId)] = targets;
            string code = mScriptGen->Config(objName, props, VARIABLE, valueType);
            *pout << code << endl;
        }
    }

    *pout << endl;
}

/// \brief Open the file to hold the logic script
void ScriptExporter::openLogic()
{
    // Swap file pointers
    poutLauncher = pout;

    const string strLogicSuffix = "_logic";
    strLogicFilename = strFilename;

    string ext = Path::GetExtension(strFilename);
    if(!ext.empty())
    {
        strLogicFilename = StringUtil::Replace(strLogicFilename, ext, strLogicSuffix + ext);
    }
    else
    {
        strLogicFilename.append(strLogicSuffix);
    }

    // Open logic script, save in pout
    pout = new ofstream(strLogicFilename.c_str());
    if (!pout)
    {
        // TODO: P2S3: Report errors to caller
        cout << "-=-=- FAIL: Error opening file: " << strLogicFilename.c_str() << endl;
    }
}

void ScriptExporter::writeLauncherScript()
{
    const string time = CTimer::GetCurrentSystemDateTime();

    vector<string> portLocations;    
    portLocations.assign(listPortLocations.begin(), listPortLocations.end());   

    string code = mScriptGen->LauncherScript(time, Path::GetFileName(strFilename),
                                             Path::GetFileName(strLogicFilename), portLocations);
    *poutLauncher << code;
}

void ScriptExporter::closeOutput()
{
    AbstractRecursiveFileExporter::closeOutput();
    if (strFilename != "")
    {
        if (NULL != poutLauncher)
        {
            if (ofstream* pofs = dynamic_cast<ofstream*>(poutLauncher))
            {
                pofs->close();
                delete poutLauncher;
                poutLauncher = NULL;
            }
        }
    }
}

int ScriptExporter::getPositionInParent(Exportable& exp)
{
    int iiInParent = 0;

    // Record type to match
    string strType = exp.getType();

    // Get Parent
    Exportable* pexpParent = exp.getParent();

    if (!pexpParent)
    {
        // No parent, return 0 by convention
        return 0;
    }

    // Get Children
    vector<Exportable*> vecChildren = pexpParent->getChildren();

    // Iterate counting those of the same type
    vector<Exportable*>::iterator iterChildren = vecChildren.begin();
    while (iterChildren != vecChildren.end())
    {
        Exportable* pexpChild = *iterChildren;

        if (pexpChild->getId() == exp.getId())
        {
            break;
        }

        if (strType == pexpChild->getType())
        {
            iiInParent++;
        }

        iterChildren++;
    }

    // Return computed index
    return iiInParent;
}

void ScriptExporter::createCodeToGetHandleForObject(Exportable& exp, string& strCode)
{
    strCode = "";

    // Get type
    string strType = exp.getType();

    // Check that we're not at the root
    if (NULL != exp.getParent())
    {
        // Get zero-based position
        int iPosition = getPositionInParent(exp);

        // Get Dan string for parents
        createDanForObject(*(exp.getParent()), strCode);

        // Add prefix and suffix
        strCode = mScriptGen->HandleForChildObject(strCode, strType, iPosition, STRING_LITERAL);
    }
    else
    {   // We _are_ StcSystem
        strCode = "system1";
    }
}

void ScriptExporter::createDanForObject(Exportable& exp, string& strCode)
{
    // Get type
    string strType = exp.getType();

    // Check that we're not at the root
    if (NULL != exp.getParent())
    {
        // Get zero-based position
        int iPosition = getPositionInParent(exp);

        // Add code for this
        strCode = mScriptGen->DanForObject(strCode, strType, (iPosition + 1));

        // Get code for parents, recursively
        createDanForObject(*(exp.getParent()), strCode);
    }
    else
    {   // We _are_ StcSystem
        strCode = "system1" + strCode;
    }
}

/**
 * Lookup the numeric ID for a property.
 *
 * @return The property ID, or -1 if none is found
 */
int ScriptExporter::getPropertyIdForName(Exportable& exp, const string strPropertyName)
{
    int id = -1;

    // Get all property ids
    vector<int> vecIds = exp.getPropertyIds();

    // For each, get the name
    vector<int>::iterator iterIds;
    for (iterIds = vecIds.begin();
         iterIds != vecIds.end();
         iterIds++)
    {
        // If there's a match, store and return
        if (strPropertyName == exp.getPropertyNameForId(*iterIds))
        {
            id = *iterIds;
            break;
        }
    }

    return id;
}

string ScriptExporter::getCsvFileName(string resultTypeStr, int iterCount)
{
    char tmpStr[8];
    sprintf(tmpStr, "%04d", iterCount);
    string iter(tmpStr);
    string csvFilename = Path::GetFileNameWithoutExtension(strFilename) + "-" + iter + "-" + resultTypeStr;
    return csvFilename;
}

string ScriptExporter::getSpecialPropertyValue(Exportable& exp,int propId)
{
    string type = exp.getPropertyTypeNameForId(propId);
    if(isFilePathReplacementEnabled() &&
      (type.compare("inputFilePath") == 0 || type.compare("outputFilePath") == 0))
    {
       string varName = mScriptGen->GetVariableSigil() + exp.getPropertyNameForId(propId);
       return varName;
    }
    else
    {
        return AbstractRecursiveExporter::getSpecialPropertyValue(exp,propId);
    }
}

void ScriptExporter::writeFilePathSubstitution(Exportable& exp)
{
    if(isFilePathReplacementEnabled())
    {
        vector<int> ids = exp.getPropertyIds();
        vector<int>::iterator it = ids.begin();
        for(; it != ids.end(); ++it)
        {
            int propId = (*it);
            if(exp.isPropertyVisible(propId) && !exp.isPropertyDeprecated(propId))
            {
                string type = exp.getPropertyTypeNameForId(propId);
                if (type.compare("inputFilePath") == 0 || type.compare("outputFilePath") == 0)
                {
                    string varName = exp.getPropertyNameForId(propId);
                    string filename = exp.getPropertyValueForId(propId);
                    string code = mScriptGen->FilePathSubstitution(varName, filename);
                    *pout << code;
                }
            }
        }
    }
}

bool ScriptExporter::isResultSetSubscribed(Exportable& exp)
{
    assert(exp.getType() == "ResultDataSet" || exp.getType() == "ResultChartQuerySet");

    bool subscribed = false;
    int resultStateId = getPropertyIdForName(exp, "ResultState");
    string resultState = exp.getPropertyValueForId(resultStateId);
    if(resultState == "SUBSCRIBED")
    {
        subscribed = true;
    }

    return subscribed;

}

void ScriptExporter::initScriptCodeGen()
{
    if(mScriptGen)
    {
        delete mScriptGen;
        mScriptGen = NULL;
    }

    switch (options.language)
    {
        case TCL:
            mScriptGen = new TclCodeGen();
            break;
        case RUBY:
            //TODO
            throw CStcRunTimeError("Ruby not yet handled.");
            break;
        case PERL:
            mScriptGen = new PerlCodeGen();
            break;
        case PYTHON:
            mScriptGen = new PythonCodeGen();
            break;
        default:
           ostringstream err;
           err << "Language option " << options.language << " not understood.";
           throw CStcRunTimeError(err.str());
    }

    if(options.indent.empty())
    {
        options.indent = "    ";
    }

    options.includeConfigCustom |= ConfigHasExposedProperties();

    ScriptCommentProvider commentProvider(options, strFilename, strLogicFilename, CTimer::GetCurrentSystemDateTime());
    mScriptGen->SetScriptCommentProvider(commentProvider);
    mScriptGen->SetOptions(options);
}

bool ScriptExporter::isSourceOrDestForRelationship(Exportable& exp)
{
    if(exp.isSuitableForExport())
    {
        set<int> relTypes = exp.getRelationshipTypes();
        set<int>::iterator it = relTypes.begin();
        for(; it != relTypes.end(); ++it)
        {
            vector<Exportable*> rels = exp.getRelatives(*it, true);
            vector<Exportable*>::iterator it_rel = rels.begin();
            for(; it_rel != rels.end(); ++it_rel)
            {
                if((*it_rel)->isSuitableForExport())
                {
                    return true;
                }
            }

            rels = exp.getRelatives(*it, false);
            for(it_rel = rels.begin(); it_rel != rels.end(); ++it_rel)
            {
                if((*it_rel)->isSuitableForExport())
                {
                    return true;
                }
            }
        }
    }

    return false;
}

string ScriptExporter::createHandleGetCodeForRemovedExportable(Exportable& exp)
{
    ostringstream code;
    vector<Exportable*> parents;
    Exportable* pexp = &exp;
    string type = pexp->getType();
    do
    {
        parents.push_back(pexp);
        pexp = pexp->getParent();
        type = pexp->getType();
    }
    while(wasRemoved(*pexp) && type != "StcSystem" && type != "Project");

    vector<Exportable*>::reverse_iterator rit = parents.rbegin();
    for(; rit < parents.rend(); ++rit)
    {
        Exportable& curExp = **rit;
        string parentHandle = getNameForObject(*curExp.getParent());
        ValueType_t valueType = ( parentHandle == "system1" ) ? STRING_LITERAL : VARIABLE;
        string rightSide = mScriptGen->HandleForChildObject(parentHandle,
                                                            curExp.getType(),
                                                            getPositionInParent(curExp),
                                                            valueType);
        code << mScriptGen->Assignment(getNameForObject(curExp), rightSide, "string", LITERAL);
    }

    return code.str();
}

bool ScriptExporter::isCandidateForRemoval(Exportable& exp)
{
    if(removalCandidates.empty())
    {
        // This is a list of types that are "potential" candidates for removal from the generated script
        // during a compact ( lossy ) save.
        // ALL subtypes are included.
        // To consider a type to be added to this list, the type MUST meet the following criteria:
        // 1) The instance of the type must be known to be auto-magically created by
        //    it's parent, grandparent, loaded from XML object graph, etc.
        // 2) The type must have instance level default values. For example, MiiRegister has default values
        //    depending on it's instance. DeviceAddrOptions has class level defaults, i.e. it has a 1-1 relation
        //    to Project and will only have those default property values defined in the meta data.
        //    The latter type cannot be added to this list.
        // 3) All subtypes meet the above criteria.
        // 4) Test cases must be added to testCompactg2s.tcl including all subtypes.

        string typeArr[] = { "HistogramConfig", "ManagementRegister", "ManagementDevice",
                             "FrameLengthDistribution", "FrameLengthDistributionSlot"
        };

        removalCandidates.assign(typeArr, typeArr + sizeof(typeArr)/sizeof(typeArr[0]));
    }

    bool candidate = false;
    ScriptableImexAdapter* imex = dynamic_cast<ScriptableImexAdapter*>(&exp);
    if(imex)
    {
        CScriptable* s = imex->getScriptable();
        vector<string>::iterator it = removalCandidates.begin();
        for(; it != removalCandidates.end(); ++it)
        {
            const ClassId classId = CMetaClassManager::Instance().GetClass(*it)->GetClassId();
            if(s->IsTypeOf(classId))
            {
                candidate = true;
                break;
            }
        }
    }

    return candidate;
}

bool ScriptExporter::ConfigHasExposedProperties()
{
    CScriptable* exposedConfig = CProject::Instance().GetObject("ExposedConfig");
    if(exposedConfig == 0)
    {
        return false;
    }

    ScriptableVec exposedProperties;
    exposedConfig->GetObjects(exposedProperties, "ExposedProperty");
    return (exposedProperties.empty() == false);
}

