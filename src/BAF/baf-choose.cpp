#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#endif

#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <json/json.h>

using namespace std;

#define pi acos(-1)
#define maxPortal 1000

char tmp_char;

struct Portal
{
	double lat, lng, x, y, z, S;
	string guid, latlng, label;
    Portal() : S(0) {}
    Portal(double x, double y, double z) : x(x), y(y), z(z), S(0) {}
    void to_xyz() {
        istringstream iss(latlng);
        iss >> lat >> tmp_char >> lng;
        x = cos(lat / 180.0 * pi) * cos(lng / 180.0 * pi);
        y = sin(lat / 180.0 * pi) * cos(lng / 180.0 * pi);
        z = sin(lng / 180.0 * pi);
    }
    bool operator==(const Portal &a) const { return guid == a.guid; }
    bool operator<(const Portal &a) const { return S < a.S; }
} P[maxPortal];

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

int num = 0;

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

	arrayMember = input["portals"]["idA"]["bkmrk"].getMemberNames();
	for(Json::Value::Members::iterator iter = arrayMember.begin(); iter != arrayMember.end(); ++iter) {
		P[num].guid = input["portals"]["idA"]["bkmrk"][*iter]["guid"].asString();
		P[num].latlng = input["portals"]["idA"]["bkmrk"][*iter]["latlng"].asString();
		P[num].label = input["portals"]["idA"]["bkmrk"][*iter]["label"].asString();
		P[num].to_xyz();
        num += 1;
	}
}

Json::Value bookmarks, drawnitems;
int bm_num = 0, dr_num = 0;

inline void addPortal(const Portal &a)
{
	bookmarks["portals"]["idOthers"]["bkmrk"][to_string(bm_num)]["guid"] = a.guid;
	bookmarks["portals"]["idOthers"]["bkmrk"][to_string(bm_num)]["latlng"] = a.latlng;
	bookmarks["portals"]["idOthers"]["bkmrk"][to_string(bm_num)]["label"] = a.label;
	bm_num++;
}

inline void addLink(const Portal &a, const Portal &b)
{
	drawnitems[dr_num]["latLngs"][0]["lat"] = to_string(a.lat);
	drawnitems[dr_num]["latLngs"][0]["lng"] = to_string(a.lng);
	drawnitems[dr_num]["latLngs"][1]["lat"] = to_string(b.lat);
	drawnitems[dr_num]["latLngs"][1]["lng"] = to_string(b.lng);
	drawnitems[dr_num]["color"] = "#0099FF";
	drawnitems[dr_num]["type"] = "polyline";
	dr_num++;
}

int layer[maxPortal], nxt[maxPortal], maxLayer = 0;

int main() {
    import("portal.txt");

    for (int i = 2; i < num; i++) P[i].S = area(P[0], P[1], P[i]);
    sort(P+2, P+num);

    for (int i = 2; i < num; i++) {
        layer[i] = 1, nxt[i] = 0;
        for (int j = 2; j < i; j++) {
            if (inField(P[0], P[1], P[i], P[j]) && layer[i] < layer[j] + 1) 
                layer[i] = layer[j] + 1, nxt[i]=j;
        }
        if (!maxLayer || layer[maxLayer] < layer[i]) maxLayer = i;
    }

    addLink(P[0], P[1]); addPortal(P[0]); addPortal(P[1]);
    while (maxLayer) addLink(P[maxLayer], P[0]), addLink(P[maxLayer], P[1]), addPortal(P[maxLayer]), maxLayer = nxt[maxLayer];

    freopen("baf-result.txt", "w", stdout);

	Json::FastWriter writer;
	puts("Bookmarks JSON:"); cout << writer.write(bookmarks) << endl;
	puts("DrawTools JSON:"); cout << writer.write(drawnitems) << endl;

	fclose(stdout);

    return 0;
}
