#include "TestScene.h"
#include "Core/ServiceLocator/ServiceLocator.h"

#define WHITE {1.0f,1.0f,1.0f,1.0f}
#define RED   {1.0f,0.0f,0.0f,1.0f}

TestScene::~TestScene() {
    CleanupResources();
}

void TestScene::CleanupResources() {

}

void TestScene::Initialize() {
	nowSceneName_ = SCENE::TEST;

	CleanupResources();

	debugCamera_.SetInput(MyInput::GetInput());

	// SkyBoxの初期化
	skyBox_.Initialize("resources/Image/rostock_laage_airport_4k.dds");
	skyBox_.SetScale(1000.0f);

	model_.Initialize("ball.obj");
	model_.SetEnvironmentMap("resources/Image/rostock_laage_airport_4k.dds");
	//model_.UseLight(false);
}

void TestScene::Update() {
	debugCamera_.Update();

	MyDebugLine::AddGrid(100.0f, 10, WHITE);
}

void TestScene::Draw() {
	// SkyBoxを最初に描画（遠景）
	skyBox_.Draw(debugCamera_);
	
	model_.Draw(debugCamera_);

	MyDebugLine::Draw(debugCamera_);
}

void TestScene::DrawImGui() {
#ifdef USE_IMGUI

#endif
}

void TestScene::PostFrameCleanup() {

}