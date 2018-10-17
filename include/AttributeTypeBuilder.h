#ifndef INCLUDED_ATTRIBUTE_TYPE_BUILDER_H
#define INCLUDED_ATTRIBUTE_TYPE_BUILDER_H
#pragma once

#include "MetaInfo.h"
#include "StringUtil.h"

namespace stc {
namespace framework {

template<class T>
class AttributeBuilderT : public AttributeType::Builder 
{
public:
	AttributeTypeId GetId() const 
	{
		return T::GetID();
	}

	AttributeType* operator ()(const char* n, const char* s)
	{
		typename T::VALUE_TYPE v;
		ConvertToType(v, s);
		return new T(v, n);
	}
};
}
}

#endif

