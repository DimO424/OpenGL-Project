#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "Application.h"
#include "FlyCamera.h"

#include "FBXFile.h"
#include "Utility.h"
#include "Vertex.h"

class Animation : public Application
{
public:
	virtual bool Startup();
	virtual void Shutdown();
	virtual bool Update();
	virtual void Draw();

	void GenerateGLMeshes(FBXFile* a_fbx);

	void EvaluateSkeleton(FBXAnimation* a_animation, FBXSkeleton* a_skeleton, float a_timer);
	void UpdateBones(FBXSkeleton* a_skeleton);

	FlyCamera m_Camera;

	float m_timer;

	FBXFile* m_file;
	std::vector<OpenGLData> m_meshes;

	unsigned int m_programID;
};

#endif