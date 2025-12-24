#pragma once
#include <glm/glm.hpp>
#include <stack>

class MatrixStack{
    public:

    private:
        //there are performance gains from making the stack use a vector instead of it's default, which is a deque
        std::stack<glm::mat4,  std::vector<glm::mat4>> matrix_stack;
        glm::mat4 curr_matrix;

};