#include "gs_pause.h"
#include "gs_playing.h"
#include "gs_main_menu.h"

using namespace Urho3D;

gs_pause::gs_pause() : game_state()
{
    globals::instance()->scene->SetUpdateEnabled(false);

    GetSubsystem<Input>()->SetMouseVisible(true);
    GetSubsystem<Input>()->SetMouseGrabbed(false);

    SubscribeToEvent(E_KEYDOWN,URHO3D_HANDLER(gs_pause,HandleKeyDown));
}

gs_pause::~gs_pause()
{
    globals::instance()->scene->SetUpdateEnabled(true);
}

void gs_pause::HandleKeyDown(StringHash eventType,VariantMap& eventData)
{
    using namespace KeyDown;
    int key=eventData[P_KEY].GetInt();
    if(key==KEY_ESC)
    {
        GetSubsystem<Input>()->SetMouseVisible(false);
        GetSubsystem<Input>()->SetMouseGrabbed(true);
        globals::instance()->game_states.resize(1);
    }
}
