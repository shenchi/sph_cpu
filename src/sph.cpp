#include "sph.h"
#include <cstdio>

namespace SPH{
	
	void density(ParticleSystem *p, Grid *g){

		for(int i = 0; i < p->n_particles; i++){
			
			p->density[i] = pow(p->smooth_radius, 6);

			for(int ni = g->nb_start[i]; ni < g->nb_end[i]; ni++){
				int j = g->nb[ni];

				float3 r = p->pos[j] - p->pos[i];
				float r_sqr = r * r;

				p->density[i] += pow(p->smooth_radius_sqr - r_sqr, 3);
			}

			p->density[i] *= p->mass * p->density_factor;
			p->pressure[i] = p->k * (pow(p->density[i] / p->rhou0, 4) - 1);
			if(p->pressure[i] < 0.0f) p->pressure[i] = 0.0f;
			p->volume[i] = p->mass / p->density[i];
		}

	}

	float3 gravity = make_float3(0, -9.8f, 0);

	void forces(ParticleSystem *p, Grid *g){
		
		float mass_reverse = 1 / p->mass;

		for(int i = 0; i < p->n_particles; i++){
			
			float3 fp = make_float3(0, 0, 0);
			float3 fv = make_float3(0, 0, 0);

			for(int ni = g->nb_start[i]; ni < g->nb_end[i]; ni++){
				int j = g->nb[ni];

				float3 r = p->pos[j] - p->pos[i];
				float dist = len(r);

				float h_r = p->smooth_radius - dist;
				float vivj = p->volume[i] * p->volume[j];
				
				fp = fp + r * (1 / dist) * vivj * 0.5f * (p->pressure[i] + p->pressure[j]) * (float)pow(h_r, 3);
				fv = fv + (p->vel[j] - p->vel[i]) * vivj * p->miu  * h_r;

			}

			fp = fp * p->pressure_factor;
			fv = fv * p->viscosity_factor;

			p->acc[i] = (fp + fv) * mass_reverse + gravity;
			//p->acc[i] = (fv) * mass_reverse + gravity;
			//p->acc[i] = gravity;
		}

	}

	const float dumping = 0.6f;
	inline void collision_plane(float3 normal, float d, float3 &pos, float3 &vel){
		float dist = normal * pos + d;

		if( dist > 0 ) return;

		pos = pos - 2.0f * dist * normal;
		vel = vel - dumping * 2.0f * vel * normal * normal;
	}

	const float3 up = make_float3(0, 1, 0);
	const float3 down = make_float3(0, -1, 0);
	const float3 right = make_float3(1, 0, 0);
	const float3 left = make_float3(-1, 0, 0);
	const float3 back = make_float3(0, 0, 1);
	const float3 front = make_float3(0, 0, -1);

	void collision(float3 &pos, float3 &vel){
		collision_plane(up, 0.0f, pos, vel);
		collision_plane(down, 0.4f, pos, vel);
		collision_plane(right, 0.0f, pos, vel);
		collision_plane(left, 0.2f, pos, vel);
		collision_plane(back, 0.0f, pos, vel);
		collision_plane(front, 0.2f, pos, vel);
	}

	void integration(float dt, ParticleSystem *p){
		
		for(int i = 0; i < p->n_particles; i++){
			p->vel[i] = p->vel[i] + p->acc[i] * dt;
			p->pos[i] = p->pos[i] + p->vel[i] * dt;

			collision(p->pos[i], p->vel[i]);
		}

	}

	void Step(float dt, ParticleSystem *p, Grid *g){

		float3 *t = p->pos;
		p->pos = p->swap_pos;
		p->swap_pos = t;

		t = p->vel;
		p->vel = p->swap_vel;
		p->swap_vel = t;

		UpdateGrid(g, p->pos, p->vel, p->swap_pos, p->swap_vel, p->smooth_radius, p->n_particles);

		density(p, g);

		forces(p, g);

		integration(dt, p);

	}

};