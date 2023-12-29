#ifndef CUBE_WINDOW_HPP
#define CUBE_WINDOW_HPP
#include <eigen3/Eigen/Eigen>
#include "environment_light.hpp"
#include "Fl/Fl.H"
#include "Fl/Fl_Double_Window.H"
#include "Fl/Fl_Box.H"
#include "Fl/Fl_Button.H"
#include "Fl/Fl_PNG_Image.H"
#include "Fl/Fl_Input.H"
#include "Fl/Fl_Output.H"
#include "Fl/Fl_Browser.H"
#include "Fl/Fl_Progress.H"
#include "FL/Fl_Choice.H"
#include "triangle.hpp"
#include "transformation.hpp"
#include "rasterize.hpp"
#include "OBJ_Loader.hpp"
#include "sphere_calculation.hpp"
#include "axis.hpp"
#include <opencv2/opencv.hpp>
#include <queue>
#define M_PI 3.14159265358979323846
class ClickableBox : public Fl_Box {
private:
    int ind;
    // std::queue<int> dx_list;
    // std::queue<int> dy_list;
    int sum_x,sum_y;
    int last_x,last_y;
    int longitude, latitude;
    int pace;
    int sensitivity;
    std::vector<Fl_PNG_Image*> images;
public:
    ClickableBox(int X, int Y, int W, int H, const char *L = 0) : 
        Fl_Box(X,Y,W,H,L), 
        ind(0),
        last_x(Fl::event_x()),
        last_y(Fl::event_y()),
        longitude(1),
        latitude(1),
        sensitivity(1),
        pace(60),
        sum_x(0),
        sum_y(0)
        {}
    void load_images() {
        for (int i = 1; i < 360; i+=pace) 
            for (int j = 1; j <180; j+=pace){
                std::string filename = "../image/image_" + std::to_string(i) + "_" + std::to_string(j) + ".png";
                Fl_PNG_Image* img = new Fl_PNG_Image(filename.c_str());
                // std::cout<<img<<std::endl;
                std::cout<<filename<<std::endl; 
                std::cout<<images.size()<<std::endl;
                images.push_back(img);
        }
    }
    // use the latitude and longitude to set the image
    // latitude and longtitude is private member of box      
    void set_image() {
        int index=(latitude-1) / pace + (longitude-1) / pace * (178 / pace+1);
        // std::cout<<index<<std::endl;
        // std::cout<<images.size()<<std::endl;
        if (index >= 0 && index < images.size()) {
            // Fl_PNG_Image *img = new Fl_PNG_Image("image.png", &images[index][0], images[index].size());
            Fl::lock();
            Fl_Image* scaled = images[index]->copy(512,512);  // 添加这行
            image(scaled);  // 修改这行
            redraw();
            Fl::unlock();
        }
    }
    void set_sensitive(int s) {
        sensitivity = s;
    }
    void set_pace(int p) {
        pace = p;
    }
    int handle(int event) override {
        // std::cout<<1;
        int dx,dy;
        int dlongitude,dlatitude;
        switch(event) {
            case FL_PUSH:
                // std::cout<<1<<std::endl;
                last_x=Fl::event_x();
                last_y=Fl::event_y();
                return 1;
            case FL_DRAG:
                dx = Fl::event_x() - last_x;
                dy = Fl::event_y() - last_y;
                sum_x+=dx;
                sum_y+=dy;
                // std::cout<<dx<<" "<<dy<<std::endl;
                last_x=Fl::event_x();
                last_y=Fl::event_y();
                if(sum_x>=sensitivity){
                    dlongitude=pace;
                    sum_x-=sensitivity;
                }
                else if(sum_x<=-sensitivity){
                    dlongitude=-pace;
                    sum_x+=sensitivity;
                }
                else
                    dlongitude=0;
                if(sum_y>=sensitivity){
                    dlatitude=pace;
                    sum_y-=sensitivity;
                }
                else if(sum_y<=-sensitivity){
                    dlatitude=-pace;
                    sum_y+=sensitivity;
                }
                else
                    dlatitude=0;
                // if longitude is out of range, then set it to 0
                longitude=(longitude-dlongitude+3600000)%360;
                // if latitude is out of 180, then set it to 180
                latitude=latitude+dlatitude;
                if(latitude>180)
                    latitude=pace*(179/pace)+1;
                if(latitude<0)
                    latitude=1;
                std::cout<<longitude<<" "<<latitude<<std::endl;
                set_image();
                return 1;
            default:
                last_x=Fl::event_x();
                last_y=Fl::event_y();
                return Fl_Box::handle(event);
        }
    }
    void redraw(){
        parent()->redraw();
    }
};
class cube_window : public Fl_Double_Window {
private:
    int W;
    int H;
    int step;
    int sensitivity;
    int pace;
    int supersampling_num;
    
private:
    // the first part
    // the welcome part
    Fl_Box start_box;
    Fl_Button start_button;
private:
    // the second part
    // you should input the light in here
    Fl_Input x_input;
    Fl_Input y_input;
    Fl_Input z_input;
    Fl_Input intensity_input;
    Axis_box axis_box;
    Fl_Browser browser;
    environment_light el;
    Fl_Button add_button; 
    Fl_Button delete_button;
    Fl_Button continue_button;
    Fl_Button Back_button1;
private:
    // the third part
    // you can choose the kernel and the resolution
    Fl_Choice MSAA_button;
    Fl_Choice kernel_button;
    Fl_Choice resolution_button;
    Fl_Button rasterize_button;
    Fl_Button Back_button2; 
private:
    // the fourth part
    // it will show the rasterize process
    Fl_Progress progress;
    Fl_Box box_show_in_process;
private:
    // the fifth part
    // it will show the result
    // Fl_Box box;
    ClickableBox box;
private:
    bool judge_if_in_the_cube(int x,int y,int z){
        if(std::fabs(x)<=1.05&&std::fabs(y)<=1.05&&std::fabs(z)<=1.05)
            return true;
        return false;
    }
    static void add_cb(Fl_Widget *w, void *data) {
        cube_window *window = (cube_window*)data;
        float x,y,z,intensity;
        try{
            x=std::stof(window->x_input.value());
            y=std::stof(window->y_input.value());
            z=std::stof(window->z_input.value());
            intensity=std::stof(window->intensity_input.value());
        }
        catch(...){
            window->x_input.value("");
            window->y_input.value("");
            window->z_input.value("");
            window->intensity_input.value("");
            return ;
        }
        std::cout<<x<<' '<<y<<' '<<z<<' '<<intensity<<std::endl;
        if(window->judge_if_in_the_cube(x,y,z)==1||intensity<=0||intensity>=25||std::abs(x)>=5||std::abs(y)>=5||std::abs(z)>=5){
            window->x_input.value("");
            window->y_input.value("");
            window->z_input.value("");
            window->intensity_input.value("");
            return ;
        }
        light *l=new light();
        l->set_intensity(Vector3f(intensity, intensity, intensity));
        l->set_pos(Vector4f(x, y, z, 1));
        window->el.add_light(*l);
        window->axis_box.add_light(*l);
        window->x_input.value("");
        window->y_input.value("");
        window->z_input.value("");
        window->intensity_input.value("");
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(1);  // 设置精度为2
        stream << "X: " << x << ", Y: " << y << ", Z: " << z << ", Intensity: " << intensity;
        std::string light_source_info = stream.str();
        window->browser.add(light_source_info.c_str());
        window->redraw();
        // std::cout<<"add"<<std::endl;
    }
    static void delete_cb(Fl_Widget *w, void *data) {
        cube_window *window = (cube_window*)data;
        int index = window->browser.size();
        std::cout<<index<<std::endl;
        std::cout<<window->el.lights.size()<<std::endl;
        if (index > 0) {
            window->el.del_light(index - 1);
            window->axis_box.del_light();
            window->browser.remove(index);
        }
        window->redraw();
        // std::cout<<"delete"<<std::endl;
    }
    void renew_progress(){
        progress.value(step);  // 更新进度条的值
        Fl::check(); 
    }
    static void rasterize_cb(Fl_Widget *w, void *data) {
        cube_window *window = (cube_window*)data;
        window->rasterize_button.deactivate();
        window->rasterize_button.hide();
        window->Back_button2.deactivate();  
        window->Back_button2.hide();
        window->box.box(FL_FLAT_BOX);
        window->box.color(FL_WHITE);
        window->box.set_sensitive(window->sensitivity);
        window->box.set_pace(window->pace); 
        
        // box.set_image(0);
        // box.image(new Fl_PNG_Image("image.png"));
        triangles ts;
        // set the supersampling_num
        std::string MSAA=window->MSAA_button.text(window->MSAA_button.value());
        if(MSAA=="none")
            window->supersampling_num=1;
        else if(MSAA=="MSAA 2*2")
            window->supersampling_num=2;
        else if(MSAA=="MSAA 3*3")
            window->supersampling_num=3;
        else if(MSAA=="MSAA 4*4")
            window->supersampling_num=4;
        int supersampling_num=window->supersampling_num;
        window->MSAA_button.deactivate();
        window->MSAA_button.hide();
        // set the kernal
        std::string s=window->kernel_button.text(window->kernel_button.value());
        window->kernel_button.deactivate();
        window->kernel_button.hide();

        int pace=window->pace;
        // set the resolution
        std::string resolution=window->resolution_button.text(window->resolution_button.value());
        window->resolution_button.deactivate();
        window->resolution_button.hide();

        window->progress.activate();
        window->progress.show();
        window->box_show_in_process.show();
        window->box_show_in_process.activate();

        int W,H;
        if(resolution=="64*64"){
            W=64;
            H=64;
        }
        else if(resolution=="128*128"){
            W=128;
            H=128;
        }
        else if(resolution=="256*256"){
            W=256;
            H=256;
        }
        else if(resolution=="512*512"){
            W=512;
            H=512;
        }
        // supersampling
        if(supersampling_num>1){
            W*=supersampling_num;
            H*=supersampling_num;
        }
        
        window->renew_progress();
        // set the light
        // Load .obj File
        objl::Loader Loader;
        std::string obj_path = "../model/cube/";
        bool loadout = Loader.LoadFile("../model/cube/cube.obj");
        for(auto mesh:Loader.LoadedMeshes)
        {
            for(int i=0;i<mesh.Vertices.size();i+=3)
            {
                triangle *t=new triangle();
                for(int j=0;j<3;j++)
                {
                    Vector3f color(120,120,120);
                    t->setv(j,Vector4f(mesh.Vertices[i+j].Position.X,mesh.Vertices[i+j].Position.Y,mesh.Vertices[i+j].Position.Z,1.0));
                    t->setNormal(j,Vector3f(mesh.Vertices[i+j].Normal.X,mesh.Vertices[i+j].Normal.Y,mesh.Vertices[i+j].Normal.Z));
                    t->setColor(color,color,color);   
                }
                ts.add_triangle(*t);
                // std::cout<<"normal:"<<std::endl;
                // std::cout<<t->normal[0].x()<<' '<<t->normal[0].y()<<' '<<t->normal[0].z()<<std::endl;
                // std::cout<<t->normal[1].x()<<' '<<t->normal[1].y()<<' '<<t->normal[1].z()<<std::endl;
                // std::cout<<t->normal[2].x()<<' '<<t->normal[2].y()<<' '<<t->normal[2].z()<<std::endl;
                // std::cout<<"vertex:"<<std::endl;
                // std::cout<<t->v[0].x()<<' '<<t->v[0].y()<<' '<<t->v[0].z()<<std::endl;
                // std::cout<<t->v[1].x()<<' '<<t->v[1].y()<<' '<<t->v[1].z()<<std::endl;
                // std::cout<<t->v[2].x()<<' '<<t->v[2].y()<<' '<<t->v[2].z()<<std::endl;
            }
        }
        // rastrize
        rasterize r(W, H);
        r.set_eye_fov(60.0f / 180.0f * M_PI);
        r.set_near_far(-0.5f, -10.0f);
        r.set_model();
        r.set_projection();
        r.set_screen();
        r.add_light(window->el);
        if(s=="none")
            r.set_use_kernal(false);
        else{
            r.set_use_kernal(true);
            r.set_kernal(s);
        }
        // set the view position and direction
        // the i is set as the longitude, the j is set as the latitude
        for(int i=1;i<360;i+=pace)
            for(int j=1;j<180;j+=pace){
                // set basic information
                auto view_pos=produce_view_pos(8.0f,i/180.0f*M_PI,j/180.0f*M_PI);
                auto g=from_pos_to_g(view_pos);
                auto t=from_pos_to_t(view_pos);
                r.set_view_pos(view_pos);
                r.set_view_dir(g,t);
                r.set_view();
                // set kernal
                // r.set_use_kernal(false);
                // r.set_kernal("gaussion");
                // rasterize    
                r.clear_buf();
                r.all_rasterize(ts);
                // transfer into the opencv format
                // supersampling
                Vector3f *col_buf=new Vector3f[W*H];
                if(supersampling_num>1){
                    for(int ii=0;ii<W;ii+=supersampling_num)
                        for(int jj=0;jj<H;jj+=supersampling_num){
                            int x=ii/supersampling_num;
                            int y=jj/supersampling_num;
                            float sum_r=0,sum_g=0,sum_b=0;
                            for(int k=0;k<supersampling_num;k++)
                                for(int l=0;l<supersampling_num;l++){
                                    // std::cout<<ii+k<<' '<<jj+l<<std::endl;
                                    // std::printf("%d %d\n", H,W);
                                    sum_r+=r.col_buf[(H-(jj+l+1))*W+ii+k].x();
                                    sum_g+=r.col_buf[(H-(jj+l+1))*W+ii+k].y();
                                    sum_b+=r.col_buf[(H-(jj+l+1))*W+ii+k].z();
                                }
                            // std::cout<<x<<' '<<y<<std::endl;
                            col_buf[(H/supersampling_num-y-1)*W/supersampling_num+x]=Vector3f(sum_r,sum_g,sum_b)/(supersampling_num*supersampling_num);
                        }
                }
                cv::Mat image(W/supersampling_num , H/supersampling_num, CV_32FC3, supersampling_num==1? r.col_buf:col_buf);
                image.convertTo(image, CV_8UC3, 1.0f);
                cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
                // cv::imshow("image", image);
                std::vector<uchar> buf;
                // cv::imencode(".png", image, buf);
                // images.push_back(buf);
                std::string filename = "../image/image_" + std::to_string(i) + "_" + std::to_string(j) + ".png";
                std::printf("%s\n", filename.c_str());
                cv::imwrite(filename, image);
                window->step++;
                Fl_Image *img = new Fl_PNG_Image(filename.c_str());
                Fl_Image* scaled = img->copy(512,512);   
                window->renew_progress();
                window->box_show_in_process.image(scaled);
                window->box_show_in_process.redraw();  
            }
        window->progress.hide();
        window->progress.deactivate();
        window->box_show_in_process.hide();
        window->box_show_in_process.deactivate();
        window->box.load_images();
        window->box.set_image();
        window->box.activate();
        window->box.show();
        window->box.redraw();
    }
    static void start_cb(Fl_Widget *w, void *data) {
        cube_window *window = (cube_window*)data;
        window->start_button.deactivate();
        window->start_button.hide();
        window->start_box.deactivate();
        window->start_box.hide();
        window->x_input.activate();
        window->y_input.activate();
        window->z_input.activate();   
        window->intensity_input.activate();
        window->add_button.activate();
        window->delete_button.activate();
        window->browser.activate();
        window->continue_button.activate(); 
        window->Back_button1.activate();
        window->x_input.show();
        window->y_input.show();
        window->z_input.show();
        window->intensity_input.show();
        window->add_button.show();
        window->delete_button.show();
        window->browser.show();
        window->continue_button.show();
        window->Back_button1.show();
        window->axis_box.show();
        window->redraw();
    }
    static void continue_cb(Fl_Widget *w, void *data) {
        cube_window *window = (cube_window*)data;
        window->x_input.deactivate();
        window->y_input.deactivate();
        window->z_input.deactivate();
        window->intensity_input.deactivate();
        window->add_button.deactivate();
        window->delete_button.deactivate();
        window->rasterize_button.deactivate();
        window->browser.deactivate();
        window->continue_button.deactivate();
        window->Back_button1.deactivate();
        window->axis_box.deactivate();
        window->Back_button1.hide();
        window->x_input.hide();
        window->y_input.hide();
        window->z_input.hide();
        window->intensity_input.hide();
        window->add_button.hide();
        window->delete_button.hide();
        window->rasterize_button.hide();
        window->browser.hide();
        window->continue_button.hide();
        window->axis_box.hide();
        window->MSAA_button.activate();
        window->kernel_button.activate();
        window->resolution_button.activate();
        window->rasterize_button.activate();
        window->Back_button2.activate();
        window->MSAA_button.show();
        window->kernel_button.show();
        window->resolution_button.show();
        window->rasterize_button.show();
        window->Back_button2.show();
        window->redraw();
    }
    static void back_cb1(Fl_Widget *w, void *data) {
        cube_window *window = (cube_window*)data;
        window->x_input.deactivate();
        window->y_input.deactivate();
        window->z_input.deactivate();
        window->intensity_input.deactivate();
        window->add_button.deactivate();
        window->delete_button.deactivate();
        window->rasterize_button.deactivate();
        window->browser.deactivate();
        window->continue_button.deactivate();
        window->Back_button1.deactivate();
        window->Back_button1.hide();
        window->x_input.hide();
        window->y_input.hide();
        window->z_input.hide();
        window->intensity_input.hide();
        window->add_button.hide();
        window->delete_button.hide();
        window->rasterize_button.hide();
        window->browser.hide();
        window->continue_button.hide();
        window->start_button.activate();
        window->start_box.activate();
        window->start_button.show();
        window->start_box.show();
        window->redraw();
    }
    static void back_cb2(Fl_Widget *w, void *data) {
        cube_window *window = (cube_window*)data;
        window->MSAA_button.deactivate();
        window->kernel_button.deactivate();
        window->resolution_button.deactivate();
        window->rasterize_button.deactivate();
        window->Back_button2.deactivate();
        window->Back_button2.hide();
        window->MSAA_button.hide();
        window->kernel_button.hide();
        window->resolution_button.hide();
        window->rasterize_button.hide();
        window->x_input.activate();
        window->y_input.activate();
        window->z_input.activate();   
        window->intensity_input.activate();
        window->add_button.activate();
        window->delete_button.activate();
        window->browser.activate();
        window->continue_button.activate(); 
        window->Back_button1.activate();
        window->axis_box.activate();
        window->x_input.show();
        window->y_input.show();
        window->z_input.show();
        window->intensity_input.show();
        window->add_button.show();
        window->delete_button.show();
        window->browser.show();
        window->continue_button.show();
        window->Back_button1.show();
        window->axis_box.show();
        window->redraw();
    }
public:

    cube_window(int w, int h, int p, int s,const char *title = 0):
        Fl_Double_Window(w, h, title), 
        W(w), 
        H(h),
        pace(p),
        step(0),
        sensitivity(s),
        supersampling_num(1),
        start_box(0, 0, w, h, ""),
        start_button(w/2-75, h/2+50, 150, 45, "Start"),

        x_input(20, 50, 80, 30, "X"),
        y_input(20, 80, 80, 30, "Y"),
        z_input(20, 110, 80, 30, "Z"),
        intensity_input(20, 140, 80, 30, "I"),
        add_button(0, 170, 100, 30, "Add"),
        delete_button(0, 200, 100, 30, "Delete"),
        continue_button(0, 402, 100, 30, "Continue"),
        Back_button1(0, 432, 100, 30, "Back"),
        browser(0, 230, 100, 202, ""),
        axis_box(100, 50, 412, 412),

        kernel_button(w/2-75, 180, 150, 30, "Kernel"),
        resolution_button(w/2-75, 210, 150, 30, "Resolution"),
        MSAA_button(w/2-75, 240, 150, 30, " MSAA "),
        rasterize_button(w/2-100, 270, 100, 30, "Rasterize"),
        Back_button2(w/2, 270, 100, 30, "Back"),
        
        box_show_in_process(0, -20, w, h),
        progress(0, h-20, w, 20, ""),

        box(0, 0, w, h)
        {
        color(FL_GRAY);
        Fl_PNG_Image *img = new Fl_PNG_Image("../background.png");
        start_box.image(img);
        add_button.callback(add_cb, this);
        delete_button.callback(delete_cb, this);
        rasterize_button.callback(rasterize_cb, this);
        start_button.callback(start_cb, this);
        continue_button.callback(continue_cb, this);
        Back_button1.callback(back_cb1, this);
        Back_button2.callback(back_cb2, this);
        progress.minimum(0);
        progress.maximum((358/pace+1)*(178/pace+1));
        progress.selection_color(FL_GRAY);
        progress.hide();
        progress.deactivate();
        Fl_PNG_Image *img2 = new Fl_PNG_Image("../white_background.png");
        axis_box.image(img2->copy(412,412));
        axis_box.set_eye_fov(60.0f / 180.0f * M_PI);
        axis_box.set_near_far(-0.5f, -10.0f);
        axis_box.set_view_pos(Vector4f(5,5,5,1));
        axis_box.get_MVPS();
        box.deactivate();
        box.hide();
        box_show_in_process.hide();
        axis_box.hide();
        x_input.hide();
        y_input.hide();
        z_input.hide();
        intensity_input.hide();
        add_button.hide();
        delete_button.hide();
        rasterize_button.hide();
        browser.hide();
        MSAA_button.hide();
        kernel_button.hide();
        resolution_button.hide();
        continue_button.hide();
        Back_button1.hide();
        Back_button2.hide();
        Back_button1.deactivate();
        Back_button2.deactivate();
        box_show_in_process.deactivate();
        axis_box.deactivate();
        x_input.deactivate();
        y_input.deactivate();
        z_input.deactivate();
        intensity_input.deactivate();
        add_button.deactivate();
        delete_button.deactivate();
        rasterize_button.deactivate();
        browser.deactivate();
        MSAA_button.deactivate();
        kernel_button.deactivate();
        resolution_button.deactivate();
        continue_button.deactivate();
        kernel_button.add("none");
        kernel_button.add("average");
        kernel_button.add("gaussion");
        kernel_button.add("sharpening");
        kernel_button.value(0);
        resolution_button.add("64*64");
        resolution_button.add("128*128");
        resolution_button.add("256*256");
        resolution_button.add("512*512");
        resolution_button.value(3);
        MSAA_button.add("none");
        MSAA_button.add("MSAA 2*2");
        MSAA_button.add("MSAA 3*3");
        MSAA_button.add("MSAA 4*4");
        MSAA_button.value(0);
        // box.box(FL_FLAT_BOX);
        // box.color(FL_WHITE);
        // box.set_sensitive(s);
        // box.set_pace(p); 
        // box.load_images();
        // box.set_image();
        // box.set_image(0);
        // box.image(new Fl_PNG_Image("image.png"));
        // box.redraw();
    }
};

#endif