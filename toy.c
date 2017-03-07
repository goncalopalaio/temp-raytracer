#include <stdio.h>
#include <GLFW/glfw3.h>

#define M_MATH_IMPLEMENTATION
#define M_IMAGE_IMPLEMENTATION
#define M_COLOR_IMPLEMENTATION
#include "m_math.h"
#include "m_image.h"
#include "m_color.h"
#include "ctoy_srgb.h"

#define SCALAR_ADD2(dest, A, SC) (dest).x = (A).x + (SC); (dest).y = (A).y + (SC);
#define SCALAR_ADD(dest, A, SC) SCALAR_ADD2(dest, A, SC) (dest).z = (A).z + (SC);
#define SCALAR_MULT2(dest, A, SC) (dest).x = (A).x * (SC); (dest).y = (A).y * (SC);
#define SCALAR_MULT(dest, A, SC) SCALAR_MULT2(dest, A, SC) (dest).z = (A).z * (SC);
#define SCALAR_DIV2(dest, A, SC) (dest).x = (A).x / (SC); (dest).y = (A).y / (SC);
#define SCALAR_DIV(dest, A, SC) SCALAR_DIV2(dest, A, SC) (dest).z = (A).z / (SC);


typedef struct {
	float3 origin;
	float3 direction;
}ray;

typedef struct {
	float3 center;
	float3 color;
	float radius;

	int object_index;
}sphere;

typedef struct {
	float t;
	float3 p;
	float3 normal;
	float3 color;
	int object_index;
}hit_information;

typedef struct {
	float3 origin;
	float3 lower_left_corner;
	float3 horizontal;
	float3 vertical;
	float3 u;
	float3 v;
	float3 w;
	float lens_radius;
}camera;


ray make_ray(float3 origin, float3 direction) {
	ray r = {origin, direction};
	return r;
}

void copy_ray(ray* ray, float3 origin, float3 direction) {
	ray->origin = origin;
	ray->direction = direction;
}

void vec_print(const char* message, float3 vec){
	printf("%s %f %f %f\n", message,vec.x,vec.y,vec.z);
}

void hit_print(const char* message, hit_information hit) {
	printf("--------%s-----\n", message);
	printf("t:%f \n", hit.t);
	vec_print("p:",hit.p);
	vec_print("normal:",hit.normal);
}

void ray_print(const char* message, ray ray) {
	printf("%s ray -> origin: %f %f %f | dir: %f %f %f\n", message, ray.origin.x,ray.origin.y,ray.origin.z, ray.direction.x,ray.direction.y,ray.direction.z );	
}

float3 vec_cross(float3 a, float3 b) {
	float3 res;
	M_CROSS3(res, a, b);
	return res;
}

float vec_dot(float3 a, float3 b) {
	return M_DOT3(a,b);
}
float3 vec_add(float3 a, float3 b) {
	float3 res;
	M_ADD3(res,a,b);
	//vec_print("vec_add", res);
	return res;
}


float3 vec_sub(float3 a, float3 b) {
	float3 res;
	M_SUB3(res,a,b);
	return res;
}

float3 vec_mult(float3 a, float3 b) {
	float3 res;
	M_MUL3(res,a,b);
	//vec_print("vec_mult", res);
	return res;
}

float3 vec_mult_3(float3 a, float3 b, float3 c) {
	float3 res;
	M_MUL3(res,a,b);
	M_MUL3(res,res,c);
	return res;
}

float3 vec_sub_3(float3 a, float3 b, float3 c) {
	float3 res;
	M_SUB3(res,a,b);
	M_SUB3(res,res,b);
	return res;
}

float3 vec_add_3(float3 a, float3 b, float3 c) {
	float3 res;
	res.x = a.x + b.x + c.x;
	res.y = a.y + b.y + c.y;
	res.z = a.z + b.z + c.z;
	return res;
}

float3 vec_sub_4(float3 a, float3 b, float3 c, float3 d) {
	float3 res;
	res = vec_sub_3(a,b,c);
	M_SUB3(res,res,d);
	return res;
}


float3 scalar_mult(float3 a, float val) {
	float3 res;
	SCALAR_MULT(res,a,val);

	//vec_print("scalar_mult", res);
	return res;
}

float3 scalar_div(float3 a, float val) {
	float3 res;
	SCALAR_DIV(res,a,val);

	//vec_print("scalar_div", res);
	return res;
}

float3 scalar_add(float3 a, float val) {
	float3 res;
	SCALAR_ADD(res,a,val);
	//vec_print("scalar_add", res);
	return res;
}

float3 vec_normalize(float3 a){
	float len = M_LENGHT3(a);

	return scalar_div(a,len);
}

float vec_norm(float3 v) {
	return M_LENGHT3(v);
}

float3 point_at_ray_param(ray ray, float t) {
	return vec_add(ray.origin, scalar_mult(ray.direction, t));
}

int sphere_check_hit(sphere sphere, ray r, float t_min, float t_max, hit_information* hit) {
	float sphere_radius = sphere.radius;
	float3 sphere_center= sphere.center;
	float3 ray_direction = r.direction; 
	float3 ray_origin = r.origin;

	float3 oc = vec_sub(ray_origin, sphere_center);
	float a = M_DOT3(ray_direction, ray_direction);
	float b = M_DOT3(oc, ray_direction);
	float c = M_DOT3(oc,oc) - sphere_radius * sphere_radius;
	float discriminant = b*b - a*c;

	if (discriminant > 0){
		// get (t) - the offset where the ray hit the sphere
		float temp = (-b - sqrt(discriminant))/ a;
		if (temp < t_max && temp > t_min){
			// t - offset of the ray
			hit->t = temp;
			// p - point of ray at t
			hit->p = point_at_ray_param(r, hit->t);
			// normal - normal at sphere point
			hit->normal = scalar_div(vec_sub(hit->p, sphere_center), sphere_radius);
			
			hit->color = sphere.color;
			hit->object_index = sphere.object_index;
			return 1;
		}
		
		temp = (-b + sqrt(discriminant))/ a;
		
		if (temp < t_max && temp > t_min){
			// t - offset of the ray
			hit->t = temp;
			// p - point of ray at t
			hit->p = point_at_ray_param(r, hit->t);
			// normal - normal at sphere point
			hit->normal = scalar_div(vec_sub(hit->p, sphere_center), sphere_radius);

			hit->color = sphere.color;
			hit->object_index = sphere.object_index;
			return 1;
		}
	}

	return -1;
}

void set_val(float3* color, float x,float y, float z) {
	color->x = x;
	color->y = y;
	color->z = z;
}

float hit_sphere(sphere sphere, ray r, hit_information* hit ) {
	float3 center = sphere.center;
	float radius = sphere.radius;

	float3 oc = vec_sub(r.origin, center);
	float a = M_DOT3(r.direction, r.direction);
	float b = 2.0 * M_DOT3(oc, r.direction);
	float c = M_DOT3(oc,oc) - radius * radius;
	float discriminant = b*b - 4*a*c;

	// get (t) - the offset where the ray hit the sphere
	if (discriminant < 0){
		return -1.0;
	}

	float t_temp = (-b - sqrt(discriminant))/ (2.0*a);

	hit->t = t_temp;
	// p - point of ray at t
	hit->p = point_at_ray_param(r, hit->t);
	// normal - normal at sphere point
	hit->normal = scalar_div(vec_sub(hit->p, center), radius);

	hit->color = sphere.color; // todo we could only pass the index to the sphere
	return t_temp;
}

char main_vert_src[] =
"attribute vec2 aVertex;\n"
"varying vec2 vTexcoord;"
"void main()"
"{"
" vec2 p = aVertex.xy-0.5;"
" vTexcoord = vec2(aVertex.x, aVertex.y);"
" gl_Position = vec4(p.x*2.0, p.y*2.0, 0.0, 1.0); \n"
"}";                             

char main_frag_src[] =
"#ifdef GL_ES\n"
"precision mediump float;\n"
"#endif\n"
"uniform sampler2D uTexture0;"
"varying vec2 vTexcoord;"
"void main()"
"{"
" gl_FragColor = texture2D(uTexture0, vTexcoord);"
"}";


int mouse_x = 0;
int mouse_y = 0;
float norm_mouse_x = 0;
float norm_mouse_y = 0;

#define _WIN_W 800
#define _WIN_H 400
int win_w = _WIN_W;
int win_h = _WIN_H;
int tex_width = _WIN_W;
int tex_height = _WIN_H;
unsigned long step = 0;

GLuint main_vert_shader;
GLuint main_frag_shader;
GLuint main_prog_object;

GLuint main_texture;

struct m_image buffer = M_IMAGE_IDENTITY();
struct m_image buffer_ubyte = M_IMAGE_IDENTITY();

float3 vec_ones = {1,1,1};
float3 vec_zeroes = {0,0,0};

#define SPHERE_LIST_LEN 4
sphere sphere_list[SPHERE_LIST_LEN];

float float_map(float value, float original_start, float original_end, float new_start, float new_end ) {
	float new_range = new_end - new_start;
	float old_range = original_end - original_start;
	return new_start + new_range * ((value - original_start) / old_range);
}

float3 vec_map(float3 vec, float os,float oe, float ns,float ne) {
	vec.x = float_map(vec.x, os,oe,ns,ne);
	vec.y = float_map(vec.y, os,oe,ns,ne);
	vec.z = float_map(vec.z, os,oe,ns,ne);
	return vec;
}

void l(char * s) {
	printf("%s\n", s);
}
void close_callback(GLFWwindow * window){
   l("close_callback");
}

void size_callback(GLFWwindow * window, int width, int height){
	l("size_callback");
}

void cursorpos_callback(GLFWwindow * window, double x, double y){
	//printf("%s %f %f\n", "cursorpos_callback", x, y);
	mouse_x = x;
	mouse_y = y;

	norm_mouse_x = x/buffer.width - 0.5;
	norm_mouse_y = y/buffer.height - 0.5;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	l("key_callback");

	l("Closing!");
	glfwSetWindowShouldClose(window, 1);
}

void mousebutton_callback(GLFWwindow * window, int button, int action, int mods){
	l("mousebutton_callback");
}

void char_callback(GLFWwindow * window, unsigned int key)
{
	l("char_callback");
}
void error_callback(int error, const char* description){
	printf("%s\n", description);
}

GLuint create_shader(GLenum type, const char *src){
    GLuint shader; GLint compiled;

    shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

void swap_buffer(struct m_image *image){
	if (image){
		glBindTexture(GL_TEXTURE_2D, main_texture);
		

      if (image->type == M_FLOAT) {
         m_image_float_to_srgb(&buffer_ubyte, image);
         glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, buffer_ubyte.width, buffer_ubyte.height, GL_RGB, GL_UNSIGNED_BYTE, buffer_ubyte.data);
      }
      else if (image->type == M_UBYTE) {
         glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image->width, image->height, GL_RGB, GL_UNSIGNED_BYTE, image->data);
      }


		float vertices[8] = {0, 0, 0, 1, 1, 0, 1, 1};

		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, main_texture);
		glUseProgram(main_prog_object);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, vertices);
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glUseProgram(0);
	} else {
		l("No buffer, what happened?");
	}
}

float3 random_in_unit_sphere() {
	float3 res;
	float len = 0;
	do {
		res.x = 2.0 * m_randf() - 1;
		res.y = 2.0 * m_randf() - 1;
		res.z = 2.0 * m_randf() - 1;
		//squared length
		len = res.x*res.x + res.y*res.y + res.z*res.z;
	}while(len >= 1.0);

	return res;
}

float3 random_in_unit_disk() {
	float3 res;
	float3 one_xy = {1,1,0};
	do {
		float3 rand_xy = {m_randf(), m_randf(), 0};
		res = scalar_mult(vec_sub(rand_xy, one_xy), 2.0);

	} while (vec_dot(res,res) >= 1.0);

	return res;
}

void reset_hit_information_values(hit_information* info) {
	float3 def_vals = {-1,-1,-1};
	info-> t = -1;
	info-> p = def_vals;
	info-> normal = def_vals;
	info-> color = def_vals;
}

int diffuse_scatter(ray r, hit_information hit, ray* scattered) {
	float3 normalized_normal = vec_normalize(hit.normal);
	float3 target = vec_add_3(hit.p, normalized_normal, random_in_unit_sphere());
	copy_ray(scattered, hit.p, vec_sub(target, hit.p));
	return 1;
}

float3 reflect(float3 v, float3 n) {
	return vec_sub(v, scalar_mult(n,vec_dot(v,n)*2.0));
}
int metal_scatter(ray ray_in, hit_information hit, ray* scattered) {
	float fuzziness = 0;
	float3 reflected = reflect(vec_normalize(ray_in.direction), hit.normal);
	copy_ray(scattered, hit.p, vec_add(reflected, scalar_mult(random_in_unit_sphere(),fuzziness)));
	return vec_dot(scattered->direction, hit.normal) > 0 ;
}

float3 get_color(ray current_ray, int depth) {
	int hit_anything = 0;
	float t_max = 100000000000;
	hit_information final_hit;

	hit_information hit;
	reset_hit_information_values(&hit);
	reset_hit_information_values(&final_hit);
	//loop through the spheres
	float closest_so_far = t_max;
	for (int i = 0; i < SPHERE_LIST_LEN; ++i){
		float has_hit = sphere_check_hit(sphere_list[i],current_ray,0.1,closest_so_far, &hit);

		if (has_hit > 0) {
			hit_anything = 1;
			closest_so_far = hit.t;
			final_hit = hit;
		}
	}
	
	if (hit_anything > 0.0){
		ray scattered;
		if (depth < 850){
			metal_scatter(current_ray, hit, &scattered);
			return vec_mult(hit.color,get_color(scattered, depth+1));
		}else {
			return vec_zeroes;
		}
		
		//float3 normalized_normal = vec_normalize(hit.normal);
		//float3 target = vec_add_3(hit.p, normalized_normal, random_in_unit_sphere());
		//float3 dir = vec_sub(target, hit.p);
		//ray new_ray = {hit.p, dir};
		//return vec_add(scalar_mult(get_color(new_ray),0.1), scalar_mult(hit.color, 0.1));
	}

	return vec_ones;
}


void image_draw()
{
	float *pixel = (float *)buffer.data;
	float sint = 1;//sin(step * 0.005);
	int y, x;

	float w = buffer.width;
	float h = buffer.height;


	float rot = 0.0314*step;
	float rot2 = 0.01*step;
	int sphere_index = 0;
	sphere sphere1;
	sphere1.radius = 0.1;
	set_val(&sphere1.color, 0.3,0.3,0.1);
	//set_val(&sphere1.center, 2+0.2*sin(step*0.01),1+0.2*cos(step*0.01), 0.0);
	set_val(&sphere1.center, 0.4*cos(rot),-0.2, 0.4*sin(rot));
	sphere1.object_index = (sphere_index++);
	
	sphere sphere2;
	sphere2.radius = 0.3;
	set_val(&sphere2.color, 0.1,0.1,0.1);
	set_val(&sphere2.center,  0.9*sin(rot2), -0.7,-0.2);
	sphere2.object_index = (sphere_index++);

	sphere sphere3;
	sphere3.radius = 0.2;
	set_val(&sphere3.color, 0.1,0.1,0.4);
	set_val(&sphere3.center, 0,-0.2, 0.0);
	sphere3.object_index = (sphere_index++);

	sphere sphere4;
	sphere4.radius = 2.2;
	set_val(&sphere4.color, 0.5,0.1,0.2);
	set_val(&sphere4.center, 0,2.5, 0.0);
	sphere4.object_index = (sphere_index++);



	sphere_list[0] = sphere1;
	sphere_list[1] = sphere2;
	sphere_list[2] = sphere3;
	sphere_list[3] = sphere4;

	float3 zeros = 		{0,0,0};
	float3 origin = 	{0.0, 0.0, -10};
	float3 horizontal = {2.0, 	0, 0};
	float3 vertical = 	{0 , 1 ,0};

	float ns = 10;

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			//todo one loop is sufficient
			
				float u = (((float) x) + m_randf()) / w;
				float v = (((float) y) + m_randf()) / h;

				u = float_map(u,0,1,-1,1);
				v = float_map(v,0,1,-1,1);

				//x: -2 to 2
				//y: -1 to 1
				float3 h_d = scalar_mult(horizontal,u);
				float3 v_d = scalar_mult(vertical,v);
				float3 point_in_plane = vec_add_3(zeros,h_d,v_d); 
				float3 direction = vec_sub(point_in_plane, origin);
				direction = vec_normalize(direction);
				
				ray r = make_ray(origin, direction);
				float3 color = get_color(r,0);

				pixel[0] = color.x;
				pixel[1] = color.y;
				pixel[2] = color.z;			

				pixel += 3;
		}
	}   
}




int main(int argc, char const *argv[]){
	
	l("Starting main");

	sphere* head;
	head = (sphere*) malloc(sizeof(sphere));

	l("end of g_list test");

	GLFWwindow* window;
	if (!glfwInit()){
		l("glfwInit failed");
		return -1;
	}
	window = glfwCreateWindow(win_w, win_h, "Window Name", NULL, NULL);
	if (!window){
		l("glfwCreateWindow failed");
		glfwTerminate();
		return -1;
	}

	// set position
	//glfwSetWindowPos(window, 650, 900);

	l("Setting callbacks");
	glfwSetWindowCloseCallback(window, close_callback);
	glfwSetWindowSizeCallback(window, size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mousebutton_callback);
	glfwSetCharCallback(window, char_callback);
	glfwSetCursorPosCallback(window, cursorpos_callback);
	glfwSetErrorCallback(error_callback);
	l("Setting context");
	glfwMakeContextCurrent(window);


	l("Setting main_texture");
	glGenTextures(1, &main_texture);
	glBindTexture(GL_TEXTURE_2D, main_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	l("Setting main_shader");
	main_vert_shader = create_shader(GL_VERTEX_SHADER, main_vert_src);
	main_frag_shader = create_shader(GL_FRAGMENT_SHADER, main_frag_src);
	main_prog_object = glCreateProgram();
	l("Attaching shaders");
	glAttachShader(main_prog_object, main_vert_shader);
	glAttachShader(main_prog_object, main_frag_shader);
	glBindAttribLocation(main_prog_object, 0, "aVertex");
	glLinkProgram(main_prog_object);
	glUseProgram(main_prog_object);
	l("Setting uniforms");
	glUniform1i(glGetUniformLocation(main_prog_object, "uTexture0"), 0);
	l("Configuring textures");
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glEnable(GL_TEXTURE_2D);

	l("Building buffers");
	m_image_create(&buffer, M_FLOAT, tex_width, tex_height, 3);

	while(!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);


		image_draw();
		swap_buffer(&buffer);

		glfwSwapBuffers(window);
		glfwPollEvents();
		++step;
	}

	l("Calling glfwDestroyWindow");
	glDeleteProgram(main_prog_object);
	glDeleteShader(main_frag_shader);
	glDeleteShader(main_vert_shader);
	glDeleteTextures(1, &main_texture);
	glfwMakeContextCurrent(NULL);
	glfwTerminate();
	window = NULL;
	m_image_destroy(&buffer);
   	m_image_destroy(&buffer_ubyte);


	glfwDestroyWindow(window);

	return 0;
}
