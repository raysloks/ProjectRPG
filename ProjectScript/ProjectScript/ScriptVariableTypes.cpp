#include "ScriptVariableTypes.h"

ScriptTypeDataOld _svt_get(int i) { return SVT_INTEGER_SIGNED; }
ScriptTypeDataOld _svt_get(unsigned int ui) { return SVT_INTEGER_UNSIGNED; }
ScriptTypeDataOld _svt_get(uint64_t ui) { return SVT_INTEGER_UNSIGNED; }
ScriptTypeDataOld _svt_get(float f) { return SVT_FLOAT; }

ScriptTypeDataOld::ScriptTypeDataOld() {}

ScriptTypeDataOld::ScriptTypeDataOld(ScriptVariableType t)
{
	type = t;
}
