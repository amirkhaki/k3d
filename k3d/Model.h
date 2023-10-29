//
// Created by amir on 10/29/23.
//

#ifndef K3D_MODEL_H
#define K3D_MODEL_H

#include "device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>

namespace k3d {

    class Model {
    public:
        struct Vertex {
            glm::vec2 position;

            static std::vector<vk::VertexInputBindingDescription> getBindingDescriptions();

            static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions();

        };

        Model(Device &device, const std::vector<Vertex> &vertices);

        ~ Model();

        Model(const Model &) = delete;

        Model operator=(const Model &) = delete;

        void bind(vk::CommandBuffer commandBuffer);

        void draw(vk::CommandBuffer commandBuffer) const;

    private:
        void createVertexBuffers(const std::vector<Vertex> &vertices);

        Device &device;
        vk::UniqueBuffer vertexBuffer;
        vk::UniqueDeviceMemory vertexMemory;
        uint32_t vertexCount{};

    };

} // k3d

#endif //K3D_MODEL_H
