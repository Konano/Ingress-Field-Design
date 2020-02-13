#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#endif

#include <cstdio>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <iomanip>
#include <json/json.h>

#define rep(i, l, r) for(int i=l; i<=r; i++)
#define dow(i, l, r) for(int i=l; i>=r; i--)
#define clr(x, c) memset(x, c, sizeof(x))
#define all(a) a.begin(), a.end()
#define pb push_back

using namespace std;

struct node
{
	double x, y;
	string guid, latlng, label;
} P[500];

inline string toString(int a, int b)
{
	char buffer[6];
	sprintf(buffer, "%c%d", 'A'+a-1, b);
	return buffer;
}

vector<int>v[500];
inline void Add(int a, int b){v[a].pb(b); v[b].pb(a);}

int n, Lv[500];
bool InBook[500];
map<int,string> M0;
map<string,int> M1;

inline void GetOpinion()
{
	M0[++n]="A1", M1["A1"]=n, Lv[n]=1;
	M0[++n]="A2", M1["A2"]=n, Lv[n]=1;
	M0[++n]="A3", M1["A3"]=n, Lv[n]=1;
	Add(1,2); Add(2,3); Add(1,3);

	ifstream fin("../LABEL");
	int tmp, num=0; fin >> tmp; for(int i=1, a=1; i<tmp; i++) num+=a, a*=3;

	string a, b, c, d;
	rep(i, 1, num)
	{
		fin >> a >> b >> c >> d;
		M0[++n]=a, M1[a]=n, Lv[n]=max(max(Lv[M1[b]],Lv[M1[c]]),Lv[M1[d]])+1;
		Add(n,M1[b]);
		Add(n,M1[c]);
		Add(n,M1[d]);
	}
}

inline void ChangetoPosition(int a)
{
	istringstream iss(P[a].latlng);
	char tmp;
	iss >> P[a].x >> tmp >> P[a].y;
}

inline void ReadBookmark(const char *localFileName) // 读入JSON
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

	Json::Value::Members arrayMember1 = input["portals"].getMemberNames();
	for(Json::Value::Members::iterator iter1 = arrayMember1.begin(); iter1 != arrayMember1.end(); ++iter1)
	{
		Json::Value::Members arrayMember = input["portals"][*iter1]["bkmrk"].getMemberNames();
		for(Json::Value::Members::iterator iter = arrayMember.begin(); iter != arrayMember.end(); ++iter)
		{
			P[M1[*iter]].guid = input["portals"][*iter1]["bkmrk"][*iter]["latlng"].asString();
			P[M1[*iter]].latlng = input["portals"][*iter1]["bkmrk"][*iter]["latlng"].asString();
			P[M1[*iter]].label = input["portals"][*iter1]["bkmrk"][*iter]["label"].asString();
			ChangetoPosition(M1[*iter]);
			InBook[M1[*iter]]=true;
		}
	}
}

inline string LayerName(int x)
{
	if (x==0) return "main";
	if (x==1) return "groupa";
	if (x==2) return "groupb";
	if (x==3) return "groupc";
	if (x==4) return "groupd";
	if (x==5) return "groupe";
	if (x==6) return "groupf";
	return "";
}

inline int Step(int a, int b)
{
	if (Lv[a]==5 && Lv[b]==6) return 1;

	if (Lv[a]==4 && Lv[b]==5) return 1;
	if (Lv[a]==4 && Lv[b]==6) return 1;
	if (Lv[a]==4 && Lv[b]==2) return 1;

	if (Lv[a]==3 && Lv[b]==4) return 2;
	if (Lv[a]==3 && Lv[b]==5) return 2;
	if (Lv[a]==3 && Lv[b]==6) return 2;
	if (Lv[a]==3 && Lv[b]==2) return 2;

	if (Lv[a]==2 && Lv[b]==5) return 3;
	if (Lv[a]==2 && Lv[b]==6) return 3;

	if (Lv[a]==1 && Lv[b]==1 && (a+1==b || a-2==b)) return 4;
	if (Lv[a]==1 && Lv[b]==2) return 4;
	if (Lv[a]==1 && Lv[b]==3) return 4;
	if (Lv[a]==1 && Lv[b]==4) return 4;
	if (Lv[a]==1 && Lv[b]==5) return 4;
	if (Lv[a]==1 && Lv[b]==6) return 4;
	return -1;
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

Json::Value dt[19], wue; // JSON 信息载体
int pt_num, lk_num, dt_num=4, dtn[19];

inline void AddLine(int o, int a, int b)
{
	dt[o][dtn[o]]["color"] = "#0000ff";

	dt[o][dtn[o]]["latLngs"][0]["lat"] = D_toString(P[a].x);
	dt[o][dtn[o]]["latLngs"][0]["lng"] = D_toString(P[a].y);
	dt[o][dtn[o]]["latLngs"][1]["lat"] = D_toString(P[b].x);
	dt[o][dtn[o]]["latLngs"][1]["lng"] = D_toString(P[b].y);

	dt[o][dtn[o]]["type"] = "polyline";

	dtn[o]++;
}

inline void OutputRESWUE()
{
	rep(x, 1, n) if (InBook[x])
	{
		wue["Portals"][pt_num]["id"] = I_toString(x);
		wue["Portals"][pt_num]["guid"] = P[x].guid;
		wue["Portals"][pt_num]["type"] = LayerName(Lv[x]);
		wue["Portals"][pt_num]["nodeName"] = P[x].label;
		wue["Portals"][pt_num]["lat"] = D_toString(P[x].x);
		wue["Portals"][pt_num]["lng"] = D_toString(P[x].y);
		pt_num++;
	}
	rep(x, 1, n) if (InBook[x]) rep(i, 0, (int)v[x].size()-1)
		if (InBook[v[x][i]] && Step(x,v[x][i])>=0)
	{
		wue["Links"][lk_num]["id"] = I_toString(lk_num+1);
		wue["Links"][lk_num]["portal1"] = I_toString(x);
		wue["Links"][lk_num]["portal2"] = I_toString(v[x][i]);
		wue["Links"][lk_num]["portal1Guid"] = P[x].guid;
		wue["Links"][lk_num]["portal2Guid"] = P[v[x][i]].guid;
		wue["Links"][lk_num]["portal1Lat"] = D_toString(P[x].x);
		wue["Links"][lk_num]["portal1Lng"] = D_toString(P[x].y);
		wue["Links"][lk_num]["portal2Lat"] = D_toString(P[v[x][i]].x);
		wue["Links"][lk_num]["portal2Lng"] = D_toString(P[v[x][i]].y);
		wue["Links"][lk_num]["type"] = LayerName(Step(x,v[x][i]));
		lk_num++;

		rep(o, Step(x,v[x][i]), dt_num) AddLine(o, x, v[x][i]);
	}

	freopen("reswue.txt", "w", stdout);

	Json::FastWriter writer;
	cout << writer.write(wue) << endl;
	rep(i, 1, dt_num) cout << writer.write(dt[i]) << endl;

	fclose(stdout);
}

int main()
{
	GetOpinion();
	ReadBookmark("bookmark.txt");
	OutputRESWUE();
	return 0;
}
