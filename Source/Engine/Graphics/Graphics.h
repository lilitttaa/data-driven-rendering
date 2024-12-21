#pragma once
#include <cstdint>
#include <memory>
#include <string>

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

enum class FontCounterClockwise
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
{};

class Pipeline
{};

class ResourceList
{};

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

class ResourcePool
{
public:
	using HandleType = uint32_t;
	static const HandleType kInvalidHandle = 0xFFFFFFFF;
	ResourcePool()
	{
		
	}
	HandleType AllocateResource()
	{
		if(current_head_count_ < max_count_)
		{
			return current_head_count_++;
		}
	}
	void ReleaseResource(HandleType handle)
	{
		
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
};
}
