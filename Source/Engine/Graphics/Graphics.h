#pragma once

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
}
