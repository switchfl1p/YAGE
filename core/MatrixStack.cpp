/* switchfl1p 2025 */

#include <MatrixStack.hpp>
#include <glm/gtc/matrix_transform.hpp>

MatrixStack::MatrixStack()
    :curr_matrix(1)
    {}

MatrixStack::~MatrixStack(){}

void MatrixStack::perspective_t(float deg_FOV, float aspect_ratio, float z_near, float z_far){
    curr_matrix *= glm::perspective(deg_FOV, aspect_ratio, z_near, z_far);
}

void MatrixStack::push_m(){
    matrix_stack.push(curr_matrix);
}

const glm::mat4& MatrixStack::top_m() const{
    return curr_matrix;
}