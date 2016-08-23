#include "particle.h"
#include <QDebug>
#include <time.h>

long Particle::random_seed = 0;//static variable

Particle::Particle(bool random,int ttl,float m,float size,float x,float y,float z,
                   float speed_x,float speed_y,float speed_z,float color_r,float color_g,float color_b)
{
    if(random)
    {
        genRandomProperty();
    }
    else
    {
//        float mPos[3];//范围(-1-----1,-1-----1,-1-----1)
//        float mColor[4];//RGBA
//        int mTTL;//-1 means never disappear 0 means need to be delete
//        float mSpeed[4];//范围(0-0.1)
//        float mForce[4];//F,Fx,Fy,Fz
//        float mM;//质量最大为2(中心点的质量)  其他质量为0.01-----0.02     月球质量相当于地球质量的1/81
//        float mSize;//大小(参考: 5)
        mPos[0] = x;
        mPos[1] = y;
        mPos[2] = z;

        mColor[0] = color_r;
        mColor[1] = color_g;
        mColor[2] = color_b;
        mColor[3] = 1.0;

        mTTL = ttl;//单位:frame

        mSpeed[1] = speed_x;
        mSpeed[2] = speed_y;
        mSpeed[3] = speed_z;//单位:,?/frame
        mSpeed[0] = mSpeed[1]*mSpeed[1] + mSpeed[2]*mSpeed[2] + mSpeed[3]*mSpeed[3];

        mForce[0] = mForce[1] = mForce[2] = mForce[3] = 0.0f;//F,Fx,Fy,Fz ps:they will be update during caculating
        mLastForce[0] = mLastForce[1] = mLastForce[2] = mLastForce[3] = 0.0f;//F,Fx,Fy,Fz ps:they will be update during caculating

        mM = m;//单位:kg

        mSize = size;

        mVisible = true;
        mDampingSpeed = DAMPING_SPEED;
//        mDampingSpeed = 0.01f;
    }
}

Particle::~Particle()
{
}
//生成-1到1的random float
float Particle::myRandomFunc(int int_i)
{
    long i = (long)int_i;
    i = (i << 13) ^ i;
    return ( 1.0f - ( (i * (i * i * 15731 + 789221) + 1376312589) & 0x7fffffff)/1073741824.0f);
}

void Particle::setRandomColor(int kind)
{
    //mColor[1] = float(rand()%30000 + 10000)/100000;
    if(kind==0)
    {
        //black
        mColor[0] = 0.0f;
        mColor[1] = 0.0f;
        mColor[2] = 0.0f;
        //red 0; green 0.1-0.4;blue 0.2-0.8;
//        mColor[0] = 1.0f;
//        mColor[1] = 1.0f;
//        mColor[2] = 1.0f;
    }
    else if(kind == 1)
    {
        mColor[0] = 1.0f;
        mColor[1] = 1.0f;
        mColor[2] = 1.0f;
        //white
    }
    else if(kind == 2)
    {
        mColor[0] = 1.0f;
        mColor[1] = 1.0f;
        mColor[2] = 0.0f;
        //yellow
    }
    else if(kind == 3)
    {
        mColor[0] = 0.9f;
        mColor[1] = 0.0f;
        mColor[2] = 0.2f;
        //brown
    }
    else if(kind == 4)
    {
        mColor[0] = 1.0f;
        mColor[1] = 0.2f;
        mColor[2] = 1.0f;
        //purple
    }
    else
    {
        mColor[0] = 0.0f;
        mColor[1] = 0.0f;
        mColor[2] = 1.0f;
        //blue
    }
    //明暗变化0.00-1.00
    float temp_offset = float(rand()%10000)/100000;
    for(int i = 0;i<3;++i)
    {
        mColor[i] -= temp_offset * mColor[i];
    }
    mColor[3] = 1.0f;
}

void Particle::genRandomProperty()
{
    //++random_seed;
    srand(random_seed++);
    //long rand()%1024 = rand()%1024;
    mPos[0] = myRandomFunc(rand());
    mPos[1] = myRandomFunc(rand());
    mPos[2] = myRandomFunc(rand());
    setRandomColor(rand()%6);
//    mTTL = abs((int)(300 * myRandomFunc(rand()%1024)));//单位:frame
    //100-160
    mTTL = rand()%60 + 100;//单位:frame
//    mTTL = MAX_TTL;//单位:frame
    mSpeed[1] = myRandomFunc(rand());///?????????改///
    mSpeed[2] = 0.0f;
    mSpeed[3] = 0.0f;
//    mSpeed[2] = myRandomFunc(rand()%1024)*0.1f;
//    mSpeed[3] = myRandomFunc(rand()%1024)*0.1f;//单位:,?/frame
    if((mPos[0] < 0 && mSpeed[1] < 0) || (mPos[0] > 0 && mSpeed[1] > 0))
    {
        mSpeed[1] = -mSpeed[1];
    }
    if((mPos[1] < 0 && mSpeed[2] < 0) || (mPos[1] > 0 && mSpeed[2] > 0))
    {
        mSpeed[2] = -mSpeed[2];
    }
    if((mPos[2] < 0 && mSpeed[3] < 0) || (mPos[2] > 0 && mSpeed[3] > 0))
    {
        mSpeed[3] = -mSpeed[3];
    }
    //qDebug("mv[0] = %f,mv[1] = %f,mv[2] = %f",mSpeed[1],mSpeed[2],mSpeed[3]);
    mSpeed[0] = mSpeed[1]*mSpeed[1] + mSpeed[2]*mSpeed[2] + mSpeed[3]*mSpeed[3];
    mForce[0] = mForce[1] = mForce[2] = mForce[3] = 0.0f;//F,Fx,Fy,Fz ps:they will be update during caculating
    mLastForce[0] = mLastForce[1] = mLastForce[2] = mLastForce[3] = 0.0f;//F,Fx,Fy,Fz ps:they will be update during caculating
    //0.02-1.0
    mM = float(rand()%98000 + 2000)/100000;
    //2.0f-6.0f
    mSize = float(rand()%40000 + 20000)/10000;
    mVisible = true;
    mDampingSpeed = DAMPING_SPEED;
}
void Particle::genRotateProperty()
{
    srand(random_seed++);
    int temp_rnd = rand()%4;
    if(temp_rnd == 0)
    {
        setRandomColor(1);
        //upleft(-1.0<x<-0.6 , 0.8 < y <1.0, 2.0 < z < 4.0)
        mPos[0] = -float(rand()%40000 + 100000)/100000;
        mPos[1] = float(rand()%20000 + 100000)/100000;
        mPos[2] = float(rand()%20000 + 20000)/10000;

        //x -2.0dao-1.6
        mSpeed[1] = float(rand()%40000 + 160000)/100000;
        mSpeed[2] = 0.0f;
        mSpeed[3] = 0.0f;
    }
    else if(temp_rnd == 1)
    {
        setRandomColor(2);
        //upright(0.6<x<1.0 , 0.8 < y <1.0,2.0 < z < 4.0)
        mPos[0] = float(rand()%40000 + 100000)/100000;
        mPos[1] = float(rand()%20000 + 100000)/100000;
        mPos[2] = float(rand()%20000 + 20000)/10000;

        //y -2.0dao-1.6
        mSpeed[1] = 0.0f;
        mSpeed[2] = -float(rand()%40000 + 160000)/100000;
//        mSpeed[2] = -float(rand()%20000 + 40000)/100000;
        mSpeed[3] = 0.0f;

    }
    else if(temp_rnd == 2)
    {
        setRandomColor(rand()%2+3);
        //down_left(-1.0<x<0.6 , -1.0 < y <-0.8, 2.0 < z < 4.0)
        mPos[0] = -float(rand()%40000 + 100000)/100000;
        mPos[1] = -float(rand()%20000 + 100000)/100000;
        mPos[2] = float(rand()%20000 + 20000)/10000;
        //y 0.4dao0.6
        mSpeed[1] = 0.0f;
        mSpeed[2] = float(rand()%40000 + 160000)/100000;
        mSpeed[3] = 0.0f;
    }
    else
    {
        setRandomColor(5);
        //down_right(0.6<x<1.0 , -1.0 < y <-0.8,2.0 < z < 4.0)
        mPos[0] = float(rand()%40000 + 100000)/100000;
        mPos[1] = -float(rand()%20000 + 100000)/100000;
        mPos[2] = float(rand()%20000 + 20000)/10000;
        //y 0.4dao0.6
        mSpeed[1] = -float(rand()%40000 + 160000)/100000;
        mSpeed[2] = 0.0f;
        mSpeed[3] = 0.0f;
    }
//    mTTL = abs((int)(300 * myRandomFunc(rand()%1024)));//单位:frame
    //100-160
    mTTL = rand()%60 + 100;//单位:frame
//    mSpeed[2] = myRandomFunc(rand()%1024)*0.1f;
//    mSpeed[3] = myRandomFunc(rand()%1024)*0.1f;//单位:,?/frame

//    if((mPos[0] < 0 && mSpeed[1] < 0) || (mPos[0] > 0 && mSpeed[1] > 0))
//    {
//        mSpeed[1] = -mSpeed[1];
//    }
//    if((mPos[1] < 0 && mSpeed[2] < 0) || (mPos[1] > 0 && mSpeed[2] > 0))
//    {
//        mSpeed[2] = -mSpeed[2];
//    }
//    if((mPos[2] < 0 && mSpeed[3] < 0) || (mPos[2] > 0 && mSpeed[3] > 0))
//    {
//        mSpeed[3] = -mSpeed[3];
//    }
    //qDebug("mv[0] = %f,mv[1] = %f,mv[2] = %f",mSpeed[1],mSpeed[2],mSpeed[3]);

    mSpeed[0] = mSpeed[1]*mSpeed[1] + mSpeed[2]*mSpeed[2] + mSpeed[3]*mSpeed[3];
    mForce[0] = mForce[1] = mForce[2] = mForce[3] = 0.0f;//F,Fx,Fy,Fz ps:they will be update during caculating
    //0.02-1.0
    mLastForce[0] = mLastForce[1] = mLastForce[2] = mLastForce[3] = 0.0f;//F,Fx,Fy,Fz ps:they will be update during caculating
    mM = float(rand()%98000 + 2000)/100000;
//    mM = 0.02f * (float)fabs(myRandomFunc(rand()));
    //2.0f-6.0f
    mSize = float(rand()%40000 + 20000)/10000;
    mVisible = true;
    mDampingSpeed = DAMPING_SPEED;
}

void Particle::setForce(float f_x,float f_y,float f_z,float f_all)
{
    mLastForce[0] = mForce[0];
    mLastForce[1] = mForce[1];
    mLastForce[2] = mForce[2];
    mLastForce[3] = mForce[3];//F,Fx,Fy,Fz ps:they will be update during caculating
    mForce[1] =  f_x;
    mForce[2] =  f_y;
    mForce[3] =  f_z;
    if(f_all == 0.0f)
    {
        mForce[0] =  sqrt(f_x * f_x + f_y * f_y + f_z * f_z);
    }
    else
    {
        mForce[0] =  f_all;
    }
}
void Particle::setSpeed(float v_x,float v_y,float v_z,float v_all)
{
    mSpeed[1] =  v_x;
    mSpeed[2] =  v_y;
    mSpeed[3] =  v_z;
    if(v_all == 0.0f)
    {
        mSpeed[0] =  sqrt(v_x * v_x + v_y * v_y + v_z * v_z);
    }
    else
    {
        mSpeed[0] =  v_all;
    }
}

void Particle::setPosition(float x,float y,float z)
{
    mPos[0] = x;
    mPos[1] = y;
    mPos[2] = z;
}

void Particle::tick(bool speed_limit)
{
    if(mTTL == 0 || (mTTL > 0 && --mTTL==0))
    {
        return;
    }

    for(int i = 0;i<4;++i)
    {
        mSpeed[i] += (mLastForce[i]/mM + mForce[i]/mM) / 2 * SPF;//此式计算出当前的速度,即Vt
    }
    //每一帧粒子根据自身参数自己运动
//    mSpeed[0] += SPF * mForce[0]/mM;//??????阻尼????
//    for(int i = 1;i<4;++i)
//    {
//        if(mSpeed[i]>0)
//        {
//            //mSpeed[i] -= mDampingSpeed * mSpeed[i];
////            if(mSpeed[i]<0.0f)
////            {
////                mSpeed[i] = 0.0f;
////            }
//            if(mSpeed[i] > MAX_AVG_V)
//            {
//                mSpeed[i] = MAX_AVG_V;
//                //qDebug("v[%d] = %f",i,mSpeed[i]);
//            }
//        }
//        else if(mSpeed[i]<0)
//        {
//            //mSpeed[i] += mDampingSpeed  * mSpeed[i];
////            if(mSpeed[i]>0.0f)
////            {
////                mSpeed[i] = 0.0f;
////            }
//            if(mSpeed[i] < -MAX_AVG_V)
//            {
//                mSpeed[i] = -MAX_AVG_V;
//                //qDebug("v[%d] = %f",i,mSpeed[i]);
//            }
//        }
//    }
    float adjust_avg_speed[4] ={0};
    for(int i = 1;i<4;++i)
    {
        adjust_avg_speed[i] = mSpeed[i] + (mForce[i] / mM)/2 * SPF;
        if(speed_limit)
        {
            if(adjust_avg_speed[i]>MAX_AVG_V)
            {
                adjust_avg_speed[i] = MAX_AVG_V;
            }
            else if(adjust_avg_speed[i]<-MAX_AVG_V)
            {
                adjust_avg_speed[i] = -MAX_AVG_V;
            }
        }
    }
    mPos[0] += adjust_avg_speed[1]*SPF;
    mPos[1] += adjust_avg_speed[2]*SPF;
    mPos[2] += adjust_avg_speed[3]*SPF;
//    mPos[0] += mSpeed[1] * SPF + (mLastForce[1] / mM)/2 * SPF*SPF;
//    mPos[1] += mSpeed[2] * SPF + (mLastForce[2] / mM)/2 * SPF*SPF;
//    mPos[2] += mSpeed[3] * SPF + (mLastForce[3] / mM)/2 * SPF*SPF;
//    mPos[0] += SPF * mSpeed[1];
//    mPos[1] += SPF * mSpeed[2];
//    mPos[2] += SPF * mSpeed[3];
//    mPos[0] += SPF * (mSpeed[1] + lastSpeed[1])/2;
//    mPos[1] += SPF * (mSpeed[2] + lastSpeed[2])/2;
//    mPos[2] += SPF * (mSpeed[3] + lastSpeed[3])/2;

    //qDebug("x = %f,y = %f,z = %f",mPos[0],mPos[1],mPos[2]);

    //2.0f-6.0f
    //-4dao4 ----0dao 4
    if(mPos[2]>-4.0)
    {
        mSize = 0.5f * (mPos[2] + 4.0 )/ 2;
    }
    else
    {
        mSize = 0.02f;
        mTTL = 0;//??????????
    }
    //qDebug("///////////");
}

void Particle::paint()
{
    float temp_size = mSize/32;
    if(mVisible)
    {
        glColor4f(mColor[0],mColor[1],mColor[2], 1.0f);
        //glutWireCube(0.5);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);
        //前面
        glVertex3f(mPos[0]-temp_size, mPos[1]-temp_size, mPos[2]+temp_size);//左下
        glTexCoord2f(1.0, 0.0);
        glVertex3f(mPos[0]+temp_size, mPos[1]-temp_size, mPos[2]+temp_size);//右下
        glTexCoord2f(1.0, 1.0);
        glVertex3f(mPos[0]+temp_size, mPos[1]+temp_size, mPos[2]+temp_size);//右上
        glTexCoord2f(0.0, 1.0);
        glVertex3f(mPos[0]-temp_size, mPos[1]+temp_size, mPos[2]+temp_size);//左上
        //后面
        glVertex3f(mPos[0]-temp_size, mPos[1]-temp_size, mPos[2]-temp_size);//左下
        glTexCoord2f(1.0, 0.0);
        glVertex3f(mPos[0]+temp_size, mPos[1]-temp_size, mPos[2]-temp_size);//右下
        glTexCoord2f(1.0, 1.0);
        glVertex3f(mPos[0]+temp_size, mPos[1]+temp_size, mPos[2]-temp_size);//右上
        glTexCoord2f(0.0, 1.0);
        glVertex3f(mPos[0]-temp_size, mPos[1]+temp_size, mPos[2]-temp_size);//左上
        //上面
        glVertex3f(mPos[0]-temp_size, mPos[1]+temp_size, mPos[2]+temp_size);//左下
        glTexCoord2f(1.0, 0.0);
        glVertex3f(mPos[0]+temp_size, mPos[1]+temp_size, mPos[2]+temp_size);//右下
        glTexCoord2f(1.0, 1.0);
        glVertex3f(mPos[0]+temp_size, mPos[1]+temp_size, mPos[2]-temp_size);//右上
        glTexCoord2f(0.0, 1.0);
        glVertex3f(mPos[0]-temp_size, mPos[1]+temp_size, mPos[2]-temp_size);//左上

        //下面
        glVertex3f(mPos[0]-temp_size, mPos[1]-temp_size, mPos[2]+temp_size);//左下
        glTexCoord2f(1.0, 0.0);
        glVertex3f(mPos[0]+temp_size, mPos[1]-temp_size, mPos[2]+temp_size);//右下
        glTexCoord2f(1.0, 1.0);
        glVertex3f(mPos[0]+temp_size, mPos[1]-temp_size, mPos[2]-temp_size);//右上
        glTexCoord2f(0.0, 1.0);
        glVertex3f(mPos[0]-temp_size, mPos[1]-temp_size, mPos[2]-temp_size);//左上

        //左面
        glVertex3f(mPos[0]-temp_size, mPos[1]-temp_size, mPos[2]-temp_size);//左下
        glTexCoord2f(1.0, 0.0);
        glVertex3f(mPos[0]-temp_size, mPos[1]-temp_size, mPos[2]+temp_size);//右下
        glTexCoord2f(1.0, 1.0);
        glVertex3f(mPos[0]-temp_size, mPos[1]+temp_size, mPos[2]+temp_size);//右上
        glTexCoord2f(0.0, 1.0);
        glVertex3f(mPos[0]-temp_size, mPos[1]+temp_size, mPos[2]-temp_size);//左上
        //右面
        glVertex3f(mPos[0]+temp_size, mPos[1]-temp_size, mPos[2]-temp_size);//左下
        glTexCoord2f(1.0, 0.0);
        glVertex3f(mPos[0]+temp_size, mPos[1]-temp_size, mPos[2]+temp_size);//右下
        glTexCoord2f(1.0, 1.0);
        glVertex3f(mPos[0]+temp_size, mPos[1]+temp_size, mPos[2]+temp_size);//右上
        glTexCoord2f(0.0, 1.0);
        glVertex3f(mPos[0]+temp_size, mPos[1]+temp_size, mPos[2]-temp_size);//左上
        glEnd();
//        glPushMatrix();
        //glColor4f(mColor[0],mColor[1],mColor[2],mColor[3]);
        //glPointSize(mSize);
        //glBegin(GL_POINTS);
        //glVertex3f(mPos[0],mPos[1],mPos[2]);
        //glEnd();
//        glPopMatrix();
    }
}


