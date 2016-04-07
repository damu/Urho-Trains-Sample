#include "misc.h"
#include "gs_playing.h"

#include <map>
#include <Urho3D/Graphics/BillboardSet.h>
#include <Urho3D/ThirdParty/PugiXml/pugixml.hpp>

#include "gs_pause.h"

using namespace std;
using namespace Urho3D;

gs_playing* gs_playing::instance;

gs_playing::gs_playing() : game_state()
{
    if(instance)
        instance->~gs_playing();
    instance=this;

    GetSubsystem<Input>()->SetMouseVisible(false);
    GetSubsystem<Input>()->SetMouseGrabbed(true);

    SubscribeToEvent(E_UPDATE,URHO3D_HANDLER(gs_playing,update));
    SubscribeToEvent(E_KEYDOWN,URHO3D_HANDLER(gs_playing,HandleKeyDown));
    SubscribeToEvent(E_MOUSEBUTTONDOWN,URHO3D_HANDLER(gs_playing,HandleMouseDown));
    SubscribeToEvent(E_MOUSEBUTTONUP,URHO3D_HANDLER(gs_playing,HandleMouseUp));

    // skybox
    {
        Node* skyNode=globals::instance()->scene->CreateChild("Sky");
        nodes.emplace_back(skyNode);
        skyNode->SetScale(50000.0f);
        Skybox* skybox=skyNode->CreateComponent<Skybox>();
        skybox->SetModel(globals::instance()->cache->GetResource<Model>("Models/Box.mdl"));
        skybox->SetMaterial(globals::instance()->cache->GetResource<Material>("Materials/Skybox.xml"));
    }

    // sun
    {
        Node* lightNode=globals::instance()->scene->CreateChild("Light");
        nodes.emplace_back(lightNode);
        Light* light=lightNode->CreateComponent<Light>();
        light->SetLightType(LIGHT_DIRECTIONAL);
        light->SetCastShadows(true);
        light->SetShadowBias(BiasParameters(0.00000025f,1.0f));
        light->SetShadowCascade(CascadeParameters(20.0f,60.0f,180.0f,560.0f,100.0f,100.0f));
        light->SetShadowResolution(1.0);
        light->SetBrightness(1.2);
        light->SetColor(Color(0.02,0.05,0.1,1));
        lightNode->SetDirection(Vector3::FORWARD);
        lightNode->Yaw(-150);   // horizontal
        lightNode->Pitch(60);   // vertical
        lightNode->Translate(Vector3(0,0,-20000));
    }
}

void gs_playing::update(StringHash eventType,VariantMap& eventData)
{
    if(globals::instance()->game_states.size()>1)   // don't do anything if there is another game mode like gs_pause on top
        return;

    Input* input=GetSubsystem<Input>();
    float timeStep=eventData[Update::P_TIMESTEP].GetFloat();


}

void gs_playing::HandleKeyDown(StringHash eventType,VariantMap& eventData)
{
    if(globals::instance()->game_states.size()>1)
        return;
    using namespace KeyDown;
    int key=eventData[P_KEY].GetInt();
    if(key==KEY_ESC)
        globals::instance()->game_states.emplace_back(new gs_pause);

    if(key==KEY_T)
        globals::instance()->camera->SetFillMode(globals::instance()->camera->GetFillMode()==FILL_WIREFRAME?FILL_SOLID:FILL_WIREFRAME);
}

void gs_playing::HandleMouseDown(StringHash eventType,VariantMap& eventData)
{
}

void gs_playing::HandleMouseUp(StringHash eventType,VariantMap& eventData)
{
}
