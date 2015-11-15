#include <fstream>
#include <GL/glew.h>
// #define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
// #include <glm/glm.hpp>
// #include <glm/mat4x4.hpp>
// #include <glm/gtc/type_ptr.hpp>
// #include <glm/gtc/matrix_transform.hpp>

#include "sph.h"

using namespace SPH;

ParticleSystem * psys;
Grid * grid;

extern const char * vertexShader;
extern const char * spherePixelShader;
GLuint shaderProgram;
GLuint posVBO, colorVBO;

//======= params ========
const int side = 15;
const int n = side * side * side;
const float time_step = 0.002f;
const float rhou0 = 1000.0f;
const float k = 18000.0f;
const float miu = 0.6f;
const float h = 0.01f;
const float step = 0.006f;

const float ox = 0.06f;
const float oy = 0.1f;
const float oz = 0.06f; 
//=======================

bool running = false;
bool debugging = false;
float fps = 0.0f;

char title_buf[80];

void init_particles(ParticleSystem *p){

	float3 *pos = new float3[n];

	for(int i = 0, z = 0; z < side; z++)
		for(int y = 0; y < side; y++)
			for(int x = 0; x < side; x++, i++){
				pos[i].x = x * step + ox;
				pos[i].y = y * step + oy;
				pos[i].z = z * step + oz;
			}

	float mass = 1000.0f * pow(step * (side + 1), 3) / n;

	LoadParticles(p, pos, mass, n);

	delete [] pos;
}

template<typename T>
void output(const char *filename, T *v, int size, int n){
	std::ofstream fout(filename);

	for(int i = 0, k = 0; i < n; i++){
		for(int j = 0; j < size; j++, k++){
			fout << v[i] << '\t';
		}
		fout << std::endl;
	}

	fout.close();
}

void debug_output(ParticleSystem * p, Grid * g){
	output("hash.txt", g->hash, 1, p->n_particles);
	output("index.txt", g->index, 1, p->n_particles);
	output("start.txt", g->nb_start, 1, p->n_particles);
	output("end.txt", g->nb_end, 1, p->n_particles);
	output("nb.txt", g->nb, 1, g->nb_end[p->n_particles - 1]);
	output("density.txt", p->density, 1, p->n_particles);

	int *n_nb = new int [p->n_particles];

	for(int i = 0; i < p->n_particles; i++){
		n_nb[i] = g->nb_end[i] - g->nb_start[i];
	}

	output("n_nb2.txt", n_nb, 1, p->n_particles);
}

GLuint compile_shader(const char * vs_src, const char * ps_src){
	GLuint vs, fs, program;

	program = glCreateProgram();

	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_src, NULL);
	glCompileShader(vs);

	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &ps_src, NULL);
	glCompileShader(fs);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);

	int suc = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &suc);
	if(!suc){
		exit(0);
	}

	return program;
}

// void idle_func(){
// 	glutPostRedisplay();
// }

void render_particles(){
	glUseProgram(shaderProgram);

	glUniform1f( glGetUniformLocation(shaderProgram, "pointScale"), 2.0f );
    glUniform1f( glGetUniformLocation(shaderProgram, "pointRadius"), 1.0f );

	glBindBuffer(GL_ARRAY_BUFFER, posVBO);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * psys->n_particles, psys->pos, GL_DYNAMIC_DRAW);

	glVertexPointer(3, GL_FLOAT, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glColorPointer(3, GL_FLOAT, 0, 0);
	glEnableClientState(GL_COLOR_ARRAY);

	glDrawArrays(GL_POINTS, 0, psys->n_particles);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glUseProgram(0);
}

void line_box(const float3 &op, const float3 &dp){
	glBegin(GL_LINES);
	glVertex3f( op.x, op.y, op.z ); glVertex3f( op.x, dp.y, op.z );
	glVertex3f( dp.x, op.y, op.z ); glVertex3f( dp.x, dp.y, op.z );
	glVertex3f( op.x, op.y, dp.z ); glVertex3f( op.x, dp.y, dp.z );
	glVertex3f( dp.x, op.y, dp.z ); glVertex3f( dp.x, dp.y, dp.z );

	glVertex3f( op.x, op.y, op.z ); glVertex3f( dp.x, op.y, op.z );
	glVertex3f( op.x, dp.y, op.z ); glVertex3f( dp.x, dp.y, op.z );
	glVertex3f( op.x, op.y, dp.z ); glVertex3f( dp.x, op.y, dp.z );
	glVertex3f( op.x, dp.y, dp.z ); glVertex3f( dp.x, dp.y, dp.z );

	glVertex3f( op.x, op.y, op.z ); glVertex3f( op.x, op.y, dp.z );
	glVertex3f( dp.x, op.y, op.z ); glVertex3f( dp.x, op.y, dp.z );
	glVertex3f( op.x, dp.y, op.z ); glVertex3f( op.x, dp.y, dp.z );
	glVertex3f( dp.x, dp.y, op.z ); glVertex3f( dp.x, dp.y, dp.z );
	glEnd();
}

const float3 orgpos = make_float3(0.0f, 0.0f, 0.0f);
const float3 endpos = make_float3(0.2f, 0.4f, 0.2f);


void display_func(){

//	static int te = 0, lte = 0;

	if (running) Step(time_step, psys, grid);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// glLoadIdentity();

	// gluLookAt(0.3, 0.2, 0.3, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// box
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	line_box(orgpos, endpos);
	//glColor3f(1.0f, 1.0f, 1.0f);

	// particles
	render_particles();

	// lte = te;
	// te = glutGet(GLUT_ELAPSED_TIME);
	// fps = 1000.0f / (te - lte);
	// sprintf_s(title_buf, "adaptively sampling SPH - %.2f fps", fps);
	// glutSetWindowTitle(title_buf);
}



void keyboard_func(GLFWwindow *win, int key, int scancode, int action, int mods){
	if (action == GLFW_PRESS) {
		
		if( key == GLFW_KEY_E && !running ){
			Step(time_step, psys, grid);
			if(debugging)debug_output(psys, grid);
			printf("s\n");
		}
		if( key == GLFW_KEY_SPACE ){
			running = !running;
			printf("running state switch\n");
		}
		/*if( key == GLFW_KEY_F ){
			sprintf_s(title_buf, "adaptively sampling SPH - %.2f fps", fps);
			glutSetWindowTitle(title_buf);
		}*/
		if( key == GLFW_KEY_D ){
			debugging = !debugging;
			printf("debugging state switch\n");
		}
		
	}
}

void reshape_func(GLFWwindow* win, int w, int h){
	double asp = (h!=0 ? (double)w / h : w);
	
	glMatrixMode(GL_PROJECTION);
	// glm::mat4 persp = glm::perspective(60.0, asp, 0.01, 100.0);
	// glLoadMatrixf(glm::value_ptr(persp));
	glLoadIdentity();
	gluPerspective(60.0, asp, 0.01, 100.0);

	glMatrixMode(GL_MODELVIEW);
	// glm::mat4 view = glm::lookAt(glm::vec3(0.3, 0.2, 0.3), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	// glLoadMatrixf(glm::value_ptr(view));
	glLoadIdentity();
	gluLookAt(0.3, 0.2, 0.3, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	
	glViewport(0, 0, w, h);
}

void error_func(int error, const char* desc) {
	puts(desc);
}

void init_gl(int argc, char **argv){
	// glutInit(&argc, argv);
	// glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	// glutInitWindowSize(800, 600);
	// glutCreateWindow("adaptively sampling SPH");
	// glutShowWindow();

	// glewInit();

	// glutIdleFunc(idle_func);
	// glutDisplayFunc(display_func);
	// glutReshapeFunc(reshape_func);
	// glutKeyboardFunc(keyboard_func);

    glEnable(GL_POINT_SPRITE_ARB);
    glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_NV);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);

	// data
	glClearColor((GLclampf)0.7, (GLclampf)0.7, (GLclampf)0.7, (GLclampf)1.0);
	shaderProgram = compile_shader(vertexShader, spherePixelShader);

	glGenBuffers(1, &posVBO);
	glGenBuffers(1, &colorVBO);

	float *color = new float[psys->n_particles * 3];
	for(int i = 0; i < psys->n_particles; i++){
		color[ i * 3		]	= 0.0f;
		color[ i * 3 + 1	]	= 0.0f;
		color[ i * 3 + 2	]	= 1.0f;
	}

	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * psys->n_particles, color, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int main(int argc, char **argv){
	
	GLFWwindow *window;
	
	if (!glfwInit())
		return -1;
	
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
	
	window = glfwCreateWindow(800, 600, "SPH", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	
	glfwSetErrorCallback(error_func);
	glfwSetWindowSizeCallback(window, reshape_func);
	glfwSetKeyCallback(window, keyboard_func);
	
	glfwMakeContextCurrent(window);
	reshape_func(window, 800, 600);

	puts((const char*)glGetString(GL_VERSION));

	if (GLEW_OK != glewInit())
	{
		glfwTerminate();
		return -1;
	}

	psys = CreateParticleSystem(n, h, k, miu, rhou0);
	grid = CreateGrid(make_float3(-0.1f, -0.1f, -0.1f), make_float3(0.4f, 0.6f, 0.4f), h, psys->max_particles, side * side);

	init_particles(psys);
	init_gl(argc, argv);

	// glutMainLoop();
	while (!glfwWindowShouldClose(window)) {
		
		display_func();
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	DestroyGrid(grid);

	DestroyParticleSystem(psys);

	return 0;

}
