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

string Name[1009];

int n, tot, Total, mxLevel, lb_cal[maxn][maxn], lv[maxs], nx[maxs], Count[11];

clock_t gap;

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
	if (v == mxLevel) return;
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
	int x=GetLabel(a,b,c), tmp;
	if (lv[x]) return lv[x]; tot++; 
	rep(i, 1, n) if (i!=a && i!=b && i!=c && inField(a,b,c,i) && (tmp=min(FieldLevel(a,b,i), min(FieldLevel(b,c,i), FieldLevel(c,a,i))))>lv[x])
		lv[x]=tmp, nx[x]=i;
	if ((double)(clock()-gap)/CLOCKS_PER_SEC>=0.1)
		system("cls"), printf("%.6lf%%", 100.0*tot/Total), gap=clock();
	return ++lv[x];
}

int main()
{
	ReadInput("portal.txt");
	
	tot=0; rep(i, 1, n) rep(j, i+1, n) lb_cal[i][j]=tot, tot+=n-j; Total=tot;
	
	gap=clock(); tot=0; rep(i, 1, n) rep(j, i+1, n) rep(k, j+1, n) Count[FieldLevel(i,j,k)]++;
	
	freopen("result.txt", "w", stdout);
	
	printf("Perfect Field:\n");
	rep(i, 2, 9) printf("%d: %d\n", i, Count[i]); 
	puts("");
	
	Json::Value ret;
	rep(i, 2, 9) if (Count[i]) mxLevel=i;
	bool fg=true; tot=0;
	for(int i=1; i<=n&&fg; i++) 
		for(int j=i+1; j<=n&&fg; j++) 
			for(int k=j+1; k<=n&&fg; k++)
				if (lv[GetLabel(i,j,k)]==mxLevel && fg) Output(ret,i,j,k,1), fg=false;
	
	Json::FastWriter writer;
	//Json::StyledWriter writer;
	cout << writer.write(ret) << endl;
	
	fclose(stdout);
	
	return 0;
}