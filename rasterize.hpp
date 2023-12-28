#ifndef RASTERIZE_HPP
#define RASTERIZE_HPP
#include <eigen3/Eigen/Eigen>
#include "transformation.hpp"
#include "triangle.hpp"
#include "environment_light.hpp"
#include "shading.hpp"
#include <math.h>
#include <iostream>
#include <string>
using namespace Eigen;
class rasterize{
private:
    // for 3d vector
    // alpha, beta, gamma
    std::tuple<float,float,float> interpolation(Vector3f v0, Vector3f v1, Vector3f v2, Vector3f p){
        auto x = p[0];
        auto y = p[1];
        float c1 = (x*(v1.y() - v2.y()) + (v2.x() - v1.x())*y + v1.x()*v2.y() - v2.x()*v1.y()) / (v0.x()*(v1.y() - v2.y()) + (v2.x() - v1.x())*v0.y() + v1.x()*v2.y() - v2.x()*v1.y());
        float c2 = (x*(v2.y() - v0.y()) + (v0.x() - v2.x())*y + v2.x()*v0.y() - v0.x()*v2.y()) / (v1.x()*(v2.y() - v0.y()) + (v0.x() - v2.x())*v1.y() + v2.x()*v0.y() - v0.x()*v2.y());
        float c3 = (x*(v0.y() - v1.y()) + (v1.x() - v0.x())*y + v0.x()*v1.y() - v1.x()*v0.y()) / (v2.x()*(v0.y() - v1.y()) + (v1.x() - v0.x())*v2.y() + v0.x()*v1.y() - v1.x()*v0.y());
        return {c1,c2,c3};
    }
    // get the bounding box of the triangle
    // return the left, right, top, bottom
    std::tuple<int,int,int,int> getBoundingBox(triangle t){
        auto v1=t.screen_coord[0].head(3)/t.screen_coord[0][3];
        auto v2=t.screen_coord[1].head(3)/t.screen_coord[1][3];
        auto v3=t.screen_coord[2].head(3)/t.screen_coord[2][3];
        int left =int(std::min(v1.x(), std::min(v2.x(), v3.x()))) ;
        int right = int(std::max(v1.x(), std::max(v2.x(), v3.x())));
        int bottom =int(std::min(v1.y(), std::min(v2.y(), v3.y())));
        int top = int(std::max(v1.y(), std::max(v2.y(), v3.y())));
        return {std::max(left,0),std::min(right,W),std::min(top,H),std::max(bottom,0)};
    }

    // to get the 1d index of a 2d point
    int get_ind(int x, int y){
        return (H-y-1)*W+x;
    }
public:
    // the basic data.
    int H;
    int W;
    Vector4f view_pos;
    Vector4f g,t;
    float eye_fov;
    float n,f;
    environment_light el;
public:
    // kernal
    Matrix3f kernal;
    bool use_kernal;
    void set_kernal(Matrix3f kernal){
        this->kernal = kernal;        
    }
    void set_use_kernal(bool use_kernal){
        this->use_kernal = use_kernal;
    }
    void set_kernal(std::string s){
        if(s=="average"){
            kernal<<1.0/9,1.0/9,1.0/9,
                    1.0/9,1.0/9,1.0/9,
                    1.0/9,1.0/9,1.0/9;
        }
        if(s=="gaussion"){
            kernal<<1.0/16,2.0/16,1.0/16,
                    2.0/16,4.0/16,2.0/16,
                    1.0/16,2.0/16,1.0/16;
        }
        if(s=="sharpening"){
            kernal<<-1,-1,-1,
                    -1,9,-1,
                    -1,-1,-1;
        }
    }
public:
    // the transformation matrix
    Matrix4f model;
    Matrix4f view;
    Matrix4f projection;
    Matrix4f screen;
public:
    // the buffer to store the depth and color of every point
    float *dep_buf; // store the depth of points
    Vector3f *col_buf; // store the color of points
public:
    // functions
    // we need set these parameterization before we use the rasterize
    void set_view_pos(Vector4f view_pos){
        this->view_pos = view_pos;
    }
    void set_view_dir(Vector4f g, Vector4f t){
        this->g = g;
        this->t = t;
    }
    void set_eye_fov(float eye_fov){
        this->eye_fov = eye_fov;
    }
    void set_near_far(float n, float f){
        this->n = n;
        this->f = f;
    }
    void set_model(){
        model = get_Model_Matrix();
    }
    void set_view(){
        view = get_View_Matrix(view_pos, g, t);
    }
    void set_projection(){
        projection = get_Projection_Matrix(eye_fov, n, f);
    }
    void set_screen(){
        screen = get_Screen_Matrix(W, H);
    }
public:
    // set for the environment light
    void add_light(environment_light el){
        this->el = el;
    }
    void add_light(light l){
        el.add_light(l);
    }
    void del_light(int i){
        el.del_light(i);
    }
    void clear_light(){
        el.clear();
    }
public:
    void clear_buf(){
        for (int i = 0; i < H*W; i++){
            dep_buf[i] = -100000;
            col_buf[i] = Vector3f(0, 0, 0);
        }
    }
    void all_rasterize(triangles ts);
    void single_rasterize(const triangle &t,int left,int right,int top,int bottom);
    rasterize(int H, int W){
        this->H = H;
        this->W = W;
        use_kernal = false;
        dep_buf = new float[H*W+10];
        col_buf = new Vector3f[H*W+10];
        for (int i = 0; i < H*W; i++){
            dep_buf[i] = -100000;
            col_buf[i] = Vector3f(0, 0, 0);
        }
    }
};
// rasterize all the triangles
// the points here is in the space coordinates.
void rasterize::all_rasterize(triangles ts){
    // because we use the blin-Phong rasteriazation
    // so we need the view coordinates and the normal of every points
    // Meanwhile we only use the MV transformation
    auto MV = view * model;

    for(auto &l:el.lights){
        l.view_pos = MV * l.pos;
    }
    // for(int i=0;i<4;i++)
    //     std::cout<<l.view_pos[i]<<' ';
    // std::cout<<std::endl;
    // for(int i=0;i<4;i++)
    //     std::cout<<l.pos[i]<<' ';
    // std::cout<<std::endl;
    int bottom_min=1000;
    int top_max=0;
    int left_min=1000;
    int right_max=0;
    for (auto &t: ts.tri){
        for (int i=0;i<3;i++){
            // to get the view coordinates
            t.view_coord[i] = MV * t.v[i];
        }
        for (int i=0;i<3;i++){
            // to get the screen coodinates
            t.screen_coord[i] =screen * projection* t.view_coord[i];
        }
        for (int i=0;i<3;i++){
            t.view_normal[i]=((MV.inverse().transpose()*(t.normal[i].homogeneous())).normalized()).head(3);
        }
        // t stores the screen coordinates
        // and the view coordinates
        // view coordinates are used to simulate the light
        // screen coordinates are used to rasterize
        // std::cout<<l.view_pos.x()<<' '<<l.view_pos.y()<<' '<<l.view_pos.z()<<std::endl;
        auto [left,right,top,bottom] = getBoundingBox(t);
        left_min=std::min(left_min,left);
        right_max=std::max(right_max,right);
        top_max=std::max(top_max,top);
        bottom_min=std::min(bottom_min,bottom);
        single_rasterize(t,left,right,top,bottom);

        // use convolution kernal to smooth the image
        // consider the bounding pixel as 0
        
    }
    if(use_kernal){
        for(int i=left_min;i<=right_max;i++)
            for(int j=bottom_min;j<=top_max;j++){
                int c11,c12,c13,c21,c22,c23,c31,c32,c33;
                c11=get_ind(i-1,j-1);
                c12=get_ind(i-1,j);
                c13=get_ind(i-1,j+1);
                c21=get_ind(i,j-1);
                c22=get_ind(i,j);
                c23=get_ind(i,j+1);
                c31=get_ind(i+1,j-1);
                c32=get_ind(i+1,j);
                c33=get_ind(i+1,j+1);
                Matrix3f color_matrix;
                int color_after_convolution=0;
                color_matrix<<col_buf[c11].x(),col_buf[c12].x(),col_buf[c13].x(),
                            col_buf[c21].x(),col_buf[c22].x(),col_buf[c23].x(),
                            col_buf[c31].x(),col_buf[c32].x(),col_buf[c33].x();
                color_after_convolution=color_matrix.cwiseProduct(kernal).sum();
                col_buf[c22]<<color_after_convolution,color_after_convolution,color_after_convolution;
            }
        }
}
// the point here is in screen coordinates
// but the shade need to be in the space coordinates
void rasterize::single_rasterize(const triangle &t,int left,int right,int top,int bottom){
    Vector3f v1 = t.screen_coord[0].head(3)/t.screen_coord[0][3];
    Vector3f v2 = t.screen_coord[1].head(3)/t.screen_coord[1][3];
    Vector3f v3 = t.screen_coord[2].head(3)/t.screen_coord[2][3];
    Vector3f vp1 = t.view_coord[0].head(3)/t.view_coord[0][3];
    Vector3f vp2 = t.view_coord[1].head(3)/t.view_coord[1][3];
    Vector3f vp3 = t.view_coord[2].head(3)/t.view_coord[2][3];
    // std::cout<<left<<' '<<right<<' '<<top<<' '<<bottom<<std::endl;
    for (int i = left; i <= right; i++){
        for (int j = bottom; j <= top; j++){
            Vector3f p(i, j, 0);
            auto [alpha, beta, gamma] = interpolation(v1, v2, v3, p);
            // std::cout<< alpha<< ' '<<beta<< ' '<<gamma<<std::endl;
            // std::cout << t.color[0].x()<<' '<<t.color[0].y()<< ' '<<t.color[0].z() << std::endl;
            if (alpha >= 0 && beta >= 0 && gamma >= 0){
                // z is dep of the point
                // the smaller z is, the deeper the point is
                float z = alpha * v1[2] + beta * v2[2] + gamma * v3[2];
                int ind=get_ind(i, j);
                if (z > dep_buf[ind]){
                    dep_buf[ind] = z;
                    // other parameters
                    auto p=150.0f;
                    auto ka=Vector3f(0.005,0.005,0.005);
                    auto ks=Vector3f(0.7937,0.7937,0.7937);
                    auto vp = alpha*vp1+beta*vp2+gamma*vp3;
                    auto normal = alpha*t.view_normal[0]+beta*t.view_normal[1]+gamma*t.view_normal[2];
                    auto ori_col = alpha * t.color[0] + beta * t.color[1] + gamma * t.color[2];
                    auto col = Blin_Phong_shading(vp, normal, el, ka, ori_col/256, ks, p);
                    col_buf[ind] = col;
                    // std::cout<<col_buf[ind]<<std::endl;
                }
            }
        }
    }
}
#endif