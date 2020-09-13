
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"
#include "Sphere.cpp"
#include "SceneObject.cpp"
#include "Ray.cpp"
#include <GL/glut.h>
#include "Plane.cpp"
#include "Cone.cpp"
#include "TextureBMP.cpp"
#include "Cylinder.cpp"
#include "glm/ext.hpp"
using namespace std;

const float WIDTH = 20.0;
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 800;
const int MAX_STEPS = 5;
float cell = 0.5;
const float XMIN = -WIDTH * cell;
const float XMAX =  WIDTH * cell;
const float YMIN = -HEIGHT * cell;
const float YMAX =  HEIGHT * cell;

TextureBMP texture1;
TextureBMP texture2;

vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene


void Cube(float x, float y, float z, float size_1, float size_2, float size_3, glm::vec3 color)
{
    glm::vec3 point_1 = glm::vec3(x,y,z);
    glm::vec3 point_2 = glm::vec3(x+size_1,y,z);
    glm::vec3 point_3 = glm::vec3(x+size_1,y+size_3,z);
    glm::vec3 point_4 = glm::vec3(x,y+size_3,z);

    glm::vec3 point_5 = glm::vec3(x+size_1,y,z-size_2);
    glm::vec3 point_6 = glm::vec3(x+size_1,y+size_3,z-size_2);
    glm::vec3 point_7 = glm::vec3(x,y+size_3,z-size_2);
    glm::vec3 point_8 = glm::vec3(x,y,z-size_2);

    Plane *plane1 = new Plane(point_1, point_2, point_3, point_4, color);
    Plane *plane2 = new Plane(point_2, point_5, point_6 ,point_3, color);
    Plane *plane3 = new Plane(point_5, point_8, point_7, point_6, color);
    Plane *plane4 = new Plane(point_4, point_7, point_8, point_1, color);
    Plane *plane5 = new Plane(point_4, point_3, point_6, point_7, color);
    Plane *plane6 = new Plane(point_8, point_5, point_2, point_1, color);

    sceneObjects.push_back(plane1);
    sceneObjects.push_back(plane2);
    sceneObjects.push_back(plane3);
    sceneObjects.push_back(plane4);
    sceneObjects.push_back(plane5);
    sceneObjects.push_back(plane6);

}

void floor(){
    Plane *plane = new Plane (glm::vec3(-60., -20, -40),//Point A
                            glm::vec3(60., -20, -40),//Point B
                            glm::vec3(60., -20, -200),//Point C
                            glm::vec3(-60., -20, -200),//Point D
                            glm::vec3(0.5, 0.5, 0));//Colour

    sceneObjects.push_back(plane);

}

void background(){
    Plane *wall = new Plane (glm::vec3(-70., -20, -200),//Point A
                            glm::vec3(70., -20, -200),//Point B
                            glm::vec3(70., 50, -200),//Point C
                            glm::vec3(-70., 50, -200),//Point D
                            glm::vec3(0,0,0));//Colour

    //wall->isReflect = true;
    sceneObjects.push_back(wall);

}


glm::vec3 trace(Ray ray, int step)
{
    glm::vec3 backgroundCol(0);

    glm::vec3 light1(100, 100, 5);    //multiple light sources
    glm::vec3 light2(-100, 100, 5);

    glm::vec3 ambientCol(0.1);   //Ambient color of light

    ray.closestPt(sceneObjects);        //Compute the closest point of intersetion of objects with the ray

    if(ray.xindex == -1) return backgroundCol;      //If there is no intersection return background colour
    glm::vec3 materialCol = sceneObjects[ray.xindex]->getColor(); //else return object's colour

    // question 2 starts here
    glm::vec3 lightVector1 = light1 - ray.xpt;
    glm::vec3 lightVector2 = light2 - ray.xpt;

    glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt);
    normalVector = glm::normalize(normalVector);
    float lightDist1 = glm::length(lightVector1);    // before normalize the unit light vector
    float lightDist2 = glm::length(lightVector2);

    lightVector1 = glm::normalize(lightVector1);
    lightVector2 = glm::normalize(lightVector2);

    float lDotn = glm::dot(lightVector1, normalVector);
    float lDotn1 = glm::dot(lightVector2, normalVector);


    /* plus specularTerm to below colorSum*/
    glm::vec3 colorSum;
    glm::vec3 viewVector = -ray.dir;

    glm::vec3 reflVector = glm::reflect(-lightVector1, normalVector);
    glm::vec3 reflVector2 = glm::reflect(-lightVector2, normalVector);

    reflVector = glm::normalize(reflVector);
    reflVector2 = glm::normalize(reflVector2);

    float rDotv = glm::dot(reflVector, viewVector);
    float rDotv2 = glm::dot(reflVector2, viewVector);

    float specularTerm;
    float specularTerm2;
    float phong = 30.0;
    float reflect_rate = 0.7;

    // convert unit lightVector to a distance from intersection point to the light source
    if(rDotv < 0) {
        specularTerm = 0;
    }else{
        specularTerm = pow(rDotv, phong);
    }

    if(rDotv2 < 0) {
        specularTerm2 = 0;
    }else{
        specularTerm2 = pow(rDotv2, phong);
    }

    if (ray.xindex == 2)     //texture football
    {
        float U = asin(normalVector.x) / M_PI + 0.5;
        float V = asin(normalVector.y) / M_PI + 0.5;
        materialCol = texture2.getColorAt(U, V);
    }

    if(ray.xindex == 1 || ray.xindex == 3 || ray.xindex == 7 || ray.xindex == 8) //textture-procedural pattern
    {
        if ((int(ray.xpt.x - ray.xpt.y) % 2 == 0)){
            materialCol = glm::vec3(0,1,0);
        }
        else if((int(ray.xpt.x + ray.xpt.y) % 2 == 0)){
            materialCol = glm::vec3(0,1,0);
        }
        else{
            materialCol = glm::vec3(0,0,0);
        }
    }

    if(ray.xindex == 4){     //make floor
        int modx = (int)((ray.xpt.x + 200) / 4) % 2;
        int modz = (int)((ray.xpt.z + 200) / 4) % 2;

       if((modx && modz) || (!modx && !modz)){
           materialCol = glm::vec3(1,0.5,0);}
       else{
           materialCol = glm::vec3(1,1,1);}
    }

    if(ray.xindex == 5)  //texture background
    {
        float s = (ray.xpt.x+70)/140;
        float t = (ray.xpt.y+20)/70;
        materialCol = texture1.getColorAt(s,t);
    }

    Ray shadow(ray.xpt, lightVector1);  // shadows1
    shadow.closestPt(sceneObjects);
    Ray shadow1(ray.xpt, lightVector2);  // shadows2
    shadow1.closestPt(sceneObjects);

    bool in_shadow_1 = (lDotn <= 0 || (shadow.xindex > -1 && shadow.xdist < lightDist1));
    bool in_shadow_2 = (lDotn1 <= 0 || (shadow1.xindex > -1 && shadow1.xdist < lightDist2));

    if(in_shadow_1)

    {
        if (shadow.xindex == 6){
            colorSum = (lDotn * materialCol) * 0.1f;

        }
        colorSum = (ambientCol*materialCol) ;
    }
    else{
        colorSum = ambientCol*materialCol + (lDotn * materialCol + specularTerm)*0.5f;
    }

    if(in_shadow_2)
    {
        if (shadow.xindex == 6){
            colorSum = (lDotn * materialCol) * 0.1f;

        }
        colorSum += (ambientCol*materialCol);

    } else {
        colorSum  += ambientCol*materialCol + (lDotn1*materialCol + specularTerm2)*0.5f ;
    }

    if (in_shadow_1 && in_shadow_2){
        colorSum += (ambientCol*materialCol)* 1.5f;
    }



    if(sceneObjects[ray.xindex]->isReflect && step < MAX_STEPS)     //reflective
     {
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
        Ray reflectedRay(ray.xpt, reflectedDir);
        glm::vec3 reflectedCol = trace(reflectedRay, step+1);
        colorSum = colorSum + (reflect_rate * reflectedCol);
     }


    if(ray.xindex == 6 && step < MAX_STEPS){             //refraction - ball
        float transp_rate = 0.3;
        float eta = 1/1.05;

        glm::vec3 normalVector1 = sceneObjects[ray.xindex]->normal(ray.xpt);
        glm::vec3 refraction1 = glm::refract(ray.dir, normalVector1, eta);
        Ray tranRay1(ray.xpt, refraction1);
        tranRay1.closestPt(sceneObjects);
        if(tranRay1.xindex == -1) return backgroundCol;

        glm::vec3 normalVector2 = sceneObjects[tranRay1.xindex]->normal(tranRay1.xpt);
        glm::vec3 refraction2 = glm::refract(refraction1, -normalVector2, 1.0f/eta);
        Ray Rayout(tranRay1.xpt, refraction2);
        Rayout.closestPt(sceneObjects);
        if(Rayout.xindex == -1) return backgroundCol;

        glm::vec3 tranCol = trace(Rayout, step+1);
        colorSum = colorSum * transp_rate + tranCol*(1 - transp_rate);
        return colorSum;
    }


    if(ray.xindex >= 9 && step < MAX_STEPS){          //transparent cube
        float transp_rate = 0.3;
        float eta = 1/1.005;

        glm::vec3 normalVector1 = sceneObjects[ray.xindex]->normal(ray.xpt);
        glm::vec3 refraction1 = glm::refract(ray.dir, normalVector1, eta);
        Ray tranRay1(ray.xpt, refraction1);
        tranRay1.closestPt(sceneObjects);
        if(tranRay1.xindex == -1) return backgroundCol;

        glm::vec3 normalVector2 = sceneObjects[tranRay1.xindex]->normal(tranRay1.xpt);
        glm::vec3 refraction2 = glm::refract(refraction1, -normalVector2, 1.0f/eta);
        Ray Rayout(tranRay1.xpt, refraction2);
        Rayout.closestPt(sceneObjects);
        if(Rayout.xindex == -1) return backgroundCol;

        glm::vec3 tranCol = trace(Rayout, step+1);
        colorSum = colorSum * transp_rate + tranCol*(1 - transp_rate);

        return colorSum;
    }
    return colorSum;
}


glm::vec3 anti_aliasing(glm::vec3 eye, float cellX, float xp, float yp){
    float prameter1 = cellX * 0.2;
    float prameter2 = cellX * 0.8;

    glm::vec3 colorSum(0);
    glm::vec3 avg(0.25);

    glm::vec3 dir1(xp+prameter1, yp+prameter1, -EDIST);
    glm::vec3 dir2(xp+prameter2, yp+prameter2, -EDIST);
    glm::vec3 dir3(xp+prameter2, yp+prameter1, -EDIST);
    glm::vec3 dir4(xp+prameter1, yp+prameter2, -EDIST);

    Ray ray1 = Ray(eye, dir1);
    Ray ray2 = Ray(eye, dir2);
    Ray ray3 = Ray(eye, dir3);
    Ray ray4 = Ray(eye, dir4);

    ray1.normalize();
    ray2.normalize();
    ray3.normalize();
    ray4.normalize();

    colorSum+=trace(ray1,1);
    colorSum+=trace(ray2,1);
    colorSum+=trace(ray3,1);
    colorSum+=trace(ray4,1);

    colorSum*= avg;
    return colorSum;
}

void display()
{
    float xp, yp;  //grid point
    float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
    float cellY = (YMAX-YMIN)/NUMDIV;  //cell height

    glm::vec3 eye(0., 0., 0.);  //The eye position (source of primary rays) is the origin

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_QUADS);  //Each cell is a quad.

    for(int i = 0; i < NUMDIV; i++)     //For each grid point xp, yp
    {
        xp = XMIN + i*cellX;
        for(int j = 0; j < NUMDIV; j++)
        {
            yp = YMIN + j*cellY;

            glm::vec3 dir(xp+0.5*cellX, yp+0.5*cellY, -EDIST);  //direction of the primary ray

            Ray ray = Ray(eye, dir);        //Create a ray originating from the camera in the direction 'dir'
            ray.normalize();                //Normalize the direction of the ray to a unit vector
            glm::vec3 col = anti_aliasing(eye,cellX,xp,yp)* glm::vec3(0.8);   //anti_aliasing
            //glm::vec3 col = trace (ray, 1); //Trace the primary ray and get the colour value

            glColor3f(col.r, col.g, col.b);
            glVertex2f(xp, yp);             //Draw each cell with its color value
            glVertex2f(xp+cellX, yp);
            glVertex2f(xp+cellX, yp+cellY);
            glVertex2f(xp, yp+cellY);
        }
    }
    glEnd();
    glFlush();
}

void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glClearColor(0, 0, 0, 1);

    texture1 = TextureBMP((char*)"wall.bmp");
    texture2 = TextureBMP((char*)"football.bmp");

    //-- Create a pointer to a sphere object
    Sphere *sphere1 = new Sphere(glm::vec3(-8,-3, -130.0), 13.0, glm::vec3(0, 0, 1));
    Cone *cone1 = new Cone(glm::vec3(-15,-9,-90), 3, 5.5, glm::vec3(0, 0.5, 0.5));
    Cone *cone2 = new Cone(glm::vec3(-15,-12,-90), 3.5, 5, glm::vec3(0, 0.5, 0.5));
    Cone *cone3 = new Cone(glm::vec3(-15,-15,-90), 3.5, 5, glm::vec3(0, 0.5, 0.5));
    Cylinder *cylinder1 = new Cylinder(glm::vec3(-15,-18,-90), 1.3, 10, glm::vec3(0, 0.5, 0.5));
    Sphere *football = new Sphere(glm::vec3(5, 7, -115.0), 3.0, glm::vec3(1,0,0));
    Sphere *sphere3 = new Sphere(glm::vec3(2, -12, -80.0), 3.0, glm::vec3(1,0,0));


    //--Add the above to the list of scene objects.
    sphere1->isReflect = true;  // set as reflective rather than use x.index == 0,1,2...

    sceneObjects.push_back(sphere1);    //0
    sceneObjects.push_back(cylinder1);   //1
    sceneObjects.push_back(football);   //2
    sceneObjects.push_back(cone1);   //3
    floor();                         //4
    background();                     //5
    sceneObjects.push_back(sphere3); //6
    sceneObjects.push_back(cone2);   //7
    sceneObjects.push_back(cone3);   //8
    Cube(10, -20, -90, 8,8,8,glm::vec3(0,1,0));   // 9 10 11 12 13 14

}


int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(520, 20);
    glutCreateWindow("Raytracer");
    initialize();
    glutDisplayFunc(display);glutMainLoop();
    return 0;
}
