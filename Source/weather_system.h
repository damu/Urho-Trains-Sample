#ifndef WEATHER_SYSTEM_H
#define WEATHER_SYSTEM_H

#include <vector>

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Texture2D.h>

class weather_system
{
public:
    Urho3D::SharedPtr<Urho3D::Scene> scene_;
    Urho3D::SharedPtr<Urho3D::ResourceCache> cache;
    Urho3D::SharedPtr<Urho3D::Zone> zone;

    std::vector<Urho3D::SharedPtr<Urho3D::Node> > nodes_cloud;
    Urho3D::SharedPtr<Urho3D::ParticleEffect> effect;
    int cloud_count=500;
    Urho3D::BoundingBox bb_clouds;
    //Urho3D::Material* material_sky=0;
    Urho3D::SharedPtr<Urho3D::Material> material_sky_sphere;
    double time=0;  // time is the number of days passed. *.0 is sunrise, *.25 midday, *.5 sunset and *.75 midnight.
    double time_multiplier=60*10;  // per default each real second are ten simulated minute
    Urho3D::SharedPtr<Urho3D::Node> node_sun;
    Urho3D::SharedPtr<Urho3D::Light> light_sun;
    Urho3D::Vector3 sun_pos;
    Urho3D::Color color_sunrise=Urho3D::Color(0.5,0,0,1);
    Urho3D::Color color_midday=Urho3D::Color(1,1,1,1);
    Urho3D::Vector3 wind_speed=Urho3D::Vector3(100,0,50);
    int cloud_density=50;
    Urho3D::Color fog_color=Urho3D::Color(1.0f,0.7f,0.5f);
    Urho3D::SharedPtr<Urho3D::Node> node_sky;

    void init(Urho3D::Scene* scene,Urho3D::ResourceCache* cache,Urho3D::Zone* zone);
    void update(float timeStep);
};

#endif // WEATHER_SYSTEM_H
