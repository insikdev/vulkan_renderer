#include "pch.h"
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "glTF_asset.h"

glTF::Asset::Asset(const VK::MemoryAllocator* pMemoryAllocator, const VK::CommandPool* pCommandPool, const VK::Queue* pQueue)
    : p_memoryAllocator { pMemoryAllocator }
    , p_commandPool { pCommandPool }
    , p_queue { pQueue }
{
}

void glTF::Asset::LoadAsset(const std::string& filename)
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

    activeSceneIndex = model.defaultScene;

    scenes.resize(model.scenes.size());
    for (size_t i = 0; i < model.scenes.size(); i++) {
        LoadScene(model, i);
    }

    nodes.resize(model.nodes.size());
    for (size_t i = 0; i < model.nodes.size(); i++) {
        LoadNode(model, i);
    }

    meshes.resize(model.meshes.size());
    for (size_t i = 0; i < model.meshes.size(); i++) {
        LoadMesh(model, i);
    }

    textures.resize(model.textures.size());
    for (size_t i = 0; i < model.textures.size(); i++) {
        LoadTexture(model, i);
    }

    materials.resize(model.materials.size());
    for (size_t i = 0; i < model.materials.size(); i++) {
        LoadMaterial(model, i);
    }

    tinygltf::Image img;
    img.image = std::vector<uint8_t>(512 * 512 * 4, 0xFF);
    img.width = 512;
    img.height = 512;
    defaultTexture = CreateTexture(img);
}

void glTF::Asset::Render(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
{
    for (int nodeIndex : scenes[activeSceneIndex].nodes) {
        glTF::Node& currentNode = nodes[nodeIndex];

        RenderNode(commandBuffer, pipelineLayout, nodeIndex);

        for (int childNodeIndex : currentNode.children) {
            RenderNode(commandBuffer, pipelineLayout, childNodeIndex);
        }
    }
}

void glTF::Asset::RenderNode(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, size_t nodeIndex)
{
    glTF::Node& currentNode = nodes[nodeIndex];

    if (!currentNode.meshIndex.has_value()) {
        return;
    }

    const glm::mat4& nodeMatrix = GetNodeMatrix(nodeIndex);

    currentNode.uniformBuffer.CopyData((void*)&nodeMatrix, sizeof(glm::mat4));

    glTF::Mesh& currentMesh = meshes[currentNode.meshIndex.value()];

    for (glTF::Primitive& primitve : currentMesh.primitives) {
        VkBuffer vertexBuffers[] = { primitve.vertexBuffer.GetHandle() };
        VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, primitve.indexBuffer.GetHandle(), 0, VK_INDEX_TYPE_UINT32);

        if (primitve.materialIndex.has_value()) {
            glTF::Material& currentMaterial = materials[primitve.materialIndex.value()];

            currentMaterial.uniformBuffer.CopyData(&currentMaterial.uniformData, sizeof(glTF::MaterialData));
            std::vector<VkDescriptorSet> descriptorSets = { currentMaterial.descriptorSet.GetHandle(), currentNode.descriptorSet.GetHandle() };
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(), 0, nullptr);
        } else {
            // TODO : use default material
        }

        vkCmdDrawIndexed(commandBuffer, primitve.indexCount, 1, 0, 0, 0);
    }
}

void glTF::Asset::LoadScene(tinygltf::Model& tinyModel, size_t sceneIndex)
{
    scenes[sceneIndex].nodes = tinyModel.scenes[sceneIndex].nodes;
}

void glTF::Asset::LoadNode(tinygltf::Model& tinyModel, size_t nodeIndex)
{
    tinygltf::Node& tinyNode = tinyModel.nodes[nodeIndex];
    glTF::Node& currentNode = nodes[nodeIndex];

    for (int childNodeIndex : tinyNode.children) {
        nodes[childNodeIndex].parentIndex = static_cast<int>(nodeIndex);
    }

    if (tinyNode.camera > -1) {
        currentNode.cameraIndex = tinyNode.camera;
    }

    currentNode.children = tinyNode.children;

    if (tinyNode.skin > -1) {
        currentNode.skinIndex = tinyNode.skin;
    }

    if (tinyNode.matrix.size() == 16) {
        currentNode.matrix = glm::make_mat4x4(tinyNode.matrix.data());
    } else {
        currentNode.matrix = glm::mat4(1.0f);
    }

    if (tinyNode.mesh > -1) {
        currentNode.meshIndex = tinyNode.mesh;
    }

    if (tinyNode.rotation.size() == 4) {
        currentNode.rotation = glm::make_quat(tinyNode.rotation.data());
    } else {
        currentNode.rotation = glm::quat(0.0f, 0.0f, 0.0f, 1.0f);
    }

    if (tinyNode.scale.size() == 3) {
        currentNode.scale = glm::make_vec3(tinyNode.scale.data());
    } else {
        currentNode.scale = glm::vec3(1.0f);
    }

    if (tinyNode.translation.size() == 3) {
        currentNode.translation = glm::make_vec3(tinyNode.translation.data());
    } else {
        currentNode.translation = glm::vec3(0.0f);
    }

    currentNode.uniformBuffer = p_memoryAllocator->CreateBuffer(sizeof(glm::mat4),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);
}

void glTF::Asset::LoadMesh(tinygltf::Model& tinyModel, size_t meshIndex)
{
    tinygltf::Mesh& tinyMesh = tinyModel.meshes[meshIndex];
    glTF::Mesh& currentMesh = meshes[meshIndex];

    currentMesh.primitives.resize(tinyMesh.primitives.size());
    for (size_t i = 0; i < tinyMesh.primitives.size(); i++) {
        LoadPrimitive(tinyModel, meshIndex, i);
    }
}

void glTF::Asset::LoadPrimitive(tinygltf::Model& tinyModel, size_t meshIndex, size_t primitiveIndex)
{
    tinygltf::Primitive& tinyPrimitive = tinyModel.meshes[meshIndex].primitives[primitiveIndex];
    glTF::Primitive& currentPrimitive = meshes[meshIndex].primitives[primitiveIndex];

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

        size_t posStride = posBufferView.byteStride
            ? posBufferView.byteStride / tinygltf::GetComponentSizeInBytes(posAccessor.componentType)
            : tinygltf::GetNumComponentsInType(posAccessor.type);

        vertices.resize(numPos);
        for (size_t i = 0; i < vertices.size(); ++i) {
            vertices[i].pos = glm::make_vec3(pPos + i * posStride);
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
        size_t texcoordStride = texcoordBufferView.byteStride
            ? texcoordBufferView.byteStride / tinygltf::GetComponentSizeInBytes(texcoordAccessor.componentType)
            : tinygltf::GetNumComponentsInType(texcoordAccessor.type);

        for (size_t i = 0; i < vertices.size(); ++i) {
            vertices[i].uv = glm::make_vec2(pTexcoord + i * texcoordStride);
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
    } else {
        // indices undefined
        for (size_t i = 0; i < vertices.size(); i++) {
            indices.push_back(static_cast<uint32_t>(i));
        }
    }

#pragma endregion

    if (tinyPrimitive.material > -1) {
        currentPrimitive.materialIndex = tinyPrimitive.material;
    }

    currentPrimitive.vertexBuffer = CreateVertexBuffer(vertices);
    currentPrimitive.indexBuffer = CreateIndexBuffer(indices);
    currentPrimitive.indexCount = static_cast<uint32_t>(indices.size());
}

void glTF::Asset::LoadTexture(tinygltf::Model& tinyModel, size_t textureIndex)
{
    tinygltf::Texture& tinyTexture = tinyModel.textures[textureIndex];
    glTF::Texture& currentTexture = textures[textureIndex];

    if (tinyTexture.source > -1) {
        currentTexture.texture = CreateTexture(tinyModel.images[tinyTexture.source]);
    }

    if (tinyTexture.sampler > -1) {
        // TODO
    } else {
        // use default sampler repeat wrapping
    }
}

void glTF::Asset::LoadMaterial(tinygltf::Model& tinyModel, size_t materialIndex)
{
    tinygltf::Material& tinyMaterial = tinyModel.materials[materialIndex];
    glTF::Material& currentMaterial = materials[materialIndex];

    currentMaterial.uniformData.baseColorFactor = glm::make_vec4(tinyMaterial.pbrMetallicRoughness.baseColorFactor.data());
    currentMaterial.uniformData.metallicFactor = static_cast<float>(tinyMaterial.pbrMetallicRoughness.metallicFactor);
    currentMaterial.uniformData.roughnessFactor = static_cast<float>(tinyMaterial.pbrMetallicRoughness.roughnessFactor);

    if (tinyMaterial.pbrMetallicRoughness.baseColorTexture.index > -1) {
        currentMaterial.baseColorTextureIndex = tinyMaterial.pbrMetallicRoughness.baseColorTexture.index;
    }

    if (tinyMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index > -1) {
        currentMaterial.metallicRoughnessTextureIndex = tinyMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index;
    }

    currentMaterial.uniformBuffer = p_memoryAllocator->CreateBuffer(
        sizeof(glm::vec4) * 2,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);
}

VK::Buffer glTF::Asset::CreateVertexBuffer(const std::vector<Vertex>& vertices)
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

VK::Buffer glTF::Asset::CreateIndexBuffer(const std::vector<uint32_t>& indices)
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

VK::Image glTF::Asset::CreateTexture(tinygltf::Image& tinyImage)
{
    VK::CommandBuffer commandBuffer = p_commandPool->AllocateCommandBuffer();
    commandBuffer.Begin();

    VkBufferUsageFlags stagingBufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VmaAllocationCreateFlags stagingBufferAllocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    VK::Buffer stagingBuffer = p_memoryAllocator->CreateBuffer(tinyImage.image.size(), stagingBufferUsage, stagingBufferAllocationFlags);
    stagingBuffer.CopyData(tinyImage.image.data(), tinyImage.image.size());

    VkExtent3D extent3D = { static_cast<uint32_t>(tinyImage.width), static_cast<uint32_t>(tinyImage.height), 1 };

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

glm::mat4 glTF::Asset::GetNodeMatrix(size_t nodeIndex)
{
    glTF::Node& currentNode = nodes[nodeIndex];

    glm::mat4 currentMatrix = currentNode.matrix;

    if (currentMatrix == glm::mat4(1.0f)) {
        glm::mat4 T { glm::translate(glm::mat4(1.0f), currentNode.translation) };
        glm::mat4 R { glm::mat4_cast(currentNode.rotation) };
        glm::mat4 S { glm::scale(glm::mat4(1.0f), currentNode.scale) };

        currentMatrix = T * R * S;
    }

    if (currentNode.parentIndex.has_value()) {
        return GetNodeMatrix(currentNode.parentIndex.value()) * currentMatrix;
    }

    return currentMatrix;
}
