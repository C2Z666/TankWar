#include"head.h"

int dirx[] = {0,1,0,-1};
int diry[] = {-1,0,1,0};//�ĸ�����
int img_zidan_detx1[] = {-SIZE/2,-SIZE,-SIZE/2,0};
int img_zidan_dety1[] = {0,-SIZE/2,-SIZE,-SIZE/2};//�ӵ����Ͻ���Ե�ͷƫ����,���ӵ�Ҫ��
int check_if_movex[][4] = { {-1,-1,-1,0},{0,-1,0,0} };
int check_if_movey[][4] = { {0,-1,-1,-1 },{0,0,-1,0} };//��ʾ�ӵ��Ϸ����λ����Ե�ͷƫ�Ƶ�λ�����������������
int all_tank_num = 3;//��������̹������
int rest_num;//ʣ��̹������
int my_lives;//�Լ���̹������
int now_level;//��ǰ�ؿ�
int rand_seed;//�������
int level;//����ǰ�ȼ�
int mat[NUMY+2][NUMX+2];//�洢��Ϸ��ͼ
int flag[NUMY + 2][NUMX + 2];//�洢̹�˱��
int stop_epoch;//��ʾֹͣ�غ�,����ʹ��
int tool_speed;//���߸�������
int door_speed;//�����Ÿ�������
//123�ֱ�洢�����ϰ��(-10)�洢�Լ�̹������λ��,(-1)-(-9)�洢�з�̹��λ�ã�һ��̹��ռ��16����Ԫ������������
int num_tank;//��ǰ���ϵ�̹��
int birth_wz[][2] = { {MAXX / 2 - SIZE * 12,MAXY - 4 * SIZE},			//��һ�����Լ��ĳ����㣬ʣ�µ�������̹�˳�����
					{0,0},{MAXX / 2 - 16 * SIZE,0},{MAXX / 2 + 16 * SIZE,0},{MAXX - 4 * SIZE,0} }; 
int rep_map[][5] = { {2,6,12,7,14},{2,6,11,10,12},{2,9,12,10,14} };//�޸��ϼҵ�Ԫ��
// ���ùؿ�
// 1��ʾ���飬2��ʾ��ש��3��ʾ�ݵأ�������x1 y1 x2 y2 ������С��ԪΪ2*SIZE�ĵ�ͼ�ϵ�ÿ�����ͷ�������ϽǺ����½�����
int map[100][5];//�洢ÿһ�صĵ�ͼ
int all_map[3][70][5] =  //���е�ͼ
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
int all_other_tank[] = {20,25,30};//�洢ÿһ�صĵз�̹������
int tool_update_speed[] = { 1000,1000,400 };//���߸�������
int door_update_speed[] = { 1500,1500,500 };//�����Ÿ�������

struct Zidan {
	int who;//��¼������̹��ϵ�У�1-3���Լ���������������
	int dir;//����
	int x;
	int y;//����
	bool count;//Ϊ�˱���Ƿ��ǵ�һ�α�����
};
queue<Zidan>qz;

struct Tool {
	int num;//���¼�ʱ��
	bool exist;//�Ƿ����
	int flag[NUMY + 2][NUMX + 2];//�洢������Ϣ
}tool;

struct Door {
	int num;
	int x1, y1, x2, y2;//���������ŵ�����
	int arrx1, arrx2, arry1, arry2;//���������Ŷ�Ӧ�ĵ�������
	int dir1, dir2;//�������ŵķ��򣬿ɶ���û�õ�?
	int type;//���������ͣ����� 0/���� 1
	int flag[NUMY + 2][NUMX + 2];//�洢��Ϣ
}door;//���ڴ����ŵĽṹ��

struct Tank {
	int type;//����
	int HP;//Ѫ��
	int dir;//����,��������0123
	int x, y;//����
	int bef_fight;//��һ�ι��������ڵļ��
	int bef_move;//��һ���ƶ������ڵļ��
	int stop_time;//��ʾ��̹���ڼ��������ڲ��ƶ�
	int dir_stop_time;//��ʾ��̹���ڼ��������ڲ��ı䷽��
	int move_type;//1�����������м䣬2��������������
	int state;//��ǰ״̬
}tank[10];//�����ʵ��4��,0��ʼ�����Լ�

// ��̹ͬ����������
struct Type {
	/*��������*/
	int HP;//Ѫ��
	int Speed;//�ٶ�,��λ�Ǹ�������,һ�����������ӵ��ƶ�һ��
	int Power;//�˺�
	int Fight_det;//�������,��λ�Ǹ�������
}type[10];
IMAGE img[40];//�洢ͼƬ

/*����*/
void test() {
	clearrectangle(0, 0, 500, 500);
}//����

/*����ֹͣ*/
void stop() {
	system("pause");
}//����ֹͣ

/*���������*/
int MyRand() {
	srand(rand_seed);
	int t = rand();
	rand_seed = rand();//����
	return t;
}//���������

/*�����д���ͼ*/
int get_map(int map[][5],int all_map[][5])
{
	//��������
	int num = 0;
	for (int i = 0; i < 70; i++) {
		if (all_map[i][0] == 0) {//���������һ��
			return num;
		}
		else {//���ǿ�
			for (int j = 0; j < 5; j++) {
				map[i][j] = all_map[i][j];
			}
			num++;
		}
	}
}//�����д���ͼ

/*ȫ�ֳ�ʼ��*/
void ini_all() {
	/*��������״̬*/
	//һ��
	type[1].HP = 1;
	type[1].Speed = 8;
	type[1].Power = 10;
	type[1].Fight_det = 30;
	//����
	type[2].HP = 1;
	type[2].Speed = 7;
	type[2].Power = 15;
	type[2].Fight_det = 25;
	//����
	type[3].HP = 2;
	type[3].Speed = 6;
	type[3].Power = 20;
	type[3].Fight_det = 20;
	/*����̹��״̬*/
	//��һ��Ѫ���ߵ�
	type[4].HP = 2;
	type[4].Speed = 10;
	type[4].Power = 10;
	type[4].Fight_det = 40;
	//�ڶ����ٶȿ��
	type[5].HP = 1;
	type[5].Speed = 5;
	type[5].Power = 10;
	type[5].Fight_det = 30;
	//�����ֹ�����
	type[6].HP = 1;
	type[6].Speed = 8;
	type[6].Power = 10;
	type[6].Fight_det = 20;
	//�������ǵ�һ�ֱ�����֮�������
	type[7].HP = 1;
	type[7].Speed = 10;
	type[7].Power = 10;
	type[7].Fight_det = 40;
	/*ͼƬ*/
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
	loadimage(&img[15], "./imgs/����.png");//����
	loadimage(&img[16], "./imgs/��ש.png");//��ש
	loadimage(&img[17], "./imgs/�ݴ�_1.png");//�ݵ�
	loadimage(&img[18], "./imgs/�ݴ�_2.png");//�ݵ�
	loadimage(&img[19], "./imgs/��.png");//��
	loadimage(&img[20], "./imgs/�з��ӵ�_1.png");
	loadimage(&img[21], "./imgs/�з��ӵ�_2.png");
	loadimage(&img[22], "./imgs/�ҷ��ӵ�_1.png");
	loadimage(&img[23], "./imgs/�ҷ��ӵ�_2.png");//�ӵ�
	loadimage(&img[24], "./imgs/�ұ�ը.png");//�ұ�ը����
	loadimage(&img[25], "./imgs/̹�˱�ը.png");//̹�˱�ը����
	loadimage(&img[26], "./imgs/ը��.png");//���ը��һ��̹��
	loadimage(&img[27], "./imgs/ֹͣ.png");//����̹��ֹͣ������������
	loadimage(&img[28], "./imgs/����.png");//̹������
	loadimage(&img[29], "./imgs/�޸�.png");//�޸��ϼ�
	loadimage(&img[30], "./imgs/������_����.png");//����������1
	loadimage(&img[31], "./imgs/������_����.png");//����������2
}//ȫ�ֳ�ʼ��

/*�ؿ���ʼ��*/
void ini_every(int mp[][5], int num) {
	door.num = 1;//һ��ʼ��ˢ�´�����
	door.x1 = -1;
	door.x2 = -1;
	door.y1 = -1;
	door.y2 = -1;
	tool.num = 0;
	tool.exist = 0;
	tool_speed = tool_update_speed[now_level];
	door_speed = door_update_speed[now_level];
	rest_num = all_other_tank[now_level];//�з�̹������
	my_lives = 3;
	level = 1;//һ��
	putimage(MAXX + 100, 100, &img[2]);
	putimage(MAXX + 100, 200, &img[8]);//�����������̹��
	PrintTankNum();
	srand((unsigned)time(NULL));//��������
	rand_seed = rand();
	line(MAXX + 1, 0, MAXX + 1, MAXY + 1);//�ָ���
	putimage(7 * SIZE * 4, 12 * SIZE * 4, &img[19]);
	for (int i = 0; i <= NUMY; i++){
		for (int j = 0; j <= NUMX; j++) {
			mat[i][j] = 0;
			flag[i][j]= -1;//�ȳ�ʼ��Ϊ-1 
			tool.flag[i][j] = 0;
			door.flag[i][j] = 0;
		}
	}
	for (int i = 12 * 4; i <= 12 * 4 + 8; i++) {
		for (int j = 7 * 4; j <= 7 * 4 + 8; j++) {
			flag[i][j] = 10;//���Ϊ�ϼ�λ��
		}
	}
	for (int i = 0; i < num; i++) {
		int k = mp[i][0];
		int x1 = mp[i][1]*4;
		int y1 = mp[i][2]*4;
		int x2 = mp[i][3]*4-1;
		int y2 = mp[i][4]*4-1;//���½Ƿ�������Ͻ�����
		for (int xx = x1; xx <= x2; xx++) {
			for (int yy = y1; yy <= y2; yy++) {
				mat[yy][xx] = k;
			}
		}
		if (k == 2) {//��������С��λSIZE
			for (int xx = x1; xx <= x2; xx++) {
				for (int yy = y1; yy <= y2; yy++) {
					putimage(xx*SIZE, yy*SIZE, &img[16]);
				}
			}
		}
		else {//����Ͳݷ�����С��λ2*SIZE
			for (int xx = mp[i][1]*2; xx <= mp[i][3]*2-1; xx++) {
				for (int yy = mp[i][2]*2; yy <= mp[i][4]*2-1; yy++) {
					if (k == 1) {//����
						putimage(xx*SIZE * 2, yy*SIZE * 2, &img[15]);
					}
					else {//�ݵ�
						putimage(xx*SIZE * 2, yy*SIZE * 2, &img[17], SRCAND);
						putimage(xx*SIZE * 2, yy*SIZE * 2, &img[18], SRCPAINT);
					}
				}
			}
		}
	}
}//�ؿ���ʼ��

/*ת������Ϊ�ַ���*/
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
}//ת������Ϊ�ַ���

/*��ӡ��Ϸ����*/
void PrintGameOver() {
	settextcolor(WHITE);
	int size = 100;//�����С
	settextstyle(size, size, 0);
	outtextxy(90,200,"��Ϸ����");
}//��ӡ��Ϸ����

/*��ӡ�ؿ����ɶ���*/
void printNextLevel() {
	cleardevice();
	char a[100];
	char info[100] = "��";//ƴ������
	//��ӡʱ��ľ�
	settextcolor(BROWN);
	int size =100;//�����С
	settextstyle(size, size, 0);
	deal(now_level+1, a);
	strcat(info, a);
	strcat(info, "��");
	outtextxy(MAXX / 2 + 200 - size * 2, MAXY / 2 - size, info);
	Sleep(1000);
	cleardevice();
}//��ӡ�ؿ����ɶ���

/*ͨ��*/
void PrintSucess() {
	settextcolor(WHITE);
	int size = 100;//�����С
	settextstyle(size, size, 0);
	outtextxy(90, 200, "�ɹ�ͨ��");
}//ͨ��

/*��ӡ̹������*/
void PrintTankNum() {
	char a[100];

	//�����Լ�̹������
	settextcolor(WHITE);
	settextstyle(40, 40, 0);
	outtextxy(MAXX+100+4*SIZE, 120, "X");
	deal(my_lives, a);
	//����Ϊ�˷�ֹ����ʱ��ʾ����
	clearrectangle(MAXX + 100 + 4 * SIZE + 50, 120, MAXX + 100 + 4 * SIZE + 50 + 70, 120 + 50);
	outtextxy(MAXX + 100 + 4*SIZE+50, 120, a);

	//����з�̹������
	settextcolor(WHITE);
	settextstyle(40, 40, 0);
	outtextxy(MAXX + 100 + 4 * SIZE, 220, "X");
	deal(rest_num, a);
	clearrectangle(MAXX + 100 + 4 * SIZE + 50, 220, MAXX + 100 + 4 * SIZE + 50 + 70, 220 + 50);
	outtextxy(MAXX + 100 + 4 * SIZE + 50, 220, a);
}//��ӡ̹������

/*���̹��ռ�ݿռ�*/
void mark_tank(int x,int y,int mark_num) {
	for (int i = 1; i <= 3; i++) {
		for (int j = 1; j <= 3; j++) {
			flag[y / SIZE + i][x / SIZE + j] = mark_num;
		}
	}
}//���̹��ռ�ݿռ�

/*����̹�˱�ǵ�*/
void update_mark(int dir, int x, int y,int index) {//�µķ�����µ����궨λ��
	switch (dir) {//���ַ�����������
		//̹�˱�ǵ���5��
	case 0:
		for (int i = 1; i < 4; i++) {
			flag[y / SIZE + 1][x / SIZE + i] = index;
			flag[y / SIZE + 4][x / SIZE + i] = -1;//��պ����
		}
		break;
	case 1:
		for (int i = 1; i < 4; i++) {
			flag[y / SIZE + i][x / SIZE + 3] = index;
			flag[y / SIZE + i][x / SIZE ] = -1;//��պ����
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
			flag[y / SIZE + i][x / SIZE + 4] = -1;//��պ����
		}
		break;
	}
}//����̹�˱�ǵ�

/*����һ��̹��*/
Tank add_tank(int add_place,int wz) {
	//add_place��ʾ����λ�ã�ȡֵ1234��ʾ�з�̹�˵ĳ���λ�ã�wz��ʾ��̹�������е�λ��
	Tank t;
	t.dir = 3;
	t.bef_fight = 0;
	t.bef_move = 0;
	t.stop_time = rand()%5;
	t.dir_stop_time = 0;
	if (add_place) {//�з�̹��
		srand(rand_seed);//����ʱ������
		rand_seed = rand();//���������
		int index = rand() % all_tank_num + 4;
		t.move_type = rand() % 2 + 1;
		t.type = index;
		t.HP = type[index].HP;
		t.x = birth_wz[add_place][0];
		t.y = birth_wz[add_place][1];
		t.state = 1;//��ʾ����
		mark_tank(t.x, t.y, wz);
	}
	else {//�Լ�
		t.HP = type[level].HP;
		t.type = level;//ͨ��type�������Եõ��ٶȵ���Ϣ
		t.x = birth_wz[0][0];
		t.y = birth_wz[0][1];
		mark_tank(t.x, t.y, 0);//0�ű�����Լ�
	}
	return t;
}//����һ��̹��

/*��ʼ��̹�˶���*/
void ini_tank() {
	num_tank = 0;
	tank[0]=add_tank(0,0);//0��ʾ��������̹�ˣ�������ʾ����̹�ˣ����index
	//��1 2 3�ų��������õз�̹��
	for (int i = 1; i <= 3; i++) {
		num_tank++;
		tank[i]=add_tank(i, num_tank);
	}
}//��ʼ��̹�˶���

/*����Ƿ񴩹��ݴ�*/
bool CheckGrass(int xx,int yy) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (mat[yy / SIZE + i][xx / SIZE + j] == 3) {
				return 1;//�д����ݵ�
			}
		}
	}
	return 0;
}//����Ƿ񴩹��ݴ�

/*�޸��ݵ�---̹�˾����ݵ�*/
void add_grass(int xx, int yy) {
	bool tmp_map[4][4];
	int detx = 0, dety = 0;//���Ͻ�
	int last_x, last_y;//���½�
	bool if_first = 0;//����Ƿ��ǵ�һ�γ��֣���Ϊ�˱�����Ͻ�����
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			tmp_map[i][j] = (mat[yy / SIZE + i][xx / SIZE + j] == 3);//��̹�˲��Ҿ���
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
			if (i < 0&&tmp_map[0][j] || j < 0&&tmp_map[i][0] || tmp_map[i][j]) {//��Ҫ����ͼƬ
				putimage(xx + j * SIZE, yy + i * SIZE, &img[17], SRCAND);
				putimage(xx + j * SIZE, yy + i * SIZE, &img[18], SRCPAINT);
			}
		}
	}
}//�޸��ݵ�---̹�˾����ݵ�

/*��һ��̹��*/
void draw_a_tank(Tank t,int xx,int yy) {
	IMAGE ans1,ans2;
	clearrectangle(t.x+3, t.y+3, t.x + 4 * SIZE-1, t.y + 4 * SIZE-1);
	rotateimage(&ans1, &img[t.type*2-1], -PI / 2 * t.dir);//˳ʱ����תdir��90��,Ȼ��ֵ��ans
	rotateimage(&ans2, &img[t.type*2], -PI / 2 * t.dir);
	putimage(xx + 4, yy + 4, &ans1, SRCAND);
	putimage(xx + 4, yy + 4, &ans2, SRCPAINT);
	if (CheckGrass(t.x,t.y)) {//����ݴ�
		add_grass(t.x, t.y);
	}
}//��һ��̹��

/*��ȫ��̹��*/
void draw_all_tank() {
	for (int i = 0; i <= num_tank; i++) {
		draw_a_tank(tank[i],tank[i].x,tank[i].y);
	}
}//��ȫ��̹��

/*��ʾ�Ӿɵı���µ�*/
void up_date_type(int new_type, int wz) {
	tank[wz].type = new_type;
	tank[wz].HP = type[new_type].HP;
	draw_a_tank(tank[wz], tank[wz].x, tank[wz].y);
}//��ʾ�Ӿɵı���µ�

/*���̹��λ�úϷ���*/
int check_wz(int xx, int yy, int wz) {
	//����Ƿ����
	if (xx<0 || yy<0 || xx + 4 * SIZE>MAXX || yy + 4 * SIZE>MAXY||wz&&tool.flag[yy/SIZE][xx/SIZE]) {
		return 0;
	}
	//���̹������λ���Ƿ����ϰ����Լ�����
	int tool_x = -1, tool_y = -1;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			//���������鲻�ܿ��������̹�˲��ܴ�Խ����,���ܲȵ�������
			if (mat[yy / SIZE + i][xx / SIZE + j] == 1 || mat[yy / SIZE + i][xx / SIZE + j] == 2
			|| flag[yy / SIZE + i][xx / SIZE + j] >= 0 && flag[yy / SIZE + i][xx / SIZE + j] != wz || door.flag[yy / SIZE + i][xx / SIZE + j])
			{
				//�Ѿ����ϰ������������̹��
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
	//������ϰ��ﲢ���е���
	int index = tool.flag[tool_y][tool_x];
	if (tool_x != -1) {
		//��ձ��
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
		draw_a_tank(tank[0], tank[0].x, tank[0].y);//�ػ�̹��
		return index;
	}
	return 10;
}//���̹��λ�úϷ���

/*���̹��*/
int clear_tank(int x,int y,int type_of) {
	//test();
	int xx = x / SIZE;
	int yy = y / SIZE;//��¼������λ����
	int bh = flag[yy][xx];//̹����̹�˶����еı�ţ�0���Լ�
	if (type_of) {//��ʾ��ը����ħ���˺�
		tank[bh].HP = 0;
	}
	else {//���������ͨ�ӵ�
		tank[bh].HP--;
	}
	if (tank[bh].HP) {
		if (bh) {
			up_date_type(7, bh);//�������ĵ���
		}
		else {
			up_date_type(level-1, bh);//����
		}
		return 0;//��ʾ��û�ҵ�,ֱ�ӷ���
	}
	//�����Ͻ�׷��
	while (flag[yy][xx] == bh) {
		xx--;
	}
	xx++;//Ϊ�˽����������Բ���y,��Ϊ�ҵ���λ��xʵ������û��flag��ǵ�
	while (flag[yy][xx] == bh) {
		yy--;
	}
	yy++;//ͳһ���������Ͻ��������[y-1][x-1]
	//���̹��
	clearrectangle((xx-1)*SIZE, (yy-1)*SIZE, (xx-1)*SIZE + 4 * SIZE - 1, (yy-1)*SIZE + 4 * SIZE - 1);
	putimage((xx - 1)*SIZE + 2, (yy - 1)*SIZE + 2, &img[25]);
	Sleep(100);
	clearrectangle((xx - 1)*SIZE, (yy - 1)*SIZE, (xx - 1)*SIZE + 4 * SIZE - 1, (yy - 1)*SIZE + 4 * SIZE - 1);
	if (CheckGrass(tank[bh].x, tank[bh].y)) {//����ݴ�
		add_grass(tank[bh].x, tank[bh].y);
	}
	//��ձ�ǵ�
	for (int i = yy ; i <yy+ 3; i++) {
		for (int j = xx ; j < xx + 3; j++) {
			flag[i][j] = -1;
		}
	}
	if (bh == 0) {//�����Լ���������
		level = 1;
		tank[0]=add_tank(0, 0);
		draw_a_tank(tank[0], tank[0].x, tank[0].y);
		my_lives--;
	}
	else {//����̹��
		rest_num--;
		if (!rest_num)//̹�����
		{
			return 1;//��ʾ��Ϸ����(������)
		}
		else {
			if (rest_num < 3) {
				tank[bh].state = 0;//��ǰλ��û��̹��
			}
			else {
				//����һ����̹��
				srand(rand_seed);//����ʱ������
				rand_seed = rand();//���������
				int rand_place;
				while (1) {
					srand(rand_seed);//����ʱ������
					rand_seed = rand();//���������
					rand_place = rand() % 4 + 1;
					if (check_wz(birth_wz[rand_place][0], birth_wz[rand_place][1], bh))
						break;
				}
				tank[bh] = add_tank(rand_place, bh);
			}
		}
	}
	PrintTankNum();
	return 0;//��Ϸ����
}//���̹��

/*�����������ڵ�����*/
void remove_any(int detx1,int dety1, int detx2, int dety2,Zidan d)
{
	int x = d.x;
	int y = d.y;
	//������ʼxy����
	int stx = d.x / SIZE;
	int sty = d.y / SIZE;
	//��������
	if (dety1 == dety2) //��������
	{
		//���ж��пյ����
		int index = 2;
		for (int i = -1; i <= 1; i++)//�ҵ�����㲢�˳�
		{
			if (mat[sty + dety1][stx + i] != mat[sty + dety1][stx + i - 1])
			{
				//Ҫ��֤���Ǹ��ձȽ�
				index = i;
				break;
			}
		}
		if (!(mat[sty + dety1][stx + index] * mat[sty + dety1][stx + index - 1])) {
			//�п�
			if (mat[sty + dety1][stx + index] + mat[sty + dety1][stx + index - 1] == 3)//һ���ǲ�
			{
				return;
			}
			index++;
			while (mat[sty + dety1][stx + index] == mat[sty + dety1][stx + index - 1]&&index<2)
			{
				index++;
			}
		}
		if (index!=2) {//�����б仯,��ô��������+�ݣ���+��������+�����������
			//�������
			if (mat[sty + dety1][stx + index] == 3 || mat[sty + dety1][stx + index - 1] == 3
				|| door.flag[sty + dety1][stx + index - 1]+ door.flag[sty + dety1][stx + index])//��������λ�ã��ݲ��ܱ����
			{
				if (mat[sty + dety1][stx + index-1] == 3||door.flag[sty + dety1][stx + index - 1])//��������ǲݻ��ߴ�����
				{
					detx1 = index;//�޸���߽�
				}
				else 
				{
					detx2 = index-1;//�޸��ұ߽�
				}
			}
			else //һ��̹�˴�ģ�������������Ҫ�е�
			{
				if (mat[sty + dety1][stx + index-1] == 1)//�����������
				{
					detx1 = index;//�޸���߽�
				}
				else
				{
					detx2 = index - 1;//�޸��ұ߽�
				}
			}
		}
		else if(mat[sty + dety1][stx + detx1] == 3){
			return;//ȫ�ǲݲ�����
		}
	}
	else //�������� 
	{
		int index = 2;
		for (int i = -1; i <= 1; i++)//�ҵ�����㲢�˳�
		{
			if (mat[sty + i][stx + detx1] != mat[sty + i-1][stx + detx1])
			{
				index = i;
				break;
			}
		}
		if (!(mat[sty + index - 1][stx + detx1] * mat[sty + index][stx + detx1])) {
			//�п�
			if (mat[sty + index][stx + detx1] + mat[sty + index - 1][stx + detx1] == 3) {//��+��
				return;
			}
			index++;
			while (mat[sty + index][stx + detx1] == mat[sty + index - 1][stx + detx1] && index < 2)
			{
				index++;
			}
		}
		if (index!=2) {//�����б仯,��ô��������+�ݣ���+��������+�����������
			//�������
			//���� 3
			if (mat[sty + index -1][stx + detx1] == 3 || mat[sty + index][stx + detx1] == 3
				||door.flag[sty + index - 1][stx + detx1]+door.flag[sty + index][stx + detx1] )//��������λ�ã��ݲ��ܱ����
			{
				if (mat[sty + index-1][stx + detx1] == 3|| door.flag[sty + index - 1][stx + detx1])//�������ǲ�
				{
					dety1 = index;//�޸��ϱ߽�
				}
				else
				{
					dety2 = index - 1;//�޸��±߽�
				}
			}
			else //һ��̹�˴�ģ�������������Ҫ�е�
			{
				if (mat[sty + index -1][stx + detx1] == 1)//�����ϱ�����
				{
					dety1 = index;//�޸��ϱ߽�
				}
				else
				{
					dety2 = index - 1;//�޸��±߽�
				}
			}
		}
		else if (mat[sty + dety1][stx + detx1] == 3) {
			return;//ȫ�ǲݲ�����
		}
	}
	if (d.who != 3 && mat[y / SIZE + dety1][x / SIZE + detx1] == 1) {//����֮��ָ����������+�ݵ����������δ������ȫ
		return;//�޷�����
	}
	//����������������
	int x1 = x + detx1*SIZE;
	int y1 = y + dety1*SIZE;
	int x2 = x + detx2*SIZE+SIZE;
	int y2 = y + dety2*SIZE+SIZE;
	clearrectangle(x1, y1, x2-1, y2-1);
	for (int i = x / SIZE + detx1; i <= x / SIZE + detx2; i++) 
	{
		for (int j = y / SIZE + dety1; j <= y / SIZE + dety2; j++) {
			mat[j][i] = 0;//������
		}
	}
}//�����������ڵ�����

/*��һ���ӵ�*/
void draw_zidan(Zidan d, int xx, int yy) {
	IMAGE ans1,ans2;
	rotateimage(&ans1, &img[((d.who <= 3) + 10) * 2], -PI / 2 * d.dir);//˳ʱ����תdir��90��,Ȼ��ֵ��ans
	rotateimage(&ans2, &img[((d.who <= 3) + 10) * 2+1], -PI / 2 * d.dir);//˳ʱ����תdir��90��,Ȼ��ֵ��ans
	putimage(xx + img_zidan_detx1[d.dir] + 1, yy + img_zidan_dety1[d.dir] + 1,&ans1, SRCAND);
	putimage(xx + img_zidan_detx1[d.dir] + 1, yy + img_zidan_dety1[d.dir] + 1,&ans2 , SRCPAINT);
}//��һ���ӵ�

/*����޲��ӵ�����Ĳ�*/
void check_add_zidan_grass(Zidan d) {
	//�������Ͻ����겢ת��Ϊ��λ��ʽ
	int left_up_x = (d.x + img_zidan_detx1[d.dir]) / SIZE;
	int left_up_y = (d.y + img_zidan_dety1[d.dir]) / SIZE;
	left_up_x -= left_up_x % 2;
	left_up_y -= left_up_y % 2;//ת��Ϊ�ݷ���Ķ�λ��λ��
	if (!(d.dir % 2)) {//����
		int if_ou = 1-(d.x) / SIZE % 2;//����ӵ��Գ���λ��ż���ߣ���ô����Ҫ�������Ų�
		//���ж�Ҫ��Ҫ����
		if (mat[left_up_y][left_up_x] == 3)//��
		{
			putimage(left_up_x*	SIZE, left_up_y*SIZE,&img[17], SRCAND);
			putimage(left_up_x*	SIZE, left_up_y*SIZE,&img[18], SRCPAINT);
		}
		if (if_ou&&mat[left_up_y][left_up_x + 2]==3)//���ұ�
		{
			putimage((left_up_x + 2) * SIZE, left_up_y * SIZE, &img[17], SRCAND);
			putimage((left_up_x + 2) * SIZE, left_up_y * SIZE, &img[18], SRCPAINT);
		}
	}
	else {//����
		int if_ou = 1 - (d.y) / SIZE % 2;//����ӵ��Գ���λ��ż���ߣ���ô����Ҫ�������Ų�
		//���ж�Ҫ��Ҫ����
		if (mat[left_up_y][left_up_x] == 3)//��
		{
			putimage(left_up_x*	SIZE, left_up_y*SIZE, &img[17], SRCAND);
			putimage(left_up_x*	SIZE, left_up_y*SIZE, &img[18], SRCPAINT);
		}
		if (if_ou&&mat[left_up_y+2][left_up_x] == 3)//���ұ�
		{
			putimage(left_up_x  * SIZE, (left_up_y+2) * SIZE, &img[17], SRCAND);
			putimage(left_up_x  * SIZE, (left_up_y+2) * SIZE, &img[18], SRCPAINT);
		}
	}
}//����޲��ӵ�����Ĳ�

/*����ӵ�*/
void clear_zidan(Zidan d) {
	int x1 = d.x + img_zidan_detx1[d.dir];
	int y1 = d.y + img_zidan_dety1[d.dir];
	int x2 = d.x + img_zidan_detx1[d.dir] + SIZE-1;
	int y2 = d.y + img_zidan_dety1[d.dir] + SIZE-1;
	clearrectangle(x1, y1, x2, y2);
	check_add_zidan_grass(d);//��鲹��ݷ���
	//��������Ͻ�����Ȼ���������½�����
}//����ӵ�

/*�����ӵ��Ϸ���*/
int check_zidan() 
{
	Zidan d;
	for (int i = 1; i <= qz.size(); i++) //��ô����ӵ�Ҫ���
	{
		d = qz.front();
		qz.pop();
		//ע��xyʵ�����ǵ�ͷ���꣬���Ͻ�����Ҫ����ƫ�����������棩
		if (d.x == 0 || d.y == 0) {
			if (d.count) {
				clear_zidan(d);
			}
			else {
				d.count = 1;
			}//����                                 
			continue;
		}
		int xx = d.x + SIZE * dirx[d.dir];
		int yy = d.y + SIZE * diry[d.dir];//ȷ����һ��λ��
		if (xx > MAXX || yy > MAXY) {
			if (d.count) {
				clear_zidan(d);
			}
			else {
				d.count = 1;
			}//����
			continue;
		}
		int checkX1 = xx / SIZE + check_if_movex[0][d.dir];
		int checkY1 = yy / SIZE + check_if_movey[0][d.dir];//ȷ�����㵥λλ��1
		int checkX2 = xx / SIZE + check_if_movex[1][d.dir];
		int checkY2 = yy / SIZE + check_if_movey[1][d.dir];//ȷ�����㵥λλ��2
		if (flag[yy/SIZE][xx/SIZE] ==-1 &&(!mat[checkY1][checkX1]&& !mat[checkY2][checkX2]
			|| mat[checkY1][checkX1]==3 && mat[checkY2][checkX2]==3)) {//��Ϊ�ᶼΪ��λ�ã����ù�
			if (d.count) {
				clear_zidan(d);
			}
			else {
				d.count = 1;
			}
			if (!tool.flag[checkY1][checkX1] && !tool.flag[checkY2][checkX2]&& !door.flag[checkY1][checkX1] && !door.flag[checkY2][checkX2]) {//���ܴ��ڵ�����
				draw_zidan(d, xx, yy);//���ӵ�
				//����Ҫ������ת���,�����������Ͻ�������ȷ
				d.x = xx;
				d.y = yy;//��������
				qz.push(d);
			}
		}
		else if((mat[checkY1][checkX1]>0 || mat[checkY2][checkX2]>0)
			  && !((mat[checkY1][checkX1] ==3  || mat[checkY2][checkX2] == 3)
			  &&(flag[checkY1][checkX1] != -1 || flag[checkY2][checkX2]== -1)))//������Ϊ�˱�֤�ݺ�̹��ͬʱ���ڿ����ж������棬��������һ��
		{//û�д�̹��,�򵽷���
			int check_grass = 0;
			if (!((mat[checkY1][checkX1] == 1 || mat[checkY2][checkX2] == 1) && d.who != 3)) //����
			{//��һ�����鲢�ҵȼ������Ͳ�����,������ס��
			    //��������
				switch (d.dir) {
				case 0:
					remove_any(-2, -1, 1, -1, d);//���ݵĲ������ӵ���Ϣ������ڵ�ͷ��λ��(�ĸ�����),�ӵ��ֽ����������
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
		else if (flag[yy / SIZE][xx / SIZE] == 10) {//�򵽼�
			clearrectangle(7 * SIZE * 4, 12 * SIZE * 4, 7 * SIZE * 4 + 8 * SIZE, 12 * SIZE * 4 + 8 * SIZE);
			putimage(7 * SIZE * 4, 12 * SIZE * 4, &img[24]);//��ը��
			return 0;
		}
		else {//����ը����̹��
		//���ݱ�ſ�˭�ҵ���
			if (d.who>3&&flag[yy / SIZE][xx / SIZE] == 0|| d.who <= 3 && flag[yy / SIZE][xx / SIZE]!=10) 
			{
				//������������
				if (clear_tank(xx, yy, 0)) {
					//test();
					return 2;//������Ϸ����
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
}//�����ӵ��Ϸ���

/*����̹�˲���һ���ӵ�*/
void add_zidan(Tank t) {
	int tmp_type = t.type;//����ȡ���ӵ����˺�
	Zidan d;
	d.dir = t.dir;
	int xx, yy;
	switch (t.dir) {//���ݷ����̹�˵����Ͻ�����ȷ���ӵ�ռ�ݿռ�
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
	d.who = t.type;//123�����Լ�����̬���������Լ�,�����ǶԷ���
	d.count = 0;
	qz.push(d);
}//����̹�˲���һ���ӵ�

/*�޸��ϼ�*/
void repair_map() {
	int num = sizeof(rep_map) / sizeof(rep_map[0]);
	for (int i = 0; i < num; i++) {
		int k = rep_map[i][0];
		int x1 = rep_map[i][1] * 4;
		int y1 = rep_map[i][2] * 4;
		int x2 = rep_map[i][3] * 4 - 1;
		int y2 = rep_map[i][4] * 4 - 1;//���½Ƿ�������Ͻ�����
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
}//�޸��ϼ�

/*������*/
int check_tool(int index) {
	//ը��ֹͣ�����޸�
	int x=0, y=0;
	int max_num = 10000;//���Ѱ�Ҵ���
	switch (index) {
	case 1:
		while (max_num--) {
			srand(rand_seed);//����ʱ������
			rand_seed = rand();//���������
			x = rand() % NUMX;
			y = rand() % NUMY;
			if (flag[y][x]>0 && flag[y][x] != 10)
				break;
		}
		if(max_num)//��ʾ�����ҵ�̹��
			if (clear_tank(x*SIZE, y*SIZE, 1)) {
				return 1;
			}
		break;
	case 2:
		stop_epoch = 300;//3��
		break;
	case 3:
		if (level < 3) {
			up_date_type(level+1,0);//��������
			level++;
		}
		break;
	case 4:
		repair_map();
		break;
	}
	return 0;//��Ϸ����
}//������

/*�����ڴ�����*/
int CheckDoor(Tank t,int x,int y)
{
	//xy����һ���ĵ�λ����
	//����Ƿ���봫���ţ�ֻ�����Դ����Ų��ܽ���
	if (1 - door.type == t.dir % 2) {
		if (t.dir % 2) {//����
			if (y != door.y2&&y != door.y1)
				return 0;//���ܽ���
			if (y==door.y1&&(t.dir == 1 && x+ 3 == door.x1|| t.dir == 3 && x == door.x1)) {
				return 1;//�ӵ�һ������
			}
			else if(y==door.y2&&(t.dir == 1 && x + 3 == door.x2 || t.dir == 3 && x == door.x2)){
				return 2;//�ӵڶ�������
			}
		}
		else {//����
			if (x != door.x2&&x != door.x1)
				return 0;
			if (x==door.x1&&(t.dir == 2 && y + 3 == door.y1 || t.dir == 0 && y == door.y1)) {
				return 1;//�ӵ�һ������
			}
			else if (x==door.x2&&(t.dir == 2 && y + 3 == door.y2 || t.dir == 0 && y == door.y2)) {
				return 2;//�ӵڶ�������
			}
		}
	}
	return 0;
}//�����ڴ�����

/*�ĸ�����*/
void control_dir(int now_dir) {
	if (tank[0].dir != now_dir) //ԭ�����������
	{
		tank[0].dir = now_dir;
		draw_a_tank(tank[0], tank[0].x, tank[0].y);
		tank[0].bef_move = now_dir;//����
	}
	else if (tank[0].bef_move >= type[tank[0].type].Speed)//�����ƶ�
	{
		int xx = tank[0].x + dirx[tank[0].dir] * SIZE;
		int yy = tank[0].y + diry[tank[0].dir] * SIZE;//����
		int check_door = CheckDoor(tank[0],xx/SIZE,yy/SIZE);//�����ڴ�����,���ݷ���ֵ����λ��
		//����λ��
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
		if (check_door||check) { //���λ��
			draw_a_tank(tank[0], xx, yy);//xx,yy��ʾ���º��λ��
			if (check_door) {
				Sleep(200);
				mark_tank(tank[0].x, tank[0].y, -1);
				mark_tank(xx, yy, 0);
			}
			else {
				update_mark(tank[0].dir, xx, yy, 0);//��ͨ�ƶ�����̹��λ����Ϣ
			}
			tank[0].x = xx;
			tank[0].y = yy;
			tank[0].bef_move = 0;//����ʱ��
			if (check != 10) {
				check_tool(check);//������
				tool.exist = 0;
			}
		}
	}
}//�ĸ�����

/*��������̹��*/
void check_tank()
{
	for (int i = 1; i <= num_tank; i++) {//����ȡ��̹��
		if (!tank[i].state)continue;//�Ѿ�û����
		if (tank[i].bef_move >= type[tank[i].type].Speed) {
			//����ͣ����ԭ�ص�
			if (tank[i].stop_time) {
				tank[i].stop_time--;
				if (tank[i].bef_fight >= type[tank[i].type].Fight_det) {
					srand(rand_seed);//����ʱ������
					rand_seed = rand();//���������
					int state = rand() % 100;
					if (state <= 50) {
						tank[i].bef_fight = 0;
						add_zidan(tank[i]);//���һ���ӵ�
					}
					else {
						tank[i].bef_fight = 0;
					}
				}
			}
			//��ʾҪ�ƶ���
			else if (tank[i].dir_stop_time) {
				int xx = tank[i].x + dirx[tank[i].dir] * SIZE;
				int yy = tank[i].y + diry[tank[i].dir] * SIZE;//����
				int check_door = CheckDoor(tank[i], xx / SIZE, yy / SIZE);//�����ڴ�����,���ݷ���ֵ����λ��
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
					draw_a_tank(tank[i], xx, yy);//xx,yy��ʾ���º��λ��
					if (check_door) {
						Sleep(200);
						mark_tank(tank[i].x, tank[i].y, -1);
						mark_tank(xx, yy, i);
					}
					else {
						update_mark(tank[i].dir, xx, yy, i);//��ͨ����̹��λ����Ϣ
					}
					tank[i].x = xx;
					tank[i].y = yy;
					tank[i].bef_move = 0;//����ʱ��
					tank[i].dir_stop_time--;
				}
				else 
				{
					//�ȴ�һ��
					if (tank[i].bef_fight >= type[tank[i].type].Fight_det) {
						tank[i].bef_fight = 0;
						add_zidan(tank[i]);//���һ���ӵ�
					}
					srand(rand_seed);//����ʱ������
					rand_seed = rand();//���������
					int state = rand() % 100;
					tank[i].dir_stop_time = state % 10+1;//������������
					if (state % 3==1) {
						tank[i].dir = (tank[i].dir + 1) % 4;
						if(state%2)
							tank[i].stop_time = state*2;//���ܼ����غ�֮��ͣ��ԭ��
					}
					else if(state % 3 == 2){
						tank[i].dir = (tank[i].dir - 1 + 4) % 4;
						if(state%2)
							tank[i].stop_time = state*2;//���ܼ����غ�֮��ͣ��ԭ��
					}
					else {
						tank[i].dir = (tank[i].dir + 2) % 4;//����
					}
					draw_a_tank(tank[i], tank[i].x, tank[i].y);
				}
			}
			else {
				//������Ĭ�����������м�Ŀ��
				srand(rand_seed);//����ʱ������
				rand_seed = rand();//���������
				int state = rand() % 100;
				//�����ƶ������Ż�·��
				if (tank[i].move_type == 1) {
					if (abs(tank[i].x - MAXX / 2) * 2 / MAXX * 100 > state) {//�����м�Զ
						if (tank[i].x - MAXX / 2 >= 0) {
							tank[i].dir = 3;//���
						}
						else {
							tank[i].dir = 1;//�ұ�
						}
					}
					else if (abs(tank[i].y - MAXY) / MAXY * 100 > state) {//��������Զ
						tank[i].dir = 2;//������
					}
					else {
						tank[i].dir = state % 4;//�������
					}
				}
				else {
					if (abs(tank[i].y - MAXY) / MAXY * 100 > state) {//��������Զ
						tank[i].dir = 2;//������
					}
					else if (abs(tank[i].x - MAXX / 2) * 2 / MAXX * 100 > state) {//�����м�Զ
						if (tank[i].x - MAXX / 2 >= 0) {
							tank[i].dir = 3;//���
						}
						else {
							tank[i].dir = 1;//�ұ�
						}
					}
					else {
						tank[i].dir = state % 4;//�������
					}
				}
				tank[i].dir_stop_time = state % 10+1;//������������
				draw_a_tank(tank[i], tank[i].x, tank[i].y);
			}
		}
		//�������
		if (tank[i].bef_fight >= type[tank[i].type].Fight_det) {
			srand(rand_seed);//����ʱ������
			rand_seed = rand();//���������
			int state = rand() % 100;
			if (state <= 50) {
				tank[i].bef_fight = 0;
				add_zidan(tank[i]);//���һ���ӵ�
			}
			else {
				tank[i].bef_fight = 0;
			}
		}
		tank[i].bef_fight = (tank[i].bef_fight + 1) % 10000;//�����ϴη���ʱ��
		tank[i].bef_move = (tank[i].bef_move + 1) % 10000;//�����ϴ��ƶ�ʱ��
	}
}//��������̹��

/*��ӵ���*/
void add_tool() 
{
	int x, y;
	bool check = 0;
	while (!check) {
		srand(rand_seed);//����ʱ������
		rand_seed = rand();//���������
		x = rand() % NUMX - 2;
		y = rand() % NUMY - 2;//���Ͻǵ�λ����
		check = 1;
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				if (mat[y + i][x + j]||flag[y+i][x+j]!=-1)
					check=0;//���������ж���������
			}
		}
	}
	int index = rand() % 4 + 1;//��������
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			tool.flag[y + i][x + j] = index;
		}
	}
	putimage(x*SIZE+2, y*SIZE+2, &img[25+index]);
	tool.exist = 1;
}//��ӵ���

/*�ж�һ�������ܷ����̹��*/
int CheckIfCanExist(int x1,int y1,int x2,int y2){
	//���������ϽǺ����½ǵ����Ͻ�����
	for (int i = y1; i <= y2; i++) {
		for (int j = x1; j <= x2; j++) {
			if (mat[i][j] && mat[i][j] != 3 || flag[i][j] != -1)
				return 0;
		}
	}
	return 1;
}

/*���´�����*/
void UpDataDoor() {
	if (door.x1 >= 0) {//���ԭ�е�
		if (door.type) {//����
			clearrectangle(door.x1*SIZE, door.y1*SIZE, door.x1*SIZE + 4 * SIZE - 1, door.y1*SIZE + SIZE - 1);
			clearrectangle(door.x2*SIZE, door.y2*SIZE, door.x2*SIZE + 4 * SIZE - 1, door.y2*SIZE + SIZE - 1);
		}
		else {//����
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
	//���ɴ�����
	door.type = MyRand() % 2;
	//door.type = 1;
	int num = 0;//��ǰ�ҵ��ĸ���
	int x, y;//������ɵ�һ������
	while (num < 2) {
		srand(rand_seed);//����ʱ������
		rand_seed = rand();//���������
		x = rand() % (NUMX - 10)+ 6;
		y = rand() % (NUMY - 8 )+ 6;//���Ͻǵ�λ����
		//�����ǿյ�(û���κζ���)
		bool if_ok = 1;//��ʾ�Ƿ����
		for (int i = 0; i <= 4; i++) {//����ĸ���
			if (door.type) {//���һ��
				if (mat[y][x + i] || flag[y][x + i] != -1 || flag[y + 1][x + i] != -1 || num && abs(x - door.x1) + abs(y - door.y1) <40) {
					if_ok = 0;
					break;
				}
			}
			else {//���һ��
				if (mat[y+i][x ] || flag[y+i][x]!=-1 || flag[y + i][x + 1]!=-1 || num && abs(x - door.x1) + abs(y - door.y1) < 40) {
					if_ok = 0;
					break;
				}
			}
		}
		//����������̹�˵ĺϷ�����λ��
		if (door.type) {//�������ʹ����Ŵ�����
			//�ȿ�������û��λ��
			if (CheckIfCanExist(x,y-4,x+3,y-1)) {//����������Դ���
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
			else {//����������
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
					if_ok = 0;//���߶��ҹ���������
				}
			}
		}
		else {
			//�ȿ������û��λ��
			if (CheckIfCanExist(x-4, y, x-1, y+3)) {//������߿��Դ���
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
			else {//�������ұ�
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
					if_ok = 0;//���߶��ҹ���������
				}
			}
		}
		if (!if_ok) {
			continue;//����
		}
		//���±�ǵ�
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
}//���´�����

/*��ʼ��Ϸ*/
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
			if (tank[0].bef_fight >= type[tank[0].type].Fight_det) {//����Ҫ������ʱ��βſ��Է����ӵ�
				tank[0].bef_fight = 0;
				add_zidan(tank[0]);//���һ���ӵ�
			}
		}
		if (KEY_DOWN(VK_ESCAPE)) {
			stop();
		}
		if (stop_epoch) {
			stop_epoch--;
		}
		else {
			check_tank();//����̹��
		}
		Sleep(10);//����Ƶ��
		int now_state = check_zidan();
		if (now_state == 2) //�ɹ�ͨ��
		{
			PrintSucess();
			return;
		}
		else if (!now_state || my_lives == 0)//�����Ч�ӵ�������Ƿ���̹��ը��
		{
			PrintGameOver();
			return;
		}
		tank[0].bef_fight = (tank[0].bef_fight + 1) % 10000;//�����ϴη���ʱ��
		tank[0].bef_move = (tank[0].bef_move + 1) % 10000;//�����ϴ��ƶ�ʱ��
		tool.num++;
		if (tool.num == tool_speed) {//Ҫ����ˢ�µ�����
			tool.num = 0;
			if (!tool.exist) {
				add_tool();
				tool.exist = 1;
			}
		}
		door.num--;
		if (door.num == 0) {//���´�����
			door.num = door_speed;//��������
			UpDataDoor();//���´�����
		}
	}
}//��ʼ��Ϸ

void game() {
	//printf("%d", (*all_map)[1][1]);
	initgraph(MAXX + 1 + 400, MAXY + 1, SHOWCONSOLE);
	ini_all();//ȫ�ֳ�ʼ������,һ����Ϸֻ��ʼ��һ��(�����ڹؿ���ʼ��)

	for (int i = 0; i <= 2;i++)//�ؿ�����ѭ��,��ǰ��i��
	{
		now_level = i;
		printNextLevel();//��ӡת������
		while (1) {
			Sleep(10);
			settextcolor(WHITE);
			settextstyle(400, 40, 0);
			outtextxy(300, 300, "���ENTER��ʼ��Ϸ...");
			if (KEY_DOWN(13)) {//���enter����
				cleardevice();
				int num = get_map(map,all_map[now_level]);//����ǰ�Ĺؿ���ͼ��all_map��д��map��.���ҷ�������
				ini_every(map, num);//�ؿ���ʼ��
				ini_tank();//��ʼ����̹��
				draw_all_tank();//��̹��
				begin();//��Ϸ��ʼ,���س���
				while (1) {
					Sleep(10);
					if (KEY_DOWN(VK_ESCAPE)) {//��ȡ��ҿ�ʼ��Ϣ
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