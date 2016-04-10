/**
* Released under the same permissive MIT-license as Urho3D.
* https://raw.githubusercontent.com/urho3d/Urho3D/master/License.txt
*/

#include <string>
#include <memory>

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/RenderPath.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Geometry.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource3D.h>
#include <Urho3D/Audio/SoundListener.h>
#include <Urho3D/Audio/Audio.h>

#include "gs_main_menu.h"
#include "gs_playing.h"

using namespace Urho3D;

/// UTS main class mainly used for setup. The control is then given to the game states (starting with gs_main_menu).
class UTS : public Application
{
public:
    SharedPtr<Scene> scene_;
    Node* cameraNode_;

    UTS(Context * context) : Application(context) {}

    virtual void Setup()
    {
        engineParameters_["FullScreen"]=false;
        engineParameters_["WindowWidth"]=1200;
        engineParameters_["WindowHeight"]=720;
        engineParameters_["WindowResizable"]=false;
        engineParameters_["Multisample"]=16;
    }

    virtual void Start()
    {
        ResourceCache* cache=GetSubsystem<ResourceCache>();
        XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
        GetSubsystem<UI>()->GetRoot()->SetDefaultStyle(style);

        scene_=new Scene(context_);
        scene_->CreateComponent<Octree>();
        scene_->CreateComponent<DebugRenderer>();

        DebugHud* debugHud=engine_->CreateDebugHud();
        debugHud->SetDefaultStyle(style);

        cameraNode_=scene_->CreateChild("Camera");
        Camera* camera=cameraNode_->CreateComponent<Camera>();
        camera->SetFarClip(20000);
        SoundListener* listener=cameraNode_->CreateComponent<SoundListener>();
        GetSubsystem<Audio>()->SetListener(listener);
        GetSubsystem<Audio>()->SetMasterGain(SOUND_MUSIC,0.3);

        Renderer* renderer=GetSubsystem<Renderer>();
        SharedPtr<Viewport> viewport(new Viewport(context_,scene_,cameraNode_->GetComponent<Camera>()));
        renderer->SetViewport(0,viewport);
        renderer->SetShadowMapSize(1024);
        renderer->SetHDRRendering(true);

        RenderPath* effectRenderPath=viewport->GetRenderPath();
        effectRenderPath->Append(cache->GetResource<XMLFile>("PostProcess/AutoExposure.xml"));
        effectRenderPath->Append(cache->GetResource<XMLFile>("PostProcess/BloomHDR_r2.xml"));
        effectRenderPath->Append(cache->GetResource<XMLFile>("PostProcess/FXAA2.xml"));

        Node* zoneNode=scene_->CreateChild("Zone");
        Zone* zone=zoneNode->CreateComponent<Zone>();
        zone->SetBoundingBox(BoundingBox(-50000.0f,50000.0f));
        zone->SetFogStart(10000.0f);
        zone->SetFogEnd(20000.0f);
        zone->SetAmbientColor(Color(0.3,0.3,0.3));

        SubscribeToEvent(E_KEYDOWN,URHO3D_HANDLER(UTS,HandleKeyDown));

        // fill our game state shared variables
        globals::instance()->cache=cache;
        globals::instance()->camera=camera;
        globals::instance()->scene=scene_;
        globals::instance()->context=context_;
        globals::instance()->engine=engine_;
        globals::instance()->game_states.emplace_back(new gs_main_menu);
    }

    virtual void Stop()
    {
        globals::instance()->game_states.clear();
    }

    void HandleKeyDown(StringHash eventType,VariantMap& eventData)
    {
        using namespace KeyDown;
        int key=eventData[P_KEY].GetInt();

        if(key==KEY_TAB)
        {
            GetSubsystem<Input>()->SetMouseVisible(!GetSubsystem<Input>()->IsMouseVisible());
            GetSubsystem<Input>()->SetMouseGrabbed(!GetSubsystem<Input>()->IsMouseGrabbed());
        }
        else if(key==KEY_T)
            globals::instance()->camera->SetFillMode(globals::instance()->camera->GetFillMode()==FILL_WIREFRAME?FILL_SOLID:FILL_WIREFRAME);
        else if(key==KEY_F2)
        {
            DebugHud* debugHud=GetSubsystem<DebugHud>();
            if (debugHud->GetMode()!=DEBUGHUD_SHOW_ALL)
                debugHud->SetMode(DEBUGHUD_SHOW_ALL);
            else
                debugHud->SetMode(DEBUGHUD_SHOW_NONE);
        }
        else if(key==KEY_F3)
        {
            DebugHud* debugHud=GetSubsystem<DebugHud>();
            if (debugHud->GetMode()!=DEBUGHUD_SHOW_ALL_MEMORY)
                debugHud->SetMode(DEBUGHUD_SHOW_ALL_MEMORY);
            else
                debugHud->SetMode(DEBUGHUD_SHOW_NONE);
        }
    }
};

URHO3D_DEFINE_APPLICATION_MAIN(UTS)
