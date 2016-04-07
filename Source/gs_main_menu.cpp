#include "gs_main_menu.h"
#include "gs_playing.h"

using namespace Urho3D;
using namespace std;

gs_main_menu::gs_main_menu() : game_state()
{
    Node* node_camera=globals::instance()->camera->GetNode();
    node_camera->SetPosition(Vector3(0,0,0));
    node_camera->SetDirection(Vector3::FORWARD);

    // skybox
    {
        Node* skyNode=globals::instance()->scene->CreateChild("Sky");
        nodes.emplace_back(skyNode);
        skyNode->SetScale(1500.0f);
        Skybox* skybox=skyNode->CreateComponent<Skybox>();
        skybox->SetModel(globals::instance()->cache->GetResource<Model>("Models/Box.mdl"));
        skybox->SetMaterial(globals::instance()->cache->GetResource<Material>("Materials/Skybox.xml"));
    }

    // sun
    {
        node_sun=globals::instance()->scene->CreateChild("Light");
        nodes.emplace_back(node_sun);
        Light* light=node_sun->CreateComponent<Light>();
        light->SetLightType(LIGHT_DIRECTIONAL);
        light->SetCastShadows(true);
        light->SetShadowBias(BiasParameters(0.000000025f,0.01f));
        light->SetShadowCascade(CascadeParameters(2.0f,6.0f,18.0f,56.0f,100.0f,100.0f));
        light->SetShadowResolution(1.0);
        light->SetBrightness(0.8);
        light->SetColor(Color(1.0,1.0,1.0,1));
        node_sun->SetDirection(Vector3::FORWARD);
        node_sun->Yaw(-150);   // horizontal
        node_sun->Pitch(30);   // vertical
        node_sun->Translate(Vector3(0,0,-20000));
    }

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

    // Movement speed as world units per second
    float MOVE_SPEED=1.0f;
    // Mouse sensitivity as degrees per pixel
    const float MOUSE_SENSITIVITY=0.1f;

    // camera movement
    Input* input=GetSubsystem<Input>();
    Node* cameraNode_=globals::instance()->camera->GetNode();
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
    }
}

void gs_main_menu::HandleKeyDown(StringHash eventType,VariantMap& eventData)
{
    using namespace KeyDown;
    int key=eventData[P_KEY].GetInt();
    if(key==KEY_ESC)
        globals::instance()->engine->Exit();
}
