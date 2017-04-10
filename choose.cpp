#include <cstdio>
#include <algorithm>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <queue>
#include "jsoncpp/json.h"

#define rep(i, l, r) for(int i=l; i<=r; i++)
#define dow(i, l, r) for(int i=l; i>=r; i--)
#define clr(x, c) memset(x, c, sizeof(x))
#define pi acos(-1)
#define travel(i) for(edge *p=fir[i]; p; p=p->n)

using namespace std;

typedef pair<double,int> Pdi;
typedef long double ld;

#define maxn 1009
#define maxs 166167001

struct Portal
{
	double x0, y0, x, y, z;
	string guid, latlng, label;
} P[maxn], G[9];

bool cmpXY(Portal a, Portal b){return a.x0<b.x0 || (a.x0==b.x0 && a.y0<b.y0);}

struct F{int v; double S;} qL[maxn], qR[maxn]; bool operator < (F a, F b){return a.S<b.S;}

struct edge{int v; edge *n;} e[maxn], *fir[9], *pt=e;






int n, tot, q[maxn], pid_cal[maxn][maxn];

inline void SWAP(int &a, int &b, int &c) {if (a>b) swap(a,b); if (b>c) swap(b,c); if (a>b) swap(a,b);}

inline int GetLB(int a, int b, int c) {SWAP(a,b,c); return pid_cal[a][b]+c;}

inline void GetPortal(int LB, int &a, int &b, int &c)
{
	a=1; while (a<n-2 && pid_cal[a+1][a+2]+a+3<=LB) a++;
	b=a+1; while (b<n-1 && pid_cal[a][b+1]+b+2<=LB) b++;
	c=LB-pid_cal[a][b];
}







inline void Spin(int lb, int a, int b)
{
	double x=(P[a].y*P[b].z-P[a].z*P[b].y);
	double y=(P[a].z*P[b].x-P[a].x*P[b].z);
	double z=(P[a].x*P[b].y-P[a].y*P[b].x);
	G[lb].x=(P[b].y*z-P[b].z*y);
	G[lb].y=(P[b].z*x-P[b].x*z);
	G[lb].z=(P[b].x*y-P[b].y*x);
}

inline double COS(){return (G[1].x*G[2].x+G[1].y*G[2].y+G[1].z*G[2].z)/sqrt(G[1].x*G[1].x+G[1].y*G[1].y+G[1].z*G[1].z)/sqrt(G[2].x*G[2].x+G[2].y*G[2].y+G[2].z*G[2].z);}

inline double Angle(int a, int b, int c) {Spin(1,a,b); Spin(2,c,b); return acos(COS());} // Spin 增加两个临时向量，COS 计算完删去

inline double Area(int a, int b, int c) {return Angle(a,b,c)+Angle(b,c,a)+Angle(c,a,b)-pi;}






inline void FXL(int a, int b)
{
	G[1].x=P[a].y*P[b].z-P[a].z*P[b].y;
	G[1].y=P[a].z*P[b].x-P[a].x*P[b].z;
	G[1].z=P[a].x*P[b].y-P[a].y*P[b].x;
}

inline double COS(int a){return P[a].x*G[1].x+P[a].y*G[1].y+P[a].z*G[1].z;}

inline bool Left(int a, int b, int c) {FXL(a,b); return COS(c)>0;}

inline bool inField(int a, int b, int c, int d)
{
	if (d==a || d==b || d==c) return false;
	if (!Left(a,b,c)) swap(b,c);
	return Left(a,b,d) && Left(b,c,d) && Left(c,a,d);
}



inline void ChangetoPosition(int a)
{
	istringstream iss(P[a].latlng);
	char tmp;
	iss >> P[a].x0 >> tmp >> P[a].y0;
	P[a].x=cos(P[a].x0/180.0*pi)*cos(P[a].y0/180.0*pi);
	P[a].y=sin(P[a].x0/180.0*pi)*cos(P[a].y0/180.0*pi);
	P[a].z=sin(P[a].y0/180.0*pi);
}

inline void ReadInput(const char *localFileName) // 读入JSON
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
	
	Json::Value::Members arrayMember = input["portals"]["idOthers"]["bkmrk"].getMemberNames();
	for(Json::Value::Members::iterator iter = arrayMember.begin(); iter != arrayMember.end(); ++iter)
	{
		n++;
		P[n].guid = input["portals"]["idOthers"]["bkmrk"][*iter]["guid"].asString();
		P[n].latlng = input["portals"]["idOthers"]["bkmrk"][*iter]["latlng"].asString();
		P[n].label = input["portals"]["idOthers"]["bkmrk"][*iter]["label"].asString();
		ChangetoPosition(n);
	}
}






inline int min3(int a, int b, int c){return (a<=b&&a<=c)?a:(b<=c?b:c);}
inline int max3(int a, int b, int c){return (a>=b&&a>=c)?a:(b>=c?b:c);}

inline void Max(int &a, int b){if (b>a) a=b;}

char Level[maxs]; short int nx[maxs]; int Count[9]; int Lmx[maxn][maxn];






clock_t gap;

Json::Value bm, dt, ini_bm, null_dt;







int QLevel;

struct xyz{double x,y,z;} pos[maxn];
bool operator < (xyz a, xyz b){return a.x<b.x || (a.x==b.x && a.y<b.y) || (a.x==b.x && a.y==b.y && a.z<b.z);}
map<string,xyz> M0;
map<xyz,string> M1;

inline xyz Ave(xyz a, xyz b, xyz c){return (xyz){(a.x+b.x+c.x)/3, (a.y+b.y+c.y)/3, (a.z+b.z+c.z)/3};}

inline void GetOpinion()
{
	M0["A1"]=(xyz){1,0,0}, M0["A2"]=(xyz){0,1,0}, M0["A3"]=(xyz){0,0,1};
	M1[(xyz){1,0,0}]="A1", M1[(xyz){0,1,0}]="A2", M1[(xyz){0,0,1}]="A3";
	
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
	
	ifstream fin("label.txt"); 
	int tmp, num=0; fin >> tmp; for(int i=1, a=1; i<tmp; i++) num+=a, a*=3;
	
	string a, b, c, d;
	rep(i, 1, num) 
	{
		fin >> a >> b >> c >> d;
		M0[a]=Ave(M0[b],M0[c],M0[d]), M1[M0[a]]=a;
		if (a[0]-'A'+1<=QLevel) tmp1[0]=a[0], ini_bm["portals"][tmp1]["bkmrk"][a]["guid"]="";
	}
}








int OPtot;

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
	if (lv == QLevel) return; SWAP(a,b,c);
	if (lv == 1)
	{
		AddLine(a,b,lv), AddLine(b,c,lv), AddLine(c,a,lv);
		pos[a]=(xyz){1,0,0}, AddPortal(a,lv);
		pos[b]=(xyz){0,1,0}, AddPortal(b,lv);
		pos[c]=(xyz){0,0,1}, AddPortal(c,lv);
	}
	int x=GetLB(a,b,c), d=nx[x]; pos[d]=Ave(pos[a],pos[b],pos[c]);
	AddPortal(d,++lv);
	AddLine(a,d,lv), AddLine(b,d,lv), AddLine(c,d,lv);
	OutputPlan(a,b,d,lv);
	OutputPlan(b,c,d,lv);
	OutputPlan(c,a,d,lv);
}

inline void OutputPlan(int LB)
{
	int a, b, c; GetPortal(LB,a,b,c);
	dt=null_dt; bm=ini_bm; OPtot=0; OutputPlan(a,b,c,1);
	
	Json::FastWriter writer;
	puts("Bookmarks JSON:"); cout << writer.write(bm) << endl;
	puts("DrawTools JSON:"); cout << writer.write(dt) << endl;
}











int OPRandom[1000000];

inline void OutputResult()
{
	freopen("result.txt", "w", stdout);
	
	priority_queue<F>q; int sz=0, tot=0, LB;
	
	rep(i, 1, n) rep(j, i+1, n) rep(k, j+1, n) if (Level[LB=GetLB(i,j,k)]>=QLevel)
	{
		if (sz==10) q.pop(), sz--;
		q.push((F){LB,Area(i,j,k)}), sz++;
		if (tot<=1000000) OPRandom[tot++]=LB;
	}
	
	dow(i, sz, 1) printf("面积最小：#%d\n\n", i), OutputPlan(q.top().v), q.pop();
	
	sz=0, tot=0;
	
	rep(i, 1, n) rep(j, i+1, n) rep(k, j+1, n) if (Level[LB=GetLB(i,j,k)]>=QLevel)
	{
		if (sz==10) q.pop(), sz--;
		q.push((F){LB,-Area(i,j,k)}), sz++;
		if (tot<=1000000) OPRandom[tot++]=LB;
	}
	
	dow(i, sz, 1) printf("面积最大：#%d\n\n", i), OutputPlan(q.top().v), q.pop();
	
	random_shuffle(OPRandom, OPRandom+tot);
	rep(i, 1, min(tot,30)) printf("Random #%d\n\n", i), OutputPlan(OPRandom[i-1]);
	
	fclose(stdout);
}












int main()
{
	ReadInput("portal.txt"); srand(time(NULL));
	
	system("cls"); printf("总点数：%d\n倒数第二个 Portal：", n); cout << P[n-1].label << endl;
	getchar();
	
	if (n>maxn-9)
	{
		puts("点数过多，请删减。");
		getchar(); return 0;
	}
	
	sort(P+1, P+1+n, cmpXY);
	
	int tot=0; rep(i, 1, n-2) rep(j, i+1, n-1) tot+=n-j, pid_cal[i][j]=tot-n;
	
	int now=0, totL, totR;
	rep(c, 3, n) dow(a, c-2, 1)
	{
		totL=totR=0; 
		rep(b, a+1, c-1) if (Left(a,c,b)) qL[++totL]=(F){b,Area(a,b,c)}; else qR[++totR]=(F){b,Area(a,c,b)};
		if (totL)
		{
			sort(qL+1, qL+1+totL); clr(fir,0); pt=e;
			rep(i, 1, totL)
			{
				int b=qL[i].v, LB=GetLB(a,b,c), mxLevel=min(Lmx[c][b],Lmx[b][a]); // Change
				char &Lv=Level[LB]; Lv=1;
				dow(o, mxLevel, 1) travel(o) if (Lv>o) break; else if (inField(a,b,c,p->v))
				{
					int tmp=min3(o,Level[GetLB(a,b,p->v)],Level[GetLB(b,c,p->v)])+1;
					if (tmp>Lv) Lv=tmp, nx[LB]=p->v;
				}
				
				int tmp=min3(Lv,Lmx[a][b],Lmx[b][c]); // Change;
				pt->v=b, pt->n=fir[tmp], fir[tmp]=pt++;
				
				Max(Lmx[a][c],Lv); Max(Lmx[c][b],Lv); Max(Lmx[b][a],Lv); // Change
				
				Count[(int)Lv]++; now++;
				if ((double)(clock()-gap)/CLOCKS_PER_SEC>=0.1)
					system("cls"), printf("%.6lf%%", 100.0*now/tot), gap=clock();
			}
		}
		if (totR)
		{
			sort(qR+1, qR+1+totR); clr(fir,0); pt=e;
			rep(i, 1, totR)
			{
				int b=qR[i].v, LB=GetLB(a,b,c), mxLevel=min(Lmx[a][b],Lmx[b][c]); // Change
				char &Lv=Level[LB]; Lv=1;
				dow(o, mxLevel, 1) travel(o) if (Lv>o) break; else if (inField(a,b,c,p->v))
				{
					int tmp=min3(o,Level[GetLB(a,b,p->v)],Level[GetLB(b,c,p->v)])+1;
					if (tmp>Lv) Lv=tmp, nx[LB]=p->v;
				}
				
				int tmp=min3(Lv,Lmx[c][b],Lmx[b][a]); // Change;
				pt->v=b, pt->n=fir[tmp], fir[tmp]=pt++;
				
				Max(Lmx[a][b],Lv); Max(Lmx[b][c],Lv); Max(Lmx[c][a],Lv); // Change
				
				Count[(int)Lv]++; now++;
				if ((double)(clock()-gap)/CLOCKS_PER_SEC>=0.1)
					system("cls"), printf("%.6lf%%", 100.0*now/tot), gap=clock();
			}
		}
	}
	
	system("cls"); 
	rep(i, 3, 7) printf("%d 重竹笋解: %d 个\n", i, Count[i]); puts(""); puts("");
	printf("请问要来份多少重的竹笋？(3-7)\n"); 
	
	QLevel=3; scanf("%d", &QLevel);
	puts("有关信息正在输出到result.txt，祝你好运>.<");
	GetOpinion();
	OutputResult();
	
	return 0;
}







