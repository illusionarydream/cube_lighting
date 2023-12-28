#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP
#include <eigen3/Eigen/Eigen>
#include <vector>
using namespace Eigen;
class triangle{
public:
    // point coordinates need to be 4d, because we need to use the matrix to do the transformation
    // normal and color need to be 3d, because we don't need to do the transformation
    // view coordinates are the coordinates in the viewing space.
    Vector4f v[3];
    Vector4f screen_coord[3];
    Vector4f view_coord[3];
    Vector3f normal[3];
    Vector3f view_normal[3];
    Vector3f color[3];
    triangle() {}
    void setNormal(int i, Vector3f v) {
        normal[i] = v;
    }
    void setColor(int i, Vector3f v) {
        color[i] = v;
    }
    void setViewCoord(int i, Vector4f v) {
        view_coord[i] = v;
    }
    void setScreenCoord(int i, Vector4f v) {
        screen_coord[i] = v;
    }
    void setv(int i, Vector4f v) {
        this->v[i] = v;
    }
    void setNormal(Vector4f v0, Vector4f v1, Vector4f v2) {
        v[0] = v0;
        v[1] = v1;
        v[2] = v2;
    }
    void setColor(Vector3f c0, Vector3f c1, Vector3f c2) {
        color[0] = c0;
        color[1] = c1;
        color[2] = c2;
    }
    void setNormal(Vector3f n0, Vector3f n1, Vector3f n2) {
        normal[0] = n0;
        normal[1] = n1;
        normal[2] = n2;
    }
    void setViewCoord(Vector4f v0, Vector4f v1, Vector4f v2) {
        view_coord[0] = v0;
        view_coord[1] = v1;
        view_coord[2] = v2;
    }
    void setScreenCoord(Vector4f v0, Vector4f v1, Vector4f v2) {
        screen_coord[0] = v0;
        screen_coord[1] = v1;
        screen_coord[2] = v2;
    }
};
class triangles{
public:
    std::vector<triangle> tri;
    triangles() {}
    void add_triangle(triangle t){
        tri.push_back(t);
    }
};
#endif