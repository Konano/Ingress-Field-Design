#include <cstdio>
#include <algorithm>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include "jsoncpp/json.h"

#define rep(i, l, r) for(int i=l; i<=r; i++)
#define dow(i, l, r) for(int i=l; i>=r; i--)
#define clr(x, c) memset(x, c, sizeof(x))
#define all(x) (x).begin,(x).end
#define pb push_back
#define pi acos(-1)

using namespace std;

typedef pair<double,int> Pdi;

#define maxn 509
#define maxs 30000009

#define CHOOSE 0

struct node
{
	double x, y;
	string guid, latlng, label;
} P[1009];

struct F{int a,b,c,P; double S,G;} q[maxs];
bool cmpP(F a, F b){return a.P<b.P;}
bool cmpS(F a, F b){return a.S<b.S;}
bool cmpG(F a, F b){return a.G<b.G;}

int n, tot, Total, QLevel, pid_cal[maxn][maxn], lv[maxs], nx[maxs], po[maxs], Count[9];
double pretty[maxs];

clock_t gap;

Json::Value bm, dt, ini_bm, null_dt;

inline int min(int a, int b){return a<b?a:b;}

inline double min3(double a, double b, double c){return (a<=b&&a<=c)?a:(b<=c?b:c);}

inline double max3(double a, double b, double c){return (a>=b&&a>=c)?a:(b>=c?b:c);}

inline double dis(int a, int b){return sqrt(pow(P[a].x-P[b].x,2) + pow(P[a].y-P[b].y,2));}

inline double FieldS(int a, int b, int c)
{
	return fabs(P[a].x*P[b].y+P[b].x*P[c].y+P[c].x*P[a].y - P[a].y*P[b].x-P[b].y*P[c].x-P[c].y*P[a].x)/2;
}

inline double disL(int a, int b, int c){return FieldS(a,b,c)*2/dis(a,b);}

inline double FangCha(double a, double b, double c)
{
	double sum=a+b+c; a=a/sum*3, b=b/sum*3, c=c/sum*3;
	return (pow(a-1,2)+pow(b-1,2)+pow(c-1,2))/3;
}

inline double abc(double a, double b, double c)
{
	double sum=a+b+c; a=a/sum, b=b/sum, c=c/sum;
	return max3(a,b,c)-min3(a,b,c);
}

inline double CalPretty(int a, int b, int c, int d)
{
	int r=FieldS(a,b,c)*2/(dis(a,d)+dis(b,d)+dis(c,d));
	return (max3(dis(a,d),dis(b,d),dis(c,d))-min3(dis(a,d),dis(b,d),dis(c,d)))/r;
}

inline double Dir(int a, int b, int c)
{
	return (P[b].x-P[a].x)*(P[c].y-P[a].y)-(P[c].x-P[a].x)*(P[b].y-P[a].y);
}

inline bool inField(int a, int b, int c, int d)
{
	return Dir(a,b,d)*Dir(b,c,d)>0 && Dir(b,c,d)*Dir(c,a,d)>0;
}

inline int GetPID(int a, int b, int c){return pid_cal[a][b]+c-b;}

struct xyz{double x,y,z;} pos[maxn];
bool operator < (xyz a, xyz b){return a.x<b.x || (a.x==b.x && a.y<b.y) || (a.x==b.x && a.y==b.y && a.z<b.z);}
map<string,xyz> M0;
map<xyz,string> M1;

inline xyz Ave(xyz a, xyz b, xyz c){return (xyz){(a.x+b.x+c.x)/3, (a.y+b.y+c.y)/3, (a.z+b.z+c.z)/3};}

inline void ChangetoPosition(int a)
{
	istringstream iss(P[a].latlng);
	char tmp;
	iss >> P[a].x >> tmp >> P[a].y;
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

int FieldLevel(int a, int b, int c)
{
	if (a>b) swap(a,b); if (b>c) swap(b,c); if (a>b) swap(a,b);
	int x=GetPID(a,b,c), tmp, y; double mn=0, tmp2;
	if (lv[x]) return lv[x]; tot++;
	for(int ta=1, tb=a, tc=b, td=c; ta<tb; ta++) if (inField(a,b,c,ta))
	{
		po[x]++; tmp=9; tmp2=0;
		y=GetPID(ta,tb,tc); if ((lv[y]?lv[y]:FieldLevel(ta,tb,tc))<lv[x]) continue; else tmp=min(tmp,lv[y]), tmp2+=pretty[y];
		y=GetPID(ta,tb,td); if ((lv[y]?lv[y]:FieldLevel(ta,tb,td))<lv[x]) continue; else tmp=min(tmp,lv[y]), tmp2+=pretty[y];
		y=GetPID(ta,tc,td); if ((lv[y]?lv[y]:FieldLevel(ta,tc,td))<lv[x]) continue; else tmp=min(tmp,lv[y]), tmp2+=pretty[y];
		tmp2*=0.6; tmp2+=CalPretty(a,b,c,ta); if (tmp>lv[x] || (tmp==lv[x] && tmp2<mn)) lv[x]=tmp, nx[x]=ta, mn=tmp2;
	}
	for(int ta=a, tb=a+1, tc=b, td=c; tb<tc; tb++) if (inField(a,b,c,tb))
	{
		po[x]++; tmp=9; tmp2=0;
		y=GetPID(ta,tb,tc); if ((lv[y]?lv[y]:FieldLevel(ta,tb,tc))<=lv[x]) continue; else tmp=min(tmp,lv[y]), tmp2+=pretty[y];
		y=GetPID(ta,tb,td); if ((lv[y]?lv[y]:FieldLevel(ta,tb,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]), tmp2+=pretty[y];
		y=GetPID(tb,tc,td); if ((lv[y]?lv[y]:FieldLevel(tb,tc,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]), tmp2+=pretty[y];
		tmp2*=0.6; tmp2+=CalPretty(a,b,c,tb); if (tmp>lv[x] || (tmp==lv[x] && tmp2<mn)) lv[x]=tmp, nx[x]=tb, mn=tmp2;
	}
	for(int ta=a, tb=b, tc=b+1, td=c; tc<td; tc++) if (inField(a,b,c,tc))
	{
		po[x]++; tmp=9; tmp2=0;
		y=GetPID(ta,tb,tc); if ((lv[y]?lv[y]:FieldLevel(ta,tb,tc))<=lv[x]) continue; else tmp=min(tmp,lv[y]), tmp2+=pretty[y];
		y=GetPID(ta,tc,td); if ((lv[y]?lv[y]:FieldLevel(ta,tc,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]), tmp2+=pretty[y];
		y=GetPID(tb,tc,td); if ((lv[y]?lv[y]:FieldLevel(tb,tc,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]), tmp2+=pretty[y];
		tmp2*=0.6; tmp2+=CalPretty(a,b,c,tc); if (tmp>lv[x] || (tmp==lv[x] && tmp2<mn)) lv[x]=tmp, nx[x]=tc, mn=tmp2;
	}
	for(int ta=a, tb=b, tc=c, td=c+1; td<=n; td++) if (inField(a,b,c,td))
	{
		po[x]++; tmp=9; tmp2=0;
		y=GetPID(ta,tb,td); if ((lv[y]?lv[y]:FieldLevel(ta,tb,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]), tmp2+=pretty[y];
		y=GetPID(ta,tc,td); if ((lv[y]?lv[y]:FieldLevel(ta,tc,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]), tmp2+=pretty[y];
		y=GetPID(tb,tc,td); if ((lv[y]?lv[y]:FieldLevel(tb,tc,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]), tmp2+=pretty[y];
		tmp2*=0.6; tmp2+=CalPretty(a,b,c,td); if (tmp>lv[x] || (tmp==lv[x] && tmp2<mn)) lv[x]=tmp, nx[x]=td, mn=tmp2;
	}
	if ((double)(clock()-gap)/CLOCKS_PER_SEC>=0.1)
		system("cls"), printf("%.6lf%%", 100.0*tot/Total), gap=clock();
	pretty[x]=mn; return ++lv[x];
}

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
	if (lv == 1) dt[tot]["color"] = "#0000ff";
	if (lv == 2) dt[tot]["color"] = "#2222ff";
	if (lv == 3) dt[tot]["color"] = "#4444ff";
	if (lv == 4) dt[tot]["color"] = "#6666ff";
	if (lv == 5) dt[tot]["color"] = "#8888ff";
	if (lv == 6) dt[tot]["color"] = "#aaaaff";
	
	dt[tot]["latLngs"][0]["lat"] = D_toString(P[a].x);
	dt[tot]["latLngs"][0]["lng"] = D_toString(P[a].y);
	dt[tot]["latLngs"][1]["lat"] = D_toString(P[b].x);
	dt[tot]["latLngs"][1]["lng"] = D_toString(P[b].y);
	
	dt[tot]["type"] = "polyline";
	
	tot++;
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
	if (a>b) swap(a,b); if (b>c) swap(b,c); if (a>b) swap(a,b);
	if (lv == 1)
	{
		AddLine(a,b,lv), AddLine(b,c,lv), AddLine(c,a,lv);
		pos[a]=(xyz){1,0,0}, AddPortal(a,lv);
		pos[b]=(xyz){0,1,0}, AddPortal(b,lv);
		pos[c]=(xyz){0,0,1}, AddPortal(c,lv);
	}
	int x=GetPID(a,b,c), d=nx[x]; pos[d]=Ave(pos[a],pos[b],pos[c]);
	AddPortal(d,++lv);
	AddLine(a,d,lv), AddLine(b,d,lv), AddLine(c,d,lv);
	OutputPlan(a,b,d,lv);
	OutputPlan(b,c,d,lv);
	OutputPlan(c,a,d,lv);
}

inline void OutputPlan(int pid)
{
	dt=null_dt; bm=ini_bm; tot=0;
	
	rep(i, 1, n) rep(j, i+1, n) if (pid_cal[i][j]<pid && pid-pid_cal[i][j]+j<=n) {OutputPlan(i,j,pid-pid_cal[i][j]+j,1); break;}
	
	Json::FastWriter writer;
	puts("Bookmarks JSON:"); cout << writer.write(bm) << endl;
	puts("DrawTools JSON:"); cout << writer.write(dt) << endl;
}

inline void OutputResult()
{
	freopen("result.txt", "w", stdout);
	
	int tmp=0;
	for(int i=1; i<=n; i++) for(int j=i+1; j<=n; j++) for(int k=j+1; k<=n; k++) if (lv[GetPID(i,j,k)]>=QLevel)
		q[++tmp]=(F){i,j,k,po[GetPID(i,j,k)],FieldS(i,j,k),pretty[GetPID(i,j,k)]+abc(dis(i,j),dis(j,k),dis(k,i))*2};
	
	sort(q+1, q+1+tmp, cmpP);
	rep(i, 1, min(tmp,10)) printf("区域点数最少：#%d\n\n", i), OutputPlan(GetPID(q[i].a,q[i].b,q[i].c));
		
	sort(q+1, q+1+tmp, cmpS);
	rep(i, 1, min(tmp,10)) printf("面积最小：#%d\n\n", i), OutputPlan(GetPID(q[i].a,q[i].b,q[i].c));
	
	sort(q+1, q+1+tmp, cmpG);
	rep(i, 1, min(tmp,10)) printf("长得最正：#%d\n\n", i), OutputPlan(GetPID(q[i].a,q[i].b,q[i].c));
	
	random_shuffle(q+1, q+1+tmp);
	rep(i, 1, min(tmp,10)) printf("Random #%d\n\n", i), OutputPlan(GetPID(q[i].a,q[i].b,q[i].c));
	
	fclose(stdout);
}

int main()
{
	ReadInput("portal.txt"); srand(time(NULL));
	
	int tmp=0; rep(i, 1, n) rep(j, i+1, n) pid_cal[i][j]=tmp, tmp+=n-j; Total=tmp;
	
	gap=clock(); tot=0; rep(i, 1, n) rep(j, i+1, n) rep(k, j+1, n) Count[FieldLevel(i,j,k)]++;
	
	system("cls");
	rep(i, 2, 7) printf("Level %d: %d\n", i, Count[i]); puts(""); puts("");
	printf("请问要来份多大的竹笋？ (3-6)    "); QLevel=1; scanf("%d", &QLevel);
	
	puts("有关信息正在输出到result.txt，祝你好运>.<");
	GetOpinion();
	OutputResult();
	
	return 0;
}