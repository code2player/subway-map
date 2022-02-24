#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include    <QTextBlock>
#include    <QGraphicsScene>
#include    <QLabel>
#include "algraph.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    static const int ItemId = 1;   //绘图项自定义数据的key
    static const int ItemDesciption = 2;   //绘图项自定义数据的key

    static const int offset_x=0;//偏移量x
    static const int offset_y=0;//偏移量y

    static const int item_w=60;//矩形宽度
    static const int item_h=40;//矩形高度

    static const int item_r=20;//圆的半径和正方形的边长

    static const int line_width=6;//线条宽度

    qreal map_offset_x=-121.5;//地图坐标偏移量x
    qreal map_offset_y=-31.2;//地图坐标偏移量y
    qreal map_scale=10000;//地图映射视图的比例尺
    /*经度坐标正确，纬度坐标要取反*/

    //线路颜色，0，14，15，18，19号线未开通
    QColor Line_colour[20]={QColor{0,0,0},

                            QColor{235,0,42},
                            QColor{143,192,30},
                            QColor{255,221,1},
                            QColor{65,26,134},
                            QColor{154,71,154},

                            QColor{221,2,102},
                            QColor{238,112,9},
                            QColor{1,152,221},
                            QColor{134,203,238},
                            QColor{194,175,203},

                            QColor{141,32,47},
                            QColor{1,119,98},
                            QColor{240,151,189},
                            QColor{0,0,0},
                            QColor{0,0,0},

                            QColor{149,206,189},
                            QColor{187,119,104},
                            QColor{0,0,0},
                            QColor{0,0,0}
                           };

    int seqNum=0;
    int backZ=0;
    int frontZ=0;

    QGraphicsScene  *scene;
    QGraphicsScene  *scene_2;

public:
    ALGraph AG;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void initialization_Map();

    void ShowAdjList();

    void on_actZoomIn_triggered();

    void on_actZoomOut_triggered();

    void print_site(QString name_stand1,qreal vx1,qreal vy1,bool whe_transfer1,int highlight_site);

    void print_line(int LineNUM1,int Line_no1[MAX_LINE_NUM],qreal x1,qreal y1,qreal x2,qreal y2,int highlight_line);

    void add_a_site();

    void add_a_line();

    void find_way();

    void print_mapkey();




  //  void show_one_item(qreal x,qreal y,int num,bool head,bool tail);

    void out_str1(QString str);

    void out_str2(QString str);

  //  void print_vexs();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
