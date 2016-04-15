#ifndef GS_MAIN_MENU_H
#define GS_MAIN_MENU_H

#include "game_state.h"

#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/Graphics/Terrain.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource3D.h>
#include <Urho3D/Audio/SoundListener.h>
#include <Urho3D/Audio/Audio.h>

#include "weather_system.h"

/// The main menu displayed when starting the game.
class gs_main_menu : public game_state
{
public:
    Urho3D::Node* node_sun=0;
    weather_system weather;

    gs_main_menu();
    void update(Urho3D::StringHash eventType,Urho3D::VariantMap& eventData);
    void HandleKeyDown(Urho3D::StringHash eventType,Urho3D::VariantMap& eventData);
    void HandleClosePressed(Urho3D::StringHash eventType,Urho3D::VariantMap& eventData){globals::instance()->engine->Exit();}

    URHO3D_OBJECT(gs_main_menu,game_state);
};

#endif // GS_MAIN_MENU_H
