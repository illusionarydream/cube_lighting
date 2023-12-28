#ifndef ENVIRONMENT_LIGHT_HPP
#define ENVIRONMENT_LIGHT_HPP
#include <eigen3/Eigen/Eigen>
#include <vector>
using namespace Eigen;
struct light{
    Vector3f intensity;
    Vector4f pos;
    Vector4f view_pos;
    light() {}
    void set_intensity(Vector3f intensity){
        this->intensity=intensity;
    }
    void set_pos(Vector4f pos){
        this->pos=pos;
    }
};
class environment_light{
public:
    std::vector<light> lights;
    environment_light() {}
    void add_light(light l){
        lights.push_back(l);
    }
    void del_light(int i){
        lights.erase(lights.begin()+i);
    }
    void clear(){
        lights.clear();
    }
};
#endif