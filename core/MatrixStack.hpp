/* switchfl1p 2025 */

#pragma once
#include <glm/glm.hpp>
#include <stack>
#include <vector>

class MatrixStack{
    public:
        MatrixStack();
        ~MatrixStack();
        explicit MatrixStack(const glm::mat4 &initial_matrix);
        void push();
        void pop();
        const glm::mat4& top() const;
        
        void perspective(float deg_FOV, float aspect_ratio, float z_near, float z_far);
        

    private:
        //there are performance gains from making the stack use a vector instead of it's default, which is a deque
        std::stack<glm::mat4,  std::vector<glm::mat4>> matrix_stack;
        glm::mat4 curr_matrix;
};