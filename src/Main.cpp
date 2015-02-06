#include "IntroToOpenGl.h"
#include "CameraAndProjection.h"

int main()
{
	IntroToOpenGl application;

	if (application.Startup() == true)
	{
		while (application.Update() == true)
		{
			application.Draw();
		}

		application.Shutdown();
	}

	return 0;
}