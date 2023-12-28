#ifndef SHADING_HPP
#define SHADING_HPP
#include <eigen3/Eigen/Eigen>
#include "transformation.hpp"
#include <iostream>
#include "triangle.hpp"
#include "environment_light.hpp"
using namespace Eigen;
// view_pos, point_pos, color, normal, el, ka, kd, ks
// view_pos, point_pos should be in view space.
// view_pos need to (0,0,0)
Vector3f Blin_Phong_shading(Vector3f point_pos, Vector3f normal, environment_light el, Vector3f ka, Vector3f kd, Vector3f ks, float p) {
    auto view_pos=Vector3f(0,0,0);
    auto amb_light_intensity=Vector3f(20,20,20);
    auto result_color=Vector3f(0,0,0);
    // std::cout<<el.lights.size()<<std::endl;
    for(auto &light: el.lights){
        // for(int i=0;i<4;i++)
        //     std::cout<<light.view_pos[i]<<' ';
        auto light_pos=light.view_pos.head(3)/light.view_pos[3];
        auto light_intensity=light.intensity;
        Eigen::Vector3f light_direction=light_pos-point_pos;
        Eigen::Vector3f view_direction=view_pos-point_pos;
        Eigen::Vector3f half_direction=light_direction+view_direction;
        float dis_p2e=light_direction.norm();
        view_direction.normalize();
        light_direction.normalize();
        normal.normalize();
        half_direction.normalize();
        // components are. Then, accumulate that result on the *result_color* object.
        // compute the ambient light
        auto amb_res=amb_light_intensity.cwiseProduct(ka);
        // compute the diffused light
        Vector3f dif_res(0,0,0);
        if(normal.dot(light_direction)>0)
            dif_res=normal.dot(light_direction)*(1/dis_p2e/dis_p2e)*(light_intensity.cwiseProduct(kd));
        // compute the specular light
        Vector3f spe_res(0,0,0);
        if(normal.dot(light_direction)>0)
            spe_res=std::pow(std::max(normal.dot(half_direction),0.0f),p)*(1/dis_p2e/dis_p2e)*(light_intensity.cwiseProduct(ks));
        result_color+=amb_res+spe_res+dif_res;
        // result_color=dif_res;
    }
    return 255.0f*result_color;
}
#endif