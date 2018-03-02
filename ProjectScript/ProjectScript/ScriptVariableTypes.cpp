#include "ScriptVariableTypes.h"

ScriptTypeDataOld _svt_get(int i) { return INTEGER_SIGNED; }
ScriptTypeDataOld _svt_get(unsigned int ui) { return INTEGER_UNSIGNED; }
ScriptTypeDataOld _svt_get(float f) { return FLOAT; }

ScriptTypeDataOld::ScriptTypeDataOld() {}

ScriptTypeDataOld::ScriptTypeDataOld(ScriptVariableType t)
{
	type = t;
}
