#include "pch.h"
#define TINYGLTF_IMPLEMENTATION
// #define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "model.h"

void Model::Init(const std::string& filename, const VK::Device& device, const VK::MemoryAllocator& memoryAllocator, const VK::CommandPool& commandPool, const VK::Queue& queue)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    m_filepath = std::filesystem::path { filename };

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

        ProcessNode(model, currentNode, device, memoryAllocator, commandPool, queue);
    }
}

void Model::Render(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
{
    for (auto& mesh : m_meshes) {
        mesh.Draw(commandBuffer, pipelineLayout);
    }
}

void Model::ProcessNode(tinygltf::Model& model, tinygltf::Node& currentNode, const VK::Device& device, const VK::MemoryAllocator& memoryAllocator, const VK::CommandPool& commandPool, const VK::Queue& queue)
{
    for (size_t i = 0; i < currentNode.children.size(); i++) {
        int childNodeIndex = currentNode.children[i];
        ProcessNode(model, model.nodes[childNodeIndex], device, memoryAllocator, commandPool, queue);
    }

    int meshIndex = currentNode.mesh;

    if (meshIndex > -1) {
        ProcessMesh(model, model.meshes[meshIndex], device, memoryAllocator, commandPool, queue);
    }
}

void Model::ProcessMesh(tinygltf::Model& model, tinygltf::Mesh& currentMesh, const VK::Device& device, const VK::MemoryAllocator& memoryAllocator, const VK::CommandPool& commandPool, const VK::Queue& queue)
{
    for (size_t i = 0; i < currentMesh.primitives.size(); i++) {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        tinygltf::Primitive& currentPrimitive = currentMesh.primitives[i];

        int posIndex = currentPrimitive.attributes.find("POSITION")->second;
        int uvIndex = currentPrimitive.attributes.find("TEXCOORD_0")->second;
        int indicesIndex = currentPrimitive.indices;

        tinygltf::Accessor& posAccessor = model.accessors[posIndex];
        tinygltf::Accessor& uvAccessor = model.accessors[uvIndex];
        tinygltf::Accessor& indexAccessor = model.accessors[indicesIndex];

        tinygltf::BufferView& posBufferView = model.bufferViews[posAccessor.bufferView];
        tinygltf::Buffer& posBuffer = model.buffers[posBufferView.buffer];

        tinygltf::BufferView& uvBufferView = model.bufferViews[uvAccessor.bufferView];
        tinygltf::Buffer& uvBuffer = model.buffers[uvBufferView.buffer];

        const float* pPos = reinterpret_cast<const float*>(posBuffer.data.data() + posAccessor.byteOffset + posBufferView.byteOffset);
        size_t numPos = posAccessor.count;
        size_t posStride = posAccessor.ByteStride(posBufferView) / sizeof(float);

        const float* pUv = reinterpret_cast<const float*>(uvBuffer.data.data() + uvAccessor.byteOffset + uvBufferView.byteOffset);
        size_t uvStride = uvAccessor.ByteStride(uvBufferView) / sizeof(float);

        for (size_t i = 0; i < numPos; ++i) {
            Vertex vertex {
                .pos { pPos[i * posStride], pPos[i * posStride + 1], pPos[i * posStride + 2] },
                .color {},
                .uv { pUv[i * uvStride], pUv[i * uvStride + 1] }
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

        Mesh& myMesh = m_meshes.emplace_back();
        myMesh.Init(memoryAllocator, commandPool, queue, vertices, indices);

        int materialIndex = currentPrimitive.material;
        if (materialIndex > -1) {
            tinygltf::Material& currentMaterial = model.materials[materialIndex];
            int baseColorTextureIndex = currentMaterial.pbrMetallicRoughness.baseColorTexture.index;

            std::string filename = model.images[baseColorTextureIndex].uri;

            std::filesystem::path finalPath = m_filepath.parent_path() / std::filesystem::path { filename };

            myMesh.texture = memoryAllocator.CreateTexture2D(finalPath.string(), commandPool, queue);
            myMesh.textureView = myMesh.texture.CreateView(device.GetHandle(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
        }
    }
}
