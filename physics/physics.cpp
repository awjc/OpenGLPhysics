#include <Angel.h>
#include <sphere.h>
#include <vector>

#include <images.h>
#include <Camera.h>
#include <Shader.h>
#include <VertexArray.h>
#include <CameraControl.h>
#include <ObjFile.h>
#include <Texture2D.h>
#include <ObjectModel.h>


//char* objFileName = "../models/cube.obj";mat4 initialTransform = mat4();
//char* objFileName = "../models/teapot.obj";mat4 initialTransform = mat4();
//char* objFileName = "../models/utah_teapot_hires.obj";mat4 initialTransform = mat4();
//char* objFileName = "../models/glove_hipoly.obj"; mat4 initialTransform = mat4();
//char* objFileName = "../models/guitar.obj"; mat4 initialTransform = RotateX(-90);
//char* objFileName = "../models/hat.obj";mat4 initialTransform = mat4();
//char* objFileName = "../models/Pencil_OBJ.obj";mat4 initialTransform = Scale(10, 10, 10);
//char* objFileName = "../models/table.obj";mat4 initialTransform = RotateX(-90);
//char* objFileName = "../models/Chair_03.obj";mat4 initialTransform = mat4();
//char* objFileName = "../models/Key_B_02.obj";mat4 initialTransform = mat4();
//char* objFileName = "../models/sphere.obj";mat4 initialTransform = mat4();
char* objFileName = "../models/torus_knot_mtl.obj";mat4 initialTransform = mat4();
//char* objFileName = "../models/bunny.obj";mat4 initialTransform = mat4();
//char* objFileName = "../models/dragon.obj";mat4 initialTransform = mat4();


vec4 lpos = vec4();

Camera * camera;
CameraControl * cameraControl;
Shader* vertexColorShader;
Shader* phongShader;
Shader* silhShader;

ObjectModel* object;
VertexArray* axesVao;

// shrink factor
GLfloat shrink = 1.0;
GLfloat explode = 0.0;
GLfloat tesselate = 0.0;

// current amount of rotation
GLfloat degrees = 0.0;

// degree change in each frame
GLfloat increment = 0.5;

// uses space bar to pause
bool paused = 1;

// transformation of cube into position that is currently being rotated
mat4 currentOrientation;

// current axis of rotation
enum {XAxis, YAxis, ZAxis};
int currentAxis = XAxis;

// elapsed time
int elapsedTime;

// frame rate in millis for 30 frames/sec
const int frameRate = 1000.0 / 30;

// Some material and light properties to try

// Shiny green plastic?
//mat3 material = mat3(
//  vec3(0.0, 0.3, 0.3),  // blue-green in ambient light
//  vec3(0.0, 0.8, 0.0),  // green surface
//  vec3(0.8, 0.8, 0.8)); // specular highlights reflect light color
//GLfloat shininess = 30.0;

// Brass-like material
mat3 material = mat3(
	vec3(0.33, 0.22, 0.03),  // 
	vec3(0.78, 0.57, 0.11),  // 
	vec3(0.99, 0.91, 0.81)); // 
GLfloat shininess = 28.0;

//mat3 material = mat3(
//  vec3(0.33, 0.22, 0.50),  // 
//  vec3(0.18, 0.07, 0.51),  // 
//  vec3(0.09, 0.91, 0.81)); // 
//GLfloat shininess = 28.0;

// White light
mat3 light = mat3(
	vec3(0.4, 0.4, 0.4),
	vec3(1.0, 1.0, 1.0),
	vec3(1.0, 1.0, 1.0));


// nonmoving set of axes
vec3 axes[6] = {
	vec3(0.0, 0.0, 0.0), vec3(0.9, 0.0, 0.0),
	vec3(0.0, 0.0, 0.0), vec3(0.0, 0.9, 0.0), 
	vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 0.9)};

// Axes colored R, G, B
vec4 colorsForAxes[6] = {
	vec4(1.0, 0.0, 0.0, 1.0), vec4(1.0, 0.0, 0.0, 1.0), 
	vec4(0.0, 1.0, 0.0, 1.0), vec4(0.0, 1.0, 0.0, 1.0), 
	vec4(0.0, 0.0, 1.0, 1.0), vec4(0.0, 0.0, 1.0, 1.0) };

void init()
{
	// Geometry shader examples
	// ------------------------

	// - geometry shader does nothing (just creates identical output triangle for each input triangle)
	//phongShader      = new Shader("vshader_phong.glsl", "fshader_phong.glsl", "gshader_passthrough.glsl");

	// - geometry shader does position and lighting calculation (vertex shader just passes normal and position)
	//phongShader      = new Shader("vshader_phong_passthrough.glsl", "fshader_phong.glsl", "gshader_lightingcalc.glsl");

	// - geometry shader can shrink each polygon about its center (use c/C keys) and/or explode along face normal (e/E keys)
	phongShader      = new Shader("vshader_phong_passthrough_with_light.glsl", "fshader_cel.glsl", "gshader_explode.glsl");

	// - geometry shader tesselates each input triangle into three output triangles, potentially moved along original normal (t/T keys)
	//phongShader      = new Shader("vshader_phong_passthrough.glsl", "fshader_phong.glsl", "gshader_spikes.glsl");

	//phongShader      = new Shader("vshader_phong_passthrough.glsl", "fshader_phong.glsl", "gshader_outline.glsl");
	silhShader      = new Shader("vshader_phong_passthrough.glsl", "fshader_silh.glsl", "gshader_silh.glsl");

	// for drawing axes
	vertexColorShader  = new Shader("vshader.glsl", "fshader.glsl");

	// Load model from obj file
	ObjFile* m = new ObjFile(objFileName);
	initialTransform = initialTransform * Translate(-m->GetCenter());

	std::string adjfilename = std::string(objFileName);
	size_t lastdot = adjfilename.find_last_of(".");
    if (lastdot != std::string::npos)
	{
		adjfilename = adjfilename.substr(0, lastdot);
	}
	object = new ObjectModel(m, true, adjfilename + ".adj");

	currentOrientation = Scale(m->GetScaleFactor()); // initial scale for model

	axesVao = new VertexArray();
	axesVao->AddAttribute("vPosition", axes, 6);
	axesVao->AddAttribute("vColor", colorsForAxes, 6);

	camera = new Camera (vec3(0.0, 0.0, 4.0),   // position
		vec3(0.0, 0.0, -1.0),  // forward
		vec3(0.0, 1.0, 0.0),   // up
		1.77f,                  // aspect
		30.0f,                 // fovy
		0.1f,                  // near
		1000.0f);              // far

	cameraControl = new CameraControl(camera);


	glEnable( GL_DEPTH_TEST );
	glClearColor( 0.2, 0.2, 0.2, 1.0 ); 

}

mat4 getCurrentRotation()
{
	return mat4();

	// rotate around the current axis
	mat4 rotation;
	if (currentAxis == XAxis) rotation = RotateX(degrees);
	else if (currentAxis == YAxis) rotation = RotateY(degrees);
	else rotation = RotateZ(degrees);
	return rotation;
}

void display( void )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// increase the rotation angle
	if (!paused)
	{
		degrees += increment;
		while (degrees >= 360.0) degrees -= 360.0;
		while (degrees <= -360.0) degrees += 360.0;
	}

	lpos.x = 2*sin(DegreesToRadians * degrees);
	lpos.y = .5;
	lpos.z = 2*cos(DegreesToRadians * degrees);

	// apply the rotation to the current orientation of the cube
	mat4 model = getCurrentRotation() * currentOrientation * initialTransform;

	mat4 view = camera->GetView();
	mat4 projection = camera->GetProjection();

	// Create normal matrix
	mat4 mv = view * model;
	mat3 normalMatrix = mat3(vec3(mv[0][0], mv[0][1], mv[0][2]),
		vec3(mv[1][0], mv[1][1], mv[1][2]),
		vec3(mv[2][0], mv[2][1], mv[2][2]));


	// Bind shader and set uniforms
	phongShader->Bind();
	phongShader->SetUniform("normalMatrix", normalMatrix);
	phongShader->SetUniform("lightPosition", lpos);
	phongShader->SetUniform("model", model);
	phongShader->SetUniform("view", view);
	phongShader->SetUniform("projection", projection);
	phongShader->SetUniform("materialProperties", material);
	phongShader->SetUniform("lightProperties", light);
	phongShader->SetUniform("shininess", shininess);
	phongShader->SetUniform("shrink", shrink);
	phongShader->SetUniform("explode", explode);
	phongShader->SetUniform("tesselate", tesselate);
	phongShader->SetUniform("black", 0);
	phongShader->Unbind();

	silhShader->Bind();
	silhShader->SetUniform("model", model);
	silhShader->SetUniform("view", view);
	silhShader->SetUniform("projection", projection);
	glLineWidth(10);
	silhShader->Unbind();

	object->Draw(phongShader, true, silhShader);

	// Bind shader for axes and set uniforms
	vertexColorShader->Bind();
	vertexColorShader->SetUniform("transform", projection * view);

	// Bind VAO for axes and draw
	axesVao->Bind(*vertexColorShader);
	glLineWidth(2);
	axesVao->Draw(GL_LINES);

	vertexColorShader->Unbind();
	axesVao->Unbind();

	// instead of glFlush, display the back buffer now
	glutSwapBuffers();

}


void keyboard( unsigned char key, int x, int y )
{
	if (!cameraControl->handleKey(key))
	{
		switch( key ) {
		case 033: // Escape Key
		case 'q': case 'Q':
			exit( EXIT_SUCCESS );
			break; 
			// when we change the axis of rotation, want to take its current
			// position and start rotating about the new axis, so we compose
			// the current rotation with the current orientation matrix
		case 'x':
			currentOrientation = getCurrentRotation() * currentOrientation;
			currentAxis = XAxis;
			degrees = 0;
			break;
		case 'y':
			currentOrientation = getCurrentRotation() * currentOrientation;
			currentAxis = YAxis;
			degrees = 0;
			break;
		case 'z':
			currentOrientation = getCurrentRotation() * currentOrientation;
			currentAxis = ZAxis;
			degrees = 0;
			break;		
		case '+':
			increment += 0.1;
			break;
		case '-':
			increment -= 0.1;
			break;
		case ' ':   // pause
			paused = 1 - paused;
			break;
		case 'c':
			shrink -= 0.05;
			if (shrink <= 0.0) shrink = 0.0;
			break;
		case 'C':
			shrink += 0.05;
			if (shrink >= 1.0) shrink = 1.0;
			break;
		case 'e':
			explode += 0.1;
			break;
		case 'E':
			explode -= 0.1;
			break;
		case 't':
			tesselate += 0.001;
			break;
		case 'T':
			tesselate -= 0.001;
			break;
		}
	}
	glutPostRedisplay();
}

// Needed to get key events for arrow keys
void keyboardSpecial(int key, int x, int y)
{
	cameraControl->handleKeySpecial(key);
	glutPostRedisplay();

}


void timer( int value )
{
	glutPostRedisplay();
	glutTimerFunc(1000/frameRate, timer, 0);
}


int main( int argc, char **argv )
{
	// Allow for models to be specified on the command line
	if (argc >= 2)
	{
		objFileName = argv[1];
	}
	glutInit( &argc, argv );

	// Set up the graphics context with double-buffering
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	glutInitWindowSize( 910, 512 );
	glutCreateWindow( "Physics" );

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboardSpecial);
	//glutIdleFunc( idle );
	glutTimerFunc(1000/frameRate, timer, 0);

	elapsedTime = glutGet(GLUT_ELAPSED_TIME);

	glutMainLoop();
	return 0;
}
