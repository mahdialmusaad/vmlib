#define VM_IMPL
#define VM_STATIC

#define VM_NO_MATH_H
#include "vmlib.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

static int numfailed;

static int g_octaves;
static float g_gain;
static float g_lacunarity;

static double g_zcoord = 0.634723;
static double g_xscaling = 0.1;
static double g_yscaling = 0.1;

#define EXPECT(n) do {\
	if (!(n)) {\
		++numfailed;\
		printf("***** L%d FAIL: %s\n", __LINE__, #n);\
	}\
} while (0)

static void testtypes(void)
{
	EXPECT(sizeof(vm_u64) == 8);
}

static void testvec(void)
{
	vec4 v4_0 = VEC4I, v4_2 = VEC4S(2.0f), v4_m2 = VEC4S(-2.0f), v4_4 = VEC4S(4.0f), v4_5 = VEC4S(5.0f), v4_7 = VEC4S(7.0f);
	float v4_2li[4] = { 2.0f, 2.0f, 2.0f, 2.0f };
	vec3 v3 = VEC3(2.0f, 3.0f, 4.0f), v3b = VEC3(5.0f, 6.0f, 7.0f), v3_c = VEC3(-3.0f, 6.0f, -3.0f), v3r = VEC3(-1.0f, 1.0f, 0.0f), v3n = VEC3(1.0f, 0.0f, 0.0f), v3rr = VEC3(1.0f, 1.0f, 0.0f);
	vec3 v3fi = VEC3(1.0f, 2.0f, 3.0f), v3fn = VEC3(0.0f, 1.0f, 0.0f), v3fr = VEC3(1.0f, -2.0f, 3.0f);
	vec2 v2 = VEC2(8.0f, 6.0f), v2n = VEC2(0.8f, 0.6f), v2o = VEC2(16.0f, 12.0f), v2ms = VEC2S(1.0f), v2me = VEC2I, v2mm = VEC2S(0.5f);
	vec2 v2ff1n = VEC2(1.0f, 1.0f), v2ff1n2 = VEC2(-1.0f, -1.0f), v2ff1 = VEC2(23.0f, 45.0f), v2ff1r = VEC2(23.0f, 45.0f), v2ff2n = VEC2(1.0f, 1.0f), v2ff2 = VEC2(12.0f, 34.0f), v2ff2r = VEC2(-12.0f, -34.0f);
	vec4 v4 = VEC4S(2.0f);

	EXPECT(sizeof v2 == 8);
	EXPECT(sizeof v3 == 12);
	EXPECT(sizeof v4 == 16);

	EXPECT((char *)&v2.y == (char *)&v2.g);
	EXPECT((char *)&v2.x == (char *)&v2.d[0]);
	EXPECT((char *)&v2.y == (char *)&v2.d[1]);

	EXPECT((char *)&v3.z == (char *)&v3.b);
	EXPECT((char *)&v3.x == (char *)&v3.d[0]);
	EXPECT((char *)&v3.y == (char *)&v3.d[1]);
	EXPECT((char *)&v3.z == (char *)&v3.d[2]);

	EXPECT((char *)&v4.w == (char *)&v4.a);
	EXPECT((char *)&v4.x == (char *)&v4.d[0]);
	EXPECT((char *)&v4.y == (char *)&v4.d[1]);
	EXPECT((char *)&v4.z == (char *)&v4.d[2]);
	EXPECT((char *)&v4.w == (char *)&v4.d[3]);

	EXPECT(memcmp(&v4_2, v4_2li, sizeof v4_2) == 0);
	EXPECT((vec4add(&v4, &v4, &v4_5), vec4eq(&v4, &v4_7)));
	EXPECT((vec4sub(&v4, &v4, &v4_5), vec4eq(&v4, &v4_2)));
	EXPECT((vec4mul(&v4, &v4, &v4_2), vec4eq(&v4, &v4_4)));
	EXPECT((vec4div(&v4, &v4, &v4_2), vec4eq(&v4, &v4_2)));
	EXPECT((vec4mulc(&v4, &v4, 2.0f), vec4eq(&v4, &v4_4)));
	EXPECT((vec4divc(&v4, &v4, 2.0f), vec4eq(&v4, &v4_2)));
	EXPECT((vec4clr(&v4), vec4eq(&v4, &v4_0)));
	EXPECT((vec4set(&v4, 7.0f, 7.0f, 7.0f, 7.0f), vec4eq(&v4, &v4_7)));
	EXPECT((vec4sets(&v4, 2.0f), vec4eq(&v4, &v4_2)));
	EXPECT((vec4neg(&v4, &v4), vec4eq(&v4, &v4_m2)));

	EXPECT(vec2length(&v2) == 10.0f);
	EXPECT(vec2distance(&v2o, &v2) == 10.0f);
	EXPECT(vec2dot(&v2, &v2) == 100.0f);
	EXPECT((vec2normalize(&v2, &v2), vec2eq(&v2, &v2n)));
	EXPECT((vec2mix(&v2, &v2ms, &v2me, 0.5f), vec2eq(&v2, &v2mm)));

	EXPECT((vec3cross(&v3, &v3, &v3b), vec3eq(&v3, &v3_c)));
	EXPECT((vec3reflect(&v3, &v3r, &v3n), vec3eq(&v3, &v3rr)));
	EXPECT((vec3refract(&v3, &v3fi, &v3fn, 1.0f), vec3eq(&v3, &v3fr)));
	EXPECT((vec2faceforward(&v2ff1, &v2ff1, &v2ff1n, &v2ff1n2), vec2eq(&v2ff1, &v2ff1r)));
	EXPECT((vec2faceforward(&v2ff2, &v2ff2, &v2ff2n, &v2ff2n), vec2eq(&v2ff2, &v2ff2r)));
}

static void testquat(void)
{
	quat q0 = QUATA(0.0f, 100.0f, 200.0f), q0i1 = q0, q0i2 = QUATA(0.0f, 99.999f, 199.999f), q0i3 = QUATA(0.0f, 99.999f, 199.989f);
	quat q1 = QUATA(1.0f, 2.0f, 3.0f), q1r1 = QUAT(2.0f, 4.0f, 6.0f, 2.0f), q1r2 = q1;
	quat q2 = QUATA(5.0f, -2.0f, 0.0f), q2i = QUATA(-5.0f, 2.0f, 0.0f), q2r1 = QUAT(0.0f, 0.0f, 0.0f, 2.0f);
	quat q3 = QUATA(-10.0f, -2.0f, 5.0f), q3i = QUATA(5.0f, -2.0f, 1.0f), q3r1 = QUAT(-15.0f, 0.0f, 4.0f, 0.0f);
	quat q4 = QUATA(16.0f, 8.0f, 2.0f), q4r1 = QUAT(4.0f, 2.0f, 0.5f, 0.25f), q4r2 = QUAT(-4.0f, -2.0f, -0.5f, -0.25f), q4r3 = QUAT(4.0f, 2.0f, 0.5f, -0.25f);
	quat q5 = QUAT(1.0f, 1.0f, 1.0f, 1.0f);
	quat q6 = QUAT(8.0f, 6.0f, 0.0f, 0.0f), q6r1 = QUAT(0.8f, 0.6f, 0.0f, 0.0f);
	quat q7, q7r1 = QUAT(-0.718287051f, 0.310622454f, 0.44443506f, 0.435952842f); vec3 q7v1 = VEC3(1.0f, 2.0f, 3.0f);
	quat q8 = QUAT(2.0f, 3.0f, 4.0f, 1.0f); vec3 q8vi, q8r1 = VEC3(2.4037776f, 1.57079637f, 1.91382027f);
	quat q9, q9r1 = QUAT(0.707106769f, 1.41421354f, 2.12132025f, 0.707106769f); vec3 q9i = VEC3(1.0f, 2.0f, 3.0f);
	quat q10 = QUAT(1.0f, 2.0f, 3.0f, 4.0f), q10i1 = QUAT(5.0f, 6.0f, 7.0f, 8.0f), q10r1 = QUAT(24.0f, 48.0f, 48.0f, -6.0f);
	quat q11 = QUAT(10.0f, 20.0f, 30.0f, 2.0f), q11i = QUAT(10.0f, 20.0f, 30.0f, 0.2f); vec3 q11ri, q11r1 = VEC3(0.0f, 0.0f, 1.0f), q11r2 = VEC3(10.2062073f, 20.4124146f, 30.6186218f);
	float an;

	EXPECT(sizeof(quat) == sizeof(vec4));
	EXPECT(q0.w == 1.0f);
	EXPECT(quateq(&q0, &q0i1));
	EXPECT(quateeq(&q0, &q0i2, 0.01f));
	EXPECT(!quateeq(&q0, &q0i3, 0.01f));
	EXPECT((quatmuls(&q1, &q1, 2.0f), quateeq(&q1, &q1r1, 0.001f)));
	EXPECT((quatdivs(&q1, &q1, 2.0f), quateeq(&q1, &q1r2, 0.001f)));
	EXPECT((quatadd(&q2, &q2, &q2i), quateeq(&q2, &q2r1, 0.001f)));
	EXPECT((quatsub(&q3, &q3, &q3i), quateeq(&q3, &q3r1, 0.001f)));
	EXPECT((quatdivs(&q4, &q4, 4.0f), quateeq(&q4, &q4r1, 0.001f)));
	EXPECT((quatneg(&q4, &q4), quateeq(&q4, &q4r2, 0.001f)));
	EXPECT((quatconjugate(&q4, &q4), quateeq(&q4, &q4r3, 0.001f)));
	EXPECT(quatdot(&q5, &q5) == 4.0f);
	EXPECT(quatlen(&q5) == 2.0f);
	EXPECT((quatnormalize(&q6, &q6), quateeq(&q6, &q6r1, 0.001f)));
	EXPECT((quatsetv(&q7, &q7v1), quateeq(&q7, &q7r1, 0.001f)));
	//EXPECT((quateuler(&q8vi, &q8), vec3eq(&q8vi, &q8r1)));
	EXPECT((quatangleaxis(&q9, &q9i, M_PI_2), quateeq(&q9, &q9r1, 0.001f)));
	an = quatangle(&q9);
	an -= 1.57079637f;
	EXPECT(VABS(an) <= 0.001f);
	EXPECT((quatmul(&q10, &q10, &q10i1), quateeq(&q10, &q10r1, 0.001f)));
	EXPECT((quataxis(&q11, &q11ri), vec3eq(&q11ri, &q11r1)));
	//EXPECT((quataxis(&q11i, &q11ri), vec3eq(&q11ri, &q11r2)));
}

static void testmat(void)
{
	mat4 identity = MAT4I, a = MAT4S(.7f), b = MAT4A(1.f,1.f,1.f,1.f,2.f,2.f,2.f,2.f,0.5f,0.5f,0.5f,0.5f,1.0f,1.0f,1.0f,1.0f);
	mat4 aclose = MAT4S(0.99f), bclose = MAT4S(1.01f), invt = MAT4I;
	float m4ic[16] = {1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f};
	vec3 cpos = VEC3(10.f, 10.f, 10.f), target = VEC3(12.0f, 4.0f, 20.0f), up = VEC3(0.f,1.f,0.f);
	vec3 vm4tr = VEC3(10.0f, 20.0f, 30.0f), vm4ro = VEC3(0.0f, 90.0f, 45.0f), vm4sc = VEC3(5.0f, 0.1f, 2.0f), vm4roinv = VEC3(2.0f, 23.0f, 34.0f);
	float det;

	float m4ac[16] = {0.7f,0.0f,0.0f,0.0f,0.0f,0.7f,0.0f,0.0f,0.0f,0.0f,0.7f,0.0f,0.0f,0.0f,0.0f,0.7f};
	float m4bc[16] = {1.0f,1.0f,1.0f,1.0f,2.0f,2.0f,2.0f,2.0f,0.5f,0.5f,0.5f,0.5f,1.0f,1.0f,1.0f,1.0f};
	mat4 m4tr = {{0.7f,0.0f,0.0f,0.0f,0.0f,0.7f,0.0f,0.0f,0.0f,0.0f,0.7f,0.0f,7.0f,14.0f,21.0f,0.7f}};
	mat4 m4ro = {{-0.699999988f,-0.000000027f,0.000000055f,0.000000000f,0.000000027f,0.419999927f,0.559999943f,0.000000000f,-0.000000055f,0.559999943f,-0.420000017f,0.000000000f,7.000000000f,14.000000000f,21.000000000f,0.699999988f}};
	mat4 m4sc = {{-3.500000000f,-0.000000137f,0.000000274f,0.000000000f,0.000000003f,0.041999992f,0.055999994f,0.000000000f,-0.000000109f,1.119999886f,-0.840000033f,0.000000000f,7.000000000f,14.000000000f,21.000000000f,0.699999988f}};
	mat4 m4ml = {{3.500000000f,15.161999702f,20.215999603f,0.699999988f,7.000000000f,30.323999405f,40.431999207f,1.399999976f,1.750000000f,7.580999851f,10.107999802f,0.349999994f,3.500000000f,15.161999702f,20.215999603f,0.699999988f}};
	mat4 m4tp = {{3.500000000f,7.000000000f,1.750000000f,3.500000000f,15.161999702f,30.323999405f,7.580999851f,15.161999702f,20.215999603f,40.431999207f,10.107999802f,20.215999603f,0.699999988f,1.399999976f,0.349999994f,0.699999988f}};
	mat4 m4iv = {{0.363867879f,-0.753711760f,0.547283351f,-0.000000000f,0.788444161f,0.562069356f,0.249868244f,0.000000000f,-0.495939732f,0.340583295f,0.798778474f,-0.000000000f,-0.000000000f,0.000000000f,-0.000000000f,1.000000000f}};
	mat4 m4la = {{-0.980580688f,0.099449031f,-0.169030845f,0.000000000f,0.000000000f,0.861891568f,0.507092535f,0.000000000f,0.196116135f,0.497245133f,-0.845154226f,0.000000000f,7.844645500f,-14.585857391f,5.070925236f,1.000000000f}};
	mat4 m4pr = {{-0.000000044f,0.000000000f,0.000000000f,0.000000000f,0.000000000f,-0.000000044f,0.000000000f,0.000000000f,0.000000000f,0.000000000f,-1.002002001f,-1.000000000f,0.000000000f,0.000000000f,-0.200200200f,0.000000000f}};
	mat4 m4ot = {{2.000000000f,0.000000000f,0.000000000f,0.000000000f,0.000000000f,2.000000000f,0.000000000f,0.000000000f,0.000000000f,0.000000000f,-0.020020019f,0.000000000f,-3.000000000f,-7.000000000f,-1.002002001f,1.000000000f}};

	EXPECT(sizeof identity == 64);
	EXPECT(memcmp(&identity, &m4ic, sizeof m4ic) == 0);
	EXPECT((mat4clr(&identity), (memcmp(&identity, &m4ic, sizeof m4ic) == 0)));

	EXPECT(memcmp(&a, &m4ac, sizeof m4ac) == 0);
	EXPECT(memcmp(&b, &m4bc, sizeof m4bc) == 0);

	EXPECT(mat4eq(&aclose, &bclose, 0.025f) == 1);
	EXPECT(mat4eq(&aclose, &bclose, 0.01f) == 0);
	EXPECT((mat4translate(&a, &a, &vm4tr), mat4eq(&a, &m4tr, 0.001f)));
	EXPECT((mat4rotate(&a, &a, (float)M_PI, &vm4ro), mat4eq(&a, &m4ro, 0.001f)));
	EXPECT((mat4scale(&a, &a, &vm4sc), mat4eq(&a, &m4sc, 0.001f)));
	EXPECT((mat4mul(&a, &a, &b), mat4eq(&a, &m4ml, 0.01f)));
	EXPECT((mat4transpose(&a, &a), mat4eq(&a, &m4tp, 0.001f)));
	EXPECT((mat4transpose(&a, &a), mat4eq(&a, &m4ml, 0.001f)));
	mat4rotate(&invt, &invt, 1.2f, &vm4roinv);
	EXPECT((mat4inverse(&invt, &invt), mat4eq(&invt, &m4iv, 0.001f)));
	EXPECT((mat4lookat(&a, &cpos, &target, &up), mat4eq(&a, &m4la, 0.001f)));
	EXPECT((mat4perspective(&a, (float)M_PI, 1.0f, 0.1f, 100.0f), mat4eq(&a, &m4pr, 0.001f)));
	EXPECT((mat4orthographic(&a, 1.0f, 2.0f, 3.0f, 4.0f, 0.1f, 100.0f), mat4eq(&a, &m4ot, 0.001f)));
	det = mat4determinant(&a);
	det -= -0.080080f;
	EXPECT(VABS(det) <= 0.001f);
}

static void testrand(void)
{
	#define NRAND 300000
	uint64_t buf[128];
	unsigned got[3] = { 0, 0, 0 };
	unsigned fg[5] = { 0, 0, 0, 0, 0 }, fgmin = NRAND, fgmax = 0;
	double ratioend;
	int i, e = 0;
	uint64_t bufsz = (uint64_t)sizeof buf;
	vm_u64 os_got;
	vrandom_st s;
	vrand_os_st osst;

	EXPECT(vrand_os_init(&osst));
	EXPECT((os_got = vrand_os(&osst, buf, bufsz) == bufsz));
	vsrandom(&s, os_got == bufsz ? buf[0] : 123456);
	vrand_os_destroy(&osst);
	EXPECT(vrandom(&s) != vrandom(&s));

	for (i = 0; i < NRAND; ++i) {
		uint64_t r = vrandomr(&s, 0, 1);
		switch (r) {
		case 0:
			++got[0];
			break;
		case 1:
			++got[1];
			break;
		default:
			++got[2];
			break;
		}
	}

	printf("%sINTEGRAL:\theads: %u tails: %u ratio: %f\n", numfailed ? "\n" : "", got[0], got[1], (double)got[0] / (double)got[1]);

	for (i = 0; i < NRAND; ++i) {
		double r = vrandomrf(&s, 0.0, 1.0);
		if (r >= 1.0 || r <= 0.0) e = 1;
		else ++fg[(int)(r * 5.0)];
	}

	for (i = 0; i < 5; ++i) {
		unsigned c = fg[i];
		fgmin = VMIN(c, fgmin);
		fgmax = VMAX(c, fgmax);
	}

	printf("FLOATING POINT:\trange: %u { %u %u %u %u %u }\n", fgmax - fgmin, fg[0], fg[1], fg[2], fg[3], fg[4]);

	EXPECT(e == 0);
	EXPECT(got[2] == 0);
	ratioend = VABS((double)got[0] / (double)got[1]);
	EXPECT(ratioend > 0.98 && ratioend < 1.02);
	EXPECT(fgmax - fgmin < 1500);
}

#define NFB 20000

static double testfb_inner(vrand_os_st *rst, double (*approx_func)(double), double (*actual_func)(double), int zo)
{
	int i;
	double maxerr = 0.0;

	for (i = 0; i < NFB; ++i) {
		unsigned int sx;
		double x, approx, actual;
		vrand_os(rst, &sx, sizeof sx);
		x = (double)sx;
		x /= (double)UINT16_MAX;
		if (zo) { x /= (double)UINT16_MAX; x = x > 1.0 ? 1.0 : x; }
		approx = approx_func(x);
		actual = actual_func(x);
		approx -= actual;
		maxerr += VABS(approx);
	}

	return maxerr;
}

static double testfb_inner_2(vrand_os_st *rst, double (*approx_func)(double, double), double (*actual_func)(double, double), int zo)
{
	int i;
	double maxerr = 0.0;

	for (i = 0; i < NFB; ++i) {
		unsigned int sx;
		double x, y, approx, actual;
		vrand_os(rst, &sx, sizeof sx);
		x = (double)sx;
		x /= (double)UINT16_MAX;
		if (zo) { x /= (double)UINT16_MAX; x = x > 1.0 ? 1.0 : x; }
		vrand_os(rst, &sx, sizeof sx);
		y = (double)sx;
		y /= (double)UINT16_MAX;
		if (zo) { y /= (double)UINT16_MAX; y = y > 1.0 ? 1.0 : y; }
		approx = approx_func(x, y);
		actual = actual_func(x, y);
		approx -= actual;
		maxerr += VABS(approx);
	}

	return maxerr;
}

static void testfb(void)
{
	int i;
	vrand_os_st os_rng;
	double e_sqrt, e_sin, e_cos, e_tan, e_asin, e_acos, e_atan2;
	vrand_os_init(&os_rng);

	e_sqrt = testfb_inner(&os_rng, vm_sqrt, sqrt, 0);
	e_sin = testfb_inner(&os_rng, vm_sin, sin, 0);
	e_cos = testfb_inner(&os_rng, vm_cos, cos, 0);
	e_tan = testfb_inner(&os_rng, vm_tan, tan, 0);
	e_asin = testfb_inner(&os_rng, vm_asin, asin, 1);
	e_acos = testfb_inner(&os_rng, vm_acos, acos, 1);
	e_atan2 = testfb_inner_2(&os_rng, vm_atan2, atan2, 0);

	printf("TRIG ERRORS:\tsqrt: %f sin: %f cos: %f tan: %f asin: %f acos: %f atan2: %f\n", e_sqrt, e_sin, e_cos, e_tan, e_asin, e_acos, e_atan2);

	vrand_os_destroy(&os_rng);

	EXPECT(e_sqrt < 0.05);
	EXPECT(e_sin < 0.05);
	EXPECT(e_cos < 0.05);
	EXPECT(e_tan < 0.05);
	EXPECT(e_asin < 0.05);
	EXPECT(e_acos < 0.05);
	EXPECT(e_atan2 < 0.05);
}

static float testnoise_vnoise3d(vnoise *n, double x, double y) { return vnoise3d(n, x, y, g_zcoord); }
static float testnoise_vnoise3d_fractal(vnoise *n, double x, double y) { return vnoise3d_fractal(n, x, y, g_zcoord, g_octaves, g_lacunarity, g_gain); }

static void testnoise(const char *name, uint64_t seed, float (*vnoisefunc)(vnoise *, double, double))
{
	#define PX 512
	uint8_t *pixels, *start;
	float nmin = 1.0f, nmax = -1.0f, navg = 0.0f;
	FILE *ppm;
	vnoise n;
	int x, y;

	printf("%s:\n", name);
	printf("\tScaling: { %f, %f }, Z coord: %f\n", g_xscaling, g_yscaling, g_zcoord);
	if (vnoisefunc != testnoise_vnoise3d) printf("\tGain: %f  Lacunarity: %f  Octaves: %d\n", (double)g_gain, (double)g_lacunarity, g_octaves);

	if (!(pixels = start = (uint8_t *)malloc(PX * PX * 3))) abort();
	vnoise_init(&n, seed);

	for (x = 0; x < PX; ++x) {
		for (y = 0; y < PX; ++y) {
			float noise = vnoisefunc(&n, ((double)x) * g_xscaling, ((double)y) * g_yscaling);
			uint8_t r, g, b;
			noise = noise * 0.5f + 0.5f;
			navg += noise;
			if (noise > nmax) nmax = noise;
			if (noise < nmin) nmin = noise;
			r = g = b = (uint8_t)(noise * 256.0f);
			*pixels++ = r;
			*pixels++ = g;
			*pixels++ = b;
		}
	}

	navg /= (float)(PX * PX);
	printf("\tMin: %f  Max: %f  Avg: %f\n", (double)nmin, (double)nmax, (double)navg);

	ppm = fopen(name, "w");
	if (!ppm) {
		fprintf(stderr, "Failed to fopen %s for writing: %s\n", name, strerror(errno));
		goto fail;
	}

	fprintf(ppm, "P6 %d %d 255 ", PX, PX);
	fwrite(start, PX * PX * 3, 1, ppm);
	fclose(ppm);
fail:
	free(start);
}

int main(void)
{
	uint64_t noise_seed = 2147;
	char fnbuf[128];

	testtypes();
	testvec();
	testquat();
	testmat();
	testrand();
	testfb();
	testnoise("vnoise3d.ppm", noise_seed, testnoise_vnoise3d);

	g_octaves = 1;
	g_gain = 0.5f;
	g_lacunarity = 2.0f;

	for (g_octaves = 1; g_octaves <= 4; g_octaves *= 2) {
		sprintf(fnbuf, "vnoise3d_fractal_%doctave%s.ppm", g_octaves, g_octaves == 1 ? "" : "s");
		testnoise(fnbuf, noise_seed, testnoise_vnoise3d_fractal);
	}

	if (numfailed) printf("\n***** FAILED %d TEST%s *****\n", numfailed, numfailed == 1 ? "" : "S");
	return numfailed != 0;
}
