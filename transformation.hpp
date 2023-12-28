#ifndef TRANSFORMATION_HPP
#define TRANSFORMATION_HPP
#include <eigen3/Eigen/Eigen>
#include <iostream>
using namespace Eigen;
// the model will be set in the [-2,2] [-2,2] [-2,2] cube
// the camera will put on a huge sphere with radius 4
// no use now but maybe useful in the future.
// because the model I set won't be moved anymore.
Matrix4f get_Model_Matrix(){
    Matrix4f model;
    model << 1, 0, 0, 0,
             0, 1, 0, 0,
             0, 0, 1, 0,
             0, 0, 0, 1;
    return model;
}

// move p to the (0,0,0)
// move g to the -z
// move t to the y
// then g cross t is the x
Matrix4f get_View_Matrix(Vector4f p,Vector4f g,Vector4f t){
    Matrix4f R;
    Vector3f g_=g.head(3).normalized();
    Vector3f t_=t.head(3).normalized();
    Vector4f e= g_.cross(t_).homogeneous();
    // std::cout<<"g:"<<g.x()<<' '<<g.y()<<' '<<g.z()<<std::endl;
    // std::cout<<"t:"<<t.x()<<' '<<t.y()<<' '<<t.z()<<std::endl;
    R << 
        e.x(), e.y(), e.z(), 0,
        t.x(), t.y(), t.z(), 0,
        -g.x(), -g.y(), -g.z(), 0,
        0, 0, 0, 1;
    // move the p to the (0,0,0)
    Matrix4f M;
    M << 
        1, 0, 0, -p[0],
        0, 1, 0, -p[1],
        0, 0, 1, -p[2],
        0, 0, 0, 1;
    return R*M;
}

// because the object is compulsory in the [-2,2] [-2,2] [-2,2] cube
// so the projection matrix is very simple
// l=-r
// b=-t
// |l|=|r|=|b|=|t|
// eye_fov need to be the radian
// n f is the near and far and both negative.
Matrix4f get_Projection_Matrix(float eye_fov, float n, float f){
    float t = -n * tan(eye_fov / 2);
    float b = -t;
    float r = t;
    float l = -r;

    // squize the cube to the [-1,1] [-1,1] [-1,1]
    Matrix4f orth;
    orth<<
        2/(r-l), 0, 0, -(r+l)/(r-l),
        0, 2/(t-b), 0, -(t+b)/(t-b),
        0, 0, 2/(n-f), -(n+f)/(n-f),
        0, 0, 0, 1;

    // squize the perspective to the cube
    Matrix4f persp;
    persp<<
        n, 0, 0, 0,
        0, n, 0, 0,
        0, 0, n+f, -n*f,
        0, 0, 1, 0;
    return orth*persp;
}
Matrix4f get_Screen_Matrix(int w, int h){
    Matrix4f screen;
    screen<<
        w/2, 0, 0, w/2,
        0, h/2, 0, h/2,
        0, 0, 1, 0,
        0, 0, 0, 1;
    return screen;
}
#endif