#include <cstdio>
#include <algorithm>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <map>
#include "jsoncpp/json.h"

#define rep(i, l, r) for(int i=l; i<=r; i++)
#define dow(i, l, r) for(int i=l; i>=r; i--)
#define clr(x, c) memset(x, c, sizeof(x))
#define all(x) (x).begin,(x).end
#define pb push_back
#define pi acos(-1)

using namespace std;

struct node
{
	double x, y;
	string latlng, label;
} P[500];

inline string toString(int a, int b)
{
	char buffer[6];
	sprintf(buffer, "%c%d", 'A'+a-1, b);
	return buffer;
}

int LB[500][500], F[500][3], LV, n;
bool vis[500], InBook[500];
map<int,string> M0;
map<string,int> M1;

inline void GetOpinion()
{
	int tot=0;
	M0[++n]="A1", M1["A1"]=n;
	M0[++n]="A2", M1["A2"]=n;
	M0[++n]="A3", M1["A3"]=n;
	LB[1][2]=++tot, LB[2][3]=++tot, LB[3][1]=++tot;
	
	ifstream fin("label.txt"); 
	int tmp, num=0; fin >> tmp; for(int i=1, a=1; i<tmp; i++) num+=a, a*=3;
	
	string a, b, c, d;
	rep(i, 1, num) 
	{
		fin >> a >> b >> c >> d;
		M0[++n]=a, M1[a]=n;
		LB[n][M1[b]]=++tot, LB[n][M1[c]]=++tot, LB[n][M1[d]]=++tot;
		F[n][0]=M1[b], F[n][1]=M1[c], F[n][2]=M1[d];
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
		LV++;
		Json::Value::Members arrayMember = input["portals"][*iter1]["bkmrk"].getMemberNames();
		for(Json::Value::Members::iterator iter = arrayMember.begin(); iter != arrayMember.end(); ++iter)
		{
			P[M1[*iter]].latlng = input["portals"][*iter1]["bkmrk"][*iter]["latlng"].asString();
			P[M1[*iter]].label = input["portals"][*iter1]["bkmrk"][*iter]["label"].asString();
			ChangetoPosition(M1[*iter]);
			InBook[M1[*iter]]=true;
		}
	}
}

inline int FindLB(double x, double y)
{
	rep(i, 1, n) if (fabs(x-P[i].x)<1e-7 && fabs(y-P[i].y)<1e-7) return i; return 0;
}

inline void ReadWay(const char *localFileName) // 读入JSON
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
	
	freopen("act.txt", "w", stdout);
	
	rep(i, 1, n) if (InBook[i]) cout << M0[i] << '\t' << P[i].label << '\t' << P[i].latlng << endl;
	cout << '\n';
	int sz=input[0]["latLngs"].size();
	rep(i, 1, sz)
	{
		int x=FindLB(input[0]["latLngs"][i-1]["lat"].asDouble(), input[0]["latLngs"][i-1]["lng"].asDouble());
		int key=0, out=0; rep(y, 1, n) if (InBook[y] && LB[x][y] && !vis[y]) key++; else if (InBook[y] && LB[x][y]) out++;
		cout << M0[x] << '\t' << P[x].label << '\t' << key << '\t' << out << '\t';
		rep(y, 1, n) if (InBook[y] && LB[x][y] && vis[y]) cout << M0[y] << ' ';
		cout << endl;
		
		if (vis[F[x][0]] && vis[F[x][1]] && vis[F[x][2]])
		{
			cout << "啊路线出现问题，需要修改（" << endl;
			cout << "出错信息：" << M0[x] << ' ' << M0[F[x][0]] << ' ' << M0[F[x][1]] << ' ' << M0[F[x][2]] << endl;
			return;
		}
		vis[x]=1;
	}
}

int main()
{
	GetOpinion();
	rep(i, 1, n) rep(j, i+1, n) if (LB[i][j]!=LB[j][i]) LB[i][j]=LB[j][i]=max(LB[i][j],LB[j][i]);
	
	ReadBookmark("bookmark.txt");
	ReadWay("way.txt");
	return 0;
}