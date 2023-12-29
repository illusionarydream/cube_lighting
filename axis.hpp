#ifndef AXIS_HPP
#define AXIS_HPP
#include <eigen3/Eigen/Eigen>
#include <vector>
#include <string>
#include <iostream>
#include <FL/Fl_box.H>
#include "environment_light.hpp"
#include "transformation.hpp"
#include "sphere_calculation.hpp"
#include "FL/fl_draw.H"
using namespace Eigen;
class Axis_box:public Fl_Box{
private:
    Vector4f x_axis[2];
    Vector4f y_axis[2];
    Vector4f z_axis[2];
    std::vector<light> lights;
    Vector4f view_pos;
    Vector4f g,t;
    Matrix4f MVPS;
    float eye_fov;
    float n,f;
    int W;
    int H;
    int X_ori;
    int Y_ori;
public:
    Axis_box(int x,int y,int W,int H):Fl_Box(x,y,W,H){
        this->W=W;
        this->H=H;
        X_ori=x+50;
        Y_ori=y+90;
        x_axis[0]=Vector4f(-5,0,0,1);
        x_axis[1]=Vector4f(5,0,0,1);
        y_axis[0]=Vector4f(0,-5,0,1);
        y_axis[1]=Vector4f(0,5,0,1);
        z_axis[0]=Vector4f(0,0,-5,1);
        z_axis[1]=Vector4f(0,0,5,1);
    }
    void set_view_pos(Vector4f view_pos){
        this->view_pos=view_pos;
        this->g=from_pos_to_g(view_pos);
        this->t=from_pos_to_t(view_pos);
    }
    void set_eye_fov(float eye_fov){
        this->eye_fov=eye_fov;
    }
    void set_near_far(float n,float f){
        this->n=n;
        this->f=f;
    }
    // get the MVPS
    // and transfer the axis
    void get_MVPS(){
        Matrix4f model=get_Model_Matrix();
        Matrix4f view=get_View_Matrix(view_pos,g,t);
        Matrix4f projection=get_Projection_Matrix(eye_fov,n,f);
        Matrix4f screen=get_Screen_Matrix(300,300);
        MVPS=screen*projection*view*model;
        for(int i=0;i<=1;i++){
            x_axis[i]=MVPS*x_axis[i];
            x_axis[i]=x_axis[i]/x_axis[i][3];
            y_axis[i]=MVPS*y_axis[i];
            y_axis[i]=y_axis[i]/y_axis[i][3];
            z_axis[i]=MVPS*z_axis[i];
            z_axis[i]=z_axis[i]/z_axis[i][3];
        }
    }
    void add_light(light l){
        l.pos=MVPS*l.pos;
        l.pos=l.pos/l.pos[3];
        lights.push_back(l);
    }
    void del_light(){
        lights.pop_back();
    }
    void draw_arrow(int x1, int y1, int x2, int y2, int arrow_size, std::string text = "") {
        // 计算线条的角度
        double angle = atan2(y2 - y1, x2 - x1) * 180 / M_PI;

        // 绘制线条
        fl_color(FL_BLACK);
        fl_line_style(FL_SOLID, 2);
        fl_line(x1, y1, x2, y2);

        // 计算箭头的两个点
        int x3 = x2 - arrow_size * cos((angle - 30) * M_PI / 180);
        int y3 = y2 - arrow_size * sin((angle - 30) * M_PI / 180);
        int x4 = x2 - arrow_size * cos((angle + 30) * M_PI / 180);
        int y4 = y2 - arrow_size * sin((angle + 30) * M_PI / 180);

        // 绘制箭头
        fl_color(FL_BLACK);
        fl_polygon(x2, y2, x3, y3, x4, y4);
        // 绘制文本
        fl_color(FL_BLACK);
        fl_font(FL_HELVETICA, 16); // 设置字体和大小
        fl_draw(text.c_str(), x2, y2 - arrow_size-2); // 在箭头上方绘制文本
    }
    void draw_light(int x,int y,int size){
        fl_color(224,177,63);
        fl_circle(x,y,size);

        // 绘制四条短线
        
        fl_color(224,177,63);
        fl_line(x - size, y, x + size, y); // 水平线
        fl_line(x, y - size, x, y + size); // 垂直线
        fl_line(x - size / sqrt(2), y - size / sqrt(2), x + size / sqrt(2), y + size / sqrt(2)); // 对角线1
        fl_line(x - size / sqrt(2), y + size / sqrt(2), x + size / sqrt(2), y - size / sqrt(2)); // 对角线2
    }
    void draw(){
        Fl_Box::draw();
        for(int i=0;i<lights.size();i++){
            draw_light(lights[i].pos[0]+X_ori,lights[i].pos[1]+Y_ori,4);
        }
        
        draw_arrow(x_axis[0][0]+X_ori,x_axis[0][1]+Y_ori,x_axis[1][0]+X_ori,x_axis[1][1]+Y_ori,8,"x");
        draw_arrow(y_axis[0][0]+X_ori,y_axis[0][1]+Y_ori,y_axis[1][0]+X_ori,y_axis[1][1]+Y_ori,8,"y");
        draw_arrow(z_axis[0][0]+X_ori,z_axis[0][1]+Y_ori,z_axis[1][0]+X_ori,z_axis[1][1]+Y_ori,8,"z");
    }
};
#endif