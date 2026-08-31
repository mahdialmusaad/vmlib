## Overview
A header-only general mathematics library.
## Features
- Constants and calculation macros for convenience
- GLSL-compatible mathematical vectors and 4x4 matrix for graphics programming
- Pseudo and cryptographic random number generation
- Noise generators
## Usage
Define `VM_IMPL` before including the library in **exactly one source file**. All other times, you can just include the header normally.

Other options:
- `VM_STATIC`: Make all functions `static`, usually for using the library in one file only.
- `VM_NOINLINE`: Do not add `inline` to the definitions when implementing the functions.
- `VM_NO_MATH_H`: Use the fallback or user-defined math functions, even if `math.h` is found. 

Documentation for individual functions can be found in the header file itself.
## Support
The implementation is written in pure C89, but some macros and structures make use of compiler extensions which are standard in at most C11.<br>
Support for thread-local, inline and attributes (const, pure) are checked for, as well as header availability. It uses `sqrt`, `sin`, `cos` and `tan` for vector/matrix calculations, so make sure to link the math library if needed (usually with `-lm`).<br>
If these are not available, a fallback implementation will be used, but you can override the fallback by defining your own (same function name, prefixed with `vm_`).
## Example
Below is some example usage of the library for different scenarios.
### Vectors
```c
#define VM_IMPL
#include "vmlib.h"

int main(void)
{
	/* Vector structures have the same name as GLSL.
	   Each value has multiple names (unions), which is useful in different contexts.
	   For example, you can use XYZW for positioning, RGBA for colors and STPQ for textures.
	   You can also access it as an array ('d' member) as well. */
	dvec4 four_doubles;
	vec3 three_floats;
	ivec2 two_ints;
	uvec2 two_uints;

	/* Simple operators and macros are available: */

	/* Initialization (replace '4' with the length of the vector, if needed): */
	vec4 a = VEC4(1.0f, 2.0f, 3.0f, 4.0f);
	vec4 b = VEC4(2.0f, 3.0f, 4.0f, 5.0f);
	vec4 c = VEC4I; /* identity init, { 0.0f, 0.0f, 0.0f, 0.0f } */
	vec4 d = VEC4S(2.0f); /* scalar init, { 2.0f, 2.0f, 2.0f, 2.0f } */

	vec4set(&c, 1.0f, 3.0f, 3.0f, 7.0f);
	vec4sets(&c, 2.0f); /* Set scalar - all components are 2.0f now. */
	vec4setv(&c, &d); /* c = d */
	vec4clr(&c); /* All components are 0.0f now. */

	/* Operators: */
	vec4add(&a, &a, &b); /* a = a + b */
	vec4sub(&b, &a, &b); /* b = a - b */
	vec4mul(&a, &b, &b); /* a = b x b (component-wise multiplication) */
	vec4div(&c, &a, &b); /* c = a / b (component-wise division) */
	vec4neg(&c, &c); /* c = -c */

	/* Scalar versions of the operators are also available; same name but with 'c' at the end (e.g. vec4mulc(&a, &b, 2.0)). */

	/* In C99+, compound literals are used so init macros can be used like normal variables.
	   Otherwise (C89), they can only be used for variable initialization. */
	vec4setv(&c, &VEC4(1.0f, 2.0f, 3.0f, 4.0f));
	vec4add(&c, &c, &VEC4(12.0f, 34.0f, 56.0f, 78.0f));

	/* Other vector operations are available as well: */

	float len = vec4length(&a); /* len = length(a) */
	float dist = vec4distance(&a, &b); /* dist = distance(a, b) */
	float dotp = vec4dot(&a, &b); /* dotp = dot(a, b) */

	vec4 n = VEC4(0.0f, 1.0f, 0.0f, 0.0f);
	vec3 v3a = VEC3(3.0f, 8.0f, -2.0f), v3b = VEC3(2.0f, -1.0f, 5.0f);
	float eta = 0.75f;

	vec4normalize(&a, &a); /* a = normalize(a) */
	vec4faceforward(&a, &a, &b, &c); /* a = faceforward(a, b, c) */
	vec4mix(&a, &a, &c, 0.5f); /* a = mix(a, c, 0.5) */
	vec4reflect(&a, &a, &n); /* a = reflect(a, n) */
	vec4refract(&a, &a, &n, eta); /* a = refract(a, n, eta) */

	/* Cross product available for vec3 and dvec3. */
	vec3cross(&v3a, &v3a, &v3b); /* v3a = cross(v3a, v3b) */
}
```
### Matrix
```c
#define VM_IMPL
#include "vmlib.h"

int main(void)
{
	vec4 v = VEC4S(1.0f);
	vec3 t = VEC3(1.0f, 2.0f, 3.0f), c = VEC3I, u = VEC3(0.0f, 1.0f, 0.0f);
	float radians = (float)M_PI * 0.75f;
	float fovy = radians;

	/* Initialization: */
	/* 4x4 matrix is available, as both mat4 and mat4x4. */
	mat4 a = MAT4I; /* Identity matrix, where the diagonals are 1.0f and the others are 0.0f. */
	mat4x4 b = MAT4S(2.0f); /* Specify diagnonal value. */
	mat4 c = MAT4V(v, v, v, v); /* Specify matrix values in terms of 4 vec4s. */
	mat4 r;

	/* Operators: */
	mat4add(&r, &a, &b); /* Component-wise addition: r = a + b. */
	mat4sub(&r, &a, &b); /* Component-wise subtraction: r = a - b. */
	mat4compmult(&r, &a, &b); /* Component-wise multiplication. */
	mat4compdiv(&r, &a, &b); /* Component-wise division. */

	mat4mul(&r, &a, &b); /* Algebraic matrix multiplication: r = a * b. */
	mat4div(&r, &a, &b); /* Algebraic matrix division: r = a / b. */

	/* View transformation: */
	mat4translate(&a, &a, &t); /* Translate matrix a by vector t. */
	mat4rotate(&a, &a, radians, &t); /* Rotate matrix a by axis t with specific radians angle. */
	mat4scale(&a, &a, &t); /* Scale matrix a by vector t. */

	mat4lookat(&a, &c, &t, &u); /* Create view matrix a at c that looks at t, with up direction as u. */
	mat4perspective(&a, fovy, 1.5f, 0.1f, 100.0f); /* Create perspective matrix with clipping and view settings. */
	mat4orthographic(&a, 1.0f, 1.0f, 1.0f, 1.0f, 0.1f, 100.0f); /* Create perspective matrix with clipping and direction settings. */

	/* Other matrix operations are available as well: */
	mat4transpose(&a, &a); /* Transpose of a. */
	mat4inverse(&a, &a); /* Inverse of a. */
}
```
### RNG
```c
#define VM_IMPL
#include "vmlib.h"

int main(void)
{
	/* You can use the standard RNG functions, which use the global state.
	   This provides a similar API to rand(), except it is thread-safe.
	   The maximum value is defined in VRAND_MAX, which is equivalent to UINT64_MAX. */
	uint64_t random_u64 = vrand();

	/* There are also (inclusively) ranged versions of vrand, both for integral and floating-point values. */
	uint64_t ranged_random_u64 = vrandr(0, 1000);
	double random_ranged_double = vrandrf(0.0, 1.0);

	uint64_t seed = 123456;

	/* Like srand, you can also seed the generator for vrand* functions. */
	vsrand(seed);

	/* You can also use the random functions but instead with your own state.
	   The functions are the same as above, but named 'random' instead of 'rand'
	   and take the state as an initial parameter. */
	vrandom_st state;

	vsrandom(&state, seed);
	uint64_t stateful_random = vrandom(&state);
	uint64_t stateful_ranged_random = vrandomr(&state, 0, 1000);
	double stateful_rangedf_random = vrandomrf(&state, 0.0, 1.0);

	/* Finally, there is also a function to get cryptographically secure random numbers.
	   This gets random numbers from a platform-dependent source and returns number of bytes filled.
	   You need to create and destroy a context with vrand_os_init and vrand_os_destroy respectively. */

	char buffer[1024];
	uint64_t bytes = 1024;
	vrand_os_st csprng_state;

	if (!vrand_os_init(&csprng_state)) { /* Failed to initialize. */ }
	else if (vrand_os(&csprng_state, buffer, bytes) != bytes) { /* Failed to generate specified number of random bytes. */ }
	else { /* Succeeded in generating. */ }

	vrand_os_destroy(&csprng_state);
}
```
### Noise
```c
#define VM_IMPL
#include "vmlib.h"

int main(void)
{
	vnoise noise_state;
	float generated[128][2];
	uint64_t seed = 123456;

	/* Initialize the noise permutation table. */
	vnoise_init(&noise_state, seed);

	for (int x = 0; x < 128; ++x) {
		double px = (double)x;
		double py = 0.0;
		double pz = 0.0;

		/* Get the noise value at the given 3D position. */
		generated[x][0] = vnoise3d(&noise_state, px, py, pz);

		/* Fractal noise is also available. Octaves determine the number of noise iterations to make,
		   whereas 'lacunarity' and 'gain' control the relative location and contribution of each iteration respectively. */

		float lacunarity = 2.0f, gain = 0.5f;
		int octaves = 4;
		generated[x][1] = vnoise3d_fractal(&noise_state, px, py, pz, octaves, lacunarity, gain);
	}

	/* No allocation is made, so no 'clear' or 'destroy' function is needed. */
}
```
Running the provided test file will produce multiple 512x512 .ppm images of the fractal noise with a varying number of octaves, as well as one of the default noise function.
