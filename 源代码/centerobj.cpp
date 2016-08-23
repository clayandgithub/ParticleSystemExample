#include "centerobj.h"
#include <QDebug>

CenterObj::CenterObj(float *pos,float *color,float m, float size,bool visible)
{
    mPos[0] = pos[0];mPos[1] = pos[1];mPos[2] = pos[2];
    mColor[0] = color[0];mColor[1] = color[1];mColor[2] = color[2];mColor[3] = color[3];
    mM = m;
    mSize = size;
    mVisible = visible;
    mExpendSpeed = 0.0f;
}

CenterObj::~CenterObj()
{
}

void CenterObj::tick()
{
    if(mVisible)
    {
        mSize += mExpendSpeed;//0.002f;
        if(mSize>=1.0f || mSize<=0.0f)
        {
            mSize = 0.1f;
            mVisible = false;
        }
    }
}
void CenterObj::paint()
{
    float temp_size = mSize;
    if(mVisible)
    {
        glColor4f(mColor[0],mColor[1],mColor[2], 1.0f);
        //glutWireCube(0.5);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);
        //前面
        glVertex3f(mPos[0]-temp_size, mPos[1]-temp_size, 0.0f);//左下
        glTexCoord2f(1.0, 0.0);
        glVertex3f(mPos[0]+temp_size, mPos[1]-temp_size, 0.0f);//右下
        glTexCoord2f(1.0, 1.0);
        glVertex3f(mPos[0]+temp_size, mPos[1]+temp_size, 0.0f);//右上
        glTexCoord2f(0.0, 1.0);
        glVertex3f(mPos[0]-temp_size, mPos[1]+temp_size, 0.0f);//左上
        glEnd();
    }
//    if(mVisible)
//    {
//        GLUquadricObj *quadPlanet;///???????????????/需要释放?
//        quadPlanet = gluNewQuadric();
////        glColor4f(mColor[0],mColor[1],mColor[2],mColor[3]);
//        glColor4f(1.0,0,0,1.0);
//        glTranslatef(mPos[0],mPos[1],mPos[2]);

//        gluSphere(quadPlanet,mSize,16.0, 16.0); //画圆球
//    }
}
void CenterObj::setPosition(float x,float y,float z)
{
    mPos[0] = x;
    mPos[1] = y;
    mPos[2] = z;
}
