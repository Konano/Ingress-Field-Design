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

struct node{double x,y;} Coo[1009];

struct F{int a,b,c,P; double S;} q[maxs];
bool cmpP(F a, F b){return a.P<b.P;}
bool cmpS(F a, F b){return a.S<b.S;}

string Name[1009];

int n, tot, Total, QLevel, lb_cal[maxn][maxn], lv[maxs], nx[maxs], po[maxs], Count[11];

clock_t gap;

inline int min(int a, int b){return a<b?a:b;}

inline void toDouble(string str)
{
	istringstream iss(str);
	char tmp;
	iss >> Coo[n].x >> tmp >> Coo[n].y;
}

inline string toString(double a)
{
	char buffer[20];
	sprintf(buffer, "%f", a);
	return buffer;
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
		toDouble(input["portals"]["idOthers"]["bkmrk"][*iter]["latlng"].asString());
		Name[n] = input["portals"]["idOthers"]["bkmrk"][*iter]["label"].asString();
	}
}

inline void AddLine(Json::Value &ret, int a, int b, int v)
{
	if (v == 1) ret[tot]["color"] = "#0000ff";
	if (v == 2) ret[tot]["color"] = "#2222ff";
	if (v == 3) ret[tot]["color"] = "#4444ff";
	if (v == 4) ret[tot]["color"] = "#6666ff";
	if (v == 5) ret[tot]["color"] = "#8888ff";
	if (v == 6) ret[tot]["color"] = "#aaaaff";
	
	ret[tot]["latLngs"][0]["lat"] = toString(Coo[a].x);
	ret[tot]["latLngs"][0]["lng"] = toString(Coo[a].y);
	ret[tot]["latLngs"][1]["lat"] = toString(Coo[b].x);
	ret[tot]["latLngs"][1]["lng"] = toString(Coo[b].y);
	
	ret[tot]["type"] = "polyline";
	
	tot++;
}

inline double FieldS(int a, int b, int c)
{
	return fabs(Coo[a].x*Coo[b].y+Coo[b].x*Coo[c].y+Coo[c].x*Coo[a].y - Coo[a].y*Coo[b].x-Coo[b].y*Coo[c].x-Coo[c].y*Coo[a].x)/2;
}

inline bool Dir(int a, int b, int c)
{
	return (Coo[b].x-Coo[a].x)*(Coo[c].y-Coo[a].y)<(Coo[c].x-Coo[a].x)*(Coo[b].y-Coo[a].y);
}

inline bool inField(int a, int b, int c, int d)
{
	return Dir(a,b,d) == Dir(b,c,d) && Dir(b,c,d) == Dir(c,a,d);
}

inline int GetLabel(int a, int b, int c){return lb_cal[a][b]+c-b;}

void Output(Json::Value &ret, int a, int b, int c, int v)
{
	if (v == QLevel) return;
	if (v == 1) AddLine(ret,a,b,v), AddLine(ret,b,c,v), AddLine(ret,c,a,v);
	if (a>b) swap(a,b); if (b>c) swap(b,c); if (a>b) swap(a,b);
	int x=GetLabel(a,b,c), d=nx[x]; v++;
	AddLine(ret,a,d,v), AddLine(ret,b,d,v), AddLine(ret,c,d,v);
	Output(ret,a,b,d,v);
	Output(ret,b,c,d,v);
	Output(ret,c,a,d,v);
}

int FieldLevel(int a, int b, int c)
{
	if (a>b) swap(a,b); if (b>c) swap(b,c); if (a>b) swap(a,b);
	int x=GetLabel(a,b,c), tmp, y;
	if (lv[x]) return lv[x]; tot++;
	for(int ta=1, tb=a, tc=b, td=c; ta<tb; ta++) if (inField(a,b,c,ta))
	{
		po[x]++; tmp=9;
		y=GetLabel(ta,tb,tc); if ((lv[y]?lv[y]:FieldLevel(ta,tb,tc))<=lv[x]) continue; else tmp=min(tmp,lv[y]);
		y=GetLabel(ta,tb,td); if ((lv[y]?lv[y]:FieldLevel(ta,tb,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]);
		y=GetLabel(ta,tc,td); if ((lv[y]?lv[y]:FieldLevel(ta,tc,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]);
		//y=GetLabel(tb,tc,td); if ((lv[y]?lv[y]:FieldLevel(tb,tc,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]);
		lv[x]=tmp, nx[x]=ta;
	}
	for(int ta=a, tb=a+1, tc=b, td=c; tb<tc; tb++) if (inField(a,b,c,tb))
	{
		po[x]++; tmp=9;
		y=GetLabel(ta,tb,tc); if ((lv[y]?lv[y]:FieldLevel(ta,tb,tc))<=lv[x]) continue; else tmp=min(tmp,lv[y]);
		y=GetLabel(ta,tb,td); if ((lv[y]?lv[y]:FieldLevel(ta,tb,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]);
		//y=GetLabel(ta,tc,td); if ((lv[y]?lv[y]:FieldLevel(ta,tc,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]);
		y=GetLabel(tb,tc,td); if ((lv[y]?lv[y]:FieldLevel(tb,tc,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]);
		lv[x]=tmp, nx[x]=tb;
	}
	for(int ta=a, tb=b, tc=b+1, td=c; tc<td; tc++) if (inField(a,b,c,tc))
	{
		po[x]++; tmp=9;
		y=GetLabel(ta,tb,tc); if ((lv[y]?lv[y]:FieldLevel(ta,tb,tc))<=lv[x]) continue; else tmp=min(tmp,lv[y]);
		//y=GetLabel(ta,tb,td); if ((lv[y]?lv[y]:FieldLevel(ta,tb,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]);
		y=GetLabel(ta,tc,td); if ((lv[y]?lv[y]:FieldLevel(ta,tc,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]);
		y=GetLabel(tb,tc,td); if ((lv[y]?lv[y]:FieldLevel(tb,tc,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]);
		lv[x]=tmp, nx[x]=tc;
	}
	for(int ta=a, tb=b, tc=c, td=c+1; td<=n; td++) if (inField(a,b,c,td))
	{
		po[x]++; tmp=9;
		//y=GetLabel(ta,tb,tc); if ((lv[y]?lv[y]:FieldLevel(ta,tb,tc))<=lv[x]) continue; else tmp=min(tmp,lv[y]);
		y=GetLabel(ta,tb,td); if ((lv[y]?lv[y]:FieldLevel(ta,tb,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]);
		y=GetLabel(ta,tc,td); if ((lv[y]?lv[y]:FieldLevel(ta,tc,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]);
		y=GetLabel(tb,tc,td); if ((lv[y]?lv[y]:FieldLevel(tb,tc,td))<=lv[x]) continue; else tmp=min(tmp,lv[y]);
		lv[x]=tmp, nx[x]=td;
	}
	if ((double)(clock()-gap)/CLOCKS_PER_SEC>=0.1)
		system("cls"), printf("%.6lf%%", 100.0*tot/Total), gap=clock();
	return ++lv[x];
}

int main()
{
	ReadInput("portal.txt");
	
	tot=0; rep(i, 1, n) rep(j, i+1, n) lb_cal[i][j]=tot, tot+=n-j; Total=tot;
	
	gap=clock(); tot=0; rep(i, 1, n) rep(j, i+1, n) rep(k, j+1, n) Count[FieldLevel(i,j,k)]++;
	
	system("cls");
	
	rep(i, 2, 9) printf("Level %d: %d\n", i, Count[i]); 
	puts(""); puts("");
	
	printf("Field Level you need? (3-6)		"); QLevel=1; scanf("%d", &QLevel);
	
	freopen("result.txt", "w", stdout);
	
	int tmp=0;
	for(int i=1; i<=n; i++) for(int j=i+1; j<=n; j++) for(int k=j+1; k<=n; k++) if (lv[GetLabel(i,j,k)]>=QLevel)
		q[++tmp]=(F){i,j,k,po[GetLabel(i,j,k)],FieldS(i,j,k)};
	
	sort(q+1, q+1+tmp, cmpP);
	rep(i, 1, min(tmp,10))
	{
		printf("#%d		P:%d\n\n", i, q[i].P);
		Json::Value ret; tot=0;
		Output(ret,q[i].a,q[i].b,q[i].c,1);
		
		Json::FastWriter writer;
		//Json::StyledWriter writer;
		cout << writer.write(ret) << endl << endl;
	}
	
	sort(q+1, q+1+tmp, cmpS);
	rep(i, 1, min(tmp,10))
	{
		printf("#%d		S:%.10lf\n\n", i, q[i].S);
		Json::Value ret; tot=0;
		Output(ret,q[i].a,q[i].b,q[i].c,1);
		
		Json::FastWriter writer;
		//Json::StyledWriter writer;
		cout << writer.write(ret) << endl << endl;
	}
	
	fclose(stdout);
	
	return 0;
}