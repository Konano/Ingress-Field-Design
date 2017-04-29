#include <cstdio>
#include <algorithm>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>

#define rep(i, l, r) for(int i=l; i<=r; i++)
#define dow(i, l, r) for(int i=l; i>=r; i--)
#define clr(x, c) memset(x, c, sizeof(x))
#define pi acos(-1)
#define travel(i) for(edge *p=fir[i]; p; p=p->n)

using namespace std;

typedef pair<double,int> Pdi;
typedef long double ld;

struct Portal
{
	double x0, y0, x, y, z, S;
	string latlng;
} P[9], G[9];



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




inline void ChangetoPosition(int a)
{
	istringstream iss(P[a].latlng);
	char tmp;
	iss >> P[a].x0 >> tmp >> P[a].y0;
	P[a].x=cos(P[a].x0/180.0*pi)*cos(P[a].y0/180.0*pi);
	P[a].y=sin(P[a].x0/180.0*pi)*cos(P[a].y0/180.0*pi);
	P[a].z=sin(P[a].y0/180.0*pi);
}





int main()
{
	cin >> P[1].latlng; ChangetoPosition(1);
	cin >> P[2].latlng; ChangetoPosition(2);
	cin >> P[3].latlng; ChangetoPosition(3);
	cout << Area(1,2,3) << endl;
	system("pause");
	
	return 0;
}






