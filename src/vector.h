#pragma once

#include <math.h>

namespace SPH{
	
	template<typename T>
	struct vector{
		T x, y, z;
	};

	template<typename T>
	inline vector<T> make_vector(T x, T y, T z){
		vector<T> r;
		r.x = x;
		r.y = y;
		r.z = z;
		return r;
	}

	typedef vector<float> float3;
	typedef vector<int> int3;

	inline float3 make_float3(float x, float y, float z){
		float3 r;
		r.x = x;
		r.y = y;
		r.z = z;
		return r;
	}

	inline float3 make_float3(int3 v){
		return make_float3((float)v.x, (float)v.y, (float)v.z);
	}

	inline int3 make_int3(int x, int y, int z){
		int3 r;
		r.x = x;
		r.y = y;
		r.z = z;
		return r;
	}

	inline int3 make_int3(float3 v){
		return make_int3((int)v.x, (int)v.y, (int)v.z);
	}

	template<typename T>
	inline const vector<T> operator + (const vector<T> &op1, const vector<T> &op2){
		return make_vector(op1.x + op2.x, op1.y + op2.y, op1.z + op2.z);
	}

	template<typename T>
	inline const vector<T> operator - (const vector<T> &op1, const vector<T> &op2){
		return make_vector(op1.x - op2.x, op1.y - op2.y, op1.z - op2.z);
	}

	template<typename T>
	inline const vector<T> operator * (const vector<T> &op1, const vector<T> &op2){
		return make_vector(op1.x * op2.x, op1.y * op2.y, op1.z * op2.z);
	}

	template<typename T>
	inline const vector<T> operator * (const vector<T> &op1, T op2){
		return make_vector(op1.x * op2, op1.y * op2, op1.z * op2);
	}

	template<typename T>
	inline const vector<T> operator * (T op1, const vector<T> &op2){
		return make_vector(op1 * op2.x, op1 * op2.y, op1 * op2.z);
	}

	template<typename T>
	inline T operator * (const vector<T> &op1, const vector<T> &op2){
		return op1.x * op2.x + op1.y * op2.y + op1.z * op2.z;
	}

	inline float operator * (const float3 &op1, const float3 &op2){
		return op1.x * op2.x + op1.y * op2.y + op1.z * op2.z;
	}

	inline float len(const float3 &v){
		return sqrt( v.x * v.x + v.y * v.y + v.z * v.z );
	}

	inline const float3 & normalize(float3 &v){
		v = v * (1 / len(v));
		return v;
	}
	
	inline const float3 normal(const float3 &v){
		return v * (1 / len(v)) ;
	}

};