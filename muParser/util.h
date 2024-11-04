#pragma once
#ifndef UTIL_H
#define UTIL_H
#include <map>
#include <list>
#include <set>
#include <vector>
#include <memory>
#include "muParser.h"
namespace QuanSystem
{
	using Value = mu::value_type;
	using String = mu::string_type;
	using Param = Value*;
	using ParamVec = std::vector<Param>;
	using ValueVec = std::vector<Value>;

	struct Func;
	using FuncVec = std::vector<Func*>;

	struct RPoint;
	using RPointList = std::list<RPoint>;
	using RPointListIter = std::list<RPoint>::iterator;

	using ID = int;
	using ConstraintID = int;
	using IDVec = std::vector<ID>;
	using IDSet = std::set<ID>;

	using StringMapParam = std::map<String, Param>;
	using ParamMapParam = std::map<Param, Param>;
	using ParamMapValue = std::map<Param, Value>;
	using ParamMapID = std::map<Param, ID>;
	using IDMapParam = std::map<ID, Param>;
	using ParamSet = std::set<Param>;

}
#endif // UTIL_H
