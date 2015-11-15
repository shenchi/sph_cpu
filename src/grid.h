#pragma once

#include "vector.h"

namespace SPH{

	struct Grid{
		float3 origin;
		float3 f_size;
		int3 i_size;
		int length;
		float h;
		int *hash;
		int *index;
		int *start;
		int *end;
		int max_nb;
		int *nb;
		int *nb_start;
		int *nb_end;
	};

	Grid * CreateGrid(float3 origin, float3 size, float h, int max_particles, int max_neighbours);
	
	void DestroyGrid(Grid *g);
	
	void UpdateGrid(Grid *g, float3 *new_pos, float3 *new_vel, float3 *old_pos, float3 *old_vel, float smooth, int n_particles);

};
