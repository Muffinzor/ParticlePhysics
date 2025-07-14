#include "SandParticle.h"
#include "Utility.h"

SandParticle::SandParticle() {
	type = ParticleType::Solid;
	hue = Utility::random_int(55, 70);
	color = Utility::getRGB(hue, 1, 1);
	base_color = Utility::getRGB(hue, 1, 1);
}
