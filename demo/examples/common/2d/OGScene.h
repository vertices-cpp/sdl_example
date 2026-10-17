#ifndef _SCENE_H_
#define _SCENE_H_


#include "2d/OGNode.h" 

OG_BEGIN

class Camera;
//class BaseLight;
class Renderer;
class EventListenerCustom;
class EventCustom;

#if OG_USE_PHYSICS
class PhysicsWorld;
#endif

class Scene: public Node
{
	
public:
	std::vector<Camera*> _cameras; //weak ref to Camera
	Camera*  _defaultCamera = nullptr;
	bool _cameraOrderDirty = true;
	Scene();
	~Scene();
	 
	static Scene * create(const std::string & fileName);

	void removeAllChildren();

	void setCameraOrderDirty() { _cameraOrderDirty = true; }
	const std::vector<Camera*>& getCameras();

	bool init();
 
	
	void render(Renderer * rendererm);  

	void draw();

	

	//物理部分
	static Scene * createWithPhysics();
	bool initWithPhysics();
#if OG_USE_PHYSICS
	void stepPhysicsAndNavigation(float deltaTime);
 
	PhysicsWorld* getPhysicsWorld() const { return _physicsWorld; }
protected:
	PhysicsWorld* _physicsWorld = nullptr;
#endif


};

OG_END

#endif  //_SCENE_H_
