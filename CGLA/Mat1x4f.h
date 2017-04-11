#ifndef __MAT1X4F_H
#define __MAT1X4F_H

// Author: Dimitrios Touloumis, derived from Mat3x4f.h
// Original 3x4f By: J. Andreas Bærentzen,
// Created: Mon Sep 35 11:55:4

#include "Vec1f.h"
#include "Vec4f.h"
#include "ArithMat.h"

namespace CGLA
{

	/**  1x4 float matrix class.
	*/
	class Mat1x4f: public ArithMat<Vec1f, Vec4f, Mat1x4f, 1>
	{

	public:
		/// Construct Mat1x4f from a single Vec4f vectors (vectors become rows)
		Mat1x4f(const Vec4f& _a): 
			ArithMat<Vec1f, Vec4f, Mat1x4f, 1> (_a) {}

		/// Construct 0 matrix.
		Mat1x4f() {}

		/// Construct matrix from array of values.
		Mat1x4f(const float* sa): ArithMat<Vec1f, Vec4f, Mat1x4f, 1> (sa) {}
	};

	/**  4x1 float matrix class.
			 This class is useful for going from plane to 4D coordinates.
	*/
	class Mat4x1f: public ArithMat<Vec4f, Vec1f, Mat4x1f, 4>
	{

	public:

		/** Construct matrix from four Vec1f vectors which become the 
				rows of the matrix. */
		Mat4x1f(const Vec1f& _a, const Vec1f& _b, const Vec1f& _c, const Vec1f& _d): 
			ArithMat<Vec4f, Vec1f, Mat4x1f, 4> (_a,_b,_c,_d) {}

		/// Construct 0 matrix.
		Mat4x1f() {}

		/// Construct matrix from array of values.
		Mat4x1f(const float* sa): ArithMat<Vec4f, Vec1f, Mat4x1f, 4> (sa) {}

	};


}
#endif
