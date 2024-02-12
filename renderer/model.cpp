#include "pch.h"
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "model.h"
#include "mesh.h"

Model::~Model()
{
    for (auto& m : m_meshes) {
        delete m;
    }
}

void Model::Render(const VkCommandBuffer& commandBuffer, const VkPipelineLayout& pipelineLayout)
{
    for (auto& m : m_meshes) {
        m->Draw(commandBuffer, pipelineLayout);
    }
}

void Model::ReadFromFile(const std::string& filename, const VK::Device* pDevice, const VK::MemoryAllocator* pAllocator, const VK::CommandPool* pCommandPool)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, filename.c_str());

    if (!warn.empty()) {
        printf("Warn: %s\n", warn.c_str());
        return;
    }

    if (!err.empty()) {
        printf("Err: %s\n", err.c_str());
        return;
    }

    if (!ret) {
        printf("Failed to parse glTF\n");
        return;
    }

    tinygltf::Scene& scene = model.scenes[model.defaultScene];

    for (size_t i = 0; i < scene.nodes.size(); i++) {
        int currentNodeIndex = scene.nodes[i];
        tinygltf::Node& currentNode = model.nodes[currentNodeIndex];

        ProcessNode(model, currentNode, pDevice, pAllocator, pCommandPool);
        /*for (size_t j = 0; j < currentNode.children.size(); j++) {
            int childNodeIndex = currentNode.children[j];
        }*/
    }
}

void Model::ProcessNode(tinygltf::Model& model, tinygltf::Node& currentNode, const VK::Device* pDevice, const VK::MemoryAllocator* pAllocator, const VK::CommandPool* pCommandPool)
{
    for (size_t i = 0; i < currentNode.children.size(); i++) {
        int childNodeIndex = currentNode.children[i];
        ProcessNode(model, model.nodes[childNodeIndex], pDevice, pAllocator, pCommandPool);
    }

    int meshIndex = currentNode.mesh;

    if (meshIndex > -1) {
        ProcessMesh(model, model.meshes[meshIndex], pDevice, pAllocator, pCommandPool);
    }
}

void Model::ProcessMesh(tinygltf::Model& model, tinygltf::Mesh& currentMesh, const VK::Device* pDevice, const VK::MemoryAllocator* pAllocator, const VK::CommandPool* pCommandPool)
{
    for (size_t i = 0; i < currentMesh.primitives.size(); i++) {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        tinygltf::Primitive& currentPrimitive = currentMesh.primitives[i];

        int posIndex = currentPrimitive.attributes.find("POSITION")->second;
        int indicesIndex = currentPrimitive.indices;

        tinygltf::Accessor& posAccessor = model.accessors[posIndex];
        tinygltf::Accessor& indexAccessor = model.accessors[indicesIndex];

        tinygltf::BufferView& posBufferView = model.bufferViews[posAccessor.bufferView];
        tinygltf::Buffer& posBuffer = model.buffers[posBufferView.buffer];

        const float* pPos = reinterpret_cast<const float*>(posBuffer.data.data() + posAccessor.byteOffset + posBufferView.byteOffset);
        size_t numPos = posAccessor.count;

        size_t stride = posAccessor.ByteStride(posBufferView) / sizeof(float);

        for (size_t i = 0; i < numPos; ++i) {
            Vertex vertex {
                .pos { pPos[i * stride], pPos[i * stride + 1], pPos[i * stride + 2] },
                .color {},
                .uv {}
            };
            vertices.push_back(vertex);
        }

        tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
        tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];
        void* pIndices = indexBuffer.data.data() + indexAccessor.byteOffset + indexBufferView.byteOffset;

        if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
            const uint16_t* indexData = reinterpret_cast<const uint16_t*>(pIndices);
            size_t numIndices = indexAccessor.count;

            for (size_t i = 0; i < numIndices; ++i) {
                indices.push_back(static_cast<uint32_t>(indexData[i]));
            }
        } else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
            const uint32_t* indexData = reinterpret_cast<const uint32_t*>(pIndices);
            size_t numIndices = indexAccessor.count;

            for (size_t i = 0; i < numIndices; ++i) {
                indices.push_back(indexData[i]);
            }
        }

        Mesh* myMesh = new Mesh { pDevice, pAllocator, pCommandPool, vertices, indices };
        m_meshes.push_back(myMesh);
    }
}
