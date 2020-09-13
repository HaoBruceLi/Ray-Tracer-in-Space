
#include "Cylinder.h"
#include <math.h>


float Cylinder::intersect(glm::vec3 posn, glm::vec3 dir)
{
    float x = posn.x - center.x;
    float z = posn.z - center.z;

    float a = dir.x * dir.x + dir.z * dir.z;
    float b = 2 * dir.x * x + 2 * dir.z* z;
    float c = x * x + z * z - radius*radius;
    float delta = b*b - 4*a*c;

    if(fabs(delta) < 0.001) return -1.0;
    if(delta < 0.0) return -1.0;

    float t1 = (-b - sqrt(delta)) / (2 * a);
    float t2 = (-b + sqrt(delta)) / (2 * a);

    float r1 = posn.y + t1*dir.y;
    float r2 = posn.y + t2*dir.y;
    if (r1<center.y){
        if (r2 < center.y) return -1;
        else
        {
            float t = (center.y - posn.y) / dir.y;
            if (t <= 0) return -1;
            return t;
        }
    }
    else if (r1 > center.y + height){
        if (r2 > center.y + height)  return -1;
        else{
            float t = (center.y + height - posn.y) / dir.y;
            if (t<=0) return -1;
            else return t;
        }
   }
   else if (r1 > center.y && r1 < center.y + height){
       if (t1<=0) return -1;
       else return t1;
   }
   return -1;

}

glm::vec3 Cylinder::normal(glm::vec3 p)
{
    glm::vec3 n = glm::vec3 (p.x - center.x, 0, p.z - center.z);
    n = glm::normalize(n);
    return n;
}
