#include "cube_window.hpp"
int main() {
    // pre_produce_image(1);
    cube_window window(512,512, 8, 6, "Cube");
    window.show();
    return Fl::run();
    // return 0;
}