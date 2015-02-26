#include "IntroToOpenGl.h"
#include "CameraAndProjection.h"
#include "RenderingGeometryh.h"
#include "Texturing.h"
#include "Lighting.h"
#include "Animation.h"
#include "Particle.h"

int main()
{
	Particles application;

	if (application.Startup() == false)
	{
		return -1;
	}
		
	while (application.Update() == true)
	{
		application.Draw();
	}

	application.Shutdown();

	return 0;
}