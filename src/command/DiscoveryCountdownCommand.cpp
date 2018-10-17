#include "StdAfx.h"

#include "DiscoveryCountdownCommand.h"
#include "DiscoveryCountdownCommand_AutoGen.cpp"
#include "ChartPoint.h"

#include <sstream>

using namespace stc::framework;

CDiscoveryCountdownCommand::CDiscoveryCountdownCommand() {}
CDiscoveryCountdownCommand::~CDiscoveryCountdownCommand() {}

void CDiscoveryCountdownCommand::DoRun()
{
	std::ostringstream os;

	std::vector<ChartPoint> pointVec;
	const int updateInterval = 2;

	for (uint32_t i = GetCountdown(); i > 0; --i)
	{
		os.str("");
		os << "T-Minus " << i << " seconods";
		SetStatus(os.str().c_str());

		pointVec.push_back(ChartPoint(i, i));

		if (i % updateInterval == 0)
		{
			GetFlightDataStream().Append(pointVec);
			pointVec.clear();
		}

		for (int j = 0; j < 50; ++j)
		{
			CTaskManager::Instance().CtmYield();
		}

		CTaskManager::Instance().CtmYield(1000);

		for (int j = 0; j < 50; ++j)
		{
			CTaskManager::Instance().CtmYield();
		}

		// random failure base on success rate
		if (GetSuccessRate() < ((double)(rand()) / RAND_MAX * 100.0))
		{
			os.str("");
			os << "Countdown aborted due to failure";
			SetStatus(os.str().c_str());

			throw CCommandErrorException();
		}
	}

	os.str("");
	os << "T-Minus 0";
	SetStatus(os.str().c_str());

	// Flush remaining data point
	pointVec.push_back(ChartPoint(0, 0));
	GetFlightDataStream().Append(pointVec);
	pointVec.clear();
}

