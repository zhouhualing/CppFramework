#include "StdAfx.h"

#include "UpdateLicenseServerInfoCommand.h"
#include "UpdateLicenseServerInfoCommand_AutoGen.cpp"

#include "LicenseServer.h"

using namespace stc::framework;

CUpdateLicenseServerInfoCommand::CUpdateLicenseServerInfoCommand()
{
}

CUpdateLicenseServerInfoCommand::~CUpdateLicenseServerInfoCommand()
{
}

/**
 * \brief
 * Updates and persists license server information
 *
 * \remarks
 * The purpose of this command is simply to provide BLL clients (UI/automation)
 * with the ability to update and persist license server information.  The UI
 * provides the ability to sort and reorder the license servers for redundancy.
 * Since this involves reordering relations in the BLL, one alternative would be
 * to handle this in OnRelationModified. However, the ReorderRelations command
 * will silently update the data store to avoid triggering the delegates and
 * for good reason because it could potentially result in a flood of
 * notifications since all relations are first deleted and then added in the
 * appropriate order.
 */
void CUpdateLicenseServerInfoCommand::DoRun()
{
    CLicenseServer::UpdateEnvVariable();
}

