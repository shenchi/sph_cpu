#pragma once

#include "particlesystem.h"
#include "grid.h"

namespace SPH{

	void Step(float dt, ParticleSystem *p, Grid *g);

};