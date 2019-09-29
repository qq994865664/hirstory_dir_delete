#include "KeyValue.h"

CKeyValue::CKeyValue(int a, string b):first(a), second(b)
{

}

CKeyValue::~CKeyValue()
{

}

bool CKeyValue::less_first(const CKeyValue& m1, const CKeyValue& m2)
{
	return m1.first > m2.first;
}
