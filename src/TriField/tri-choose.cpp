#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#endif

#include <cstdio>
#include <algorithm>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <json/json.h>

#define rep(i, l, r) for(int i=l; i<=r; i++)
#define dow(i, l, r) for(int i=l; i>=r; i--)
#define clr(x, c) memset(x, c, sizeof(x))
#define all(x) (x).begin,(x).end
#define pb push_back
#define pi acos(-1)

using namespace std;

typedef pair<double,int> Pdi;

#define maxn 409
#define maxs 64000009

#define CHOOSE 0

struct node
{
	double x, y;
	string guid, latlng, label;
} P[1009];

int n, tot, Total, lv[maxs], nx[maxs];

clock_t gap;

Json::Value bm, dt;

inline int min(int a, int b){return a<b?a:b;}

inline double Dir(int a, int b, int c)
{
	return (P[b].x-P[a].x)*(P[c].y-P[a].y)-(P[c].x-P[a].x)*(P[b].y-P[a].y);
}

inline bool inField(int a, int b, int c, int d)
{
	return Dir(a,b,d)*Dir(b,c,d)>0 && Dir(b,c,d)*Dir(c,a,d)>0;
}

inline int GetPID(int a, int b, int c){return ((a-1)*n+b-1)*n+c-1;}

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
	int x=GetPID(a,b,c), tmp;
	if (lv[x]) return lv[x];
	tot++; lv[x]=0;
	rep(i, 1, n) if (i!=a && i!=b && i!=c && inField(a,b,c,i) && (tmp=FieldLevel(b,c,i))>lv[x])
		lv[x]=tmp, nx[x]=i;
	if ((double)(clock()-gap)/CLOCKS_PER_SEC>=0.1)
		system("cls"), printf("%.6lf%%", 100.0*tot/Total), gap=clock();
	return ++lv[x];
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

/* inline void AddField(int a, int b, int c)
{
	dt[tot]["color"] = "#0099FF";

	dt[tot]["latLngs"][0]["lat"] = D_toString(P[a].x);
	dt[tot]["latLngs"][0]["lng"] = D_toString(P[a].y);
	dt[tot]["latLngs"][1]["lat"] = D_toString(P[b].x);
	dt[tot]["latLngs"][1]["lng"] = D_toString(P[b].y);
	dt[tot]["latLngs"][2]["lat"] = D_toString(P[c].x);
	dt[tot]["latLngs"][2]["lng"] = D_toString(P[c].y);

	dt[tot]["type"] = "polygon";

	tot++;
} */

inline void AddField(int a, int b, int c)
{
	dt[tot]["color"] = "#0099FF";

	dt[tot]["latLngs"][0]["lat"] = D_toString(P[a].x);
	dt[tot]["latLngs"][0]["lng"] = D_toString(P[a].y);
	dt[tot]["latLngs"][1]["lat"] = D_toString(P[b].x);
	dt[tot]["latLngs"][1]["lng"] = D_toString(P[b].y);
	dt[tot]["latLngs"][2]["lat"] = D_toString(P[c].x);
	dt[tot]["latLngs"][2]["lng"] = D_toString(P[c].y);
	dt[tot]["latLngs"][3]["lat"] = D_toString(P[a].x);
	dt[tot]["latLngs"][3]["lng"] = D_toString(P[a].y);

	dt[tot]["type"] = "polyline";

	tot++;
}

inline void AddPortal(int a, int dir)
{
	string tmp="A", tmp2=I_toString(rand()*rand()); tmp[0]+=dir;
	bm["portals"][tmp]["bkmrk"][tmp2]["guid"] = P[a].guid;
	bm["portals"][tmp]["bkmrk"][tmp2]["latlng"] = P[a].latlng;
	bm["portals"][tmp]["bkmrk"][tmp2]["label"] = P[a].label;
}

void OutputPlan(int a, int b, int c, int dir)
{
	int x=GetPID(a,b,c);

	AddPortal(a,dir), AddField(a,b,c);

	if (lv[x]==1)
		AddPortal(b,(dir+1)%3), AddPortal(c,(dir+2)%3);
	else
		OutputPlan(b,c,nx[x],(dir+1)%3);
}

inline void OutputResult(int a, int b, int c)
{
	freopen("tri-result.txt", "w", stdout);

	string tmp1="A", tmp2="Group 0";
	rep(i, 1, 3)
	{
		tmp1[0]='A'+i-1, tmp2[6]='0'+i;
		bm["portals"][tmp1]["label"]=tmp2;
		bm["portals"][tmp1]["state"]=0;
	}

	tot=0; OutputPlan(a,b,c,0);

	Json::FastWriter writer;
	puts("Bookmarks JSON:"); cout << writer.write(bm) << endl;
	puts("DrawTools JSON:"); cout << writer.write(dt) << endl;

	printf("可能搞混的重复 Po 名：\n");
	rep(i, 1, n) rep(j, i+1, n) if (P[i].label == P[j].label) cout << P[i].label << endl;

	fclose(stdout);
}

int main()
{
	ReadInput("portal.txt"); srand(time(NULL));

	Total=n*(n-1)*(n-2);

	printf("总点数：%d\n倒数第二个 Portal：", n); cout << P[n-1].label << endl;
	getchar(); system("cls");

	if (n>400)
	{
		puts("点数过多，请删减。");
		getchar(); return 0;
	}

	int Pa, Pb, Pc, mx=0;

	gap=clock(); tot=0;
	rep(i, 1, n) rep(j, 1, n) if (i!=j) rep(k, 1, n) if (k!=i && k!=j) if (mx<FieldLevel(i,j,k))
		Pa=i, Pb=j, Pc=k, mx=FieldLevel(i,j,k);

	OutputResult(Pa, Pb, Pc);

	return 0;
}
