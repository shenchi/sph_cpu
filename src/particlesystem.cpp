#include "ParticleSystem.h"
#include <cstring>

namespace SPH{
	
#ifndef M_PI
	const float M_PI = 3.1415926f;
#endif

	ParticleSystem *CreateParticleSystem(int max_particles, float smooth_radius, float k, float miu, float rhou0){
		ParticleSystem *p = new ParticleSystem;
		p->max_particles = max_particles;
		p->n_particles = 0;

		p->smooth_radius = smooth_radius;
		p->smooth_radius_sqr = smooth_radius * smooth_radius;

		p->k = k;
		p->miu = miu;
		p->rhou0 = rhou0;

		p->density_factor = 315.0f / 64.0f / M_PI / pow(smooth_radius, 9);
		p->pressure_factor = - 45.0f / M_PI / pow(smooth_radius, 6);
		p->viscosity_factor = - p->pressure_factor;

		p->pos = new float3[max_particles];
		p->vel = new float3[max_particles];
		p->swap_pos = new float3[max_particles];
		p->swap_vel = new float3[max_particles];

		p->acc = new float3[max_particles];

		p->density = new float[max_particles];
		p->pressure = new float[max_particles];
		p->volume = new float[max_particles];

		return p;
	}

	void DestroyParticleSystem(ParticleSystem *p){
		delete [] p->pos;
		delete [] p->vel;
		delete [] p->swap_pos;
		delete [] p->swap_vel;
		delete [] p->acc;
		delete [] p->density;
		delete [] p->pressure;
		delete [] p->volume;
		delete p;
	}

	void LoadParticles(ParticleSystem *p, float3 *pos, float mass, int n_particles){
		if( n_particles > p->max_particles ) return;

		p->n_particles = n_particles;
		p->mass = mass;

		memcpy(p->pos, pos, sizeof(float3) * n_particles);
		memset(p->vel, 0, sizeof(float3) * n_particles);
	}
	
};