#include "ArriveDemo.hpp"

#include "../ECS/GameConfig.hpp"
#include "../ECS/Component/Components.hpp"
#include "../ECS/System/Systems.h"
#include "../ECS/Entity/EntityUtils.hpp"
#include "../ECSImplements/System/ISystems.h"
#include "../ECSImplements/Component/IComponents.hpp"
#include "../Core/Animations/SpineAnimation.hpp"
#include "../Core/Animations/SpriteAnimation.hpp"

#include "../Network/AppConnectorConfig.hpp"
#include "../Game/Controller/ToastController.hpp"
#include "../Core/Command/CommandDispatcher.hpp"
#include "../Core/Command/Commands.hpp"
#include "../Config/GameConstants.hpp"
#include "../Config/Cmd.hpp"
#include "../UIParser/CustomUI/CustomUIImageView.hpp"
#include "../UIParser/Atlas/AtlasCache.hpp"
#include "../ECS/Utils/ECSParser.hpp"

USING_NS_CC;

ArriveDemo::ArriveDemo() : AbstractDemo() {}
ArriveDemo::~ArriveDemo() {}

void ArriveDemo::onRecieveCommand(int type, std::shared_ptr<connection_object> data) {

	if (type == command::ON_GLOBAL_TAP) {
		this->onGlobalTap(data);
	}
	else if (type == command::ON_GLOBAL_TOUCH_MOVE) {
		this->onGlobalTouchMove(data);
	}
	else if (type == command::ON_GLOBAL_TOUCH_END) {
		this->onGlobalTouchEnd(data);
	}
	else if (type == command::ON_GLOBAL_FLING) {
		this->onGlobalTouchFling(data);
	}
	else if (type == command::ON_GLOBAL_ZOOM) {
		this->onGlobalTouchZoom(data);
	}
}

void ArriveDemo::onGlobalTap(std::shared_ptr<connection_object> data)
{
	if (game::config->isBlockInput()) return;
	cocos2d::Vec2 pos(data->getFloat("x"), data->getFloat("y"));
	pos = getGameLayout()->convertToNodeSpace(pos);

	auto target = &mWorld->getTagManager()->getEntity("target");
	getComponent<PositionComponent>(target)->setPosition(pos.x, pos.y);

	auto mover = &mWorld->getTagManager()->getEntity("mover");
	getComponent<PathFollowComponent>(mover)->addWayPoint(pos, 30, true);
	getComponent<StateComponent>(mover)->setState(game::state::TROOP_MOVE);
}

void ArriveDemo::loadEntities()
{
	/*create background*/
	{
		auto map = cocos2d::experimental::TMXTiledMap::create(cocos2d::StringUtils::format("maps/map-%d.tmx", game::config->getMapType()));
		map->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
		map->setPosition(getGameLayout()->getContentSize() / 2);
		getGameLayout()->addChild(map);
	}
	ECSParser::getInstance()->parse(mWorld, "xml/demo/arrive-demo-entities.xml");
}

void ArriveDemo::loadSystemAndConfig()
{
	game::config->setPlayerID(1);
	game::config->setPlaySide(game::side::SIDE_1);
	game::config->setEnemySide(game::side::SIDE_2);
	game::config->setMapType(game::map::SPRING);
	game::config->setBlockInput(false);

	addSystem(new TroopSystem());
	addSystem(new MovementSystem());
	addSystem(new AnimationSystem());
	addSystem(new AfterPhysicSystem());
	addSystem(new AnimationSystem());
	addSystem(new RemoveEntitySystem());
	addSystem(new VelocityDrawSystem());
	addSystem(new RenderSystem(getGameLayout()));
	addSystem(new SortRenderSystem());

	mWorld->getSystemManager()->initializeAll();

	sendCommand(cmd::camera::ZOOM_TO, DataBuilder::builder()->putFloat("zoom", 1.2f)->putFloat("time", 1)->build());
}



