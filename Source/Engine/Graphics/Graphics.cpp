#include "Graphics.h"

#include <stdexcept>

#include "glad/glad.h"

namespace graphics
{
void BeginPassCommand::Execute(Device& device)
{
	RenderPass* render_pass = device.AccessRenderPass(handle_);
	device.device_state_.fbo_handle_ = render_pass->fbo_handle_;
	device.device_state_.swapchain_flag_ = render_pass->is_swapchain_;
	device.device_state_.scissor_ = nullptr;
	device.device_state_.viewport_ = nullptr;
}

void EndPassCommand::Execute(Device& device)
{
	device.device_state_.end_pass_flag_ = true;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BindVertexBufferCommand::Execute(Device& device)
{
	Buffer* buffer = device.AccessBuffer(buffer_handle_);
	DeviceState::VertexBufferBinding& vb_binding = device.device_state_.vertex_buffer_bindings_[device.device_state_.vertex_streams_num_++];
	vb_binding.vb_handle_ = buffer->gl_handle_;
	vb_binding.offset_ = byte_offset_;
	vb_binding.binding_ = binding_;
}

void BindIndexBufferCommand::Execute(Device& device)
{
	Buffer* buffer = device.AccessBuffer(buffer_handle_);
	device.device_state_.index_buffer_handle_ = buffer->gl_handle_;
}

void SetViewportCommand::Execute(Device& device) { device.device_state_.viewport_ = &viewport_; }
void SetScissorCommand::Execute(Device& device) { device.device_state_.scissor_ = &scissor_; }

void ClearColorCommand::Execute(Device& device)
{
	device.device_state_.clear_color_value_ = clear_value_;
	device.device_state_.clear_color_flag_ = true;
}

void ClearDepthCommand::Execute(Device& device)
{
	device.device_state_.clear_depth_value_ = clear_value_;
	device.device_state_.clear_depth_flag_ = true;
}

void ClearStencilCommand::Execute(Device& device)
{
	device.device_state_.clear_stencil_value_ = clear_value_;
	device.device_state_.clear_stencil_flag_ = true;
}

void BindPipelineCommand::Execute(Device& device)
{
	const Pipeline* pipeline = device.AccessPipeline(handle_);
	device.device_state_.pipeline_ = pipeline;
}

void BindResourceListCommand::Execute(Device& device)
{
	for (uint32_t i = 0; i < handles_.size(); ++i)
	{
		const ResourceList* resource_list = device.AccessResourceList(handles_[i]);
		device.device_state_.resource_lists_[i] = resource_list;
	}
	for (uint32_t i = 0; i < offsets_.size(); ++i) { device.device_state_.resource_offsets_[i] = offsets_[i]; }
}

void DispatchCommand::Execute(Device& device)
{
	device.device_state_.Apply();
	glDispatchCompute(group_count_x_, group_count_y_, group_count_z_);
	// TODO: barrier handling.
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void DrawCommand::Execute(Device& device)
{
	device.device_state_.Apply();
	if (instance_count_) { glDrawArraysInstanced(GL_TRIANGLES, first_vertex_, vertex_count_, instance_count_); }
	else { glDrawArrays(GL_TRIANGLES, first_vertex_, vertex_count_); }
}

void DrawIndexedCommand::Execute(Device& device)
{
	device.device_state_.Apply();
	const uint32_t index_buffer_size = 2;
	const GLuint start_index_offset = first_index_;
	const GLuint end_index_offset = start_index_offset + index_count_;
	if (instance_count_)
	{
		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, (GLsizei)index_count_, GL_UNSIGNED_SHORT,
			(void*)(start_index_offset * index_buffer_size), instance_count_, vertex_offset_, first_instance_);
	}
	else
	{
		glDrawRangeElementsBaseVertex(GL_TRIANGLES, start_index_offset, end_index_offset, index_count_,GL_UNSIGNED_SHORT, (void*)(
			start_index_offset * index_buffer_size), vertex_offset_);
	}
}

//
// Magnification filter conversion to GL values.
//
static GLuint ToGlMagFilterType(TextureFilter::Enum filter)
{
	static GLuint kGlMagFilterType[TextureFilter::Count] = {GL_NEAREST, GL_LINEAR};

	return kGlMagFilterType[filter];
}

//
// Minification filter conversion to GL values.
//
static GLuint ToGlMinFilterType(TextureFilter::Enum filter, TextureMipFilter::Enum mipmap)
{
	static GLuint kGlMinFilterType[4] = {GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR};

	return kGlMinFilterType[(filter * 2) + mipmap];
}

//
// Texture address mode conversion to GL values.
//
static GLuint ToGlTextureAddressMode(TextureAddressMode::Enum mode)
{
	static GLuint kGlTextureAddressMode[TextureAddressMode::Count] = {GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER};

	return kGlTextureAddressMode[mode];
}

static GLuint ToGlShaderStage(ShaderStage::Enum stage)
{
	// TODO: hull/domain shader not supported for now.
	static GLuint kGlShaderStage[ShaderStage::Count] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER, GL_COMPUTE_SHADER, 0, 0};
	return kGlShaderStage[stage];
}

static GLuint ToGlBufferType(BufferType::Enum type)
{
	static GLuint kGlBufferTypes[BufferType::Count] = {GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER, GL_UNIFORM_BUFFER, GL_DRAW_INDIRECT_BUFFER};
	return kGlBufferTypes[type];
}

static GLuint ToGlBufferUsage(ResourceUsageType::Enum type)
{
	static GLuint kGlBufferUsages[ResourceUsageType::Count] = {GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_DYNAMIC_DRAW};
	return kGlBufferUsages[type];
}

static GLuint ToGlComparison(ComparisonFunction::Enum comparison)
{
	static GLuint kGlComparison[ComparisonFunction::Enum::Count] = {
		GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, GL_ALWAYS
	};
	return kGlComparison[comparison];
}

static GLenum ToGlBlendFunction(Blend::Enum blend)
{
	static GLenum kGlBlendFunction[] = {
		GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA,
		GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR,
		GL_SRC_ALPHA_SATURATE, GL_SRC1_COLOR, GL_ONE_MINUS_SRC1_COLOR, GL_SRC1_ALPHA, GL_ONE_MINUS_SRC1_ALPHA
	};
	return kGlBlendFunction[blend];
}

static GLenum ToGlBlendEquation(BlendOperation::Enum blend)
{
	static GLenum kGlBlendEquation[] = {GL_FUNC_ADD, GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT, GL_MIN, GL_MAX};
	return kGlBlendEquation[blend];
}

// Float, Float2, Float3, Float4, Mat4, Byte, Byte4N, UByte, UByte4N, Short2, Short2N, Short4, Short4N, Count
static GLuint ToGlComponents(VertexComponentFormat::Enum format)
{
	static GLuint kGlComponents[] = {1, 2, 3, 4, 16, 1, 4, 1, 4, 2, 2, 4, 4};
	return kGlComponents[format];
}

static GLenum ToGlVertexType(VertexComponentFormat::Enum format)
{
	static GLenum kGlVertexType[] = {
		GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_BYTE, GL_BYTE, GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_SHORT, GL_SHORT, GL_SHORT
	};
	return kGlVertexType[format];
}

static GLboolean ToGlVertexNorm(VertexComponentFormat::Enum format)
{
	static GLboolean kGlVertexNorm[] = {
		GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE
	};
	return kGlVertexNorm[format];
}

void DeviceState::Apply()
{
	if (pipeline_->graphics_pipeline)
	{
		// Bind FrameBuffer
		if (!swapchain_flag_ && fbo_handle_ > 0) { glBindFramebuffer(GL_FRAMEBUFFER, fbo_handle_); }

		if (viewport_) { glViewport(viewport_->rect.x, viewport_->rect.y, viewport_->rect.width, viewport_->rect.height); }

		if (scissor_)
		{
			glEnable(GL_SCISSOR_TEST);
			glScissor(scissor_->x, scissor_->y, scissor_->width, scissor_->height);
		}
		else { glDisable(GL_SCISSOR_TEST); }

		// Bind shaders
		glUseProgram(pipeline_->gl_program_cached);

		if (num_lists) { for (uint32_t l = 0; l < num_lists; ++l) { resource_lists[l]->set(resource_offsets, num_offsets); } }

		// Set depth
		if (pipeline_->depth_stencil.depth_enable)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(ToGlComparison(pipeline_->depth_stencil.depth_comparison));
			glDepthMask(pipeline_->depth_stencil.depth_write_enable);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
			glDepthMask(false);
		}

		// Set stencil
		if (pipeline_->depth_stencil.stencil_enable) { throw std::runtime_error("Not implemented."); }
		else { glDisable(GL_STENCIL_TEST); }

		if (clear_color_flag_ || clear_depth_flag_ || clear_stencil_flag_)
		{
			glClearColor(clear_color_value_.r, clear_color_value_.g, clear_color_value_.b, clear_color_value_.a);

			// TODO[gabriel]: single color mask enabling/disabling.
			GLuint clear_mask = GL_COLOR_BUFFER_BIT;
			clear_mask = clear_depth_flag_
			             ? clear_mask | GL_DEPTH_BUFFER_BIT
			             : clear_mask;
			clear_mask = clear_stencil_flag_
			             ? clear_mask | GL_STENCIL_BUFFER_BIT
			             : clear_mask;

			if (clear_depth_flag_) { glClearDepth(clear_depth_value_); }

			if (clear_stencil_flag_) { glClearStencil(clear_stencil_value_); }

			glClear(clear_mask);

			// TODO: use this version instead ?
			//glClearBufferfv( GL_COLOR, col_buff_index, &rgba );
		}

		// Set blend
		if (pipeline_->blend_state.active_states)
		{
			// If there is different states, set them accordingly.
			glEnablei(GL_BLEND, 0);

			const BlendState& blend_state = pipeline_->blend_state.blend_states[0];
			glBlendFunc(ToGlBlendFunction(blend_state.source_color),
				ToGlBlendFunction(blend_state.destination_color));

			glBlendEquation(ToGlBlendEquation(blend_state.color_operation));
		}
		else if (pipeline_->blend_state.active_states > 1)
		{
			throw std::runtime_error("Not implemented.");

			//glBlendFuncSeparate( glSrcFunction, glDstFunction, glSrcAlphaFunction, glDstAlphaFunction );
		}
		else { glDisable(GL_BLEND); }

		const RasterizationCreation& rasterization = pipeline_->rasterization;
		if (rasterization.cull_mode == CullMode::kNone) { glDisable(GL_CULL_FACE); }
		else
		{
			glEnable(GL_CULL_FACE);
			glCullFace(rasterization.cull_mode == CullMode::kFront
			           ? GL_FRONT
			           : GL_BACK);
		}

		glFrontFace(rasterization.front == FontCounterClockwise::kTrue
		            ? GL_CW
		            : GL_CCW);

		// Bind vertex array, containing vertex attributes.
		glBindVertexArray(pipeline_->gl_vao);

		// Bind Index Buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_handle_);

		// Bind Vertex Buffers with offsets.
		const VertexInput& vertex_input = pipeline_->vertex_input;
		for (uint32_t i = 0; i < vertex_input.num_streams; i++)
		{
			const VertexStream& stream = vertex_input.vertex_streams[i];

			glBindVertexBuffer(stream.binding, vertex_buffer_bindings_[i].vb_handle_, vertex_buffer_bindings_[i].offset_, stream.stride);
		}

		// Reset cached states
		clear_color_flag_ = false;
		clear_depth_flag_ = false;
		clear_stencil_flag_ = false;
		vertex_streams_num_ = 0;
	}
	else
	{
		glUseProgram(pipeline_->gl_program_cached);

		for (uint32_t i = 0; i < resource_lists_.size(); ++i) { resource_lists_[i]->set(resource_offsets, num_offsets); }
	}
}
}
