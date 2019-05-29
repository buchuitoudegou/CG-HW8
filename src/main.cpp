#include "headers.h"
#include "create_window/CreateWindow.hpp"
#include <vector>
#include <utility>
#include <cmath>
#include "shader.h"
#include <iostream>

using namespace std;

vector<pair<float, float> > ctrlPoints;
double dynamicT = 0;
// float lastFrame = 0, curFrame = 0;
// bool finish = true;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
vector<pair<float, float> > bezier(vector<pair<float, float> >& points);
double bernstein(int i, int n, double t);
int factorial(int num);
void drawCurve(Shader* shader, vector<pair<float, float> >& points);
void drawLine(Shader*, pair<float, float>&, pair<float, float>&);
void drawDynamicProcedure(Shader*);

int main() {
  GLFWwindow* window = CreateWindow::createWindow();
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  Shader shader("./src/glsl/shader.vs", "./src/glsl/shader.fs");
  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
    if (ctrlPoints.size() > 2) {
      for (int i = 0; i < ctrlPoints.size() - 1; ++i) {
        drawLine(&shader, ctrlPoints[i], ctrlPoints[i + 1]);
      }
      auto bezierPoints = bezier(ctrlPoints);
      drawCurve(&shader, bezierPoints);
      drawDynamicProcedure(&shader);
    }
    glfwSwapBuffers(window);
		glfwPollEvents();
  }
  glfwTerminate();
}

vector<pair<float, float> > bezier(vector<pair<float, float> >& points) {
	vector<pair<float, float>> resultPoints;

	int n = points.size() - 1;
	for (double t = 0; t <= 1; t += 0.00005) {
		pair<float, float> targetPoint;
		for (int i = 0; i <= n; i++) {
			double B = bernstein(i, n, t);
			pair<float, float> curPoint = points[i];
			targetPoint.first += curPoint.first * B;
			targetPoint.second += curPoint.second * B;
		}
		resultPoints.push_back(targetPoint);
	}
	return resultPoints;
}

double bernstein(int i, int n, double t) {
	double B = factorial(n) / (factorial(i) * factorial(n - i));
	B = B * pow(t, i) * pow((1 - t), (n - i));
	return B;
}

int factorial(int num) {
	int ans = 1;
	for (int i = 1; i <= num; i++)
		ans *= i;
	return ans;
}

void drawLine(Shader* shader, pair<float, float>& p1, pair<float, float>& p2) {
  unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
  float vertices[6] = { 
    p1.first, p1.second, 0.0f,
    p2.first, p2.second, 0.0f
  };
  glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
  shader->use();
  shader->setVec4("drawColor", glm::vec4(1, 0, 0, 1));
	glDrawArrays(GL_LINE_STRIP, 0, 2);
	glBindVertexArray(0);
  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);
}

void drawCurve(Shader* shader, vector<pair<float, float> >& points) {
  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
  vector<float> vertices;
  for (int i = 0; i < points.size(); ++i) {
    vertices.push_back(points[i].first);
    vertices.push_back(points[i].second);
    vertices.push_back(0.0f);
  }
  float* glVertices = new float[vertices.size()];
  for (int i = 0; i < vertices.size(); ++i) {
    glVertices[i] = vertices[i];
  }
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * 3 * sizeof(float), glVertices, GL_STREAM_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
  shader->use();
  shader->setVec4("drawColor", glm::vec4(1, 0, 0, 1));
  glDrawArrays(GL_POINTS, 0, points.size());
  glBindVertexArray(0);
  delete[] glVertices;
  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		ctrlPoints.pop_back();
		dynamicT = 0;
    // finish = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double tempX = 0, tempY = 0;
		glfwGetCursorPos(window, &tempX, &tempY);
		tempX = (-1) + (tempX / SCR_WIDTH) * 2;
		tempY = (-1)*((-1) + (tempY / SCR_HEIGHT) * 2);
		pair<float, float> p;
		p.first = tempX;
		p.second = tempY;
		ctrlPoints.push_back(p);
		dynamicT = 0;
    // finish = false;
	}
}

void drawDynamicProcedure(Shader* shader) {
  const double t = dynamicT;
  vector<pair<float, float> > prePoints(ctrlPoints);
  while (prePoints.size() != 0) {
    vector<pair<float, float> > tmp(prePoints);
    prePoints.clear();
    for (int j = 0; j < tmp.size() - 1; ++j) {
      pair<float, float> p;
      auto fir = tmp[j];
      auto sec = tmp[j + 1];
      p.first = t * sec.first + (1 - t) * fir.first;
      p.second = t * sec.second + (1 - t) * fir.second;
      prePoints.push_back(p);
    }
    for (int j = 0; prePoints.size() >= 1 &&  j < prePoints.size() - 1; j++) {
      drawLine(shader, prePoints[j], prePoints[j + 1]);
    }
  }
  if (dynamicT + 0.005 > 1) {
		dynamicT = 1;
  }
	else {
		dynamicT += 0.005;
    // cout << dynamicT << endl;
  }
  // if (dynamicT == 1 && !finish) {
  //   curFrame = glfwGetTime();
  //   cout << "--------------" << curFrame - lastFrame << "--------------" << endl;
  //   lastFrame = curFrame;
  //   finish = true;
  // } else if (dynamicT == 1) {
  //   curFrame = glfwGetTime();
  //   lastFrame = curFrame;
  // }
}