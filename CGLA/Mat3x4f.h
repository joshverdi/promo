#ifndef __MAT3X4F_H
#define __MAT3X4F_H
// Author: Dimitrios Touloumis, derived from Mat2x3f.h
// Original 3x4f By: J. Andreas Bærentzen,
// Created: Mon Sep 35 11:55:4

#include "Vec3f.h"
#include "Vec4f.h"
#include "ArithMat.h"

namespace CGLA
{

	/**  3x4 float matrix class.
			 This class is useful for projecting a vector from 4D space to 3D.
	*/
	class Mat3x4f: public ArithMat<Vec3f, Vec4f, Mat3x4f, 3>
	{

	public:
		/// Construct Mat3x4f from three Vec4f vectors (vectors become rows)
		Mat3x4f(const Vec4f& _a, const Vec4f& _b, const Vec4f& _c): 
			ArithMat<Vec3f, Vec4f, Mat3x4f, 3> (_a,_b,_c) {}

		/// Construct 0 matrix.
		Mat3x4f() {}

		/// Construct matrix from array of values.
		Mat3x4f(const float* sa): ArithMat<Vec3f, Vec4f, Mat3x4f, 3> (sa) {}
	};

	/**  4x3 float matrix class.
			 This class is useful for going from plane to 4D coordinates.
	*/
	class Mat4x3f: public ArithMat<Vec4f, Vec3f, Mat4x3f, 4>
	{

	public:

		/** Construct matrix from four Vec3f vectors which become the 
				rows of the matrix. */
		Mat4x3f(const Vec3f& _a, const Vec3f& _b, const Vec3f& _c, const Vec3f& _d): 
			ArithMat<Vec4f, Vec3f, Mat4x3f, 4> (_a,_b,_c,_d) {}

		/// Construct 0 matrix.
		Mat4x3f() {}

		/// Construct matrix from array of values.
		Mat4x3f(const float* sa): ArithMat<Vec4f, Vec3f, Mat4x3f, 4> (sa) {}

	};


}
#endif
