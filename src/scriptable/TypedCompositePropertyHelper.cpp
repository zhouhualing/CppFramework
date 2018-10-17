#include "StdAfx.h"
#include "TypedCompositePropertyHelper.h"
#include "CompositePropertyManager.h"
#include "ChassisSlotPort.h"
#include "ChartPoint.h"

using namespace std;
using namespace stc::framework;

template<typename T>
void stc::framework::composite::HandleSet(stc::framework::CScriptable* s, stc::framework::PropId propId, T val)
{
    CCompositePropertyManager::Instance().HandleSet<T>(s, propId, val);
}

template<typename T>
T stc::framework::composite::HandleGet(const stc::framework::CScriptable* s, stc::framework::PropId propId)
{
    return CCompositePropertyManager::Instance().HandleGet<T>(s, propId);
}

bool stc::framework::composite::IsApplicable(const stc::framework::CScriptable* s, stc::framework::PropId propId)
{
    return CCompositePropertyManager::Instance().IsApplicable(s, propId);
}

bool stc::framework::composite::IsCompositeProperty(stc::framework::PropId propId)
{
    return CCompositePropertyManager::Instance().IsCompositeProperty(propId);
}

template STC_PUBLIC void stc::framework::composite::HandleSet(CScriptable*, PropId, string);
template STC_PUBLIC void stc::framework::composite::HandleSet(CScriptable*, PropId, bool);
template STC_PUBLIC void stc::framework::composite::HandleSet(CScriptable*, PropId, uint8_t);
template STC_PUBLIC void stc::framework::composite::HandleSet(CScriptable*, PropId, uint16_t);
template STC_PUBLIC void stc::framework::composite::HandleSet(CScriptable*, PropId, uint32_t);
template STC_PUBLIC void stc::framework::composite::HandleSet(CScriptable*, PropId, uint64_t);
template STC_PUBLIC void stc::framework::composite::HandleSet(CScriptable*, PropId, int8_t);
template STC_PUBLIC void stc::framework::composite::HandleSet(CScriptable*, PropId, int16_t);
template STC_PUBLIC void stc::framework::composite::HandleSet(CScriptable*, PropId, int32_t);
template STC_PUBLIC void stc::framework::composite::HandleSet(CScriptable*, PropId, int64_t);
template STC_PUBLIC void stc::framework::composite::HandleSet(CScriptable*, PropId, double);
template STC_PUBLIC void stc::framework::composite::HandleSet(CScriptable*, PropId, CIpAddress);
template STC_PUBLIC void stc::framework::composite::HandleSet(CScriptable*, PropId, CIpv6Address);
template STC_PUBLIC void stc::framework::composite::HandleSet(CScriptable*, PropId, CMacAddress);
template STC_PUBLIC void stc::framework::composite::HandleSet(CScriptable*, PropId, InputFilePath);
template STC_PUBLIC void stc::framework::composite::HandleSet(CScriptable*, PropId, OutputFilePath);
template STC_PUBLIC void stc::framework::composite::HandleSet(CScriptable*, PropId, CWorldWideName);
template STC_PUBLIC void stc::framework::composite::HandleSet(CScriptable*, PropId, CChassisSlotPort);
template STC_PUBLIC void stc::framework::composite::HandleSet(CScriptable*, PropId, ChartPoint);
template STC_PUBLIC void stc::framework::composite::HandleSet(CScriptable*, PropId, PropId);
template STC_PUBLIC void stc::framework::composite::HandleSet(CScriptable*, PropId, ClassId);
template STC_PUBLIC void stc::framework::composite::HandleSet(CScriptable*, PropId, EventMessage);

template STC_PUBLIC string stc::framework::composite::HandleGet(const CScriptable*, PropId);
template STC_PUBLIC bool stc::framework::composite::HandleGet(const CScriptable*, PropId);
template STC_PUBLIC uint8_t stc::framework::composite::HandleGet(const CScriptable*, PropId);
template STC_PUBLIC uint16_t stc::framework::composite::HandleGet(const CScriptable*, PropId);
template STC_PUBLIC uint32_t stc::framework::composite::HandleGet(const CScriptable*, PropId);
template STC_PUBLIC uint64_t stc::framework::composite::HandleGet(const CScriptable*, PropId);
template STC_PUBLIC int8_t stc::framework::composite::HandleGet(const CScriptable*, PropId);
template STC_PUBLIC int16_t stc::framework::composite::HandleGet(const CScriptable*, PropId);
template STC_PUBLIC int32_t stc::framework::composite::HandleGet(const CScriptable*, PropId);
template STC_PUBLIC int64_t stc::framework::composite::HandleGet(const CScriptable*, PropId);
template STC_PUBLIC double stc::framework::composite::HandleGet(const CScriptable*, PropId);
template STC_PUBLIC CIpAddress stc::framework::composite::HandleGet(const CScriptable*, PropId);
template STC_PUBLIC CIpv6Address stc::framework::composite::HandleGet(const CScriptable*, PropId);
template STC_PUBLIC CMacAddress stc::framework::composite::HandleGet(const CScriptable*, PropId);
template STC_PUBLIC InputFilePath stc::framework::composite::HandleGet(const CScriptable*, PropId);
template STC_PUBLIC OutputFilePath stc::framework::composite::HandleGet(const CScriptable*, PropId);
template STC_PUBLIC CWorldWideName stc::framework::composite::HandleGet(const CScriptable*, PropId);
template STC_PUBLIC CChassisSlotPort stc::framework::composite::HandleGet(const CScriptable*, PropId);
template STC_PUBLIC ChartPoint stc::framework::composite::HandleGet(const CScriptable*, PropId);
template STC_PUBLIC PropId stc::framework::composite::HandleGet(const CScriptable*, PropId);
template STC_PUBLIC ClassId stc::framework::composite::HandleGet(const CScriptable*, PropId);
template STC_PUBLIC EventMessage stc::framework::composite::HandleGet(const CScriptable*, PropId);

