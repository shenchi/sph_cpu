#pragma once

#include "vector.h"
#include "grid.h"

namespace SPH{

	struct ParticleSystem{

		int max_particles;
		int n_particles;

		float smooth_radius;
		float smooth_radius_sqr;
		float k;
		float miu;
		float rhou0;
		float mass;
		float density_factor;
		float pressure_factor;
		float viscosity_factor;

		float3 *pos;
		float3 *vel;

		float3 *swap_pos;
		float3 *swap_vel;

		float3 *acc;

		float *density;
		float *pressure;
		float *volume;

	};

	ParticleSystem *CreateParticleSystem(int max_particles, float smooth_radius, float k, float miu, float rhou0);

	void DestroyParticleSystem(ParticleSystem *p);
	
	void LoadParticles(ParticleSystem *p, float3 *pos, float mass, int n_particles);

};