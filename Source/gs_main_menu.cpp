#include "gs_main_menu.h"
#include "gs_playing.h"

using namespace Urho3D;
using namespace std;

gs_main_menu::gs_main_menu() : game_state()
{
    Node* node_camera=globals::instance()->camera->GetNode();
    node_camera->SetPosition(Vector3(0,0,0));
    node_camera->SetDirection(Vector3::FORWARD);

    weather.init(globals::instance()->scene,globals::instance()->cache,globals::instance()->zone);

    // terrain
    {
        Node* terrainNode=globals::instance()->scene->CreateChild("Terrain");
        terrainNode->SetPosition(Vector3(3.0f,-0.4f));
        Terrain* terrain=terrainNode->CreateComponent<Terrain>();
        terrain->SetPatchSize(128);
        terrain->SetSpacing(Vector3(2,0.5,2));
        terrain->SetSmoothing(true);
        terrain->SetHeightMap(globals::instance()->cache->GetResource<Image>("Textures/HeightMap.png"));
        terrain->SetMaterial(globals::instance()->cache->GetResource<Material>("Materials/Terrain.xml"));
        terrain->SetCastShadows(true);
        terrain->SetOccluder(true);
    }

    GetSubsystem<Input>()->SetMouseVisible(true);
    GetSubsystem<Input>()->SetMouseGrabbed(false);

    SubscribeToEvent(E_UPDATE,URHO3D_HANDLER(gs_main_menu,update));
    SubscribeToEvent(E_KEYDOWN,URHO3D_HANDLER(gs_main_menu,HandleKeyDown));
}

void gs_main_menu::update(StringHash eventType,VariantMap& eventData)
{
    float timeStep=eventData[Update::P_TIMESTEP].GetFloat();
    weather.update(timeStep);

    // Movement speed as world units per second
    //float MOVE_SPEED=1.0f;
	// Mouse sensitivity as degrees per pixel
    const float MOUSE_SENSITIVITY=0.1f;

    // camera movement
    Input* input=GetSubsystem<Input>();
    Node* cameraNode_=globals::instance()->camera->GetNode();
	// Let the camera speed high as its position above the terrain. Max 500m/s
	Terrain* terr = globals::instance()->scene->GetChild("Terrain")->GetComponent<Terrain>();
	Vector3 vv = cameraNode_->GetPosition();
	float hh = terr->GetHeight(vv);
	float MOVE_SPEED = Clamp(vv.y_ - hh, 5.0f, 500.0f);

    if(input->GetQualifierDown(1))  // 1 is shift, 2 is ctrl, 4 is alt
        MOVE_SPEED*=20;
    if(input->GetKeyDown('W'))
        cameraNode_->Translate(Vector3(0,0, 1)*MOVE_SPEED*timeStep);
    if(input->GetKeyDown('S'))
        cameraNode_->Translate(Vector3(0,0,-1)*MOVE_SPEED*timeStep);
    if(input->GetKeyDown('A'))
        cameraNode_->Translate(Vector3(-1,0,0)*MOVE_SPEED*timeStep);
    if(input->GetKeyDown('D'))
        cameraNode_->Translate(Vector3( 1,0,0)*MOVE_SPEED*timeStep);
    if(input->GetKeyDown('Q'))
        cameraNode_->Translate(Vector3(0,-1,0)*MOVE_SPEED*timeStep);
    if(input->GetKeyDown('E'))
        cameraNode_->Translate(Vector3(0,1,0)*MOVE_SPEED*timeStep);

    if(!GetSubsystem<Input>()->IsMouseVisible())
    {
        IntVector2 mouseMove=input->GetMouseMove();
        if(mouseMove.x_>-2000000000&&mouseMove.y_>-2000000000)
        {
            static float yaw_=0;
            static float pitch_=0;
            yaw_+=MOUSE_SENSITIVITY*mouseMove.x_;
            pitch_+=MOUSE_SENSITIVITY*mouseMove.y_;
            pitch_=Clamp(pitch_,-90.0f,90.0f);
            // Reset rotation and set yaw and pitch again
            cameraNode_->SetDirection(Vector3::FORWARD);
            cameraNode_->Yaw(yaw_);
            cameraNode_->Pitch(pitch_);
        }
		Vector3 v = cameraNode_->GetPosition();
		float h = terr->GetHeight(v);
		if (v.y_ < h + 1.0f)
		{
			v.y_ = h + 1.0f;
			cameraNode_->SetPosition(v);
		}
	}
}

void gs_main_menu::HandleKeyDown(StringHash eventType,VariantMap& eventData)
{
    using namespace KeyDown;
    int key=eventData[P_KEY].GetInt();
    if(key==KEY_ESC)
        globals::instance()->engine->Exit();
}
