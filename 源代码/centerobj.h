#ifndef CENTEROBJ_H
#define CENTEROBJ_H

#include <GL/glu.h>
#include <GL/glut.h>
#include <QtOpenGL>

//若为圆球,则是半径
#define MAX_CENTER_SIZE 0.2f

class CenterObj
{
public:
    CenterObj(float *pos,float *color,float m = 4.0,float size = 0.02f,bool visible = true);
    ~CenterObj();
    void tick();
    void paint();
    void setPosition(float x = 0.0f,float y = 0.0f,float z = 0.0f);

    //变量
    float mPos[3];//范围(-1-----1,-1-----1,-1-----1)
    float mColor[4];//RGBA  范围(-1-----1,-1-----1,-1-----1)
    float mM;//参考范围:  (中心粒子的质量)2.0+  (其他粒子质量)2.0-  (0.01-----0.02)
    float mSize;//大小(参考: 4)
    bool mVisible;
    float mExpendSpeed;//0 - 0.04f
};

#endif // CENTEROBJ_H
