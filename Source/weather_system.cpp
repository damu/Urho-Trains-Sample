#include <iostream>

#include "weather_system.h"

using namespace std;
using namespace Urho3D;

void weather_system::init(Scene* scene,ResourceCache* cache,Zone* zone)
{
    this->scene_=scene;
    this->cache=cache;
    this->zone=zone;

    effect=cache->GetResource<ParticleEffect>("Particle/cloud.xml");

    bb_clouds=BoundingBox(Vector3(-10000,3000,-10000),Vector3(10000,5000,10000));

    {   // skybox
        node_sky=scene_->CreateChild("Sky");
        node_sky->SetScale(5200.0f);
        Skybox* skybox=node_sky->CreateComponent<Skybox>();
        skybox->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
        auto material_sky=cache->GetResource<Material>("Materials/skybox_stars.xml");
        skybox->SetMaterial(material_sky);

        node_sky=scene_->CreateChild("Skysphere");
        auto* skysphere=node_sky->CreateComponent<StaticModel>();
        skysphere->SetModel(cache->GetResource<Model>("Models/cubesphere.mdl"));
        material_sky_sphere=cache->GetResource<Material>("Materials/skysphere.xml");
        skysphere->SetMaterial(material_sky_sphere);
        node_sky->SetScale(1000.0f);
    }

    {   // sun
        node_sun=scene_->CreateChild("Light");
        node_sun->SetDirection(Vector3(0.8f,-0.9f,0.3f));
        light_sun=node_sun->CreateComponent<Light>();
        light_sun->SetLightType(LIGHT_DIRECTIONAL);
        light_sun->SetCastShadows(true);
        light_sun->SetShadowBias(BiasParameters(0.0000025f,0.8f));
        //light_sun->SetShadowCascade(CascadeParameters(20.0f,50.0f,200.0f,2000.0f,0.01f,0.5f));
        light_sun->SetShadowCascade(CascadeParameters(2.0f,6.0f,18.0f,56.0f,0.001f,0.5f));
        light_sun->SetBrightness(1.0);
        light_sun->SetColor(Color(1,1,1,1));
        /*light->SetCastShadows(true);
        light->SetShadowBias(BiasParameters(0.000000025f,0.01f));
        light->SetShadowCascade(CascadeParameters(2.0f,6.0f,18.0f,56.0f,100.0f,100.0f));
        light->SetShadowResolution(1.0);
        light->SetBrightness(0.8);
        light->SetColor(Color(1.0,1.0,1.0,1));*/


        /*BillboardSet* billboardObject=node_sun->CreateComponent<BillboardSet>();
        billboardObject->SetNumBillboards(1);
        billboardObject->SetMaterial(cache->GetResource<Material>("Materials/sun.xml"));
        billboardObject->SetSorted(true);
        billboardObject->SetFaceCameraMode(FC_LOOKAT_XYZ);

        Billboard* bb=billboardObject->GetBillboard(0);
        bb->size_=Vector2(3000,3000);
        bb->enabled_=true;

        billboardObject->Commit();*/
    }
}

void weather_system::update(float timeStep)
{
    time+=timeStep*time_multiplier/24/60/60;    // time is a number of days, so bring timeStep to seconds

    while(nodes_cloud.size()<cloud_count)
    {
        Urho3D::SharedPtr<Node> n(scene_->CreateChild());
        n->SetPosition(Vector3(Random(bb_clouds.Size().x_)+bb_clouds.min_.x_,
                               Random(bb_clouds.Size().y_)+bb_clouds.min_.y_,
                               Random(bb_clouds.Size().z_)+bb_clouds.min_.z_));
        ParticleEmitter* emitter=n->CreateComponent<ParticleEmitter>();
        emitter->SetFaceCameraMode(FC_LOOKAT_XYZ);
        emitter->SetEffect(effect);
        emitter->SetSorted(true);
        effect->SetEmitterSize(Vector3(500,500,500));
        nodes_cloud.push_back(n);
    }
    // didn't get this to work (I also gave 'time' the meaning of days instead of years)
    /*{   // sun position, from http://en.wikipedia.org/wiki/Position_of_the_Sun
        double L=280.460*0.9856474*time;    // time is the n mentioned on that page
        double g=357.528+0.9856003*time;
        L-=(int(L)/360)*360;  // bring to 360 range
        g-=(int(g)/360)*360;  // bring to 360 range
        double lambda=L+1.915*sin(g*180/M_PI)+0.02*sin(2*g*180/M_PI);   // ecliptic longitude
        double epsilon=23.439-0.0000004*time;
        sun_pos=Vector3(-cos(epsilon*180/M_PI)*sin(lambda*180/M_PI),    // this is the sun seen from the equator
                        -cos(lambda*180/M_PI),
                        -sin(epsilon*180/M_PI)*sin(lambda*180/M_PI));
        double sun_angle=45;
        Matrix3 rot(cos(sun_angle*180/M_PI),-sin(sun_angle*180/M_PI),0,
                    sin(sun_angle*180/M_PI),cos(sun_angle*180/M_PI),0,
                    0,0,1);
        sun_pos=rot*sun_pos;
        node_sun->SetDirection(sun_pos);
        auto c=Clamp(-sun_pos.y_,0.0,1.0);
        light_sun->SetColor(Color(c,c,c,1));
    }*/
    {   // simple sun position
        auto cameraNode_=scene_->GetChild("Camera");
        auto pos_cam=cameraNode_->GetPosition();
        node_sun->SetPosition(pos_cam);

        //sun_pos=Vector3(cos(time*2)/2,-sin(time*2),cos(time*2)/2);
        //sun_pos=Vector3(0.3,-sin(time*M_PI*2),cos(time*M_PI*2)/4);
        //sun_pos.Normalize();
        //node_sun->SetDirection(sun_pos);
        node_sun->SetDirection(Vector3::FORWARD);
        node_sun->Yaw(45);
        node_sun->Roll(30);
        node_sun->Pitch(time*360);

        float c=Clamp(-node_sun->GetDirection().y_,0.0f,1.0f);
        light_sun->SetColor(color_sunrise.Lerp(color_midday,Clamp(abs(c-0.5f)*2.0f,0.0f,1.0f)));
        auto sun_brightness=Clamp(c,0.0f,1.0f);
        light_sun->SetBrightness(sun_brightness);

        node_sun->Translate(Vector3(0,0,-4000));
        material_sky_sphere->SetShaderParameter("SunDir",node_sun->GetDirection());
        node_sky->SetRotation(node_sun->GetRotation());

        for(auto&& n:nodes_cloud)
        {
            n->Translate(wind_speed*timeStep);
            auto bb=bb_clouds.Transformed(
                Matrix4(1,0,0,pos_cam.x_,
                        0,1,0,0,
                        0,0,1,pos_cam.z_,
                        0,0,0,1));
            if(false)
            if(!bb.IsInside(n->GetPosition()))
                n->SetPosition(Vector3(Random(bb_clouds.Size().x_)+bb.min_.x_,
                                       Random(bb_clouds.Size().y_)+bb.min_.y_,
                                       Random(bb_clouds.Size().z_)+bb.min_.z_));
        }

        effect->SetMinDirection(wind_speed);
        effect->SetMaxDirection(wind_speed);
        effect->SetMinVelocity(wind_speed.Length()*0.9);
        effect->SetMaxVelocity(wind_speed.Length()*1.1);
        float ttl=effect->GetMinTimeToLive();
        effect->SetMinEmissionRate(cloud_density/ttl);
        effect->SetMaxEmissionRate(cloud_density/ttl);
        effect->SetNumParticles(cloud_density);
        sun_brightness+=0.5;
        //effect->GetMaterial()->SetShaderParameter("MatDiffColor",Color(sun_brightness,sun_brightness,sun_brightness).Lerp(zone->GetAmbientColor(),sun_brightness));
        //zone->SetFogColor(fog_color*(1.0-(0.5+sun_pos.y_/2))*sun_brightness);
        zone->SetFogColor(light_sun->GetColor()*(1.0-(0.5+sun_pos.y_/2))*sun_brightness);
        zone->SetAmbientColor(Color(0.1,0.1,0.1)*sun_brightness);
    }
}
