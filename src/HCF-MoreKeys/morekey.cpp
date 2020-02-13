#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#endif

#include <cstdio>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <queue>
#include <json/json.h>

#define rep(i, l, r) for(int i=l; i<=r; i++)
#define dow(i, l, r) for(int i=l; i>=r; i--)
#define clr(x, c) memset(x, c, sizeof(x))
#define pi acos(-1)
#define travel(i) for(PortalLevelList *p=PlistFirst[i]; p; p=p->next) // 遍历第 i 个 List

using namespace std;

#define maxTotal 1000
#define maxs 166167001

struct Portal // 定义 Portal 结构体，x0,y0 是经纬度坐标，x,y,z 是三维坐标（半径为 1 的球面上），guid,latlng,label 是 JSON 信息
{
	double x0, y0, x, y, z;
	string guid, latlng, label;
	bool MoreKeys;
} P[maxTotal+1], tmpP[2]; // P[] 储存 Portal 信息，tmpP[] 是记录临时三维向量

bool cmpXY(Portal a, Portal b){return a.x0<b.x0 || (a.x0==b.x0 && a.y0<b.y0);}
// 从小到大排序，x0 为第一关键字，y0 为第二关键字

struct Field{int x; double S;} qL[maxTotal+1], qR[maxTotal+1];
bool operator < (Field a, Field b){return a.S<b.S;}
// 记录 Field 类型，x 为中间点 Portal 的编号，S 为 Field 的面积

struct PortalLevelList{int x; PortalLevelList *next;} Plist[maxTotal+1], *PlistFirst[9], *PlistEnd=Plist;
// 构建一个 List 类型

int n;
// n 为 Portal 数量

int Field_ID[maxTotal+1][maxTotal+1];
// Field_ID 是为了方便计算每个 Field 所对应的编号（详细见 GetFL 函数）

inline void SWAP(int &a, int &b, int &c)
{
	if (a>b) swap(a,b);
	if (b>c) swap(b,c);
	if (a>b) swap(a,b);
}
// 将三个数从小到大排序

inline int GetFL(int a, int b, int c) {SWAP(a,b,c); return Field_ID[a][b]+c;}
// 给三个 Portal 的编号求其对应的 Field 的编号

inline void GetPortal(int FieldLabel, int &a, int &b, int &c)
{
	a=1; while (a<n-2 && Field_ID[a+1][a+2]+a+3<=FieldLabel) a++;
	b=a+1; while (b<n-1 && Field_ID[a][b+1]+b+2<=FieldLabel) b++;
	c=FieldLabel-Field_ID[a][b];
}
// 给 Field 的编号求其对应的三个 Portal 的编号






inline void Spin(int lb, int a, int b)
{
	double x=(P[a].y*P[b].z-P[a].z*P[b].y);
	double y=(P[a].z*P[b].x-P[a].x*P[b].z);
	double z=(P[a].x*P[b].y-P[a].y*P[b].x);
	tmpP[lb].x=(P[b].y*z-P[b].z*y);
	tmpP[lb].y=(P[b].z*x-P[b].x*z);
	tmpP[lb].z=(P[b].x*y-P[b].y*x);
}

inline double COS()
{
	return (tmpP[0].x*tmpP[1].x+tmpP[0].y*tmpP[1].y+tmpP[0].z*tmpP[1].z)
			/ sqrt(tmpP[0].x*tmpP[0].x+tmpP[0].y*tmpP[0].y+tmpP[0].z*tmpP[0].z)
			/ sqrt(tmpP[1].x*tmpP[1].x+tmpP[1].y*tmpP[1].y+tmpP[1].z*tmpP[1].z);
}

inline double Angle(int a, int b, int c) {Spin(0,a,b); Spin(1,c,b); return acos(COS());}
// 计算 a-b-c 在球面上的角度

inline double Area(int a, int b, int c) {return Angle(a,b,c)+Angle(b,c,a)+Angle(c,a,b)-pi;}
// 计算 a-b-c 形成的 Field 的大小

inline void FXL(int a, int b)
{
	tmpP[0].x=P[a].y*P[b].z-P[a].z*P[b].y;
	tmpP[0].y=P[a].z*P[b].x-P[a].x*P[b].z;
	tmpP[0].z=P[a].x*P[b].y-P[a].y*P[b].x;
}

inline double COS(int a){return P[a].x*tmpP[0].x+P[a].y*tmpP[0].y+P[a].z*tmpP[0].z;}

inline bool Left(int a, int b, int c) {FXL(a,b); return COS(c)>0;}
// 判断 c 是否在 a→b 左侧

inline bool inField(int a, int b, int c, int d)
{
	if (d==a || d==b || d==c) return false;
	if (!Left(a,b,c)) swap(b,c);
	return Left(a,b,d) && Left(b,c,d) && Left(c,a,d);
}
// 判断 d 是否在 a-b-c 形成的 Field 内

inline void ChangetoPosition(int a)
{
	istringstream iss(P[a].latlng);
	char tmp;
	iss >> P[a].x0 >> tmp >> P[a].y0;
	P[a].x=cos(P[a].x0/180.0*pi)*cos(P[a].y0/180.0*pi);
	P[a].y=sin(P[a].x0/180.0*pi)*cos(P[a].y0/180.0*pi);
	P[a].z=sin(P[a].y0/180.0*pi);
}
// 把经纬度坐标转成三维坐标

inline void ReadInput(const char *localFileName) // 读入 JSON
{
	string str, chunk;

	if (localFileName)
	{
		ifstream fin(localFileName);
		if (fin)
			while (getline(fin, chunk) && chunk != "")
				str += chunk;
		else
			while (getline(cin, chunk) && chunk != "")
				str += chunk;
	}
	else
		while (getline(cin, chunk) && chunk != "") str += chunk;

	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input);

	Json::Value::Members arrayMember = input["portals"]["idMoreKeys"]["bkmrk"].getMemberNames();
	for(Json::Value::Members::iterator iter = arrayMember.begin(); iter != arrayMember.end(); ++iter)
	{
		n++;
		P[n].guid = input["portals"]["idMoreKeys"]["bkmrk"][*iter]["guid"].asString();
		P[n].latlng = input["portals"]["idMoreKeys"]["bkmrk"][*iter]["latlng"].asString();
		P[n].label = input["portals"]["idMoreKeys"]["bkmrk"][*iter]["label"].asString();
		P[n].MoreKeys = true;
		ChangetoPosition(n);
	}

	arrayMember = input["portals"]["idOthers"]["bkmrk"].getMemberNames();
	for(Json::Value::Members::iterator iter = arrayMember.begin(); iter != arrayMember.end(); ++iter)
	{
		n++;
		P[n].guid = input["portals"]["idOthers"]["bkmrk"][*iter]["guid"].asString();
		P[n].latlng = input["portals"]["idOthers"]["bkmrk"][*iter]["latlng"].asString();
		P[n].label = input["portals"]["idOthers"]["bkmrk"][*iter]["label"].asString();
		P[n].MoreKeys = false;
		ChangetoPosition(n); // 把经纬度坐标转成三维坐标
	}
}






inline int min3(int a, int b, int c){return (a<=b&&a<=c)?a:(b<=c?b:c);}
inline int min(int a, int b){return a<b?a:b;}
inline int max3(int a, int b, int c){return (a>=b&&a>=c)?a:(b>=c?b:c);}

inline void Max(int &a, int b){if (b>a) a=b;}

char Level[maxs]; // 记录每个 Field 的最大层数
short int NextPortal[maxs];  // 记录每个 Field 在达到最大层数时的内点
int Count[9]; // Count[i] 表示有多少个 Field 其最大层数为 i
int Lmx[maxTotal+1][maxTotal+1]; // Lmx[a][b] 表示向量 a→b 左边所形成的 Field 的最大层数






clock_t gap; // 记录时间

Json::Value bm, dt, ini_bm, null_dt; // JSON 信息载体







int QLevel; // 询问层数

struct xyz{int x,y,z;} pos[maxTotal+1]; // xyz 是一个自设的参数
bool operator < (xyz a, xyz b){return a.x<b.x || (a.x==b.x && a.y<b.y) || (a.x==b.x && a.y==b.y && a.z<b.z);}
map<string,xyz> M0; // 编号映射到 xyz 参数
map<xyz,string> M1; // xyz 参数映射到编号

inline xyz Ave(xyz a, xyz b, xyz c){return (xyz){(a.x+b.x+c.x)/3, (a.y+b.y+c.y)/3, (a.z+b.z+c.z)/3};}
// 由 Field 上三点的 xyz 得到内点的 xyz

inline void GetOpinion()
{
	M0["A1"]=(xyz){(int)5e8,0,0}, M0["A2"]=(xyz){0,(int)5e8,0}, M0["A3"]=(xyz){0,0,(int)5e8}; // 设置三个顶点的 xyz 参数
	M1[(xyz){(int)5e8,0,0}]="A1", M1[(xyz){0,(int)5e8,0}]="A2", M1[(xyz){0,0,(int)5e8}]="A3";

	string tmp1="A", tmp2="Lv 0";
	rep(i, 1, QLevel)
	{
		tmp1[0]='A'+i-1, tmp2[3]='0'+i;
		ini_bm["portals"][tmp1]["label"]=tmp2;
		ini_bm["portals"][tmp1]["state"]=0;
	}
	ini_bm["portals"]["A"]["bkmrk"]["A1"]["guid"]="";
	ini_bm["portals"]["A"]["bkmrk"]["A2"]["guid"]="";
	ini_bm["portals"]["A"]["bkmrk"]["A3"]["guid"]="";

	// LABEL 每一行有四个字符串，“A B C D” 表示以编号为 A,B,C 形成的 Field 的内点的编号为 D

	ifstream fin("../LABEL");
	int tmp, num=0; fin >> tmp; for(int i=1, a=1; i<tmp; i++) num+=a, a*=3;

	string a, b, c, d;
	rep(i, 1, num)
	{
		fin >> a >> b >> c >> d;
		M0[a]=Ave(M0[b],M0[c],M0[d]), M1[M0[a]]=a;
		if (a[0]-'A'+1<=QLevel) tmp1[0]=a[0], ini_bm["portals"][tmp1]["bkmrk"][a]["guid"]="";
	}
}








int OPtot; // JSON 内 Line 的数量

inline string D_toString(double a)
{
	char buffer[20];
	sprintf(buffer, "%f", a);
	return buffer;
}

inline string I_toString(int a)
{
	char buffer[20];
	sprintf(buffer, "%d", a);
	return buffer;
}

inline void AddLine(int a, int b, int lv)
{
	if (lv == 1) dt[OPtot]["color"] = "#0000ff";
	if (lv == 2) dt[OPtot]["color"] = "#2222ff";
	if (lv == 3) dt[OPtot]["color"] = "#4444ff";
	if (lv == 4) dt[OPtot]["color"] = "#6666ff";
	if (lv == 5) dt[OPtot]["color"] = "#8888ff";
	if (lv == 6) dt[OPtot]["color"] = "#aaaaff";
	if (lv == 7) dt[OPtot]["color"] = "#ccccff";

	dt[OPtot]["latLngs"][0]["lat"] = D_toString(P[a].x0);
	dt[OPtot]["latLngs"][0]["lng"] = D_toString(P[a].y0);
	dt[OPtot]["latLngs"][1]["lat"] = D_toString(P[b].x0);
	dt[OPtot]["latLngs"][1]["lng"] = D_toString(P[b].y0);

	dt[OPtot]["type"] = "polyline";

	OPtot++;
}

inline void AddPortal(int a, int lv)
{
	string tmp="A"; tmp[0]+=lv-1;
	bm["portals"][tmp]["bkmrk"][M1[pos[a]]]["guid"] = P[a].guid;
	bm["portals"][tmp]["bkmrk"][M1[pos[a]]]["latlng"] = P[a].latlng;
	bm["portals"][tmp]["bkmrk"][M1[pos[a]]]["label"] = P[a].label;
}

void OutputPlan(int a, int b, int c, int lv)
{
	if (lv == QLevel) return;
	SWAP(a,b,c);
	if (lv == 1)
	{
		AddLine(a,b,lv), AddLine(b,c,lv), AddLine(c,a,lv);
		pos[a]=(xyz){(int)5e8,0,0}, AddPortal(a,lv); // 由 xyz 参数查询到当前点所对应的编号
		pos[b]=(xyz){0,(int)5e8,0}, AddPortal(b,lv);
		pos[c]=(xyz){0,0,(int)5e8}, AddPortal(c,lv);
	}
	int x=GetFL(a,b,c), d=NextPortal[x]; pos[d]=Ave(pos[a],pos[b],pos[c]);
	AddPortal(d,++lv);
	AddLine(a,d,lv), AddLine(b,d,lv), AddLine(c,d,lv);
	OutputPlan(a,b,d,lv); // 递归输出方案
	OutputPlan(b,c,d,lv);
	OutputPlan(c,a,d,lv);
}

inline void OutputPlan(int FieldLabel) // 输出方案到 JSON
{
	int a, b, c; GetPortal(FieldLabel,a,b,c);
	dt=null_dt; bm=ini_bm; OPtot=0; OutputPlan(a,b,c,1);

	Json::FastWriter writer;
	puts("Bookmarks JSON:"); cout << writer.write(bm) << endl;
	puts("DrawTools JSON:"); cout << writer.write(dt) << endl;
}











#define OPRandomSize 1000000
int OPRandom[OPRandomSize];
int MoreKeys[maxTotal], Mn;
int noMoreKeys[maxTotal], Nn;
#define minField 10
#define maxField 10
#define ranField 30

inline void OutputResult()
{
	freopen("result.txt", "w", stdout);

	priority_queue<Field>qmin, qmax; int tot, FieldLabel;



	printf("无 MoreKeys Portal 限制：\n\n");
	tot=0;
	rep(i, 1, n) rep(j, i+1, n) rep(k, j+1, n) if (Level[FieldLabel=GetFL(i,j,k)]>=QLevel)
	{
		if ((int)qmin.size()==minField) qmin.pop();
		qmin.push((Field){FieldLabel,Area(i,j,k)});
		if ((int)qmax.size()==maxField) qmax.pop();
		qmax.push((Field){FieldLabel,-Area(i,j,k)});
		if (tot<=OPRandomSize) OPRandom[tot++]=FieldLabel;
	}
	// 输出最小的前 minField 个方案
	while (!qmin.empty()) printf("面积最小：#%d\n\n", (int)qmin.size()), OutputPlan(qmin.top().x), qmin.pop();
	// 输出最大的前 maxField 个方案
	while (!qmax.empty()) printf("面积最大：#%d\n\n", (int)qmax.size()), OutputPlan(qmax.top().x), qmax.pop();
	// 随机输出 ranField 个方案
	random_shuffle(OPRandom, OPRandom+tot);
	rep(i, 1, min(tot,ranField)) printf("Random #%d\n\n", i), OutputPlan(OPRandom[i-1]);



	rep(i, 1, n) if (P[i].MoreKeys) MoreKeys[++Mn]=i; else noMoreKeys[++Nn]=i;

#define nMK(i, ii, l) for(int ii=l,i=noMoreKeys[l]; ii<=Nn; i=noMoreKeys[++ii])
#define MK(i, ii, l) for(int ii=l,i=MoreKeys[l]; ii<=Mn; i=MoreKeys[++ii])



	printf("1 个 MoreKey Portal 在顶点或中点上：\n\n");
	tot=0;
	MK(i, ii, 1) nMK(j, jj, 1) nMK(k, kk, jj+1) if (Level[FieldLabel=GetFL(i,j,k)]>=QLevel) nMK(h, hh, 1)
	 	if (h!=i && h!=j && h!=k && inField(i,j,k,h) &&
			Level[GetFL(i,j,h)]>=QLevel-1 &&
			Level[GetFL(i,h,k)]>=QLevel-1 &&
			Level[GetFL(h,j,k)]>=QLevel-1)
		{
			if ((int)qmin.size()==minField) qmin.pop();
			qmin.push((Field){FieldLabel,Area(i,j,k)});
			if ((int)qmax.size()==maxField) qmax.pop();
			qmax.push((Field){FieldLabel,-Area(i,j,k)});
			if (tot<=OPRandomSize) OPRandom[tot++]=FieldLabel;
		}
	nMK(i, ii, 1) nMK(j, jj, ii+1) nMK(k, kk, jj+1) if (Level[FieldLabel=GetFL(i,j,k)]>=QLevel) MK(h, hh, 1)
	 	if (h!=i && h!=j && h!=k && inField(i,j,k,h) &&
			Level[GetFL(i,j,h)]>=QLevel-1 &&
			Level[GetFL(i,h,k)]>=QLevel-1 &&
			Level[GetFL(h,j,k)]>=QLevel-1)
		{
			if ((int)qmin.size()==minField) qmin.pop();
			qmin.push((Field){FieldLabel,Area(i,j,k)});
			if ((int)qmax.size()==maxField) qmax.pop();
			qmax.push((Field){FieldLabel,-Area(i,j,k)});
			if (tot<=OPRandomSize) OPRandom[tot++]=FieldLabel;
		}
	// 输出最小的前 minField 个方案
	while (!qmin.empty()) printf("面积最小：#%d\n\n", (int)qmin.size()), OutputPlan(qmin.top().x), qmin.pop();
	// 输出最大的前 maxField 个方案
	while (!qmax.empty()) printf("面积最大：#%d\n\n", (int)qmax.size()), OutputPlan(qmax.top().x), qmax.pop();
	// 随机输出 ranField 个方案
	random_shuffle(OPRandom, OPRandom+tot);
	rep(i, 1, min(tot,ranField)) printf("Random #%d\n\n", i), OutputPlan(OPRandom[i-1]);



	printf("2 个 MoreKey Portal 在顶点或中点上：\n\n");
	tot=0;
	MK(i, ii, 1) MK(j, jj, ii+1) nMK(k, kk, 1) if (Level[FieldLabel=GetFL(i,j,k)]>=QLevel) nMK(h, hh, 1)
	 	if (h!=i && h!=j && h!=k && inField(i,j,k,h) &&
			Level[GetFL(i,j,h)]>=QLevel-1 &&
			Level[GetFL(i,h,k)]>=QLevel-1 &&
			Level[GetFL(h,j,k)]>=QLevel-1)
		{
			if ((int)qmin.size()==minField) qmin.pop();
			qmin.push((Field){FieldLabel,Area(i,j,k)});
			if ((int)qmax.size()==maxField) qmax.pop();
			qmax.push((Field){FieldLabel,-Area(i,j,k)});
			if (tot<=OPRandomSize) OPRandom[tot++]=FieldLabel;
		}
	MK(i, ii, 1) nMK(j, jj, 1) nMK(k, kk, jj+1) if (Level[FieldLabel=GetFL(i,j,k)]>=QLevel) MK(h, hh, 1)
	 	if (h!=i && h!=j && h!=k && inField(i,j,k,h) &&
			Level[GetFL(i,j,h)]>=QLevel-1 &&
			Level[GetFL(i,h,k)]>=QLevel-1 &&
			Level[GetFL(h,j,k)]>=QLevel-1)
		{
			if ((int)qmin.size()==minField) qmin.pop();
			qmin.push((Field){FieldLabel,Area(i,j,k)});
			if ((int)qmax.size()==maxField) qmax.pop();
			qmax.push((Field){FieldLabel,-Area(i,j,k)});
			if (tot<=OPRandomSize) OPRandom[tot++]=FieldLabel;
		}
	// 输出最小的前 minField 个方案
	while (!qmin.empty()) printf("面积最小：#%d\n\n", (int)qmin.size()), OutputPlan(qmin.top().x), qmin.pop();
	// 输出最大的前 maxField 个方案
	while (!qmax.empty()) printf("面积最大：#%d\n\n", (int)qmax.size()), OutputPlan(qmax.top().x), qmax.pop();
	// 随机输出 ranField 个方案
	random_shuffle(OPRandom, OPRandom+tot);
	rep(i, 1, min(tot,ranField)) printf("Random #%d\n\n", i), OutputPlan(OPRandom[i-1]);



	printf("3 个 MoreKey Portal 在顶点或中点上：\n\n");
	tot=0;
	MK(i, ii, 1) MK(j, jj, ii+1) MK(k, kk, jj+1) if (Level[FieldLabel=GetFL(i,j,k)]>=QLevel) nMK(h, hh, 1)
	 	if (h!=i && h!=j && h!=k && inField(i,j,k,h) &&
			Level[GetFL(i,j,h)]>=QLevel-1 &&
			Level[GetFL(i,h,k)]>=QLevel-1 &&
			Level[GetFL(h,j,k)]>=QLevel-1)
		{
			if ((int)qmin.size()==minField) qmin.pop();
			qmin.push((Field){FieldLabel,Area(i,j,k)});
			if ((int)qmax.size()==maxField) qmax.pop();
			qmax.push((Field){FieldLabel,-Area(i,j,k)});
			if (tot<=OPRandomSize) OPRandom[tot++]=FieldLabel;
		}
	MK(i, ii, 1) MK(j, jj, ii+1) nMK(k, kk, 1) if (Level[FieldLabel=GetFL(i,j,k)]>=QLevel) MK(h, hh, 1)
	 	if (h!=i && h!=j && h!=k && inField(i,j,k,h) &&
			Level[GetFL(i,j,h)]>=QLevel-1 &&
			Level[GetFL(i,h,k)]>=QLevel-1 &&
			Level[GetFL(h,j,k)]>=QLevel-1)
		{
			if ((int)qmin.size()==minField) qmin.pop();
			qmin.push((Field){FieldLabel,Area(i,j,k)});
			if ((int)qmax.size()==maxField) qmax.pop();
			qmax.push((Field){FieldLabel,-Area(i,j,k)});
			if (tot<=OPRandomSize) OPRandom[tot++]=FieldLabel;
		}
	// 输出最小的前 minField 个方案
	while (!qmin.empty()) printf("面积最小：#%d\n\n", (int)qmin.size()), OutputPlan(qmin.top().x), qmin.pop();
	// 输出最大的前 maxField 个方案
	while (!qmax.empty()) printf("面积最大：#%d\n\n", (int)qmax.size()), OutputPlan(qmax.top().x), qmax.pop();
	// 随机输出 ranField 个方案
	random_shuffle(OPRandom, OPRandom+tot);
	rep(i, 1, min(tot,ranField)) printf("Random #%d\n\n", i), OutputPlan(OPRandom[i-1]);



	printf("4 个 MoreKey Portal 在顶点或中点上：\n\n");
	tot=0;
	MK(i, ii, 1) MK(j, jj, ii+1) MK(k, kk, jj+1) if (Level[FieldLabel=GetFL(i,j,k)]>=QLevel) MK(h, hh, 1)
	 	if (h!=i && h!=j && h!=k && inField(i,j,k,h) &&
			Level[GetFL(i,j,h)]>=QLevel-1 &&
			Level[GetFL(i,h,k)]>=QLevel-1 &&
			Level[GetFL(h,j,k)]>=QLevel-1)
		{
			if ((int)qmin.size()==minField) qmin.pop();
			qmin.push((Field){FieldLabel,Area(i,j,k)});
			if ((int)qmax.size()==maxField) qmax.pop();
			qmax.push((Field){FieldLabel,-Area(i,j,k)});
			if (tot<=OPRandomSize) OPRandom[tot++]=FieldLabel;
		}
	// 输出最小的前 minField 个方案
	while (!qmin.empty()) printf("面积最小：#%d\n\n", (int)qmin.size()), OutputPlan(qmin.top().x), qmin.pop();
	// 输出最大的前 maxField 个方案
	while (!qmax.empty()) printf("面积最大：#%d\n\n", (int)qmax.size()), OutputPlan(qmax.top().x), qmax.pop();
	// 随机输出 ranField 个方案
	random_shuffle(OPRandom, OPRandom+tot);
	rep(i, 1, min(tot,ranField)) printf("Random #%d\n\n", i), OutputPlan(OPRandom[i-1]);



	// 重名预警
	printf("需要注意的重名 Po：\n");
	rep(i, 1, n) rep(j, i+1, n) if (P[i].label == P[j].label) {cout << P[i].label << endl; break;}

	fclose(stdout);
}












int main()
{
	system("cls");

	cout << "请将导出的 Bookmark 储存为 portal.txt 并放置于同目录下" << endl;
	getchar();

	ReadInput("portal.txt"); srand(time(NULL));

	cout << "总点数：" << n << endl;
	cout << "倒数第二个 Portal：" << P[n-1].label << endl;
	cout << "若与读入数据不吻合，则请检查下是否操作有误（或者是触发了尚未修复的 Portal 名称 Bug）" << endl;
	getchar();

	if (n>maxTotal)
	{
		cout << "Portal 数量超过了 " << maxTotal << " 的上限，请删减。" << endl;
		getchar(); return 0;
	}

	sort(P+1, P+1+n, cmpXY); // 按经纬坐标排序

	int tot=0; rep(i, 1, n-2) rep(j, i+1, n-1) tot+=n-j, Field_ID[i][j]=tot-n;
	// 枚举左端点和中间端点，tot 记录 Field 总数，Field_ID 是为了方便计算每个 Field 所对应的编号（详细见 GetFL 函数）

	int now=0, totL, totR; // now 表示当前已经处理完了多少个 Field
	rep(c, 3, n) dow(a, c-2, 1) // 从左往右枚举右端点，从右往左枚举左端点
	{
		totL=totR=0;
		rep(b, a+1, c-1) if (Left(a,c,b)) qL[++totL]=(Field){b,Area(a,b,c)}; else qR[++totR]=(Field){b,Area(a,c,b)};
		// 枚举中间端点，qL[] 为向量 a→c 左边的 Portal（总数为 totL），qR[] 为向量 a→c 右边的 Portal（总数为 totR）
		if (totL)
		{
			sort(qL+1, qL+1+totL); // 按面积从小到大排序
			clr(PlistFirst,0); PlistEnd=Plist; // List 初始化
			rep(i, 1, totL)
			{
				int b=qL[i].x, FieldLabel=GetFL(a,b,c), mxLevel=max(Lmx[c][b],Lmx[b][a]); // mxLevel 是由向量 c→b 和 b→a 左边所形成的 Field 的最大层数所决定的 Level 上限
				char &Lv=Level[FieldLabel]; Lv=1; // Lv 为当前 Field 的 Level
				dow(o, mxLevel, 1) travel(o) if (Lv>o) break; else if (inField(a,b,c,p->x)) // 从高到低枚举内层 Field 等级并遍历 List
				{
					int tmp=min3(o,Level[GetFL(a,b,p->x)],Level[GetFL(b,c,p->x)])+1; // 求内点为 p->x 时当前 Field 所能达到的最大层数
					if (tmp>Lv) Lv=tmp, NextPortal[FieldLabel]=p->x;
				}

				int tmp=min(Lv,max(Lmx[a][b],Lmx[b][c])); // 判断下当 b 作为内点 a-c 做边的时候最多能多少层
				PlistEnd->x=b, PlistEnd->next=PlistFirst[tmp], PlistFirst[tmp]=PlistEnd++; // 将 b 加入到 List[tmp]

				Max(Lmx[a][c],Lv); Max(Lmx[c][b],Lv); Max(Lmx[b][a],Lv); // 更新 Lmx 数组

				Count[(int)Lv]++; now++;
				if ((double)(clock()-gap)/CLOCKS_PER_SEC>=0.1)
					system("cls"), printf("%.6lf%%\n", 100.0*now/tot), gap=clock(); // 每隔 0.1s 显示一次百分比进度
			}
		}
		if (totR) // 注释同上
		{
			sort(qR+1, qR+1+totR);
			clr(PlistFirst,0); PlistEnd=Plist;
			rep(i, 1, totR)
			{
				int b=qR[i].x, FieldLabel=GetFL(a,b,c), mxLevel=max(Lmx[a][b],Lmx[b][c]);
				char &Lv=Level[FieldLabel]; Lv=1;
				dow(o, mxLevel, 1) travel(o) if (Lv>o) break; else if (inField(a,b,c,p->x))
				{
					int tmp=min3(o,Level[GetFL(a,b,p->x)],Level[GetFL(b,c,p->x)])+1;
					if (tmp>Lv) Lv=tmp, NextPortal[FieldLabel]=p->x;
				}

				int tmp=min3(Lv,Lmx[c][b],Lmx[b][a]);
				PlistEnd->x=b, PlistEnd->next=PlistFirst[tmp], PlistFirst[tmp]=PlistEnd++;

				Max(Lmx[a][b],Lv); Max(Lmx[b][c],Lv); Max(Lmx[c][a],Lv);

				Count[(int)Lv]++; now++;
				if ((double)(clock()-gap)/CLOCKS_PER_SEC>=0.1)
					system("cls"), printf("%.6lf%%\n", 100.0*now/tot), gap=clock();
			}
		}
	}

	system("cls");
	rep(i, 3, 7) printf("%d 重竹笋解: %d 个\n", i, Count[i]);
	puts(""); puts("");
	printf("请问要来份多少重的竹笋？(3-7)\n");

	QLevel=3; scanf("%d", &QLevel);
	puts("有关信息正在输出到result.txt，祝你好运>.<");
	GetOpinion();
	OutputResult();

	return 0;
}
