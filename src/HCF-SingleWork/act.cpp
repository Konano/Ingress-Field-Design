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
#include <map>
#include <cmath>
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
	string latlng, label;
} P[500];

inline string toString(int a, int b)
{
	char buffer[6];
	sprintf(buffer, "%c%d", 'A'+a-1, b);
	return buffer;
}

vector<int>v[500];
inline void Add(int a, int b){v[a].pb(b); v[b].pb(a);}

int F[500][3], n;
bool vis[500], InBook[500];
map<int,string> M0;
map<string,int> M1;

inline void getOpinion()
{
	M0[++n]="A1", M1["A1"]=n;
	M0[++n]="A2", M1["A2"]=n;
	M0[++n]="A3", M1["A3"]=n;
	Add(1,2); Add(2,3); Add(1,3);

	ifstream fin("../LABEL");
	int tmp, num=0; fin >> tmp; for(int i=1, a=1; i<tmp; i++) num+=a, a*=3;

	string a, b, c, d;
	rep(i, 1, num)
	{
		fin >> a >> b >> c >> d;
		M0[++n]=a, M1[a]=n;
		Add(n,F[n][0]=M1[b]);
		Add(n,F[n][1]=M1[c]);
		Add(n,F[n][2]=M1[d]);
	}
}


inline void ChangetoPosition(int a)
{
	istringstream iss(P[a].latlng);
	char tmp;
	iss >> P[a].x >> tmp >> P[a].y;
}

inline void importBookmarks(const char *localFileName) // 读入JSON
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
			P[M1[*iter]].latlng = input["portals"][*iter1]["bkmrk"][*iter]["latlng"].asString();
			P[M1[*iter]].label = input["portals"][*iter1]["bkmrk"][*iter]["label"].asString();
			ChangetoPosition(M1[*iter]);
			InBook[M1[*iter]]=true;
		}
	}
}

inline int FindLB(double x, double y)
{
	rep(i, 1, n) if (fabs(x-P[i].x)<1e-7 && fabs(y-P[i].y)<1e-7) return i;
	return 0;
}

inline void importWay(const char *localFileName) // 读入JSON
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
	cout << "\n\tPortal\tKeys\tOutLinks\n";

	int L=input[0]["latLngs"].size(); rep(o, 1, L)
	{
		int x=FindLB(input[0]["latLngs"][o-1]["lat"].asDouble(),input[0]["latLngs"][o-1]["lng"].asDouble());
		if (!x)
		{
			cout << endl << "路线出现问题，需要修改！！！！！！！！！！！！" << endl;
			cout << "坐标：" << setiosflags(ios::fixed) << setprecision(6) << input[0]["latLngs"][o-1]["lat"].asDouble() << ',' << input[0]["latLngs"][o-1]["lng"].asDouble() << endl;
			cout << "未在 Bookmark 中找到此点。" << endl;
			fclose(stdout); return;
		}
		if (vis[x])
		{
			cout << endl << "路线出现问题，需要修改！！！！！！！！！！！！" << endl;
			cout << "<" << P[x].label << "> 重复经过两次！" << endl;
			fclose(stdout); return;
		}

		int key=0, out=0;
		rep(i, 0, (int)v[x].size()-1) if (!InBook[v[x][i]])
			v[x][i--]=v[x][(int)v[x].size()-1], v[x].pop_back();
		else if (!vis[v[x][i]]) key++; else out++;

		cout << M0[x] << '\t' << P[x].label << '\t' << key << '\t' << out << '\t';

		sort(all(v[x])); bool fg=false;
		rep(i, 0, (int)v[x].size()-1) if (InBook[v[x][i]] && vis[v[x][i]])
		{
			if (fg) cout << endl << '\t' << '\t' << '\t' << '\t';
			cout << P[v[x][i]].label;
			fg=true;
		}
		cout << endl;

		if (vis[F[x][0]] && vis[F[x][1]] && vis[F[x][2]])
		{
			cout << endl << "啊路线出现问题，需要修改！！！！！！！！！！！！" << endl;
			cout << "<" << P[x].label << "> 必须比 <" << P[F[x][0]].label << ">,<" << P[F[x][1]].label << ">,<" << P[F[x][2]].label << "> 三个中的任意一个先经过" << endl;
			fclose(stdout); return;
		}
		vis[x]=1;
	}

	rep(i, 1, n) if (InBook[i] && !vis[i])
	{
		cout << endl << "啊路线出现问题，需要修改！！！！！！！！！！！！" << endl;
		cout << "<" << P[i].label << "> 未经过！" << endl;
		fclose(stdout); return;
	}

	fclose(stdout);
}

int main()
{
	getOpinion();
	importBookmarks("bookmark.txt");
	importWay("way.txt");
	return 0;
}
