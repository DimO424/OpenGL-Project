#ifndef _ADVANCED_NAVIGATION_H_
#define _ADVANCED_NAVIGATION_H_

#include "Application.h"
#include "FlyCamera.h"
#include "Vertex.h"
#include <vector>

struct NavMeshNode
{
	//Centre point of the polygon
	vec3 position;
	vec3 corners[3];
	NavMeshNode* edges[3];

	//A* data
	unsigned int flags;
	float edge_costs[3];
};

class AdvancedNavigation : public Application
{
public:
	virtual bool Startup();
	virtual void Shutdown();
	virtual bool Update();
	virtual void Draw();

	void BuildNavMesh();

	unsigned int m_program_id;
	OpenGLData m_SponzaMesh;
	std::vector<NavMeshNode> m_aNavNodes;

	FlyCamera m_Camera;

	float m_timer;
};

#endif