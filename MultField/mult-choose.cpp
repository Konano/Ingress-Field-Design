#include <cstdio>
#include <algorithm>
#include <sstream>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include "../jsoncpp/json.h"

#define rep(i, l, r) for(int i=l; i<=r; i++)
#define dow(i, l, r) for(int i=l; i>=r; i--)
#define clr(x, c) memset(x, c, sizeof(x))
#define pi acos(-1)
#define travel(i) for(edge *p=fir[i]; p; p=p->n)
#define fi first
#define se second

using namespace std;

typedef pair<double,int> Pdi;
typedef long double ld;

#define maxn 1009

struct Portal
{
	double x0, y0, x, y, z, S;
	string guid, latlng, label;
} P[maxn], G[9];

bool cmpS(Portal a, Portal b){return a.S<b.S;}






int n, An, Bn, Ans;





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

	Json::Value::Members arrayMember = input["portals"]["idA"]["bkmrk"].getMemberNames();
	for(Json::Value::Members::iterator iter = arrayMember.begin(); iter != arrayMember.end(); ++iter)
	{
		n++; An++;
		P[n].guid = input["portals"]["idA"]["bkmrk"][*iter]["guid"].asString();
		P[n].latlng = input["portals"]["idA"]["bkmrk"][*iter]["latlng"].asString();
		P[n].label = input["portals"]["idA"]["bkmrk"][*iter]["label"].asString();
		ChangetoPosition(n);
	}

	arrayMember = input["portals"]["idB"]["bkmrk"].getMemberNames();
	for(Json::Value::Members::iterator iter = arrayMember.begin(); iter != arrayMember.end(); ++iter)
	{
		n++; Bn++;
		P[n].guid = input["portals"]["idB"]["bkmrk"][*iter]["guid"].asString();
		P[n].latlng = input["portals"]["idB"]["bkmrk"][*iter]["latlng"].asString();
		P[n].label = input["portals"]["idB"]["bkmrk"][*iter]["label"].asString();
		ChangetoPosition(n);
	}

	arrayMember = input["portals"]["idOthers"]["bkmrk"].getMemberNames();
	for(Json::Value::Members::iterator iter = arrayMember.begin(); iter != arrayMember.end(); ++iter)
	{
		n++;
		P[n].guid = input["portals"]["idOthers"]["bkmrk"][*iter]["guid"].asString();
		P[n].latlng = input["portals"]["idOthers"]["bkmrk"][*iter]["latlng"].asString();
		P[n].label = input["portals"]["idOthers"]["bkmrk"][*iter]["label"].asString();
		ChangetoPosition(n);
	}
}











Json::Value bm, dt;








int Ltot, Ptot;

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

inline void AddLine(int a, int b)
{
	dt[Ltot]["latLngs"][0]["lat"] = D_toString(P[a].x0);
	dt[Ltot]["latLngs"][0]["lng"] = D_toString(P[a].y0);
	dt[Ltot]["latLngs"][1]["lat"] = D_toString(P[b].x0);
	dt[Ltot]["latLngs"][1]["lng"] = D_toString(P[b].y0);
	dt[Ltot]["color"] = "#0099FF";
	dt[Ltot]["type"] = "polyline";

	Ltot++;
}

inline void AddPortal(int a)
{
	bm["portals"]["idOthers"]["bkmrk"][I_toString(Ptot)]["guid"] = P[a].guid;
	bm["portals"]["idOthers"]["bkmrk"][I_toString(Ptot)]["latlng"] = P[a].latlng;
	bm["portals"]["idOthers"]["bkmrk"][I_toString(Ptot)]["label"] = P[a].label;

	Ptot++;
}





//int f[maxn][maxn][maxn], g[maxn][maxn];

int f[maxn], nx[maxn];






/* inline void OutputResult()
{
	freopen("mult-result.txt", "w", stdout);

	rep(i, 1, n) rep(j, i+1, n) if (A==g[i][j]+g[j][i])
	{
		AddLine(i,j); AddPortal(i); AddPortal(j);

		rep(o, 1, n) if (f[i][j][o]==g[i][j])
		{
			AddPortal(o), AddLine(i,o), AddLine(j,o);
			while (f[i][j][o]!=1)
				AddLine(o,-f[j][i][o]), o=-f[j][i][o], AddPortal(o), AddLine(i,o), AddLine(j,o);
			break;
		}

		rep(o, 1, n) if (f[j][i][o]==g[j][i])
		{
			AddPortal(o), AddLine(i,o), AddLine(j,o);
			while (f[j][i][o]!=1)
				AddLine(o,-f[i][j][o]), o=-f[i][j][o], AddPortal(o), AddLine(i,o), AddLine(j,o);
			break;
		}

		Json::FastWriter writer;
		puts("Bookmarks JSON:"); cout << writer.write(bm) << endl;
		puts("DrawTools JSON:"); cout << writer.write(dt) << endl;

		break;
	}

	Json::FastWriter writer;
	puts("Bookmarks JSON:"); cout << writer.write(bm) << endl;
	puts("DrawTools JSON:"); cout << writer.write(dt) << endl;

	fclose(stdout);
} */






Pdi q[maxn]; int tot;





int main()
{
	ReadInput("portal.txt");

	system("cls"); printf("总点数：%d\n倒数第二个 Portal：", n); cout << P[n-1].label << endl;
	getchar();

	if (n>maxn-9)
	{
		puts("点数过多，请删减。");
		getchar(); return 0;
	}

	/* int All=n*(n-1), now=0; clock_t gap=clock();

	rep(i, 1, n) rep(j, 1, n) if (i!=j)
	{
		tot=0;
		rep(o, 1, n) if (i!=o && j!=o && Left(i,j,o)) tot++, q[tot]=Pdi(Area(i,j,o),o);
		sort(q+1, q+1+tot);
		rep(o, 1, tot)
		{
			f[i][j][q[o].se]=1;
			rep(k, 1, o-1) if (inField(i,j,q[o].se,q[k].se) && f[i][j][q[o].se]<f[i][j][q[k].se]+1)
				f[i][j][q[o].se]=f[i][j][q[k].se]+1, f[j][i][q[o].se]=-q[k].se;
			g[i][j]=max(g[i][j], f[i][j][q[o].se]);
		}

		now++;
		if ((double)(clock()-gap)/CLOCKS_PER_SEC>=0.5)
			system("cls"), printf("%.6lf%%", 100.0*now/All), gap=clock();
	}

	rep(i, 1, n) rep(j, i+1, n) A=max(A, g[i][j]+g[j][i]); */

	freopen("mult-result.txt", "w", stdout); AddLine(1,An+1);

	tot=0; rep(o, An+Bn+1, n) if (Left(1,An+1,o)) q[++tot]=Pdi(Area(1,An+1,o),o);
	if (tot)
	{
		sort(q+1, q+1+tot);
		rep(o, 1, tot)
		{
			f[q[o].se]=1;
			rep(k, 1, o-1) if (f[q[o].se]<f[q[k].se]+1)
			{
				bool fg=true;
				//rep(j, An+1, An+Bn) if (Angle(j,q[o].se,q[k].se)>Angle(q[o].se,q[k].se,j)) {fg=false; goto BK;}
				rep(i, 1, An) rep(j, An+1, An+Bn) if (!inField(i,j,q[o].se,q[k].se)) {fg=false; goto BK;}
				BK:if (fg) f[q[o].se]=f[q[k].se]+1, nx[q[o].se]=q[k].se;
			}
			Ans=max(Ans, f[q[o].se]);
		}

		rep(o, 1, n) if (f[o]==Ans)
		{
			AddPortal(o), AddLine(1,o), AddLine(An+1,o);
			while (f[o]!=1)
				AddLine(o,nx[o]), o=nx[o], AddPortal(o), AddLine(1,o), AddLine(An+1,o);
			break;
		}
	}

	tot=Ans=0; clr(f,0); rep(o, An+Bn+1, n) if (Left(An+1,1,o)) q[++tot]=Pdi(Area(1,An+1,o),o);
	if (tot)
	{
		sort(q+1, q+1+tot);
		rep(o, 1, tot)
		{
			f[q[o].se]=1;
			rep(k, 1, o-1) if (f[q[o].se]<f[q[k].se]+1)
			{
				bool fg=true;
				//rep(j, An+1, An+Bn) if (Angle(j,q[o].se,q[k].se)>Angle(q[o].se,q[k].se,j)) {fg=false; goto BK2;}
				rep(i, 1, An) rep(j, An+1, An+Bn) if (!inField(i,j,q[o].se,q[k].se)) {fg=false; goto BK2;}
				BK2:if (fg) f[q[o].se]=f[q[k].se]+1, nx[q[o].se]=q[k].se;
			}
			Ans=max(Ans, f[q[o].se]);
		}

		rep(o, 1, n) if (f[o]==Ans)
		{
			AddPortal(o), AddLine(1,o), AddLine(An+1,o);
			while (f[o]!=1)
				AddLine(o,nx[o]), o=nx[o], AddPortal(o), AddLine(1,o), AddLine(An+1,o);
			break;
		}
	}

	Json::FastWriter writer;
	puts("Bookmarks JSON:"); cout << writer.write(bm) << endl;
	puts("DrawTools JSON:"); cout << writer.write(dt) << endl;

	fclose(stdout);

	//OutputResult();

	return 0;
}






