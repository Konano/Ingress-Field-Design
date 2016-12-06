#include <cstdio>
#include <algorithm>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include "jsoncpp/json.h"

#define rep(i, l, r) for(int i=l; i<=r; i++)
#define dow(i, l, r) for(int i=l; i>=r; i--)
#define clr(x, c) memset(x, c, sizeof(x))
#define all(x) (x).begin,(x).end
#define pb push_back
#define pi acos(-1)

using namespace std;

typedef pair<double,int> Pdi;

#define maxn 309

struct node{double x,y;} Coo[maxn], q[maxn<<2];

string Name[maxn];

int n, Ltot, mxField, lb[maxn][maxn][maxn], nx[maxn][maxn][maxn], Count[9];

inline void toDouble(string str)
{
	istringstream iss(str);
	char tmp;
	iss >> Coo[n].x >> tmp >> Coo[n].y;
}

inline void ReadInput(const char *localFileName)
{
	// 读入JSON
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

/* inline void AddtoField(int a, int b, int c, int v)
{
	if (a>b) swap(a,b); if (b>c) swap(b,c); if (a>b) swap(a,b);
	num[a][b][c]+=v;
} */

inline bool Dir(int a, int b, int c)
{
	return (Coo[b].x-Coo[a].x)*(Coo[c].y-Coo[a].y)<(Coo[c].x-Coo[a].x)*(Coo[b].y-Coo[a].y);
}

inline bool inField(int a, int b, int c, int d)
{
	return Dir(a,b,d) == Dir(b,c,d) && Dir(b,c,d) == Dir(c,a,d);
}

inline int FieldLabel(int a, int b, int c)
{
	if (a>b) swap(a,b); if (b>c) swap(b,c); if (a>b) swap(a,b);
	return lb[a][b][c];
}

inline void FieldUp(int a, int b, int c, int d)
{
	if (a>b) swap(a,b); if (b>c) swap(b,c); if (a>b) swap(a,b);
	lb[a][b][c]++, nx[a][b][c]=d;
}

inline string toString(double a)
{
	char buffer[20];
	sprintf(buffer, "%f", a);
	return buffer;
}

inline void AddLine(Json::Value &ret, int a, int b, int v)
{
	if (v == 1) ret[Ltot]["color"] = "#0000ff";
	if (v == 2) ret[Ltot]["color"] = "#2222ff";
	if (v == 3) ret[Ltot]["color"] = "#4444ff";
	if (v == 4) ret[Ltot]["color"] = "#6666ff";
	if (v == 5) ret[Ltot]["color"] = "#8888ff";
	if (v == 6) ret[Ltot]["color"] = "#aaaaff";
	
	ret[Ltot]["latLngs"][0]["lat"] = toString(Coo[a].x);
	ret[Ltot]["latLngs"][0]["lng"] = toString(Coo[a].y);
	ret[Ltot]["latLngs"][1]["lat"] = toString(Coo[b].x);
	ret[Ltot]["latLngs"][1]["lng"] = toString(Coo[b].y);
	
	ret[Ltot]["type"] = "polyline";
	
	Ltot++;
}

void dfs(Json::Value &ret, int a, int b, int c, int v)
{
	if (v == mxField) return;
	if (v == 1) AddLine(ret,a,b,v), AddLine(ret,b,c,v), AddLine(ret,c,a,v);
	if (a>b) swap(a,b); if (b>c) swap(b,c); if (a>b) swap(a,b);
	int d=nx[a][b][c]; v++;
	AddLine(ret,a,d,v), AddLine(ret,b,d,v), AddLine(ret,c,d,v);
	dfs(ret,a,b,d,v);
	dfs(ret,b,c,d,v);
	dfs(ret,c,a,d,v);
}

inline void Init(){n=0; mxField=0; Ltot=0;}

int main()
{
	Init();
	ReadInput("portal.txt");
	
	// 查找三点共线
	
/*	// 计算三角形内点数 O(n^3)
 	rep(i, 1, n)
	{
		int tot=0;
		rep(j, 1, n) if (j!=i) q[++tot]=Pdi(atan2(Coo[j].y-Coo[i].y, Coo[j].x-Coo[i].x), j);
		sort(q+1, q+1+tot, cmp);
		int st=1; rep(ed, 1, tot*2)
		{
			while ((st<=tot?q[st].fi:q[st].fi+pi*2)+pi < (ed<=tot?q[ed].fi:q[ed].fi+pi*2)) st++;
			rep(j, st, ed-1) AddtoField(i, q[ed].se, q[j].se, ed-1-j);
		}
		rep(j, 1, n) if (j!=i) q[++tot]=Pdi(atan2(Coo[i].y-Coo[j].y, Coo[i].x-Coo[j].x), -j);
		sort(q+1, q+1+tot, cmp);
		st=1; int sz=0; rep(ed, 1, tot*2) if (q[ed].se<0)
		{
			while ((st<=tot?q[st].fi:q[st].fi+pi*2)+pi < (ed<=tot?q[ed].fi:q[ed].fi+pi*2)) sz-=(q[st].se<0?1:0), st++;
			int tmp=sz;
			rep(j, st, ed-1) if (q[j].se<0) tmp--, AddtoField(i, -q[ed].se, -q[j].se, ed-1-j-tmp);
			sz++;
		}
	}
	rep(i, 1, n) rep(j, i+1, n) rep(k, j+1, n) num[i][j][k]-=(n-3), num[i][j][k]/=2, b[i][j][k]=1; */
	
	rep(i, 1, n) rep(j, i+1, n) rep(k, j+1, n) lb[i][j][k]=1;
	
	rep(o, 1, 6)
	{
		bool fg = false;
		rep(i, 1, n) rep(j, i+1, n) rep(k, j+1, n) if (FieldLabel(i,j,k)==o)
		{
			Count[o]++;
			rep(h, 1, n) if (h!=i && h!=j && h!=k && FieldLabel(i,j,h)==o && FieldLabel(j,k,h)==o && FieldLabel(k,i,h)==o)
			{
				if (inField(i,j,h,k)) FieldUp(i,j,h,k), fg=true;
				else if (inField(j,k,h,i)) FieldUp(j,k,h,i), fg=true;
				else if (inField(k,i,h,j)) FieldUp(k,i,h,j), fg=true;
			}
		}
		if (!fg) break;
	}
	
	freopen("result.txt", "w", stdout);
	
	printf("Perfect Field:\n");
	rep(i, 2, 6) printf("%d: %d\n", i, Count[i]); 
	puts("");
	
	Json::Value ret;
	rep(i, 2, 6) if (Count[i]) mxField=i;
	bool fg=true;
	for(int i=1; i<=n&&fg; i++) 
		for(int j=i+1; j<=n&&fg; j++) 
			for(int k=j+1; k<=n&&fg; k++)
				if (lb[i][j][k]==mxField && fg) dfs(ret,i,j,k,1), fg=false;
	
	Json::FastWriter writer;
	//Json::StyledWriter writer;
	cout << writer.write(ret) << endl;
	
	fclose(stdout);
	
	return 0;
}