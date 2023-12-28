#ifndef SPHERE_CALCULATION_HPP
#define SPHERE_CALCULATION_HPP
#include <eigen3/Eigen/Eigen>
using namespace Eigen;
Vector4f from_pos_to_g(Vector4f view_pos){
    view_pos/=view_pos[3];
    Vector4f g;
    g<<0-view_pos[0],0-view_pos[1],0-view_pos[2],0;
    g.normalize();
    return g;
}
Vector4f from_pos_to_t(Vector4f view_pos){
    view_pos/=view_pos[3];
    Vector4f g_cross_t;
    g_cross_t<<0-view_pos[1],view_pos[0],0 ,0;
    g_cross_t.normalize();
    Vector3f gg=g_cross_t.head(3);
    Vector3f vv=view_pos.head(3);
    Vector3f t=(gg).cross(vv);
    // std::cout<<"view:"<<view_pos.x()<<' '<<view_pos.y()<<' '<<view_pos.z()<<std::endl;
    // std::cout<<"gg:"<<gg.x()<<' '<<gg.y()<<' '<<gg.z()<<std::endl;
    // std::cout<<"vv:"<<vv.x()<<' '<<vv.y()<<' '<<vv.z()<<std::endl;
    // std::cout<<"tt:"<<t.x()<<' '<<t.y()<<' '<<t.z()<<std::endl;
    t.normalize();
    Vector4f tt=t.homogeneous();
    return tt;
}
Vector4f produce_view_pos(float radius, float longitude, float latitude){
    Vector4f view_pos;
    view_pos<<radius*sin(latitude)*sin(longitude),radius*sin(latitude)*cos(longitude),radius*cos(latitude),1;
    // std::cout<<view_pos.x()<<' '<<view_pos.y()<<' '<<view_pos.z()<<std::endl;
    return view_pos;
}
#endif