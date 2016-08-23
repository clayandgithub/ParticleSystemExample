#ifndef PARTICLE_H
#define PARTICLE_H

//160帧,即8秒 (1/SPF) * 8 = 160
#define MAX_TTL 160
//SPF = 1/FPS
#define SPF 0.04f
#define MAX_AVG_V 2.0f
//G is adjust variable
#define G 8.0f
#define ORGIN_POINT_M 4.0f
#define SMALLEST_DIS 0.16f
//SMALLEST_DIS > MAX_V * SPF
#define DAMPING_SPEED 0.04f

#include <GL/glu.h>
#include <GL/glut.h>
#include <QtOpenGL>
#include <cmath>
class Particle
{
    //粒子对象
public:
    //构造函数提供两种初始化方式,若不填任何参数,则随机生成初始位置,速度,色彩等属性;另一中则是指定属性
    Particle(bool random = true,int ttl = MAX_TTL,
             float m = 0.02,float size = 10.0f,float x = 0.0f,float y = 0.0f,float z = 0.0f,
             float speed_x = 0.0f,float speed_y = 0.0f,float speed_z = 0.0f,
             float color_r = 1.0,float color_g = 1.0,float color_b = 1.0);
    ~Particle();
    //设置力以及与中心体的距离,其中gmm/r^2为万有引力,f_xyz为力的矢量(可用其计算出xyz三个方向的分力)
    void setForce(float f_x = 0.0f,float f_y = 0.0f,float f_z = 0.0f,float f_all = 0.0f);
    void setSpeed(float v_x = 0.0f,float v_y = 0.0f,float v_z = 0.0f,float v_all = 0.0f);
    void setPosition(float x = 0.0f,float y = 0.0f,float z = 0.0f);
    //生成随机属性
    void genRandomProperty();
    //生成半随机属性(旋转)
    void genRotateProperty();
    void setRandomColor(int kind = 0);
    //随机函数-1到1的float
    float myRandomFunc(int i);
    //每一帧的运动函数
    void tick(bool speed_limit = true);
    //绘制函数
    void paint();

    //variables
    float mPos[3];//范围(-1-----1,-1-----1,-1-----1)
    float mColor[4];//RGBA  范围(-1-----1,-1-----1,-1-----1)
    int mTTL;//生命周期   -1 means never disappear 0 means need to be delete, greater than 0 means the time to libe (单位:帧数)
    float mSpeed[4];//范围(-MAX_V to MAX_V)
    float mForce[4];//F,Fx,Fy,Fz   参考范围:-3.0 到 3.0
    float mLastForce[4];//F,Fx,Fy,Fz   参考范围:-3.0 到 3.0
    float mM;//参考范围:  (中心粒子的质量)2.0+  (其他粒子质量)2.0-  (0.01-----0.02)
    float mSize;//大小(参考: 4)
    bool mVisible;
    float mDampingSpeed;//阻力相关
private:
    //生成随机属性的种子,属于静态变量
    static long random_seed;
};

#endif // PARTICLE_H
