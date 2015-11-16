#include "grid.h"
#include "radixsort.h"
#include <cstring>

namespace SPH{
	
	Grid * CreateGrid(float3 origin, float3 size, float h, int max_particles, int max_neighbours){

		Grid* g = new Grid;
		
		g->origin = origin;
		g->f_size = size;
		g->i_size = make_int3(size * (1 / h));
		g->length = g->i_size.x * g->i_size.y * g->i_size.z;
		g->h = h;

		g->hash = new int[max_particles];
		g->index = new int[max_particles];

		g->start = new int[g->length];
		g->end = new int[g->length];

		g->nb = new int[max_particles * max_neighbours];
		g->nb_start = new int[max_particles];
		g->nb_end = new int[max_particles];

		return g;

	}

	void DestroyGrid(Grid *g){
		delete [] g->hash;
		delete [] g->index;
		delete [] g->start;
		delete [] g->end;
		delete [] g->nb;
		delete [] g->nb_start;
		delete [] g->nb_end;
		delete g;
	}

	inline int gridHash(Grid *g, float3 &pos){
		int3 p = make_int3((pos - g->origin) * ( 1.0f / g->h));
		if( p.x < 0 || p.x >= g->i_size.x ) return 0;
		if( p.y < 0 || p.y >= g->i_size.y ) return 0;
		if( p.z < 0 || p.z >= g->i_size.z ) return 0;
		return p.x + p.y * g->i_size.x + p.z * g->i_size.x * g->i_size.y;
	}

	inline int gridHash(Grid *g, int3 &p){
		return p.x + p.y * g->i_size.x + p.z * g->i_size.x * g->i_size.y;
	}

	const int3 int3_zero = make_int3(0, 0, 0);
	inline int3 gridPos(const float3 &pos, Grid *g){
		int3 p = make_int3((pos - g->origin) * ( 1.0f / g->h));
		if( p.x < 0 || p.x >= g->i_size.x ) return int3_zero;
		if( p.y < 0 || p.y >= g->i_size.y ) return int3_zero;
		if( p.z < 0 || p.z >= g->i_size.z ) return int3_zero;
		return p;
	}

	void UpdateGrid(Grid *g, float3 *new_pos, float3 *new_vel, float3 *old_pos, float3 *old_vel, float smooth, int n_particles){
	
		for(int i = 0; i < n_particles; i++){
			g->hash[i] = gridHash(g, old_pos[i]);
			g->index[i] = i;
		}

		RadixSort(g->hash, g->index, n_particles);

		memset(g->start, 0xff, n_particles * sizeof(int));

		g->start[g->hash[0]] = 0;
		for(int i = 1; i < n_particles; i++){
			if(g->hash[i] != g->hash[i - 1]){
				g->start[g->hash[i]] = i;
				g->end[g->hash[i-1]] = i;
			}
		}
		g->end[g->hash[n_particles - 1]] = n_particles;

		for(int i = 0; i < n_particles; i++){
			new_pos[i] = old_pos[g->index[i]];
			new_vel[i] = old_vel[g->index[i]];
		}

		int3 posi, posj;
		int nb_hash, sj, ej;
		for(int i = 0, k = 0; i < n_particles; i++){
			posi = gridPos(new_pos[i], g);
			g->nb_start[i] = k;
			for(int z = -1; z <= 1; z++)
				for(int y = -1; y <= 1; y++)
					for(int x = -1; x <= 1; x++){
						posj = posi + make_int3(x, y, z);
						nb_hash = gridHash(g, posj);
					
						sj = g->start[nb_hash];
						if( sj == 0xffffffff ) continue;
						ej = g->end[nb_hash];
						for(int j = sj; j < ej; j++){
							if( len(new_pos[j] - new_pos[i]) <= smooth && i != j){
								g->nb[k++] = j;
							}
						}
					}
			g->nb_end[i] = k;
		}
	}

};
