#pragma once

#include <iostream>
#include <algorithm>
#include <functional>
#include <vector>
using namespace std;

class CKeyValue
{
public:
	CKeyValue(int a, string b);
	~CKeyValue();

    long long first;
	string second;
    bool operator < (const CKeyValue &m)const 
	{
         return first < m.first;
    }
	static bool less_first(const CKeyValue& m1, const CKeyValue& m2);
};
