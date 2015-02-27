#include "Animation.h"

bool Animation::Startup()
{
	if (Application::Startup() == false)
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();
	glfwSetTime(0.0f);

	m_Camera = FlyCamera(1280.0f / 720.0f, 10.0f);
	m_Camera.SetLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	m_Camera.m_fSensitivity = 3;

	m_file = new FBXFile();
	m_file->load("./models/characters/pyro/pyro.fbx");

	m_file->initialiseOpenGLTextures();

	GenerateGLMeshes(m_file);

	LoadShader("./Shaders/SkinnedVertex.glsl", 0, "./Shaders/SkinnedFragment.glsl", &m_programID);

	m_timer = 0;

	return true;
}

void Animation::Shutdown()
{	
	m_file->unload();
	delete m_file;

	Gizmos::destroy();
	Application::Shutdown();
}

bool Animation::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0f);

	m_timer += dt;

	vec4 white(1);
	vec4 black(0, 0, 0, 1);

	Gizmos::clear();

	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0.01f, -10), vec3(-10 + i, -0.01f, 10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, -0.01f, -10 + i), vec3(10, -0.01f, -10 + i),
			i == 10 ? white : black);
	}

	FBXSkeleton* skele = m_file -> getSkeletonByIndex(0);
	FBXAnimation* anim = m_file -> getAnimationByIndex(0);

	skele -> evaluate(anim, m_timer);

	for (unsigned int i = 0; i < skele->m_boneCount; ++i)
	{
		skele->m_nodes[i]->updateGlobalTransform();

		mat4 node_global = skele->m_nodes[i]->m_globalTransform;
		vec3 node_pos = node_global[3].xyz;

		Gizmos::addAABBFilled(node_pos, vec3(0.5f), vec4(1, 0, 0, 1), &node_global);

		if (skele -> m_nodes[i]->m_parent != nullptr)
		{
			vec3 parent_pos = skele->m_nodes[i]->m_parent->m_globalTransform[3].xyz;
			Gizmos::addLine(node_pos, parent_pos, vec4(0, 1, 0, 1));
		}
	}

	m_Camera.Update(dt);

	return true;
}

void Animation::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Gizmos::draw(m_Camera.projectionTransform, m_Camera.viewTransform);

	glUseProgram(m_programID);

	int view_proj_uniform = glGetUniformLocation(m_programID, "projection_view");
	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, (float*)&m_Camera.view_Projection);

	int diffuse_uniform = glGetUniformLocation(m_programID, "diffuse");
	glUniform1i(diffuse_uniform, 0);

	FBXSkeleton* skeleton = m_file->getSkeletonByIndex(0);
	skeleton->updateBones();

	int bones_uniform = glGetUniformLocation(m_programID, "bones");
	glUniformMatrix4fv(bones_uniform, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

	for (unsigned int i = 0; i < m_meshes.size(); ++i)
	{
		FBXMeshNode* currMesh = m_file->getMeshByIndex(i);
		FBXMaterial* meshMaterial = currMesh->m_material;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, meshMaterial->textures[FBXMaterial::DiffuseTexture]->handle);

		mat4 worldTransform = currMesh->m_globalTransform;
		int worldUniform = glGetUniformLocation(m_programID, "world");
		glUniformMatrix4fv(worldUniform, 1, GL_FALSE, (float*)&worldTransform);

		glBindVertexArray(m_meshes[i].m_VAO);
		glDrawElements(GL_TRIANGLES, m_meshes[i].m_indexCount, GL_UNSIGNED_INT, 0);
	}

	glfwSwapBuffers(this->m_pWindow);
	glfwPollEvents();
}

void Animation::GenerateGLMeshes(FBXFile* a_fbx)
{
	unsigned int mesh_count = a_fbx->getMeshCount();
	m_meshes.resize(mesh_count);

	for (unsigned int meshIndex = 0; meshIndex < mesh_count; ++meshIndex)
	{
		FBXMeshNode* currMesh = a_fbx->getMeshByIndex(meshIndex);

		m_meshes[meshIndex].m_indexCount = currMesh-> m_indices.size();

		glGenBuffers(1, &m_meshes[meshIndex].m_VBO);
		glGenBuffers(1, &m_meshes[meshIndex].m_IBO);
		glGenVertexArrays(1, &m_meshes[meshIndex].m_VAO);

		glBindVertexArray(m_meshes[meshIndex].m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_meshes[meshIndex].m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(FBXVertex)* currMesh->m_indices.size(), currMesh->m_vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, m_meshes[meshIndex].m_IBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned int)* currMesh->m_indices.size(), currMesh->m_vertices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); //pos
		glEnableVertexAttribArray(1); //texCoord
		glEnableVertexAttribArray(2); //bone indices
		glEnableVertexAttribArray(3); //bone weights

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void Animation::EvaluateSkeleton(FBXAnimation* a_animation, FBXSkeleton* a_skeleton, float a_timer)
{
	float fps = 24.0f;
	int current_frame = (int)(a_timer * fps);

	//loop through all the tracks
	for (unsigned int track_index = 0; track_index < a_animation->m_trackCount; ++track_index)
	{
		//wrap back to the start of the track if we've gone too far
		int track_frame_count = a_animation->m_tracks[track_index].m_keyframeCount;
		int track_frame = current_frame % track_frame_count;
		// current_frame %= track_frame_count;

		//find what keyframes are affecting the bones
		FBXKeyFrame curr_frame = a_animation->m_tracks[track_index].m_keyframes[track_frame];

		FBXKeyFrame next_frame = a_animation->m_tracks[track_index].m_keyframes[(track_frame + 1) % track_frame_count];

		//interpolate between which keyframes to generate the matrix
		//for the current pose
		float time_since_frame_flip = a_timer - ((int)(current_frame / fps));
		float t = time_since_frame_flip * fps;

		vec3 new_pos = glm::mix(curr_frame.m_translation, next_frame.m_translation, t);
		vec3 new_scale = glm::mix(curr_frame.m_scale, next_frame.m_scale, t);
		glm::quat new_rot = glm::slerp(curr_frame.m_rotation, next_frame.m_rotation, t);

		mat4 local_transform = glm::translate(new_pos) * glm::toMat4(new_rot) * glm::scale(new_scale);

		//get the right bone for the given track
		int bone_index = a_animation->m_tracks[track_index].m_boneIndex;

		//set the FBXNode's local transform to match
		if (bone_index < a_skeleton -> m_boneCount)
		{
			a_skeleton->m_nodes[bone_index]->m_localTransform = local_transform;
		}
	}
}

void Animation::UpdateBones(FBXSkeleton* a_skeleton)
{
	// loop through the nodes in skeleton
	for (unsigned int bone_index = 0; bone_index < a_skeleton->m_boneCount; ++bone_index)
	{
		// generate their global transforms
		int parent_index = a_skeleton->m_parentIndex[bone_index];

		if (parent_index == -1)
		{
			a_skeleton->m_bones[bone_index] = a_skeleton->m_nodes[bone_index]->m_localTransform;
		}
		else
		{
			a_skeleton->m_bones[bone_index] = a_skeleton->m_nodes[bone_index]->m_localTransform * a_skeleton->m_bones[parent_index];
		}

	}

	for (unsigned int bone_index = 0; bone_index < a_skeleton->m_boneCount; ++bone_index)
	{
		// multiply the global transform by the inverse bind pose
		a_skeleton->m_bones[bone_index] = a_skeleton->m_bones[bone_index] * a_skeleton -> m_bindPoses[bone_index];
	}
}