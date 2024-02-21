#include "pch.h"
#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "model.h"
#include "image.h"

Model::Model(const VK::MemoryAllocator* pMemoryAllocator, const VK::CommandPool* pCommandPool, const VK::Queue* pQueue)
    : p_memoryAllocator { pMemoryAllocator }
    , p_commandPool { pCommandPool }
    , p_queue { pQueue }
{
}

void Model::LoadFile(const std::string& filename)
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

    activeSceneIndex = model.defaultScene;

    for (size_t i = 0; i < model.scenes.size(); i++) {
        LoadScene(model, i);
    }

    for (size_t i = 0; i < model.nodes.size(); i++) {
        LoadNode(model, i);
    }

    for (size_t i = 0; i < model.meshes.size(); i++) {
        LoadMesh(model, i);
    }

    for (size_t i = 0; i < model.materials.size(); i++) {
        LoadMaterial(model, i);
    }
}

void Model::Render(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
{
    for (int nodeIndex : scenes[activeSceneIndex].nodes) {
        glTF::Node& currentNode = nodes[nodeIndex];

        if (!currentNode.meshIndex.has_value()) {
            continue;
        }

        glTF::Mesh& currentMesh = meshes[currentNode.meshIndex.value()];

        for (glTF::Primitive& primitve : currentMesh.primitives) {
            VkBuffer vertexBuffers[] = { primitve.vertexBuffer.GetHandle() };
            VkDeviceSize offsets[] = { 0 };

            vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(commandBuffer, primitve.indexBuffer.GetHandle(), 0, VK_INDEX_TYPE_UINT32);

            // m_uniformData.world = transform.GetWorldMatrix();
            // m_uniformBuffer.CopyData(&m_uniformData, sizeof(MeshUniformData));

            // VkDescriptorSet descriptorSets[] = { m_descriptorSet.GetHandle() };

            // vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, descriptorSets, 0, nullptr);

            vkCmdDrawIndexed(commandBuffer, primitve.indexCount, 1, 0, 0, 0);
        }
    }
}
/*
void Model::ProcessNode(tinygltf::Model& model, int nodeIndex, const VK::MemoryAllocator& memoryAllocator, const VK::CommandPool& commandPool, const VK::Queue& queue)
{
    tinygltf::Node& currentNode = model.nodes[nodeIndex];

    for (int childNodeIndex : currentNode.children) {
        ProcessNode(model, childNodeIndex, memoryAllocator, commandPool, queue);
    }

    ProcessMesh(model, currentNode.mesh, memoryAllocator, commandPool, queue);
}

void Model::ProcessMesh(tinygltf::Model& model, int meshIndex, const VK::MemoryAllocator& memoryAllocator, const VK::CommandPool& commandPool, const VK::Queue& queue)
{
    if (meshIndex < 0) {
        return;
    }

    tinygltf::Mesh& currentMesh = model.meshes[meshIndex];

    for (tinygltf::Primitive& currentPrimitive : currentMesh.primitives) {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

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

        int materialIndex = currentPrimitive.material;
        if (materialIndex > -1) {
            tinygltf::Material& currentMaterial = model.materials[materialIndex];
            int baseColorTextureIndex = currentMaterial.pbrMetallicRoughness.baseColorTexture.index;

            std::string filename = model.images[baseColorTextureIndex].uri;

            std::filesystem::path finalPath = m_filepath.parent_path() / std::filesystem::path { filename };

            myMesh.Init(memoryAllocator, commandPool.AllocateCommandBuffer(), queue, vertices, indices, finalPath.string());
            //  myMesh.transform.SetRotation({ 0.0f, 2.0f, 0.0f, 0.0f });
        }
    }
}
*/
void Model::LoadScene(tinygltf::Model& tinyModel, size_t sceneIndex)
{
    scenes.emplace_back().nodes = tinyModel.scenes[sceneIndex].nodes;
}

void Model::LoadNode(tinygltf::Model& tinyModel, size_t nodeIndex)
{
    tinygltf::Node& tinyNode = tinyModel.nodes[nodeIndex];
    glTF::Node& currentNode = nodes.emplace_back();

    if (tinyNode.camera > -1) {
        currentNode.cameraIndex = tinyNode.camera;
    }

    if (tinyNode.skin > -1) {
        currentNode.skinIndex = tinyNode.skin;
    }

    if (tinyNode.matrix.size() == 16) {
        currentNode.matrix = glm::make_mat4x4(tinyNode.matrix.data());
    }

    if (tinyNode.mesh > -1) {
        currentNode.meshIndex = tinyNode.mesh;
    }

    if (tinyNode.rotation.size() == 4) {
        currentNode.rotation = glm::make_quat(tinyNode.rotation.data());
    }

    if (tinyNode.scale.size() == 3) {
        currentNode.scale = glm::make_vec3(tinyNode.scale.data());
    }

    if (tinyNode.translation.size() == 3) {
        currentNode.translation = glm::make_vec3(tinyNode.translation.data());
    }
}

void Model::LoadMesh(tinygltf::Model& tinyModel, size_t meshIndex)
{
    tinygltf::Mesh& tinyMesh = tinyModel.meshes[meshIndex];
    glTF::Mesh& currentMesh = meshes.emplace_back();

    for (auto& primitive : tinyMesh.primitives) {
        LoadPrimitive(tinyModel, meshIndex, primitive);
    }
}

void Model::LoadPrimitive(tinygltf::Model& tinyModel, size_t meshIndex, tinygltf::Primitive& tinyPrimitive)
{
    glTF::Primitive& currentPrimitive = meshes[meshIndex].primitives.emplace_back();
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

#pragma region POSITION
    auto iter = tinyPrimitive.attributes.find("POSITION");

    if (iter != tinyPrimitive.attributes.end()) {
        int posIndex = iter->second;
        tinygltf::Accessor& posAccessor = tinyModel.accessors[posIndex];
        tinygltf::BufferView& posBufferView = tinyModel.bufferViews[posAccessor.bufferView];
        tinygltf::Buffer& posBuffer = tinyModel.buffers[posBufferView.buffer];

        const float* pPos = reinterpret_cast<const float*>(posBuffer.data.data() + posAccessor.byteOffset + posBufferView.byteOffset);
        size_t numPos = posAccessor.count;

        size_t posStride = posAccessor.ByteStride(posBufferView) != -1
            ? posAccessor.ByteStride(posBufferView) / tinygltf::GetComponentSizeInBytes(posAccessor.type)
            : tinygltf::GetNumComponentsInType(posAccessor.type);

        vertices.resize(numPos);
        for (size_t i = 0; i < vertices.size(); ++i) {
            vertices[i].pos = glm::vec3(pPos[i * posStride], pPos[i * posStride + 1], pPos[i * posStride + 2]);
        }
    }
#pragma endregion

#pragma region TEXCOORD_0
    iter = tinyPrimitive.attributes.find("TEXCOORD_0");

    if (iter != tinyPrimitive.attributes.end()) {
        int texcoordIndex = iter->second;
        tinygltf::Accessor& texcoordAccessor = tinyModel.accessors[texcoordIndex];
        tinygltf::BufferView& texcoordBufferView = tinyModel.bufferViews[texcoordAccessor.bufferView];
        tinygltf::Buffer& texcoordBuffer = tinyModel.buffers[texcoordBufferView.buffer];

        const float* pTexcoord = reinterpret_cast<const float*>(texcoordBuffer.data.data() + texcoordAccessor.byteOffset + texcoordBufferView.byteOffset);
        size_t texcoordStride = texcoordAccessor.ByteStride(texcoordBufferView) != -1
            ? texcoordAccessor.ByteStride(texcoordBufferView) / tinygltf::GetComponentSizeInBytes(texcoordAccessor.type)
            : tinygltf::GetNumComponentsInType(texcoordAccessor.type);

        for (size_t i = 0; i < vertices.size(); ++i) {
            vertices[i].uv = glm::vec2(pTexcoord[i * texcoordStride], pTexcoord[i * texcoordStride + 1]);
        }
    }
#pragma endregion

#pragma region indices
    int indicesIndex = tinyPrimitive.indices;

    if (indicesIndex > -1) {
        tinygltf::Accessor& indexAccessor = tinyModel.accessors[indicesIndex];
        tinygltf::BufferView& indexBufferView = tinyModel.bufferViews[indexAccessor.bufferView];
        tinygltf::Buffer& indexBuffer = tinyModel.buffers[indexBufferView.buffer];

        void* pIndices = indexBuffer.data.data() + indexAccessor.byteOffset + indexBufferView.byteOffset;
        size_t indexCount = indexAccessor.count;

        indices.reserve(indexCount);

        if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
            const uint16_t* pIndices16 = reinterpret_cast<const uint16_t*>(pIndices);

            for (size_t i = 0; i < indexCount; ++i) {
                indices.push_back(static_cast<uint32_t>(pIndices16[i]));
            }
        } else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
            const uint32_t* pIndices32 = reinterpret_cast<const uint32_t*>(pIndices);

            for (size_t i = 0; i < indexCount; ++i) {
                indices.push_back(pIndices32[i]);
            }
        }
    }

#pragma endregion

    currentPrimitive.vertexBuffer = CreateVertexBuffer(vertices);
    currentPrimitive.indexBuffer = CreateIndexBuffer(indices);
    currentPrimitive.indexCount = static_cast<uint32_t>(indices.size());
}

void Model::LoadMaterial(tinygltf::Model& tinyModel, size_t materialIndex)
{
    tinygltf::Material& tinyMaterial = tinyModel.materials[materialIndex];

    glTF::Material& currentMaterial = materials.emplace_back();

    currentMaterial.baseColorFactor = glm::make_vec4(tinyMaterial.pbrMetallicRoughness.baseColorFactor.data());
    currentMaterial.metallicRoughnessFactor[0] = tinyMaterial.pbrMetallicRoughness.metallicFactor;
    currentMaterial.metallicRoughnessFactor[1] = tinyMaterial.pbrMetallicRoughness.roughnessFactor;

    if (tinyMaterial.pbrMetallicRoughness.baseColorTexture.index > -1) {
        int imageIndex = tinyMaterial.pbrMetallicRoughness.baseColorTexture.index;
        tinygltf::Image& tinyImage = tinyModel.images[imageIndex];

        std::filesystem::path texturePath = m_filepath.parent_path() / std::filesystem::path { tinyImage.uri };
        currentMaterial.baseColorTexture = CreateTexture(texturePath.string());
    }

    if (tinyMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index > -1) {
        int imageIndex = tinyMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index;
        tinygltf::Image& tinyImage = tinyModel.images[imageIndex];

        std::filesystem::path texturePath = m_filepath.parent_path() / std::filesystem::path { tinyImage.uri };
        currentMaterial.metallicRoughnessTexture = CreateTexture(texturePath.string());
    }
}

VK::Buffer Model::CreateVertexBuffer(const std::vector<Vertex>& vertices)
{
    VK::CommandBuffer commandBuffer = p_commandPool->AllocateCommandBuffer();
    commandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkDeviceSize vertexBufferSize = sizeof(Vertex) * vertices.size();

    VK::Buffer vertexStagingBuffer = p_memoryAllocator->CreateBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
    vertexStagingBuffer.CopyData((void*)vertices.data(), vertexBufferSize);

    VK::Buffer vertexBuffer = p_memoryAllocator->CreateBuffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

    vertexStagingBuffer.CopyToBuffer(commandBuffer, vertexBuffer.GetHandle(), vertexBufferSize);

    commandBuffer.End();
    p_queue->Submit({ commandBuffer.GetHandle() });
    p_queue->WaitIdle();
    commandBuffer.Free();

    return vertexBuffer;
}

VK::Buffer Model::CreateIndexBuffer(const std::vector<uint32_t>& indices)
{
    VK::CommandBuffer commandBuffer = p_commandPool->AllocateCommandBuffer();
    commandBuffer.Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkDeviceSize indexBufferSize = sizeof(uint32_t) * indices.size();

    VK::Buffer indexStagingBuffer = p_memoryAllocator->CreateBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
    indexStagingBuffer.CopyData((void*)indices.data(), indexBufferSize);

    VK::Buffer indexBuffer = p_memoryAllocator->CreateBuffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

    indexStagingBuffer.CopyToBuffer(commandBuffer, indexBuffer.GetHandle(), indexBufferSize);

    commandBuffer.End();
    p_queue->Submit({ commandBuffer.GetHandle() });
    p_queue->WaitIdle();
    commandBuffer.Free();

    return indexBuffer;
}

VK::Image Model::CreateTexture(const std::string& path)
{
    VK::CommandBuffer commandBuffer = p_commandPool->AllocateCommandBuffer();

    ImageFile image;
    image.Init(path);

    VkBufferUsageFlags stagingBufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VmaAllocationCreateFlags stagingBufferAllocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    VK::Buffer stagingBuffer = p_memoryAllocator->CreateBuffer(image.GetSize(), stagingBufferUsage, stagingBufferAllocationFlags);
    stagingBuffer.CopyData(image.GetData(), image.GetSize());

    VkExtent3D extent3D = { static_cast<uint32_t>(image.GetWidth()), static_cast<uint32_t>(image.GetHeight()), 1 };

    VK::Image texture = p_memoryAllocator->CreateImage(extent3D, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    texture.TransitionLayout(commandBuffer,
        VK_ACCESS_NONE, VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

    stagingBuffer.CopyToImage(commandBuffer, texture.GetHandle(), extent3D);

    texture.TransitionLayout(commandBuffer,
        VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

    commandBuffer.End();
    p_queue->Submit({ commandBuffer.GetHandle() });
    p_queue->WaitIdle();
    commandBuffer.Free();

    return texture;
}