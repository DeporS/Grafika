/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "myCube.h"
#include "myTeapot.h"

float speed_x = 0;
float speed_y = 0;
float aspectRatio = 1;

ShaderProgram* sp;


//Odkomentuj, żeby rysować kostkę
float* vertices = myCubeVertices;
float* normals = myCubeNormals;
float* texCoords = myCubeTexCoords;
float* colors = myCubeColors;
int vertexCount = myCubeVertexCount;


//Odkomentuj, żeby rysować czajnik
//float* vertices = myTeapotVertices;
//float* normals = myTeapotVertexNormals;
//float* texCoords = myTeapotTexCoords;
//float* colors = myTeapotColors;
//int vertexCount = myTeapotVertexCount;

GLuint tex0;
GLuint tex1;
GLuint tex2;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, -4.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


float cameraSpeedW = 0.0f; 
float cameraSpeedS = 0.0f;
float cameraSpeedA = 0.0f;
float cameraSpeedD = 0.0f;
float cameraSpeedSpace = 0.0f;
float cameraSpeedCtrl = 0.0f;
float shiftSpeed = 1.0f;

float lastX = 400, lastY = 300;

float yaw = 90.0f, pitch = 0.0f;

bool firstMouse = true;

int initialHeight = 900;
int initialWidth = 1600;


//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) speed_x = -PI / 2;
		if (key == GLFW_KEY_RIGHT) speed_x = PI / 2;
		if (key == GLFW_KEY_UP) speed_y = PI / 2;
		if (key == GLFW_KEY_DOWN) speed_y = -PI / 2;
		if (key == GLFW_KEY_W) cameraSpeedW = 0.05f;
		if (key == GLFW_KEY_S) cameraSpeedS = 0.05f;
		if (key == GLFW_KEY_A) cameraSpeedA = 0.05f;
		if (key == GLFW_KEY_D) cameraSpeedD = 0.05f;
		if (key == GLFW_KEY_SPACE) cameraSpeedSpace = 0.05f;
		if (key == GLFW_KEY_LEFT_CONTROL) cameraSpeedCtrl = 0.05f;
		if (key == GLFW_KEY_LEFT_SHIFT) shiftSpeed = 3.0f; // shift przyspiesza poruszanie
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT) speed_x = 0;
		if (key == GLFW_KEY_RIGHT) speed_x = 0;
		if (key == GLFW_KEY_UP) speed_y = 0;
		if (key == GLFW_KEY_DOWN) speed_y = 0;
		if (key == GLFW_KEY_W) cameraSpeedW = 0.0f;
		if (key == GLFW_KEY_S) cameraSpeedS = 0.0f;
		if (key == GLFW_KEY_A) cameraSpeedA = 0.0f;
		if (key == GLFW_KEY_D) cameraSpeedD = 0.0f;
		if (key == GLFW_KEY_SPACE) cameraSpeedSpace = 0.0f;
		if (key == GLFW_KEY_LEFT_CONTROL) cameraSpeedCtrl = 0.0f;
		if (key == GLFW_KEY_LEFT_SHIFT) shiftSpeed = 1.0f;
	}
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	if (height == 0) return;
	aspectRatio = (float)width / (float)height;
	glViewport(0, 0, width, height);
}




GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Wczytanie do pamięci komputera
	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
	unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
	//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import do pamięci karty graficznej
	glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
	//Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}


//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);

	sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
	tex0 = readTexture("bricks2_diffuse.png");
	tex1 = readTexture("bricks2_normal.png");
	tex2 = readTexture("bricks2_height.png");
}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************

	delete sp;
}




//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float angle_x, float angle_y) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*glm::mat4 V = glm::lookAt(
		glm::vec3(0, 0, -4),
		glm::vec3(0, 0, -4),
		glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz widoku */
	cameraPos += cameraSpeedW * cameraFront * shiftSpeed;
	cameraPos -= cameraSpeedS * cameraFront * shiftSpeed;
	cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeedA * shiftSpeed;
	cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeedD * shiftSpeed;
	cameraPos += cameraSpeedSpace * glm::vec3(0, 1, 0) * shiftSpeed;
	cameraPos -= cameraSpeedCtrl * glm::vec3(0, 1, 0) * shiftSpeed;

	// Oblicz macierz widoku dla kamery z trzeciej osoby
	glm::vec3 ufoPosition = cameraPos + cameraFront;
	glm::vec3 cameraTarget = ufoPosition; // Punkt, na który kamera patrzy
	glm::mat4 V = glm::lookAt(cameraPos, cameraTarget, cameraUp);

	//glm::mat4 V = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	glm::mat4 P = glm::perspective(50.0f * PI / 180.0f, aspectRatio, 0.01f, 50.0f); //Wylicz macierz rzutowania

	glm::mat4 M = glm::mat4(1.0f);
	M = glm::rotate(M, angle_y, glm::vec3(1.0f, 0.0f, 0.0f)); //Wylicz macierz modelu
	M = glm::rotate(M, angle_x, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu

	sp->use();//Aktywacja programu cieniującego
	//Przeslij parametry programu cieniującego do karty graficznej
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

	glEnableVertexAttribArray(sp->a("vertex"));  //Włącz przesyłanie danych do atrybutu vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices); //Wskaż tablicę z danymi dla atrybutu vertex

	glEnableVertexAttribArray(sp->a("color"));  //Włącz przesyłanie danych do atrybutu color
	glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, colors); //Wskaż tablicę z danymi dla atrybutu color

	glEnableVertexAttribArray(sp->a("normal"));  //Włącz przesyłanie danych do atrybutu normal
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals); //Wskaż tablicę z danymi dla atrybutu normal

	glEnableVertexAttribArray(sp->a("c1"));  //Włącz przesyłanie danych do atrybutu normal
	glVertexAttribPointer(sp->a("c1"), 4, GL_FLOAT, false, 0, myCubeC1); //Wskaż tablicę z danymi dla atrybutu normal

	glEnableVertexAttribArray(sp->a("c2"));  //Włącz przesyłanie danych do atrybutu normal
	glVertexAttribPointer(sp->a("c2"), 4, GL_FLOAT, false, 0, myCubeC2); //Wskaż tablicę z danymi dla atrybutu normal

	glEnableVertexAttribArray(sp->a("c3"));  //Włącz przesyłanie danych do atrybutu normal
	glVertexAttribPointer(sp->a("c3"), 4, GL_FLOAT, false, 0, myCubeC3); //Wskaż tablicę z danymi dla atrybutu normal

	glEnableVertexAttribArray(sp->a("texCoord0"));  //Włącz przesyłanie danych do atrybutu texCoord
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords); //Wskaż tablicę z danymi dla atrybutu texCoord

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);

	glUniform1i(sp->u("textureMap1"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex1);

	glUniform1i(sp->u("textureMap2"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tex2);

	glDrawArrays(GL_TRIANGLES, 0, vertexCount); //Narysuj obiekt

	glDisableVertexAttribArray(sp->a("vertex"));  //Wyłącz przesyłanie danych do atrybutu vertex
	glDisableVertexAttribArray(sp->a("color"));  //Wyłącz przesyłanie danych do atrybutu color
	glDisableVertexAttribArray(sp->a("normal"));  //Wyłącz przesyłanie danych do atrybutu normal
	glDisableVertexAttribArray(sp->a("texCoord0"));  //Wyłącz przesyłanie danych do atrybutu texCoord0




	// Podloga
	glm::mat4 M2 = glm::mat4(1.0f);
	M2 = glm::translate(M2, glm::vec3(0.0f, 0.0f, 0.0f)); // Przesuń drugi obiekt wzdłuż osi x
	M2 = glm::scale(M2, glm::vec3(25.0f, 1.0f, 25.0f));
	
	// Przekaż nową macierz modelu (M2) do programu cieniującego
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M2));

	// Przekaż inne dane dla drugiego obiektu do atrybutów programu cieniującego
	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices);

	glEnableVertexAttribArray(sp->a("color"));
	glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, colors);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords);

	// Wywołaj glDrawArrays dla drugiego obiektu, aby go narysować
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);

	// Wyłącz przesyłanie danych do atrybutów dla drugiego obiektu
	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("color"));
	glDisableVertexAttribArray(sp->a("normal"));
	glDisableVertexAttribArray(sp->a("texCoord0"));

	// Budynek 1 // bez colors i normals (cokolwiek to zmienia)
	glm::mat4 M3 = glm::mat4(1.0f);
	M3 = glm::translate(M3, glm::vec3(0.0f, 2.0f, 0.0f)); // Przesuń drugi obiekt wzdłuż osi x
	M3 = glm::scale(M3, glm::vec3(2.0f, 10.0f, 2.0f));
	// Przekaż nową macierz modelu (M2) do programu cieniującego
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M3));

	// Przekaż inne dane dla drugiego obiektu do atrybutów programu cieniującego
	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords);

	// Wywołaj glDrawArrays dla drugiego obiektu, aby go narysować
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);

	// Wyłącz przesyłanie danych do atrybutów dla drugiego obiektu
	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("texCoord0"));

	// Budynek 2
	glm::mat4 M4 = glm::mat4(1.0f);
	M4 = glm::translate(M4, glm::vec3(10.0f, 2.0f, 7.0f)); // Przesuń drugi obiekt wzdłuż osi x
	M4 = glm::scale(M4, glm::vec3(2.0f, 10.0f, 2.0f));
	// Przekaż nową macierz modelu (M2) do programu cieniującego
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M4));

	// Przekaż inne dane dla drugiego obiektu do atrybutów programu cieniującego
	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices);

	glEnableVertexAttribArray(sp->a("color"));
	glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, colors);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords);

	// Wywołaj glDrawArrays dla drugiego obiektu, aby go narysować
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);

	// Wyłącz przesyłanie danych do atrybutów dla drugiego obiektu
	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("color"));
	glDisableVertexAttribArray(sp->a("normal"));
	glDisableVertexAttribArray(sp->a("texCoord0"));

	

	// UFO, wokol ktorego bedzie sie obracac kamera
	glm::mat4 M6 = glm::mat4(1.0f);
	M6 = glm::translate(M6, ufoPosition); // Przesuń drugi obiekt wzdłuż osi x
	M6 = glm::scale(M6, glm::vec3(0.1f,0.1f,0.1f));

	// Przekaż nową macierz modelu (M2) do programu cieniującego
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M6));

	// Przekaż inne dane dla drugiego obiektu do atrybutów programu cieniującego
	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices);

	glEnableVertexAttribArray(sp->a("color"));
	glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, colors);

	glEnableVertexAttribArray(sp->a("normal"));
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords);

	// Wywołaj glDrawArrays dla drugiego obiektu, aby go narysować
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);

	// Wyłącz przesyłanie danych do atrybutów dla drugiego obiektu
	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("color"));
	glDisableVertexAttribArray(sp->a("normal"));
	glDisableVertexAttribArray(sp->a("texCoord0"));


	glfwSwapBuffers(window); //Przerzuć tylny bufor na przedni
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) // initially set to true
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	const float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
};


int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(initialWidth, initialHeight, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.
	windowResizeCallback(window, initialWidth, initialHeight); // skalowanie obrazu aby odpowiadał rozmiarom okna

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Ukrycie kursora podczas wlaczenia aplikacji
	glfwSetCursorPosCallback(window, mouse_callback); // Wyslanie wiadomosci o tym ze mysz sie ruszyla
	

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	//Główna pętla
	float angle_x = 0; //Aktualny kąt obrotu obiektu
	float angle_y = 0; //Aktualny kąt obrotu obiektu
	glfwSetTime(0); //Zeruj timer
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		angle_x += speed_x * glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
		angle_y += speed_y * glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwSetTime(0); //Zeruj timer
		drawScene(window, angle_x, angle_y); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}