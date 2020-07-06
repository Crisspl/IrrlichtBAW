// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_VIDEO_OPEN_GL_H_INCLUDED__
#define __C_VIDEO_OPEN_GL_H_INCLUDED__

#include "irr/core/core.h"

#include "SIrrCreationParameters.h"

namespace irr
{
	class CIrrDeviceWin32;
	class CIrrDeviceLinux;
	class CIrrDeviceSDL;
	class CIrrDeviceMacOSX;
}

#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "IDriverMemoryAllocation.h"
#include "irr/video/COpenGLSpecializedShader.h"
#include "irr/video/COpenGLRenderpassIndependentPipeline.h"
#include "irr/video/COpenGLDescriptorSet.h"
#include "irr/video/COpenGLPipelineLayout.h"
#include "irr/video/COpenGLComputePipeline.h"

#include "CNullDriver.h"
// also includes the OpenGL stuff
#include "COpenGLFrameBuffer.h"
#include "COpenGLDriverFence.h"
#include "irr/video/CCUDAHandler.h"
#include "COpenCLHandler.h"

#include <map>
//#include <atomic>
//#include <thread>
#include <mutex>
//#include <condition_variable>

namespace irr
{
namespace video
{

    enum GL_STATE_BITS : uint32_t
    {
        // has to be flushed before constants are pushed (before `extGlProgramUniform*`)
        GSB_PIPELINE = 1u << 0,
        GSB_RASTER_PARAMETERS = 1u << 1,
        // we want the two to happen together and just before a draw (set VAO first, then binding)
        GSB_VAO_AND_VERTEX_INPUT = 1u << 2,
        // flush just before (indirect)dispatch or (multi)(indirect)draw, textures and samplers first, then storage image, then SSBO, finally UBO
        GSB_DESCRIPTOR_SETS = 1u << 3,
        // GL_DISPATCH_INDIRECT_BUFFER 
        GSB_DISPATCH_INDIRECT = 1u << 4,
        GSB_PUSH_CONSTANTS = 1u << 5,
        GSB_PIXEL_PACK_UNPACK = 1u << 6,
        // flush everything
        GSB_ALL = ~0x0u
    };


struct SOpenGLState
{
    struct SVAO {
        GLuint GLname;
        uint64_t lastUsed;
    };
    struct HashVAOPair
    {
		COpenGLRenderpassIndependentPipeline::SVAOHash first = {};
		SVAO second = { 0u,0ull };
		//extra vao state being cached
		std::array<asset::SBufferBinding<const COpenGLBuffer>, IGPUMeshBuffer::MAX_ATTR_BUF_BINDING_COUNT> vtxBindings;
		core::smart_refctd_ptr<const COpenGLBuffer> idxBinding;

        inline bool operator<(const HashVAOPair& rhs) const { return first < rhs.first; }
    };
    struct SDescSetBnd {
        core::smart_refctd_ptr<const COpenGLPipelineLayout> pplnLayout;
        core::smart_refctd_ptr<const COpenGLDescriptorSet> set;
        core::smart_refctd_dynamic_array<uint32_t> dynamicOffsets;
    };

    using SGraphicsPipelineHash = std::array<GLuint, COpenGLRenderpassIndependentPipeline::SHADER_STAGE_COUNT>;

    struct {
        struct {
            core::smart_refctd_ptr<const COpenGLRenderpassIndependentPipeline> pipeline;
            SGraphicsPipelineHash usedShadersHash = { 0u, 0u, 0u, 0u, 0u };
			GLuint usedPipeline = 0u;
        } graphics;
        struct {
            core::smart_refctd_ptr<const COpenGLComputePipeline> pipeline;
            GLuint usedShader = 0u;
        } compute;
    } pipeline;

    struct {
        core::smart_refctd_ptr<const COpenGLBuffer> buffer;
    } dispatchIndirect;

    struct {
        //in GL it is possible to set polygon mode separately for back- and front-faces, but in VK it's one setting for both
        GLenum polygonMode = GL_FILL;
        GLenum faceCullingEnable = 0;
        GLenum cullFace = GL_BACK;
        //in VK stencil params (both: stencilOp and stencilFunc) are 2 distinct for back- and front-faces, but in GL it's one for both
        struct SStencilOp {
            GLenum sfail = GL_KEEP;
            GLenum dpfail = GL_KEEP;
            GLenum dppass = GL_KEEP;
            bool operator!=(const SStencilOp& rhs) const { return sfail!=rhs.sfail || dpfail!=rhs.dpfail || dppass!=rhs.dppass; }
        };
        SStencilOp stencilOp_front, stencilOp_back;
        struct SStencilFunc {
            GLenum func = GL_ALWAYS;
            GLint ref = 0;
            GLuint mask = ~static_cast<GLuint>(0u);
            bool operator!=(const SStencilFunc& rhs) const { return func!=rhs.func || ref!=rhs.ref || mask!=rhs.mask; }
        };
        SStencilFunc stencilFunc_front, stencilFunc_back;
        GLenum depthFunc = GL_LESS;
        GLenum frontFace = GL_CCW;
        GLboolean depthClampEnable = 0;
        GLboolean rasterizerDiscardEnable = 0;
        GLboolean polygonOffsetEnable = 0;
        struct SPolyOffset {
            GLfloat factor = 0.f;//depthBiasSlopeFactor 
            GLfloat units = 0.f;//depthBiasConstantFactor 
            bool operator!=(const SPolyOffset& rhs) const { return factor!=rhs.factor || units!=rhs.units; }
        } polygonOffset;
        GLfloat lineWidth = 1.f;
        GLboolean sampleShadingEnable = 0;
        GLfloat minSampleShading = 0.f;
        GLboolean sampleMaskEnable = 0;
        GLbitfield sampleMask[2]{~static_cast<GLbitfield>(0), ~static_cast<GLbitfield>(0)};
        GLboolean sampleAlphaToCoverageEnable = 0;
        GLboolean sampleAlphaToOneEnable = 0;
        GLboolean depthTestEnable = 0;
        GLboolean depthWriteEnable = 1;
        //GLboolean depthBoundsTestEnable;
        GLboolean stencilTestEnable = 0;
        GLboolean multisampleEnable = 1;
        GLboolean primitiveRestartEnable = 0;

        GLboolean logicOpEnable = 0;
        GLenum logicOp = GL_COPY;
        struct SDrawbufferBlending
        {
            GLboolean blendEnable = 0;
            struct SBlendFunc {
                GLenum srcRGB = GL_ONE;
                GLenum dstRGB = GL_ZERO;
                GLenum srcAlpha = GL_ONE;
                GLenum dstAlpha = GL_ZERO;
                bool operator!=(const SBlendFunc& rhs) const { return srcRGB!=rhs.srcRGB || dstRGB!=rhs.dstRGB || srcAlpha!=rhs.srcAlpha || dstAlpha!=rhs.dstAlpha; }
            } blendFunc;
            struct SBlendEq {
                GLenum modeRGB = GL_FUNC_ADD;
                GLenum modeAlpha = GL_FUNC_ADD;
                bool operator!=(const SBlendEq& rhs) const { return modeRGB!=rhs.modeRGB || modeAlpha!=rhs.modeAlpha; }
            } blendEquation;
            struct SColorWritemask {
                GLboolean colorWritemask[4]{ 1,1,1,1 };
                bool operator!=(const SColorWritemask& rhs) const { return memcmp(colorWritemask, rhs.colorWritemask, 4); }
            } colorMask;
        } drawbufferBlend[asset::SBlendParams::MAX_COLOR_ATTACHMENT_COUNT];
    } rasterParams;

    struct {
		HashVAOPair vao = {};

        //putting it here because idk where else
        core::smart_refctd_ptr<const COpenGLBuffer> indirectDrawBuf;
        core::smart_refctd_ptr<const COpenGLBuffer> parameterBuf;//GL>=4.6
    } vertexInputParams;

    struct {
        SDescSetBnd descSets[IGPUPipelineLayout::DESCRIPTOR_SET_COUNT];
    } descriptorsParams[E_PIPELINE_BIND_POINT::EPBP_COUNT];

    struct SPixelPackUnpack {
        core::smart_refctd_ptr<const COpenGLBuffer> buffer;
        GLint alignment = 4;
        GLint rowLength = 0;
        GLint imgHeight = 0;
        GLint BCwidth = 0;
        GLint BCheight = 0;
        GLint BCdepth = 0;
    };
    SPixelPackUnpack pixelPack;
    SPixelPackUnpack pixelUnpack;
};

// GCC is special
template<E_PIPELINE_BIND_POINT>
struct pipeline_for_bindpoint;
template<> struct pipeline_for_bindpoint<EPBP_COMPUTE > { using type = COpenGLComputePipeline; };
template<> struct pipeline_for_bindpoint<EPBP_GRAPHICS> { using type = COpenGLRenderpassIndependentPipeline; };

template<E_PIPELINE_BIND_POINT PBP>
using pipeline_for_bindpoint_t = typename pipeline_for_bindpoint<PBP>::type;


class COpenGLDriver final : public CNullDriver
{
    protected:
		//! destructor
		virtual ~COpenGLDriver();

		//! inits the parts of the open gl driver used on all platforms
		bool genericDriverInit(asset::IAssetManager* assMgr) override;

	public:
        struct SAuxContext;

					enum EOpenGLFeatures {
				IRR_3DFX_multisample = 0,
				IRR_3DFX_tbuffer,
				IRR_3DFX_texture_compression_FXT1,
				IRR_AMD_blend_minmax_factor,
				IRR_AMD_conservative_depth,
				IRR_AMD_debug_output,
				IRR_AMD_depth_clamp_separate,
				IRR_AMD_draw_buffers_blend,
				IRR_AMD_multi_draw_indirect,
				IRR_AMD_name_gen_delete,
				IRR_AMD_performance_monitor,
				IRR_AMD_sample_positions,
				IRR_AMD_seamless_cubemap_per_texture,
				IRR_AMD_shader_stencil_export,
				IRR_AMD_texture_texture4,
				IRR_AMD_transform_feedback3_lines_triangles,
				IRR_AMD_vertex_shader_tesselator,
				IRR_AMD_gcn_shader,
				IRR_AMD_gpu_shader_half_float_fetch,
				IRR_AMD_shader_explicit_vertex_parameter,
				IRR_AMD_shader_fragment_mask,
				IRR_AMD_shader_image_load_store_lod,
				IRR_AMD_shader_trinary_minmax,
				IRR_AMD_texture_gather_bias_lod,
				IRR_AMD_vertex_shader_viewport_index,
				IRR_AMD_vertex_shader_layer,
				IRR_AMD_sparse_texture,
				IRR_AMD_shader_stencil_value_export,
				IRR_AMD_gpu_shader_int64,
				IRR_AMD_shader_ballot,
				IRR_APPLE_aux_depth_stencil,
				IRR_APPLE_client_storage,
				IRR_APPLE_element_array,
				IRR_APPLE_fence,
				IRR_APPLE_float_pixels,
				IRR_APPLE_flush_buffer_range,
				IRR_APPLE_object_purgeable,
				IRR_APPLE_rgb_422,
				IRR_APPLE_row_bytes,
				IRR_APPLE_specular_vector,
				IRR_APPLE_texture_range,
				IRR_APPLE_transform_hint,
				IRR_APPLE_vertex_array_object,
				IRR_APPLE_vertex_array_range,
				IRR_APPLE_vertex_program_evaluators,
				IRR_APPLE_ycbcr_422,
				IRR_ARB_base_instance,
				IRR_ARB_bindless_texture,
				IRR_ARB_buffer_storage,
				IRR_ARB_blend_func_extended,
				IRR_ARB_clip_control,
				IRR_ARB_cl_event,
				IRR_ARB_color_buffer_float,
				IRR_ARB_compatibility,
				IRR_ARB_compressed_texture_pixel_storage,
				IRR_ARB_compute_shader,
				IRR_ARB_conservative_depth,
				IRR_ARB_copy_buffer,
				IRR_ARB_debug_output,
				IRR_ARB_depth_buffer_float,
				IRR_ARB_depth_clamp,
				IRR_ARB_depth_texture,
				IRR_ARB_direct_state_access,
				IRR_ARB_draw_buffers,
				IRR_ARB_draw_buffers_blend,
				IRR_ARB_draw_elements_base_vertex,
				IRR_ARB_draw_indirect,
				IRR_ARB_draw_instanced,
				IRR_ARB_ES2_compatibility,
				IRR_ARB_explicit_attrib_location,
				IRR_ARB_explicit_uniform_location,
				IRR_ARB_fragment_coord_conventions,
				IRR_ARB_fragment_program,
				IRR_ARB_fragment_program_shadow,
				IRR_ARB_fragment_shader,
				IRR_ARB_fragment_shader_interlock,
				IRR_ARB_framebuffer_object,
				IRR_ARB_framebuffer_sRGB,
				IRR_ARB_geometry_shader4,
				IRR_ARB_get_program_binary,
				IRR_ARB_get_texture_sub_image,
				IRR_ARB_gpu_shader5,
				IRR_ARB_gpu_shader_fp64,
				IRR_ARB_half_float_pixel,
				IRR_ARB_half_float_vertex,
				IRR_ARB_imaging,
				IRR_ARB_instanced_arrays,
				IRR_ARB_indirect_parameters,
				IRR_ARB_internalformat_query,
				IRR_ARB_internalformat_query2,
				IRR_ARB_map_buffer_alignment,
				IRR_ARB_map_buffer_range,
				IRR_ARB_matrix_palette,
				IRR_ARB_multi_bind,
				IRR_ARB_multi_draw_indirect,
				IRR_ARB_multisample,
				IRR_ARB_multitexture,
				IRR_ARB_occlusion_query,
				IRR_ARB_occlusion_query2,
				IRR_ARB_pixel_buffer_object,
				IRR_ARB_point_parameters,
				IRR_ARB_point_sprite,
				IRR_ARB_program_interface_query,
				IRR_ARB_provoking_vertex,
				IRR_ARB_query_buffer_object,
				IRR_ARB_robustness,
				IRR_ARB_sample_shading,
				IRR_ARB_sampler_objects,
				IRR_ARB_seamless_cube_map,
				IRR_ARB_separate_shader_objects,
				IRR_ARB_shader_atomic_counters,
				IRR_ARB_shader_ballot,
				IRR_ARB_shader_bit_encoding,
				IRR_ARB_shader_draw_parameters,
				IRR_ARB_shader_group_vote,
				IRR_ARB_shader_image_load_store,
				IRR_ARB_shader_objects,
				IRR_ARB_shader_precision,
				IRR_ARB_shader_stencil_export,
				IRR_ARB_shader_subroutine,
				IRR_ARB_shader_texture_lod,
				IRR_ARB_shading_language_100,
				IRR_ARB_shading_language_420pack,
				IRR_ARB_shading_language_include,
				IRR_ARB_shading_language_packing,
				IRR_ARB_shadow,
				IRR_ARB_shadow_ambient,
				IRR_ARB_sync,
				IRR_ARB_tessellation_shader,
				IRR_ARB_texture_barrier,
				IRR_ARB_texture_border_clamp,
				IRR_ARB_texture_buffer_object,
				IRR_ARB_texture_buffer_object_rgb32,
				IRR_ARB_texture_buffer_range,
				IRR_ARB_texture_compression,
				IRR_ARB_texture_compression_bptc,
				IRR_ARB_texture_compression_rgtc,
				IRR_ARB_texture_cube_map,
				IRR_ARB_texture_cube_map_array,
				IRR_ARB_texture_env_add,
				IRR_ARB_texture_env_combine,
				IRR_ARB_texture_env_crossbar,
				IRR_ARB_texture_env_dot3,
				IRR_ARB_texture_float,
				IRR_ARB_texture_gather,
				IRR_ARB_texture_mirrored_repeat,
				IRR_ARB_texture_multisample,
				IRR_ARB_texture_non_power_of_two,
				IRR_ARB_texture_query_lod,
				IRR_ARB_texture_rectangle,
				IRR_ARB_texture_rg,
				IRR_ARB_texture_rgb10_a2ui,
				IRR_ARB_texture_stencil8,
				IRR_ARB_texture_storage,
				IRR_ARB_texture_storage_multisample,
				IRR_ARB_texture_swizzle,
				IRR_ARB_texture_view,
				IRR_ARB_timer_query,
				IRR_ARB_transform_feedback2,
				IRR_ARB_transform_feedback3,
				IRR_ARB_transform_feedback_instanced,
				IRR_ARB_transpose_matrix,
				IRR_ARB_uniform_buffer_object,
				IRR_ARB_vertex_array_bgra,
				IRR_ARB_vertex_array_object,
				IRR_ARB_vertex_attrib_64bit,
				IRR_ARB_vertex_attrib_binding,
				IRR_ARB_vertex_blend,
				IRR_ARB_vertex_buffer_object,
				IRR_ARB_vertex_program,
				IRR_ARB_vertex_shader,
				IRR_ARB_vertex_type_2_10_10_10_rev,
				IRR_ARB_viewport_array,
				IRR_ARB_window_pos,
				IRR_ARB_enhanced_layouts,
				IRR_ARB_cull_distance,
				IRR_ARB_derivative_control,
				IRR_ARB_shader_texture_image_samples,
				IRR_ARB_gpu_shader_int64,
				IRR_ARB_post_depth_coverage,
				IRR_ARB_shader_clock,
				IRR_ARB_shader_viewport_layer_array,
				IRR_ARB_sparse_texture2,
				IRR_ARB_sparse_texture_clamp,
				IRR_ARB_gl_spirv,
				IRR_ARB_spirv_extensions,
				IRR_ATI_draw_buffers,
				IRR_ATI_element_array,
				IRR_ATI_envmap_bumpmap,
				IRR_ATI_fragment_shader,
				IRR_ATI_map_object_buffer,
				IRR_ATI_meminfo,
				IRR_ATI_pixel_format_float,
				IRR_ATI_pn_triangles,
				IRR_ATI_separate_stencil,
				IRR_ATI_text_fragment_shader,
				IRR_ATI_texture_env_combine3,
				IRR_ATI_texture_float,
				IRR_ATI_texture_mirror_once,
				IRR_ATI_vertex_array_object,
				IRR_ATI_vertex_attrib_array_object,
				IRR_ATI_vertex_streams,
				IRR_EXT_422_pixels,
				IRR_EXT_abgr,
				IRR_EXT_bgra,
				IRR_EXT_bindable_uniform,
				IRR_EXT_blend_color,
				IRR_EXT_blend_equation_separate,
				IRR_EXT_blend_func_separate,
				IRR_EXT_blend_logic_op,
				IRR_EXT_blend_minmax,
				IRR_EXT_blend_subtract,
				IRR_EXT_clip_volume_hint,
				IRR_EXT_cmyka,
				IRR_EXT_color_subtable,
				IRR_EXT_compiled_vertex_array,
				IRR_EXT_convolution,
				IRR_EXT_coordinate_frame,
				IRR_EXT_copy_texture,
				IRR_EXT_cull_vertex,
				IRR_EXT_depth_bounds_test,
				IRR_EXT_direct_state_access,
				IRR_EXT_draw_buffers2,
				IRR_EXT_draw_instanced,
				IRR_EXT_draw_range_elements,
				IRR_EXT_fog_coord,
				IRR_EXT_framebuffer_blit,
				IRR_EXT_framebuffer_multisample,
				IRR_EXT_framebuffer_multisample_blit_scaled,
				IRR_EXT_framebuffer_object,
				IRR_EXT_framebuffer_sRGB,
				IRR_EXT_geometry_shader4,
				IRR_EXT_gpu_program_parameters,
				IRR_EXT_gpu_shader4,
				IRR_EXT_histogram,
				IRR_EXT_index_array_formats,
				IRR_EXT_index_func,
				IRR_EXT_index_material,
				IRR_EXT_index_texture,
				IRR_EXT_light_texture,
				IRR_EXT_misc_attribute,
				IRR_EXT_multi_draw_arrays,
				IRR_EXT_multisample,
				IRR_EXT_packed_depth_stencil,
				IRR_EXT_packed_float,
				IRR_EXT_packed_pixels,
				IRR_EXT_paletted_texture,
				IRR_EXT_pixel_buffer_object,
				IRR_EXT_pixel_transform,
				IRR_EXT_pixel_transform_color_table,
				IRR_EXT_point_parameters,
				IRR_EXT_polygon_offset,
				IRR_EXT_provoking_vertex,
				IRR_EXT_rescale_normal,
				IRR_EXT_secondary_color,
				IRR_EXT_separate_shader_objects,
				IRR_EXT_separate_specular_color,
				IRR_EXT_shader_image_load_store,
				IRR_EXT_shadow_funcs,
				IRR_EXT_shared_texture_palette,
				IRR_EXT_stencil_clear_tag,
				IRR_EXT_stencil_two_side,
				IRR_EXT_stencil_wrap,
				IRR_EXT_subtexture,
				IRR_EXT_texture,
				IRR_EXT_texture3D,
				IRR_EXT_texture_array,
				IRR_EXT_texture_buffer_object,
				IRR_EXT_texture_compression_latc,
				IRR_EXT_texture_compression_rgtc,
				IRR_EXT_texture_compression_s3tc,
				IRR_EXT_texture_cube_map,
				IRR_EXT_texture_env_add,
				IRR_EXT_texture_env_combine,
				IRR_EXT_texture_env_dot3,
				IRR_EXT_texture_filter_anisotropic,
				IRR_EXT_texture_integer,
				IRR_EXT_texture_lod_bias,
				IRR_EXT_texture_mirror_clamp,
				IRR_EXT_texture_object,
				IRR_EXT_texture_perturb_normal,
				IRR_EXT_texture_shared_exponent,
				IRR_EXT_texture_snorm,
				IRR_EXT_texture_sRGB,
				IRR_EXT_texture_sRGB_decode,
				IRR_EXT_texture_sRGB_R8,
				IRR_EXT_texture_sRGB_RG8,
				IRR_EXT_texture_swizzle,
				IRR_EXT_texture_view,
				IRR_EXT_timer_query,
				IRR_EXT_transform_feedback,
				IRR_EXT_vertex_array,
				IRR_EXT_vertex_array_bgra,
				IRR_EXT_vertex_attrib_64bit,
				IRR_EXT_vertex_shader,
				IRR_EXT_vertex_weighting,
				IRR_EXT_x11_sync_object,
				IRR_EXT_shader_pixel_local_storage,
				IRR_EXT_shader_pixel_local_storage2,
				IRR_EXT_shader_integer_mix,
				IRR_EXT_shader_image_load_formatted,
				IRR_EXT_post_depth_coverage,
				IRR_EXT_sparse_texture2,
				IRR_EXT_shader_framebuffer_fetch,
				IRR_EXT_shader_framebuffer_fetch_non_coherent,
				IRR_FfdMaskSGIX,
				IRR_GREMEDY_frame_terminator,
				IRR_GREMEDY_string_marker,
				IRR_HP_convolution_border_modes,
				IRR_HP_image_transform,
				IRR_HP_occlusion_test,
				IRR_HP_texture_lighting,
				IRR_IBM_cull_vertex,
				IRR_IBM_multimode_draw_arrays,
				IRR_IBM_rasterpos_clip,
				IRR_IBM_texture_mirrored_repeat,
				IRR_IBM_vertex_array_lists,
				IRR_INGR_blend_func_separate,
				IRR_INGR_color_clamp,
				IRR_INGR_interlace_read,
				IRR_INGR_palette_buffer,
				IRR_INTEL_fragment_shader_ordering,
				IRR_INTEL_parallel_arrays,
				IRR_INTEL_texture_scissor,
				IRR_INTEL_conservative_rasterization,
				IRR_INTEL_blackhole_render,
				IRR_KHR_debug,
				IRR_MESA_pack_invert,
				IRR_MESA_resize_buffers,
				IRR_MESA_window_pos,
				IRR_MESAX_texture_stack,
				IRR_MESA_ycbcr_texture,
				IRR_NV_blend_square,
				IRR_NV_conditional_render,
				IRR_NV_copy_depth_to_color,
				IRR_NV_copy_image,
				IRR_NV_depth_buffer_float,
				IRR_NV_depth_clamp,
				IRR_NV_evaluators,
				IRR_NV_explicit_multisample,
				IRR_NV_fence,
				IRR_NV_float_buffer,
				IRR_NV_fog_distance,
				IRR_NV_fragment_program,
				IRR_NV_fragment_program2,
				IRR_NV_fragment_program4,
				IRR_NV_fragment_program_option,
				IRR_NV_fragment_shader_interlock,
				IRR_NV_framebuffer_multisample_coverage,
				IRR_NV_geometry_program4,
				IRR_NV_geometry_shader4,
				IRR_NV_gpu_program4,
				IRR_NV_gpu_program5,
				IRR_NV_gpu_shader5,
				IRR_NV_half_float,
				IRR_NV_light_max_exponent,
				IRR_NV_multisample_coverage,
				IRR_NV_multisample_filter_hint,
				IRR_NV_occlusion_query,
				IRR_NV_packed_depth_stencil,
				IRR_NV_parameter_buffer_object,
				IRR_NV_parameter_buffer_object2,
				IRR_NV_pixel_data_range,
				IRR_NV_point_sprite,
				IRR_NV_present_video,
				IRR_NV_primitive_restart,
				IRR_NV_register_combiners,
				IRR_NV_register_combiners2,
				IRR_NV_shader_buffer_load,
				IRR_NV_shader_buffer_store,
				IRR_NV_shader_thread_group,
				IRR_NV_shader_thread_shuffle,
				IRR_NV_tessellation_program5,
				IRR_NV_texgen_emboss,
				IRR_NV_texgen_reflection,
				IRR_NV_texture_barrier,
				IRR_NV_texture_compression_vtc,
				IRR_NV_texture_env_combine4,
				IRR_NV_texture_expand_normal,
				IRR_NV_texture_multisample,
				IRR_NV_texture_rectangle,
				IRR_NV_texture_shader,
				IRR_NV_texture_shader2,
				IRR_NV_texture_shader3,
				IRR_NV_transform_feedback,
				IRR_NV_transform_feedback2,
				IRR_NV_vdpau_interop,
				IRR_NV_vertex_array_range,
				IRR_NV_vertex_array_range2,
				IRR_NV_vertex_attrib_integer_64bit,
				IRR_NV_vertex_buffer_unified_memory,
				IRR_NV_vertex_program,
				IRR_NV_vertex_program1_1,
				IRR_NV_vertex_program2,
				IRR_NV_vertex_program2_option,
				IRR_NV_vertex_program3,
				IRR_NV_vertex_program4,
				IRR_NV_video_capture,
				IRR_NV_viewport_array2,
				IRR_NV_stereo_view_rendering,
				IRR_NV_sample_mask_override_coverage,
				IRR_NV_geometry_shader_passthrough,
				IRR_NV_shader_subgroup_partitioned,
				IRR_NV_compute_shader_derivatives,
				IRR_NV_fragment_shader_barycentric,
				IRR_NV_mesh_shader,
				IRR_NV_shader_image_footprint,
				IRR_NV_shading_rate_image,
				IRR_NV_bindless_texture,
				IRR_NV_shader_atomic_float,
				IRR_NV_shader_atomic_int64,
				IRR_NV_sample_locations,
				IRR_NV_shader_atomic_fp16_vector,
				IRR_NV_command_list,
				IRR_NV_shader_atomic_float64,
				IRR_NV_conservative_raster_pre_snap,
				IRR_NV_shader_texture_footprint,
				IRR_OES_read_format,
				IRR_OML_interlace,
				IRR_OML_resample,
				IRR_OML_subsample,
				IRR_OVR_multiview,
				IRR_OVR_multiview2,
				IRR_PGI_misc_hints,
				IRR_PGI_vertex_hints,
				IRR_REND_screen_coordinates,
				IRR_S3_s3tc,
				IRR_SGI_color_matrix,
				IRR_SGI_color_table,
				IRR_SGI_depth_pass_instrument,
				IRR_SGIS_detail_texture,
				IRR_SGIS_fog_function,
				IRR_SGIS_generate_mipmap,
				IRR_SGIS_multisample,
				IRR_SGIS_pixel_texture,
				IRR_SGIS_point_line_texgen,
				IRR_SGIS_point_parameters,
				IRR_SGIS_sharpen_texture,
				IRR_SGIS_texture4D,
				IRR_SGIS_texture_border_clamp,
				IRR_SGIS_texture_color_mask,
				IRR_SGIS_texture_edge_clamp,
				IRR_SGIS_texture_filter4,
				IRR_SGIS_texture_lod,
				IRR_SGIS_texture_select,
				IRR_SGI_texture_color_table,
				IRR_SGIX_async,
				IRR_SGIX_async_histogram,
				IRR_SGIX_async_pixel,
				IRR_SGIX_blend_alpha_minmax,
				IRR_SGIX_calligraphic_fragment,
				IRR_SGIX_clipmap,
				IRR_SGIX_convolution_accuracy,
				IRR_SGIX_depth_pass_instrument,
				IRR_SGIX_depth_texture,
				IRR_SGIX_flush_raster,
				IRR_SGIX_fog_offset,
				IRR_SGIX_fog_scale,
				IRR_SGIX_fragment_lighting,
				IRR_SGIX_framezoom,
				IRR_SGIX_igloo_interface,
				IRR_SGIX_impact_pixel_texture,
				IRR_SGIX_instruments,
				IRR_SGIX_interlace,
				IRR_SGIX_ir_instrument1,
				IRR_SGIX_list_priority,
				IRR_SGIX_pixel_texture,
				IRR_SGIX_pixel_tiles,
				IRR_SGIX_polynomial_ffd,
				IRR_SGIX_reference_plane,
				IRR_SGIX_resample,
				IRR_SGIX_scalebias_hint,
				IRR_SGIX_shadow,
				IRR_SGIX_shadow_ambient,
				IRR_SGIX_sprite,
				IRR_SGIX_subsample,
				IRR_SGIX_tag_sample_buffer,
				IRR_SGIX_texture_add_env,
				IRR_SGIX_texture_coordinate_clamp,
				IRR_SGIX_texture_lod_bias,
				IRR_SGIX_texture_multi_buffer,
				IRR_SGIX_texture_scale_bias,
				IRR_SGIX_texture_select,
				IRR_SGIX_vertex_preclip,
				IRR_SGIX_ycrcb,
				IRR_SGIX_ycrcba,
				IRR_SGIX_ycrcb_subsample,
				IRR_SUN_convolution_border_modes,
				IRR_SUN_global_alpha,
				IRR_SUN_mesh_array,
				IRR_SUN_slice_accum,
				IRR_SUN_triangle_list,
				IRR_SUN_vertex,
				IRR_SUNX_constant_data,
				IRR_WIN_phong_shading,
				IRR_WIN_specular_fog,
				IRR_KHR_texture_compression_astc_hdr,
				IRR_KHR_texture_compression_astc_ldr,
				IRR_KHR_blend_equation_advanced,
				IRR_KHR_blend_equation_advanced_coherent,
				IRR_NVX_gpu_memory_info,
				IRR_NVX_multiview_per_view_attributes,
				IRR_OpenGL_Feature_Count
			};
		static bool FeatureAvailable[IRR_OpenGL_Feature_Count];

		static bool IsIntelGPU;
		static bool needsDSAFramebufferHack;
		static uint16_t Version;

		#ifdef _IRR_COMPILE_WITH_WINDOWS_DEVICE_
		COpenGLDriver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, CIrrDeviceWin32* device, const asset::IGLSLCompiler* glslcomp);
		//! inits the windows specific parts of the open gl driver
		bool initDriver(CIrrDeviceWin32* device);
		bool changeRenderContext(const SExposedVideoData& videoData, CIrrDeviceWin32* device);
		#endif

		#ifdef _IRR_COMPILE_WITH_X11_DEVICE_
		COpenGLDriver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, CIrrDeviceLinux* device, const asset::IGLSLCompiler* glslcomp);
		//! inits the GLX specific parts of the open gl driver
		bool initDriver(CIrrDeviceLinux* device, SAuxContext* auxCtxts);
		bool changeRenderContext(const SExposedVideoData& videoData, CIrrDeviceLinux* device);
		#endif

		#ifdef _IRR_COMPILE_WITH_SDL_DEVICE_
		COpenGLDriver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, CIrrDeviceSDL* device, const asset::IGLSLCompiler* glslcomp);
		#endif


        inline bool isAllowedBufferViewFormat(asset::E_FORMAT _fmt) const override
        {
            using namespace asset;
            switch (_fmt)
            {
				case EF_R8_UNORM: _IRR_FALLTHROUGH;
				case EF_R16_UNORM: _IRR_FALLTHROUGH;
				case EF_R16_SFLOAT: _IRR_FALLTHROUGH;
				case EF_R32_SFLOAT: _IRR_FALLTHROUGH;
				case EF_R8_SINT: _IRR_FALLTHROUGH;
				case EF_R16_SINT: _IRR_FALLTHROUGH;
				case EF_R32_SINT: _IRR_FALLTHROUGH;
				case EF_R8_UINT: _IRR_FALLTHROUGH;
				case EF_R16_UINT: _IRR_FALLTHROUGH;
				case EF_R32_UINT: _IRR_FALLTHROUGH;
				case EF_R8G8_UNORM: _IRR_FALLTHROUGH;
				case EF_R16G16_UNORM: _IRR_FALLTHROUGH;
				case EF_R16G16_SFLOAT: _IRR_FALLTHROUGH;
				case EF_R32G32_SFLOAT: _IRR_FALLTHROUGH;
				case EF_R8G8_SINT: _IRR_FALLTHROUGH;
				case EF_R16G16_SINT: _IRR_FALLTHROUGH;
				case EF_R32G32_SINT: _IRR_FALLTHROUGH;
				case EF_R8G8_UINT: _IRR_FALLTHROUGH;
				case EF_R16G16_UINT: _IRR_FALLTHROUGH;
				case EF_R32G32_UINT: _IRR_FALLTHROUGH;
				case EF_R32G32B32_SFLOAT: _IRR_FALLTHROUGH;
				case EF_R32G32B32_SINT: _IRR_FALLTHROUGH;
				case EF_R32G32B32_UINT: _IRR_FALLTHROUGH;
				case EF_R8G8B8A8_UNORM: _IRR_FALLTHROUGH;
				case EF_R16G16B16A16_UNORM: _IRR_FALLTHROUGH;
				case EF_R16G16B16A16_SFLOAT: _IRR_FALLTHROUGH;
				case EF_R32G32B32A32_SFLOAT: _IRR_FALLTHROUGH;
				case EF_R8G8B8A8_SINT: _IRR_FALLTHROUGH;
				case EF_R16G16B16A16_SINT: _IRR_FALLTHROUGH;
				case EF_R32G32B32A32_SINT: _IRR_FALLTHROUGH;
				case EF_R8G8B8A8_UINT: _IRR_FALLTHROUGH;
				case EF_R16G16B16A16_UINT: _IRR_FALLTHROUGH;
				case EF_R32G32B32A32_UINT:
					return true;
					break;
				default:
					return false;
					break;
            }
        }

        inline bool isAllowedVertexAttribFormat(asset::E_FORMAT _fmt) const override
        {
            using namespace asset;
            switch (_fmt)
            {
				// signed/unsigned byte
				case EF_R8_UNORM:
				case EF_R8_SNORM:
				case EF_R8_UINT:
				case EF_R8_SINT:
				case EF_R8G8_UNORM:
				case EF_R8G8_SNORM:
				case EF_R8G8_UINT:
				case EF_R8G8_SINT:
				case EF_R8G8B8_UNORM:
				case EF_R8G8B8_SNORM:
				case EF_R8G8B8_UINT:
				case EF_R8G8B8_SINT:
				case EF_R8G8B8A8_UNORM:
				case EF_R8G8B8A8_SNORM:
				case EF_R8G8B8A8_UINT:
				case EF_R8G8B8A8_SINT:
				case EF_R8_USCALED:
				case EF_R8_SSCALED:
				case EF_R8G8_USCALED:
				case EF_R8G8_SSCALED:
				case EF_R8G8B8_USCALED:
				case EF_R8G8B8_SSCALED:
				case EF_R8G8B8A8_USCALED:
				case EF_R8G8B8A8_SSCALED:
				// unsigned byte BGRA (normalized only)
				case EF_B8G8R8A8_UNORM:
				// unsigned/signed short
				case EF_R16_UNORM:
				case EF_R16_SNORM:
				case EF_R16_UINT:
				case EF_R16_SINT:
				case EF_R16G16_UNORM:
				case EF_R16G16_SNORM:
				case EF_R16G16_UINT:
				case EF_R16G16_SINT:
				case EF_R16G16B16_UNORM:
				case EF_R16G16B16_SNORM:
				case EF_R16G16B16_UINT:
				case EF_R16G16B16_SINT:
				case EF_R16G16B16A16_UNORM:
				case EF_R16G16B16A16_SNORM:
				case EF_R16G16B16A16_UINT:
				case EF_R16G16B16A16_SINT:
				case EF_R16_USCALED:
				case EF_R16_SSCALED:
				case EF_R16G16_USCALED:
				case EF_R16G16_SSCALED:
				case EF_R16G16B16_USCALED:
				case EF_R16G16B16_SSCALED:
				case EF_R16G16B16A16_USCALED:
				case EF_R16G16B16A16_SSCALED:
				// unsigned/signed int
				case EF_R32_UINT:
				case EF_R32_SINT:
				case EF_R32G32_UINT:
				case EF_R32G32_SINT:
				case EF_R32G32B32_UINT:
				case EF_R32G32B32_SINT:
				case EF_R32G32B32A32_UINT:
				case EF_R32G32B32A32_SINT:
				// unsigned/signed rgb10a2 BGRA (normalized only)
				case EF_A2R10G10B10_UNORM_PACK32:
				case EF_A2R10G10B10_SNORM_PACK32:
				// unsigned/signed rgb10a2
				case EF_A2B10G10R10_UNORM_PACK32:
				case EF_A2B10G10R10_SNORM_PACK32:
				case EF_A2B10G10R10_UINT_PACK32:
				case EF_A2B10G10R10_SINT_PACK32:
				case EF_A2B10G10R10_SSCALED_PACK32:
				case EF_A2B10G10R10_USCALED_PACK32:
				// GL_UNSIGNED_INT_10F_11F_11F_REV
				case EF_B10G11R11_UFLOAT_PACK32:
				// half float
				case EF_R16_SFLOAT:
				case EF_R16G16_SFLOAT:
				case EF_R16G16B16_SFLOAT:
				case EF_R16G16B16A16_SFLOAT:
				// float
				case EF_R32_SFLOAT:
				case EF_R32G32_SFLOAT:
				case EF_R32G32B32_SFLOAT:
				case EF_R32G32B32A32_SFLOAT:
				// double
				case EF_R64_SFLOAT:
				case EF_R64G64_SFLOAT:
				case EF_R64G64B64_SFLOAT:
				case EF_R64G64B64A64_SFLOAT:
					return true;
				default: return false;
            }
        }
        inline bool isColorRenderableFormat(asset::E_FORMAT _fmt) const override
        {
            using namespace asset;
            switch (_fmt)
            {
				case EF_A1R5G5B5_UNORM_PACK16:
				case EF_B5G6R5_UNORM_PACK16:
				case EF_R5G6B5_UNORM_PACK16:
				case EF_R4G4_UNORM_PACK8:
				case EF_R4G4B4A4_UNORM_PACK16:
				case EF_B4G4R4A4_UNORM_PACK16:
				case EF_R8_UNORM:
				case EF_R8_SNORM:
				case EF_R8_UINT:
				case EF_R8_SINT:
				case EF_R8G8_UNORM:
				case EF_R8G8_SNORM:
				case EF_R8G8_UINT:
				case EF_R8G8_SINT:
				case EF_R8G8B8_UNORM:
				case EF_R8G8B8_SNORM:
				case EF_R8G8B8_UINT:
				case EF_R8G8B8_SINT:
				case EF_R8G8B8_SRGB:
				case EF_R8G8B8A8_UNORM:
				case EF_R8G8B8A8_SNORM:
				case EF_R8G8B8A8_UINT:
				case EF_R8G8B8A8_SINT:
				case EF_R8G8B8A8_SRGB:
				case EF_A8B8G8R8_UNORM_PACK32:
				case EF_A8B8G8R8_SNORM_PACK32:
				case EF_A8B8G8R8_UINT_PACK32:
				case EF_A8B8G8R8_SINT_PACK32:
				case EF_A8B8G8R8_SRGB_PACK32:
				case EF_A2B10G10R10_UNORM_PACK32:
				case EF_A2B10G10R10_UINT_PACK32:
				case EF_R16_UNORM:
				case EF_R16_SNORM:
				case EF_R16_UINT:
				case EF_R16_SINT:
				case EF_R16_SFLOAT:
				case EF_R16G16_UNORM:
				case EF_R16G16_SNORM:
				case EF_R16G16_UINT:
				case EF_R16G16_SINT:
				case EF_R16G16_SFLOAT:
				case EF_R16G16B16_UNORM:
				case EF_R16G16B16_SNORM:
				case EF_R16G16B16_UINT:
				case EF_R16G16B16_SINT:
				case EF_R16G16B16_SFLOAT:
				case EF_R16G16B16A16_UNORM:
				case EF_R16G16B16A16_SNORM:
				case EF_R16G16B16A16_UINT:
				case EF_R16G16B16A16_SINT:
				case EF_R16G16B16A16_SFLOAT:
				case EF_R32_UINT:
				case EF_R32_SINT:
				case EF_R32_SFLOAT:
				case EF_R32G32_UINT:
				case EF_R32G32_SINT:
				case EF_R32G32_SFLOAT:
				case EF_R32G32B32_UINT:
				case EF_R32G32B32_SINT:
				case EF_R32G32B32_SFLOAT:
				case EF_R32G32B32A32_UINT:
				case EF_R32G32B32A32_SINT:
				case EF_R32G32B32A32_SFLOAT:
					return true;
				default:
				{
					GLint res = GL_FALSE;
					extGlGetInternalformativ(GL_TEXTURE_2D, getSizedOpenGLFormatFromOurFormat(_fmt), GL_COLOR_RENDERABLE, sizeof(res), &res);
					return res==GL_TRUE;
				}
            }
        }
        inline bool isAllowedImageStoreFormat(asset::E_FORMAT _fmt) const override
        {
            using namespace asset;
            switch (_fmt)
            {
				case EF_R32G32B32A32_SFLOAT:
				case EF_R16G16B16A16_SFLOAT:
				case EF_R32G32_SFLOAT:
				case EF_R16G16_SFLOAT:
				case EF_B10G11R11_UFLOAT_PACK32:
				case EF_R32_SFLOAT:
				case EF_R16_SFLOAT:
				case EF_R16G16B16A16_UNORM:
				case EF_A2B10G10R10_UNORM_PACK32:
				case EF_R8G8B8A8_UNORM:
				case EF_R16G16_UNORM:
				case EF_R8G8_UNORM:
				case EF_R16_UNORM:
				case EF_R8_UNORM:
				case EF_R16G16B16A16_SNORM:
				case EF_R8G8B8A8_SNORM:
				case EF_R16G16_SNORM:
				case EF_R8G8_SNORM:
				case EF_R16_SNORM:
				case EF_R32G32B32A32_UINT:
				case EF_R16G16B16A16_UINT:
				case EF_A2B10G10R10_UINT_PACK32:
				case EF_R8G8B8A8_UINT:
				case EF_R32G32_UINT:
				case EF_R16G16_UINT:
				case EF_R8G8_UINT:
				case EF_R32_UINT:
				case EF_R16_UINT:
				case EF_R8_UINT:
				case EF_R32G32B32A32_SINT:
				case EF_R16G16B16A16_SINT:
				case EF_R8G8B8A8_SINT:
				case EF_R32G32_SINT:
				case EF_R16G16_SINT:
				case EF_R8G8_SINT:
				case EF_R32_SINT:
				case EF_R16_SINT:
				case EF_R8_SINT:
					return true;
				default: return false;
            }
        }
        inline bool isAllowedTextureFormat(asset::E_FORMAT _fmt) const override
        {
            using namespace asset;
            // opengl spec section 8.5.1
            switch (_fmt)
            {
				// formats checked as "Req. tex"
				case EF_R8_UNORM:
				case EF_R8_SNORM:
				case EF_R16_UNORM:
				case EF_R16_SNORM:
				case EF_R8G8_UNORM:
				case EF_R8G8_SNORM:
				case EF_R16G16_UNORM:
				case EF_R16G16_SNORM:
				case EF_R8G8B8_UNORM:
				case EF_R8G8B8_SNORM:
				case EF_A1R5G5B5_UNORM_PACK16:
				case EF_R8G8B8A8_SRGB:
				case EF_A8B8G8R8_UNORM_PACK32:
				case EF_A8B8G8R8_SNORM_PACK32:
				case EF_A8B8G8R8_SRGB_PACK32:
				case EF_R16_SFLOAT:
				case EF_R16G16_SFLOAT:
				case EF_R16G16B16_SFLOAT:
				case EF_R16G16B16A16_SFLOAT:
				case EF_R32_SFLOAT:
				case EF_R32G32_SFLOAT:
				case EF_R32G32B32_SFLOAT:
				case EF_R32G32B32A32_SFLOAT:
				case EF_B10G11R11_UFLOAT_PACK32:
				case EF_E5B9G9R9_UFLOAT_PACK32:
				case EF_A2B10G10R10_UNORM_PACK32:
				case EF_A2B10G10R10_UINT_PACK32:
				case EF_R16G16B16A16_UNORM:
				case EF_R8_UINT:
				case EF_R8_SINT:
				case EF_R8G8_UINT:
				case EF_R8G8_SINT:
				case EF_R8G8B8_UINT:
				case EF_R8G8B8_SINT:
				case EF_R8G8B8A8_UNORM:
				case EF_R8G8B8A8_SNORM:
				case EF_R8G8B8A8_UINT:
				case EF_R8G8B8A8_SINT:
				case EF_B8G8R8A8_UINT:
				case EF_R16_UINT:
				case EF_R16_SINT:
				case EF_R16G16_UINT:
				case EF_R16G16_SINT:
				case EF_R16G16B16_UINT:
				case EF_R16G16B16_SINT:
				case EF_R16G16B16A16_UINT:
				case EF_R16G16B16A16_SINT:
				case EF_R32_UINT:
				case EF_R32_SINT:
				case EF_R32G32_UINT:
				case EF_R32G32_SINT:
				case EF_R32G32B32_UINT:
				case EF_R32G32B32_SINT:
				case EF_R32G32B32A32_UINT:
				case EF_R32G32B32A32_SINT:

				// depth/stencil/depth+stencil formats checked as "Req. format"
				case EF_D16_UNORM:
				case EF_X8_D24_UNORM_PACK32:
				case EF_D32_SFLOAT:
				case EF_D24_UNORM_S8_UINT:
				case EF_S8_UINT:

				// specific compressed formats
				case EF_BC6H_UFLOAT_BLOCK:
				case EF_BC6H_SFLOAT_BLOCK:
				case EF_BC7_UNORM_BLOCK:
				case EF_BC7_SRGB_BLOCK:
				case EF_ETC2_R8G8B8_UNORM_BLOCK:
				case EF_ETC2_R8G8B8_SRGB_BLOCK:
				case EF_ETC2_R8G8B8A1_UNORM_BLOCK:
				case EF_ETC2_R8G8B8A1_SRGB_BLOCK:
				case EF_ETC2_R8G8B8A8_UNORM_BLOCK:
				case EF_ETC2_R8G8B8A8_SRGB_BLOCK:
				case EF_EAC_R11_UNORM_BLOCK:
				case EF_EAC_R11_SNORM_BLOCK:
				case EF_EAC_R11G11_UNORM_BLOCK:
				case EF_EAC_R11G11_SNORM_BLOCK:
					return true;

				// astc
				case EF_ASTC_4x4_UNORM_BLOCK:
				case EF_ASTC_5x4_UNORM_BLOCK:
				case EF_ASTC_5x5_UNORM_BLOCK:
				case EF_ASTC_6x5_UNORM_BLOCK:
				case EF_ASTC_6x6_UNORM_BLOCK:
				case EF_ASTC_8x5_UNORM_BLOCK:
				case EF_ASTC_8x6_UNORM_BLOCK:
				case EF_ASTC_8x8_UNORM_BLOCK:
				case EF_ASTC_10x5_UNORM_BLOCK:
				case EF_ASTC_10x6_UNORM_BLOCK:
				case EF_ASTC_10x8_UNORM_BLOCK:
				case EF_ASTC_10x10_UNORM_BLOCK:
				case EF_ASTC_12x10_UNORM_BLOCK:
				case EF_ASTC_12x12_UNORM_BLOCK:
				case EF_ASTC_4x4_SRGB_BLOCK:
				case EF_ASTC_5x4_SRGB_BLOCK:
				case EF_ASTC_5x5_SRGB_BLOCK:
				case EF_ASTC_6x5_SRGB_BLOCK:
				case EF_ASTC_6x6_SRGB_BLOCK:
				case EF_ASTC_8x5_SRGB_BLOCK:
				case EF_ASTC_8x6_SRGB_BLOCK:
				case EF_ASTC_8x8_SRGB_BLOCK:
				case EF_ASTC_10x5_SRGB_BLOCK:
				case EF_ASTC_10x6_SRGB_BLOCK:
				case EF_ASTC_10x8_SRGB_BLOCK:
				case EF_ASTC_10x10_SRGB_BLOCK:
				case EF_ASTC_12x10_SRGB_BLOCK:
				case EF_ASTC_12x12_SRGB_BLOCK:
					return queryOpenGLFeature(IRR_KHR_texture_compression_astc_ldr);

				default: return false;
            }
        }
        inline bool isHardwareBlendableFormat(asset::E_FORMAT _fmt) const override
        {
            return isColorRenderableFormat(_fmt) && (asset::isNormalizedFormat(_fmt) || asset::isFloatingPointFormat(_fmt));
        }


        const core::smart_refctd_dynamic_array<std::string> getSupportedGLSLExtensions() const override;


        bool bindGraphicsPipeline(const video::IGPURenderpassIndependentPipeline* _gpipeline) override;

        bool bindComputePipeline(const video::IGPUComputePipeline* _cpipeline) override;

        bool bindDescriptorSets(E_PIPELINE_BIND_POINT _pipelineType, const IGPUPipelineLayout* _layout,
            uint32_t _first, uint32_t _count, const IGPUDescriptorSet* const* _descSets, core::smart_refctd_dynamic_array<uint32_t>* _dynamicOffsets) override;


		core::smart_refctd_ptr<IGPUBuffer> createGPUBufferOnDedMem(const IDriverMemoryBacked::SDriverMemoryRequirements& initialMreqs, const bool canModifySubData = false) override;

		core::smart_refctd_ptr<IGPUBufferView> createGPUBufferView(IGPUBuffer* _underlying, asset::E_FORMAT _fmt, size_t _offset, size_t _size) override;

		core::smart_refctd_ptr<IGPUSampler> createGPUSampler(const IGPUSampler::SParams& _params) override;

		core::smart_refctd_ptr<IGPUImage> createGPUImageOnDedMem(IGPUImage::SCreationParams&& params, const IDriverMemoryBacked::SDriverMemoryRequirements& initialMreqs) override;

        core::smart_refctd_ptr<IGPUImageView> createGPUImageView(IGPUImageView::SCreationParams&& params) override;

        core::smart_refctd_ptr<IGPUShader> createGPUShader(core::smart_refctd_ptr<const asset::ICPUShader>&& _cpushader) override;
        core::smart_refctd_ptr<IGPUSpecializedShader> createGPUSpecializedShader(const IGPUShader* _unspecialized, const asset::ISpecializedShader::SInfo& _specInfo) override;

        core::smart_refctd_ptr<IGPUDescriptorSetLayout> createGPUDescriptorSetLayout(const IGPUDescriptorSetLayout::SBinding* _begin, const IGPUDescriptorSetLayout::SBinding* _end) override;

        core::smart_refctd_ptr<IGPUPipelineLayout> createGPUPipelineLayout(
            const asset::SPushConstantRange* const _pcRangesBegin = nullptr, const asset::SPushConstantRange* const _pcRangesEnd = nullptr,
            core::smart_refctd_ptr<IGPUDescriptorSetLayout>&& _layout0 = nullptr, core::smart_refctd_ptr<IGPUDescriptorSetLayout>&& _layout1 = nullptr,
            core::smart_refctd_ptr<IGPUDescriptorSetLayout>&& _layout2 = nullptr, core::smart_refctd_ptr<IGPUDescriptorSetLayout>&& _layout3 = nullptr
        ) override;

        core::smart_refctd_ptr<IGPURenderpassIndependentPipeline> createGPURenderpassIndependentPipeline(
			IGPUPipelineCache* _pipelineCache,
            core::smart_refctd_ptr<IGPUPipelineLayout>&& _layout,
            IGPUSpecializedShader** _shadersBegin, IGPUSpecializedShader** _shadersEnd,
            const asset::SVertexInputParams& _vertexInputParams,
            const asset::SBlendParams& _blendParams,
            const asset::SPrimitiveAssemblyParams& _primAsmParams,
            const asset::SRasterizationParams& _rasterParams
        ) override;

        virtual core::smart_refctd_ptr<IGPUComputePipeline> createGPUComputePipeline(
			IGPUPipelineCache* _pipelineCache,
            core::smart_refctd_ptr<IGPUPipelineLayout>&& _layout,
            core::smart_refctd_ptr<IGPUSpecializedShader>&& _shader
        ) override;

		core::smart_refctd_ptr<IGPUPipelineCache> createGPUPipelineCache() override;

        core::smart_refctd_ptr<IGPUDescriptorSet> createGPUDescriptorSet(core::smart_refctd_ptr<const IGPUDescriptorSetLayout>&& _layout) override;

		void updateDescriptorSets(uint32_t descriptorWriteCount, const IGPUDescriptorSet::SWriteDescriptorSet* pDescriptorWrites, uint32_t descriptorCopyCount, const IGPUDescriptorSet::SCopyDescriptorSet* pDescriptorCopies) override;


		bool pushConstants(const IGPUPipelineLayout* _layout, uint32_t _stages, uint32_t _offset, uint32_t _size, const void* _values) override;

		bool dispatch(uint32_t _groupCountX, uint32_t _groupCountY, uint32_t _groupCountZ) override;
		bool dispatchIndirect(const IGPUBuffer* _indirectBuf, size_t _offset) override;


		//! generic version which overloads the unimplemented versions
		bool changeRenderContext(const SExposedVideoData& videoData, void* device) {return false;}

        bool initAuxContext();
        const SAuxContext* getThreadContext(const std::thread::id& tid=std::this_thread::get_id());
        bool deinitAuxContext();

        uint16_t retrieveDisplayRefreshRate() const override;


        void flushMappedMemoryRanges(uint32_t memoryRangeCount, const video::IDriverMemoryAllocation::MappedMemoryRange* pMemoryRanges) override;

        void invalidateMappedMemoryRanges(uint32_t memoryRangeCount, const video::IDriverMemoryAllocation::MappedMemoryRange* pMemoryRanges) override;

		void fillBuffer(IGPUBuffer* buffer, size_t offset, size_t length, uint32_t value) override;

        void copyBuffer(IGPUBuffer* readBuffer, IGPUBuffer* writeBuffer, size_t readOffset, size_t writeOffset, size_t length) override;

		void copyImage(IGPUImage* srcImage, IGPUImage* dstImage, uint32_t regionCount, const IGPUImage::SImageCopy* pRegions) override;

		void copyBufferToImage(IGPUBuffer* srcBuffer, IGPUImage* dstImage, uint32_t regionCount, const IGPUImage::SBufferCopy* pRegions) override;

		void copyImageToBuffer(IGPUImage* srcImage, IGPUBuffer* dstBuffer, uint32_t regionCount, const IGPUImage::SBufferCopy* pRegions) override;

		//! clears the zbuffer
		bool beginScene(bool backBuffer=true, bool zBuffer=true,
				SColor color=SColor(255,0,0,0),
				const SExposedVideoData& videoData=SExposedVideoData(),
				core::rect<int32_t>* sourceRect=0);

		//! presents the rendered scene on the screen, returns false if failed
		bool endScene();


		void beginQuery(IQueryObject* query);
		void endQuery(IQueryObject* query);

        IQueryObject* createPrimitivesGeneratedQuery() override;
        IQueryObject* createElapsedTimeQuery() override;
        IGPUTimestampQuery* createTimestampQuery() override;


        virtual void drawMeshBuffer(const video::IGPUMeshBuffer* mb);

		virtual void drawArraysIndirect(const asset::SBufferBinding<IGPUBuffer> _vtxBindings[IGPUMeshBuffer::MAX_ATTR_BUF_BINDING_COUNT],
                                        asset::E_PRIMITIVE_TOPOLOGY mode,
                                        const IGPUBuffer* indirectDrawBuff,
                                        size_t offset, size_t maxCount, size_t stride,
                                        const IGPUBuffer* countBuffer = nullptr, size_t countOffset = 0u) override;
		virtual void drawIndexedIndirect(const asset::SBufferBinding<IGPUBuffer> _vtxBindings[IGPUMeshBuffer::MAX_ATTR_BUF_BINDING_COUNT],
                                        asset::E_PRIMITIVE_TOPOLOGY mode,
                                        asset::E_INDEX_TYPE indexType, const IGPUBuffer* indexBuff,
                                        const IGPUBuffer* indirectDrawBuff,
                                        size_t offset, size_t maxCount, size_t stride,
                                        const IGPUBuffer* countBuffer = nullptr, size_t countOffset = 0u) override;


		//! queries the features of the driver, returns true if feature is available
		virtual bool queryFeature(const E_DRIVER_FEATURE& feature) const;

		//!
		virtual void issueGPUTextureBarrier() {COpenGLExtensionHandler::extGlTextureBarrier();}

        //! needs to be "deleted" since its not refcounted
        virtual core::smart_refctd_ptr<IDriverFence> placeFence(const bool& implicitFlushWaitSameThread=false) override final
        {
            return core::make_smart_refctd_ptr<COpenGLDriverFence>(implicitFlushWaitSameThread);
        }

		//! \return Returns the name of the video driver. Example: In case of the Direct3D8
		//! driver, it would return "Direct3D8.1".
		virtual const wchar_t* getName() const;

		//! sets a viewport
		virtual void setViewPort(const core::rect<int32_t>& area);

		//! Returns type of video driver
		inline E_DRIVER_TYPE getDriverType() const override { return EDT_OPENGL; }

		//! get color format of the current color buffer
		inline asset::E_FORMAT getColorFormat() const override { return ColorFormat; }


        virtual IFrameBuffer* addFrameBuffer();

        //! Remove
        virtual void removeFrameBuffer(IFrameBuffer* framebuf);

        virtual void removeAllFrameBuffers();

		virtual bool setRenderTarget(IFrameBuffer* frameBuffer, bool setNewViewport=true);

		virtual void blitRenderTargets(IFrameBuffer* in, IFrameBuffer* out,
                                        bool copyDepth=true, bool copyStencil=true,
										core::recti srcRect=core::recti(0,0,0,0),
										core::recti dstRect=core::recti(0,0,0,0),
										bool bilinearFilter=false);


    private:
        void clearColor_gatherAndOverrideState(SAuxContext* found, uint32_t _attIx, GLboolean* _rasterDiscard, GLboolean* _colorWmask);
        void clearColor_bringbackState(SAuxContext* found, uint32_t _attIx, GLboolean _rasterDiscard, const GLboolean* _colorWmask);

    public:
		//! Clears the ZBuffer.
		virtual void clearZBuffer(const float &depth=0.0);

		virtual void clearStencilBuffer(const int32_t &stencil);

		virtual void clearZStencilBuffers(const float &depth, const int32_t &stencil);

		virtual void clearColorBuffer(const E_FBO_ATTACHMENT_POINT &attachment, const int32_t* vals);
		virtual void clearColorBuffer(const E_FBO_ATTACHMENT_POINT &attachment, const uint32_t* vals);
		virtual void clearColorBuffer(const E_FBO_ATTACHMENT_POINT &attachment, const float* vals);

		virtual void clearScreen(const E_SCREEN_BUFFERS &buffer, const float* vals) override;
		virtual void clearScreen(const E_SCREEN_BUFFERS &buffer, const uint32_t* vals) override;

		//! Enable/disable a clipping plane.
		//! There are at least 6 clipping planes available for the user to set at will.
		//! \param index: The plane index. Must be between 0 and MaxUserClipPlanes.
		//! \param enable: If true, enable the clipping plane else disable it.
		virtual void enableClipPlane(uint32_t index, bool enable);

		//! Returns the graphics card vendor name.
		virtual std::string getVendorInfo() {return VendorName;}

		//!
		const size_t& getMaxConcurrentShaderInvocations() const {return maxConcurrentShaderInvocations;}

		//!
		const uint32_t& getMaxShaderComputeUnits() const {return maxShaderComputeUnits;}

		//!
		const size_t& getMaxShaderInvocationsPerALU() const {return maxALUShaderInvocations;}

#ifdef _IRR_COMPILE_WITH_OPENCL_
        const cl_device_id& getOpenCLAssociatedDevice() const {return clDevice;}
		const cl_context_properties* getOpenCLAssociatedContextProperties() const { return clProperties; }

        size_t getOpenCLAssociatedDeviceID() const {return clDeviceIx;}
        size_t getOpenCLAssociatedPlatformID() const {return clPlatformIx;}
#endif // _IRR_COMPILE_WITH_OPENCL_

        struct SAuxContext
        {
        //public:
            struct SPipelineCacheVal
            {
                GLuint GLname;
                core::smart_refctd_ptr<const COpenGLRenderpassIndependentPipeline> object;//so that it holds shaders which concerns hash
                uint64_t lastUsed;
            };

            _IRR_STATIC_INLINE_CONSTEXPR size_t maxVAOCacheSize = 0x1u<<10; //make this cache configurable
            _IRR_STATIC_INLINE_CONSTEXPR size_t maxPipelineCacheSize = 0x1u<<13;//8k

            SAuxContext() : threadId(std::thread::id()), ctx(NULL),
                            CurrentFBO(0), CurrentRendertargetSize(0,0)
            {
                VAOMap.reserve(maxVAOCacheSize);
            }

            void flushState_descriptors(E_PIPELINE_BIND_POINT _pbp, const COpenGLPipelineLayout* _currentLayout, const COpenGLPipelineLayout* _prevLayout);
            void flushStateGraphics(uint32_t stateBits);
            void flushStateCompute(uint32_t stateBits);

            SOpenGLState currentState;
            SOpenGLState nextState;
            struct {
                SOpenGLState::SDescSetBnd descSets[IGPUPipelineLayout::DESCRIPTOR_SET_COUNT];
            } effectivelyBoundDescriptors;

            //push constants are tracked outside of next/currentState because there can be multiple pushConstants() calls and each of them kinda depends on the pervious one (layout compatibility)
			pipeline_for_bindpoint_t<EPBP_COMPUTE>::PushConstantsState pushConstantsStateCompute;
			pipeline_for_bindpoint_t<EPBP_GRAPHICS>::PushConstantsState pushConstantsStateGraphics;
			template<E_PIPELINE_BIND_POINT PBP>
			typename pipeline_for_bindpoint_t<PBP>::PushConstantsState* pushConstantsState()
			{
				if constexpr(PBP==EPBP_COMPUTE)
					return &pushConstantsStateCompute;
				else if (PBP== EPBP_GRAPHICS)
					return &pushConstantsStateGraphics;
				else
					return nullptr;
			}

        //private:
            std::thread::id threadId;
            uint8_t ID; //index in array of contexts, just to be easier in use
            #ifdef _IRR_WINDOWS_API_
                HGLRC ctx;
            #endif
            #ifdef _IRR_COMPILE_WITH_X11_DEVICE_
                GLXContext ctx;
                GLXPbuffer pbuff;
            #endif
            #ifdef _IRR_COMPILE_WITH_OSX_DEVICE_
                AppleMakesAUselessOSWhichHoldsBackTheGamingIndustryAndSabotagesOpenStandards ctx;
            #endif

            //! FBOs
            core::vector<IFrameBuffer*>  FrameBuffers;
            COpenGLFrameBuffer*         CurrentFBO;
            core::dimension2d<uint32_t> CurrentRendertargetSize; // @Crisspl TODO: Fold this into SOpenGLState, as well as the Vulkan dynamic state (scissor rect, viewport, etc.)

            //!
            core::vector<SOpenGLState::HashVAOPair> VAOMap;
            struct HashPipelinePair
            {
                SOpenGLState::SGraphicsPipelineHash first;
                SPipelineCacheVal second;

                inline bool operator<(const HashPipelinePair& rhs) const { return first < rhs.first; }
            };
            core::vector<HashPipelinePair> GraphicsPipelineMap;

            GLuint createGraphicsPipeline(const SOpenGLState::SGraphicsPipelineHash& _hash);

            void updateNextState_pipelineAndRaster(const IGPURenderpassIndependentPipeline* _pipeline);
            //! Must be called AFTER updateNextState_pipelineAndRaster() if pipeline and raster params have to be modified at all in this pass
            void updateNextState_vertexInput(
                const asset::SBufferBinding<IGPUBuffer> _vtxBindings[IGPUMeshBuffer::MAX_ATTR_BUF_BINDING_COUNT],
                const IGPUBuffer* _indexBuffer,
                const IGPUBuffer* _indirectDrawBuffer,
                const IGPUBuffer* _paramBuffer
            );
			template<E_PIPELINE_BIND_POINT PBP>
            inline void pushConstants(const COpenGLPipelineLayout* _layout, uint32_t _stages, uint32_t _offset, uint32_t _size, const void* _values)
			{
				//validation is done in CNullDriver::pushConstants()
				//if arguments were invalid (dont comply Valid Usage section of vkCmdPushConstants docs), execution should not even get to this point

				if (pushConstantsState<PBP>()->layout && !pushConstantsState<PBP>()->layout->isCompatibleForPushConstants(_layout))
				{
				//#ifdef _IRR_DEBUG
					constexpr size_t toFill = IGPUMeshBuffer::MAX_PUSH_CONSTANT_BYTESIZE / sizeof(uint64_t);
					constexpr size_t bytesLeft = IGPUMeshBuffer::MAX_PUSH_CONSTANT_BYTESIZE - (toFill * sizeof(uint64_t));
					constexpr uint64_t pattern = 0xdeadbeefDEADBEEFull;
					std::fill(reinterpret_cast<uint64_t*>(pushConstantsState<PBP>()->data), reinterpret_cast<uint64_t*>(pushConstantsState<PBP>()->data)+toFill, pattern);
					if constexpr (bytesLeft > 0ull)
						memcpy(reinterpret_cast<uint64_t*>(pushConstantsState<PBP>()->data) + toFill, &pattern, bytesLeft);
				//#endif

					_stages |= IGPUSpecializedShader::ESS_ALL;
				}
				pushConstantsState<PBP>()->incrementStamps(_stages);

				pushConstantsState<PBP>()->layout = core::smart_refctd_ptr<const COpenGLPipelineLayout>(_layout);
				memcpy(pushConstantsState<PBP>()->data+_offset, _values, _size);
			}

            inline size_t getVAOCacheSize() const
            {
                return VAOMap.size();
            }

            inline void freeUpVAOCache(bool exitOnFirstDelete)
            {
                for(auto it = VAOMap.begin(); VAOMap.size()>maxVAOCacheSize&&it!=VAOMap.end();)
                {
                    if (it->first==currentState.vertexInputParams.vao.first)
                        continue;

                    if (CNullDriver::ReallocationCounter-it->second.lastUsed>1000) //maybe make this configurable
                    {
                        COpenGLExtensionHandler::extGlDeleteVertexArrays(1, &it->second.GLname);
                        it = VAOMap.erase(it);
                        if (exitOnFirstDelete)
                            return;
                    }
                    else
                        it++;
                }
            }
            //TODO DRY
            inline void freeUpGraphicsPipelineCache(bool exitOnFirstDelete)
            {
                for (auto it = GraphicsPipelineMap.begin(); GraphicsPipelineMap.size() > maxPipelineCacheSize&&it != GraphicsPipelineMap.end();)
                {
                    if (it->first == currentState.pipeline.graphics.usedShadersHash)
                        continue;

                    if (CNullDriver::ReallocationCounter-it->second.lastUsed > 1000) //maybe make this configurable
                    {
                        COpenGLExtensionHandler::extGlDeleteProgramPipelines(1, &it->second.GLname);
                        it = GraphicsPipelineMap.erase(it);
                        if (exitOnFirstDelete)
                            return;
                    }
                    else
                        it++;
                }
            }
        };


		//!
		virtual uint32_t getRequiredUBOAlignment() const {return COpenGLExtensionHandler::reqUBOAlignment;}

		//!
		virtual uint32_t getRequiredSSBOAlignment() const {return COpenGLExtensionHandler::reqSSBOAlignment;}

		//!
		virtual uint32_t getRequiredTBOAlignment() const {return COpenGLExtensionHandler::reqTBOAlignment;}

		//!
		virtual uint32_t getMinimumMemoryMapAlignment() const {return COpenGLExtensionHandler::minMemoryMapAlignment;}

        //!
        virtual uint32_t getMaxComputeWorkGroupSize(uint32_t _dimension) const { return COpenGLExtensionHandler::MaxComputeWGSize[_dimension]; }

        //!
        virtual uint64_t getMaxUBOSize() const override { return COpenGLExtensionHandler::maxUBOSize; }

        //!
        virtual uint64_t getMaxSSBOSize() const override { return COpenGLExtensionHandler::maxSSBOSize; }

        //!
        virtual uint64_t getMaxTBOSizeInTexels() const override { return COpenGLExtensionHandler::maxTBOSizeInTexels; }

        //!
        virtual uint64_t getMaxBufferSize() const override { return COpenGLExtensionHandler::maxBufferSize; }

        uint32_t getMaxUBOBindings() const override { return COpenGLExtensionHandler::maxUBOBindings; }
        uint32_t getMaxSSBOBindings() const override { return COpenGLExtensionHandler::maxSSBOBindings; }
        uint32_t getMaxTextureBindings() const override { return COpenGLExtensionHandler::maxTextureBindings; }
        uint32_t getMaxTextureBindingsCompute() const override { return COpenGLExtensionHandler::maxTextureBindingsCompute; }
        uint32_t getMaxImageBindings() const override { return COpenGLExtensionHandler::maxImageBindings; }

    private:
        SAuxContext* getThreadContext_helper(const bool& alreadyLockedMutex, const std::thread::id& tid = std::this_thread::get_id());

        void cleanUpContextBeforeDelete();


        //COpenGLDriver::CGPUObjectFromAssetConverter
        class CGPUObjectFromAssetConverter;
        friend class CGPUObjectFromAssetConverter;

        using PipelineMapKeyT = std::pair<std::array<core::smart_refctd_ptr<IGPUSpecializedShader>, 5u>, std::thread::id>;
        core::map<PipelineMapKeyT, GLuint> Pipelines;

        bool runningInRenderDoc;

		// returns the current size of the screen or rendertarget
		virtual const core::dimension2d<uint32_t>& getCurrentRenderTargetSize();

		void createMaterialRenderers();

		core::stringw Name;

		std::string VendorName;

		//! Color buffer format
		asset::E_FORMAT ColorFormat; //FIXME

        mutable core::smart_refctd_dynamic_array<std::string> m_supportedGLSLExtsNames;

		#ifdef _IRR_WINDOWS_API_
			HDC HDc; // Private GDI Device Context
			HWND Window;
		#ifdef _IRR_COMPILE_WITH_WINDOWS_DEVICE_
			CIrrDeviceWin32 *Win32Device;
		#endif
		#endif
		#ifdef _IRR_COMPILE_WITH_X11_DEVICE_
			GLXDrawable Drawable;
			Display* X11Display;
			CIrrDeviceLinux *X11Device;
		#endif
		#ifdef _IRR_COMPILE_WITH_OSX_DEVICE_
			CIrrDeviceMacOSX *OSXDevice;
		#endif
		#ifdef _IRR_COMPILE_WITH_SDL_DEVICE_
			CIrrDeviceSDL *SDLDevice;
		#endif

        size_t maxALUShaderInvocations;
        size_t maxConcurrentShaderInvocations;
        uint32_t maxShaderComputeUnits;
#ifdef _IRR_COMPILE_WITH_OPENCL_
        cl_device_id clDevice;
		cl_context_properties clProperties[7];
        size_t clPlatformIx, clDeviceIx;
#endif // _IRR_COMPILE_WITH_OPENCL_

        std::mutex glContextMutex;
		SAuxContext* AuxContexts;
        core::smart_refctd_ptr<const asset::IGLSLCompiler> GLSLCompiler;

		E_DEVICE_TYPE DeviceType;
	};

} // end namespace video
} // end namespace irr


#endif // _IRR_COMPILE_WITH_OPENGL_
#endif

