#include"head.h"

int dirx[] = {0,1,0,-1};
int diry[] = {-1,0,1,0};//四个方向
int img_zidan_detx1[] = {-SIZE/2,-SIZE,-SIZE/2,0};
int img_zidan_dety1[] = {0,-SIZE/2,-SIZE,-SIZE/2};//子弹左上角相对弹头偏移量,画子弹要用
int check_if_movex[][4] = { {-1,-1,-1,0},{0,-1,0,0} };
int check_if_movey[][4] = { {0,-1,-1,-1 },{0,0,-1,0} };//表示子弹合法检查位置相对弹头偏移单位数，相邻两个都检查
int all_tank_num = 3;//其他类型坦克数量
int rest_num;//剩余坦克数量
int my_lives;//自己的坦克数量
int now_level;//当前关卡
int rand_seed;//随机种子
int level;//自身当前等级
int mat[NUMY+2][NUMX+2];//存储游戏地图
int flag[NUMY + 2][NUMX + 2];//存储坦克标记
int stop_epoch;//表示停止回合,道具使用
int tool_speed;//道具更新速率
int door_speed;//传送门更新速率
//123分别存储三种障碍物，(-10)存储自己坦克所在位置,(-1)-(-9)存储敌方坦克位置，一个坦克占据16个单元格，左上坐标标记
int num_tank;//当前场上的坦克
int birth_wz[][2] = { {MAXX / 2 - SIZE * 12,MAXY - 4 * SIZE},			//第一个是自己的出生点，剩下的是其他坦克出生点
					{0,0},{MAXX / 2 - 16 * SIZE,0},{MAXX / 2 + 16 * SIZE,0},{MAXX - 4 * SIZE,0} }; 
int rep_map[][5] = { {2,6,12,7,14},{2,6,11,10,12},{2,9,12,10,14} };//修复老家的元素
// 配置关卡
// 1表示铁块，2表示红砖，3表示草地，后面是x1 y1 x2 y2 是在最小单元为2*SIZE的地图上的每个类型方块的左上角和右下角坐标
int map[100][5];//存储每一关的地图
int all_map[3][70][5] =  //所有地图
{
	{
		{2,0,2,2,8} ,{2,4,2,6,3},{1,7,0,9,2},{1,12,2,14,4},
		{3,4,4,6,6},{2,6,4,14,6},{3,14,4,16,6},{1,2,6,4,10},
		{2,1,10,3,11},{3,4,7,5,8},{1,5,7,7,9},{2,7,7,11,9},
		{1,11,7,13,9},{2,13,7,15,14},{2,1,11,3,14},{1,4,3,6,4},
		{2,6,12,7,14},{2,6,11,10,12},{2,9,12,10,14}
	},
	{
		{1,2,2,4,4},{1,7,0,9,1},{1,12,2,13,4},{1,5,3,7,4},{1,10,3,11,4},{1,6,6,7,8},
		{1,9,6,10,8},{1,12,6,13,8},{1,7,7,8,10},{1,9,9,10,10},{1,13,5,15,6},{1,1,2,2,4},
		{2,5,1,11,3},{2,7,3,9,5},{2,13,2,15,4},{2,2,6,5,8},{2,3,5,5,6},{2,7,6,9,8},{2,13,6,15,9},
		{2,2,10,3,14},{2,3,9,6,10},{2,12,9,12,10},{2,12,12,13,14},{2,14,10,15,14},
		{2,1,10,2,14},{2,1,6,2,8},{2,10,6,12,8},{2,0,6,1,7},{2,15,6,16,7},
		{3,0,2,1,4},{3,0,7,1,8},{3,0,10,1,14},{3,4,2,5,4},{3,11,2,12,4},{3,7,5,7,6},{3,15,2,16,4},
		{3,15,7,16,8},{3,15,10,16,14},{3,0,8,1,10},{3,15,8,16,10},
		{2,6,12,7,14},{2,6,11,10,12},{2,9,12,10,14}
	},
	{
		{2,0,4,16,10},
		{2,6,12,7,14},{2,6,11,10,12},{2,9,12,10,14}
	}
};
int all_other_tank[] = {20,25,30};//存储每一关的敌方坦克数量
int tool_update_speed[] = { 1000,1000,400 };//道具更新速率
int door_update_speed[] = { 1500,1500,500 };//传送门更新速率

struct Zidan {
	int who;//记录的是在坦克系列，1-3是自己，其他是其他人
	int dir;//方向
	int x;
	int y;//坐标
	bool count;//为了标记是否是第一次被更新
};
queue<Zidan>qz;

struct Tool {
	int num;//更新计时器
	bool exist;//是否存在
	int flag[NUMY + 2][NUMX + 2];//存储道具信息
}tool;

struct Door {
	int num;
	int x1, y1, x2, y2;//两个传送门的坐标
	int arrx1, arrx2, arry1, arry2;//两个传送门对应的到达坐标
	int dir1, dir2;//出传送门的方向，可额能没用到?
	int type;//传送门类型，上下 0/左右 1
	int flag[NUMY + 2][NUMX + 2];//存储信息
}door;//关于传送门的结构体

struct Tank {
	int type;//类型
	int HP;//血量
	int dir;//方向,上下左右0123
	int x, y;//坐标
	int bef_fight;//上一次攻击到现在的间隔
	int bef_move;//上一次移动到现在的间隔
	int stop_time;//表示该坦克在几个周期内不移动
	int dir_stop_time;//表示该坦克在几个周期内不改变方向
	int move_type;//1代表优先向中间，2代表优先向下面
	int state;//当前状态
}tank[10];//最多其实就4个,0号始终是自己

// 不同坦克类型属性
struct Type {
	/*自身属性*/
	int HP;//血量
	int Speed;//速度,单位是更新周期,一个更新周期子弹移动一次
	int Power;//伤害
	int Fight_det;//攻击间隔,单位是更新周期
}type[10];
IMAGE img[40];//存储图片

/*调试*/
void test() {
	clearrectangle(0, 0, 500, 500);
}//调试

/*程序停止*/
void stop() {
	system("pause");
}//程序停止

/*生成随机数*/
int MyRand() {
	srand(rand_seed);
	int t = rand();
	rand_seed = rand();//重置
	return t;
}//生成随机数

/*读入和写入地图*/
int get_map(int map[][5],int all_map[][5])
{
	//返回数量
	int num = 0;
	for (int i = 0; i < 70; i++) {
		if (all_map[i][0] == 0) {//代表是最后一个
			return num;
		}
		else {//不是空
			for (int j = 0; j < 5; j++) {
				map[i][j] = all_map[i][j];
			}
			num++;
		}
	}
}//读入和写入地图

/*全局初始化*/
void ini_all() {
	/*自身属性状态*/
	//一级
	type[1].HP = 1;
	type[1].Speed = 8;
	type[1].Power = 10;
	type[1].Fight_det = 30;
	//二级
	type[2].HP = 1;
	type[2].Speed = 7;
	type[2].Power = 15;
	type[2].Fight_det = 25;
	//三级
	type[3].HP = 2;
	type[3].Speed = 6;
	type[3].Power = 20;
	type[3].Fight_det = 20;
	/*其他坦克状态*/
	//第一种血量高的
	type[4].HP = 2;
	type[4].Speed = 10;
	type[4].Power = 10;
	type[4].Fight_det = 40;
	//第二种速度快的
	type[5].HP = 1;
	type[5].Speed = 5;
	type[5].Power = 10;
	type[5].Fight_det = 30;
	//第三种攻击快
	type[6].HP = 1;
	type[6].Speed = 8;
	type[6].Power = 10;
	type[6].Fight_det = 20;
	//第四种是的一种被打了之后的样子
	type[7].HP = 1;
	type[7].Speed = 10;
	type[7].Power = 10;
	type[7].Fight_det = 40;
	/*图片*/
	loadimage(&img[1], "./imgs/type1_1.png");
	loadimage(&img[2], "./imgs/type1_2.png");
	loadimage(&img[3], "./imgs/type2_1.png");
	loadimage(&img[4], "./imgs/type2_2.png");
	loadimage(&img[5], "./imgs/type3_1.png");
	loadimage(&img[6], "./imgs/type3_2.png");
	loadimage(&img[7], "./imgs/type4_1.png");
	loadimage(&img[8], "./imgs/type4_2.png");
	loadimage(&img[9], "./imgs/type5_1.png");
	loadimage(&img[10], "./imgs/type5_2.png");
	loadimage(&img[11], "./imgs/type6_1.png");
	loadimage(&img[12], "./imgs/type6_2.png");
	loadimage(&img[13], "./imgs/type7_1.png");
	loadimage(&img[14], "./imgs/type7_2.png");
	loadimage(&img[15], "./imgs/铁块.png");//铁块
	loadimage(&img[16], "./imgs/红砖.png");//红砖
	loadimage(&img[17], "./imgs/草丛_1.png");//草地
	loadimage(&img[18], "./imgs/草丛_2.png");//草地
	loadimage(&img[19], "./imgs/家.png");//家
	loadimage(&img[20], "./imgs/敌房子弹_1.png");
	loadimage(&img[21], "./imgs/敌房子弹_2.png");
	loadimage(&img[22], "./imgs/我房子弹_1.png");
	loadimage(&img[23], "./imgs/我房子弹_2.png");//子弹
	loadimage(&img[24], "./imgs/家爆炸.png");//家被炸掉了
	loadimage(&img[25], "./imgs/坦克爆炸.png");//坦克被炸掉了
	loadimage(&img[26], "./imgs/炸弹.png");//随机炸掉一个坦克
	loadimage(&img[27], "./imgs/停止.png");//其他坦克停止操作几个周期
	loadimage(&img[28], "./imgs/升级.png");//坦克升级
	loadimage(&img[29], "./imgs/修复.png");//修复老家
	loadimage(&img[30], "./imgs/传送门_上下.png");//传送门类型1
	loadimage(&img[31], "./imgs/传送门_左右.png");//传送门类型2
}//全局初始化

/*关卡初始化*/
void ini_every(int mp[][5], int num) {
	door.num = 1;//一开始就刷新传送门
	door.x1 = -1;
	door.x2 = -1;
	door.y1 = -1;
	door.y2 = -1;
	tool.num = 0;
	tool.exist = 0;
	tool_speed = tool_update_speed[now_level];
	door_speed = door_update_speed[now_level];
	rest_num = all_other_tank[now_level];//敌方坦克数量
	my_lives = 3;
	level = 1;//一级
	putimage(MAXX + 100, 100, &img[2]);
	putimage(MAXX + 100, 200, &img[8]);//计数榜里面的坦克
	PrintTankNum();
	srand((unsigned)time(NULL));//调整种子
	rand_seed = rand();
	line(MAXX + 1, 0, MAXX + 1, MAXY + 1);//分割线
	putimage(7 * SIZE * 4, 12 * SIZE * 4, &img[19]);
	for (int i = 0; i <= NUMY; i++){
		for (int j = 0; j <= NUMX; j++) {
			mat[i][j] = 0;
			flag[i][j]= -1;//先初始化为-1 
			tool.flag[i][j] = 0;
			door.flag[i][j] = 0;
		}
	}
	for (int i = 12 * 4; i <= 12 * 4 + 8; i++) {
		for (int j = 7 * 4; j <= 7 * 4 + 8; j++) {
			flag[i][j] = 10;//标记为老家位置
		}
	}
	for (int i = 0; i < num; i++) {
		int k = mp[i][0];
		int x1 = mp[i][1]*4;
		int y1 = mp[i][2]*4;
		int x2 = mp[i][3]*4-1;
		int y2 = mp[i][4]*4-1;//右下角方块的左上角坐标
		for (int xx = x1; xx <= x2; xx++) {
			for (int yy = y1; yy <= y2; yy++) {
				mat[yy][xx] = k;
			}
		}
		if (k == 2) {//泥土块最小单位SIZE
			for (int xx = x1; xx <= x2; xx++) {
				for (int yy = y1; yy <= y2; yy++) {
					putimage(xx*SIZE, yy*SIZE, &img[16]);
				}
			}
		}
		else {//铁块和草方块最小单位2*SIZE
			for (int xx = mp[i][1]*2; xx <= mp[i][3]*2-1; xx++) {
				for (int yy = mp[i][2]*2; yy <= mp[i][4]*2-1; yy++) {
					if (k == 1) {//铁块
						putimage(xx*SIZE * 2, yy*SIZE * 2, &img[15]);
					}
					else {//草地
						putimage(xx*SIZE * 2, yy*SIZE * 2, &img[17], SRCAND);
						putimage(xx*SIZE * 2, yy*SIZE * 2, &img[18], SRCPAINT);
					}
				}
			}
		}
	}
}//关卡初始化

/*转换数字为字符串*/
void deal(int x, char *a) {
	stack <int>sst;
	while (x) {
		sst.push(x % 10);
		x /= 10;
	}
	int l = 0;
	while (!sst.empty()) {
		a[l++] = sst.top() + '0';
		sst.pop();
	}
	a[l] = '\0';
	if (a[0] == '\0') {
		a[0] = '0';
		a[1] = '\0';
	}
}//转换数字为字符串

/*打印游戏结束*/
void PrintGameOver() {
	settextcolor(WHITE);
	int size = 100;//字体大小
	settextstyle(size, size, 0);
	outtextxy(90,200,"游戏结束");
}//打印游戏结束

/*打印关卡过渡动画*/
void printNextLevel() {
	cleardevice();
	char a[100];
	char info[100] = "第";//拼接数组
	//打印时间耗尽
	settextcolor(BROWN);
	int size =100;//字体大小
	settextstyle(size, size, 0);
	deal(now_level+1, a);
	strcat(info, a);
	strcat(info, "关");
	outtextxy(MAXX / 2 + 200 - size * 2, MAXY / 2 - size, info);
	Sleep(1000);
	cleardevice();
}//打印关卡过渡动画

/*通关*/
void PrintSucess() {
	settextcolor(WHITE);
	int size = 100;//字体大小
	settextstyle(size, size, 0);
	outtextxy(90, 200, "成功通关");
}//通关

/*打印坦克数量*/
void PrintTankNum() {
	char a[100];

	//计算自己坦克数量
	settextcolor(WHITE);
	settextstyle(40, 40, 0);
	outtextxy(MAXX+100+4*SIZE, 120, "X");
	deal(my_lives, a);
	//这是为了防止倒计时显示错误
	clearrectangle(MAXX + 100 + 4 * SIZE + 50, 120, MAXX + 100 + 4 * SIZE + 50 + 70, 120 + 50);
	outtextxy(MAXX + 100 + 4*SIZE+50, 120, a);

	//计算敌方坦克数量
	settextcolor(WHITE);
	settextstyle(40, 40, 0);
	outtextxy(MAXX + 100 + 4 * SIZE, 220, "X");
	deal(rest_num, a);
	clearrectangle(MAXX + 100 + 4 * SIZE + 50, 220, MAXX + 100 + 4 * SIZE + 50 + 70, 220 + 50);
	outtextxy(MAXX + 100 + 4 * SIZE + 50, 220, a);
}//打印坦克数量

/*标记坦克占据空间*/
void mark_tank(int x,int y,int mark_num) {
	for (int i = 1; i <= 3; i++) {
		for (int j = 1; j <= 3; j++) {
			flag[y / SIZE + i][x / SIZE + j] = mark_num;
		}
	}
}//标记坦克占据空间

/*更新坦克标记点*/
void update_mark(int dir, int x, int y,int index) {//新的方向和新的坐标定位点
	switch (dir) {//四种方向，上右下左
		//坦克标记点有5个
	case 0:
		for (int i = 1; i < 4; i++) {
			flag[y / SIZE + 1][x / SIZE + i] = index;
			flag[y / SIZE + 4][x / SIZE + i] = -1;//清空后面的
		}
		break;
	case 1:
		for (int i = 1; i < 4; i++) {
			flag[y / SIZE + i][x / SIZE + 3] = index;
			flag[y / SIZE + i][x / SIZE ] = -1;//清空后面的
		}
		break;
	case 2:
		for (int i = 1; i < 4; i++) {
			flag[y / SIZE ][x / SIZE + i] = -1;
			flag[y / SIZE + 3][x / SIZE + i] = index;
		}
		break;
	case 3:
		for (int i = 1; i < 4; i++) {
			flag[y / SIZE + i][x / SIZE+1] = index;
			flag[y / SIZE + i][x / SIZE + 4] = -1;//清空后面的
		}
		break;
	}
}//更新坦克标记点

/*产生一个坦克*/
Tank add_tank(int add_place,int wz) {
	//add_place表示产生位置，取值1234表示敌方坦克的出生位置，wz表示在坦克数组中的位置
	Tank t;
	t.dir = 3;
	t.bef_fight = 0;
	t.bef_move = 0;
	t.stop_time = rand()%5;
	t.dir_stop_time = 0;
	if (add_place) {//敌方坦克
		srand(rand_seed);//重置时间种子
		rand_seed = rand();//随机种子链
		int index = rand() % all_tank_num + 4;
		t.move_type = rand() % 2 + 1;
		t.type = index;
		t.HP = type[index].HP;
		t.x = birth_wz[add_place][0];
		t.y = birth_wz[add_place][1];
		t.state = 1;//表示活着
		mark_tank(t.x, t.y, wz);
	}
	else {//自己
		t.HP = type[level].HP;
		t.type = level;//通过type索引可以得到速度等信息
		t.x = birth_wz[0][0];
		t.y = birth_wz[0][1];
		mark_tank(t.x, t.y, 0);//0号标记是自己
	}
	return t;
}//产生一个坦克

/*初始化坦克队列*/
void ini_tank() {
	num_tank = 0;
	tank[0]=add_tank(0,0);//0表示产生己方坦克，其他表示其他坦克，编号index
	//在1 2 3号出生点设置敌方坦克
	for (int i = 1; i <= 3; i++) {
		num_tank++;
		tank[i]=add_tank(i, num_tank);
	}
}//初始化坦克队列

/*检查是否穿过草丛*/
bool CheckGrass(int xx,int yy) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (mat[yy / SIZE + i][xx / SIZE + j] == 3) {
				return 1;//有穿过草地
			}
		}
	}
	return 0;
}//检查是否穿过草丛

/*修复草地---坦克经过草地*/
void add_grass(int xx, int yy) {
	bool tmp_map[4][4];
	int detx = 0, dety = 0;//左上角
	int last_x, last_y;//右下角
	bool if_first = 0;//标记是否是第一次出现，是为了标记左上角坐标
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			tmp_map[i][j] = (mat[yy / SIZE + i][xx / SIZE + j] == 3);//有坦克并且经过
			if (!if_first&&mat[yy / SIZE + i][xx / SIZE + j] == 3) {
				detx = j;
				dety = i;
				if_first = 1;
			}
			if (tmp_map[i][j]) {
				last_x = j;
				last_y = i;
			}
		}
	}
	detx -= (xx / SIZE + detx) % 2;
	dety -= (yy / SIZE + dety) % 2;
	for (int i = dety; i <= last_y; i += 2) {
		for (int j = detx; j <= last_x; j += 2) {
			if (i < 0&&tmp_map[0][j] || j < 0&&tmp_map[i][0] || tmp_map[i][j]) {//需要放上图片
				putimage(xx + j * SIZE, yy + i * SIZE, &img[17], SRCAND);
				putimage(xx + j * SIZE, yy + i * SIZE, &img[18], SRCPAINT);
			}
		}
	}
}//修复草地---坦克经过草地

/*画一个坦克*/
void draw_a_tank(Tank t,int xx,int yy) {
	IMAGE ans1,ans2;
	clearrectangle(t.x+3, t.y+3, t.x + 4 * SIZE-1, t.y + 4 * SIZE-1);
	rotateimage(&ans1, &img[t.type*2-1], -PI / 2 * t.dir);//顺时针旋转dir个90度,然后赋值给ans
	rotateimage(&ans2, &img[t.type*2], -PI / 2 * t.dir);
	putimage(xx + 4, yy + 4, &ans1, SRCAND);
	putimage(xx + 4, yy + 4, &ans2, SRCPAINT);
	if (CheckGrass(t.x,t.y)) {//如果草丛
		add_grass(t.x, t.y);
	}
}//画一个坦克

/*画全部坦克*/
void draw_all_tank() {
	for (int i = 0; i <= num_tank; i++) {
		draw_a_tank(tank[i],tank[i].x,tank[i].y);
	}
}//画全部坦克

/*表示从旧的变成新的*/
void up_date_type(int new_type, int wz) {
	tank[wz].type = new_type;
	tank[wz].HP = type[new_type].HP;
	draw_a_tank(tank[wz], tank[wz].x, tank[wz].y);
}//表示从旧的变成新的

/*检查坦克位置合法性*/
int check_wz(int xx, int yy, int wz) {
	//检查是否出界
	if (xx<0 || yy<0 || xx + 4 * SIZE>MAXX || yy + 4 * SIZE>MAXY||wz&&tool.flag[yy/SIZE][xx/SIZE]) {
		return 0;
	}
	//检查坦克所在位置是否有障碍物以及道具
	int tool_x = -1, tool_y = -1;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			//泥土和铁块不能跨过，其他坦克不能穿越道具,不能踩到传送门
			if (mat[yy / SIZE + i][xx / SIZE + j] == 1 || mat[yy / SIZE + i][xx / SIZE + j] == 2
			|| flag[yy / SIZE + i][xx / SIZE + j] >= 0 && flag[yy / SIZE + i][xx / SIZE + j] != wz || door.flag[yy / SIZE + i][xx / SIZE + j])
			{
				//已经有障碍物或者有其他坦克
				return 0;
			}
			if (tool.flag[yy / SIZE + i][xx / SIZE + j]) {
				if (wz)return 0;
				else {
					tool_x = xx / SIZE + j;
					tool_y = yy / SIZE + i;
				}
			}
		}
	}
	//检查无障碍物并且有道具
	int index = tool.flag[tool_y][tool_x];
	if (tool_x != -1) {
		//清空标记
		if (tool_x > 0 && tool.flag[tool_y][tool_x - 1])
			tool_x--;
		if (tool_y > 0 && tool.flag[tool_y - 1][tool_x])
			tool_y--;
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				tool.flag[tool_y + i][tool_x + j] = 0;
			}
		}
		clearrectangle(tool_x*SIZE, tool_y*SIZE, tool_x*SIZE + 2 * SIZE, tool_y*SIZE + 2 * SIZE);
		draw_a_tank(tank[0], tank[0].x, tank[0].y);//重画坦克
		return index;
	}
	return 10;
}//检查坦克位置合法性

/*清除坦克*/
int clear_tank(int x,int y,int type_of) {
	//test();
	int xx = x / SIZE;
	int yy = y / SIZE;//记录消除单位坐标
	int bh = flag[yy][xx];//坦克在坦克队列中的编号，0是自己
	if (type_of) {//表示是炸弹的魔法伤害
		tank[bh].HP = 0;
	}
	else {//否则就是普通子弹
		tank[bh].HP--;
	}
	if (tank[bh].HP) {
		if (bh) {
			up_date_type(7, bh);//两条命的敌人
		}
		else {
			up_date_type(level-1, bh);//自身
		}
		return 0;//表示还没挂掉,直接返回
	}
	//向左上角追溯
	while (flag[yy][xx] == bh) {
		xx--;
	}
	xx++;//为了接下来还可以查找y,因为找到的位置x实际上是没有flag标记的
	while (flag[yy][xx] == bh) {
		yy--;
	}
	yy++;//统一，这样左上角坐标就是[y-1][x-1]
	//清除坦克
	clearrectangle((xx-1)*SIZE, (yy-1)*SIZE, (xx-1)*SIZE + 4 * SIZE - 1, (yy-1)*SIZE + 4 * SIZE - 1);
	putimage((xx - 1)*SIZE + 2, (yy - 1)*SIZE + 2, &img[25]);
	Sleep(100);
	clearrectangle((xx - 1)*SIZE, (yy - 1)*SIZE, (xx - 1)*SIZE + 4 * SIZE - 1, (yy - 1)*SIZE + 4 * SIZE - 1);
	if (CheckGrass(tank[bh].x, tank[bh].y)) {//如果草丛
		add_grass(tank[bh].x, tank[bh].y);
	}
	//清空标记点
	for (int i = yy ; i <yy+ 3; i++) {
		for (int j = xx ; j < xx + 3; j++) {
			flag[i][j] = -1;
		}
	}
	if (bh == 0) {//代表自己被打死了
		level = 1;
		tank[0]=add_tank(0, 0);
		draw_a_tank(tank[0], tank[0].x, tank[0].y);
		my_lives--;
	}
	else {//其他坦克
		rest_num--;
		if (!rest_num)//坦克清空
		{
			return 1;//表示游戏结束(打完了)
		}
		else {
			if (rest_num < 3) {
				tank[bh].state = 0;//当前位置没有坦克
			}
			else {
				//产生一个新坦克
				srand(rand_seed);//重置时间种子
				rand_seed = rand();//随机种子链
				int rand_place;
				while (1) {
					srand(rand_seed);//重置时间种子
					rand_seed = rand();//随机种子链
					rand_place = rand() % 4 + 1;
					if (check_wz(birth_wz[rand_place][0], birth_wz[rand_place][1], bh))
						break;
				}
				tank[bh] = add_tank(rand_place, bh);
			}
		}
	}
	PrintTankNum();
	return 0;//游戏继续
}//清除坦克

/*消除该区域内的物体*/
void remove_any(int detx1,int dety1, int detx2, int dety2,Zidan d)
{
	int x = d.x;
	int y = d.y;
	//计算起始xy坐标
	int stx = d.x / SIZE;
	int sty = d.y / SIZE;
	//控制区间
	if (dety1 == dety2) //横向排列
	{
		//先判断有空的情况
		int index = 2;
		for (int i = -1; i <= 1; i++)//找到跳变点并退出
		{
			if (mat[sty + dety1][stx + i] != mat[sty + dety1][stx + i - 1])
			{
				//要保证不是跟空比较
				index = i;
				break;
			}
		}
		if (!(mat[sty + dety1][stx + index] * mat[sty + dety1][stx + index - 1])) {
			//有空
			if (mat[sty + dety1][stx + index] + mat[sty + dety1][stx + index - 1] == 3)//一边是草
			{
				return;
			}
			index++;
			while (mat[sty + dety1][stx + index] == mat[sty + dety1][stx + index - 1]&&index<2)
			{
				index++;
			}
		}
		if (index!=2) {//表明有变化,那么可能是铁+草，铁+泥土，草+泥土任意组合
			//逐个讨论
			if (mat[sty + dety1][stx + index] == 3 || mat[sty + dety1][stx + index - 1] == 3
				|| door.flag[sty + dety1][stx + index - 1]+ door.flag[sty + dety1][stx + index])//修正消除位置，草不能被打掉
			{
				if (mat[sty + dety1][stx + index-1] == 3||door.flag[sty + dety1][stx + index - 1])//代表左边是草或者传送门
				{
					detx1 = index;//修改左边界
				}
				else 
				{
					detx2 = index-1;//修改右边界
				}
			}
			else //一般坦克打的，不能消除铁，要判掉
			{
				if (mat[sty + dety1][stx + index-1] == 1)//代表左边是铁
				{
					detx1 = index;//修改左边界
				}
				else
				{
					detx2 = index - 1;//修改右边界
				}
			}
		}
		else if(mat[sty + dety1][stx + detx1] == 3){
			return;//全是草不考虑
		}
	}
	else //纵向排列 
	{
		int index = 2;
		for (int i = -1; i <= 1; i++)//找到跳变点并退出
		{
			if (mat[sty + i][stx + detx1] != mat[sty + i-1][stx + detx1])
			{
				index = i;
				break;
			}
		}
		if (!(mat[sty + index - 1][stx + detx1] * mat[sty + index][stx + detx1])) {
			//有空
			if (mat[sty + index][stx + detx1] + mat[sty + index - 1][stx + detx1] == 3) {//空+草
				return;
			}
			index++;
			while (mat[sty + index][stx + detx1] == mat[sty + index - 1][stx + detx1] && index < 2)
			{
				index++;
			}
		}
		if (index!=2) {//表明有变化,那么可能是铁+草，铁+泥土，草+泥土任意组合
			//逐个讨论
			//带草 3
			if (mat[sty + index -1][stx + detx1] == 3 || mat[sty + index][stx + detx1] == 3
				||door.flag[sty + index - 1][stx + detx1]+door.flag[sty + index][stx + detx1] )//修正消除位置，草不能被打掉
			{
				if (mat[sty + index-1][stx + detx1] == 3|| door.flag[sty + index - 1][stx + detx1])//代表上是草
				{
					dety1 = index;//修改上边界
				}
				else
				{
					dety2 = index - 1;//修改下边界
				}
			}
			else //一般坦克打的，不能消除铁，要判掉
			{
				if (mat[sty + index -1][stx + detx1] == 1)//代表上边是铁
				{
					dety1 = index;//修改上边界
				}
				else
				{
					dety2 = index - 1;//修改下边界
				}
			}
		}
		else if (mat[sty + dety1][stx + detx1] == 3) {
			return;//全是草不考虑
		}
	}
	if (d.who != 3 && mat[y / SIZE + dety1][x / SIZE + detx1] == 1) {//修正之后指向铁，是铁+草的情况，上面未修正完全
		return;//无法消除
	}
	//计算消除矩阵坐标
	int x1 = x + detx1*SIZE;
	int y1 = y + dety1*SIZE;
	int x2 = x + detx2*SIZE+SIZE;
	int y2 = y + dety2*SIZE+SIZE;
	clearrectangle(x1, y1, x2-1, y2-1);
	for (int i = x / SIZE + detx1; i <= x / SIZE + detx2; i++) 
	{
		for (int j = y / SIZE + dety1; j <= y / SIZE + dety2; j++) {
			mat[j][i] = 0;//清除标记
		}
	}
}//消除该区域内的物体

/*画一个子弹*/
void draw_zidan(Zidan d, int xx, int yy) {
	IMAGE ans1,ans2;
	rotateimage(&ans1, &img[((d.who <= 3) + 10) * 2], -PI / 2 * d.dir);//顺时针旋转dir个90度,然后赋值给ans
	rotateimage(&ans2, &img[((d.who <= 3) + 10) * 2+1], -PI / 2 * d.dir);//顺时针旋转dir个90度,然后赋值给ans
	putimage(xx + img_zidan_detx1[d.dir] + 1, yy + img_zidan_dety1[d.dir] + 1,&ans1, SRCAND);
	putimage(xx + img_zidan_detx1[d.dir] + 1, yy + img_zidan_dety1[d.dir] + 1,&ans2 , SRCPAINT);
}//画一个子弹

/*检查修补子弹打掉的草*/
void check_add_zidan_grass(Zidan d) {
	//推算左上角坐标并转化为单位形式
	int left_up_x = (d.x + img_zidan_detx1[d.dir]) / SIZE;
	int left_up_y = (d.y + img_zidan_dety1[d.dir]) / SIZE;
	left_up_x -= left_up_x % 2;
	left_up_y -= left_up_y % 2;//转换为草方块的定位点位置
	if (!(d.dir % 2)) {//上下
		int if_ou = 1-(d.x) / SIZE % 2;//如果子弹对称轴位于偶数线，那么就需要补充两张草
		//先判断要不要补充
		if (mat[left_up_y][left_up_x] == 3)//草
		{
			putimage(left_up_x*	SIZE, left_up_y*SIZE,&img[17], SRCAND);
			putimage(left_up_x*	SIZE, left_up_y*SIZE,&img[18], SRCPAINT);
		}
		if (if_ou&&mat[left_up_y][left_up_x + 2]==3)//看右边
		{
			putimage((left_up_x + 2) * SIZE, left_up_y * SIZE, &img[17], SRCAND);
			putimage((left_up_x + 2) * SIZE, left_up_y * SIZE, &img[18], SRCPAINT);
		}
	}
	else {//左右
		int if_ou = 1 - (d.y) / SIZE % 2;//如果子弹对称轴位于偶数线，那么就需要补充两张草
		//先判断要不要补充
		if (mat[left_up_y][left_up_x] == 3)//草
		{
			putimage(left_up_x*	SIZE, left_up_y*SIZE, &img[17], SRCAND);
			putimage(left_up_x*	SIZE, left_up_y*SIZE, &img[18], SRCPAINT);
		}
		if (if_ou&&mat[left_up_y+2][left_up_x] == 3)//看右边
		{
			putimage(left_up_x  * SIZE, (left_up_y+2) * SIZE, &img[17], SRCAND);
			putimage(left_up_x  * SIZE, (left_up_y+2) * SIZE, &img[18], SRCPAINT);
		}
	}
}//检查修补子弹打掉的草

/*清除子弹*/
void clear_zidan(Zidan d) {
	int x1 = d.x + img_zidan_detx1[d.dir];
	int y1 = d.y + img_zidan_dety1[d.dir];
	int x2 = d.x + img_zidan_detx1[d.dir] + SIZE-1;
	int y2 = d.y + img_zidan_dety1[d.dir] + SIZE-1;
	clearrectangle(x1, y1, x2, y2);
	check_add_zidan_grass(d);//检查补充草方块
	//先算出左上角坐标然后推算右下角坐标
}//清除子弹

/*检验子弹合法性*/
int check_zidan() 
{
	Zidan d;
	for (int i = 1; i <= qz.size(); i++) //这么多个子弹要检查
	{
		d = qz.front();
		qz.pop();
		//注意xy实际上是弹头坐标，左上角坐标要加上偏移量（最上面）
		if (d.x == 0 || d.y == 0) {
			if (d.count) {
				clear_zidan(d);
			}
			else {
				d.count = 1;
			}//出界                                 
			continue;
		}
		int xx = d.x + SIZE * dirx[d.dir];
		int yy = d.y + SIZE * diry[d.dir];//确定下一步位置
		if (xx > MAXX || yy > MAXY) {
			if (d.count) {
				clear_zidan(d);
			}
			else {
				d.count = 1;
			}//出界
			continue;
		}
		int checkX1 = xx / SIZE + check_if_movex[0][d.dir];
		int checkY1 = yy / SIZE + check_if_movey[0][d.dir];//确定检查点单位位置1
		int checkX2 = xx / SIZE + check_if_movex[1][d.dir];
		int checkY2 = yy / SIZE + check_if_movey[1][d.dir];//确定检查点单位位置2
		if (flag[yy/SIZE][xx/SIZE] ==-1 &&(!mat[checkY1][checkX1]&& !mat[checkY2][checkX2]
			|| mat[checkY1][checkX1]==3 && mat[checkY2][checkX2]==3)) {//都为会都为草位置，不用管
			if (d.count) {
				clear_zidan(d);
			}
			else {
				d.count = 1;
			}
			if (!tool.flag[checkY1][checkX1] && !tool.flag[checkY2][checkX2]&& !door.flag[checkY1][checkX1] && !door.flag[checkY2][checkX2]) {//不能打在道具上
				draw_zidan(d, xx, yy);//画子弹
				//这里要画出旋转后的,这个里面的左上角坐标正确
				d.x = xx;
				d.y = yy;//更新坐标
				qz.push(d);
			}
		}
		else if((mat[checkY1][checkX1]>0 || mat[checkY2][checkX2]>0)
			  && !((mat[checkY1][checkX1] ==3  || mat[checkY2][checkX2] == 3)
			  &&(flag[checkY1][checkX1] != -1 || flag[checkY2][checkX2]== -1)))//后面是为了保证草和坦克同时存在可以判定到下面，而不是这一层
		{//没有打到坦克,打到方块
			int check_grass = 0;
			if (!((mat[checkY1][checkX1] == 1 || mat[checkY2][checkX2] == 1) && d.who != 3)) //左右
			{//有一个铁块并且等级不够就不消除,代表被挡住了
			    //消除方块
				switch (d.dir) {
				case 0:
					remove_any(-2, -1, 1, -1, d);//传递的参数是子弹信息和相对于弹头的位置(四个方块),子弹分界的左右两个
					break;
				case 1:
					remove_any(0, -2, 0, 1, d);
					break;
				case 2:
					remove_any(-2, 0, 1, 0, d);
					break;
				case 3:
					remove_any(-1, -2, -1, 1, d);
					break;
				}
			}
			if (d.count) {
				if (d.count) {
					clear_zidan(d);
				}
				else {
					d.count = 1;
				}
			}
			else {
				d.count = 1;
			}
		}
		else if (flag[yy / SIZE][xx / SIZE] == 10) {//打到家
			clearrectangle(7 * SIZE * 4, 12 * SIZE * 4, 7 * SIZE * 4 + 8 * SIZE, 12 * SIZE * 4 + 8 * SIZE);
			putimage(7 * SIZE * 4, 12 * SIZE * 4, &img[24]);//家炸了
			return 0;
		}
		else {//代表炸到了坦克
		//根据编号看谁挂掉了
			if (d.who>3&&flag[yy / SIZE][xx / SIZE] == 0|| d.who <= 3 && flag[yy / SIZE][xx / SIZE]!=10) 
			{
				//代表两股势力
				if (clear_tank(xx, yy, 0)) {
					//test();
					return 2;//代表游戏结束
				}
				if (d.count) {
					clear_zidan(d);
				}
				else {
					d.count = 1;
				}
			}
			else {
				if (d.count) {
					clear_zidan(d);
				}
				else {
					d.count = 1;
				}
			}
		}
	}
	return 1;
}//检验子弹合法性

/*根据坦克产生一个子弹*/
void add_zidan(Tank t) {
	int tmp_type = t.type;//可以取到子弹的伤害
	Zidan d;
	d.dir = t.dir;
	int xx, yy;
	switch (t.dir) {//根据方向和坦克的左上角坐标确定子弹占据空间
	case 0:
		xx = t.x + 2 * SIZE;
		yy = t.y;
		break;
	case 1:
		xx = t.x + 4 * SIZE;
		yy = t.y + 2 * SIZE;
		break;
	case 2:
		xx = t.x + 2 * SIZE;
		yy = t.y + 4 * SIZE;
		break;
	case 3:
		xx = t.x;
		yy = t.y + 2 * SIZE;
		break;
	}
	d.x = xx;
	d.y = yy;
	d.who = t.type;//123都是自己的形态，代表是自己,否则是对方的
	d.count = 0;
	qz.push(d);
}//根据坦克产生一个子弹

/*修复老家*/
void repair_map() {
	int num = sizeof(rep_map) / sizeof(rep_map[0]);
	for (int i = 0; i < num; i++) {
		int k = rep_map[i][0];
		int x1 = rep_map[i][1] * 4;
		int y1 = rep_map[i][2] * 4;
		int x2 = rep_map[i][3] * 4 - 1;
		int y2 = rep_map[i][4] * 4 - 1;//右下角方块的左上角坐标
		for (int xx = x1; xx <= x2; xx++) {
			for (int yy = y1; yy <= y2; yy++) {
				mat[yy][xx] = k;
			}
		}
		for (int xx = x1; xx <= x2; xx++) {
			for (int yy = y1; yy <= y2; yy++) {
				putimage(xx*SIZE, yy*SIZE, &img[16]);
			}
		}
	}
}//修复老家

/*检查道具*/
int check_tool(int index) {
	//炸弹停止升级修复
	int x=0, y=0;
	int max_num = 10000;//最多寻找次数
	switch (index) {
	case 1:
		while (max_num--) {
			srand(rand_seed);//重置时间种子
			rand_seed = rand();//随机种子链
			x = rand() % NUMX;
			y = rand() % NUMY;
			if (flag[y][x]>0 && flag[y][x] != 10)
				break;
		}
		if(max_num)//表示可以找到坦克
			if (clear_tank(x*SIZE, y*SIZE, 1)) {
				return 1;
			}
		break;
	case 2:
		stop_epoch = 300;//3秒
		break;
	case 3:
		if (level < 3) {
			up_date_type(level+1,0);//更换类型
			level++;
		}
		break;
	case 4:
		repair_map();
		break;
	}
	return 0;//游戏继续
}//检查道具

/*检查关于传送门*/
int CheckDoor(Tank t,int x,int y)
{
	//xy是下一步的单位坐标
	//检查是否进入传送门，只有正对传送门才能进入
	if (1 - door.type == t.dir % 2) {
		if (t.dir % 2) {//左右
			if (y != door.y2&&y != door.y1)
				return 0;//不能进入
			if (y==door.y1&&(t.dir == 1 && x+ 3 == door.x1|| t.dir == 3 && x == door.x1)) {
				return 1;//从第一个进入
			}
			else if(y==door.y2&&(t.dir == 1 && x + 3 == door.x2 || t.dir == 3 && x == door.x2)){
				return 2;//从第二个进入
			}
		}
		else {//上下
			if (x != door.x2&&x != door.x1)
				return 0;
			if (x==door.x1&&(t.dir == 2 && y + 3 == door.y1 || t.dir == 0 && y == door.y1)) {
				return 1;//从第一个进入
			}
			else if (x==door.x2&&(t.dir == 2 && y + 3 == door.y2 || t.dir == 0 && y == door.y2)) {
				return 2;//从第二个进入
			}
		}
	}
	return 0;
}//检查关于传送门

/*四个方向*/
void control_dir(int now_dir) {
	if (tank[0].dir != now_dir) //原来不是在这个
	{
		tank[0].dir = now_dir;
		draw_a_tank(tank[0], tank[0].x, tank[0].y);
		tank[0].bef_move = now_dir;//更新
	}
	else if (tank[0].bef_move >= type[tank[0].type].Speed)//可以移动
	{
		int xx = tank[0].x + dirx[tank[0].dir] * SIZE;
		int yy = tank[0].y + diry[tank[0].dir] * SIZE;//坐标
		int check_door = CheckDoor(tank[0],xx/SIZE,yy/SIZE);//检查关于传送门,根据返回值来看位置
		//更新位置
		if (check_door) {
			if (check_door == 1) {
				xx = door.arrx2*SIZE;
				yy = door.arry2*SIZE;
			}
			else {
				xx = door.arrx1*SIZE;
				yy = door.arry1*SIZE;
			}
		}
		int check = check_wz(xx, yy, 0);
		if (check_door||check) { //检查位置
			draw_a_tank(tank[0], xx, yy);//xx,yy表示更新后的位置
			if (check_door) {
				Sleep(200);
				mark_tank(tank[0].x, tank[0].y, -1);
				mark_tank(xx, yy, 0);
			}
			else {
				update_mark(tank[0].dir, xx, yy, 0);//普通移动更新坦克位置信息
			}
			tank[0].x = xx;
			tank[0].y = yy;
			tank[0].bef_move = 0;//更新时间
			if (check != 10) {
				check_tool(check);//检查道具
				tool.exist = 0;
			}
		}
	}
}//四个方向

/*控制其他坦克*/
void check_tank()
{
	for (int i = 1; i <= num_tank; i++) {//依次取出坦克
		if (!tank[i].state)continue;//已经没有了
		if (tank[i].bef_move >= type[tank[i].type].Speed) {
			//可能停留在原地的
			if (tank[i].stop_time) {
				tank[i].stop_time--;
				if (tank[i].bef_fight >= type[tank[i].type].Fight_det) {
					srand(rand_seed);//重置时间种子
					rand_seed = rand();//随机种子链
					int state = rand() % 100;
					if (state <= 50) {
						tank[i].bef_fight = 0;
						add_zidan(tank[i]);//添加一个子弹
					}
					else {
						tank[i].bef_fight = 0;
					}
				}
			}
			//表示要移动的
			else if (tank[i].dir_stop_time) {
				int xx = tank[i].x + dirx[tank[i].dir] * SIZE;
				int yy = tank[i].y + diry[tank[i].dir] * SIZE;//坐标
				int check_door = CheckDoor(tank[i], xx / SIZE, yy / SIZE);//检查关于传送门,根据返回值来看位置
				int dir;
				if (check_door) {
					if (check_door == 1) {
						xx = door.arrx2*SIZE;
						yy = door.arry2*SIZE;
						dir = door.dir2;
					}
					else {
						xx = door.arrx1*SIZE;
						yy = door.arry1*SIZE;
						dir = door.dir1;
					}
					tank[i].dir = dir;
					tank[i].dir_stop_time = rand() % 5+2;
				}
				if (check_door||check_wz(xx, yy, i))
				{
					draw_a_tank(tank[i], xx, yy);//xx,yy表示更新后的位置
					if (check_door) {
						Sleep(200);
						mark_tank(tank[i].x, tank[i].y, -1);
						mark_tank(xx, yy, i);
					}
					else {
						update_mark(tank[i].dir, xx, yy, i);//普通更新坦克位置信息
					}
					tank[i].x = xx;
					tank[i].y = yy;
					tank[i].bef_move = 0;//更新时间
					tank[i].dir_stop_time--;
				}
				else 
				{
					//先打一发
					if (tank[i].bef_fight >= type[tank[i].type].Fight_det) {
						tank[i].bef_fight = 0;
						add_zidan(tank[i]);//添加一个子弹
					}
					srand(rand_seed);//重置时间种子
					rand_seed = rand();//随机种子链
					int state = rand() % 100;
					tank[i].dir_stop_time = state % 10+1;//方向锁定次数
					if (state % 3==1) {
						tank[i].dir = (tank[i].dir + 1) % 4;
						if(state%2)
							tank[i].stop_time = state*2;//可能几个回合之内停在原地
					}
					else if(state % 3 == 2){
						tank[i].dir = (tank[i].dir - 1 + 4) % 4;
						if(state%2)
							tank[i].stop_time = state*2;//可能几个回合之内停在原地
					}
					else {
						tank[i].dir = (tank[i].dir + 2) % 4;//反向
					}
					draw_a_tank(tank[i], tank[i].x, tank[i].y);
				}
			}
			else {
				//换方向默认优先向着中间目标
				srand(rand_seed);//重置时间种子
				rand_seed = rand();//随机种子链
				int state = rand() % 100;
				//根据移动类型优化路径
				if (tank[i].move_type == 1) {
					if (abs(tank[i].x - MAXX / 2) * 2 / MAXX * 100 > state) {//距离中间远
						if (tank[i].x - MAXX / 2 >= 0) {
							tank[i].dir = 3;//左边
						}
						else {
							tank[i].dir = 1;//右边
						}
					}
					else if (abs(tank[i].y - MAXY) / MAXY * 100 > state) {//距离下面远
						tank[i].dir = 2;//向下走
					}
					else {
						tank[i].dir = state % 4;//随机换向
					}
				}
				else {
					if (abs(tank[i].y - MAXY) / MAXY * 100 > state) {//距离下面远
						tank[i].dir = 2;//向下走
					}
					else if (abs(tank[i].x - MAXX / 2) * 2 / MAXX * 100 > state) {//距离中间远
						if (tank[i].x - MAXX / 2 >= 0) {
							tank[i].dir = 3;//左边
						}
						else {
							tank[i].dir = 1;//右边
						}
					}
					else {
						tank[i].dir = state % 4;//随机换向
					}
				}
				tank[i].dir_stop_time = state % 10+1;//方向锁定次数
				draw_a_tank(tank[i], tank[i].x, tank[i].y);
			}
		}
		//控制射击
		if (tank[i].bef_fight >= type[tank[i].type].Fight_det) {
			srand(rand_seed);//重置时间种子
			rand_seed = rand();//随机种子链
			int state = rand() % 100;
			if (state <= 50) {
				tank[i].bef_fight = 0;
				add_zidan(tank[i]);//添加一个子弹
			}
			else {
				tank[i].bef_fight = 0;
			}
		}
		tank[i].bef_fight = (tank[i].bef_fight + 1) % 10000;//距离上次发射时间
		tank[i].bef_move = (tank[i].bef_move + 1) % 10000;//距离上次移动时间
	}
}//控制其他坦克

/*添加道具*/
void add_tool() 
{
	int x, y;
	bool check = 0;
	while (!check) {
		srand(rand_seed);//重置时间种子
		rand_seed = rand();//随机种子链
		x = rand() % NUMX - 2;
		y = rand() % NUMY - 2;//左上角单位坐标
		check = 1;
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				if (mat[y + i][x + j]||flag[y+i][x+j]!=-1)
					check=0;//不能落在有东西的上面
			}
		}
	}
	int index = rand() % 4 + 1;//道具种类
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			tool.flag[y + i][x + j] = index;
		}
	}
	putimage(x*SIZE+2, y*SIZE+2, &img[25+index]);
	tool.exist = 1;
}//添加道具

/*判断一个区域能否存在坦克*/
int CheckIfCanExist(int x1,int y1,int x2,int y2){
	//传参是左上角和右下角的左上角坐标
	for (int i = y1; i <= y2; i++) {
		for (int j = x1; j <= x2; j++) {
			if (mat[i][j] && mat[i][j] != 3 || flag[i][j] != -1)
				return 0;
		}
	}
	return 1;
}

/*更新传送门*/
void UpDataDoor() {
	if (door.x1 >= 0) {//清除原有的
		if (door.type) {//左右
			clearrectangle(door.x1*SIZE, door.y1*SIZE, door.x1*SIZE + 4 * SIZE - 1, door.y1*SIZE + SIZE - 1);
			clearrectangle(door.x2*SIZE, door.y2*SIZE, door.x2*SIZE + 4 * SIZE - 1, door.y2*SIZE + SIZE - 1);
		}
		else {//上下
			clearrectangle(door.x1*SIZE, door.y1*SIZE, door.x1*SIZE + SIZE - 1, door.y1*SIZE + 4 * SIZE - 1);
			clearrectangle(door.x2*SIZE, door.y2*SIZE, door.x2*SIZE + SIZE - 1, door.y2*SIZE + 4 * SIZE - 1);
		}
		for (int i = 0; i <= 4; i++) {
			if (door.type) {
				door.flag[door.y1][door.x1 + i] = 0;
				door.flag[door.y2][door.x2 + i] = 0;
			}
			else {
				door.flag[door.y1 + i][door.x1] = 0;
				door.flag[door.y2 + i][door.x2] = 0;
			}
		}
	}
	//生成传送门
	door.type = MyRand() % 2;
	//door.type = 1;
	int num = 0;//当前找到的个数
	int x, y;//随机生成的一组坐标
	while (num < 2) {
		srand(rand_seed);//重置时间种子
		rand_seed = rand();//随机种子链
		x = rand() % (NUMX - 10)+ 6;
		y = rand() % (NUMY - 8 )+ 6;//左上角单位坐标
		//必须是空地(没有任何东西)
		bool if_ok = 1;//表示是否可以
		for (int i = 0; i <= 4; i++) {//检查四个点
			if (door.type) {//检查一排
				if (mat[y][x + i] || flag[y][x + i] != -1 || flag[y + 1][x + i] != -1 || num && abs(x - door.x1) + abs(y - door.y1) <40) {
					if_ok = 0;
					break;
				}
			}
			else {//检查一列
				if (mat[y+i][x ] || flag[y+i][x]!=-1 || flag[y + i][x + 1]!=-1 || num && abs(x - door.x1) + abs(y - door.y1) < 40) {
					if_ok = 0;
					break;
				}
			}
		}
		//接下来查找坦克的合法传送位置
		if (door.type) {//左右类型传送门传上下
			//先看上面有没有位置
			if (CheckIfCanExist(x,y-4,x+3,y-1)) {//表明上面可以传送
				if (num) {
					door.arrx2 = x;
					door.arry2 = y - 4;
					door.dir2 = 0;
				}
				else{
					door.arrx1 = x;
					door.arry1 = y - 4;
					door.dir1 = 0;
				}
			}
			else {//否则找下面
				if (CheckIfCanExist(x,y+1,x+3,y+4)) {
					if (num) {
						door.arrx2 = x;
						door.arry2 = y + 1;
						door.dir2 = 2;
					}
					else {
						door.arrx1 = x;
						door.arry1 = y + 1;
						door.dir1 = 2;
					}
				}
				else {
					if_ok = 0;//两边都找过还不可以
				}
			}
		}
		else {
			//先看左边有没有位置
			if (CheckIfCanExist(x-4, y, x-1, y+3)) {//表明左边可以传送
				if (num) {
					door.arrx2 = x - 4;
					door.arry2 = y;
					door.dir2 = 3;
				}
				else {
					door.arrx1 = x - 4;
					door.arry1 = y;
					door.dir1 = 3;
				}
			}
			else {//否则找右边
				if (CheckIfCanExist(x+1, y , x + 4, y +3)) {
					if (num) {
						door.arrx2 = x + 1;
						door.arry2 = y;
						door.dir2 = 1;
					}
					else {
						door.arrx1 = x + 1;
						door.arry1 = y;
						door.dir1 = 1;
					}
				}
				else {
					if_ok = 0;//两边都找过还不可以
				}
			}
		}
		if (!if_ok) {
			continue;//不行
		}
		//更新标记点
		for (int i = 0; i < 4; i++) {
			if (door.type) {
				door.flag[y][x + i] = 1;
			}
			else {
				door.flag[y + i][x] = 1;
			}
		}
		num++;
		putimage(x*SIZE + 1, y*SIZE + 1, &img[30+door.type]);
		if (num == 1) {
			door.x1 = x;
			door.y1 = y;
		}
		else {
			door.x2 = x;
			door.y2 = y;
		}
	}
}//更新传送门

/*开始游戏*/
void begin() {
	int num_t = 0;
	int num_door = 0;
	while (1) {
		if (KEY_DOWN(VK_UP)) {
			control_dir(0);
		}
		if (KEY_DOWN(VK_RIGHT)) {
			control_dir(1);
		}
		if (KEY_DOWN(VK_DOWN)) {
			control_dir(2);
		}
		if (KEY_DOWN(VK_LEFT)) {
			control_dir(3);
		}
		if (KEY_DOWN(VK_SPACE)) {
			if (tank[0].bef_fight >= type[tank[0].type].Fight_det) {//至少要过几个时间段才可以发射子弹
				tank[0].bef_fight = 0;
				add_zidan(tank[0]);//添加一个子弹
			}
		}
		if (KEY_DOWN(VK_ESCAPE)) {
			stop();
		}
		if (stop_epoch) {
			stop_epoch--;
		}
		else {
			check_tank();//其他坦克
		}
		Sleep(10);//更新频率
		int now_state = check_zidan();
		if (now_state == 2) //成功通关
		{
			PrintSucess();
			return;
		}
		else if (!now_state || my_lives == 0)//清除无效子弹，检查是否有坦克炸毁
		{
			PrintGameOver();
			return;
		}
		tank[0].bef_fight = (tank[0].bef_fight + 1) % 10000;//距离上次发射时间
		tank[0].bef_move = (tank[0].bef_move + 1) % 10000;//距离上次移动时间
		tool.num++;
		if (tool.num == tool_speed) {//要考虑刷新道具了
			tool.num = 0;
			if (!tool.exist) {
				add_tool();
				tool.exist = 1;
			}
		}
		door.num--;
		if (door.num == 0) {//更新传送门
			door.num = door_speed;//更新周期
			UpDataDoor();//更新传送门
		}
	}
}//开始游戏

void game() {
	//printf("%d", (*all_map)[1][1]);
	initgraph(MAXX + 1 + 400, MAXY + 1, SHOWCONSOLE);
	ini_all();//全局初始化函数,一次游戏只初始化一次(区别于关卡初始化)

	for (int i = 0; i <= 2;i++)//关卡数字循环,当前是i关
	{
		now_level = i;
		printNextLevel();//打印转场动画
		while (1) {
			Sleep(10);
			settextcolor(WHITE);
			settextstyle(400, 40, 0);
			outtextxy(300, 300, "点击ENTER开始游戏...");
			if (KEY_DOWN(13)) {//点击enter进入
				cleardevice();
				int num = get_map(map,all_map[now_level]);//将当前的关卡地图从all_map中写到map中.并且返回数量
				ini_every(map, num);//关卡初始化
				ini_tank();//初始化出坦克
				draw_all_tank();//画坦克
				begin();//游戏开始,主控程序
				while (1) {
					Sleep(10);
					if (KEY_DOWN(VK_ESCAPE)) {//读取玩家开始信息
						cleardevice();
						break;
					}
				}
				break;
			}
		}
		if (!my_lives) {
			break;
		}
	}
}