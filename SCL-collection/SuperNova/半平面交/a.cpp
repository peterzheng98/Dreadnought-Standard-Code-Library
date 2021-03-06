#include <cmath>
#include <cstdio>
#include <vector>
#include <cstring>
#include <iostream>
#include <algorithm>
#define foreach(e,x) for(__typeof(x.begin()) e=x.begin();e!=x.end();++e)
using namespace std;

const double PI = acos(-1.);
const double EPS = 1e-8;
inline int sign(double a) {
	return a < -EPS ? -1 : a > EPS;
}

struct Point {
	double x, y;
	Point() {
	}
	Point(double _x, double _y) :
			x(_x), y(_y) {
	}
	Point operator+(const Point&p) const {
		return Point(x + p.x, y + p.y);
	}
	Point operator-(const Point&p) const {
		return Point(x - p.x, y - p.y);
	}
	Point operator*(double d) const {
		return Point(x * d, y * d);
	}
	Point operator/(double d) const {
		return Point(x / d, y / d);
	}
	bool operator<(const Point&p) const {
		int c = sign(x - p.x);
		if (c)
			return c == -1;
		return sign(y - p.y) == -1;
	}
	double dot(const Point&p) const {
		return x * p.x + y * p.y;
	}
	double det(const Point&p) const {
		return x * p.y - y * p.x;
	}
	double alpha() const {
		return atan2(y, x);
	}
	double distTo(const Point&p) const {
		double dx = x - p.x, dy = y - p.y;
		return hypot(dx, dy);
	}
	double alphaTo(const Point&p) const {
		double dx = x - p.x, dy = y - p.y;
		return atan2(dy, dx);
	}
	//clockwise
	Point rotAlpha(const double &alpha, const Point &o = Point(0, 0)) const {
		double nx = cos(alpha) * (x - o.x) + sin(alpha) * (y - o.y);
		double ny = -sin(alpha) * (x - o.x) + cos(alpha) * (y - o.y);
		return Point(nx, ny) + o;
	}
	Point rot90() const {
		return Point(-y, x);
	}
	Point unit() {
		return *this / abs();
	}
	void read() {
		scanf("%lf%lf", &x, &y);
	}
	double abs() {
		return hypot(x, y);
	}
	double abs2() {
		return x * x + y * y;
	}
	void write() {
		cout << "(" << x << "," << y << ")" << endl;
	}
};

#define cross(p1,p2,p3) ((p2.x-p1.x)*(p3.y-p1.y)-(p3.x-p1.x)*(p2.y-p1.y))

#define crossOp(p1,p2,p3) sign(cross(p1,p2,p3))

Point isSS(Point p1, Point p2, Point q1, Point q2) {
	double a1 = cross(q1,q2,p1), a2 = -cross(q1,q2,p2);
	return (p1 * a2 + p2 * a1) / (a1 + a2);
}

vector<Point> convexCut(const vector<Point>&ps, Point q1, Point q2) {
	vector<Point> qs;
	int n = ps.size();
	for (int i = 0; i < n; ++i) {
		Point p1 = ps[i], p2 = ps[(i + 1) % n];
		int d1 = crossOp(q1,q2,p1), d2 = crossOp(q1,q2,p2);
		if (d1 >= 0)
			qs.push_back(p1);
		if (d1 * d2 < 0)
			qs.push_back(isSS(p1, p2, q1, q2));
	}
	return qs;
}

double calcArea(const vector<Point>&ps) {
	int n = ps.size();
	double ret = 0;
	for (int i = 0; i < n; ++i) {
		ret += ps[i].det(ps[(i + 1) % n]);
	}
	return ret / 2;
}

vector<Point> convexHull(vector<Point> ps) {
	int n = ps.size();
	if (n <= 1)
		return ps;
	sort(ps.begin(), ps.end());
	vector<Point> qs;
	for (int i = 0; i < n; qs.push_back(ps[i++])) {
		while (qs.size() > 1 && crossOp(qs[qs.size()-2],qs.back(),ps[i]) <= 0)
			qs.pop_back();
	}
	for (int i = n - 2, t = qs.size(); i >= 0; qs.push_back(ps[i--])) {
		while ((int)qs.size() > t && crossOp(qs[(int)qs.size()-2],qs.back(),ps[i]) <= 0)
			qs.pop_back();
	}
	qs.pop_back();
	return qs;
}

double convexDiameter(const vector<Point>&ps) {
	int n = ps.size();
	int is = 0, js = 0;
	for (int i = 1; i < n; ++i) {
		if (ps[i].x > ps[is].x)
			is = i;
		if (ps[i].x < ps[js].x)
			js = i;
	}
	double maxd = ps[is].distTo(ps[js]);
	int i = is, j = js;
	do {
		if ((ps[(i + 1) % n] - ps[i]).det(ps[(j + 1) % n] - ps[j]) >= 0)
			(++j) %= n;
		else
			(++i) %= n;
		maxd = max(maxd, ps[i].distTo(ps[j]));
	} while (i != is || j != js);
	return maxd;
}

struct Border {
	Point p1, p2;
	double alpha;
	void setAlpha() {
		alpha = atan2(p2.y - p1.y, p2.x - p1.x);
	}
	void read() {
		p1.read();
		p2.read();
		setAlpha();
	}
};

int n;
const int MAX_N_BORDER = 20000 + 10;
Border border[MAX_N_BORDER];

bool operator<(const Border&a, const Border&b) {
	int c = sign(a.alpha - b.alpha);
	if (c != 0)
		return c == 1;
	return crossOp(b.p1,b.p2,a.p1) >= 0;
}

bool operator==(const Border&a, const Border&b) {
	return sign(a.alpha - b.alpha) == 0;
}

const double LARGE = 10000;

void add(double x, double y, double nx, double ny) {
	border[n].p1 = Point(x, y);
	border[n].p2 = Point(nx, ny);
	border[n].setAlpha();
	n++;
}

Point isBorder(const Border&a, const Border&b) {
	return isSS(a.p1, a.p2, b.p1, b.p2);
}

Border que[MAX_N_BORDER];
int qh, qt;

bool check(const Border&a, const Border&b, const Border&me) {
	Point is = isBorder(a, b);
	return crossOp(me.p1,me.p2,is) > 0;
}

void convexIntersection() {
	qh = qt = 0;
	sort(border, border + n);
	n = unique(border, border + n) - border;
	for (int i = 0; i < n; ++i) {
		Border cur = border[i];
		while (qh + 1 < qt && !check(que[qt - 2], que[qt - 1], cur))
			--qt;
		while (qh + 1 < qt && !check(que[qh], que[qh + 1], cur))
			++qh;
		que[qt++] = cur;
	}
	while (qh + 1 < qt && !check(que[qt - 2], que[qt - 1], que[qh]))
		--qt;
	while (qh + 1 < qt && !check(que[qh], que[qh + 1], que[qt - 1]))
		++qh;
}

void calcArea() {
	static Point ps[MAX_N_BORDER];
	int cnt = 0;

	if (qt - qh <= 2) {
		puts("0.0");
		return;
	}

	for (int i = qh; i < qt; ++i) {
		int next = i + 1 == qt ? qh : i + 1;
		ps[cnt++] = isBorder(que[i], que[next]);
	}

	double area = 0;
	for (int i = 0; i < cnt; ++i) {
		area += ps[i].det(ps[(i + 1) % cnt]);
	}
	area /= 2;
	area = fabsl(area);
	cout.setf(ios::fixed);
	cout.precision(1);
	cout << area << endl;
}

void halfPlaneIntersection()
{
	cin >> n;
	for (int i = 0; i < n; ++i) {
		border[i].read();
	}
	add(0, 0, LARGE, 0);
	add(LARGE, 0, LARGE, LARGE);
	add(LARGE, LARGE, 0, LARGE);
	add(0, LARGE, 0, 0);

	convexIntersection();
	calcArea();
}

int main()
{
	return 0;
}
