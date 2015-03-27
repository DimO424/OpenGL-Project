#include "IntroToOpenGl.h"
#include "CameraAndProjection.h"
#include "RenderingGeometryh.h"
#include "Texturing.h"
#include "Lighting.h"
#include "Animation.h"
#include "Particle.h"
#include "GPUParticle.h"
#include "SceneManagement.h"
#include "RenderTargets.h"
#include "PostProcess.h"
#include "Shadows.h"
#include "DefferedRendering.h"
#include "ProceduralGeneration.h"

int main()
{
	ProceduralGeneration application;

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