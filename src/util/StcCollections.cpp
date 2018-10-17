#include "StdAfx.h"
#include "StcCollections.h"

using namespace stc::framework;

ScriptableVec::ScriptableVec()
{
}
ScriptableVec::ScriptableVec(size_type count) : _base(count)
{
}
ScriptableVec::ScriptableVec(const ScriptableVec& op) : _base(op)
{
}
ScriptableVec::ScriptableVec(size_type count, CScriptable *pVal) : _base(count, pVal)
{
}
