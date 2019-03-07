#include "ScriptVariableTypes.h"

template <>
ScriptTypeDataOld _svt_get<int64_t>() { return SVT_INTEGER_SIGNED; }
template <>
ScriptTypeDataOld _svt_get<uint64_t>() { return SVT_INTEGER_UNSIGNED; }

ScriptTypeDataOld::ScriptTypeDataOld() {}

ScriptTypeDataOld::ScriptTypeDataOld(ScriptVariableType t)
{
	type = t;
}
