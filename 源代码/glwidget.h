#ifndef GLWIDGET_H
#define GLWIDGET_H


//mState程序状态
#define NO_ANIMATE 0
//收缩
#define SHRINKING 1
//爆炸
#define EXPLODING 2

#define MAX_SMALL_PARTICLES 10000

//200 = 25(fps) * 8(time(s))
#define MAX_FRAME_NUM 160

#define BITMAP_ID 0x4D42

#include "particle.h"
#include "centerobj.h"
#include <GL/glu.h>
#include <GL/glut.h>
#include <QWidget>
#include <QtOpenGL>
#include <vector>

namespace Ui {
class GLWidget;
}
using std::vector;

class GLWidget : public QGLWidget
{
    Q_OBJECT
    
public:
    explicit GLWidget(QWidget *parent = 0);
    ~GLWidget();
    
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    //初始化粒子
    void initPartices();
    //初始化爆炸前的粒子
    void initExploding();
    //初始化收缩前的粒子
    void initShrinking();
    //每一帧更新粒子状态
    void updatePartices();
    //设定当前程序状态()
    void setState(int state);
private:
    Ui::GLWidget *ui;
    QTimer *mUpdateTimer;
    vector<Particle*> mPartices;
    int mState;//NO_ANIMATE 0 SHRINKING 1 EXPLODING 2
    int mRemain_small_particles;
    CenterObj *mCenterOBJ;
    int mCameraShakingFlag;
    int mCurrentFrame;


    //about texture
    //bool twinkle;//启动星星闪烁的标志!!
    void loadTextures();
    GLuint texture[2];//用来存储1星星的纹理信息

private slots:
    void myUpdate();
};

#endif // GLWIDGET_H
