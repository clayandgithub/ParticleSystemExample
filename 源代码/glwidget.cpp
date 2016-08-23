#include "glwidget.h"
#include "ui_glwidget.h"

#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>
#include <QDebug>
#include <time.h>

/*c++中可以在类的外部定义变量*/
GLfloat light_ambient[4]={0.5, 0.5, 0.5, 1.0};
GLfloat light_diffuse[4]={1.0, 1.0, 1.0, 1.0};
GLfloat light_position[4]={0.0, 0.0, 2.0, 0.0};

GLWidget::GLWidget(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::DoubleBuffer|QGL::DepthBuffer),parent),
    ui(new Ui::GLWidget)
{
    ui->setupUi(this);
    mPartices.clear();
    mUpdateTimer = new QTimer(this);
    connect(mUpdateTimer, SIGNAL(timeout()), this, SLOT(myUpdate()));
    mState = NO_ANIMATE;
    mRemain_small_particles = 0;
    mCameraShakingFlag = 0;
    mCurrentFrame = 0;

    initializeGL();
    resizeGL(500,500);
    paintGL();

    float temp_color[3] = {1.0f,0.0f,0.0f};
    float temp_pos[3] = {0.0f,0.0f,0.0f};
    mCenterOBJ = new CenterObj(temp_pos,temp_color,4.0f,0.02f,false);
    initPartices();//初始化粒子
}

void GLWidget::initializeGL()
{
    loadTextures();
    glEnable(GL_TEXTURE_2D);//允许采用2D纹理技术

    glShadeModel(GL_SMOOTH);//设置阴影平滑模式
    glClearColor(0.0, 0.0, 0.0, 0.5);//改变窗口的背景颜色
    glClearDepth(1.0);//设置深度缓存

    glEnable(GL_DEPTH_TEST);//允许深度测试
    glDepthFunc(GL_LEQUAL);//设置深度测试类型

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);//源像素因子采用alpha通道值，目标像素因子采用1.0
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);//色彩融合和深度缓存不能同时开启
    //dl = Gen3DBunnyList();//生成显示列表
}

/*装载纹理*/
void GLWidget::loadTextures()
{
    QImage tex, buf,tex1,buf1;
    if(!buf.load("D:/Particle.bmp"))
    {
        qWarning("Cannot open the image...");
        QImage dummy(128, 128, QImage::Format_RGB32);//当没找到所需打开的图片时，创建一副128*128大小，深度为32位的位图
        dummy.fill(Qt::green);
        buf = dummy;
    }

    if(!buf1.load("D:/back.jpg"))
    {
        qWarning("Cannot open the image...");
        QImage dummy(128, 128, QImage::Format_RGB32);//当没找到所需打开的图片时，创建一副128*128大小，深度为32位的位图
        dummy.fill(Qt::black);
        buf1 = dummy;
    }

    tex = convertToGLFormat(buf);//将Qt图片的格式buf转换成opengl的图片格式tex
    tex1 = convertToGLFormat(buf1);//将Qt图片的格式buf转换成opengl的图片格式tex
    glGenTextures(2, &texture[0]);//开辟2个纹理内存，索引指向texture[0]

    /*建立第一个纹理*/
    glBindTexture(GL_TEXTURE_2D, texture[0]);//将创建的纹理内存指向的内容绑定到纹理对象GL_TEXTURE_2D上，经过这句代码后，以后对
                                            //GL_TEXTURE_2D的操作的任何操作都同时对应与它所绑定的纹理对象
    glTexImage2D(GL_TEXTURE_2D, 0, 3, tex.width(), tex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());//开始真正创建纹理数据
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//当所显示的纹理比加载进来的纹理小时，采用GL_NEAREST的方法来处理
                                                                      //GL_NEAREST方式速度非常快，因为它不是真正的滤波，所以占用内存非常
                                                                      // 小，速度就快了
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//当所显示的纹理比加载进来的纹理大时，采用GL_NEAREST的方法来处理

    /*建立第2个纹理*/
    glBindTexture(GL_TEXTURE_2D, texture[1]);//将创建的纹理内存指向的内容绑定到纹理对象GL_TEXTURE_2D上，经过这句代码后，以后对
                                            //GL_TEXTURE_2D的操作的任何操作都同时对应与它所绑定的纹理对象
    glTexImage2D(GL_TEXTURE_2D, 0, 3, tex1.width(), tex1.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex1.bits());//开始真正创建纹理数据
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//当所显示的纹理比加载进来的纹理小时，采用GL_NEAREST的方法来处理
                                                                      //GL_NEAREST方式速度非常快，因为它不是真正的滤波，所以占用内存非常
                                                                      // 小，速度就快了
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//当所显示的纹理比加载进来的纹理大时，采用GL_NEAREST的方法来处理

}

void GLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
    glLoadIdentity();									// Reset The Projection Matrix
    GLdouble whRatio = width*1.0/height;
    //若当前窗口宽 > 高,则截取宽度多一点
    if(width>=height)
    {
        glOrtho(-1.0*whRatio,1.0*whRatio, -1.0,1.0,-100,100);
    }
    else
    {
        //若当前窗口宽 < 高,则截取高度多一点
        glOrtho(-1.0,1.0,-1.0/whRatio,1.0/whRatio,-100,100);
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glMatrixMode(GL_MODELVIEW);
//    if (false)
//        gluPerspective(45,1,1,100);
}


void GLWidget::paintGL()
{
    float eye[] = {-1.0f, 1.0f, 4.0f};
    float center[] = {0.0f, 0.0f, 0.0f};
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();// Reset The Current Modelview Matrix

//    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_BLEND);//色彩融合和深度缓存不能同时开启
    //画背景
    glBindTexture(GL_TEXTURE_2D, texture[1]);//绑定纹理目标
    glPushMatrix();
    glColor4f(0.0f,0.0f,0.0f, 1.0f);
    //glutWireCube(0.5);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-4.0f, -4.0f, 0.0f);//左下
    glTexCoord2f(1.0, 0.0);
    glVertex3f(4.0f, -4.0f, 0.0f);//右下
    glTexCoord2f(1.0, 1.0);
    glVertex3f(4.0f, 4.0f, 0.0f);//右上
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-4.0f, 4.0f, 0.0f);//左上
    glEnd();
    glPopMatrix();

    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);//源像素因子采用alpha通道值，目标像素因子采用1.0
    //glEnable(GL_BLEND);//色彩融合和深度缓存不能同时开启

    glBindTexture(GL_TEXTURE_2D, texture[0]);//绑定纹理目标
    switch(mState)
    {
    case NO_ANIMATE:
        break;
    case SHRINKING:
        gluLookAt(0.0f, 0.0f, eye[2],center[0], center[1], center[2],0.0, 1.0, 0.0); //0,1,0表示Y轴正方向为头顶方向
        if(mCenterOBJ != NULL && mCenterOBJ->mVisible)
        {
            glPushMatrix();
            //qDebug("mCenterOBJ->mSize = %f!",mCenterOBJ->mSize);
            mCenterOBJ->paint();
            glPopMatrix();
        }
        glPushMatrix();
        for(unsigned int i = 0;i < mPartices.size();++i)
        {
            if(mPartices.at(i) != NULL && mPartices.at(i)->mVisible)
            {
                mPartices.at(i)->paint();//还可以再改进,提高点的绘制速度
            }
        }
        glPopMatrix();
        break;
    case EXPLODING:
        if(mCenterOBJ != NULL && mCenterOBJ->mVisible)
        {
            gluLookAt(eye[0], eye[1], eye[2],center[0], center[1], center[2],0.0, 1.0, 0.0); //0,1,0表示Y轴正方向为头顶方向
            glPushMatrix();
            //还可以看到中心体,因此粒子不能爆炸
            mCenterOBJ->paint();
            glPopMatrix();
        }
        else
        {
            if(mCameraShakingFlag != 0)
            {
                float u = 1.0f * mCurrentFrame / MAX_FRAME_NUM;
                if(mCameraShakingFlag == 1)
                {
                    eye[0] += (1.0f - (3-2*u)*u*u) * 0.05f;
                    eye[1] += (1.0f - (3-2*u)*u*u) * 0.05f;
                    center[0] += (1.0f - (3-2*u)*u*u) * 0.05f;
                    center[1] += (1.0f - (3-2*u)*u*u) * 0.05f;
                    mCameraShakingFlag =2;
                }
                else
                {
                    eye[0] -= (1.0f - (3-2*u)*u*u) * 0.05f;
                    eye[1] -= (1.0f - (3-2*u)*u*u) * 0.05f;
                    center[0] -= (1.0f - (3-2*u)*u*u) * 0.05f;
                    center[1] -= (1.0f - (3-2*u)*u*u) * 0.05f;
                    mCameraShakingFlag =1;
                }
            }
            gluLookAt(eye[0], eye[1], eye[2],center[0], center[1], center[2],0.0, 1.0, 0.0); //0,1,0表示Y轴正方向为头顶方向
//            gluLookAt(4.0f, 0.0f, 0.0f,center[0], center[1], center[2],0.0, 1.0, 0.0); //0,1,0表示Y轴正方向为头顶方向
//            gluLookAt(eye[0], eye[1], eye[2],center[0], center[1], center[2],0.3, 1.0, 0.0); //0,1,0表示Y轴正方向为头顶方向
            glPushMatrix();
            for(unsigned int i = 0;i < mPartices.size();++i)
            {
                if(mPartices.at(i) != NULL && mPartices.at(i)->mVisible)
                {
                    mPartices.at(i)->paint();//????????还可以再改进,提高点的绘制速度
                }
            }
            glPopMatrix();
        }
    default:
        break;
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    switch(mState)
    {
    case NO_ANIMATE:
        //qDebug("x = %d,y = %d,width = %d,height = %d",event->x(),event->y(),this->width(),this->height());
        //x 0 zhi width/2---> -1 zhi 0   width/2 zhi width ---->0 zhi 1
        mCenterOBJ->mPos[0] = 1.0f * (event->x() - this->width()/2) / (this->width()/2);
        mCenterOBJ->mPos[1] = 1.0f * (this->height()/2 - event->y()) / (this->height()/2);
//        mCenterOBJ->mPos[0]
//        mCenterOBJ->mPos[1]
        setState(SHRINKING);
        break;
    case SHRINKING:
        //setState(EXPLODING);
        break;
    case EXPLODING:
        setState(NO_ANIMATE);
        break;
    default:
        break;
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
//     QPoint lastPos = event->pos();
}

void GLWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    //QMessageBox::about(this,"MyTitle",tr("双击事件暂时还没有指定哦!"));
}

void GLWidget::setState(int state)
{
    mUpdateTimer->stop();
    switch(state)
    {
    case NO_ANIMATE:
        mCurrentFrame = 0;
        mCameraShakingFlag = 0;
        mState = NO_ANIMATE;
        break;
    case SHRINKING:
        mCurrentFrame = 0;
        mCameraShakingFlag = 0;
        initShrinking();
        mState = SHRINKING;
        mUpdateTimer->start(int(SPF*1000));//update start
        break;
    case EXPLODING:
        mCurrentFrame = 0;
        mCameraShakingFlag = 1;
        initExploding();
        mState = EXPLODING;
        mUpdateTimer->start(int(SPF*1000));//update start
        break;
    default:
        mCameraShakingFlag = 0;
        mUpdateTimer->stop();
        mState = NO_ANIMATE;
        break;
    }
}

void GLWidget::initShrinking()
{
    mRemain_small_particles = MAX_SMALL_PARTICLES;
    mCenterOBJ->mVisible = true;
    mCenterOBJ->mSize = 0.01f;
    mCenterOBJ->mColor[0] = 0.0f;
    mCenterOBJ->mColor[1] = 0.0f;
    mCenterOBJ->mColor[2] = 0.0f;
    mCenterOBJ->mExpendSpeed = 0.0f;
    for(unsigned int i = 0;i < mPartices.size(); ++i)
    {
        if(mPartices.at(i)!=NULL)
        {
//            mPartices.at(i)->genRandomProperty();
            mPartices.at(i)->genRotateProperty();
        }
    }
}

void GLWidget::initExploding()
{
    mCenterOBJ->mVisible = true;
    mCenterOBJ->mColor[0] = 1.0f;
    mCenterOBJ->mColor[1] = 0.0f;
    mCenterOBJ->mColor[2] = 1.0f;
    mCenterOBJ->mColor[3] = 1.0f;
    mRemain_small_particles = MAX_SMALL_PARTICLES;
    srand(6);
    float temp_v_axyz[4] = {0.0f};
    float temp_f_axyz[4] = {0.0f};
    float max_circle_v = 6.0f;
    float circle_v = 0.0f;
    unsigned int circle_num = 100;
    unsigned int particles_per_circle = mPartices.size() / circle_num;
    unsigned int start_i = 0;
    unsigned int end_i = 0;
    int color_kind = 6;
    int temp_ball_num = 30;
    float temp_u  = 0.0f;


    //最里面的10层处理成球状层(white)
    for(unsigned int c = circle_num-temp_ball_num;c < circle_num-1; ++c)
    {
        temp_u = (float)c / circle_num;
        circle_v = max_circle_v * (1.0f - (3-2*temp_u)*temp_u*temp_u);
        //circle_v = max_circle_v / (c+1);
        start_i = c * particles_per_circle;
        end_i = (c+1) * particles_per_circle;
        for(unsigned int i = start_i;i < end_i;++i)
        {
            if(mPartices.at(i)!=NULL)
            {
                //x方向上的速度:取值范围:circle_v * (0-1)
                temp_v_axyz[1] = circle_v * (i-start_i) / (particles_per_circle-1);
                if(rand()%2==0)
                {
                    temp_v_axyz[1] = -temp_v_axyz[1];
                }
                //y方向的速度:与x方向有关 Vz平方 = V平方 - Vx平方
                temp_v_axyz[2] = sqrt(circle_v * circle_v - temp_v_axyz[1]*temp_v_axyz[1]) * (1.0f * (rand()%1025))/1024;
                if(rand()%2==0)
                {
                    temp_v_axyz[2] = -temp_v_axyz[2];
                }
                //z方向上的速度:与x方向有关 Vz平方 = V平方 - Vx平方
                temp_v_axyz[3] =  sqrt(circle_v * circle_v - temp_v_axyz[1]*temp_v_axyz[1] - temp_v_axyz[2] * temp_v_axyz[2]);
                if(rand()%2==0)
                {
                    temp_v_axyz[3] = -temp_v_axyz[3];
                }
                //qDebug("x = %f, = %f,z = %f",mPartices.at(i)->mPos[0],mPartices.at(i)->mPos[1],mPartices.at(i)->mPos[2]);
                mPartices.at(i)->mTTL = rand()%60 + 100;//单位:frame //100-160
                mPartices.at(i)->mVisible = true;
                mPartices.at(i)->mDampingSpeed = 0.0f;
                //mPartices.at(i)->mDampingSpeed = DAMPING_SPEED;
                mPartices.at(i)->setPosition(mCenterOBJ->mPos[0],mCenterOBJ->mPos[1],mCenterOBJ->mPos[2]);
                //调用两次setForce()清除mLastForce
                mPartices.at(i)->setForce(temp_f_axyz[1],temp_f_axyz[2],temp_f_axyz[3],temp_f_axyz[0]);
                mPartices.at(i)->setForce(temp_f_axyz[1],temp_f_axyz[2],temp_f_axyz[3],temp_f_axyz[0]);

                mPartices.at(i)->setSpeed(temp_v_axyz[1],temp_v_axyz[2],temp_v_axyz[3],temp_v_axyz[0]);
                mPartices.at(i)->setRandomColor(1);
            }
        }
    }
    //(速度随圈数递增而递减)//第一圈粒子(速度最快,即最外层粒子)

    for(unsigned int c = 0;c < circle_num-temp_ball_num; ++c)
    {
        if(c % (circle_num / 5) == 0)
        {
            --color_kind;
        }
//        circle_v = max_circle_v / (c+1);
        temp_u = (float)c / circle_num;
        circle_v = max_circle_v * (1.0f - (3-2*temp_u)*temp_u*temp_u);
        start_i = c * particles_per_circle;
        end_i = (c+1) * particles_per_circle;
        for(unsigned int i = start_i;i < end_i;++i)
        {
            if(mPartices.at(i)!=NULL)
            {
                //x方向上的速度:取值范围:circle_v * (0-1)
                temp_v_axyz[1] = circle_v * (i-start_i) / (particles_per_circle-1);
                if(rand()%2==0)
                {
                    temp_v_axyz[1] = -temp_v_axyz[1];
                }
                //z方向上的速度:与x方向有关 Vz平方 = V平方 - Vx平方
                temp_v_axyz[3] =  sqrt(circle_v * circle_v - temp_v_axyz[1]*temp_v_axyz[1]);
                if(rand()%2==0)
                {
                    temp_v_axyz[3] = -temp_v_axyz[3];
                }

                //x和y方向的速度平方和根据z方向的速度值可以得到固定的值
                temp_v_axyz[1] = temp_v_axyz[1]/sqrt(2);
                temp_v_axyz[2] = temp_v_axyz[1];
                if((temp_v_axyz[1] < 0 && temp_v_axyz[2]>0) ||(temp_v_axyz[1] > 0 && temp_v_axyz[2]<0 ))
                {
                    temp_v_axyz[2] = -temp_v_axyz[2];
                }
//                if(rand()%2==0)
//                {
//                    temp_v_axyz[2] = -temp_v_axyz[2];
//                }

                //qDebug("x = %f, = %f,z = %f",mPartices.at(i)->mPos[0],mPartices.at(i)->mPos[1],mPartices.at(i)->mPos[2]);
                mPartices.at(i)->mTTL = rand()%60 + 100;//单位:frame //100-160
                mPartices.at(i)->mVisible = true;
                mPartices.at(i)->mDampingSpeed = 0.0f;
                //mPartices.at(i)->mDampingSpeed = DAMPING_SPEED;
                mPartices.at(i)->setPosition(mCenterOBJ->mPos[0],mCenterOBJ->mPos[1],mCenterOBJ->mPos[2]);
                //调用两次setForce()清除mLastForce
                mPartices.at(i)->setForce(temp_f_axyz[1],temp_f_axyz[2],temp_f_axyz[3],temp_f_axyz[0]);
                mPartices.at(i)->setForce(temp_f_axyz[1],temp_f_axyz[2],temp_f_axyz[3],temp_f_axyz[0]);
                mPartices.at(i)->setSpeed(temp_v_axyz[1],temp_v_axyz[2],temp_v_axyz[3],temp_v_axyz[0]);
                mPartices.at(i)->setRandomColor(color_kind);
            }
        }
    }
    //qDebug("start_i =%d, end_i = %d",start_i,end_i);
}

void GLWidget::myUpdate()
{
    ++mCurrentFrame;
    switch(mState)
    {
    case NO_ANIMATE:
        break;
    case SHRINKING:
        updatePartices();
        updateGL();
        break;
    case EXPLODING:
        updatePartices();
        updateGL();
        break;
    default:
        break;
    }
}

void GLWidget::initPartices()
{
    //    bool random,int ttl,float m,float size,float x,float y,float z,
    //                       float speed_x,float speed_y,float speed_z,float color_r,float color_g,float color_b
    Particle *temp_partice = NULL;
//    = new Particle(false,-1,ORGIN_POINT_M,10.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f);
    //mPartices.push_back(temp_partice);
    mCenterOBJ->mVisible = true;//中心点
    //粒子
    for(int i = 0;i < MAX_SMALL_PARTICLES;++i)
    {
        temp_partice = new Particle(false);
        mPartices.push_back(temp_partice);
    }
    mRemain_small_particles = MAX_SMALL_PARTICLES;
}
void GLWidget::updatePartices()
{
    float temp_r = 0.0f;
    float temp_f_xyz[3] = {0.0f};
    float temp_f_axyz[4] = {0.0f};
    float temp_add_size = 0.0f;
    float temp_add_color = 0.0f;

    switch(mState)
    {
    case NO_ANIMATE:
        break;
    case SHRINKING:
        for(unsigned int i = 0;i < mPartices.size();++i)
        {

            if(mPartices.at(i)!=NULL && mPartices.at(i)->mVisible)
            {
                mPartices.at(i)->tick();//每个粒子根据所设定的参数自己运动(改变位置)
                //检查生命周期(考虑如何写在粒子类里面)
                if(mPartices.at(i)->mTTL == 0)
                {
//                    qDebug("mRemain_small_particles = %d!",mRemain_small_particles);
                    mPartices.at(i)->mVisible = false;
                    if(--mRemain_small_particles == 0)
                    {
                        setState(EXPLODING);
                        break;
                    }
                    //delete mPartices.at(i);
                    //mPartices.at(i) = NULL;
                }
                else
                {
                    //计算合外力
                    if(mCenterOBJ!=NULL)
                    {
                        temp_f_xyz[0] = (mCenterOBJ->mPos[0] - mPartices.at(i)->mPos[0]);
                        temp_f_xyz[1] = (mCenterOBJ->mPos[1] - mPartices.at(i)->mPos[1]);
                        temp_f_xyz[2] = (mCenterOBJ->mPos[2] - mPartices.at(i)->mPos[2]);
                        temp_r = (float)sqrt(
                                temp_f_xyz[0] * temp_f_xyz[0] + temp_f_xyz[1] * temp_f_xyz[1] + temp_f_xyz[2] * temp_f_xyz[2]);
                        if(temp_r <= SMALLEST_DIS || temp_r <= mCenterOBJ->mSize)
                        {
                           // qDebug("ssssss mCenterOBJ->mSize = %f!",mCenterOBJ->mSize);
                           temp_add_size += MAX_CENTER_SIZE / MAX_SMALL_PARTICLES;//0.00001f;
                           temp_add_color += 1.0f / MAX_SMALL_PARTICLES;
                            // || qAbs(mPartices.at(i)->mPos[0]) > 1.0f || qAbs(mPartices.at(i)->mPos[1]) > 1.0f
                            //|| qAbs(mPartices.at(i)->mPos[2]) > 4.0f
                            //qDebug("dis_appear_i = %d mRemain_small_particles = %d",i,mRemain_small_particles);
                            //若超出边界,则认为此点失去控制,此点消失
                            //若与中心点小于最小距离误差,则认为已经重合,此点消失
                            mPartices.at(i)->mTTL = 0;
                            //qDebug("x = %f, = %f,z = %f",mPartices.at(i)->mPos[0],mPartices.at(i)->mPos[1],mPartices.at(i)->mPos[2]);
                        }
                        else
                        {
                            temp_f_axyz[0] =  (G * mPartices.at(i)->mM * mCenterOBJ->mM ) / (temp_r * temp_r);
                            temp_f_axyz[1] =  (temp_f_axyz[0] / temp_r ) * temp_f_xyz[0];
                            temp_f_axyz[2] =  (temp_f_axyz[0] / temp_r ) * temp_f_xyz[1];
                            temp_f_axyz[3] =  (temp_f_axyz[0] / temp_r ) * temp_f_xyz[2];
                            mPartices.at(i)->setForce(temp_f_axyz[1],temp_f_axyz[2],temp_f_axyz[3],temp_f_axyz[0]);
                        }
                    }

                }
            }
        }
        if(mCenterOBJ!=NULL && mCenterOBJ->mVisible)
        {
            mCenterOBJ->mSize += temp_add_size;//0.002f;
            if(mCenterOBJ->mSize > MAX_CENTER_SIZE)
            {
                mCenterOBJ->mSize = MAX_CENTER_SIZE;
            }

            mCenterOBJ->mColor[0] += temp_add_color;
//            mCenterOBJ->mColor[1] += temp_add_color;
            mCenterOBJ->mColor[2] += temp_add_color;

            if(mCenterOBJ->mSize>=1.0f)
            {
                mCenterOBJ->mSize = 1.0f;
            }
        }
        break;
    case EXPLODING:
        if(mCenterOBJ!=NULL && mCenterOBJ->mVisible)
        {
            mCenterOBJ->mSize -= 0.1f;//0.002f;
            //mCenterOBJ->mColor[3]-=0.05f;
            if(mCenterOBJ->mSize<0.0f)
            {
                mCenterOBJ->mSize = 0.0f;
                mCenterOBJ->mVisible = false;
            }
            //mCenterOBJ->mColor[3]-=0.05f;
//            if(mCenterOBJ->mColor[3] < 0.0f)
//            {
//                //mCenterOBJ->mColor[3] = 0.0f;
//                //mCenterOBJ->mVisible = false;//?????????????????????
//            }
        }
        if(mCenterOBJ!=NULL && mCenterOBJ->mVisible);//还可以看到中心体,因此粒子不能爆炸
        else
        {
            for(unsigned int i = 0;i < mPartices.size();++i)
            {
                if(mPartices.at(i)!=NULL && mPartices.at(i)->mVisible)
                {
                    mPartices.at(i)->tick(false);//每个粒子根据所设定的参数自己运动(改变位置)
                    //检查生命周期(考虑如何写在粒子类里面)
                    if(mPartices.at(i)->mTTL == 0)
                    {
                        //qDebug("delete!");
                        mPartices.at(i)->mVisible = false;
                        if(--mRemain_small_particles == 0)
                        {
                            setState(NO_ANIMATE);
                        }
                        //delete mPartices.at(i);
                        //mPartices.at(i) = NULL;
                    }
                    else
                    {
//                        if(qAbs(mPartices.at(i)->mPos[0]) >= 1.0f || qAbs(mPartices.at(i)->mPos[1]) >= 1.0f)
//                        {
//                            //|| qAbs(mPartices.at(i)->mPos[2]) >= 1.0f
//                            //若超出边界,则此点消失
//                            mPartices.at(i)->mTTL = 0;
//                        }
                    }
                }
            }
        }
        break;
    default:
        break;
    }
}

GLWidget::~GLWidget()
{
    delete ui;
    if(mUpdateTimer!=NULL)
    {
        delete mUpdateTimer;
    }
    if(mCenterOBJ!=NULL)
    {
        delete mCenterOBJ;
    }

    for(unsigned int i = 0;i<mPartices.size();++i)
    {
        if(mPartices.at(i)!=NULL)
        {
            delete mPartices.at(i);
            mPartices.at(i) = NULL;
        }
    }
}
