#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "algraph.h"
#include"math.h"

#include    <QGraphicsRectItem>
#include    <QInputDialog>
#include    <QColorDialog>
#include    <QFontDialog>
#include    <QTime>
#include    <QKeyEvent>
#include    <QTextBlock>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scene=new QGraphicsScene(-5000,-5000,10000,10000); //创建QGraphicsScene

    ui->graphicsView->setScene(scene); //与view关联

    scene_2=new QGraphicsScene(-100,-200,300,600); //创建QGraphicsScene_2

    ui->graphicsView_2->setScene(scene_2); //与view_2关联

    print_mapkey();



    ui->graphicsView->scale(0.8,0.8);

    connect(ui->pushButton_3,SIGNAL(clicked()),this,SLOT(initialization_Map()));
    connect(ui->pushButton_5,SIGNAL(clicked()),this,SLOT(add_a_site()));
    connect(ui->pushButton_6,SIGNAL(clicked()),this,SLOT(add_a_line()));
    connect(ui->pushButton_7,SIGNAL(clicked()),this,SLOT(find_way()));


    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(on_actZoomIn_triggered()));
    connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(on_actZoomOut_triggered()));



    //connect(&AG,SIGNAL(out_str(QString)),this,SLOT(out_str1(QString)));
    //connect(&AG,SIGNAL(print_vexsignal()),this,SLOT(print_vexs()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//在提示栏中打印内容
void MainWindow::out_str1(QString str)
{
    ui->textBrowser_2->insertPlainText(str);
}

//在换乘提示栏中打印内容
void MainWindow::out_str2(QString str)
{
    ui->textBrowser->insertPlainText(str);
}

//放大视图
void MainWindow::on_actZoomIn_triggered()
{
    ui->graphicsView->scale(1.1,1.1);
}

//缩小视图
void MainWindow::on_actZoomOut_triggered()
{
    ui->graphicsView->scale(0.9,0.9);
}


void MainWindow::initialization_Map()
{
    AG.CreateALGraph_adjlist();
    ShowAdjList();
    out_str1("初始化加载已有地铁线路完成!\n");
}

//在图形视图上显示地铁线路图
void MainWindow::ShowAdjList()
{
    scene->clear();//清空场景
    ui->graphicsView->update();//刷新视图

    int i,j;
    for(i=0;i<AG.vexnum;i++)
    {
        ArcNode* p=AG.vertices[i].firstarc;
        while(p)
        {
            int l2=p->adjvex;
            //for(j=0;j<p->LineNum;j++)
            //{
               // if(i>=l2)
               // {
                    print_line(p->LineNum,p->Line_no,AG.vertices[i].vx,AG.vertices[i].vy,AG.vertices[l2].vx,AG.vertices[l2].vy,p->trans_way);
               // }
            //}


            p=p->nextarc;
        }

    }


    for(i=0;i<AG.vexnum;i++)
    {
        int highlight_site=0;
        if(AG.start_site==-1)
            highlight_site=0;
        else
        {
            if(AG.PM[AG.end_site][i]==1)
                highlight_site=1;
        }
        //先进行换乘站检测
        ArcNode* q=AG.vertices[i].firstarc;
        if(!q)
        {
            print_site(AG.vertices[i].name_stand,AG.vertices[i].vx,AG.vertices[i].vy,AG.vertices[i].whe_transfer,highlight_site);
            continue;
        }
        int tr_one=q->Line_no[0];
        bool trans=false;
        while(q)
        {
            for(j=0;j<q->LineNum;j++)
            {
                if(q->Line_no[j]!=tr_one)
                {
                    trans=true;//是换乘站
                    break;
                }
            }
            if(trans==true)
                break;
            q=q->nextarc;
        }
        AG.vertices[i].whe_transfer=trans;
        print_site(AG.vertices[i].name_stand,AG.vertices[i].vx,AG.vertices[i].vy,trans,highlight_site);
    }

}

//添加一个站点
void MainWindow::add_a_site()
{
    QString str1=ui->lineEdit->text();
    QString str2=ui->lineEdit_2->text();
    qreal east_x=str2.toDouble();
    QString str3=ui->lineEdit_3->text();
    qreal north_y=str3.toDouble();
    bool transf;
    int index_num=ui->comboBox->currentIndex();
    if(index_num==0)
        transf=false;
    else
        transf=true;


    Status state=AG.AddSite(str1,east_x,north_y,transf);
    if(state==ERROR)
    {
        out_str1("站点名重复，添加失败！\n");
    }
    else
    {
        out_str1("添加站点成功！\n");
    }
    ShowAdjList();
}

//添加一条线路
void MainWindow::add_a_line()
{
    QString str4=ui->lineEdit_4->text();
    QString str5=ui->lineEdit_5->text();
    int index_num=ui->comboBox_2->currentIndex()+1;

    if(index_num>=14)
        index_num+=2;



    Status state=AG.AddLine(index_num,str4,str5);
    if(state==-1)
    {
        out_str1("站点不存在，添加失败！\n");
    }
    else if(state==-2)
    {
        out_str1("线路已存在，添加失败！\n");
    }
    else
    {
        out_str1("添加线路成功！\n");
    }
    ShowAdjList();
}

//打印一个站点
void MainWindow::print_site(QString name_stand1,qreal vx1,qreal vy1,bool whe_transfer1,int highlight_site)
{
    vx1+=map_offset_x;
    vy1+=map_offset_y;


    vx1*=map_scale;
    vy1*=-map_scale;


    qreal circle_x=offset_x+vx1;
    qreal circle_y=offset_y+vy1;


    qreal whe_item;
    if(highlight_site==0)
    {

        whe_item=item_r;
    }
    else
    {
        circle_x-=10;
        circle_y-=10;

        whe_item=item_r*2;
    }



    if(whe_transfer1==false)//非换乘站
    {


        QGraphicsEllipseItem   *item=new QGraphicsEllipseItem(circle_x,circle_y,whe_item,whe_item);
        item->setFlags(QGraphicsItem::ItemIsFocusable);


        if(highlight_site==0)
            item->setBrush(QBrush(Qt::white));
        else
            item->setBrush(QBrush(Qt::red));


        item->setZValue(++frontZ);
        item->setPos(0,0);

        item->setData(ItemId,++seqNum);
        item->setData(ItemDesciption,"圆形");

        scene->addItem(item);
        scene->clearSelection();
        item->setSelected(true);

    }
    else//换乘站
    {
        /*打印矩形1*/
        /*--------------------------------------------*/
        QGraphicsRectItem   *rect1=new QGraphicsRectItem(circle_x,circle_y,whe_item,whe_item);//x,y 为左上角的图元局部坐标，图元中心点为0,0
        rect1->setFlags(QGraphicsItem::ItemIsFocusable);

        if(highlight_site==0)
            rect1->setBrush(QBrush(Qt::white));
        else
            rect1->setBrush(QBrush(Qt::red));

        rect1->setZValue(++frontZ);
        rect1->setPos(0,0);

        rect1->setData(ItemId,++seqNum);
        rect1->setData(ItemDesciption,"矩形");

        scene->addItem(rect1);
        scene->clearSelection();
        rect1->setSelected(true);
        /*--------------------------------------------*/



    }

    /*打印字符*/
    /*--------------------------------------------*/
    QGraphicsTextItem   *item1=new QGraphicsTextItem(name_stand1);

    QFont   font=this->font();
    font.setPointSize(10);
    font.setBold(true);
    item1->setFont(font);

    item1->setFlags(QGraphicsItem::ItemIsFocusable);
    item1->setPos(circle_x+16,circle_y+8);
    item1->setZValue(++frontZ);

    item1->setData(ItemId,++seqNum);
    item1->setData(ItemDesciption,"文字");

    scene->addItem(item1);
    scene->clearSelection();
    item1->setSelected(true);
    /*--------------------------------------------*/

}


//打印一条线路
void MainWindow::print_line(int LineNUM1,int Line_no1[MAX_LINE_NUM],qreal x1,qreal y1,qreal x2,qreal y2,int highlight_line)
{
    x1+=map_offset_x;
    y1+=map_offset_y;
    x2+=map_offset_x;
    y2+=map_offset_y;

    x1*=map_scale;
    y1*=-map_scale;
    x2*=map_scale;
    y2*=-map_scale;



    qreal lx1=offset_x+x1+item_r/2;
    qreal ly1=offset_y+y1+item_r/2;
    qreal lx2=offset_x+x2+item_r/2;
    qreal ly2=offset_y+y2+item_r/2;


    int i;
    for(i=0;i<LineNUM1;i++)
    {
        int Line_colour_no=Line_no1[i];
        int wid=line_width;
        if(highlight_line==1)
            wid=line_width*3;

        qreal llx1=lx1+wid*i;
        qreal lly1=ly1+wid*i;
        qreal llx2=lx2+wid*i;
        qreal lly2=ly2+wid*i;

        QGraphicsLineItem   *line1=new QGraphicsLineItem(llx1,lly1,llx2,lly2);//x,y 为左上角的图元局部坐标，图元中心点为0,0
        line1->setFlags(QGraphicsItem::ItemIsFocusable);



       // QPen    pen(Qt::black);
        QPen    pen(Line_colour[Line_colour_no]);


        pen.setWidth(wid);
        line1->setPen(pen);

        line1->setZValue(++frontZ);
        line1->setPos(0,0);//偏移量

        line1->setData(ItemId,++seqNum);
        line1->setData(ItemDesciption,"直线");

        scene->addItem(line1);
        scene->clearSelection();
        line1->setSelected(true);


    }




}

//寻找站点路线，换乘站
void MainWindow::find_way()
{
    ui->textBrowser->clear();
    memset(AG.visited,0,sizeof(AG.visited));

    QString name1=ui->lineEdit_7->text();
    QString name2=ui->lineEdit_6->text();
    //先查找，只有两个站点均存在才能添加线路，否则返回失败
    int l1,l2;
    l1=AG.LocateVexByName(name1);
    l2=AG.LocateVexByName(name2);
    if(l1==-1||l2==-1)
    {
        out_str2("换乘失败，站点不存在！\n");
        return;
    }
    AG.start_site=l1;
    AG.end_site=l2;
    AG.dij(l1);

    /*初始化边的标记*/
    /*--------------------------------------*/
    int i1;
    for(i1=0;i1<AG.vexnum;i1++)
    {
        ArcNode* p=AG.vertices[i1].firstarc;
        while(p)
        {
            p->trans_way=0;
            p=p->nextarc;
        }
    }
    /*--------------------------------------*/

    int x=AG.SPT[l2];
    QString strx="换乘路线搜索完成，总共经过";
    strx+=QString::number(x)+1;
    strx+="个站点。\n";
    out_str2(strx);


    int i=AG.start_site;
    int j,k;
    QString str1="";
    str1+=AG.vertices[i].name_stand;
    AG.visited[i]=1;

    int trans_no=-1;
    while(1)
    {
        if(i==AG.end_site)
            break;


        ArcNode *p;
        p=AG.vertices[i].firstarc;


        while(p)
        {
            for(j=0;j<AG.vexnum;j++)
            {
                if(AG.PM[AG.end_site][j]==true&&p->adjvex==j&&AG.visited[j]==0)//下一个顶点
                {
                    if(trans_no==-1)
                    {
                        trans_no=p->Line_no[0];
                    }
                    else
                    {
                        int over1=0;
                        for(k=0;k<p->LineNum;k++)
                        {
                            if(p->Line_no[k]==trans_no)
                                over1=1;
                        }
                        if(over1==0)//换乘
                        {
                            trans_no=p->Line_no[0];
                            str1+="(换乘";
                            str1+=QString::number(trans_no);

                            str1+="号线)";
                        }
                    }


                    i=j;
                    AG.visited[j]=1;
                    p->trans_way+=1;
                }
            }

            p=p->nextarc;
        }
        str1+="->";
        str1+=AG.vertices[i].name_stand;
    }
    str1+="\n";
    out_str2(str1);


ShowAdjList();
memset(AG.visited,0,sizeof(AG.visited));

}


//打印图例，在scene_2和view_2中展示
void MainWindow::print_mapkey()
{
    qreal offset_x2=-100;
    qreal offset_x21=offset_x2+100;
    qreal offset_y2=-240;
    qreal add_y=35;


    /*打印字符*/
    /*--------------------------------------------*/
    QGraphicsTextItem   *item0=new QGraphicsTextItem("图例：");

    QFont   font0=this->font();
    font0.setPointSize(10);
    font0.setBold(true);
    item0->setFont(font0);

    item0->setFlags(QGraphicsItem::ItemIsFocusable);
    item0->setPos(offset_x2,offset_y2);
    item0->setZValue(++frontZ);

    item0->setData(ItemId,++seqNum);
    item0->setData(ItemDesciption,"文字");

    scene_2->addItem(item0);
    scene_2->clearSelection();
    item0->setSelected(true);
    /*--------------------------------------------*/
    offset_y2+=add_y*2;


    /*打印字符*/
    /*--------------------------------------------*/
    QGraphicsTextItem   *item1=new QGraphicsTextItem("非换乘站：");

    QFont   font=this->font();
    font.setPointSize(10);
    font.setBold(true);
    item1->setFont(font);

    item1->setFlags(QGraphicsItem::ItemIsFocusable);
    item1->setPos(offset_x2,offset_y2);
    item1->setZValue(++frontZ);

    item1->setData(ItemId,++seqNum);
    item1->setData(ItemDesciption,"文字");

    scene_2->addItem(item1);
    scene_2->clearSelection();
    item1->setSelected(true);
    /*--------------------------------------------*/

    QGraphicsEllipseItem   *item=new QGraphicsEllipseItem(offset_x21,offset_y2,item_r,item_r);
    item->setFlags(QGraphicsItem::ItemIsFocusable);


    item->setBrush(QBrush(Qt::white));

    item->setZValue(++frontZ);
    item->setPos(0,0);

    item->setData(ItemId,++seqNum);
    item->setData(ItemDesciption,"圆形");

    scene_2->addItem(item);
    scene_2->clearSelection();
    item->setSelected(true);




    offset_y2+=add_y;
    /*打印字符*/
    /*--------------------------------------------*/
    QGraphicsTextItem   *item2=new QGraphicsTextItem("换乘站：");

    QFont   font2=this->font();
    font2.setPointSize(10);
    font2.setBold(true);
    item2->setFont(font2);

    item2->setFlags(QGraphicsItem::ItemIsFocusable);
    item2->setPos(offset_x2,offset_y2);
    item2->setZValue(++frontZ);

    item2->setData(ItemId,++seqNum);
    item2->setData(ItemDesciption,"文字");

    scene_2->addItem(item2);
    scene_2->clearSelection();
    item2->setSelected(true);
    /*--------------------------------------------*/

    /*打印矩形1*/
    /*--------------------------------------------*/
    QGraphicsRectItem   *rect1=new QGraphicsRectItem(offset_x21,offset_y2,item_r,item_r);//x,y 为左上角的图元局部坐标，图元中心点为0,0
    rect1->setFlags(QGraphicsItem::ItemIsFocusable);
    rect1->setBrush(QBrush(Qt::white));
    rect1->setZValue(++frontZ);
    rect1->setPos(0,0);

    rect1->setData(ItemId,++seqNum);
    rect1->setData(ItemDesciption,"矩形");

    scene_2->addItem(rect1);
    scene_2->clearSelection();
    rect1->setSelected(true);
    /*--------------------------------------------*/
    offset_y2+=add_y;

    int i;
    for(i=1;i<=17;i++)
    {
        if(i==14||i==15)
        {
            continue;
        }
        /*打印字符*/
        /*--------------------------------------------*/
        QString str1;
        str1+=QString::number(i);
        str1+="号线：";

        QGraphicsTextItem   *item2=new QGraphicsTextItem(str1);

        QFont   font2=this->font();
        font2.setPointSize(10);
        font2.setBold(true);
        item2->setFont(font2);

        item2->setFlags(QGraphicsItem::ItemIsFocusable);
        item2->setPos(offset_x2,offset_y2);
        item2->setZValue(++frontZ);

        item2->setData(ItemId,++seqNum);
        item2->setData(ItemDesciption,"文字");

        scene_2->addItem(item2);
        scene_2->clearSelection();
        item2->setSelected(true);
        /*--------------------------------------------*/

        int Line_colour_no=i;
        int wid=line_width;


        QGraphicsLineItem   *line1=new QGraphicsLineItem(offset_x21,offset_y2+add_y/2-3,offset_x21+150,offset_y2+add_y/2-3);//x,y 为左上角的图元局部坐标，图元中心点为0,0
        line1->setFlags(QGraphicsItem::ItemIsFocusable);



       // QPen    pen(Qt::black);
        QPen    pen(Line_colour[Line_colour_no]);


        pen.setWidth(wid);
        line1->setPen(pen);

        line1->setZValue(++frontZ);
        line1->setPos(0,0);//偏移量

        line1->setData(ItemId,++seqNum);
        line1->setData(ItemDesciption,"直线");

        scene_2->addItem(line1);
        scene_2->clearSelection();
        line1->setSelected(true);




        offset_y2+=add_y;


    }



}



















