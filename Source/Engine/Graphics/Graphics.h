#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "vec4.hpp"
#include "../../ThirdParty/glfw/deps/glad/gl.h"
#include "code/Common/Win32DebugLogStream.h"

namespace graphics
{
enum class ShaderType
{
	kVertex = 0, kFragment, kGeometry, kCompute, kHull, kDomain, kCount
};

enum class ResourceType
{
	kSampler = 0, kTexture, kTextureRW, kConstants, kBuffer, kBufferRW, kCount
};

enum class PassType
{
	kGraphics = 0, kCompute, kCount
};

enum class PropertyType
{
	kFloat = 0, kInt, kRange, kColor, kVector, kTexture1D, kTexture2D, kTexture3D, kTextureVolume, kUnknown
};

enum class CullMode
{
	kNone = 0, kFront, kBack, kCount
};

enum class FontCounterClockwise //TODO:rename
{
	kFalse = 0, kTrue, kCount
};

enum class FillMode
{
	kSolid = 0, kWireframe, kPoint, kCount
};

enum class TextureFormat
{
	UNKNOWN,
	R32G32B32A32_TYPELESS,
	R32G32B32A32_FLOAT,
	R32G32B32A32_UINT,
	R32G32B32A32_SINT,
	R32G32B32_TYPELESS,
	R32G32B32_FLOAT,
	R32G32B32_UINT,
	R32G32B32_SINT,
	R16G16B16A16_TYPELESS,
	R16G16B16A16_FLOAT,
	R16G16B16A16_UNORM,
	R16G16B16A16_UINT,
	R16G16B16A16_SNORM,
	R16G16B16A16_SINT,
	R32G32_TYPELESS,
	R32G32_FLOAT,
	R32G32_UINT,
	R32G32_SINT,
	R10G10B10A2_TYPELESS,
	R10G10B10A2_UNORM,
	R10G10B10A2_UINT,
	R11G11B10_FLOAT,
	R8G8B8A8_TYPELESS,
	R8G8B8A8_UNORM,
	R8G8B8A8_UNORM_SRGB,
	R8G8B8A8_UINT,
	R8G8B8A8_SNORM,
	R8G8B8A8_SINT,
	R16G16_TYPELESS,
	R16G16_FLOAT,
	R16G16_UNORM,
	R16G16_UINT,
	R16G16_SNORM,
	R16G16_SINT,
	R32_TYPELESS,
	R32_FLOAT,
	R32_UINT,
	R32_SINT,
	R8G8_TYPELESS,
	R8G8_UNORM,
	R8G8_UINT,
	R8G8_SNORM,
	R8G8_SINT,
	R16_TYPELESS,
	R16_FLOAT,
	R16_UNORM,
	R16_UINT,
	R16_SNORM,
	R16_SINT,
	R8_TYPELESS,
	R8_UNORM,
	R8_UINT,
	R8_SNORM,
	R8_SINT,
	R9G9B9E5_SHAREDEXP,
	D32_FLOAT_S8X24_UINT,
	D24_UNORM_S8_UINT,
	D32_FLOAT,
	D24_UNORM_X8_UINT,
	D16_UNORM,
	S8_UINT,
	BC1_TYPELESS,
	BC1_UNORM,
	BC1_UNORM_SRGB,
	BC2_TYPELESS,
	BC2_UNORM,
	BC2_UNORM_SRGB,
	BC3_TYPELESS,
	BC3_UNORM,
	BC3_UNORM_SRGB,
	BC4_TYPELESS,
	BC4_UNORM,
	BC4_SNORM,
	BC5_TYPELESS,
	BC5_UNORM,
	BC5_SNORM,
	B5G6R5_UNORM,
	B5G5R5A1_UNORM,
	B8G8R8A8_UNORM,
	B8G8R8X8_UNORM,
	R10G10B10_XR_BIAS_A2_UNORM,
	B8G8R8A8_TYPELESS,
	B8G8R8A8_UNORM_SRGB,
	B8G8R8X8_TYPELESS,
	B8G8R8X8_UNORM_SRGB,
	BC6H_TYPELESS,
	BC6H_UF16,
	BC6H_SF16,
	BC7_TYPELESS,
	BC7_UNORM,
	BC7_UNORM_SRGB,
	FORCE_UINT,
	Count
};

enum class BufferType
{
	Vertex, Index, Constant, Indirect, Count
};

enum class ResourceUsageType
{
	Immutable, Dynamic, Stream, Count
};

enum class PrimitiveType
{
	Unknown, Point, Line, Triangle, Patch, Count
};

enum class CommandType
{
	BindPipeline, BindResourceListLayout, BindVertexBuffer, BindIndexBuffer, BindResourceList, Draw, DrawIndexed, DrawInstanced, DrawIndexedInstanced,
	Dispatch, CopyResource, SetScissor, SetViewport, Clear, ClearDepth, ClearStencil, BeginPass, EndPass, Count
};

struct RasterizationState
{
	CullMode cull_mode_;
	FontCounterClockwise front_counter_clockwise_;
	FillMode fill_mode_;
};

struct DepthStencilState
{
	//TODO
	bool depth_test_;
	bool depth_write_;
};

struct BlendState
{
	//TODO
	bool blend_enable_;
};

class PipelineCreation
{};

class ResourceListLayoutCreation
{};

class ResourceListLayout
{};

class ResourceListCreation
{};

class BufferCreation
{
public:
	BufferCreation(BufferType type, ResourceUsageType usage, uint32_t size, std::string name) :
		type_(type), usage_(usage), size_(size), name_(std::move(name)) {}

protected:
	BufferType type_;
	ResourceUsageType usage_;
	uint32_t size_;
	std::string name_;
};

class TextureCreation
{
public:
	TextureCreation(uint32_t width, uint32_t height, TextureFormat format, std::string name) :
		width_(width), height_(height), format_(format), name_(std::move(name)) {}

	uint32_t width_;
	uint32_t height_;
	TextureFormat format_;
	std::string name_;
};

class Texture
{};

class Buffer
{
public:
	GLuint gl_handle_;
};

class Pipeline
{};

class ResourceList
{};

class Shader
{};

struct Rect2D
{
	float x = 0.0f;
	float y = 0.0f;
	float width = 0.0f;
	float height = 0.0f;
};

struct Viewport
{
	Rect2D rect;
	float min_depth = 0.0f;
	float max_depth = 0.0f;
};

class VertexBufferFactory
{
public:
	static std::shared_ptr<Buffer> CreateFullScreenQuad() { return std::make_shared<Buffer>(); }
};

class CommandBuffer
{
public:
	void BeginSubmit() {}
	void EndSubmit() {}
	void BindPipeline(std::shared_ptr<Pipeline> pipeline) {}
	void BindResourceList(std::shared_ptr<ResourceList> resource_list) {}
	void BindVertexBuffer(std::shared_ptr<Buffer> buffer, uint32_t binding, uint32_t offset) {}
	void Draw(PrimitiveType type, uint32_t start, uint32_t count, uint32_t instance_count = 0) {}
	void Dispatch(uint32_t group_x, uint32_t group_y, uint32_t group_z) {}
};

class Sampler
{};

class RenderPass
{
public:
	GLuint fbo_handle_;
	uint32_t is_swapchain_;
};

class ResourcePool
{
public:
	using HandleType = uint32_t;
	static const HandleType kInvalidHandle = 0xFFFFFFFF;
	ResourcePool() {}

	HandleType AllocateResource()
	{
		if (current_head_count_ < max_count_)
		{
			//TODO
			return current_head_count_++;
		}
	}

	void ReleaseResource(HandleType handle)
	{
		// TODO
	}

	void* AccessResource(HandleType handle) { return const_cast<void*>(static_cast<const ResourcePool*>(this)->AccessResource(handle)); }

	const void* AccessResource(HandleType handle) const
	{
		if (handle != kInvalidHandle) { return &memory_[handle * resource_size_]; }
		return nullptr;
	}

protected:
	uint8_t* memory_;
	uint32_t max_count_;
	uint32_t resource_size_;
	uint32_t current_head_count_;
};

class Device;

class Command
{
public:
	virtual void Execute(Device& Device) = 0;

protected:
	CommandType type_;
};

class BeginPassCommand : public Command
{
public:
	virtual void Execute(Device& device) override;

protected:
	uint32_t handle_;
};

class EndPassCommand : public Command
{
public:
	virtual void Execute(Device& device) override;
};

class BindVertexBufferCommand : public Command
{
public:
	virtual void Execute(Device& device) override;

protected:
	uint32_t buffer_handle_;
	uint32_t binding_;
	uint32_t byte_offset_;
};

class BindIndexBufferCommand : public Command
{
public:
	virtual void Execute(Device& device) override;

protected:
	uint32_t buffer_handle_;
};

class SetViewportCommand : public Command
{
public:
	virtual void Execute(Device& device) override;

protected:
	Viewport viewport_;
};

class SetScissorCommand : public Command
{
public:
	virtual void Execute(Device& device) override;

protected:
	Rect2D scissor_;
};

class ClearColorCommand : public Command
{
public:
	virtual void Execute(Device& device) override;

protected:
	glm::vec4 clear_value_;
};

class ClearDepthCommand : public Command
{
public:
	virtual void Execute(Device& device) override;

protected:
	float clear_value_;
};

class ClearStencilCommand : public Command
{
public:
	virtual void Execute(Device& device) override;

protected:
	uint8_t clear_value_;
};

class BindPipelineCommand : public Command
{
public:
	virtual void Execute(Device& device) override;

protected:
	uint32_t handle_;
};

class BindResourceListCommand : public Command
{
public:
	virtual void Execute(Device& device) override;

protected:
	std::vector<uint32_t> handles_;
	std::vector<uint32_t> offsets_;
};

class DispatchCommand : public Command
{
public:
	virtual void Execute(Device& device) override;

protected:
	uint32_t group_count_x_;
	uint32_t group_count_y_;
	uint32_t group_count_z_;
};

class DrawCommand : public Command
{
public:
	virtual void Execute(Device& device) override;

protected:
	PrimitiveType primitive_type_;
	uint32_t first_vertex_;
	uint32_t vertex_count_;
	uint32_t instance_count_;
};

class DrawIndexedCommand : public Command
{
public:
	virtual void Execute(Device& device) override;

protected:
	PrimitiveType primitive_type_;
	uint32_t index_count_;
	uint32_t instance_count_;
	uint32_t first_index_;
	uint32_t vertex_offset_;
	uint32_t first_instance_;
};

class DeviceState
{
public:
	void Apply();

	const Viewport* viewport_;
	const Rect2D* scissor_;
	const Pipeline* pipeline_;
	std::vector<const ResourceList*> resource_lists_;
	std::vector<uint32_t> resource_offsets_;

	glm::vec4 clear_color_value_;
	float clear_depth_value_;
	uint8_t clear_stencil_value_;
	bool clear_color_flag_;
	bool clear_depth_flag_;
	bool clear_stencil_flag_;
	GLuint fbo_handle_;
	GLuint index_buffer_handle_;

	struct VertexBufferBinding
	{
		GLuint vb_handle_;
		uint32_t binding_;
		uint32_t offset_;
	};

	std::vector<VertexBufferBinding> vertex_buffer_bindings_;
	uint32_t vertex_streams_num_;

	bool swapchain_flag_;
	bool end_pass_flag_;
};

class Device
{
public:
	std::shared_ptr<Texture> CreateTexture(const TextureCreation& creation) { return std::make_shared<Texture>(); }

	std::shared_ptr<ResourceListLayout> CreateResourceListLayout(const ResourceListLayoutCreation& creation)
	{
		return std::make_shared<ResourceListLayout>();
	}

	std::shared_ptr<Buffer> CreateBuffer(const BufferCreation& creation) { return std::make_shared<Buffer>(); }
	std::shared_ptr<ResourceList> CreateResourceList(const ResourceListCreation& creation) { return std::make_shared<ResourceList>(); }
	std::shared_ptr<Pipeline> CreatePipeline(const PipelineCreation& creation) { return std::make_shared<Pipeline>(); }
	std::shared_ptr<CommandBuffer> ResetCommandBuffer() { return std::make_shared<CommandBuffer>(); }

#pragma region AccessResource

public:
	Pipeline* AccessPipeline(uint32_t handle) { return static_cast<Pipeline*>(pipelines_.AccessResource(handle)); }
	ResourceList* AccessResourceList(uint32_t handle) { return static_cast<ResourceList*>(resource_lists_.AccessResource(handle)); }
	Buffer* AccessBuffer(uint32_t handle) { return static_cast<Buffer*>(buffers_.AccessResource(handle)); }
	Texture* AccessTexture(uint32_t handle) { return static_cast<Texture*>(textures_.AccessResource(handle)); }
	Shader* AccessShader(uint32_t handle) { return static_cast<Shader*>(shaders_.AccessResource(handle)); }

	ResourceListLayout* AccessResourceListLayout(uint32_t handle)
	{
		return static_cast<ResourceListLayout*>(resource_list_layouts_.AccessResource(handle));
	}

	CommandBuffer* AccessCommandBuffer(uint32_t handle) { return static_cast<CommandBuffer*>(command_buffers_.AccessResource(handle)); }
	Sampler* AccessSampler(uint32_t handle) { return static_cast<Sampler*>(samplers_.AccessResource(handle)); }
	RenderPass* AccessRenderPass(uint32_t handle) { return static_cast<RenderPass*>(render_passes_.AccessResource(handle)); }

protected:
	ResourcePool buffers_;
	ResourcePool textures_;
	ResourcePool pipelines_;
	ResourcePool samplers_;
	ResourcePool resource_list_layouts_;
	ResourcePool resource_lists_;
	ResourcePool render_passes_;
	ResourcePool command_buffers_;
	ResourcePool shaders_;
#pragma endregion

public:
	DeviceState device_state_; //TODO hide
};
}
