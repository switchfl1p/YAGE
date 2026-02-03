/* switchfl1p 2025-2026 */

#include <gltf_util.hpp>

gltf_util::Model gltf_util::Loader::loadModel(std::string filename){
    tinygltf::Model model;
    Model model_m;

    std::string err;
    std::string warn;

    std::string filepath = "meshes/" + filename;

    bool response = loader.LoadBinaryFromFile(&model, &err, &warn, filepath);

    if(!warn.empty()) {
        printf("Warn: %s\n", warn.c_str());
    }

    if(!err.empty()) {
        printf("Err: %s\n", err.c_str());
    }

    if(!response) {
        printf("Failed to parse glTF: %s\n", filename.c_str());
        return model_m;
    }

    const tinygltf::Mesh &mesh = model.meshes[0];
    const tinygltf::Primitive &primitive = mesh.primitives[0];

    //Get vertex positions
    const tinygltf::Accessor &pos_accessor = model.accessors[primitive.attributes.at("POSITION")];
    const tinygltf::BufferView &pos_view = model.bufferViews[pos_accessor.bufferView];
    const float *positions = reinterpret_cast<const float*>(
        &model.buffers[pos_view.buffer].data[pos_view.byteOffset + pos_accessor.byteOffset]
    );

    int vertex_count = pos_accessor.count;
    model_m.vertex_count = vertex_count;
    model_m.vertex_data.resize(vertex_count);

    //Fill positions (always present)
    for(int i = 0; i < vertex_count; i++) {
        model_m.vertex_data[i].position = glm::vec3(
            positions[i*3], positions[i*3+1], positions[i*3+2]
        );
    }

    //Get normals
    if(primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
        const tinygltf::Accessor &norm_accessor = model.accessors[primitive.attributes.at("NORMAL")];
        const tinygltf::BufferView &norm_view = model.bufferViews[norm_accessor.bufferView];
        const float *normals = reinterpret_cast<const float*>(
            &model.buffers[norm_view.buffer].data[norm_view.byteOffset + norm_accessor.byteOffset]
        );
        for(int i = 0; i < vertex_count; i++) {
            model_m.vertex_data[i].normal = glm::vec3(
                normals[i*3], normals[i*3+1], normals[i*3+2]
            );
        }
    }
    else {
        printf("%s does not contain Normal data\n", filename.c_str());
    }

    // Get colors
    if(primitive.attributes.find("COLOR_0") != primitive.attributes.end()) {
        const tinygltf::Accessor &color_accessor = model.accessors[primitive.attributes.at("COLOR_0")];
        const tinygltf::BufferView &color_view = model.bufferViews[color_accessor.bufferView];
        const float *colors = reinterpret_cast<const float*>(
            &model.buffers[color_view.buffer].data[color_view.byteOffset + color_accessor.byteOffset]
        );
        int components = (color_accessor.type == TINYGLTF_TYPE_VEC4) ? 4 : 3;
        for(int i = 0; i < vertex_count; i++) {
            model_m.vertex_data[i].color = glm::vec4(
                colors[i*components],
                colors[i*components+1],
                colors[i*components+2],
                (components == 4) ? colors[i*components+3] : 1.0f
            );
        }
    }
    else {
        printf("%s does not contain Color data\n", filename.c_str());
    }

    // Get UVs
    if(primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
        const tinygltf::Accessor &uv_accessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
        const tinygltf::BufferView &uv_view = model.bufferViews[uv_accessor.bufferView];
        const float *uvs = reinterpret_cast<const float*>(
            &model.buffers[uv_view.buffer].data[uv_view.byteOffset + uv_accessor.byteOffset]
        );
        for(int i = 0; i < vertex_count; i++) {
            model_m.vertex_data[i].uv = glm::vec2(
                uvs[i*2], uvs[i*2+1]
            );
        }
    }
    else {
        printf("%s does not contain UV data\n", filename.c_str());
    }

    // Get indices
    const tinygltf::Accessor &ind_accessor = model.accessors[primitive.indices];
    const tinygltf::BufferView &ind_view = model.bufferViews[ind_accessor.bufferView];
    const unsigned short *indices = reinterpret_cast<const unsigned short*>(
        &model.buffers[ind_view.buffer].data[ind_view.byteOffset + ind_accessor.byteOffset]
    );
    model_m.index_count = ind_accessor.count;
    model_m.indices.resize(ind_accessor.count);
    std::memcpy(model_m.indices.data(), indices, ind_accessor.count * sizeof(unsigned short));

    return model_m;
}