#ifndef __MAT1X3F_H
#define __MAT1X3F_H

// Author: Dimitrios Touloumis, derived from Mat2x3f.h
// Original 3x4f By: J. Andreas Bærentzen,
// Created: Mon Sep 35 11:55:4

#include "Vec1f.h"
#include "Vec3f.h"
#include "ArithMat.h"

namespace CGLA
{

	/**  1x3 float matrix class.
	*/
	class Mat1x3f: public ArithMat<Vec1f, Vec3f, Mat1x3f, 1>
	{

	public:
		/// Construct Mat1x3f from a single Vec3f vectors (vectors become rows)
		Mat1x3f(const Vec3f& _a): 
			ArithMat<Vec1f, Vec3f, Mat1x3f, 1> (_a) {}

		/// Construct 0 matrix.
		Mat1x3f() {}

		/// Construct matrix from array of values.
		Mat1x3f(const float* sa): ArithMat<Vec1f, Vec3f, Mat1x3f, 1> (sa) {}
	};

	/**  3x1 float matrix class.
	*/
	class Mat3x1f: public ArithMat<Vec3f, Vec1f, Mat3x1f, 3>
	{

	public:

		/** Construct matrix from three Vec1f vectors which become the 
				rows of the matrix. */
		Mat3x1f(const Vec1f& _a, const Vec1f& _b, const Vec1f& _c): 
			ArithMat<Vec3f, Vec1f, Mat3x1f, 3> (_a,_b,_c) {}

		/// Construct 0 matrix.
		Mat3x1f() {}

		/// Construct matrix from array of values.
		Mat3x1f(const float* sa): ArithMat<Vec3f, Vec1f, Mat3x1f, 3> (sa) {}

	};


}
#endif
