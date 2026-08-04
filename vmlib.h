/*
	vmlib - A header-only general mathematics library.
	Copyright (c) 2026 Mahdi Almusaad (MIT License, provided)
*/

#ifndef VMATHLIB_HDR
#define VMATHLIB_HDR

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __STDC_VERSION__
# define VM_CVER __STDC_VERSION__
#else
# define VM_CVER 198900L
#endif

#ifdef __GNUC__
# ifdef __clang__
#  define VM_PACK_START(n) _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wc11-extensions\"") _Pragma("GCC diagnostic ignored \"-Wpacked\"")
#  define VM_RSTATEW_START _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wreserved-identifier\"")
#  define VM_RSTATEW_END _Pragma("GCC diagnostic pop")
# else
#  define VM_PACK_START(n) _Pragma("GCC diagnostic push")  _Pragma("GCC diagnostic ignored \"-Wpedantic\"")
#  define VM_RSTATEW_START
#  define VM_RSTATEW_END
# endif
# define VM_PACK_MID(n) __attribute__((packed, aligned(n)))
# define VM_PACK_END _Pragma("GCC diagnostic pop")
# define VM_INLINE __inline__
# define VM_THREADLOCAL __thread
# define VM_NOINLINE_IMPL __attribute__((__noinline__))
#elif defined(_MSC_VER)
# define VM_PACK_START(n) __pragma(warning(push)) __pragma(warning(disable:4201)) __pragma(pack(push, 1)) __declspec(align(n))
# define VM_PACK_MID(n)
# define VM_PACK_END __pragma(pack(pop)) __pragma(warning(pop))
# define VM_RSTATEW_START
# define VM_RSTATEW_END
# define VM_INLINE __inline
# define VM_THREADLOCAL __declspec(thread)
# define VM_NOINLINE_IMPL __declspec(noinline)
#else
# if VM_CVER >= 202311L
#  define VM_THREADLOCAL thread_local
# elif VM_CVER >= 201112L
#  define VM_THREADLOCAL _Thread_local
# else
#  define VM_THREADLOCAL
# endif
# if VM_CVER >= 199901L
#  define VM_INLINE inline
# else
#  define VM_INLINE
# endif
# define VM_PACK_START(n)
# define VM_PACK_MID(n)
# define VM_PACK_END
# define VM_RSTATEW_START
# define VM_RSTATEW_END
# define VM_NOINLINE_IMPL
#endif

#ifdef VM_NOINLINE
# undef VM_NOINLINE
# define VM_NOINLINE VM_NOINLINE_IMPL
#else
# define VM_NOINLINE
#endif

#ifdef VM_STATIC
# define VM_API VM_NOINLINE static
#else
# define VM_API VM_NOINLINE extern
#endif

#ifdef __has_include
# define VM_HASINC(a) __has_include(a)
#else
# define VM_HASINC(a) 0
#endif

#if defined(__MINGW32__) || defined(__MINGW64__)
# define VM_MINGW
#endif

#ifdef __GNUC__
# define VM_ATTRIBCONST __attribute__((__const__))
# define VM_ATTRIBPURE __attribute__((__pure__))
# define VM_EXTENSION __extension__
#else
# define VM_ATTRIBCONST
# define VM_ATTRIBPURE
# define VM_EXTENSION
#endif

#if defined(__LLP64__) || defined(_ILP32) || defined(__ILP32__) || defined(VM_MINGW) || defined(_MSC_VER)
# define VM_LONGLONG
#endif

#if VM_CVER > 198900L || VM_HASINC(<stdint.h>)
# include <stdint.h>
typedef uint64_t vm_u64;
typedef int64_t vm_i64;
# define VM_U64C(a) VM_EXTENSION UINT64_C(a)
#elif defined(VM_LONGLONG)
typedef VM_EXTENSION unsigned long long vm_u64;
typedef VM_EXTENSION long long vm_i64;
# define VM_U64C(a) VM_EXTENSION a ## ULL
#else
typedef unsigned long vm_u64;
typedef long vm_i64;
# define VM_U64C(a) a ## UL
#endif

#if VM_HASINC(<math.h>)
# include <math.h>
# define vm_sqrt sqrt
# define vm_sin sin
# define vm_cos cos
#elif !defined (vm_sqrt) || !defined(vm_cos) || !defined(vm_sin)
# define VM_NEEDMATH
#endif

#ifndef M_PI
# define M_E 2.7182818284590452354
# define M_LOG2E 1.4426950408889634074
# define M_LOG10E 0.43429448190325182765
# define M_LN2 0.69314718055994530942
# define M_LN10 2.30258509299404568402
# define M_PI 3.14159265358979323846
# define M_PI_2 1.57079632679489661923
# define M_PI_4 0.78539816339744830962
# define M_1_PI 0.31830988618379067154
# define M_2_PI 0.63661977236758134308
# define M_2_SQRTPI 1.12837916709551257390
# define M_SQRT2 1.41421356237309504880
# define M_SQRT1_2 0.70710678118654752440
#endif

#define VABS(x) ((x) < 0 ? -(x) : (x))
#define VMAX(a,b) ((a) > (b) ? (a) : (b))
#define VMIN(a,b) ((a) < (b) ? (a) : (b))
#define VCLAMP(x,min,max) (VMAX(VMIN(x,max),min))
#define VFMA(a,b,c) (((a) * (b)) + (c))
#define VMIX(x,y,a) ((x) * (1 - (a)) + (y) * (a))
#define VSTEP(edge,x) (((x) < (edge)) ? 0 : 1)
#define VRADIANS(deg) (__typeof__(deg)(M_PI / 180.0) * (deg))
#define VDEGREES(rad) (__typeof__(rad)(180.0 / M_PI) * (deg))

/* ---------- Mathematical vectors ---------- */

/* Mathematical 4d, 3d and 2d vector struct definitions with the same names as GLSL for convenience (e.g. vec3, dvec4, uvec2).
   Alignment and packing is done as appropriate, but you need to ensure to pad 3D vectors to the 4D size when using the std140 layout.
   Most functions have the same/similar names (no overloading in C, so operators are provided as well with appropriate names).
   Functions that would result in or return a vector in GLSL instead have an output vector 'r', which can alias the other vector pointers. */

/* Other functions/operators are also defined that may not be strictly available in GLSL but useful in graphics programming. */

#define VM_VECDEF(n,k) VM_PACK_START(2)\
typedef union n##2 { k d[2]; struct { k x, y; }; struct { k r, g; }; struct { k s, t; }; } VM_PACK_MID(2) n##2;\
typedef union n##3 { k d[3]; struct { k x, y, z; }; struct { k r, g, b; }; struct { k s, t, p; }; } VM_PACK_MID(4) n##3;\
typedef union n##4 { k d[4]; struct { k x, y, z, w; }; struct { k r, g, b, a; }; struct { k s, t, p, q; }; } VM_PACK_MID(4) n##4; VM_PACK_END

VM_VECDEF(vec, float)
VM_VECDEF(dvec, double)
VM_VECDEF(ivec, int)
VM_VECDEF(uvec, unsigned)

#define VM_VEC_DEF(m,t)\
VM_API void m##clr(m *a);\
VM_API void m##sets(m *a, t s);\
VM_API void m##setv(m *a, const m *b);\
VM_API void m##add(m *r, const m *a, const m *b);\
VM_API void m##addc(m *r, const m *a, t s);\
VM_API void m##sub(m *r, const m *a, const m *b);\
VM_API void m##subc(m *r, const m *a, t s);\
VM_API void m##mul(m *r, const m *a, const m *b);\
VM_API void m##mulc(m *r, const m *a, t s);\
VM_API void m##div(m *r, const m *a, const m *b);\
VM_API void m##divc(m *r, const m *a, t s);\
VM_API void m##neg(m *r, const m *a);\
VM_API int m##eq(const m *a, const m *b);
#define VM_VECF_DEF(m,t) VM_VEC_DEF(m,t)\
VM_API t m##length(const m *a);\
VM_API t m##distance(const m *a, const m *b);\
VM_API t m##dot(const m *a, const m *b);\
VM_API void m##mix(m *r, const m *x, const m *y, t a);\
VM_API void m##normalize(m *r, const m *a);\
VM_API void m##faceforward(m *r, const m *N, const m *i, const m *Nref);\
VM_API void m##reflect(m *r, const m *i, const m *n);\
VM_API void m##refract(m *r, const m *i, const m *n, t eta);

#define VM_VEC4_SET(m,t) VM_API void m##set(m *a, t x, t y, t z, t w);
#define VM_VEC3_SET(m,t) VM_API void m##set(m *a, t x, t y, t z);
#define VM_VEC2_SET(m,t) VM_API void m##set(m *a, t x, t y);

#define VM_VEC4F_DEF(m,t)VM_VEC4_SET(m,t)VM_VECF_DEF(m,t)
#define VM_VEC4_DEF(m,t)VM_VEC4_SET(m,t)VM_VEC_DEF(m,t)
#define VM_VEC3F_DEF(m,t)VM_VEC3_SET(m,t)VM_VECF_DEF(m,t)VM_API void m##cross(m *r, const m *a, const m *b);
#define VM_VEC3_DEF(m,t)VM_VEC3_SET(m,t)VM_VEC_DEF(m,t)
#define VM_VEC2F_DEF(m,t)VM_VEC2_SET(m,t)VM_VECF_DEF(m,t)
#define VM_VEC2_DEF(m,t)VM_VEC2_SET(m,t)VM_VEC_DEF(m,t)

VM_VEC4F_DEF(vec4, float)
VM_VEC4F_DEF(dvec4, double)
VM_VEC4_DEF(ivec4, int)
VM_VEC4_DEF(uvec4, unsigned)

VM_VEC3F_DEF(vec3, float)
VM_VEC3F_DEF(dvec3, double)
VM_VEC3_DEF(ivec3, int)
VM_VEC3_DEF(uvec3, unsigned)

VM_VEC2F_DEF(vec2, float)
VM_VEC2F_DEF(dvec2, double)
VM_VEC2_DEF(ivec2, int)
VM_VEC2_DEF(uvec2, unsigned)

#if VM_CVER == 198900L
# define VM_COM_DOUBLE(x,y) ((x),(y))
# define VM_COM_TRIPLE(x,y,z) VM_COM_DOUBLE(VM_COM_DOUBLE(x, y), z)
# define VEC2(a,b) (*VM_COM_TRIPLE(\
	__vxvec2_aginit_glob.x = a,\
	__vxvec2_aginit_glob.y = b,\
	&__vxvec2_aginit_glob))
# define VEC3(a,b,c) (*VM_COM_DOUBLE(VM_COM_TRIPLE(\
	__vxvec3_aginit_glob.x = a,\
	__vxvec3_aginit_glob.y = b,\
	__vxvec3_aginit_glob.z = c),\
	&__vxvec3_aginit_glob))
# define VEC4(a,b,c,d) (*VM_COM_TRIPLE(VM_COM_TRIPLE(\
	__vxvec4_aginit_glob.x = a,\
	__vxvec4_aginit_glob.y = b,\
	__vxvec4_aginit_glob.z = c),\
	__vxvec4_aginit_glob.w = d,\
	&__vxvec4_aginit_glob))
#else
# define VEC2(a,b) (vec2){{a,b}}
# define VEC3(a,b,c) (vec3){{a,b,c}}
# define VEC4(a,b,c,d) (vec4){{a,b,c,d}}
#endif

#define VEC2S(x) VEC2(x,x)
#define VEC3S(x) VEC3(x,x,x)
#define VEC4S(x) VEC4(x,x,x,x)
#define VEC2I VEC2S(0)
#define VEC3I VEC3S(0)
#define VEC4I VEC4S(0)

/* 4x4 matrix definition. Same notes written above (on vectors) apply here.
   View transformation functions assume right-handed and -1 to 1 depth. */

VM_PACK_START(4)
typedef union mat4 { float d[16]; vec4 v[4]; float n[4][4]; } VM_PACK_MID(4) mat4;
VM_PACK_END

typedef mat4 mat4x4;

#if VM_CVER == 198900L
# define MAT4S(s) (__vmat4_aginit_glob = VM_COM_TRIPLE(VM_COM_TRIPLE(\
	vec4set(&__vmat4_aginit_glob.v[0], s, 0, 0, 0),\
	vec4set(&__vmat4_aginit_glob.v[1], 0, s, 0, 0),\
	vec4set(&__vmat4_aginit_glob.v[2], 0, 0, s, 0)),\
	vec4set(&__vmat4_aginit_glob.v[3], 0, 0, 0, s),\
	__vmat4_aginit_glob))
# define MAT4V(a,b,c,d) VM_COM_TRIPLE(VM_COM_TRIPLE(\
	__vmat4_aginit_glob.v[0] = a,\
	__vmat4_aginit_glob.v[1] = b,\
	__vmat4_aginit_glob.v[2] = c),\
	__vmat4_aginit_glob.v[3] = d,\
	__vmat4_aginit_glob)
#else
# define MAT4S(s) (mat4){{s,0,0,0,0,s,0,0,0,0,s,0,0,0,0,s}}
# define MAT4V(a,b,c,d) (mat4){{a.x,a.y,a.z,a.w,b.x,b.y,b.z,b.w,c.x,c.y,c.z,c.w,d.x,d.y,d.z,d.w}}
#endif

#define MAT4(x) MAT4S(x)
#define MAT4I MAT4S(1.f)

/* Copy matrix m into r. */
VM_API void mat4set(mat4 *r, const mat4 *m);
/* Set diagonal of matrix to s and others to 0. */
VM_API void mat4sets(mat4 *r, float s);
/* Set matrix components using vectors. */
VM_API void mat4setv(mat4 *r, const vec4 *a, const vec4 *b, const vec4 *c, const vec4 *d);
/* Set m to identity matrix, i.e. mat4sets(m, 1.0f). */
VM_API void mat4clr(mat4 *m);

/* Component-wise addition of a and b into r. */
VM_API void mat4add(mat4 *r, const mat4 *a, const mat4 *b);
/* Component-wise addition of a with scalar s into r. */
VM_API void mat4addc(mat4 *r, const mat4 *m, float s);
/* Component-wise subtraction of a and b into r. */
VM_API void mat4sub(mat4 *r, const mat4 *a, const mat4 *b);
/* Component-wise subtraction of a with scalar s into r. */
VM_API void mat4subc(mat4 *r, const mat4 *m, float s);
/* Perform linear algebraic matrix multiplication of a with b into r. */
VM_API void mat4mul(mat4 *r, const mat4 *a, const mat4 *b);
/* Component-wise multiplication of a with scalar s into r. */
VM_API void mat4mulc(mat4 *r, const mat4 *m, float s);
/* Perform linear algebraic matrix division of a with b into r. */
VM_API void mat4div(mat4 *r, const mat4 *a, const mat4 *b);
/* Component-wise division of a with scalar s into r. */
VM_API void mat4divc(mat4 *r, const mat4 *m, float s);
/* Returns whether every component in a equals (with margin) to the corresponding component in b. */
VM_API int mat4eq(const mat4 *a, const mat4 *b, float epsilon);

/* Component-wise multiplication of a and b into r. */
VM_API void mat4compmult(mat4 *r, const mat4 *a, const mat4 *b);
/* Component-wise division of a and b into r. */
VM_API void mat4compdiv(mat4 *r, const mat4 *a, const mat4 *b);
/* Perform linear algebraic matrix multiplication of c (as if a 1-column matrix) with vr (as if a 1-row matrix) into r. */
VM_API void mat4outerproduct(mat4 *r, const vec4 *c, const vec4 *vr);
/* Calculate determinant of m. */
VM_API float mat4determinant(const mat4 *m);
/* Put transpose of m into r. */
VM_API void mat4transpose(mat4 *r, const mat4 *m);
/* Calculates scary inverse of m into r. */
VM_API void mat4inverse(mat4 *r, const mat4 *m);

/* Move matrix m by v. */
VM_API void mat4translate(mat4 *r, const mat4 *m, const vec3 *v);
/* Rotate matrix m around the axis v by radians angle. */
VM_API void mat4rotate(mat4 *r, const mat4 *m, float radians, const vec3 *v);
/* Scale matrix m by v. */
VM_API void mat4scale(mat4 *r, const mat4 *m, const vec3 *v);

/* Create a view matrix m that looks at a target. */
VM_API void mat4lookat(mat4 *m, const vec3 *campos, const vec3 *targetpos, const vec3 *up);
/* Create a perspective projection matrix m, where distant objects appear smaller (depth). */
VM_API void mat4perspective(mat4 *m, float fovy, float aspect, float znear, float zfar);
/* Create an orthographic projection matrix m, where objects always maintain their size. */
VM_API void mat4orthographic(mat4 *m, float left, float right, float bottom, float top, float znear, float zfar);

/* Terrible C89/MinGW aggregate initialization workaround variables. */

#if VM_CVER == 198900L
VM_RSTATEW_START
extern VM_THREADLOCAL mat4 __vmat4_aginit_glob;
extern VM_THREADLOCAL vec2 __vxvec2_aginit_glob;
extern VM_THREADLOCAL vec3 __vxvec3_aginit_glob;
extern VM_THREADLOCAL vec4 __vxvec4_aginit_glob;
VM_RSTATEW_END
#endif

/* ---------- Random number generation ---------- */

/* PRNG state. */
typedef struct vrandom_st { vm_u64 s[4]; } vrandom_st;

/* Maximum integral value that can be generated by PRNG functions (max. unsigned 64-bit value). */
#define VRAND_MAX VM_U64C(18446744073709551615)

/* Get the next random unsigned 64-bit number from the random state. */
VM_API vm_u64 vrandom(vrandom_st *s);
/* Seed the given random state. */
VM_API void vsrandom(vrandom_st *s, vm_u64 seed);
/* Get the next random unsigned 64-bit number from the random state within an inclusive range. */
VM_API vm_u64 vrandomr(vrandom_st *s, vm_u64 min, vm_u64 max);
/* Get the next random floating-point number from the random state within an inclusive range. */
VM_API double vrandomrf(vrandom_st *s, double min, double max);

VM_RSTATEW_START
/* Global, thread-local random state used for vrand* macros below. */
extern VM_THREADLOCAL vrandom_st __vrand_tlglob_state;
VM_RSTATEW_END

/* These have the same functionality as the v*random* functions above,
   except they use the global state, so do not have a state parameter. */

#define vrand() vrandom(&__vrand_tlglob_state)
#define vsrand(seed) vsrandom(&__vrand_tlglob_state, seed)
#define vrandr(min, max) vrandomr(&__vrand_tlglob_state, min, max)
#define vrandrf(min, max) vrandomrf(&__vrand_tlglob_state, min, max)

#ifdef _WIN32
# ifdef WINAPI
#  define VM_WINAPI WINAPI
# elif defined(VM_MINGW)
#  define VM_WINAPI __attribute__((__stdcall__))
# elif defined(_MSC_VER)
#  define VM_WINAPI __stdcall
# endif
typedef int (VM_WINAPI *vm_pfn_RtlGenRandom)(void *b, unsigned long n);
struct vrand_os_st
{
	void *advapi;
	vm_pfn_RtlGenRandom pfn_rtlgr;
};
# elif VM_HASINC(<stdio.h>)
struct vrand_os_st { void *urandom; };
# else
struct vrand_os_st { char unused; };
# endif

/* vrand_os context for CSPRNG. */
typedef struct vrand_os_st vrand_os_st;

/* Initialize OS-specific CSPRNG context. */
VM_API int vrand_os_init(vrand_os_st *s);
/* Destroy CSPRNG context. */
VM_API void vrand_os_destroy(vrand_os_st *s);
/* Fill at most n bytes of buf from CSPRNG context and return number of bytes filled. */
VM_API vm_u64 vrand_os(vrand_os_st *s, void *buf, vm_u64 n);

/* ---------- Noise generation ---------- */

/* Noise permutation state. */
typedef struct vnoise { unsigned char p[512]; } vnoise;

/* Initialize vnoise with a default permutation table that is shuffled depending on the seed. */
VM_API void vnoise_init(vnoise *n, vm_u64 seed);
/* Default 3D noise function, returning a value in the range [-1.0, 1.0]. */
VM_API float vnoise3d(const vnoise *n, double x, double y, double z);
/* Fractal noise function. Same range, uses above function internally (vnoise3d). */
VM_API float vnoise3d_fractal(const vnoise *n, double x, double y, double z, int octaves, float lacunarity, float gain);

#ifdef VM_IMPL

VM_RSTATEW_START
#if VM_CVER == 198900L
VM_THREADLOCAL mat4 __vmat4_aginit_glob;
VM_THREADLOCAL vec2 __vxvec2_aginit_glob;
VM_THREADLOCAL vec3 __vxvec3_aginit_glob;
VM_THREADLOCAL vec4 __vxvec4_aginit_glob;
#endif
VM_THREADLOCAL vrandom_st __vrand_tlglob_state = { { VM_U64C(18396103567723650381), VM_U64C(16949579112299196916), VM_U64C(14974155592387081785), VM_U64C(6981522026073024744) } };
VM_RSTATEW_END

#if VM_HASINC(<string.h>)
# include <string.h>
# define vm_memcpy memcpy
# define vm_memcmp memcmp
#else
static VM_INLINE void *vm_memcpy(void *dst, const void *src, vm_u64 n)
{
	unsigned char *d = (unsigned char *)dst;
	const unsigned char *s = (const unsigned char *)src;
	while (n--) *d++ = *s++;
	return dst;
}
static VM_INLINE int vm_memcmp(const void *a, const void *b, vm_u64 n)
{
	const unsigned char *x = (const unsigned char *)a;
	const unsigned char *y = (const unsigned char *)b;
	while (n--) { if (*x != *y) { return (int)(*x - *y); } ++x; ++y; }
	return 0;
}
#endif

#ifdef VM_NEEDMATH
# undef VM_NEEDMATH
static VM_ATTRIBCONST double vm_sqrt(double x)
{
	double g = 1.0;
	int i;
	for (i = 0; i < 20; ++i) g -= (g * g - x) / (2.0 * g);
	return g;
}
static VM_ATTRIBCONST double vm_sin(double x)
{
	double x2 = x * x;
	double x3 = x2 * x, f3 = 6.0, x5 = x3 * x2, f5 = 120.0;
	double x7 = x5 * x2, f7 = 5040.0, x9 = x7 * x2, f9 = 362880.0;
	return x - (x3/f3) + (x5/f5) - (x7/f7) + (x9/f9);
}
static VM_ATTRIBCONST double vm_cos(double x)
{
	double x2 = x * x, f2 = 2.0, x4 = x2 * x2, f4 = 24.0;
	double x6 = x4 * x2, f6 = 720.0, x8 = x6 * x2, f8 = 40320.0;
	return 1.0 - (x2/f2) + (x4/f4) - (x6/f6) + (x8/f8);
}
#endif

/* ---------- Mathematical vectors ---------- */

#define VM_VEC_IMPL(m,t)\
VM_API void m##clr(m *a) { m##sets(a, 0); }\
VM_API void m##setv(m *a, const m *b) { vm_memcpy(a, b, sizeof *a); }\
VM_API int m##eq(const m *a, const m *b) { return vm_memcmp(a, b, sizeof *a) == 0; }\
VM_API void m##neg(m *r, const m *a) { m##mulc(r, a, (t)-1); }
#define VM_VECF_IMPL(m,t)\
VM_API t m##length(const m *a) { return (t)vm_sqrt((double)m##dot(a,a)); }\
VM_API t m##distance(const m *a, const m *b) { m c; m##sub(&c, a, b); return m##length(&c); }\
VM_API void m##mix(m *r, const m *x, const m *y, t a) { m d; m##mulc(&d, y, a); m##mulc(r, x, 1 - a); m##add(r, r, &d); }\
VM_API void m##normalize(m *r, const m *a) { t c = (t)1 / m##length(a); m##mulc(r, a, c); }\
VM_API void m##faceforward(m *r, const m *N, const m *i, const m *Nref) { if (m##dot(Nref, i) < (t)0) m##setv(r, N); else m##neg(r, N); }\
VM_API void m##reflect(m *r, const m *i, const m *n) { t dni2 = m##dot(n, i) * (t)2; m##mulc(r, n, dni2); m##sub(r, i, r); }\
VM_API void m##refract(m *r, const m *i, const m *n, t eta) { t dni = m##dot(n, i), k = (t)1.0 - eta * eta * ((t)1.0 - dni * dni); if (k < (t)0) m##clr(r); else { t b = eta * dni + (t)vm_sqrt((double)k); m tmp; m##mulc(&tmp, n, b); m##mulc(r, i, eta); m##sub(r, r, &tmp); } }

#define VM_VEC4_IMPL(m,t)VM_VEC_IMPL(m,t)\
VM_API void m##set(m *a, t x, t y, t z, t w) { a->x = x; a->y = y; a->z = z; a->w = w; }\
VM_API void m##sets(m *a, t s) { a->x = a->y = a->z = a->w = s; }\
VM_API void m##add(m *r, const m *a, const m *b) { r->x = a->x + b->x; r->y = a->y + b->y; r->z = a->z + b->z; r->w = a->w + b->w; }\
VM_API void m##addc(m *r, const m *a, t s) { r->x = a->x + s; r->y = a->y + s; r->z = a->z + s; r->w = a->w + s; }\
VM_API void m##sub(m *r, const m *a, const m *b) { r->x = a->x - b->x; r->y = a->y - b->y; r->z = a->z - b->z; r->w = a->w - b->w; }\
VM_API void m##subc(m *r, const m *a, t s) { r->x = a->x - s; r->y = a->y - s; r->z = a->z - s; r->w = a->w - s; }\
VM_API void m##mul(m *r, const m *a, const m *b) { r->x = a->x * b->x; r->y = a->y * b->y; r->z = a->z * b->z; r->w = a->w * b->w; }\
VM_API void m##mulc(m *r, const m *a, t s) { r->x = a->x * s; r->y = a->y * s; r->z = a->z * s; r->w = a->w * s; }\
VM_API void m##div(m *r, const m *a, const m *b) { r->x = a->x / b->x; r->y = a->y / b->y; r->z = a->z / b->z; r->w = a->w / b->w; }\
VM_API void m##divc(m *r, const m *a, t s) { r->x = a->x / s; r->y = a->y / s; r->z = a->z / s; r->w = a->w / s; }
#define VM_VEC4F_IMPL(m,t)VM_VEC4_IMPL(m,t)VM_VECF_IMPL(m,t)\
VM_API t m##dot(const m *a, const m *b) { return a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w; }
#define VM_VEC3_IMPL(m,t)VM_VEC_IMPL(m,t)\
VM_API void m##set(m *a, t x, t y, t z) { a->x = x; a->y = y; a->z = z; }\
VM_API void m##sets(m *a, t s) { a->x = a->y = a->z = s; }\
VM_API void m##add(m *r, const m *a, const m *b) { r->x = a->x + b->x; r->y = a->y + b->y; r->z = a->z + b->z; }\
VM_API void m##addc(m *r, const m *a, t s) { r->x = a->x + s; r->y = a->y + s; r->z = a->z + s; }\
VM_API void m##sub(m *r, const m *a, const m *b) { r->x = a->x - b->x; r->y = a->y - b->y; r->z = a->z - b->z; }\
VM_API void m##subc(m *r, const m *a, t s) { r->x = a->x - s; r->y = a->y - s; r->z = a->z - s; }\
VM_API void m##mul(m *r, const m *a, const m *b) { r->x = a->x * b->x; r->y = a->y * b->y; r->z = a->z * b->z; }\
VM_API void m##mulc(m *r, const m *a, t s) { r->x = a->x * s; r->y = a->y * s; r->z = a->z * s; }\
VM_API void m##div(m *r, const m *a, const m *b) { r->x = a->x / b->x; r->y = a->y / b->y; r->z = a->z / b->z; }\
VM_API void m##divc(m *r, const m *a, t s) { r->x = a->x / s; r->y = a->y / s; r->z = a->z / s; }
#define VM_VEC3F_IMPL(m,t)VM_VEC3_IMPL(m,t)VM_VECF_IMPL(m,t)\
VM_API void m##cross(m *r, const m *a, const m *b) { m##set(r, a->y*b->z - a->z*b->y, a->z*b->x - a->x*b->z, a->x*b->y - a->y*b->x); }\
VM_API t m##dot(const m *a, const m *b) { return a->x * b->x + a->y * b->y + a->z * b->z; }
#define VM_VEC2_IMPL(m,t)VM_VEC_IMPL(m,t)\
VM_API void m##set(m *a, t x, t y) { a->x = x; a->y = y; }\
VM_API void m##sets(m *a, t s) { a->x = a->y = s; }\
VM_API void m##add(m *r, const m *a, const m *b) { r->x = a->x + b->x; r->y = a->y + b->y; }\
VM_API void m##addc(m *r, const m *a, t s) { r->x = a->x + s; r->y = a->y + s; }\
VM_API void m##sub(m *r, const m *a, const m *b) { r->x = a->x - b->x; r->y = a->y - b->y; }\
VM_API void m##subc(m *r, const m *a, t s) { r->x = a->x - s; r->y = a->y - s; }\
VM_API void m##mul(m *r, const m *a, const m *b) { r->x = a->x * b->x; r->y = a->y * b->y; }\
VM_API void m##mulc(m *r, const m *a, t s) { r->x = a->x * s; r->y = a->y * s; }\
VM_API void m##div(m *r, const m *a, const m *b) { r->x = a->x / b->x; r->y = a->y / b->y; }\
VM_API void m##divc(m *r, const m *a, t s) { r->x = a->x / s; r->y = a->y / s; }
#define VM_VEC2F_IMPL(m,t)VM_VEC2_IMPL(m,t)VM_VECF_IMPL(m,t)\
VM_API t m##dot(const m *a, const m *b) { return a->x * b->x + a->y * b->y; }

VM_VEC4F_IMPL(vec4, float)
VM_VEC4F_IMPL(dvec4, double)
VM_VEC4_IMPL(ivec4, int)
VM_VEC4_IMPL(uvec4, unsigned)

VM_VEC3F_IMPL(vec3, float)
VM_VEC3F_IMPL(dvec3, double)
VM_VEC3_IMPL(ivec3, int)
VM_VEC3_IMPL(uvec3, unsigned)

VM_VEC2F_IMPL(vec2, float)
VM_VEC2F_IMPL(dvec2, double)
VM_VEC2_IMPL(ivec2, int)
VM_VEC2_IMPL(uvec2, unsigned)

VM_API void mat4set(mat4 *r, const mat4 *m)
{
	int i;
	for (i = 0; i < 16; ++i) r->d[i] = m->d[i];
}
VM_API void mat4sets(mat4 *r, float s)
{
	int i, j;
	for (i = 0; i < 4; ++i) for (j = 0; j < 4; ++j) r->n[i][j] = i == j ? s : 0;
}
VM_API void mat4setv(mat4 *r, const vec4 *a, const vec4 *b, const vec4 *c, const vec4 *d)
{
	vec4setv(&r->v[0], a);
	vec4setv(&r->v[1], b);
	vec4setv(&r->v[2], c);
	vec4setv(&r->v[3], d);
}
VM_API void mat4clr(mat4 *m)
{
	mat4sets(m, 1.0f);
}

VM_API void mat4add(mat4 *r, const mat4 *a, const mat4 *b)
{
	int i;
	for (i = 0; i < 16; ++i) r->d[i] = a->d[i] + b->d[i];
}
VM_API void mat4addc(mat4 *r, const mat4 *m, float s)
{
	int i;
	for (i = 0; i < 16; ++i) r->d[i] = m->d[i] + s;
}
VM_API void mat4sub(mat4 *r, const mat4 *a, const mat4 *b)
{
	int i;
	for (i = 0; i < 16; ++i) r->d[i] = a->d[i] - b->d[i];
}
VM_API void mat4subc(mat4 *r, const mat4 *m, float s)
{
	int i;
	for (i = 0; i < 16; ++i) r->d[i] = m->d[i] - s;
}
VM_API void mat4mul(mat4 *r, const mat4 *a, const mat4 *b)
{
	mat4 tmp;
	vec4 t1, t2, t3;

	vec4mulc(&t1, &a->v[0], b->v[0].x);
	vec4mulc(&t2, &a->v[1], b->v[0].y);
	vec4mulc(&t3, &a->v[2], b->v[0].z);
	vec4mulc(&tmp.v[0], &a->v[3], b->v[0].w);
	vec4add(&tmp.v[0], &tmp.v[0], &t1);
	vec4add(&tmp.v[0], &tmp.v[0], &t2);
	vec4add(&tmp.v[0], &tmp.v[0], &t3);

	vec4mulc(&t1, &a->v[0], b->v[1].x);
	vec4mulc(&t2, &a->v[1], b->v[1].y);
	vec4mulc(&t3, &a->v[2], b->v[1].z);
	vec4mulc(&tmp.v[1], &a->v[3], b->v[1].w);
	vec4add(&tmp.v[1], &tmp.v[1], &t1);
	vec4add(&tmp.v[1], &tmp.v[1], &t2);
	vec4add(&tmp.v[1], &tmp.v[1], &t3);

	vec4mulc(&t1, &a->v[0], b->v[2].x);
	vec4mulc(&t2, &a->v[1], b->v[2].y);
	vec4mulc(&t3, &a->v[2], b->v[2].z);
	vec4mulc(&tmp.v[2], &a->v[3], b->v[2].w);
	vec4add(&tmp.v[2], &tmp.v[2], &t1);
	vec4add(&tmp.v[2], &tmp.v[2], &t2);
	vec4add(&tmp.v[2], &tmp.v[2], &t3);

	vec4mulc(&t1, &a->v[0], b->v[3].x);
	vec4mulc(&t2, &a->v[1], b->v[3].y);
	vec4mulc(&t3, &a->v[2], b->v[3].z);
	vec4mulc(&tmp.v[3], &a->v[3], b->v[3].w);
	vec4add(&tmp.v[3], &tmp.v[3], &t1);
	vec4add(&tmp.v[3], &tmp.v[3], &t2);
	vec4add(&tmp.v[3], &tmp.v[3], &t3);

	mat4set(r, &tmp);
}
VM_API void mat4mulc(mat4 *r, const mat4 *m, float s)
{
	int i;
	for (i = 0; i < 16; ++i) r->d[i] = m->d[i] * s;
}
VM_API void mat4div(mat4 *r, const mat4 *a, const mat4 *b)
{
	mat4inverse(r, b);
	mat4mul(r, a, r);
}
VM_API void mat4divc(mat4 *r, const mat4 *m, float s)
{
	int i;
	for (i = 0; i < 16; ++i) r->d[i] = m->d[i] / s;
}
int mat4eq(const mat4 *a, const mat4 *b, float epsilon)
{
	int i;
	for (i = 0; i < 16; ++i) {
		float m = a->d[i] - b->d[i];
		if (VABS(m) > epsilon) return 0;
	}
	return 1;
}

VM_API void mat4compmult(mat4 *r, const mat4 *a, const mat4 *b)
{
	int i;
	for (i = 0; i < 16; ++i) r->d[i] = a->d[i] * b->d[i];
}
VM_API void mat4compdiv(mat4 *r, const mat4 *a, const mat4 *b)
{
	int i;
	for (i = 0; i < 16; ++i) r->d[i] = a->d[i] / b->d[i];
}
VM_API void mat4outerproduct(mat4 *r, const vec4 *c, const vec4 *vr)
{
	int i;
	for (i = 0; i < 4; ++i) vec4mulc(&r->v[i], c, vr->d[i]);
}
VM_API float mat4determinant(const mat4 *m)
{
	float sf0 = m->n[2][2] * m->n[3][3] - m->n[3][2] * m->n[2][3];
	float sf1 = m->n[2][1] * m->n[3][3] - m->n[3][1] * m->n[2][3];
	float sf2 = m->n[2][1] * m->n[3][2] - m->n[3][1] * m->n[2][2];
	float sf3 = m->n[2][0] * m->n[3][3] - m->n[3][0] * m->n[2][3];
	float sf4 = m->n[2][0] * m->n[3][2] - m->n[3][0] * m->n[2][2];
	float sf5 = m->n[2][0] * m->n[3][1] - m->n[3][0] * m->n[2][1];

	vec4 dc;
	vec4set(&dc,
		 (m->n[1][1] * sf0 - m->n[1][2] * sf1 + m->n[1][3] * sf2),
		-(m->n[1][0] * sf0 - m->n[1][2] * sf3 + m->n[1][3] * sf4),
		 (m->n[1][0] * sf1 - m->n[1][1] * sf3 + m->n[1][3] * sf5),
		-(m->n[1][0] * sf2 - m->n[1][1] * sf4 + m->n[1][2] * sf5)
	);

	return m->n[0][0] * dc.d[0] + m->n[0][1] * dc.d[1] + m->n[0][2] * dc.d[2] + m->n[0][3] * dc.d[3];
}
VM_API void mat4transpose(mat4 *r, const mat4 *m)
{
	int i, j;
	mat4 tmp;
	for (i = 0; i < 4; ++i) for (j = 0; j < 4; ++j) tmp.n[i][j] = m->n[j][i];
	mat4set(r, &tmp);
}
VM_API void mat4inverse(mat4 *r, const mat4 *m)
{
	float cf00 = m->n[2][2] * m->n[3][3] - m->n[3][2] * m->n[2][3];
	float cf02 = m->n[1][2] * m->n[3][3] - m->n[3][2] * m->n[1][3];
	float cf03 = m->n[1][2] * m->n[2][3] - m->n[2][2] * m->n[1][3];
	float cf04 = m->n[2][1] * m->n[3][3] - m->n[3][1] * m->n[2][3];
	float cf06 = m->n[1][1] * m->n[3][3] - m->n[3][1] * m->n[1][3];
	float cf07 = m->n[1][1] * m->n[2][3] - m->n[2][1] * m->n[1][3];
	float cf08 = m->n[2][1] * m->n[3][2] - m->n[3][1] * m->n[2][2];
	float cf10 = m->n[1][1] * m->n[3][2] - m->n[3][1] * m->n[1][2];
	float cf11 = m->n[1][1] * m->n[2][2] - m->n[2][1] * m->n[1][2];
	float cf12 = m->n[2][0] * m->n[3][3] - m->n[3][0] * m->n[2][3];
	float cf14 = m->n[1][0] * m->n[3][3] - m->n[3][0] * m->n[1][3];
	float cf15 = m->n[1][0] * m->n[2][3] - m->n[2][0] * m->n[1][3];
	float cf16 = m->n[2][0] * m->n[3][2] - m->n[3][0] * m->n[2][2];
	float cf18 = m->n[1][0] * m->n[3][2] - m->n[3][0] * m->n[1][2];
	float cf19 = m->n[1][0] * m->n[2][2] - m->n[2][0] * m->n[1][2];
	float cf20 = m->n[2][0] * m->n[3][1] - m->n[3][0] * m->n[2][1];
	float cf22 = m->n[1][0] * m->n[3][1] - m->n[3][0] * m->n[1][1];
	float cf23 = m->n[1][0] * m->n[2][1] - m->n[2][0] * m->n[1][1];

	vec4 f0, f1, f2, f3, f4, f5;
	vec4 v0, v1, v2, v3;

	vec4 sa = {{ 1.0f, -1.0f, 1.0f, -1.0f }}, sb = {{ -1.0f, 1.0f, -1.0f, 1.0f }};
	vec4 i0, i1, i2, i3;
	vec4 t1, t2;
	vec4 r0, d0;

	mat4 inv;

	vec4set(&f0, cf00, cf00, cf02, cf03);
	vec4set(&f1, cf04, cf04, cf06, cf07);
	vec4set(&f2, cf08, cf08, cf10, cf11);
	vec4set(&f3, cf12, cf12, cf14, cf15);
	vec4set(&f4, cf16, cf16, cf18, cf19);
	vec4set(&f5, cf20, cf20, cf22, cf23);

	vec4set(&v0, m->n[1][0], m->n[0][0], m->n[0][0], m->n[0][0]);
	vec4set(&v1, m->n[1][1], m->n[0][1], m->n[0][1], m->n[0][1]);
	vec4set(&v2, m->n[1][2], m->n[0][2], m->n[0][2], m->n[0][2]);
	vec4set(&v3, m->n[1][3], m->n[0][3], m->n[0][3], m->n[0][3]);

	vec4mul(&i0, &v1, &f0);
	vec4mul(&t1, &v2, &f1);
	vec4mul(&t2, &v3, &f2);
	vec4sub(&i0, &i0, &t1);
	vec4add(&i0, &i0, &t2);

	vec4mul(&i1, &v0, &f0);
	vec4mul(&t1, &v2, &f3);
	vec4mul(&t2, &v3, &f4);
	vec4sub(&i1, &i1, &t1);
	vec4add(&i1, &i1, &t2);

	vec4mul(&i2, &v0, &f1);
	vec4mul(&t1, &v1, &f3);
	vec4mul(&t2, &v3, &f5);
	vec4sub(&i2, &i2, &t1);
	vec4add(&i2, &i2, &t2);

	vec4mul(&i3, &v0, &f2);
	vec4mul(&t1, &v1, &f4);
	vec4mul(&t2, &v2, &f5);
	vec4sub(&i3, &i3, &t1);
	vec4add(&i3, &i3, &t2);

	vec4mul(&i0, &i0, &sa);
	vec4mul(&i1, &i1, &sb);
	vec4mul(&i2, &i2, &sa);
	vec4mul(&i3, &i3, &sb);

	mat4setv(&inv, &i0, &i1, &i2, &i3);
	vec4set(&r0, inv.n[0][0], inv.n[1][0], inv.n[2][0], inv.n[3][0]);
	vec4mul(&d0, &m->v[0], &r0);

	mat4mulc(r, &inv, 1.0f / (d0.x + d0.y + d0.z + d0.w));
}

VM_API void mat4translate(mat4 *r, const mat4 *m, const vec3 *v)
{
	vec4 t1, t2;
	mat4 tmp;
	mat4set(&tmp, m);
	vec4mulc(&tmp.v[3], &m->v[0], v->x);
	vec4mulc(&t1, &m->v[1], v->y);
	vec4mulc(&t2, &m->v[2], v->z);
	vec4add(&t1, &t1, &t2);
	vec4add(&tmp.v[3], &tmp.v[3], &t1);
	vec4add(&tmp.v[3], &tmp.v[3], &m->v[3]);
	mat4set(r, &tmp);
}
VM_API void mat4rotate(mat4 *r, const mat4 *m, float radians, const vec3 *v)
{
	float c = (float)vm_cos((double)radians);
	float s = (float)vm_sin((double)radians);

	mat4 tmp;
	vec3 axis, t;
	vec4 t1, t2;
	mat4 rotate;

	vec3normalize(&axis, v);
	vec3mulc(&t, &axis, 1.0f - c);

	rotate.n[0][0] = c + t.d[0] * axis.d[0];
	rotate.n[0][1] = t.d[0] * axis.d[1] + s * axis.d[2];
	rotate.n[0][2] = t.d[0] * axis.d[2] - s * axis.d[1];

	rotate.n[1][0] = t.d[1] * axis.d[0] - s * axis.d[2];
	rotate.n[1][1] = c + t.d[1] * axis.d[1];
	rotate.n[1][2] = t.d[1] * axis.d[2] + s * axis.d[0];

	rotate.n[2][0] = t.d[2] * axis.d[0] + s * axis.d[1];
	rotate.n[2][1] = t.d[2] * axis.d[1] - s * axis.d[0];
	rotate.n[2][2] = c + t.d[2] * axis.d[2];

	vec4mulc(&tmp.v[0], &m->v[0], rotate.n[0][0]);
	vec4mulc(&t1, &m->v[1], rotate.n[0][1]);
	vec4mulc(&t2, &m->v[2], rotate.n[0][2]);
	vec4add(&tmp.v[0], &tmp.v[0], &t1);
	vec4add(&tmp.v[0], &tmp.v[0], &t2);

	vec4mulc(&tmp.v[1], &m->v[0], rotate.n[1][0]);
	vec4mulc(&t1, &m->v[1], rotate.n[1][1]);
	vec4mulc(&t2, &m->v[2], rotate.n[1][2]);
	vec4add(&tmp.v[1], &tmp.v[1], &t1);
	vec4add(&tmp.v[1], &tmp.v[1], &t2);

	vec4mulc(&tmp.v[2], &m->v[0], rotate.n[2][0]);
	vec4mulc(&t1, &m->v[1], rotate.n[2][1]);
	vec4mulc(&t2, &m->v[2], rotate.n[2][2]);
	vec4add(&tmp.v[2], &tmp.v[2], &t1);
	vec4add(&tmp.v[2], &tmp.v[2], &t2);

	vec4setv(&tmp.v[3], &m->v[3]);
	mat4set(r, &tmp);
}
VM_API void mat4scale(mat4 *r, const mat4 *m, const vec3 *v)
{
	vec4mulc(&r->v[0], &m->v[0], v->x);
	vec4mulc(&r->v[1], &m->v[1], v->y);
	vec4mulc(&r->v[2], &m->v[2], v->z);
	vec4setv(&r->v[3], &m->v[3]);
}

VM_API void mat4lookat(mat4 *m, const vec3 *campos, const vec3 *targetpos, const vec3 *up)
{
	vec3 f, s, u;
	vec3sub(&f, targetpos, campos);
	vec3normalize(&f, &f);
	vec3cross(&s, &f, up);
	vec3normalize(&s, &s);
	vec3cross(&u, &s, &f);
	vec4set(&m->v[0], s.x, u.x, -f.x, 0);
	vec4set(&m->v[1], s.y, u.y, -f.y, 0);
	vec4set(&m->v[2], s.z, u.z, -f.z, 0);
	vec4set(&m->v[3], -vec3dot(&s, campos), -vec3dot(&u, campos), vec3dot(&f, campos), 1.0f);
}
VM_API void mat4perspective(mat4 *m, float fovy, float aspect, float znear, float zfar)
{
	float t = (float)vm_sin((double)fovy * 0.5) / (float)vm_cos((double)fovy * 0.5), r = zfar - znear;
	mat4sets(m, 0);
	m->n[0][0] = 1.0f / (aspect * t);
	m->n[1][1] = 1.0f / t;
	m->n[2][2] = -(zfar + znear) / r;
	m->n[2][3] = -1.0f;
	m->n[3][2] = -(2.0f * zfar * znear) / r;
}
VM_API void mat4orthographic(mat4 *m, float left, float right, float bottom, float top, float znear, float zfar)
{
	float r = zfar - znear;
	mat4sets(m, 1.0f);
	m->n[0][0] = 2.0f / (right - left);
	m->n[1][1] = 2.0f / (top - bottom);
	m->n[2][2] = -2.0f / r;
	m->n[3][0] = -(right + left) / (right - left);
	m->n[3][1] = -(top + bottom) / (top - bottom);
	m->n[3][2] = -(zfar + znear) / r;
}

/* ---------- Random number generation ---------- */

VM_API vm_u64 vrandom(vrandom_st *s)
{
	#define rotl(x, k) ((x << k) | (x >> (64 - k)))
	const vm_u64 x = s->s[0] + s->s[3], r = rotl(x, 23) + s->s[0], t = s->s[1] << 17;
	s->s[2] ^= s->s[0];
	s->s[3] ^= s->s[1];
	s->s[1] ^= s->s[2];
	s->s[0] ^= s->s[3];
	s->s[3] = rotl(s->s[3], 45);
	s->s[2] ^= t;
	return r;
	#undef rotl
}
VM_API void vsrandom(vrandom_st *s, vm_u64 seed)
{
	vm_u64 *p, *e = s->s + 4;
	for (p = s->s; p != e; ++p) {
		vm_u64 r = (seed += VM_U64C(0x9E3779B97F4A7C15));
		r = (r ^ (r >> 30)) * VM_U64C(0xBF58476D1CE4E5B9);
		r = (r ^ (r >> 27)) * VM_U64C(0x94D049BB133111EB);
		*p = r ^ (r >> 31);
	}
}
VM_API vm_u64 vrandomr(vrandom_st *s, vm_u64 min, vm_u64 max)
{
	vm_u64 o;
	if (min > max || !max) return 0;
	o = max - min + 1;
	return min + vrandom(s) / (VRAND_MAX / VMAX(o, max) + 1);
}
VM_API double vrandomrf(vrandom_st *s, double min, double max)
{
	vm_u64 d = vrandom(s);
	return min + (double)d / ((double)VRAND_MAX / (max - min) + 1.0);
}


#ifdef _WIN32
# ifdef WINAPI
#  define VM_WINAPI WINAPI
#  define VM_IMPORT WINBASEAPI
typedef HMODULE VM_HMODULE;
typedef FARPROC VM_FARPROC;
# elif defined(VM_MINGW)
#  define VM_WINAPI __attribute__((__stdcall__))
#  define VM_IMPORT __attribute__((__dllimport__))
typedef struct { int unused; } *VM_HMODULE;
typedef int (VM_WINAPI *VM_FARPROC)(void);
# elif defined(_MSC_VER)
#  define VM_WINAPI __stdcall
#  define VM_IMPORT __declspec(dllimport)
typedef void *VM_HMODULE;
typedef int (VM_WINAPI *VM_FARPROC)(void);
# endif

extern VM_IMPORT int VM_WINAPI FreeLibrary(VM_HMODULE hlib);
extern VM_IMPORT VM_HMODULE VM_WINAPI LoadLibraryA(const char *fname);
extern VM_IMPORT VM_FARPROC VM_WINAPI GetProcAddress(VM_HMODULE hmod, const char *pname);

# undef VM_WINAPI
# undef VM_IMPORT

VM_API int vrand_os_init(vrand_os_st *s)
{
	s->advapi = (void *)LoadLibraryA("Advapi32.dll");
	if (!s->advapi) return 0;
#if defined(VM_MINGW)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wcast-function-type"
#endif
	s->pfn_rtlgr = (vm_pfn_RtlGenRandom)GetProcAddress((VM_HMODULE)s->advapi, "SystemFunction036");
#if defined(VM_MINGW)
# pragma GCC diagnostic pop
#endif
	if (!s->pfn_rtlgr) return 0;
	return 1;
}
VM_API void vrand_os_destroy(vrand_os_st *s)
{
	if (s->advapi) FreeLibrary((VM_HMODULE)s->advapi);
}
VM_API vm_u64 vrand_os(vrand_os_st *s, void *buf, vm_u64 n)
{
	unsigned char *b = (unsigned char *)buf;
	vm_u64 o = n;
	while (n) {
		unsigned long l = (unsigned long)(n & UINT32_MAX);
		if (s->pfn_rtlgr(b, l) == 0) break;
		n -= (vm_u64)l;
		b += l;
	}
	return o - n;
}
# elif VM_HASINC(<stdio.h>)
#  include <stdio.h>
VM_API int vrand_os_init(vrand_os_st *s) { return (s->urandom = (void *)fopen("/dev/urandom", "r")) != NULL; }
VM_API void vrand_os_destroy(vrand_os_st *s) { fclose((FILE *)s->urandom); }
VM_API vm_u64 vrand_os(vrand_os_st *s, void *buf, vm_u64 n) { return (vm_u64)fread(buf, 1, n, (FILE *)s->urandom); }
# else
VM_API int vrand_os_init(vrand_os_st *s) { (void)s; return 1; }
VM_API void vrand_os_destroy(vrand_os_st *s) { (void)s; }
VM_API vm_u64 vrand_os(vrand_os_st *s, void *buf, vm_u64 n) { (void)s; (void)buf; (void)n; return 0; }
# endif

/* ---------- Noise generation ---------- */

VM_API void vnoise_init(vnoise *n, vm_u64 seed)
{
	static const unsigned char vnoise_init_default_permutation[512] = {
		23, 125, 161, 52, 103, 117, 70, 37, 247, 101, 203, 169, 124, 126, 44, 123,
		152, 238, 145, 45, 171, 114, 253, 10, 192, 136, 4, 157, 249, 30, 35, 72,
		175, 63, 77, 90, 181, 16, 96, 111, 133, 104, 75, 162, 93, 56, 66, 240,
		8, 50, 84, 229, 49, 210, 173, 239, 141, 1, 87, 18, 2, 198, 143, 57,
		225, 160, 58, 217, 168, 206, 245, 204, 199, 6, 73, 60, 20, 230, 211, 233,
		94, 200, 88, 9, 74, 155, 33, 15, 219, 130, 226, 202, 83, 236, 42, 172,
		165, 218, 55, 222, 46, 107, 98, 154, 109, 67, 196, 178, 127, 158, 13, 243,
		65, 79, 166, 248, 25, 224, 115, 80, 68, 51, 184, 128, 232, 208, 151, 122,
		26, 212, 105, 43, 179, 213, 235, 148, 146, 89, 14, 195, 28, 78, 112, 76,
		250, 47, 24, 251, 140, 108, 186, 190, 228, 170, 183, 139, 39, 188, 244, 246,
		132, 48, 119, 144, 180, 138, 134, 193, 82, 182, 120, 121, 86, 220, 209, 3,
		91, 241, 149, 85, 205, 150, 113, 216, 31, 100, 41, 164, 177, 214, 153, 231,
		38, 71, 185, 174, 97, 201, 29, 95, 7, 92, 54, 254, 191, 118, 34, 221,
		131, 11, 163, 99, 234, 81, 227, 147, 156, 176, 17, 142, 69, 12, 110, 62,
		27, 255, 0, 194, 59, 116, 242, 252, 19, 21, 187, 53, 207, 129, 64, 135,
		61, 40, 167, 237, 102, 223, 106, 159, 197, 189, 215, 137, 36, 32, 22, 5,
		23, 125, 161, 52, 103, 117, 70, 37, 247, 101, 203, 169, 124, 126, 44, 123,
		152, 238, 145, 45, 171, 114, 253, 10, 192, 136, 4, 157, 249, 30, 35, 72,
		175, 63, 77, 90, 181, 16, 96, 111, 133, 104, 75, 162, 93, 56, 66, 240,
		8, 50, 84, 229, 49, 210, 173, 239, 141, 1, 87, 18, 2, 198, 143, 57,
		225, 160, 58, 217, 168, 206, 245, 204, 199, 6, 73, 60, 20, 230, 211, 233,
		94, 200, 88, 9, 74, 155, 33, 15, 219, 130, 226, 202, 83, 236, 42, 172,
		165, 218, 55, 222, 46, 107, 98, 154, 109, 67, 196, 178, 127, 158, 13, 243,
		65, 79, 166, 248, 25, 224, 115, 80, 68, 51, 184, 128, 232, 208, 151, 122,
		26, 212, 105, 43, 179, 213, 235, 148, 146, 89, 14, 195, 28, 78, 112, 76,
		250, 47, 24, 251, 140, 108, 186, 190, 228, 170, 183, 139, 39, 188, 244, 246,
		132, 48, 119, 144, 180, 138, 134, 193, 82, 182, 120, 121, 86, 220, 209, 3,
		91, 241, 149, 85, 205, 150, 113, 216, 31, 100, 41, 164, 177, 214, 153, 231,
		38, 71, 185, 174, 97, 201, 29, 95, 7, 92, 54, 254, 191, 118, 34, 221,
		131, 11, 163, 99, 234, 81, 227, 147, 156, 176, 17, 142, 69, 12, 110, 62,
		27, 255, 0, 194, 59, 116, 242, 252, 19, 21, 187, 53, 207, 129, 64, 135,
		61, 40, 167, 237, 102, 223, 106, 159, 197, 189, 215, 137, 36, 32, 22, 5,
	};

	vrandom_st s;
	int i;
	vsrandom(&s, seed);

	vm_memcpy(n->p, vnoise_init_default_permutation, sizeof vnoise_init_default_permutation);

	for (i = 0; i < 512; ++i) {
		vm_u64 r = vrandomr(&s, 0, (vm_u64)i);
		unsigned char tmp = n->p[i];
		n->p[i] = n->p[r];
		n->p[r] = tmp;
	}
}

static VM_INLINE VM_ATTRIBCONST float vnoise3d_grad(int id, float x, float y, float z)
{
	static const unsigned char vnoise3d_grad_itable[512] = {
		7, 9, 5, 0, 11, 1, 6, 9, 3, 9, 11, 1, 8, 10, 4, 7,
		8, 6, 1, 5, 3, 10, 9, 10, 0, 8, 4, 1, 5, 2, 7, 8,
		7, 11, 9, 10, 1, 0, 4, 7, 5, 0, 11, 6, 1, 4, 2, 8,
		8, 10, 4, 9, 9, 2, 5, 7, 9, 1, 7, 2, 2, 6, 11, 5,
		5, 4, 6, 9, 0, 1, 1, 0, 7, 6, 9, 8, 4, 10, 3, 1,
		2, 8, 8, 9, 10, 11, 5, 11, 11, 2, 6, 10, 3, 4, 2, 4,
		9, 10, 3, 2, 6, 3, 6, 10, 5, 3, 4, 10, 11, 2, 9, 11,
		1, 11, 10, 4, 9, 4, 11, 0, 4, 11, 4, 0, 0, 0, 7, 6,
		10, 4, 1, 3, 11, 5, 3, 4, 2, 9, 1, 3, 0, 1, 8, 0,
		6, 7, 8, 7, 0, 4, 6, 10, 8, 2, 3, 11, 11, 8, 0, 2,
		4, 8, 3, 0, 0, 10, 6, 1, 2, 2, 4, 5, 6, 0, 1, 3,
		11, 9, 5, 5, 9, 6, 9, 8, 3, 8, 1, 8, 9, 6, 9, 11,
		10, 7, 5, 6, 5, 9, 1, 3, 7, 0, 2, 10, 11, 2, 6, 1,
		3, 11, 7, 7, 2, 1, 7, 3, 0, 8, 1, 1, 5, 0, 6, 10,
		11, 11, 0, 2, 7, 0, 10, 8, 3, 5, 7, 1, 11, 1, 0, 7,
		9, 0, 11, 5, 10, 3, 2, 3, 5, 9, 7, 9, 8, 4, 6, 5,
		7, 9, 5, 0, 11, 1, 6, 9, 3, 9, 11, 1, 8, 10, 4, 7,
		8, 6, 1, 5, 3, 10, 9, 10, 0, 8, 4, 1, 5, 2, 7, 8,
		7, 11, 9, 10, 1, 0, 4, 7, 5, 0, 11, 6, 1, 4, 2, 8,
		8, 10, 4, 9, 9, 2, 5, 7, 9, 1, 7, 2, 2, 6, 11, 5,
		5, 4, 6, 9, 0, 1, 1, 0, 7, 6, 9, 8, 4, 10, 3, 1,
		2, 8, 8, 9, 10, 11, 5, 11, 11, 2, 6, 10, 3, 4, 2, 4,
		9, 10, 3, 2, 6, 3, 6, 10, 5, 3, 4, 10, 11, 2, 9, 11,
		1, 11, 10, 4, 9, 4, 11, 0, 4, 11, 4, 0, 0, 0, 7, 6,
		10, 4, 1, 3, 11, 5, 3, 4, 2, 9, 1, 3, 0, 1, 8, 0,
		6, 7, 8, 7, 0, 4, 6, 10, 8, 2, 3, 11, 11, 8, 0, 2,
		4, 8, 3, 0, 0, 10, 6, 1, 2, 2, 4, 5, 6, 0, 1, 3,
		11, 9, 5, 5, 9, 6, 9, 8, 3, 8, 1, 8, 9, 6, 9, 11,
		10, 7, 5, 6, 5, 9, 1, 3, 7, 0, 2, 10, 11, 2, 6, 1,
		3, 11, 7, 7, 2, 1, 7, 3, 0, 8, 1, 1, 5, 0, 6, 10,
		11, 11, 0, 2, 7, 0, 10, 8, 3, 5, 7, 1, 11, 1, 0, 7,
		9, 0, 11, 5, 10, 3, 2, 3, 5, 9, 7, 9, 8, 4, 6, 5,
	};
	static const float vnoise3d_grad_ptable[12][4] = {
		{ 1, 1, 0 }, { -1, 1, 0 }, { 1,-1, 0 }, { -1,-1, 0 },
		{ 1, 0, 1 }, { -1, 0, 1 }, { 1, 0,-1 }, { -1, 0,-1 },
		{ 0, 1, 1 }, {  0,-1, 1 }, { 0, 1,-1 }, {  0,-1,-1 },
	};

	const float *g = vnoise3d_grad_ptable[vnoise3d_grad_itable[id]];
	return g[0] * x + g[1] * y + g[2] * z;
}

VM_API VM_ATTRIBPURE float vnoise3d(const vnoise *n, double x, double y, double z)
{
	#define vnoise3d_lerp(a, b, t) (a + (b - a) * t)
	#define vnoise3d_ease(w) (((w * 6.0f - 15.0f) * w + 10) * w * w * w)
	#define vnoise3d_floor(w) ((fr = (vm_i64)w), (fr - ((double)fr > w)))

	vm_i64 fr, ix = vnoise3d_floor(x), iy = vnoise3d_floor(y), iz = vnoise3d_floor(z);
	int x0 = ix & 255, x1 = (ix + 1) & 255, y0 = iy & 255, y1 = (iy + 1) & 255, z0 = iz & 255, z1 = (iz + 1) & 255;
	float fx = (float)(x - (double)ix), fy = (float)(y - (double)iy), fz = (float)(z - (double)iz);
	float u = vnoise3d_ease(fx), v = vnoise3d_ease(fy), w = vnoise3d_ease(fz);
	int r0 = n->p[x0], r1 = n->p[x1];
	int r00 = n->p[r0+y0], r01 = n->p[r0+y1], r10 = n->p[r1+y0], r11 = n->p[r1+y1];

	float n000 = vnoise3d_grad(r00+z0,fx-0,fy-0,fz-0);
	float n001 = vnoise3d_grad(r00+z1,fx-0,fy-0,fz-1.f);
	float n010 = vnoise3d_grad(r01+z0,fx-0,fy-1.f,fz-0);
	float n011 = vnoise3d_grad(r01+z1,fx-0,fy-1.f,fz-1.f);
	float n100 = vnoise3d_grad(r10+z0,fx-1.f,fy-0,fz-0);
	float n101 = vnoise3d_grad(r10+z1,fx-1.f,fy-0,fz-1.f);
	float n110 = vnoise3d_grad(r11+z0,fx-1.f,fy-1.f,fz-0);
	float n111 = vnoise3d_grad(r11+z1,fx-1.f,fy-1.f,fz-1.f);

	float n00 = vnoise3d_lerp(n000,n001,w);
	float n01 = vnoise3d_lerp(n010,n011,w);
	float n10 = vnoise3d_lerp(n100,n101,w);
	float n11 = vnoise3d_lerp(n110,n111,w);

	float n0 = vnoise3d_lerp(n00,n01,v);
	float n1 = vnoise3d_lerp(n10,n11,v);

	return vnoise3d_lerp(n0,n1,u);

	#undef vnoise3d_floor
	#undef vnoise3d_lerp
	#undef vnoise3d_ease
}

VM_API VM_ATTRIBPURE float vnoise3d_fractal(const vnoise *n, double x, double y, double z, int octaves, float lacunarity, float gain)
{
	float total = 0, amplitude = 0.5f;
	double frequency = 0.5;
	while (octaves--) {
		total += amplitude * vnoise3d(n, x * frequency, y * frequency, z * frequency);
		frequency *= (double)lacunarity;
		amplitude *= gain;
	}
	return total;
}

#endif

#undef VM_HASINC
#undef VM_API
#undef VM_CVER
#undef VM_INLINE
#undef VM_THREADLOCAL
#undef VM_NOINLINE_IMPL
#undef VM_MINGW
#undef VM_LONGLONG

#undef VM_ATTRIBCONST
#undef VM_ATTRIBPURE
#undef VM_EXTENSION

#undef VM_RSTATEW_START
#undef VM_RSTATEW_END
#undef VM_PACK_START
#undef VM_PACK_MID
#undef VM_PACK_END

#undef VM_VEC_DEF
#undef VM_VECF_DEF
#undef VM_VEC4_SET
#undef VM_VEC3_SET
#undef VM_VEC2_SET
#undef VM_VEC4F_DEF
#undef VM_VEC4_DEF
#undef VM_VEC3F_DEF
#undef VM_VEC3_DEF
#undef VM_VEC2F_DEF
#undef VM_VEC2_DEF

#undef VM_VECF_IMPL
#undef VM_VEC_IMPL
#undef VM_VEC4F_IMPL
#undef VM_VEC4_IMPL
#undef VM_VEC3F_IMPL
#undef VM_VEC3_IMPL
#undef VM_VEC2F_IMPL
#undef VM_VEC2_IMPL

#ifdef __cplusplus
}
#endif

#endif /* VMATHLIB_HDR */
