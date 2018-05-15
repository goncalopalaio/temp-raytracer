#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "glfw_fb/glfw_fb.h"
#include "m_math.h"
#define WIDTH 1000
#define HEIGHT 300

void sanity_check();

// types

struct f3 {
	float x;
	float y;
	float z;

	//
	f3() {}
	f3(float val) {x = val; y = val; z = val;}
	f3(float ix, float iy, float iz) {x = ix; y = iy; z = iz;}
	float length();
	float squared_length();
	void make_unit_vector();
	float dot(const f3 &v2);
	f3 cross(const f3 &v2);
	f3 operator-();
	f3& operator+=(f3 &v2);
	f3& operator-=(f3 &v2);
	f3& operator*=(f3 &v2);
	f3& operator/=(f3 &v2);
	f3& operator*=(const float t);
	f3& operator/=(const float t);
};

struct ray {
	f3 origin;
	f3 direction;

	ray(f3 iorigin, f3 idirection) {origin = iorigin; direction = idirection;}
	f3 point_at(float t);
};

struct hit {
	float t;
	f3 p;
	f3 normal;
	f3 color;
	int object_index;
};

// operators

// f3
f3 operator+(const f3 &v1, const f3 &v2) {
	f3 r;
	r.x = v1.x + v2.x;  
	r.y = v1.y + v2.y; 
	r.z = v1.z + v2.z;
	return r;
}

f3 operator-(const f3 &v1, const f3 &v2) {
	f3 r;
	r.x = v1.x - v2.x;  
	r.y = v1.y - v2.y; 
	r.z = v1.z - v2.z;
	return r;
}

f3 operator*(const f3 &v1, const f3 &v2) {
	f3 r;
	r.x = v1.x * v2.x;  
	r.y = v1.y * v2.y; 
	r.z = v1.z * v2.z;
	return r;
}

f3 operator/(const f3 &v1, const f3 &v2) {
	f3 r;
	r.x = v1.x / v2.x;  
	r.y = v1.y / v2.y; 
	r.z = v1.z / v2.z;
	return r;
}

f3 operator*(float t, const f3 &v) {
	f3 r;
	r.x = t * v.x;  
	r.y = t * v.y; 
	r.z = t * v.z;
	return r;	
}

f3 operator*(const f3 &v, float t) {
	f3 r;
	r.x = t * v.x;  
	r.y = t * v.y; 
	r.z = t * v.z;
	return r;	
}

f3 operator/(const f3 &v, float t) {
	f3 r;
	r.x =  v.x / t;  
	r.y =  v.y / t; 
	r.z =  v.z / t;
	return r;	
}

f3 f3::operator-() {
    return f3(-x, -y, -z);
}

f3& f3::operator+=(f3 &v2) {
	x += v2.x;
	y += v2.y;
	z += v2.z;
	return *this;
}

f3& f3::operator-=(f3 &v2) {
	x -= v2.x;
	y -= v2.y;
	z -= v2.z;
	return *this;	
}

f3& f3::operator*=(f3 &v2) {
	x *= v2.x;
	y *= v2.y;
	z *= v2.z;
	return *this;
}

f3& f3::operator/=(f3 &v2) {
	x /= v2.x;
	y /= v2.y;
	z /= v2.z;
	return *this;
}

f3& f3::operator*=(const float t) {
	x *= t;
	y *= t;
	z *= t;
	return *this;
}

f3& f3::operator/=(const float t) {
	x /= t;
	y /= t;
	z /= t;
	return *this;	
}

float f3::dot(const f3 &v2) {
	return (x * v2.x + y * v2.y + z * v2.z);
}

float dot(const f3 &v1, const f3 &v2) {
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

f3 f3::cross(const f3 &v2) {
	f3 dest;
	dest.x = y * v2.z - z * v2.y;
	dest.y = z * v2.x - x * v2.z;
	dest.z = x * v2.y - y * v2.x;
	return dest;
}

float f3::length() {
	return sqrtf(x * x + y * y + z * z);
}

float f3::squared_length() {
	return (x * x + y * y + z * z);
}

void f3::make_unit_vector() {
	float l = length();
	x /= l;
	y /= l;
	z /= l;
}

// ray
f3 ray::point_at(float t) {return origin + t * direction;}

void ray_copy(ray* ray, f3 origin, f3 direction) {
	ray->origin = origin;
	ray->direction = direction;
}

void f3_print(const char* message, f3 v) {
	printf("%s %f %f %f\n", message, v.x, v.y, v.z);
}

f3 f3_unit_vector(f3 v) {
	return v / v.length();
}

void hit_print(const char* message, hit h) {
	printf("%s t:%f\n", message, h.t);
	f3_print("p: ", h.p);
	f3_print("normal: ", h.normal);
}

// rendering

float width = float(WIDTH);
float height = float(HEIGHT);

unsigned int buffer[WIDTH * HEIGHT];
f3 rays_lower_left_corner(-2.0, -1.0, -1.0);
f3 rays_horizontal(width / 100.0, 0.0, 0.0);
f3 rays_vertical(0.0, height / 100.0, 0.0);
f3 rays_origin(0.0, 0.0, 0.0);
float u = 0;
float v = 0;

int fb_get_texture_width() {
	return WIDTH;
}

int fb_get_texture_height() {
	return HEIGHT;
}

struct sphere {
	f3 center;
	f3 color;
	float radius;

	int index;
	sphere(f3 icenter, float iradius) {center = icenter; radius = iradius;}

	bool hit_simple(ray& r) {
		f3 oc = r.origin - center;
		float a = dot(r.direction, r.direction);
		float b = 2.0 * dot(oc, r.direction);
		float c = dot(oc, oc) - radius * radius;
		float discriminant = b * b - 4 * a * c;
		return discriminant > 0;
	}

	float hit(ray& r) {
		//f3_print("ray origin: ", r.origin);
		//f3_print("ray direction: ", r.direction);
		//f3_print("sphere origin: ", center);
		//printf("sphere radius: %f\n", radius);

		f3 oc = r.origin - (center + 0.04);

		//f3_print("oc: ", oc);

		float a = dot(r.direction, r.direction);
		//f3_print("r.dir ", r.direction);		
		//printf("dot: r.dir r.dir %f\n", a);
		
		float b = 2.0 * dot(oc, r.direction);
		float c = dot(oc, oc) - radius * radius;
		float discriminant = b * b - 4 * a * c;
		if (discriminant < 0) {
			return -1.0;
		}
		return (-b - sqrt(discriminant)) / (2.0 * a);
	}
};

f3 compute_color(ray& r) {
	sphere sph = sphere (f3(0.0, 0.0, -1.0), 0.5);

	float hit = sph.hit(r);
	if (hit > 0.0) {
		//return f3(hit, 0.0, 0.0);
		f3 N = f3_unit_vector(r.point_at(hit) - f3(0.0, 0.0, -1.0));
		return 0.5 * (1.0 + N);
	}

	// background
	f3 direction = f3_unit_vector(r.direction);
	float t = 0.5 * (direction.y + 1.0);
	return (1.0 - t) * f3(1.0) + t * f3(0.5, 0.7, 1.0);
}

unsigned int* fb_update(int timestep) {
	for (int i = 0; i < WIDTH; ++i) {
		for (int j = 0; j < HEIGHT; ++j) {
			u = float(i) / width;
			v = float(j) / height;
			
			ray r(rays_origin, rays_lower_left_corner + u * rays_horizontal + v * rays_vertical);
			f3 color = compute_color(r);

			buffer[i + j * WIDTH] = FB_RGB(int(color.x * 255), int(color.y * 255), int(color.z * 255));	
			
		}
	}

	return buffer;
}

int main(int argc, char const *argv[]) {
	sanity_check();

	printf("Hi\n");

	int error = fb_start("Window title", WIDTH, HEIGHT);
	if (error) {
		printf("error, exiting\n");
		return 1;
	}
	fb_close();


	return 0;
}

void sanity_check() {
	f3 a (1.0, 1.0, 1.0);
	f3 b (1.0, 1.0, 1.0);

	//// operators
	//f3 operator+(const f3 &v1, const f3 &v2)
	f3 c = a + b;
	assert(c.x == 2.0);

	//f3 operator-(const f3 &v1, const f3 &v2) 
	f3 d = c - a;
	assert(d.x == 1.0);

	//f3 operator*(const f3 &v1, const f3 &v2) 
	f3 a1 (11.0, 11.0, 11.0);	
	f3 e = a1 * c;
	assert(e.y == 22.0);

	//f3 operator/(const f3 &v1, const f3 &v2) 
	f3 k1 = e / c;
	assert(k1.z == 11.0);

	//f3 operator*(float t, const f3 &v) 
	f3 k2 = 100.0 * b;
	assert(k2.x == 100.0);

	//f3 operator*(const f3 &v, float t) 
	f3 k3 = b * 200.0;
	assert(k3.x == 200.0);

	//f3 operator/(const f3 &v, float t) 
	f3 k4 (100.0, 100.0, 100.0);
	f3 k5 = k4 / 2.0f;
	assert(k5.x == 50.0);

	//f3 f3::operator-() 
	k5 = -k5;
	assert(k5.x == -50.0);
	k5 = -k5;
	assert((k5).x == 50.0);
	//f3& f3::operator+=(f3 &v2) 
	f3 k6 (10.0, 10.0, 10.0);
	k5+=k6;
	assert(k5.x == 60.0);

	//f3& f3::operator-=(f3 &v2) 
	k5-=k6;
	assert(k5.x == 50.0);
	//f3& f3::operator*=(f3 &v2) 
	k5*=k6;
	assert(k5.x == 500.0);
	//f3& f3::operator/=(f3 &v2) 
	k5/=k6;
	assert(k5.x == 50.0);
	//f3& f3::operator*=(const float t) 
	k5*=2.0;
	assert(k5.x == 100.0);
	//f3& f3::operator/=(const float t) 
	k5/=2.0;
	assert(k5.x == 50.0);


	assert(dot(f3(1, 3, -5), f3(4, -2, -1)) == 3);
	assert(dot(f3(1, 2, 3), f3(4, 5, 6)) == 32);
	assert(f3(1, 3, -5).dot(f3(4, -2, -1)) == 3);

}