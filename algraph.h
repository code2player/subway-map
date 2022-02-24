#ifndef ALGRAPH_H
#define ALGRAPH_H

#include <QObject>
#include    <QTextBlock>
#include    <QTextDocument>

#define TRUE		1
#define FALSE		0
#define OK		1
#define ERROR		0
#define INFEASIBLE	-1
#define TOVERFLOW	-2
typedef int Status;

#define MAX_VERTEX_NUM 400//最大支持400个站点

#define MAX_LINE_NUM 20//最大支持20个线路

#define VertexType int

typedef int AdjMatrix[MAX_VERTEX_NUM][MAX_VERTEX_NUM];

#define QUEUE_MAX_SIZE	1000	//初始大小定义为100（可按需修改）

typedef int QElemType;

#define SUBWAY_STAND_NUM	250	//初始化的站点总数

typedef int PathMatrix[MAX_VERTEX_NUM][MAX_VERTEX_NUM];
//最短路径数组
typedef int ShortPathTable[MAX_VERTEX_NUM];
//最短路径长度

#define INFINITY 1000000

class ArcNode;


//站点
class VNode {
public:
    VertexType data;//数据
    ArcNode* firstarc;//结点的首个边



    QString name_stand;//站点名
    qreal vx,vy;//站点坐标
    bool whe_transfer;//是否为换乘站




    friend class ALGraph;
};


//线路
class ArcNode {
public:
    int adjvex;//顶点序号
    ArcNode* nextarc;//下一条边
    int info;//标号信息

    int trans_way;//是否为经过的线路



    int LineNum;//属于几条线路（最大MAX_LINE_NUM）
    int Line_no[MAX_LINE_NUM];//所属线路
    qreal length;//线路长度，计算得到


    friend class ALGraph;
    friend class VNode;
}; //边结点类型



class ALGraph : public QObject
{
    Q_OBJECT

public:
    VNode vertices[MAX_VERTEX_NUM]; //邻接表
    /*邻接表形式，首结点为始发站结点，每一轮*/

    AdjMatrix arcs;//邻接矩阵
    int vexnum, arcnum;//顶点数、边数
    int visited[MAX_VERTEX_NUM];//访问标志

    //寻路的起点站，终点站
    int start_site;
    int end_site;

    //迪杰斯特拉算法
    PathMatrix PM;//存储路径
    ShortPathTable SPT;//存储长度




    void CreateALGraph_adjlist();
    Status AddSite(QString name_stand1,qreal vx1,qreal vy1,bool whe_transfer1);
    Status AddLine(int LineNUM,QString name1,QString name2);
    int LocateVexByName(QString name1);

    void dij(int v0);






public:
    explicit ALGraph(QObject *parent = nullptr);

signals:

public slots:
};

#endif // ALGRAPH_H
