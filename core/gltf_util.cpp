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
    const tinygltf::BufferView &posView = model.bufferViews[pos_accessor.bufferView];
    const tinygltf::Buffer &pos_buffer = model.buffers[posView.buffer];

    const float *positions = reinterpret_cast<const float*>(
        &pos_buffer.data[posView.byteOffset + pos_accessor.byteOffset]
    );

    int vertex_count = pos_accessor.count;

    //copy to Model struct
    model_m.vertex_count = vertex_count;
    model_m.positions.resize(vertex_count * 3);
    std::memcpy(model_m.positions.data(), positions, vertex_count * 3 * sizeof(float));


    //get indices
    const tinygltf::Accessor &ind_accessor = model.accessors[primitive.indices];
    const tinygltf::BufferView &ind_view = model.bufferViews[ind_accessor.bufferView];
    const tinygltf::Buffer &ind_buffer = model.buffers[ind_view.buffer];
    
    const unsigned short *indices = reinterpret_cast<const unsigned short*>(
        &ind_buffer.data[ind_view.byteOffset + ind_accessor.byteOffset]
    );

    int index_count = ind_accessor.count;
    model_m.index_count = index_count;

    model_m.indices.resize(index_count);
    std::memcpy(model_m.indices.data(), indices, index_count * sizeof(unsigned short));

    /* for(int i = 0; i < index_count; i++) {
            model_m.indices[i] = indices[i];
    } */

    //get vertex normals
    if(primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
        const tinygltf::Accessor &norm_accessor = model.accessors[primitive.attributes.at("NORMAL")];
        const tinygltf::BufferView &norm_view = model.bufferViews[norm_accessor.bufferView];
        const tinygltf::Buffer &norm_buffer = model.buffers[norm_view.buffer];

        const float *normals = reinterpret_cast<const float*>(
            &norm_buffer.data[norm_view.byteOffset + norm_accessor.byteOffset]
        );

        model_m.normals.resize(vertex_count * 3);
        memcpy(model_m.normals.data(), normals, vertex_count * 3 * sizeof(float));
    }
    else{
        printf("%s does not contain Normals data\n", filename.c_str());
    }

    return model_m;
}