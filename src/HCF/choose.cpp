#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#endif

#include <cmath>
#include <ctime>
#include <queue>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <json/json.h>

using namespace std;

#define pi acos(-1)
#define maxPortal 1000
#define maxField 166167001

char tmp_char;

int num = 0;

inline int min3(const int a, const int b, const int c) { return (a <= b && a <= c) ? a : (b <= c ? b : c); }

#define foreach(i) for(List *p = pl_head[i]; p; p = p->next)
struct List{int x; List *next;} pl[maxPortal], *pl_head[9], *pl_end = pl;

struct xyz {
	int x, y, z;
	xyz() {}
	xyz(int x, int y, int z) : x(x), y(y), z(z) {}
	bool operator < (const xyz &a) const { return x < a.x || (x == a.x && y < a.y) || (x == a.x && y == a.y && z < a.z); }
	xyz operator + (const xyz &a) const { return xyz(x + a.x, y + a.y, z + a.z); }
	xyz operator - (const xyz &a) const { return xyz(x - a.x, y - a.y, z - a.z); }
	xyz operator / (const double a) const { return xyz(x / a, y / a, z / a); }
};

struct Portal
{
	double lat, lng, x, y, z, S;
	string guid, latlng, label;
	xyz pos;
    Portal() : pos(0,0,0) {}
    Portal(double x, double y, double z) : x(x), y(y), z(z), pos(0,0,0) {}
    void to_xyz() {
        istringstream iss(latlng);
        iss >> lat >> tmp_char >> lng;
        x = cos(lat / 180.0 * pi) * cos(lng / 180.0 * pi);
        y = sin(lat / 180.0 * pi) * cos(lng / 180.0 * pi);
        z = sin(lng / 180.0 * pi);
    }
    bool operator == (const Portal &a) const { return guid == a.guid; }
    bool operator < (const Portal &a) const { return lat < a.lat || (lat == a.lat && lng < a.lng); }
} P[maxPortal];

struct Field {
	int x;
	double S;
	Field() {}
	Field(int x, double S) : x(x), S(S) {}
	bool operator < (const Field &a) const { return S < a.S; }
} qL[maxPortal], qR[maxPortal];

// Field_ID 是为了方便计算每个 Field 所对应的编号（详细见 GetFL 函数）
int fieldID[maxPortal+1][maxPortal+1];

inline void swap3(int &a, int &b, int &c) {
	if (a > b) swap(a, b);
	if (b > c) swap(b, c);
	if (a > b) swap(a, b);
}
inline int portal2field(int a, int b, int c) { swap3(a, b, c); return fieldID[a][b] + c; }
inline void field2portal(int f_id, int &a, int &b, int &c) {
	a = 0;
	while (a+1 < num-2 && fieldID[a+1][a+2] + a + 3 <= f_id) a++;
	b = a + 1;
	while (b+1 < num-1 && fieldID[a][b+1] + b + 2 <= f_id) b++;
	c = f_id - fieldID[a][b];
}

inline Portal spin(const Portal &a, const Portal &b) {
    return Portal(
        (b.y * (a.x * b.y - a.y * b.x) - b.z * (a.z * b.x - a.x * b.z)),
        (b.z * (a.y * b.z - a.z * b.y) - b.x * (a.x * b.y - a.y * b.x)),
        (b.x * (a.z * b.x - a.x * b.z) - b.y * (a.y * b.z - a.z * b.y))
    );
}

inline double _cos(const Portal &a, const Portal &b) {
    return (a.x * b.x + a.y * b.y + a.z * b.z)
        / sqrt(a.x * a.x + a.y * a.y + a.z * a.z)
        / sqrt(b.x * b.x + b.y * b.y + b.z * b.z);
}

inline double angle(const Portal &a, const Portal &b, const Portal &c) {
    return acos(_cos(spin(a, b), spin(c, b)));
}

inline double area(const Portal &a, const Portal &b, const Portal &c) {
    return angle(a,b,c) + angle(b,c,a) + angle(c,a,b) - pi;
}

inline Portal normal(const Portal &a, const Portal &b)
{
    return Portal(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}
inline double dot(const Portal &a, const Portal &b) { return a.x * b.x + a.y * b.y + a.z * b.z;}
inline bool isLeft(const Portal &a, const Portal &b, const Portal &c) { return dot(c, normal(a, b))>0;}

inline bool inField(const Portal &a, const Portal &b, const Portal &c, const Portal &d)
{
	if (d == a || d == b || d == c) return false;
	if (!isLeft(a, b, c))
        return isLeft(a, c, d) && isLeft(c, b, d) && isLeft(b, a, d);
	else
        return isLeft(a, b, d) && isLeft(b, c, d) && isLeft(c, a, d);
}

inline void import(const char *localFileName) {
    string str, chunk;

    try {
        ifstream fin(localFileName);
        if (fin) {
            while (getline(fin, chunk) && chunk != "") str += chunk;
        } else throw "";
    } catch (...) {
        puts("Please copy bookmarks into here:");
        while (getline(cin, chunk) && chunk != "") str += chunk;
    }

    Json::Reader reader;
	Json::Value input;
    Json::Value::Members arrayMember;
	reader.parse(str, input);

    num = input["portals"]["idOthers"]["bkmrk"].getMemberNames().size();

    printf("Portal nums: %d\n", num);

    if (num > maxPortal) {
        printf("Only support no more than %d portals.\n", maxPortal);
        getchar();
        exit(0);
    }

    num = 0;

	arrayMember = input["portals"]["idOthers"]["bkmrk"].getMemberNames();
	for(Json::Value::Members::iterator iter = arrayMember.begin(); iter != arrayMember.end(); ++iter) {
		P[num].guid = input["portals"]["idOthers"]["bkmrk"][*iter]["guid"].asString();
		P[num].latlng = input["portals"]["idOthers"]["bkmrk"][*iter]["latlng"].asString();
		P[num].label = input["portals"]["idOthers"]["bkmrk"][*iter]["label"].asString();
		P[num].to_xyz();
        num += 1;
	}
}

char layer[maxField]; // 记录每个 Field 的最大层数

short int nextPortal[maxField];  // 记录每个 Field 在达到最大层数时的内点
int layerCount[8]; // layerCount[i] 表示有多少个 Field 其最大层数为 i
int mxLayer[maxPortal+1][maxPortal+1]; // mxLayer[a][b] 表示向量 a→b 左边所形成的 Field 的最大层数

clock_t gap; // 记录时间

int needLayer; // 询问层数

Json::Value bookmarks, drawnitems, bookmarks_INIT, drawnitems_NULL;
int dr_num = 0;

map<string, xyz> l2p; // 编号映射到 xyz 参数
map<xyz, string> p2l; // xyz 参数映射到编号

inline void getLabel() {

	// 设置三个顶点的 xyz 参数
	l2p["A1"] = xyz(5e8, 0, 0);
	l2p["A2"] = xyz(0, 5e8, 0);
	l2p["A3"] = xyz(0, 0, 5e8);
	p2l[xyz(5e8, 0, 0)] = "A1";
	p2l[xyz(0, 5e8, 0)] = "A2";
	p2l[xyz(0, 0, 5e8)] = "A3";

	string str1 = "A", str2 = "Lv 0";
	for (int i = 1; i <= needLayer; i++) {
		str1[0] = 'A' + i-1;
		str2[3] = '0' + i;
		bookmarks_INIT["portals"][str1]["label"] = str2;
		bookmarks_INIT["portals"][str1]["state"] = 0;
	}
	bookmarks_INIT["portals"]["A"]["bkmrk"]["A1"]["guid"] = "";
	bookmarks_INIT["portals"]["A"]["bkmrk"]["A2"]["guid"] = "";
	bookmarks_INIT["portals"]["A"]["bkmrk"]["A3"]["guid"] = "";

	// LABEL 每一行有四个字符串，“A B C D” 表示以编号为 A,B,C 形成的 Field 的内点的编号为 D
	ifstream fin("../LABEL");
	int maxLayer = 0, f_num = 0;
	fin >> maxLayer;
	for(int i = 1, j = 1; i < maxLayer; i++, j *= 3) f_num += j;

	string a, b, c, d;
	for (int i = 1; i <= f_num; i++) {
		fin >> a >> b >> c >> d;
		l2p[a] = (l2p[b] + l2p[c] + l2p[d]) / 3;
		p2l[l2p[a]] = a;
		if (a[0] - 'A' + 1 <= needLayer) {
			str1[0] = a[0];
			bookmarks_INIT["portals"][str1]["bkmrk"][a]["guid"] = "";
		}
	}
}

inline void addPortal(const Portal &a, const int level)
{
	string groupName = "A";
	groupName[0] += level - 1;
	bookmarks["portals"][groupName]["bkmrk"][p2l[a.pos]]["guid"] = a.guid;
	bookmarks["portals"][groupName]["bkmrk"][p2l[a.pos]]["latlng"] = a.latlng;
	bookmarks["portals"][groupName]["bkmrk"][p2l[a.pos]]["label"] = a.label;
}

inline void addLink(const Portal &a, const Portal &b, const int level)
{
	switch (level) {
		case 1: drawnitems[dr_num]["color"] = "#0000ff"; break;
		case 2: drawnitems[dr_num]["color"] = "#2222ff"; break;
		case 3: drawnitems[dr_num]["color"] = "#4444ff"; break;
		case 4: drawnitems[dr_num]["color"] = "#6666ff"; break;
		case 5: drawnitems[dr_num]["color"] = "#8888ff"; break;
		case 6: drawnitems[dr_num]["color"] = "#aaaaff"; break;
		case 7: drawnitems[dr_num]["color"] = "#ccccff"; break;
	}

	drawnitems[dr_num]["latLngs"][0]["lat"] = to_string(a.lat);
	drawnitems[dr_num]["latLngs"][0]["lng"] = to_string(a.lng);
	drawnitems[dr_num]["latLngs"][1]["lat"] = to_string(b.lat);
	drawnitems[dr_num]["latLngs"][1]["lng"] = to_string(b.lng);
	drawnitems[dr_num]["type"] = "polyline";
	dr_num++;
}

void outputSolution(int a, int b, int c, int level) {
	if (level == needLayer) return;
	swap3(a, b, c);
	if (level == 1) {
		addLink(P[a], P[b], level);
		addLink(P[b], P[c], level);
		addLink(P[c], P[a], level);
		P[a].pos = xyz(5e8, 0, 0), addPortal(P[a], level); // 由 xyz 参数查询到当前点所对应的编号
		P[b].pos = xyz(0, 5e8, 0), addPortal(P[b], level);
		P[c].pos = xyz(0, 0, 5e8), addPortal(P[c], level);
	}
	int x = portal2field(a, b, c);
	int d = nextPortal[x];
	P[d].pos = (P[a].pos + P[b].pos + P[c].pos) / 3;
	addPortal(P[d], ++level);
	addLink(P[a], P[d], level);
	addLink(P[b], P[d], level);
	addLink(P[c], P[d], level);

	// 递归输出方案
	outputSolution(a, b, d, level);
	outputSolution(b, c, d, level);
	outputSolution(c, a, d, level);
}

// 输出方案到 JSON
inline void outputSolution(int f_id) {
	int a, b, c;
	field2portal(f_id, a, b, c);
	drawnitems = drawnitems_NULL;
	bookmarks = bookmarks_INIT;
	dr_num = 0;
	outputSolution(a, b, c, 1);

	Json::FastWriter writer;
	puts("Bookmarks JSON:"); cout << writer.write(bookmarks) << endl;
	puts("DrawTools JSON:"); cout << writer.write(drawnitems) << endl;
}

#define maxRandomChoose 1000000
int rChoose[maxRandomChoose];
#define minFieldChoose 10
#define maxFieldChoose 10
#define ranFieldChoose 30

inline void outputResult() {
	freopen("result.txt", "w", stdout);

	priority_queue<Field> q;
	int q_tot, r_tot, f_id;

	// 输出最小的前 minFieldChoose 个方案
	q_tot = 0, r_tot = 0;
	for (int i = 0; i < num; i++) for (int j = i+1; j < num; j++) for (int k = j+1; k < num; k++) {
		if (layer[f_id = portal2field(i, j, k)] >= needLayer) {
			if (q_tot == minFieldChoose) {
				q.pop(), q_tot--;
			}
			q.push(Field(f_id, area(P[i], P[j], P[k]))); q_tot++;

			if (r_tot < maxRandomChoose) rChoose[r_tot++] = f_id;
		}
	}
	while (q_tot) {
		printf("minimum area: #%d\n\n", q_tot);
		outputSolution(q.top().x);
		q.pop(); q_tot--;
	}

	// 输出最大的前 maxFieldChoose 个方案
	q_tot = 0, r_tot = 0;
	for (int i = 0; i < num; i++) for (int j = i+1; j < num; j++) for (int k = j+1; k < num; k++) {
		if (layer[f_id = portal2field(i, j, k)] >= needLayer) {
			if (q_tot == minFieldChoose) {
				q.pop(), q_tot--;
			}
			q.push(Field(f_id, -area(P[i], P[j], P[k]))); q_tot++;

			if (r_tot < maxRandomChoose) rChoose[r_tot++] = f_id;
		}
	}
	while (q_tot) {
		printf("maximum area: #%d\n\n", q_tot);
		outputSolution(q.top().x);
		q.pop(); q_tot--;
	}

	// 随机输出 ranFieldChoose 个方案
	random_shuffle(rChoose, rChoose + r_tot);
	for (int i = 0; i < min(r_tot, ranFieldChoose); i++) {
		printf("random op: #%d\n\n", i+1);
		outputSolution(rChoose[i]);
	}

	// 重名预警
	printf("duplication of Portal name：\n");
	for (int i = 0; i < num; i++) for (int j = i+1; j < num; j++) if (P[i].label == P[j].label) {
		cout << P[i].label << endl;
		break;
	}

	fclose(stdout);
}

int main() {
	srand(time(NULL));

	cout << "请将导出的 Bookmark 储存为 portal.txt 并放置于同目录下" << endl;
	getchar();
    import("portal.txt");

	// 按经纬坐标排序
	sort(P, P + num);

	// 枚举左端点和中间端点，f_tot 记录 Field 总数，fieldID 是为了方便计算每个 Field 所对应的编号（详细见 portal2field 函数）
	int f_tot = 0;
	for (int i = 0; i < num-2; i++) for (int j = i+1; j < num-1; j++) {
		f_tot += num-j;
		fieldID[i][j] = f_tot - num;
	}

	int f_now = 0, totL, totR; // f_now 表示当前已经处理完了多少个 Field
	// 从左往右枚举右端点，从右往左枚举左端点
	for (int c = 2; c < num; c++) for (int a = c-2; a >= 0; a--) {
		totL = totR = 0;

		// 枚举中间端点，qL[] 为向量 a→c 左边的 Portal（总数为 totL），qR[] 为向量 a→c 右边的 Portal（总数为 totR）
		for (int b = a+1; b <= c-1; b++) {
			if (isLeft(P[a], P[c], P[b])) {
				qL[totL++] = Field(b, area(P[a], P[b], P[c]));
			} else {
				qR[totR++] = Field(b, area(P[a], P[c], P[b]));
			}
		}

		if (totL) {
			sort(qL, qL + totL);
			memset(pl_head, 0, sizeof(pl_head));
			pl_end = pl;
			for (int i = 0; i < totL; i++) {
				int b = qL[i].x;
				int f_id = portal2field(a, b, c);
				// int mxLayer = ;
				// char &Lv = layer[f_id];
				layer[f_id] = 1;

				// 从高到低枚举内层 Field 等级并遍历 List
				for(int o = max(mxLayer[c][b], mxLayer[b][a]); o >= 1; o--) {
					foreach(o) {
						if (layer[f_id] > o) break; else if (inField(P[a], P[b], P[c], P[p->x])) {
							int tmp = min3(o, layer[portal2field(a, b, p->x)], layer[portal2field(b, c, p->x)]) + 1;
							if (tmp > layer[f_id]) {
								layer[f_id] = tmp;
								nextPortal[f_id] = p->x;
							}
						}
					}
				}

				// 判断下当 b 作为内点 a-c 做边的时候最多能多少层
				int tmp = min3((int)layer[f_id], mxLayer[a][b], mxLayer[b][c]);

				// 将 b 加入到 List[tmp]
				pl_end->x = b;
				pl_end->next = pl_head[tmp];
				pl_head[tmp] = pl_end++;

				// 更新 Lmx 数组
				if (mxLayer[a][c] < layer[f_id]) mxLayer[a][c] = layer[f_id];
				if (mxLayer[c][b] < layer[f_id]) mxLayer[c][b] = layer[f_id];
				if (mxLayer[b][a] < layer[f_id]) mxLayer[b][a] = layer[f_id];

				layerCount[(int)layer[f_id]]++;
				f_now++;

				// 每隔 0.1s 显示一次百分比进度
				if ((double)(clock() - gap) / CLOCKS_PER_SEC >= 0.1) {
					printf("%.6lf%%\n", 100.0 * f_now / f_tot);
					gap = clock();
				}
			}
		}

		if (totR) {
			sort(qR, qR + totR);
			memset(pl_head, 0, sizeof(pl_head));
			pl_end = pl;
			for (int i = 0; i < totR; i++) {
				int b = qR[i].x;
				int f_id = portal2field(a, b, c);
				// int mxLayer = ;
				// char &Lv = layer[f_id];
				layer[f_id] = 1;

				// 从高到低枚举内层 Field 等级并遍历 List
				for(int o = max(mxLayer[a][b], mxLayer[b][c]); o >= 1; o--) {
					foreach(o) {
						if (layer[f_id] > o) break; else if (inField(P[a], P[b], P[c], P[p->x])) {
							int tmp = min3(o, layer[portal2field(a, b, p->x)], layer[portal2field(b, c, p->x)]) + 1;
							if (tmp > layer[f_id]) {
								layer[f_id] = tmp;
								nextPortal[f_id] = p->x;
							}
						}
					}
				}

				// 判断下当 b 作为内点 a-c 做边的时候最多能多少层
				int tmp = min3((int)layer[f_id], mxLayer[c][b], mxLayer[b][a]);

				// 将 b 加入到 List[tmp]
				pl_end->x = b;
				pl_end->next = pl_head[tmp];
				pl_head[tmp] = pl_end++;

				// 更新 Lmx 数组
				if (mxLayer[a][b] < layer[f_id]) mxLayer[a][b] = layer[f_id];
				if (mxLayer[b][c] < layer[f_id]) mxLayer[b][c] = layer[f_id];
				if (mxLayer[c][a] < layer[f_id]) mxLayer[c][a] = layer[f_id];

				layerCount[(int)layer[f_id]]++;
				f_now++;

				// 每隔 0.1s 显示一次百分比进度
				if ((double)(clock() - gap) / CLOCKS_PER_SEC >= 0.1) {
					printf("%.6lf%%\n", 100.0 * f_now / f_tot);
					gap = clock();
				}
			}
		}
	}

	for (int i = 3; i <= 7; i++) printf("%d Layers: %d Solutions\n", i, layerCount[i]);
	needLayer = 0;
	while (needLayer < 3 || needLayer > 7) {
		puts("What kind of solutions do you need? (3-7)\n");
		scanf("%d", &needLayer);
	}

	puts("Wait for a moment, and then you can see the results in `result.txt`, good luck >.<");
	getLabel();
	outputResult();

	return 0;
}
