#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "glfw_fb/glfw_fb.h"
#include "m_math.h"
#define WIDTH 1000
#define HEIGHT 300

void sanity_check();

// types

struct V3 {
	float x;
	float y;
	float z;

	//
	float length();
	float squared_length();
	void make_unit_vector();
	float dot(const V3 &v2);
	V3 cross(const V3 &v2);
	V3 operator-();
	V3& operator+=(V3 &v2);
	V3& operator-=(V3 &v2);
	V3& operator*=(V3 &v2);
	V3& operator/=(V3 &v2);
	V3& operator*=(const float t);
	V3& operator/=(const float t);
};

V3 v3(float x, float y, float z) {
	V3 r;
	r.x = x;
	r.y = y;
	r.z = z;
	return r;
}

V3 v3(float val) {
	V3 r;
	r.x = val;
	r.y = val;
	r.z = val;
	return r;
}

struct ray {
	V3 origin;
	V3 direction;

	ray(V3 iorigin, V3 idirection) {origin = iorigin; direction = idirection;}
	V3 point_at(float t);
};

struct hit {
	float t;
	V3 p;
	V3 normal;
	V3 color;
	int object_index;
};

// operators

// V3
V3 operator+(const V3 &v1, const V3 &v2) {
	V3 r;
	r.x = v1.x + v2.x;  
	r.y = v1.y + v2.y; 
	r.z = v1.z + v2.z;
	return r;
}

V3 operator+(const V3 &v1, float val) {
	V3 r;
	r.x = v1.x + val;  
	r.y = v1.y + val; 
	r.z = v1.z + val;
	return r;
}

//@note should declare this in an order invariant way to not repeat this
V3 operator+(float val, const V3 &v1) {
	V3 r;
	r.x = v1.x + val;  
	r.y = v1.y + val; 
	r.z = v1.z + val;
	return r;
}

V3 operator-(const V3 &v1, const V3 &v2) {
	V3 r;
	r.x = v1.x - v2.x;  
	r.y = v1.y - v2.y; 
	r.z = v1.z - v2.z;
	return r;
}

V3 operator*(const V3 &v1, const V3 &v2) {
	V3 r;
	r.x = v1.x * v2.x;  
	r.y = v1.y * v2.y; 
	r.z = v1.z * v2.z;
	return r;
}

V3 operator/(const V3 &v1, const V3 &v2) {
	V3 r;
	r.x = v1.x / v2.x;  
	r.y = v1.y / v2.y; 
	r.z = v1.z / v2.z;
	return r;
}

V3 operator*(float t, const V3 &v) {
	V3 r;
	r.x = t * v.x;  
	r.y = t * v.y; 
	r.z = t * v.z;
	return r;	
}

V3 operator*(const V3 &v, float t) {
	V3 r;
	r.x = t * v.x;  
	r.y = t * v.y; 
	r.z = t * v.z;
	return r;	
}

V3 operator/(const V3 &v, float t) {
	V3 r;
	r.x =  v.x / t;  
	r.y =  v.y / t; 
	r.z =  v.z / t;
	return r;	
}

V3 V3::operator-() {
    return v3(-x, -y, -z);
}

V3& V3::operator+=(V3 &v2) {
	x += v2.x;
	y += v2.y;
	z += v2.z;
	return *this;
}

V3& V3::operator-=(V3 &v2) {
	x -= v2.x;
	y -= v2.y;
	z -= v2.z;
	return *this;	
}

V3& V3::operator*=(V3 &v2) {
	x *= v2.x;
	y *= v2.y;
	z *= v2.z;
	return *this;
}

V3& V3::operator/=(V3 &v2) {
	x /= v2.x;
	y /= v2.y;
	z /= v2.z;
	return *this;
}

V3& V3::operator*=(const float t) {
	x *= t;
	y *= t;
	z *= t;
	return *this;
}

V3& V3::operator/=(const float t) {
	x /= t;
	y /= t;
	z /= t;
	return *this;	
}

float V3::dot(const V3 &v2) {
	return (x * v2.x + y * v2.y + z * v2.z);
}

float dot(const V3 &v1, const V3 &v2) {
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

V3 V3::cross(const V3 &v2) {
	V3 dest;
	dest.x = y * v2.z - z * v2.y;
	dest.y = z * v2.x - x * v2.z;
	dest.z = x * v2.y - y * v2.x;
	return dest;
}

float V3::length() {
	return sqrtf(x * x + y * y + z * z);
}

float V3::squared_length() {
	return (x * x + y * y + z * z);
}

void V3::make_unit_vector() {
	float l = length();
	x /= l;
	y /= l;
	z /= l;
}

// ray
V3 ray::point_at(float t) {return origin + t * direction;}

void ray_copy(ray* ray, V3 origin, V3 direction) {
	ray->origin = origin;
	ray->direction = direction;
}

void f3_print(const char* message, V3 v) {
	printf("%s %f %f %f\n", message, v.x, v.y, v.z);
}

V3 f3_unit_vector(V3 v) {
	return v / v.length();
}

void hit_print(const char* message, hit h) {
	printf("%s t:%f\n", message, h.t);
	f3_print("p: ", h.p);
	f3_print("normal: ", h.normal);
}

enum ObjectType {
	SPHERE,
};


typedef struct SphereObject {
	V3 center;
	V3 color;
	float radius;
}SphereObject;


typedef struct Object {
	ObjectType type;
	int index;
	union {
		SphereObject sphere;
	};
}Object;

typedef struct Hit {
	float t;
	V3 p;
	V3 normal;
}Hit;

Object sphere_object(V3 center, float radius) {
	Object object;
	object.type = SPHERE;
	SphereObject sphere;
	sphere.center = center;
	sphere.radius = radius;
	sphere.color = v3(1,0,0);
	object.sphere = sphere;
	return object;
}

// rendering
#define N_SCENE_OBJECTS 2
Object scene_objects[N_SCENE_OBJECTS];
float width = float(WIDTH);
float height = float(HEIGHT);

unsigned int buffer[WIDTH * HEIGHT];
V3 rays_lower_left_corner = v3(-2.0, -1.0, -1.0);
V3 rays_horizontal = v3(width / 100.0, 0.0, 0.0);
V3 rays_vertical = v3(0.0, height / 100.0, 0.0);
V3 rays_origin = v3(0.0, 0.0, 0.0);
float u = 0;
float v = 0;

int fb_get_texture_width() {
	return WIDTH;
}

int fb_get_texture_height() {
	return HEIGHT;
}

bool hit_sphere_simple(SphereObject sphere, ray& r) {
	V3 oc = r.origin - sphere.center;
	float a = dot(r.direction, r.direction);
	float b = 2.0 * dot(oc, r.direction);
	float c = dot(oc, oc) - sphere.radius * sphere.radius;
	float discriminant = b * b - 4 * a * c;
	return discriminant > 0;
}

bool hit_sphere(SphereObject sphere, ray& r, float min, float max, Hit& hit) {
	V3 oc = r.origin - (sphere.center + 0.04f);
	float a = dot(r.direction, r.direction);
	float b = 2.0 * dot(oc, r.direction);
	float c = dot(oc, oc) - sphere.radius * sphere.radius;
	float discriminant = b * b - 4 * a * c;
	if (discriminant < 0) {
		return false;
	}
	/*float ct = (-b - sqrt(discriminant)) / (2.0 * a);
	if (ct < max && ct > min) {
		hit.t = ct;
		hit.p = r.point_at(ct);
		hit.normal = (hit.p - sphere.center) / sphere.radius;
		return true;
	}
	ct = (-b + sqrt(discriminant)) / (2.0 * a);
	if (ct < max && ct > min) {
		hit.t = ct;
		hit.p = r.point_at(ct);
		hit.normal = (hit.p - sphere.center) / sphere.radius;
		return true;
	}*/
	hit.t = (-b - sqrt(discriminant)) / (2.0 * a);
	hit.p = f3_unit_vector(r.point_at(hit.t));
	hit.normal = f3_unit_vector((hit.p - sphere.center));
	return true;
}

V3 compute_color(ray& r, int timestep) {
	float hit_min = 0;
	float hit_closest = 9999999.0;
	bool hit_anything = false;
	Hit hit_info;
	
	for (int i = 0; i < N_SCENE_OBJECTS; ++i) {
		Object obj = scene_objects[i];
		switch (obj.type) {
			case SPHERE:
				scene_objects[i].sphere.center.y = 1.1*sin(scene_objects[i].sphere.center.z * timestep * 0.1);
				Hit hit;
				bool is_hit = hit_sphere(obj.sphere, r, hit_min, hit_closest, hit);
				if (is_hit && hit.t < hit_closest) {
					hit_closest = hit.t;
					hit_info = hit;
					hit_anything = true;
				}
		}
	}

	if (hit_anything) {
		//return V3(hit, 0.0, 0.0);
		return 0.5 * (1.0f + hit_info.normal);
	}

	// background
	V3 direction = f3_unit_vector(r.direction);
	float t = 0.5 * (direction.y + 1.0);
	return (1.0 - t) * v3(1.0) + t * v3(0.5, 0.7, 1.0);
}

unsigned int* fb_update(int timestep) {
	for (int i = 0; i < WIDTH; ++i) {
		for (int j = 0; j < HEIGHT; ++j) {
			u = float(i) / width;
			v = float(j) / height;
			
			ray r(rays_origin, rays_lower_left_corner + u * rays_horizontal + v * rays_vertical);
			V3 color = compute_color(r, timestep);

			buffer[i + j * WIDTH] = FB_RGB(int(color.x * 255), int(color.y * 255), int(color.z * 255));	
			
		}
	}

	return buffer;
}

int main(int argc, char const *argv[]) {
	sanity_check();

	printf("Hi\n");

	for (int i = 0; i < N_SCENE_OBJECTS; ++i) {
		scene_objects[i] = sphere_object(v3(i*-1.4,i*-1.1, -3.0 - i * 0.9), 1.0);
	}


	int error = fb_start("Window title", WIDTH, HEIGHT);
	if (error) {
		printf("error, exiting\n");
		return 1;
	}
	fb_close();


	return 0;
}

void sanity_check() {
	V3 a = v3(1.0, 1.0, 1.0);
	V3 b = v3(1.0, 1.0, 1.0);

	//// operators
	//V3 operator+(const V3 &v1, const V3 &v2)
	V3 c = a + b;
	assert(c.x == 2.0);

	//V3 operator-(const V3 &v1, const V3 &v2) 
	V3 d = c - a;
	assert(d.x == 1.0);

	//V3 operator*(const V3 &v1, const V3 &v2) 
	V3 a1 = v3(11.0, 11.0, 11.0);	
	V3 e = a1 * c;
	assert(e.y == 22.0);

	//V3 operator/(const V3 &v1, const V3 &v2) 
	V3 k1 = e / c;
	assert(k1.z == 11.0);

	//V3 operator*(float t, const V3 &v) 
	V3 k2 = 100.0 * b;
	assert(k2.x == 100.0);

	//V3 operator*(const V3 &v, float t) 
	V3 k3 = b * 200.0;
	assert(k3.x == 200.0);

	//V3 operator/(const V3 &v, float t) 
	V3 k4 = v3(100.0, 100.0, 100.0);
	V3 k5 = k4 / 2.0f;
	assert(k5.x == 50.0);

	//V3 V3::operator-() 
	k5 = -k5;
	assert(k5.x == -50.0);
	k5 = -k5;
	assert((k5).x == 50.0);
	//V3& V3::operator+=(V3 &v2) 
	V3 k6 = v3(10.0, 10.0, 10.0);
	k5+=k6;
	assert(k5.x == 60.0);

	//V3& V3::operator-=(V3 &v2) 
	k5-=k6;
	assert(k5.x == 50.0);
	//V3& V3::operator*=(V3 &v2) 
	k5*=k6;
	assert(k5.x == 500.0);
	//V3& V3::operator/=(V3 &v2) 
	k5/=k6;
	assert(k5.x == 50.0);
	//V3& V3::operator*=(const float t) 
	k5*=2.0;
	assert(k5.x == 100.0);
	//V3& V3::operator/=(const float t) 
	k5/=2.0;
	assert(k5.x == 50.0);


	assert(dot(v3(1, 3, -5), v3(4, -2, -1)) == 3);
	assert(dot(v3(1, 2, 3), v3(4, 5, 6)) == 32);
	assert(v3(1, 3, -5).dot(v3(4, -2, -1)) == 3);

}