#include "2d/OGScene.h"
#include "base/OGDirector.h"
#include "2d/OGCamera.h"
#include "base/OGEventDispatcher.h"
#include "base/OGEventListenerCustom.h"
#include "renderer/OGRenderer.h"


#if OG_USE_PHYSICS
#include "physics/OGPhysicsWorld.h"
#endif

OG_BEGIN

Scene::Scene()
	: _defaultCamera(Camera::create())
{
	_name = "Scene";
	addChild(_defaultCamera);
	 
}


Scene::~Scene()
{


#if OG_USE_PHYSICS
	delete _physicsWorld;
#endif
}



Scene* Scene::create(const std::string &fileName)
{
	Scene *ret = new (std::nothrow) Scene();
	if (ret && ret->init())
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		OG_SAFE_DELETE(ret);
		return nullptr;
	}
}
void Scene::removeAllChildren()
{
	if (_defaultCamera)
		_defaultCamera->retain();

	Node::removeAllChildren();

	if (_defaultCamera)
	{
		addChild(_defaultCamera);
		_defaultCamera->release();
	}
}
static bool camera_cmp(const Camera* a, const Camera* b)
{
	return a->getRenderOrder() < b->getRenderOrder();
}
const std::vector<Camera*>& Scene::getCameras()
{
	if (_cameraOrderDirty)
	{
		stable_sort(_cameras.begin(), _cameras.end(), camera_cmp);
		_cameraOrderDirty = false;
	}
	return _cameras;
}

bool Scene::init() {
	auto size = Director::getInstance()->getWinSize();
	setContentSize(size);
	setPosition(Vec2::ZERO);

	return true;
}



void Scene::render(Renderer* renderer)
{
	// 默认相机指针，当前这段逻辑里没用到，可留作扩展
	Camera* defaultCamera = nullptr;


	// 遍历场景中注册的所有相机
	for (const auto& camera : getCameras())
	{
		// 相机不可见则跳过，不参与本帧渲染
		if (!camera->isVisible())
			continue;

		// 关键：把当前正在渲染的相机设为 visitingCamera
		// ParticleSystem::draw 等绘制逻辑里会通过 Camera::getVisitingCamera() 拿到它，
		// 然后用这个相机做可见性剔除、坐标变换等。
		Camera::_visitingCamera = camera;
		//camera->setEnableClip(true);
		camera->setShowDebug(true);//因为相机改变需要同步下去位置，对drawNode
		// 如果这个相机启用了裁剪
		if (camera->isEnableClip())
		{
			// 取相机的视口，转成 SDL 的裁剪矩形
			const Rect& cameraClip = camera->getViewport();
			SDL_Rect clip = {
				(int)std::floor(cameraClip.origin.x),
				(int)std::floor(cameraClip.origin.y),
				(int)std::ceil(cameraClip.size.width),
				(int)std::ceil(cameraClip.size.height)
			};

			// 设置 SDL 渲染裁剪区域，只有这个矩形内的像素会被绘制
			SDL_RenderSetClipRect(SDLView::getInstance()->getRender(), &clip);

			// visit：用当前相机的视图矩阵作为父变换，遍历场景节点
			// camera->isViewProjectionUpdated() 表示相机视图是否更新，用于节点变换脏标记
			visit(renderer, camera->getViewMatrix(), camera->isViewProjectionUpdated());

			// 提交本相机收集到的渲染命令
			renderer->render();

			// 恢复 SDL 裁剪区域，避免影响下一个相机
			SDL_RenderSetClipRect(SDLView::getInstance()->getRender(), nullptr);
		}
		else
		{
			// 未启用裁剪：直接用当前相机视图矩阵遍历并渲染
			visit(renderer, camera->getViewMatrix(), camera->isViewProjectionUpdated());
			renderer->render();
		}
	}

	// 所有相机渲染完毕，清空当前访问相机，防止后续逻辑误用
	Camera::_visitingCamera = nullptr;
}


void Scene::draw()
{

}



Scene* Scene::createWithPhysics()
{
	Scene *ret = new (std::nothrow) Scene();
	if (ret && ret->initWithPhysics())
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		OG_SAFE_DELETE(ret);
		return nullptr;
	}
}

bool Scene::initWithPhysics()
{
 
#if OG_USE_PHYSICS
	_physicsWorld = PhysicsWorld::construct(this);
#endif

	bool ret = false;
	do
	{
		Director * director;
		OG_BREAK_IF(!(director = Director::getInstance()));

		this->setContentSize(director->getWinSize());


		// success
		ret = true;
	} while (0);
	return ret;
}

#if (OG_USE_PHYSICS )
void Scene::stepPhysicsAndNavigation(float deltaTime)
{
#if OG_USE_PHYSICS
	if (_physicsWorld && _physicsWorld->isAutoStep())
		_physicsWorld->update(deltaTime);
#endif

}
#endif

OG_END
