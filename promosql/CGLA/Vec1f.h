#ifndef __VEC1F_H
#define __VEC1F_H

#include "ArithVec.h"

namespace CGLA {

	class Vec1f: public ArithVec<float,Vec1f,1>
	{
	public:
  
		Vec1f() {}

		Vec1f(float _a): ArithVec<float,Vec1f,1>(_a) {}

	};
}
#endif

