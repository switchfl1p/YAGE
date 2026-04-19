/* switchfl1p 2025-2026 */

#include <Node.hpp>
#include <glm/ext/matrix_transform.hpp>

//T*R*S
glm::mat4 Transform::calc_model_mat(){
    //translation
    model_mat = glm::translate(glm::mat4(1), translation_component);

    //rotation
    //order: YXZ
    model_mat = glm::rotate(model_mat, rotation_component.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model_mat = glm::rotate(model_mat, rotation_component.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model_mat = glm::rotate(model_mat, rotation_component.z, glm::vec3(0.0f, 0.0f, 1.0f));

    //scale
    model_mat = glm::scale(model_mat, scale_component);

    return model_mat;
}

void MaterialBlock::getMaterialInformation(const Node& node){
    classic_color = node.material.classic.color;
    pbr_color = node.material.pbr.color;
    specular_color = node.material.classic.specular_color;
    shininess_factor = node.material.classic.shininess;
    metallic = node.material.pbr.metallic;
    roughness = node.material.pbr.roughness;
    is_emissive = node.material.is_emissive ? 1.0f : 0.0f;
}