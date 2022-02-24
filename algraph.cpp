#include "algraph.h"

ALGraph::ALGraph(QObject *parent) : QObject(parent)
{
    int i;


    //输入框1中输入顶点数
    vexnum = 0;//写在数据中，注意修改
    arcnum = 0;//初始化，后续处理

    start_site=-1;
    end_site=-1;

    //初始化邻接矩阵
    memset(arcs,INFINITY,sizeof(arcs));//邻接矩阵，若不相连则为无限
    memset(PM,0,sizeof(PM));
    memset(SPT,INFINITY,sizeof(SPT));
    memset(visited,0,sizeof(visited));

    //初始化邻接表
    for (i = 0; i < MAX_VERTEX_NUM; i++) {
        vertices[i].data = i;
        vertices[i].firstarc = NULL;
        vertices[i].name_stand="";
        vertices[i].vx=0;
        vertices[i].vy=0;
        vertices[i].whe_transfer=false;//默认不是换乘站
    }
    /*初始化边的标记*/
    /*--------------------------------------*/
    int i1;
    for(i1=0;i1<vexnum;i1++)
    {
        ArcNode* p=vertices[i1].firstarc;
        while(p)
        {
            p->trans_way=0;
            p=p->nextarc;
        }
    }
    /*--------------------------------------*/
}

//按名称查找顶点
int ALGraph::LocateVexByName(QString name1)
{
    int i;
    for (i = 0; i < vexnum; i++)
    {
        if (name1 == vertices[i].name_stand)
            return i;
    }

    return -1;
}

//添加一个站点
Status ALGraph::AddSite(QString name_stand1,qreal vx1,qreal vy1,bool whe_transfer1)
{
    int l;
    l=LocateVexByName(name_stand1);
    if(l!=-1)
        return ERROR;//已经存在同名站点，添加失败

    vertices[vexnum].name_stand=name_stand1;
    vertices[vexnum].vx=vx1;
    vertices[vexnum].vy=vy1;
    vertices[vexnum].whe_transfer=whe_transfer1;
    vexnum++;//最后加

    return OK;
}

//添加一条线路
Status ALGraph::AddLine(int Lineno,QString name1,QString name2)
{
    //先查找，只有两个站点均存在才能添加线路，否则返回失败
    int l1,l2;
    l1=LocateVexByName(name1);
    l2=LocateVexByName(name2);
    if(l1==-1||l2==-1)
    {
        return -1;//站点不存在
    }

    int i;

    ArcNode* p;
    ArcNode* q;

    p=vertices[l1].firstarc;
    while(p)//由于一个边的两侧顶点是对称的，所以只需要检查一个即可
    {
        if(p->adjvex==l2)//若进入，则说明两个顶点之间的边已经存在（线路号可能不同），此时不需要申请空间
        {
            //检验线路是否存在
            int exist_line=0;
            for(i=0;i<p->LineNum;i++)
            {
                if(p->Line_no[i]==Lineno)//线路已存在，不添加
                {
                    exist_line=1;

                    //out_str1("已存在")
                    return -2;
                    //break;
                }
            }
            if(exist_line==0)//线路不存在，添加
            {
                p->Line_no[p->LineNum]=Lineno;
                p->LineNum=p->LineNum+1;



                q=vertices[l2].firstarc;
                while(q)
                {
                    if(q->adjvex==l1)
                    {
                        q->Line_no[q->LineNum]=Lineno;
                        q->LineNum=q->LineNum+1;

                        break;
                    }
                    q=q->nextarc;
                }
            }

            return OK;
            //break;
        }

        p=p->nextarc;
    }


    //此时确定两个顶点之间的边一定不存在，申请空间
    //边的坐标，长度等暂未考虑
    arcs[l1][l2]=1;
    arcs[l2][l1]=1;

    p = (ArcNode*)malloc(sizeof(ArcNode));
    p->adjvex = l2; p->info = 1;
    p->Line_no[0]=Lineno;
    p->LineNum=1;
    p->trans_way=0;
    p->nextarc = vertices[l1].firstarc;
    vertices[l1].firstarc = p;


    q = (ArcNode*)malloc(sizeof(ArcNode));
    q->adjvex = l1; q->info = 1;
    q->Line_no[0]=Lineno;
    q->LineNum=1;
    q->trans_way=0;
    q->nextarc = vertices[l2].firstarc;
    vertices[l2].firstarc = q;

    arcnum++;//边数+1

    return OK;
}

//迪杰斯特拉算法，找最短路径
void ALGraph::dij(int v0)
{
    //初始化
    memset(PM,0,sizeof(PM));
    memset(SPT,INFINITY,sizeof(SPT));

    int final[MAX_VERTEX_NUM];
       int v,w;
       for (v = 0; v < vexnum; ++v)
       {
           final[v] = FALSE;
           SPT[v] = arcs[v0][v];
           for (w = 0; w < vexnum; ++w)
               PM[v][w] = FALSE;
           if (SPT[v] < INFINITY)
           {
               PM[v][v0] = TRUE;
               PM[v][v] = TRUE;
           }
       } //初始化
       SPT[v0] = 0;
       final[v0] = TRUE;
       for (int i = 1; i < vexnum; ++i)
       {
           int min = INFINITY;
           for (w = 0; w < vexnum; ++w)
               if (!final[w])
                   if (SPT[w] < min)
                   {
                       v = w;
                       min = SPT[w];
                   }
           final[v] = TRUE;
           for (w = 0; w < vexnum; ++w)
               if (!final[w] && (min + arcs[v][w] < SPT[w]))
               {
                   SPT[w] = min + arcs[v][w];
                   for (int j = 0; j < vexnum; ++j)
                       PM[w][j] = PM[v][j];
                   PM[w][w] = TRUE;
               } //更新S外的顶点的最短路径值
       }


}

//初始化创建地铁线路图
void ALGraph::CreateALGraph_adjlist()
{
    /*先添加顶点*/
    //地铁1号线
    int x;
        x=AddSite("莘庄",121.3921,31.1168,true);
       x= AddSite("外环路",121.3996,31.1266,false);
       x= AddSite("莲花路",121.4093,31.1367,false);
        x=AddSite("锦江乐园",121.4206,31.1480,false);
        AddSite("上海南站",121.4367,31.1604,true);
        AddSite("漕宝路",121.4404,31.1748,true);
        AddSite("上海体育馆",121.4433,31.1888,true);
        AddSite("徐家汇",121.4432,31.2006,true);
        AddSite("衡山路",121.4531,31.2113,false);
        AddSite("常熟路",121.4566,31.2198,true);
        AddSite("陕西南路",121.4656,31.2220,true);
        AddSite("黄陂南路",121.4797,31.2287,false);
        AddSite("人民广场",121.4810,31.2386,true);
        AddSite("新闸路",121.4748,31.2446,false);
        AddSite("汉中路",121.4652,31.2472,true);
        AddSite("上海火车站",121.4623,31.2536,true);
        AddSite("中山北路",121.4656,31.2649,false);
        AddSite("延长路",121.4618,31.2780,false);
        AddSite("上海马戏城",121.4585,31.2858,false);
        AddSite("汶水路",121.4565,31.2989,false);
        AddSite("彭浦新村",121.4550,31.3129,false);
        AddSite("共康路",121.4534,31.3253,false);
        AddSite("通河新村",121.4479,31.3374,false);
        AddSite("呼兰路",121.4441,31.3461,false);
        AddSite("共富新村",121.4405,31.3614,false);
        AddSite("宝安公路",121.4374,31.3758,false);
        AddSite("友谊西路",121.4344,31.3874,false);
        AddSite("富锦路",121.4310,31.3985,false);

    //地铁2号线
        AddSite("徐泾东",121.3058,31.1944,false);
        AddSite("虹桥火车站",121.3279,31.2003,true);
        AddSite("虹桥2号航站楼",121.3327,31.2006,true);
        AddSite("淞虹路",121.3660,31.2239,false);
        AddSite("北新泾",121.3806,31.2221,false);
        AddSite("威宁路",121.3936,31.2204,false);
        AddSite("娄山关路",121.4103,31.2170,false);
        AddSite("中山公园",121.4228,31.2243,true);
        AddSite("江苏路",121.4377,31.2265,true);
        AddSite("静安寺",121.4540,31.2295,true);
        AddSite("南京西路",121.4659,31.2341,true);
        AddSite("人民广场",121.4810,31.2386,true);
        AddSite("南京东路",121.4908,31.2437,true);
        AddSite("陆家嘴",121.5089,31.2438,false);
        AddSite("东昌路",121.5219,31.2390,false);
        AddSite("世纪大道",121.5335,31.2348,true);
        AddSite("上海科技馆",121.5513,31.2248,false);
        AddSite("世纪公园",121.5573,31.2156,false);
        AddSite("龙阳路",121.5639,31.2091,true);
        AddSite("张江高科",121.5941,31.2077,false);
        AddSite("金科路",121.6085,31.2099,false);
        AddSite("广兰路",121.6277,31.2165,false);
        AddSite("唐镇",121.6627,31.2204,false);
        AddSite("创新中路",121.6801,31.2198,false);
        AddSite("华夏东路",121.6873,31.2029,false);
        AddSite("川沙",121.7044,31.1928,false);
        AddSite("凌空路",121.7306,31.1985,false);
        AddSite("远东大道",121.7620,31.2051,false);
        AddSite("海天三路",121.8033,31.1748,false);
        AddSite("浦东国际机场",121.8129,31.1560,false);

    //地铁3号线
        AddSite("上海南站",121.4367,31.1604,true);
        AddSite("石龙路",121.4495,31.1642,false);
        AddSite("龙漕路",121.4504,31.1761,false);
        AddSite("漕溪路",121.4447,31.1829,false);
        AddSite("宜山路",121.4336,31.1925,true);
        AddSite("虹桥路",121.4280,31.2031,true);
        AddSite("延安西路",121.4235,31.2157,true);
        AddSite("中山公园",121.4228,31.2243,true);
        AddSite("金沙江路",121.4193,31.2379,true);
        AddSite("曹杨路",121.4244,31.2446,true);
        AddSite("镇坪路",121.4368,31.2528,true);
        AddSite("中潭路",121.4474,31.2609,true);
        AddSite("上海火车站",121.4623,31.2536,true);
        AddSite("宝山路",121.4829,31.2573,true);
        AddSite("东宝兴路",121.4866,31.2657,false);
        AddSite("虹口足球场",121.4857,31.2771,true);
        AddSite("赤峰路",121.4889,31.2869,false);
        AddSite("大柏树",121.4897,31.2952,false);
        AddSite("江湾镇",121.4917,31.3113,false);
        AddSite("殷高西路",121.4914,31.3255,false);
        AddSite("长江南路",121.4981,31.3378,false);
        AddSite("淞发路",121.5070,31.3508,false);
        AddSite("张华浜",121.5053,31.3634,false);
        AddSite("淞滨路",121.4993,31.3769,false);
        AddSite("水产路",121.4947,31.3870,false);
        AddSite("宝杨路",121.4861,31.4011,false);
        AddSite("友谊路",121.4823,31.4100,false);
        AddSite("铁力路",121.4676,31.4142,false);
        AddSite("江杨北路",121.4462,31.4142,false);

    //地铁4号线
        AddSite("宜山路",121.4336,31.1925,true);
        AddSite("虹桥路",121.4280,31.2031,true);
        AddSite("延安西路",121.4235,31.2157,true);
        AddSite("中山公园",121.4228,31.2243,true);
        AddSite("金沙江路",121.4193,31.2379,true);
        AddSite("曹杨路",121.4244,31.2446,true);
        AddSite("镇坪路",121.4368,31.2528,true);
        AddSite("中潭路",121.4474,31.2609,true);
        AddSite("上海火车站",121.4623,31.2536,true);
        AddSite("宝山路",121.4829,31.2573,true);
        AddSite("海伦路",121.4956,31.2648,true);
        AddSite("临平路",121.5076,31.2665,false);
        AddSite("大连路",121.5194,31.2639,true);
        AddSite("杨树浦路",121.5239,31.2576,false);
        AddSite("浦东大道",121.5260,31.2457,false);
        AddSite("世纪大道",121.5335,31.2348,true);
        AddSite("浦电路",121.5386,31.2285,false);
        AddSite("蓝村路",121.5343,31.2176,true);
        AddSite("塘桥",121.5253,31.2154,false);
        AddSite("南浦大桥",121.5060,31.2140,false);
        AddSite("西藏南路",121.4960,31.2076,true);
        AddSite("鲁班路",121.4813,31.2050,false);
        AddSite("大木桥路",121.4701,31.1998,true);
        AddSite("东安路",121.4611,31.1973,true);
        AddSite("上海体育场",121.4498,31.1918,false);
        AddSite("上海体育馆",121.4433,31.1888,true);
        AddSite("宜山路",121.4336,31.1925,true);

    //地铁5号线
        AddSite("莘庄",121.3921,31.1168,true);
        AddSite("春申路",121.3923,31.1039,false);
        AddSite("银都路",121.3968,31.0950,false);
        AddSite("颛桥",121.4084,31.0728,false);
        AddSite("北桥",121.4164,31.0511,false);
        AddSite("剑川路",121.4229,31.0325,false);
        AddSite("东川路",121.4263,31.0244,false);
        AddSite("金平路",121.4165,31.0171,false);
        AddSite("华宁路",121.4017,31.0131,false);
        AddSite("文井路",121.3874,31.0091,false);
        AddSite("东川路",121.4263,31.0244,false);
        AddSite("江川路",121.4298,31.0117,false);
        AddSite("西渡",121.4389,30.9956,false);
        AddSite("萧塘",121.4483,30.9722,false);
        AddSite("奉浦大道",121.4557,30.9481,false);
        AddSite("环城东路",121.4700,30.9371,false);
        AddSite("望园路",121.4902,30.9377,false);
        AddSite("金海湖",121.4990,30.9346,false);
        AddSite("奉贤新城",121.5027,30.9198,false);

    //地铁6号线
        AddSite("东方体育中心",121.4871,31.1592,true);
        AddSite("灵岩南路",121.5018,31.1543,false);
        AddSite("上南路",121.5129,31.1547,false);
        AddSite("华夏西路",121.5212,31.1555,false);
        AddSite("高青路",121.5220,31.1655,false);
        AddSite("东明路",121.5179,31.1784,false);
        AddSite("高科西路",121.5165,31.1914,true);
        AddSite("临沂新村",121.5235,31.1987,false);
        AddSite("上海儿童医学中心",121.5300,31.2089,false);
        AddSite("蓝村路",121.5343,31.2176,true);
        AddSite("浦电路",121.5386,31.2285,false);
        AddSite("世纪大道",121.5335,31.2348,true);
        AddSite("源深体育中心",121.5411,31.2390,false);
        AddSite("民生路",121.5499,31.2421,false);
        AddSite("北洋泾路",121.5588,31.2455,false);
        AddSite("德平路",121.5706,31.2517,false);
        AddSite("云山路",121.5792,31.2566,false);
        AddSite("金桥路",121.5883,31.2632,false);
        AddSite("博兴路",121.5932,31.2695,false);
        AddSite("五莲路",121.5945,31.2780,false);
        AddSite("巨峰路",121.5955,31.2862,true);
        AddSite("东靖路",121.5953,31.2966,false);
        AddSite("五洲大道",121.5958,31.3086,false);
        AddSite("洲海路",121.5959,31.3183,false);
        AddSite("外高桥保税区南",121.6085,31.3274,false);
        AddSite("航津路",121.6005,31.3413,false);
        AddSite("外高桥保税区北",121.5934,31.3538,false);
        AddSite("港城路",121.5813,31.3592,false);

    //地铁7号线
        AddSite("花木路",121.5692,31.2175,false);
        AddSite("龙阳路",121.5639,31.2091,true);
        AddSite("芳华路",121.5565,31.1994,false);
        AddSite("锦绣路",121.5465,31.1936,false);
        AddSite("杨高南路",121.5318,31.1933,false);
        AddSite("高科西路",121.5165,31.1914,true);
        AddSite("云台路",121.5069,31.1877,false);
        AddSite("耀华路",121.5012,31.1841,true);
        AddSite("长清路",121.4926,31.1803,false);
        AddSite("后滩",121.4805,31.1778,false);
        AddSite("龙华中路",121.4632,31.1906,true);
        AddSite("东安路",121.4611,31.1973,true);
        AddSite("肇嘉浜路",121.4569,31.2053,true);
        AddSite("常熟路",121.4566,31.2198,true);
        AddSite("静安寺",121.4540,31.2295,true);
        AddSite("昌平路",121.4490,31.2401,false);
        AddSite("长寿路",121.4447,31.2467,true);
        AddSite("镇坪路",121.4368,31.2528,true);
        AddSite("岚皋路",121.4283,31.2625,false);
        AddSite("新村路",121.4290,31.2701,false);
        AddSite("大华三路",121.4296,31.2799,false);
        AddSite("行知路",121.4278,31.2910,false);
        AddSite("大场镇",121.4229,31.2997,false);
        AddSite("场中路",121.4201,31.3098,false);
        AddSite("上大路",121.4150,31.3207,false);
        AddSite("南陈路",121.4054,31.3275,false);
        AddSite("上海大学",121.3955,31.3261,false);
        AddSite("祁华路",121.3801,31.3281,false);
        AddSite("顾村公园",121.3796,31.3502,false);
        AddSite("刘行",121.3689,31.3633,false);
        AddSite("潘广路",121.3623,31.3701,false);
        AddSite("罗南新村",121.3640,31.3945,false);
        AddSite("美兰湖",121.3564,31.4078,false);

    //地铁8号线
        AddSite("沈杜公路",121.5190,31.0670,false);
        AddSite("联航路",121.5172,31.0792,false);
        AddSite("江月路",121.5152,31.0899,false);
        AddSite("浦江镇",121.5129,31.1022,false);
        AddSite("芦恒路",121.5047,31.1247,false);
        AddSite("凌兆新村",121.4961,31.1469,false);
        AddSite("东方体育中心",121.4871,31.1592,false);
        AddSite("杨思",121.5002,31.1666,false);
        AddSite("成山路",121.5033,31.1766,false);
        AddSite("耀华路",121.5012,31.1841,false);
        AddSite("中华艺术宫",121.5002,31.1909,false);
        AddSite("西藏南路",121.4960,31.2076,false);
        AddSite("陆家浜路",121.4926,31.2173,false);
        AddSite("老西门",121.4900,31.2247,false);
        AddSite("大世界",121.4858,31.2331,false);
        AddSite("人民广场",121.4810,31.2386,false);
        AddSite("曲阜路",121.4781,31.2483,false);
        AddSite("中兴路",121.4754,31.2590,false);
        AddSite("西藏北路",121.4752,31.2693,false);
        AddSite("虹口足球场",121.4857,31.2771,false);
        AddSite("曲阳路",121.4973,31.2821,false);
        AddSite("四平路",121.5080,31.2805,false);
        AddSite("鞍山新村",121.5161,31.2789,false);
        AddSite("江浦路",121.5247,31.2808,false);
        AddSite("黄兴路",121.5349,31.2847,false);
        AddSite("延吉中路",121.5413,31.2944,false);
        AddSite("黄兴公园",121.5396,31.3017,false);
        AddSite("翔殷路",121.5384,31.3110,false);
        AddSite("嫩江路",121.5385,31.3209,false);
        AddSite("市光路",121.5385,31.3287,false);

    //地铁9号线
        AddSite("松江南站",121.2373,30.9909,false);
        AddSite("醉白池",121.2356,31.0070,false);
        AddSite("松江体育中心",121.2370,31.0220,false);
        AddSite("松江新城",121.2372,31.0362,false);
        AddSite("松江大学城",121.2392,31.0600,false);
        AddSite("洞泾",121.2370,31.0903,false);
        AddSite("佘山",121.2362,31.1103,false);
        AddSite("泗泾",121.2669,31.1239,false);
        AddSite("九亭",121.3259,31.1436,false);
        AddSite("中春路",121.3440,31.1556,false);
        AddSite("七宝",121.3560,31.1613,false);
        AddSite("星中路",121.3755,31.1638,false);
        AddSite("合川路",121.3912,31.1721,false);
        AddSite("漕河泾开发区",121.4043,31.1762,false);
        AddSite("桂林路",121.4249,31.1810,false);
        AddSite("宜山路",121.4336,31.1925,false);
        AddSite("徐家汇",121.4432,31.2006,false);
        AddSite("肇嘉浜路",121.4569,31.2053,false);
        AddSite("嘉善路",121.4675,31.2085,false);
        AddSite("打浦桥",121.4754,31.2120,false);
        AddSite("马当路",121.4833,31.2156,false);
        AddSite("陆家浜路",121.4926,31.2173,false);
        AddSite("小南门",121.5048,31.2226,false);
        AddSite("商城路",121.5229,31.2361,false);
        AddSite("世纪大道",121.5335,31.2348,false);
        AddSite("杨高中路",121.5551,31.2338,false);
        AddSite("芳甸路",121.5648,31.2383,false);
        AddSite("蓝天路",121.5841,31.2471,false);
        AddSite("台儿庄路",121.6043,31.2584,false);
        AddSite("金桥",121.6178,31.2663,false);
        AddSite("金吉路",121.6351,31.2700,false);
        AddSite("金海路",121.6453,31.2692,false);
        AddSite("顾唐路",121.6630,31.2721,false);
        AddSite("民雷路",121.6746,31.2746,false);
        AddSite("曹路",121.6894,31.2775,false);

    //地铁10号线
        AddSite("新江湾城",121.5131,31.3344,false);
        AddSite("殷高东路",121.5135,31.3275,false);
        AddSite("三门路",121.5145,31.3187,false);
        AddSite("江湾体育场",121.5200,31.3097,false);
        AddSite("五角场",121.5211,31.3038,false);
        AddSite("国权路",121.5167,31.2950,false);
        AddSite("同济大学",121.5130,31.2882,false);
        AddSite("四平路",121.5080,31.2805,false);
        AddSite("邮电新村",121.5008,31.2741,false);
        AddSite("海伦路",121.4956,31.2648,false);
        AddSite("四川北路",121.4907,31.2577,false);
        AddSite("天潼路",121.4887,31.2500,false);
        AddSite("南京东路",121.4908,31.2437,false);
        AddSite("豫园",121.4939,31.2340,false);
        AddSite("老西门",121.4900,31.2247,false);
        AddSite("新天地",121.4817,31.2220,false);
        AddSite("陕西南路",121.4656,31.2220,false);
        AddSite("上海图书馆",121.4507,31.2144,false);
        AddSite("交通大学",121.4410,31.2084,false);
        AddSite("虹桥路",121.4280,31.2031,false);
        AddSite("宋园路",121.4184,31.2025,false);
        AddSite("伊犁路",121.4106,31.2046,false);
        AddSite("水城路",121.3984,31.2050,false);
        AddSite("龙溪路",121.3867,31.2000,false);
        AddSite("龙柏新村",121.3769,31.1832,false);
        AddSite("紫藤路",121.3709,31.1755,false);
        AddSite("龙溪路",121.3867,31.2000,false);
        AddSite("上海动物园",121.3748,31.1959,false);
        AddSite("虹桥1号航站楼",121.3537,31.1976,false);
        AddSite("虹桥2号航站楼",121.3327,31.2006,false);
        AddSite("虹桥火车站",121.3279,31.2003,false);

    //地铁11号线
        AddSite("迪士尼",121.6744,31.1475,false);
        AddSite("康新公路",121.6239,31.1360,false);
        AddSite("秀沿路",121.6050,31.1439,false);
        AddSite("罗山路",121.5996,31.1593,false);
        AddSite("御桥",121.5771,31.1644,false);
        AddSite("浦三路",121.5455,31.1571,false);
        AddSite("三林东",121.5296,31.1523,false);
        AddSite("三林",121.5181,31.1487,false);
        AddSite("东方体育中心",121.4871,31.1592,false);
        AddSite("龙耀路",121.4661,31.1659,false);
        AddSite("云锦路",121.4648,31.1730,false);
        AddSite("龙华",121.4593,31.1790,false);
        AddSite("上海游泳馆",121.4479,31.1855,false);
        AddSite("徐家汇",121.4432,31.2006,false);
        AddSite("交通大学",121.4410,31.2084,false);
        AddSite("江苏路",121.4377,31.2265,false);
        AddSite("隆德路",121.4300,31.2367,false);
        AddSite("曹杨路",121.4244,31.2446,false);
        AddSite("枫桥路",121.4176,31.2479,false);
        AddSite("真如",121.4137,31.2566,false);
        AddSite("上海西站",121.4074,31.2686,false);
        AddSite("李子园",121.3966,31.2747,false);
        AddSite("祁连山路",121.3824,31.2772,false);
        AddSite("武威路",121.3714,31.2825,false);
        AddSite("桃浦新村",121.3560,31.2875,false);
        AddSite("南翔",121.3297,31.3033,false);
        AddSite("马陆",121.2836,31.3254,false);
        AddSite("嘉定新城",121.2610,31.3357,false);
        AddSite("白银路",121.2519,31.3511,false);
        AddSite("嘉定西",121.2344,31.3832,false);
        AddSite("嘉定新城",121.2610,31.3357,false);
        AddSite("上海赛车场",121.2326,31.3379,false);
        AddSite("昌吉东路",121.2068,31.2999,false);
        AddSite("上海汽车城",121.1872,31.2915,false);
        AddSite("安亭",121.1686,31.2943,false);
        AddSite("兆丰路",121.1568,31.2946,false);
        AddSite("光明路",121.1239,31.3021,false);
        AddSite("花桥",121.1107,31.3048,false);

    //地铁12号线
        AddSite("七莘路",121.3697,31.1376,false);
        AddSite("虹莘路",121.3867,31.1430,false);
        AddSite("顾戴路",121.3989,31.1466,false);
        AddSite("东兰路",121.3985,31.1614,false);
        AddSite("虹梅路",121.4039,31.1660,false);
        AddSite("虹漕路",121.4170,31.1701,false);
        AddSite("桂林公园",121.4257,31.1731,false);
        AddSite("漕宝路",121.4404,31.1748,false);
        AddSite("龙漕路",121.4504,31.1761,false);
        AddSite("龙华",121.4593,31.1790,false);
        AddSite("龙华中路",121.4632,31.1906,false);
        AddSite("大木桥路",121.4701,31.1998,false);
        AddSite("嘉善路",121.4675,31.2085,false);
        AddSite("陕西南路",121.4656,31.2220,false);
        AddSite("南京西路",121.4659,31.2341,false);
        AddSite("汉中路",121.4652,31.2472,false);
        AddSite("曲阜路",121.4781,31.2483,false);
        AddSite("天潼路",121.4887,31.2500,false);
        AddSite("国际客运中心",121.5048,31.2558,false);
        AddSite("提篮桥",121.5132,31.2591,false);
        AddSite("大连路",121.5194,31.2639,false);
        AddSite("江浦公园",121.5302,31.2703,false);
        AddSite("宁国路",121.5389,31.2745,false);
        AddSite("隆昌路",121.5508,31.2813,false);
        AddSite("爱国路",121.5594,31.2863,false);
        AddSite("复兴岛",121.5675,31.2871,false);
        AddSite("东陆路",121.5857,31.2886,false);
        AddSite("巨峰路",121.5955,31.2862,false);
        AddSite("杨高北路",121.6094,31.2858,false);
        AddSite("金京路",121.6222,31.2854,false);
        AddSite("申江路",121.6337,31.2858,false);
        AddSite("金海路",121.6453,31.2692,false);

    //地铁13号线
        AddSite("金运路",121.3260,31.2473,false);
        AddSite("金沙江西路",121.3410,31.2473,false);
        AddSite("丰庄",121.3618,31.2483,false);
        AddSite("祁连山南路",121.3739,31.2432,false);
        AddSite("真北路",121.3885,31.2379,false);
        AddSite("大渡河路",121.4012,31.2375,false);
        AddSite("金沙江路",121.4193,31.2379,false);
        AddSite("隆德路",121.4300,31.2367,false);
        AddSite("武宁路",121.4370,31.2406,false);
        AddSite("长寿路",121.4447,31.2467,false);
        AddSite("江宁路",121.4510,31.2507,false);
        AddSite("汉中路",121.4652,31.2472,false);
        AddSite("自然博物馆",121.4689,31.2424,false);
        AddSite("南京西路",121.4659,31.2341,false);
        AddSite("淮海中路",121.4706,31.2261,false);
        AddSite("新天地",121.4817,31.2220,false);
        AddSite("马当路",121.4833,31.2156,false);
        AddSite("世博会博物馆",121.4882,31.2031,false);
        AddSite("世博大道",121.4908,31.1885,false);
        AddSite("长清路",121.4926,31.1803,false);
        AddSite("成山路",121.5033,31.1766,false);
        AddSite("东明路",121.5179,31.1784,false);
        AddSite("华鹏路",121.5331,31.1823,false);
        AddSite("下南路",121.5467,31.1854,false);
        AddSite("北蔡",121.5587,31.1864,false);
        AddSite("陈春路",121.5640,31.1813,false);
        AddSite("莲溪路",121.5728,31.1753,false);
        AddSite("华夏中路",121.5887,31.1812,false);
        AddSite("中科路",121.6086,31.1846,false);
        AddSite("学林路",121.6210,31.1891,false);
        AddSite("张江路",121.6360,31.1947,false);

    //地铁16号线
        AddSite("龙阳路",121.5639,31.2091,false);
        AddSite("华夏中路",121.5887,31.1812,false);
        AddSite("罗山路",121.5996,31.1593,false);
        AddSite("周浦东",121.6136,31.1156,false);
        AddSite("鹤沙航城",121.6178,31.0836,false);
        AddSite("航头东",121.6241,31.0605,false);
        AddSite("新场",121.6555,31.0515,false);
        AddSite("野生动物园",121.7057,31.0564,false);
        AddSite("惠南",121.7682,31.0596,false);
        AddSite("惠南东",121.8002,31.0328,false);
        AddSite("书院",121.8572,30.9648,false);
        AddSite("临港大道",121.9172,30.9298,false);
        AddSite("滴水湖",121.9363,30.9132,false);

    //地铁17号线
        AddSite("虹桥火车站",121.3279,31.2003,false);
        AddSite("诸光路",121.2996,31.1978,false);
        AddSite("蟠龙路",121.2851,31.1924,false);
        AddSite("徐盈路",121.2606,31.1839,false);
        AddSite("徐泾北城",121.2482,31.1812,false);
        AddSite("嘉松中路",121.2305,31.1701,false);
        AddSite("赵巷",121.1987,31.1675,false);
        AddSite("汇金路",121.1583,31.1669,false);
        AddSite("青浦新城",121.1319,31.1645,false);
        AddSite("漕盈路",121.1031,31.1666,false);
        AddSite("淀山湖大道",121.0884,31.1407,false);
        AddSite("朱家角",121.0555,31.1065,false);



    /*再添加边*/
        //地铁1号线
            AddLine(1,"莘庄","外环路");
            AddLine(1,"外环路","莲花路");
            AddLine(1,"莲花路","锦江乐园");
            AddLine(1,"锦江乐园","上海南站");
            AddLine(1,"上海南站","漕宝路");
            AddLine(1,"漕宝路","上海体育馆");
            AddLine(1,"上海体育馆","徐家汇");
            AddLine(1,"徐家汇","衡山路");
            AddLine(1,"衡山路","常熟路");
            AddLine(1,"常熟路","陕西南路");
            AddLine(1,"陕西南路","黄陂南路");
            AddLine(1,"黄陂南路","人民广场");
            AddLine(1,"人民广场","新闸路");
            AddLine(1,"新闸路","汉中路");
            AddLine(1,"汉中路","上海火车站");
            AddLine(1,"上海火车站","中山北路");
            AddLine(1,"中山北路","延长路");
            AddLine(1,"延长路","上海马戏城");
            AddLine(1,"上海马戏城","汶水路");
            AddLine(1,"汶水路","彭浦新村");
            AddLine(1,"彭浦新村","共康路");
            AddLine(1,"共康路","通河新村");
            AddLine(1,"通河新村","呼兰路");
            AddLine(1,"呼兰路","共富新村");
            AddLine(1,"共富新村","宝安公路");
            AddLine(1,"宝安公路","友谊西路");
            AddLine(1,"友谊西路","富锦路");

        //地铁2号线
            AddLine(2,"徐泾东","虹桥火车站");
            AddLine(2,"虹桥火车站","虹桥2号航站楼");
            AddLine(2,"虹桥2号航站楼","淞虹路");
            AddLine(2,"淞虹路","北新泾");
            AddLine(2,"北新泾","威宁路");
            AddLine(2,"威宁路","娄山关路");
            AddLine(2,"娄山关路","中山公园");
            AddLine(2,"中山公园","江苏路");
            AddLine(2,"江苏路","静安寺");
            AddLine(2,"静安寺","南京西路");
            AddLine(2,"南京西路","人民广场");
            AddLine(2,"人民广场","南京东路");
            AddLine(2,"南京东路","陆家嘴");
            AddLine(2,"陆家嘴","东昌路");
            AddLine(2,"东昌路","世纪大道");
            AddLine(2,"世纪大道","上海科技馆");
            AddLine(2,"上海科技馆","世纪公园");
            AddLine(2,"世纪公园","龙阳路");
            AddLine(2,"龙阳路","张江高科");
            AddLine(2,"张江高科","金科路");
            AddLine(2,"金科路","广兰路");
            AddLine(2,"广兰路","唐镇");
            AddLine(2,"唐镇","创新中路");
            AddLine(2,"创新中路","华夏东路");
            AddLine(2,"华夏东路","川沙");
            AddLine(2,"川沙","凌空路");
            AddLine(2,"凌空路","远东大道");
            AddLine(2,"远东大道","海天三路");
            AddLine(2,"海天三路","浦东国际机场");

        //地铁3号线
            AddLine(3,"上海南站","石龙路");
            AddLine(3,"石龙路","龙漕路");
            AddLine(3,"龙漕路","漕溪路");
            AddLine(3,"漕溪路","宜山路");
            AddLine(3,"宜山路","虹桥路");
            AddLine(3,"虹桥路","延安西路");
            AddLine(3,"延安西路","中山公园");
            AddLine(3,"中山公园","金沙江路");
            AddLine(3,"金沙江路","曹杨路");
            AddLine(3,"曹杨路","镇坪路");
            AddLine(3,"镇坪路","中潭路");
            AddLine(3,"中潭路","上海火车站");
            AddLine(3,"上海火车站","宝山路");
            AddLine(3,"宝山路","东宝兴路");
            AddLine(3,"东宝兴路","虹口足球场");
            AddLine(3,"虹口足球场","赤峰路");
            AddLine(3,"赤峰路","大柏树");
            AddLine(3,"大柏树","江湾镇");
            AddLine(3,"江湾镇","殷高西路");
            AddLine(3,"殷高西路","长江南路");
            AddLine(3,"长江南路","淞发路");
            AddLine(3,"淞发路","张华浜");
            AddLine(3,"张华浜","淞滨路");
            AddLine(3,"淞滨路","水产路");
            AddLine(3,"水产路","宝杨路");
            AddLine(3,"宝杨路","友谊路");
            AddLine(3,"友谊路","铁力路");
            AddLine(3,"铁力路","江杨北路");

        //地铁4号线
            AddLine(4,"宜山路","虹桥路");
            AddLine(4,"虹桥路","延安西路");
            AddLine(4,"延安西路","中山公园");
            AddLine(4,"中山公园","金沙江路");
            AddLine(4,"金沙江路","曹杨路");
            AddLine(4,"曹杨路","镇坪路");
            AddLine(4,"镇坪路","中潭路");
            AddLine(4,"中潭路","上海火车站");
            AddLine(4,"上海火车站","宝山路");
            AddLine(4,"宝山路","海伦路");
            AddLine(4,"海伦路","临平路");
            AddLine(4,"临平路","大连路");
            AddLine(4,"大连路","杨树浦路");
            AddLine(4,"杨树浦路","浦东大道");
            AddLine(4,"浦东大道","世纪大道");
            AddLine(4,"世纪大道","浦电路");
            AddLine(4,"浦电路","蓝村路");
            AddLine(4,"蓝村路","塘桥");
            AddLine(4,"塘桥","南浦大桥");
            AddLine(4,"南浦大桥","西藏南路");
            AddLine(4,"西藏南路","鲁班路");
            AddLine(4,"鲁班路","大木桥路");
            AddLine(4,"大木桥路","东安路");
            AddLine(4,"东安路","上海体育场");
            AddLine(4,"上海体育场","上海体育馆");
            AddLine(4,"上海体育馆","宜山路");

        //地铁5号线
            AddLine(5,"莘庄","春申路");
            AddLine(5,"春申路","银都路");
            AddLine(5,"银都路","颛桥");
            AddLine(5,"颛桥","北桥");
            AddLine(5,"北桥","剑川路");
            AddLine(5,"剑川路","东川路");
            AddLine(5,"东川路","金平路");
            AddLine(5,"金平路","华宁路");
            AddLine(5,"华宁路","文井路");
            AddLine(5,"文井路","闵行开发区");
            AddLine(5,"东川路","江川路");
            AddLine(5,"江川路","西渡");
            AddLine(5,"西渡","萧塘");
            AddLine(5,"萧塘","奉浦大道");
            AddLine(5,"奉浦大道","环城东路");
            AddLine(5,"环城东路","望园路");
            AddLine(5,"望园路","金海湖");
            AddLine(5,"金海湖","奉贤新城");

        //地铁6号线
            AddLine(6,"东方体育中心","灵岩南路");
            AddLine(6,"灵岩南路","上南路");
            AddLine(6,"上南路","华夏西路");
            AddLine(6,"华夏西路","高青路");
            AddLine(6,"高青路","东明路");
            AddLine(6,"东明路","高科西路");
            AddLine(6,"高科西路","临沂新村");
            AddLine(6,"临沂新村","上海儿童医学中心");
            AddLine(6,"上海儿童医学中心","蓝村路");
            AddLine(6,"蓝村路","浦电路");
            AddLine(6,"浦电路","世纪大道");
            AddLine(6,"世纪大道","源深体育中心");
            AddLine(6,"源深体育中心","民生路");
            AddLine(6,"民生路","北洋泾路");
            AddLine(6,"北洋泾路","德平路");
            AddLine(6,"德平路","云山路");
            AddLine(6,"云山路","金桥路");
            AddLine(6,"金桥路","博兴路");
            AddLine(6,"博兴路","五莲路");
            AddLine(6,"五莲路","巨峰路");
            AddLine(6,"巨峰路","东靖路");
            AddLine(6,"东靖路","五洲大道");
            AddLine(6,"五洲大道","洲海路");
            AddLine(6,"洲海路","外高桥保税区南");
            AddLine(6,"外高桥保税区南","航津路");
            AddLine(6,"航津路","外高桥保税区北");
            AddLine(6,"外高桥保税区北","港城路");

        //地铁7号线
            AddLine(7,"花木路","龙阳路");
            AddLine(7,"龙阳路","芳华路");
            AddLine(7,"芳华路","锦绣路");
            AddLine(7,"锦绣路","杨高南路");
            AddLine(7,"杨高南路","高科西路");
            AddLine(7,"高科西路","云台路");
            AddLine(7,"云台路","耀华路");
            AddLine(7,"耀华路","长清路");
            AddLine(7,"长清路","后滩");
            AddLine(7,"后滩","龙华中路");
            AddLine(7,"龙华中路","东安路");
            AddLine(7,"东安路","肇嘉浜路");
            AddLine(7,"肇嘉浜路","常熟路");
            AddLine(7,"常熟路","静安寺");
            AddLine(7,"静安寺","昌平路");
            AddLine(7,"昌平路","长寿路");
            AddLine(7,"长寿路","镇坪路");
            AddLine(7,"镇坪路","岚皋路");
            AddLine(7,"岚皋路","新村路");
            AddLine(7,"新村路","大华三路");
            AddLine(7,"大华三路","行知路");
            AddLine(7,"行知路","大场镇");
            AddLine(7,"大场镇","场中路");
            AddLine(7,"场中路","上大路");
            AddLine(7,"上大路","南陈路");
            AddLine(7,"南陈路","上海大学");
            AddLine(7,"上海大学","祁华路");
            AddLine(7,"祁华路","顾村公园");
            AddLine(7,"顾村公园","刘行");
            AddLine(7,"刘行","潘广路");
            AddLine(7,"潘广路","罗南新村");
            AddLine(7,"罗南新村","美兰湖");

        //地铁8号线
            AddLine(8,"沈杜公路","联航路");
            AddLine(8,"联航路","江月路");
            AddLine(8,"江月路","浦江镇");
            AddLine(8,"浦江镇","芦恒路");
            AddLine(8,"芦恒路","凌兆新村");
            AddLine(8,"凌兆新村","东方体育中心");
            AddLine(8,"东方体育中心","杨思");
            AddLine(8,"杨思","成山路");
            AddLine(8,"成山路","耀华路");
            AddLine(8,"耀华路","中华艺术宫");
            AddLine(8,"中华艺术宫","西藏南路");
            AddLine(8,"西藏南路","陆家浜路");
            AddLine(8,"陆家浜路","老西门");
            AddLine(8,"老西门","大世界");
            AddLine(8,"大世界","人民广场");
            AddLine(8,"人民广场","曲阜路");
            AddLine(8,"曲阜路","中兴路");
            AddLine(8,"中兴路","西藏北路");
            AddLine(8,"西藏北路","虹口足球场");
            AddLine(8,"虹口足球场","曲阳路");
            AddLine(8,"曲阳路","四平路");
            AddLine(8,"四平路","鞍山新村");
            AddLine(8,"鞍山新村","江浦路");
            AddLine(8,"江浦路","黄兴路");
            AddLine(8,"黄兴路","延吉中路");
            AddLine(8,"延吉中路","黄兴公园");
            AddLine(8,"黄兴公园","翔殷路");
            AddLine(8,"翔殷路","嫩江路");
            AddLine(8,"嫩江路","市光路");

        //地铁9号线
            AddLine(9,"松江南站","醉白池");
            AddLine(9,"醉白池","松江体育中心");
            AddLine(9,"松江体育中心","松江新城");
            AddLine(9,"松江新城","松江大学城");
            AddLine(9,"松江大学城","洞泾");
            AddLine(9,"洞泾","佘山");
            AddLine(9,"佘山","泗泾");
            AddLine(9,"泗泾","九亭");
            AddLine(9,"九亭","中春路");
            AddLine(9,"中春路","七宝");
            AddLine(9,"七宝","星中路");
            AddLine(9,"星中路","合川路");
            AddLine(9,"合川路","漕河泾开发区");
            AddLine(9,"漕河泾开发区","桂林路");
            AddLine(9,"桂林路","宜山路");
            AddLine(9,"宜山路","徐家汇");
            AddLine(9,"徐家汇","肇嘉浜路");
            AddLine(9,"肇嘉浜路","嘉善路");
            AddLine(9,"嘉善路","打浦桥");
            AddLine(9,"打浦桥","马当路");
            AddLine(9,"马当路","陆家浜路");
            AddLine(9,"陆家浜路","小南门");
            AddLine(9,"小南门","商城路");
            AddLine(9,"商城路","世纪大道");
            AddLine(9,"世纪大道","杨高中路");
            AddLine(9,"杨高中路","芳甸路");
            AddLine(9,"芳甸路","蓝天路");
            AddLine(9,"蓝天路","台儿庄路");
            AddLine(9,"台儿庄路","金桥");
            AddLine(9,"金桥","金吉路");
            AddLine(9,"金吉路","金海路");
            AddLine(9,"金海路","顾唐路");
            AddLine(9,"顾唐路","民雷路");
            AddLine(9,"民雷路","曹路");

        //地铁10号线
            AddLine(10,"新江湾城","殷高东路");
            AddLine(10,"殷高东路","三门路");
            AddLine(10,"三门路","江湾体育场");
            AddLine(10,"江湾体育场","五角场");
            AddLine(10,"五角场","国权路");
            AddLine(10,"国权路","同济大学");
            AddLine(10,"同济大学","四平路");
            AddLine(10,"四平路","邮电新村");
            AddLine(10,"邮电新村","海伦路");
            AddLine(10,"海伦路","四川北路");
            AddLine(10,"四川北路","天潼路");
            AddLine(10,"天潼路","南京东路");
            AddLine(10,"南京东路","豫园");
            AddLine(10,"豫园","老西门");
            AddLine(10,"老西门","新天地");
            AddLine(10,"新天地","陕西南路");
            AddLine(10,"陕西南路","上海图书馆");
            AddLine(10,"上海图书馆","交通大学");
            AddLine(10,"交通大学","虹桥路");
            AddLine(10,"虹桥路","宋园路");
            AddLine(10,"宋园路","伊犁路");
            AddLine(10,"伊犁路","水城路");
            AddLine(10,"水城路","龙溪路");
            AddLine(10,"龙溪路","龙柏新村");
            AddLine(10,"龙柏新村","紫藤路");
            AddLine(10,"紫藤路","航中路");
            AddLine(10,"龙溪路","上海动物园");
            AddLine(10,"上海动物园","虹桥1号航站楼");
            AddLine(10,"虹桥1号航站楼","虹桥2号航站楼");
            AddLine(10,"虹桥2号航站楼","虹桥火车站");

        //地铁11号线
            AddLine(11,"迪士尼","康新公路");
            AddLine(11,"康新公路","秀沿路");
            AddLine(11,"秀沿路","罗山路");
            AddLine(11,"罗山路","御桥");
            AddLine(11,"御桥","浦三路");
            AddLine(11,"浦三路","三林东");
            AddLine(11,"三林东","三林");
            AddLine(11,"三林","东方体育中心");
            AddLine(11,"东方体育中心","龙耀路");
            AddLine(11,"龙耀路","云锦路");
            AddLine(11,"云锦路","龙华");
            AddLine(11,"龙华","上海游泳馆");
            AddLine(11,"上海游泳馆","徐家汇");
            AddLine(11,"徐家汇","交通大学");
            AddLine(11,"交通大学","江苏路");
            AddLine(11,"江苏路","隆德路");
            AddLine(11,"隆德路","曹杨路");
            AddLine(11,"曹杨路","枫桥路");
            AddLine(11,"枫桥路","真如");
            AddLine(11,"真如","上海西站");
            AddLine(11,"上海西站","李子园");
            AddLine(11,"李子园","祁连山路");
            AddLine(11,"祁连山路","武威路");
            AddLine(11,"武威路","桃浦新村");
            AddLine(11,"桃浦新村","南翔");
            AddLine(11,"南翔","马陆");
            AddLine(11,"马陆","嘉定新城");
            AddLine(11,"嘉定新城","白银路");
            AddLine(11,"白银路","嘉定西");
            AddLine(11,"嘉定西","嘉定北");
            AddLine(11,"嘉定新城","上海赛车场");
            AddLine(11,"上海赛车场","昌吉东路");
            AddLine(11,"昌吉东路","上海汽车城");
            AddLine(11,"上海汽车城","安亭");
            AddLine(11,"安亭","兆丰路");
            AddLine(11,"兆丰路","光明路");
            AddLine(11,"光明路","花桥");

        //地铁12号线
            AddLine(12,"七莘路","虹莘路");
            AddLine(12,"虹莘路","顾戴路");
            AddLine(12,"顾戴路","东兰路");
            AddLine(12,"东兰路","虹梅路");
            AddLine(12,"虹梅路","虹漕路");
            AddLine(12,"虹漕路","桂林公园");
            AddLine(12,"桂林公园","漕宝路");
            AddLine(12,"漕宝路","龙漕路");
            AddLine(12,"龙漕路","龙华");
            AddLine(12,"龙华","龙华中路");
            AddLine(12,"龙华中路","大木桥路");
            AddLine(12,"大木桥路","嘉善路");
            AddLine(12,"嘉善路","陕西南路");
            AddLine(12,"陕西南路","南京西路");
            AddLine(12,"南京西路","汉中路");
            AddLine(12,"汉中路","曲阜路");
            AddLine(12,"曲阜路","天潼路");
            AddLine(12,"天潼路","国际客运中心");
            AddLine(12,"国际客运中心","提篮桥");
            AddLine(12,"提篮桥","大连路");
            AddLine(12,"大连路","江浦公园");
            AddLine(12,"江浦公园","宁国路");
            AddLine(12,"宁国路","隆昌路");
            AddLine(12,"隆昌路","爱国路");
            AddLine(12,"爱国路","复兴岛");
            AddLine(12,"复兴岛","东陆路");
            AddLine(12,"东陆路","巨峰路");
            AddLine(12,"巨峰路","杨高北路");
            AddLine(12,"杨高北路","金京路");
            AddLine(12,"金京路","申江路");
            AddLine(12,"申江路","金海路");

        //地铁13号线
            AddLine(13,"金运路","金沙江西路");
            AddLine(13,"金沙江西路","丰庄");
            AddLine(13,"丰庄","祁连山南路");
            AddLine(13,"祁连山南路","真北路");
            AddLine(13,"真北路","大渡河路");
            AddLine(13,"大渡河路","金沙江路");
            AddLine(13,"金沙江路","隆德路");
            AddLine(13,"隆德路","武宁路");
            AddLine(13,"武宁路","长寿路");
            AddLine(13,"长寿路","江宁路");
            AddLine(13,"江宁路","汉中路");
            AddLine(13,"汉中路","自然博物馆");
            AddLine(13,"自然博物馆","南京西路");
            AddLine(13,"南京西路","淮海中路");
            AddLine(13,"淮海中路","新天地");
            AddLine(13,"新天地","马当路");
            AddLine(13,"马当路","世博会博物馆");
            AddLine(13,"世博会博物馆","世博大道");
            AddLine(13,"世博大道","长清路");
            AddLine(13,"长清路","成山路");
            AddLine(13,"成山路","东明路");
            AddLine(13,"东明路","华鹏路");
            AddLine(13,"华鹏路","下南路");
            AddLine(13,"下南路","北蔡");
            AddLine(13,"北蔡","陈春路");
            AddLine(13,"陈春路","莲溪路");
            AddLine(13,"莲溪路","华夏中路");
            AddLine(13,"华夏中路","中科路");
            AddLine(13,"中科路","学林路");
            AddLine(13,"学林路","张江路");

        //地铁16号线
            AddLine(16,"龙阳路","华夏中路");
            AddLine(16,"华夏中路","罗山路");
            AddLine(16,"罗山路","周浦东");
            AddLine(16,"周浦东","鹤沙航城");
            AddLine(16,"鹤沙航城","航头东");
            AddLine(16,"航头东","新场");
            AddLine(16,"新场","野生动物园");
            AddLine(16,"野生动物园","惠南");
            AddLine(16,"惠南","惠南东");
            AddLine(16,"惠南东","书院");
            AddLine(16,"书院","临港大道");
            AddLine(16,"临港大道","滴水湖");

        //地铁17号线
            AddLine(17,"虹桥火车站","诸光路");
            AddLine(17,"诸光路","蟠龙路");
            AddLine(17,"蟠龙路","徐盈路");
            AddLine(17,"徐盈路","徐泾北城");
            AddLine(17,"徐泾北城","嘉松中路");
            AddLine(17,"嘉松中路","赵巷");
            AddLine(17,"赵巷","汇金路");
            AddLine(17,"汇金路","青浦新城");
            AddLine(17,"青浦新城","漕盈路");
            AddLine(17,"漕盈路","淀山湖大道");
            AddLine(17,"淀山湖大道","朱家角");
            AddLine(17,"朱家角","东方绿舟");





}











