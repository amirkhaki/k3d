//
// Created by amir on 10/29/23.
//

#include "Model.h"

namespace k3d {
    Model::Model(Device &device, const std::vector<Vertex> &vertices) : device{device} {
        createVertexBuffers(vertices);
    }

    void Model::createVertexBuffers(const std::vector<Vertex> &vertices) {
        vertexCount = vertices.size();
        assert(vertexCount >= 3 && "at least 3 vertices are required");
        vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

        device.createBuffer(bufferSize,
                            vk::BufferUsageFlagBits::eVertexBuffer,
                            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                            vertexBuffer, vertexMemory
        );
        void *data = device.device().mapMemory(vertexMemory.get(), 0, bufferSize);
        memcpy(data, vertices.data(), bufferSize);
        device.device().unmapMemory(vertexMemory.get());
    }

    void Model::bind(vk::CommandBuffer commandBuffer) {
        commandBuffer.bindVertexBuffers(0, vertexBuffer.get(), vk::DeviceSize{0});
    }

    void Model::draw(vk::CommandBuffer commandBuffer) const {
        commandBuffer.draw(vertexCount, 1, 0, 0);
    }

    Model::~Model() = default;

    std::vector<vk::VertexInputBindingDescription> Model::Vertex::getBindingDescriptions() {
        return {
                {
                        .binding = 0,
                        .stride = sizeof(Vertex),
                        .inputRate = vk::VertexInputRate::eVertex,
                },
        };
    }

    std::vector<vk::VertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions() {
        return {
                {
                        .location = 0,
                        .binding = 0,
                        .format = vk::Format::eR32G32Sfloat,
                        .offset = 0,
                },
        };
    }
} // k3d