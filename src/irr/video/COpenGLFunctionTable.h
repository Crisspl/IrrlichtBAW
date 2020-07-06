#ifndef __C_OPEN_GL_FUNCTION_TABLE_H_INCLUDED__
#define __C_OPEN_GL_FUNCTION_TABLE_H_INCLUDED__

#define GL_GLEXT_PROTOTYPES


#ifdef _IRR_WINDOWS_API_
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <wingdi.h>
#endif
#if defined(_IRR_COMPILE_WITH_SDL_DEVICE_) && !(defined(_IRR_COMPILE_WITH_X11_DEVICE_)||defined(_IRR_WINDOWS_API_))
#include <SDL/SDL_video.h>
#define GL_GLEXT_LEGACY 1
#include <SDL/SDL_opengl.h>
#undef GL_GLEXT_LEGACY
#else
#define GL_GLEXT_LEGACY 1
#include "GL/gl.h"
//missing something of sort "#include <GL/glx.h>"
#undef GL_GLEXT_LEGACY
#endif

#include "irr/core/string/UniqueStringLiteralType.h"
#include "GL/glext.h"
#include "GL/glxext.h"
#include "GL/wglext.h"
#include "irr/video/IGPUImageView.h"
#include "SDL2/include/SDL.h"
#include <irr\system\DynamicFunctionCaller.h>


//maybe glx and wgl too
//#include "../../3rdparty/GL/glext.h"

//#ifdef __gl_glext_h_
//#error chuj
//#endif
namespace irr {
	namespace video {

		class COpenGLFunctionTable
		{
		public:


			class OpenGLFunctionLoader final : system::FuncPtrLoader
			{
			public:
				inline bool isLibraryLoaded() override final
				{
					return true;
				}

				inline void* loadFuncPtr(const char* funcname) override final
				{
					return SDL_GL_GetProcAddress(funcname);
				}
			};

			IRR_SYSTEM_DECLARE_DYNAMIC_FUNCTION_CALLER_CLASS(GLsync, OpenGLFunctionLoader
				, glFenceSync
				, glDeleteSync
				, glClientWaitSync
				, glWaitSync
				, glTextureBarrier
				, glTextureBarrierNV
				, glMemoryBarrier
			);

			IRR_SYSTEM_DECLARE_DYNAMIC_FUNCTION_CALLER_CLASS(GLframeBuffer, OpenGLFunctionLoader
				, glBlitNamedFramebuffer
				, glBlitFramebuffer
				, glDeleteFramebuffers
				, glCreateFramebuffers
				, glGenFramebuffers
				, glBindFramebuffer
				, glCheckFramebufferStatus
				, glCheckNamedFramebufferStatus
				, glCheckNamedFramebufferStatusEXT
				, glFramebufferTexture
				, glNamedFramebufferTexture
				, glNamedFramebufferTextureEXT
				, glFramebufferTextureLayer
				, glNamedFramebufferTextureLayer
				, glNamedFramebufferTextureLayerEXT
				, glFramebufferTexture1D
				, glFramebufferTexture2D
				, glFramebufferTexture3D
				, glNamedFramebufferTexture2DEXT
				, glNamedFramebufferDrawBuffers
				, glFramebufferDrawBuffersEXT
				, glNamedFramebufferDrawBuffer
				, glFramebufferDrawBufferEXT
				, glNamedFramebufferReadBuffer
				, glFramebufferReadBufferEXT
				, glClearNamedFramebufferiv
				, glClearNamedFramebufferuiv
				, glClearNamedFramebufferfv
				, glClearNamedFramebufferfi
				, glClearBufferiv
				, glClearBufferuiv
				, glClearBufferfv
				, glClearBufferfi
				, glDrawBuffers
				, glDrawBuffer
				, glReadBuffer
			);
			IRR_SYSTEM_DECLARE_DYNAMIC_FUNCTION_CALLER_CLASS(GLbuffer, OpenGLFunctionLoader
				, glBindBufferBase
				, glBindBufferRange
				, glBindBuffersBase
				, glBindBuffersRange
				, glGenBuffers
				, glCreateBuffers
				, glBindBuffer
				, glDeleteBuffers
				, glBufferStorage
				, glNamedBufferStorage
				, glNamedBufferStorageEXT
				, glBufferSubData
				, glNamedBufferSubData
				, glNamedBufferSubDataEXT
				, glGetBufferSubData
				, glGetNamedBufferSubData
				, glGetNamedBufferSubDataEXT
				, glMapBuffer
				, glMapNamedBuffer
				, glMapNamedBufferEXT
				, glMapBufferRange
				, glMapNamedBufferRange
				, glMapNamedBufferRangeEXT
				, glFlushMappedBufferRange
				, glFlushMappedNamedBufferRange
				, glFlushMappedNamedBufferRangeEXT
				, glUnmapBuffer
				, glUnmapNamedBuffer
				, glUnmapNamedBufferEXT
				, glClearBufferData
				, glClearNamedBufferData
				, glClearNamedBufferDataEXT
				, glClearBufferSubData
				, glClearNamedBufferSubData
				, glClearNamedBufferSubDataEXT
				, glCopyBufferSubData
				, glCopyNamedBufferSubData
				, glNamedCopyBufferSubDataEXT
				, glIsBuffer
				, glGetNamedBufferParameteri64v
				, glGetBufferParameteri64v
				, glGetNamedBufferParameteriv
				, glGetNamedBufferParameterivEXT
				, glGetBufferParameteriv
			);
			IRR_SYSTEM_DECLARE_DYNAMIC_FUNCTION_CALLER_CLASS(GLtexture, OpenGLFunctionLoader
				, glActiveTexture
				, glBindTextures
				, glCreateTextures
				, glTexStorage1D
				, glTexStorage2D
				, glTexStorage3D
				, glTexStorage2DMultisample
				, glTexStorage3DMultisample
				, glTexBuffer
				, glTexBufferRange
				, glTextureStorage1D
				, glTextureStorage2D
				, glTextureStorage3D
				, glTextureStorage2DMultisample
				, glTextureStorage3DMultisample
				, glTextureBuffer
				, glTextureBufferRange
				, glTextureView
				, glTextureStorage1DEXT
				, glTextureStorage2DEXT
				, glTextureStorage3DEXT
				, glTextureBufferEXT
				, glTextureBufferRangeEXT
				, glTextureStorage2DMultisampleEXT
				, glTextureStorage3DMultisampleEXT
				, glGetTextureSubImage
				, glGetCompressedTextureSubImage
				, glGetTextureImage
				, glGetTextureImageEXT
				, glGetCompressedTextureImage
				, glGetCompressedTextureImageEXT
				, glGetCompressedTexImage
				, glTexSubImage3D
				, glMultiTexSubImage1DEXT
				, glMultiTexSubImage2DEXT
				, glMultiTexSubImage3DEXT
				, glTextureSubImage1D
				, glTextureSubImage2D
				, glTextureSubImage3D
				, glTextureSubImage1DEXT
				, glTextureSubImage2DEXT
				, glTextureSubImage3DEXT
				, glCompressedTexSubImage1D
				, glCompressedTexSubImage2D
				, glCompressedTexSubImage3D
				, glCompressedTextureSubImage1D
				, glCompressedTextureSubImage2D
				, glCompressedTextureSubImage3D
				, glCompressedTextureSubImage1DEXT
				, glCompressedTextureSubImage2DEXT
				, glCompressedTextureSubImage3DEXT
				, glCopyImageSubData
				, glTextureParameterIuiv
				, glTextureParameterIuivEXT
				, glTexParameterIuiv
				, glGenerateMipmap
				, glGenerateTextureMipmap
				, glGenerateTextureMipmapEXT
				, glClampColor
				, glCreateSamplers
				, glGenSamplers
				, glDeleteSamplers
				, glBindSampler
				, glBindSamplers
				, glSamplerParameteri
				, glSamplerParameterf
				, glSamplerParameterfv
				, glBindImageTexture
				, glBindImageTextures
				, glGetTextureHandleARB
				, glGetTextureSamplerHandleARB
				//, glMakeTextureHandleResidentAR
				, glMakeTextureHandleNonResidentARB
				, glGetImageHandleARB
				, glMakeImageHandleResidentARB
				, glMakeImageHandleNonResidentARB
				, glIsTextureHandleResidentARB
				, glIsImageHandleResidentARB
				, glGetTextureHandleNV
				, glGetTextureSamplerHandleNV
				//, glMakeTextureHandleResidentAR
				, glMakeTextureHandleNonResidentNV
				, glGetImageHandleNV
				, glMakeImageHandleResidentNV
				, glMakeImageHandleNonResidentNV
				, glIsTextureHandleResidentNV
				, glIsImageHandleResidentNV
				, glMakeTextureHandleResidentARB
				, glMakeTextureHandleResidentNV
			);
			IRR_SYSTEM_DECLARE_DYNAMIC_FUNCTION_CALLER_CLASS(GLshader, OpenGLFunctionLoader
				, glCreateShader
				, glCreateShaderProgramv
				, glCreateProgramPipelines
				, glDeleteProgramPipelines
				, glUseProgramStages
				, glShaderSource
				, glCompileShader
				, glCreateProgram
				, glAttachShader
				, glTransformFeedbackVaryings
				, glLinkProgram
				, glUseProgram
				, glDeleteProgram
				, glDeleteShader
				, glGetAttachedShaders
				, glGetShaderInfoLog
				, glGetProgramInfoLog
				, glGetShaderiv
				, glGetProgramiv
				, glGetUniformLocation
				, glProgramUniform1fv
				, glProgramUniform2fv
				, glProgramUniform3fv
				, glProgramUniform4fv
				, glProgramUniform1iv
				, glProgramUniform2iv
				, glProgramUniform3iv
				, glProgramUniform4iv
				, glProgramUniform1uiv
				, glProgramUniform2uiv
				, glProgramUniform3uiv
				, glProgramUniform4uiv
				, glProgramUniformMatrix2fv
				, glProgramUniformMatrix3fv
				, glProgramUniformMatrix4fv
				, glProgramUniformMatrix2x3fv
				, glProgramUniformMatrix3x2fv
				, glProgramUniformMatrix4x2fv
				, glProgramUniformMatrix2x4fv
				, glProgramUniformMatrix3x4fv
				, glProgramUniformMatrix4x3fv
				, glGetActiveUniform
				, glBindProgramPipeline
				, glGetProgramBinary
				, glProgramBinary
				, glProgramParameteri
				, glPatchParameterfv
				, glPatchParameteri
				, glDepthMask
				, glPixelStorei
				, glPolygonOffset
				, glPointSize
				, glLineWidth
				, glDepthFunc
				, glHint
				, glPrimitiveRestartIndex
				, glProvokingVertex

			);
			IRR_SYSTEM_DECLARE_DYNAMIC_FUNCTION_CALLER_CLASS(GLfragment, OpenGLFunctionLoader
				, glPointParameterf
				, glPointParameterfv
				, glBlendEquationEXT
				, glBlendEquation
				, glBlendColor
				, glDepthRangeIndexed
				, glViewportIndexedfv
				, glScissorIndexedv
				, glSampleCoverage
				, glSampleMaski
				, glMinSampleShading
				, glBlendEquationSeparatei
				, glBlendFuncSeparatei
				, glColorMaski
				, glStencilFuncSeparate
				, glStencilOpSeparate
				, glStencilMaskSeparate
				, glBlendFuncIndexedAMD
				, glBlendFunciARB
				, glBlendEquationIndexedAMD
				, glBlendEquationiARB
				, glBlendFuncSeparate
			);
			IRR_SYSTEM_DECLARE_DYNAMIC_FUNCTION_CALLER_CLASS(GLvertex, OpenGLFunctionLoader
				, glGenVertexArrays
				, glCreateVertexArrays
				, glDeleteVertexArrays
				, glBindVertexArray
				, glVertexArrayElementBuffer
				, glBindVertexBuffer
				, glVertexArrayVertexBuffer
				, glVertexArrayBindVertexBufferEXT
				, glVertexAttribBinding
				, glVertexArrayAttribBinding
				, glVertexArrayVertexAttribBindingEXT
				, glEnableVertexAttribArray
				, glEnableVertexArrayAttrib
				, glEnableVertexArrayAttribEXT
				, glDisableVertexAttribArray
				, glDisableVertexArrayAttrib
				, glDisableVertexArrayAttribEXT
				, glVertexAttribFormat
				, glVertexAttribIFormat
				, glVertexAttribLFormat
				, glVertexArrayAttribFormat
				, glVertexArrayAttribIFormat
				, glVertexArrayAttribLFormat
				, glVertexArrayVertexAttribFormatEXT
				, glVertexArrayVertexAttribIFormatEXT
				, glVertexArrayVertexAttribLFormatEXT
				, glVertexArrayBindingDivisor
				, glVertexArrayVertexBindingDivisorEXT
				, glVertexBindingDivisor
			);
			IRR_SYSTEM_DECLARE_DYNAMIC_FUNCTION_CALLER_CLASS(GLdrawing, OpenGLFunctionLoader
				, glDrawArraysInstanced
				, glDrawArraysInstancedBaseInstance
				, glDrawElementsInstancedBaseVertex
				, glDrawElementsInstancedBaseVertexBaseInstance
				, glDrawTransformFeedback
				, glDrawTransformFeedbackInstanced
				, glDrawTransformFeedbackStream
				, glDrawTransformFeedbackStreamInstanced
				, glDrawArraysIndirect
				, glDrawElementsIndirect
				, glMultiDrawArraysIndirect
				, glMultiDrawElementsIndirect
				, glMultiDrawArraysIndirectCount
				, glMultiDrawElementsIndirectCount
				, glMultiDrawArraysIndirectCountARB
				, glMultiDrawElementsIndirectCountARB
			);
			IRR_SYSTEM_DECLARE_DYNAMIC_FUNCTION_CALLER_CLASS(GLtransformFeedback, OpenGLFunctionLoader
				, glCreateTransformFeedbacks
				, glGenTransformFeedbacks
				, glDeleteTransformFeedbacks
				, glBindTransformFeedback
				, glBeginTransformFeedback
				, glPauseTransformFeedback
				, glResumeTransformFeedback
				, glEndTransformFeedback
				, glTransformFeedbackBufferBase
				, glTransformFeedbackBufferRange
			);
			IRR_SYSTEM_DECLARE_DYNAMIC_FUNCTION_CALLER_CLASS(GLquery, OpenGLFunctionLoader
				, glCreateQueries
				, glGenQueries
				, glDeleteQueries
				, glIsQuery
				, glBeginQuery
				, glEndQuery
				, glBeginQueryIndexed
				, glEndQueryIndexed
				, glGetQueryiv
				, glGetQueryObjectuiv
				, glGetQueryObjectui64v
				, glGetQueryBufferObjectuiv
				, glGetQueryBufferObjectui64v
				, glQueryCounter
				, glBeginConditionalRender
				, glEndConditionalRender
			);
			IRR_SYSTEM_DECLARE_DYNAMIC_FUNCTION_CALLER_CLASS(GLdebug, OpenGLFunctionLoader
				, glDebugMessageControl
				, glDebugMessageControlARB
				, glDebugMessageCallback
				, glDebugMessageCallbackARB
			);
			IRR_SYSTEM_DECLARE_DYNAMIC_FUNCTION_CALLER_CLASS(GLOS, OpenGLFunctionLoader
#if defined(WGL_EXT_swap_control) && !defined(_IRR_COMPILE_WITH_SDL_DEVICE_)
				, wglSwapIntervalEXT
#endif
#if defined(GLX_SGI_swap_control) && !defined(_IRR_COMPILE_WITH_SDL_DEVICE_)
				, glXSwapIntervalSGI
#endif
#if defined(GLX_EXT_swap_control) && !defined(_IRR_COMPILE_WITH_SDL_DEVICE_)
				, glXSwapIntervalEXT
#endif
#if defined(GLX_MESA_swap_control) && !defined(_IRR_COMPILE_WITH_SDL_DEVICE_)
				, glXSwapIntervalMESA
#endif
				, glxWin
				, glXMakeContextCurrent
				, glXMakeCurrent
				, glXDestroyContext
				, glXDestroyWindow
				, glXQueryExtension
				, glXChooseFBConfig
				, glXGetVisualFromFBConfig
				, glXGetFBConfigAttrib
			);
			IRR_SYSTEM_DECLARE_DYNAMIC_FUNCTION_CALLER_CLASS(GLgeneral, OpenGLFunctionLoader
				, glEnablei
				, glDisablei
				, glEnable
				, glDisable
				, glIsEnabledi
				//glGet functions
				, glGetIntegerv
				, glGetBooleanv
				, glGetDoublev
				, glGetFloatv
				, glGetInteger64v
				, glGetFloati_v
				, glGetIntegeri_v
				, glGetBooleani_v
				, glGetInternalformativ
				, glGetInternalformati64v
				, glLogicOp
				, glFlush
			);
			IRR_SYSTEM_DECLARE_DYNAMIC_FUNCTION_CALLER_CLASS(GLcompute, OpenGLFunctionLoader
				, glDispatchCompute
				, glDispatchComputeIndirect
			);

			static GLsync glSync;
			static GLframeBuffer glFrameBuffer;
			static GLbuffer glBuffer;
			static GLtexture glTexture;
			static GLshader glShader;
			static GLfragment glFragment;
			static GLvertex glVertex;
			static GLdrawing glDrawing;
			static GLtransformFeedback glTransformFeedback;
			static GLquery glQuery;
			static GLdebug glDebug;
			static GLOS glOS;
			static GLgeneral glGeneral;
			static GLcompute glCompute;



			void extGlBindTextures(const GLuint& first, const GLsizei& count, const GLuint* textures, const GLenum* targets);
			void extGlCreateTextures(GLenum target, GLsizei n, GLuint* textures);
			void extGlTextureBuffer(GLuint texture, GLenum internalformat, GLuint buffer);
			void extGlTextureBufferRange(GLuint texture, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizei length);
			void extGlTextureStorage1D(GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width);
			void extGlTextureStorage2D(GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
			void extGlTextureStorage3D(GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
			void extGlTextureStorage2DMultisample(GLuint texture, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
			void extGlTextureStorage3DMultisample(GLuint texture, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
			void extGlGetTextureSubImage(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLsizei bufSize, void* pixels);
			void extGlGetTextureImage(GLuint texture, GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSizeHint, void* pixels);
			void extGlGetCompressedTextureImage(GLuint texture, GLenum target, GLint level, GLsizei bufSizeHint, void* pixels);
			void extGlTextureSubImage1D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
			void extGlTextureSubImage2D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
			void extGlTextureSubImage3D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
			void extGlCompressedTextureSubImage1D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void* data);
			void extGlCompressedTextureSubImage2D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data);
			void extGlCompressedTextureSubImage3D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data);
			void extGlGenerateTextureMipmap(GLuint texture, GLenum target);
			void extGlTextureParameterIuiv(GLuint texture, GLenum target, GLenum pname, const GLuint* params);
			void extGlCreateSamplers(GLsizei n, GLuint* samplers);
			void extGlBindSamplers(const GLuint& first, const GLsizei& count, const GLuint* samplers);
			void extGlBindImageTextures(GLuint first, GLsizei count, const GLuint* textures, const GLenum* formats);
			GLuint64 extGlGetTextureHandle(GLuint texture);
			GLuint64 extGlGetTextureSamplerHandle(GLuint texture, GLuint sampler);
			void extGlMakeTextureHandleResident(GLuint64 handle);
			void extGlMakeTextureHandleNonResident(GLuint64 handle);
			GLuint64 extGlGetImageHandle(GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum format);
			void extGlMakeImageHandleResident(GLuint64 handle, GLenum access);
			void extGlMakeImageHandleNonResident(GLuint64 handle);
			GLboolean extGlIsTextureHandleResident(GLuint64 handle);
			GLboolean extGlIsImageHandleResident(GLuint64 handle);
			void extGlCreateFramebuffers(GLsizei n, GLuint* framebuffers);
			GLenum extGlCheckNamedFramebufferStatus(GLuint framebuffer, GLenum target);
			void extGlNamedFramebufferTexture(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level);
			void extGlNamedFramebufferTextureLayer(GLuint framebuffer, GLenum attachment, GLuint texture, GLenum textureType, GLint level, GLint layer);
			void extGlBlitNamedFramebuffer(GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
			void extGlActiveStencilFace(GLenum face);
			void extGlNamedFramebufferReadBuffer(GLuint framebuffer, GLenum mode);
			void extGlNamedFramebufferDrawBuffer(GLuint framebuffer, GLenum buf);
			void extGlNamedFramebufferDrawBuffers(GLuint framebuffer, GLsizei n, const GLenum* bufs);
			void extGlClearNamedFramebufferiv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint* value);
			void extGlClearNamedFramebufferuiv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLuint* value);
			void extGlClearNamedFramebufferfv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat* value);
			void extGlClearNamedFramebufferfi(GLuint framebuffer, GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
			void extGlCreateBuffers(GLsizei n, GLuint* buffers);
			void extGlBindBuffersBase(const GLenum& target, const GLuint& first, const GLsizei& count, const GLuint* buffers);
			void extGlBindBuffersRange(const GLenum& target, const GLuint& first, const GLsizei& count, const GLuint* buffers, const GLintptr* offsets, const GLsizeiptr* sizes);
			void extGlNamedBufferStorage(GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags);
			void extGlNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data);
			void extGlGetNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, void* data);
			void* extGlMapNamedBuffer(GLuint buffer, GLbitfield access);
			void* extGlMapNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access);
			void extGlFlushMappedNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length);
			GLboolean extGlUnmapNamedBuffer(GLuint buffer);
			void extGlClearNamedBufferData(GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const void* data);
			void extGlClearNamedBufferSubData(GLuint buffer, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void* data);
			void extGlCopyNamedBufferSubData(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
			GLboolean extGlIsBuffer(GLuint buffer);
			void extGlGetNamedBufferParameteriv(const GLuint& buffer, const GLenum& value, GLint* data);
			void extGlGetNamedBufferParameteri64v(const GLuint& buffer, const GLenum& value, GLint64* data);
			void extGlCreateVertexArrays(GLsizei n, GLuint* arrays);
			void extGlVertexArrayElementBuffer(GLuint vaobj, GLuint buffer);
			void extGlVertexArrayVertexBuffer(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
			void extGlVertexArrayAttribBinding(GLuint vaobj, GLuint attribindex, GLuint bindingindex);
			void extGlEnableVertexArrayAttrib(GLuint vaobj, GLuint index);
			void extGlDisableVertexArrayAttrib(GLuint vaobj, GLuint index);
			void extGlVertexArrayAttribFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
			void extGlVertexArrayAttribIFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
			void extGlVertexArrayAttribLFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
			void extGlVertexArrayBindingDivisor(GLuint vaobj, GLuint bindingindex, GLuint divisor);
			void extGlCreateTransformFeedbacks(GLsizei n, GLuint* ids);
			void extGlTransformFeedbackBufferBase(GLuint xfb, GLuint index, GLuint buffer);
			void extGlTransformFeedbackBufferRange(GLuint xfb, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
			void extGlCreateQueries(GLenum target, GLsizei n, GLuint* ids);
			void extGlGetQueryBufferObjectuiv(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
			void extGlGetQueryBufferObjectui64v(GLuint id, GLuint buffer, GLenum pname, GLintptr offset);
			void extGlTextureBarrier();
			void extGlSwapInterval(int interval);
			void extGlGetInternalformativ(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint* params);
			void extGlGetInternalformati64v(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint64* params);

		protected:
			// constructor
			COpenGLFunctionTable();

		private:


		};	// end of class COpenGLFunctionTable

		inline void COpenGLFunctionTable::extGlBindTextures(const GLuint& first, const GLsizei& count, const GLuint* textures, const GLenum* targets)
		{
			const GLenum supportedTargets[] = { GL_TEXTURE_1D,GL_TEXTURE_2D, // GL 1.x
												GL_TEXTURE_3D,GL_TEXTURE_RECTANGLE,GL_TEXTURE_CUBE_MAP, // GL 2.x
												GL_TEXTURE_1D_ARRAY,GL_TEXTURE_2D_ARRAY,GL_TEXTURE_BUFFER, // GL 3.x
												GL_TEXTURE_CUBE_MAP_ARRAY,GL_TEXTURE_2D_MULTISAMPLE,GL_TEXTURE_2D_MULTISAMPLE_ARRAY }; // GL 4.x

			if (COpenGLDriver::Version >= 440 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_multi_bind])
				glTexture.pglBindTextures(first, count, textures);
			else
			{
				int32_t activeTex = 0;
				glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTex);

				for (GLsizei i = 0; i < count; i++)
				{
					GLuint texture = textures ? textures[i] : 0;

					GLuint unit = first + i;
					glTexture.pglActiveTexture(GL_TEXTURE0 + unit);

					if (texture)
						glBindTexture(targets[i], texture);
					else
					{
						for (size_t j = 0; j < sizeof(supportedTargets) / sizeof(GLenum); j++)
							glBindTexture(supportedTargets[j], 0);
					}
				}

				glTexture.pglActiveTexture(activeTex);
			}
		}
		inline void COpenGLFunctionTable::extGlCreateTextures(GLenum target, GLsizei n, GLuint* textures)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
				glTexture.pglCreateTextures(target, n, textures);
			else
				glGenTextures(n, textures);
		}
		inline void COpenGLFunctionTable::extGlTextureBuffer(GLuint texture, GLenum internalformat, GLuint buffer)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
				glTexture.pglTextureBuffer(texture, internalformat, buffer);
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
				glTexture.pglTextureBufferEXT(texture, GL_TEXTURE_BUFFER, internalformat, buffer);
			else
			{
				GLint bound;
				glGetIntegerv(GL_TEXTURE_BINDING_BUFFER, &bound);
				glBindTexture(GL_TEXTURE_BUFFER, texture);
				glTexture.pglTexBuffer(GL_TEXTURE_BUFFER, internalformat, buffer);
				glBindTexture(GL_TEXTURE_BUFFER, bound);
			}
		}
		inline void COpenGLFunctionTable::extGlTextureBufferRange(GLuint texture, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizei length)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{

				if (glTexture.pglTextureBufferRange)
					glTexture.pglTextureBufferRange(texture, internalformat, buffer, offset, length);
			}
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glTexture.pglTextureBufferRangeEXT)
					glTexture.pglTextureBufferRangeEXT(texture, GL_TEXTURE_BUFFER, internalformat, buffer, offset, length);
			}
			else
			{
				GLint bound;
				glGetIntegerv(GL_TEXTURE_BINDING_BUFFER, &bound);
				glBindTexture(GL_TEXTURE_BUFFER, texture);
				if (glTexture.pglTexBufferRange)
					glTexture.pglTexBufferRange(GL_TEXTURE_BUFFER, internalformat, buffer, offset, length);
				glBindTexture(GL_TEXTURE_BUFFER, bound);
			}
		}
		inline void COpenGLFunctionTable::extGlTextureStorage1D(GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glTexture.pglTextureStorage1D)
					glTexture.pglTextureStorage1D(texture, levels, internalformat, width);
			}
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glTexture.pglTextureStorage1DEXT)
					glTexture.pglTextureStorage1DEXT(texture, target, levels, internalformat, width);
			}
			else if (glTexture.pglTexStorage1D)
			{
				GLint bound;
				switch (target)
				{
				case GL_TEXTURE_1D:
					glGetIntegerv(GL_TEXTURE_BINDING_1D, &bound);
					break;
				default:
					os::Printer::log("DevSH would like to ask you what are you doing!!??\n", ELL_ERROR);
					return;
				}
				glBindTexture(target, texture);
				glTexture.pglTexStorage1D(target, levels, internalformat, width);
				glBindTexture(target, bound);
			}
		}
		inline void COpenGLFunctionTable::extGlTextureStorage2D(GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glTexture.pglTextureStorage2D)
					glTexture.pglTextureStorage2D(texture, levels, internalformat, width, height);
			}
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glTexture.pglTextureStorage2DEXT)
					glTexture.pglTextureStorage2DEXT(texture, target, levels, internalformat, width, height);
			}
			else if (glTexture.pglTexStorage2D)
			{
				GLint bound;
				switch (target)
				{
				case GL_TEXTURE_1D_ARRAY:
					glGetIntegerv(GL_TEXTURE_BINDING_1D_ARRAY, &bound);
					break;
				case GL_TEXTURE_2D:
					glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);
					break;
				case GL_TEXTURE_CUBE_MAP:
					glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &bound);
					break;
				case GL_TEXTURE_RECTANGLE:
					glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE, &bound);
					break;
				default:
					os::Printer::log("DevSH would like to ask you what are you doing!!??\n", ELL_ERROR);
					return;
				}
				glBindTexture(target, texture);
				glTexture.pglTexStorage2D(target, levels, internalformat, width, height);
				glBindTexture(target, bound);
			}
		}
		inline void COpenGLFunctionTable::extGlTextureStorage3D(GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
				glTexture.pglTextureStorage3D(texture, levels, internalformat, width, height, depth);
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
				glTexture.pglTextureStorage3DEXT(texture, target, levels, internalformat, width, height, depth);
			else
			{
				GLint bound;
				switch (target)
				{
				case GL_TEXTURE_2D_ARRAY:
					glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY, &bound);
					break;
				case GL_TEXTURE_3D:
					glGetIntegerv(GL_TEXTURE_BINDING_3D, &bound);
					break;
				case GL_TEXTURE_CUBE_MAP_ARRAY:
					glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP_ARRAY, &bound);
					break;
				default:
					os::Printer::log("DevSH would like to ask you what are you doing!!??\n", ELL_ERROR);
					return;
				}
				glBindTexture(target, texture);
				glTexture.pglTexStorage3D(target, levels, internalformat, width, height, depth);
				glBindTexture(target, bound);
			}
		}
		inline void COpenGLFunctionTable::extGlTextureStorage2DMultisample(GLuint texture, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
				glTexture.pglTextureStorage2DMultisample(texture, samples, internalformat, width, height, fixedsamplelocations);
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
				glTexture.pglTextureStorage2DMultisampleEXT(texture, target, samples, internalformat, width, height, fixedsamplelocations);
			else
			{
				GLint bound;
				if (target != GL_TEXTURE_2D_MULTISAMPLE)
				{
					os::Printer::log("DevSH would like to ask you what are you doing!!??\n", ELL_ERROR);
					return;
				}
				else
					glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE, &bound);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
				glTexture.pglTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalformat, width, height, fixedsamplelocations);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, bound);
			}
		}
		inline void COpenGLFunctionTable::extGlTextureStorage3DMultisample(GLuint texture, GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
				glTexture.pglTextureStorage3DMultisample(texture, samples, internalformat, width, height, depth, fixedsamplelocations);
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
				glTexture.pglTextureStorage3DMultisampleEXT(texture, target, samples, internalformat, width, height, depth, fixedsamplelocations);
			else
			{
				GLint bound;
				if (target != GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
				{
					os::Printer::log("DevSH would like to ask you what are you doing!!??\n", ELL_ERROR);
					return;
				}
				else
					glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY, &bound);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, texture);
				glTexture.pglTexStorage3DMultisample(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, samples, internalformat, width, height, depth, fixedsamplelocations);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, bound);
			}
		}
		inline void COpenGLFunctionTable::extGlGetTextureSubImage(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLsizei bufSize, void* pixels)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_get_texture_sub_image])
				glTexture.pglGetTextureSubImage(texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, bufSize, pixels);
#ifdef _IRR_DEBUG
			else
				os::Printer::log("EDF_GET_TEXTURE_SUB_IMAGE Not Available! Tell DevSH to implement!\n", ELL_ERROR);
#endif // _IRR_DEBUG
		}
		inline void COpenGLFunctionTable::extGlGetTextureImage(GLuint texture, GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSizeHint, void* pixels)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
				glTexture.pglGetTextureImage(texture, level, format, type, bufSizeHint, pixels);
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
				glTexture.pglGetTextureImageEXT(texture, target, level, format, type, pixels);
			else
			{
				GLint bound = 0;
				switch (target)
				{
				case GL_TEXTURE_1D:
					glGetIntegerv(GL_TEXTURE_BINDING_1D, &bound);
					break;
				case GL_TEXTURE_1D_ARRAY:
					glGetIntegerv(GL_TEXTURE_BINDING_1D_ARRAY, &bound);
					break;
				case GL_TEXTURE_2D:
					glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);
					break;
				case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
				case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
				case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
				case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
				case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
				case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
				case GL_TEXTURE_CUBE_MAP:
					glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &bound);
					break;
				case GL_TEXTURE_RECTANGLE:
					glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE, &bound);
					break;
				case GL_TEXTURE_2D_ARRAY:
					glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY, &bound);
					break;
				case GL_TEXTURE_CUBE_MAP_ARRAY:
					glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP_ARRAY, &bound);
					break;
				case GL_TEXTURE_3D:
					glGetIntegerv(GL_TEXTURE_BINDING_3D, &bound);
					break;
				default:
					break;
				}
				glBindTexture(target, texture);
				glGetTexImage(target, level, format, type, pixels);
				glBindTexture(target, bound);
			}
		}
		inline void COpenGLFunctionTable::extGlGetCompressedTextureImage(GLuint texture, GLenum target, GLint level, GLsizei bufSizeHint, void* pixels)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
				glTexture.pglGetCompressedTextureImage(texture, level, bufSizeHint, pixels);
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
				glTexture.pglGetCompressedTextureImageEXT(texture, target, level, pixels);
			else
			{
				GLint bound = 0;
				switch (target)
				{
				case GL_TEXTURE_1D:
					glGetIntegerv(GL_TEXTURE_BINDING_1D, &bound);
					break;
				case GL_TEXTURE_1D_ARRAY:
					glGetIntegerv(GL_TEXTURE_BINDING_1D_ARRAY, &bound);
					break;
				case GL_TEXTURE_2D:
					glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);
					break;
				case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
				case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
				case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
				case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
				case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
				case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
				case GL_TEXTURE_CUBE_MAP:
					glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &bound);
					break;
				case GL_TEXTURE_RECTANGLE:
					glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE, &bound);
					break;
				case GL_TEXTURE_2D_ARRAY:
					glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY, &bound);
					break;
				case GL_TEXTURE_CUBE_MAP_ARRAY:
					glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP_ARRAY, &bound);
					break;
				case GL_TEXTURE_3D:
					glGetIntegerv(GL_TEXTURE_BINDING_3D, &bound);
					break;
				default:
					break;
				}
				glBindTexture(target, texture);
				glTexture.pglGetCompressedTexImage(target, level, pixels);
				glBindTexture(target, bound);
			}
		}
		inline void COpenGLFunctionTable::extGlTextureSubImage1D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
				glTexture.pglTextureSubImage1D(texture, level, xoffset, width, format, type, pixels);
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
				glTexture.pglTextureSubImage1DEXT(texture, target, level, xoffset, width, format, type, pixels);
			else
			{
				GLint bound;
				switch (target)
				{
				case GL_TEXTURE_1D:
					glGetIntegerv(GL_TEXTURE_BINDING_1D, &bound);
					break;
				default:
					os::Printer::log("DevSH would like to ask you what are you doing!!??\n", ELL_ERROR);
					return;
				}
				glBindTexture(target, texture);
				glTexSubImage1D(target, level, xoffset, width, format, type, pixels);
				glBindTexture(target, bound);
			}
		}
		inline void COpenGLFunctionTable::extGlTextureSubImage2D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
				glTexture.pglTextureSubImage2D(texture, level, xoffset, yoffset, width, height, format, type, pixels);
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
				glTexture.pglTextureSubImage2DEXT(texture, target, level, xoffset, yoffset, width, height, format, type, pixels);
			else
			{
				GLint bound;
				switch (target)
				{
				case GL_TEXTURE_1D_ARRAY:
					glGetIntegerv(GL_TEXTURE_BINDING_1D_ARRAY, &bound);
					break;
				case GL_TEXTURE_2D:
					glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);
					break;
				case GL_TEXTURE_2D_MULTISAMPLE:
					glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE, &bound);
					break;
				case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
				case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
				case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
				case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
				case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
				case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
					glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &bound);
					break;
				case GL_TEXTURE_RECTANGLE:
					glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE, &bound);
					break;
				default:
					os::Printer::log("DevSH would like to ask you what are you doing!!??\n", ELL_ERROR);
					return;
				}
				glBindTexture(target, texture);
				glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
				glBindTexture(target, bound);
			}
		}
		inline void COpenGLFunctionTable::extGlTextureSubImage3D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
				glTexture.pglTextureSubImage3D(texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
				glTexture.pglTextureSubImage3DEXT(texture, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
			else
			{
				GLint bound;
				switch (target)
				{
				case GL_TEXTURE_2D_ARRAY:
					glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY, &bound);
					break;
				case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
					glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY, &bound);
					break;
				case GL_TEXTURE_3D:
					glGetIntegerv(GL_TEXTURE_BINDING_3D, &bound);
					break;
				case GL_TEXTURE_CUBE_MAP:
					glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &bound);
					break;
				case GL_TEXTURE_CUBE_MAP_ARRAY:
					glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP_ARRAY, &bound);
					break;
				default:
					os::Printer::log("DevSH would like to ask you what are you doing!!??\n", ELL_ERROR);
					return;
				}
				glBindTexture(target, texture);
				glTexture.pglTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
				glBindTexture(target, bound);
			}
		}
		inline void COpenGLFunctionTable::extGlCompressedTextureSubImage1D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void* data)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glTexture.pglCompressedTextureSubImage1D)
					glTexture.pglCompressedTextureSubImage1D(texture, level, xoffset, width, format, imageSize, data);
			}
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glTexture.pglCompressedTextureSubImage1DEXT)
					glTexture.pglCompressedTextureSubImage1DEXT(texture, target, level, xoffset, width, format, imageSize, data);
			}
			else if (glTexture.pglCompressedTexSubImage1D)
			{
				GLint bound;
				switch (target)
				{
				case GL_TEXTURE_1D:
					glGetIntegerv(GL_TEXTURE_BINDING_1D, &bound);
					break;
				default:
					os::Printer::log("DevSH would like to ask you what are you doing!!??\n", ELL_ERROR);
					return;
				}
				glBindTexture(target, texture);
				glTexture.pglCompressedTexSubImage1D(target, level, xoffset, width, format, imageSize, data);
				glBindTexture(target, bound);
			}
		}
		inline void COpenGLFunctionTable::extGlCompressedTextureSubImage2D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glTexture.pglCompressedTextureSubImage2D)
					glTexture.pglCompressedTextureSubImage2D(texture, level, xoffset, yoffset, width, height, format, imageSize, data);
			}
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glTexture.pglCompressedTextureSubImage2DEXT)
					glTexture.pglCompressedTextureSubImage2DEXT(texture, target, level, xoffset, yoffset, width, height, format, imageSize, data);
			}
			else if (glTexture.pglCompressedTexSubImage2D)
			{
				GLint bound;
				switch (target)
				{
				case GL_TEXTURE_1D_ARRAY:
					glGetIntegerv(GL_TEXTURE_BINDING_1D_ARRAY, &bound);
					break;
				case GL_TEXTURE_2D:
					glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);
					break;
				case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
				case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
				case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
				case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
				case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
				case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
					glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &bound);
					break;
				default:
					os::Printer::log("DevSH would like to ask you what are you doing!!??\n", ELL_ERROR);
					return;
				}
				glBindTexture(target, texture);
				glTexture.pglCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
				glBindTexture(target, bound);
			}
		}
		inline void COpenGLFunctionTable::extGlCompressedTextureSubImage3D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glTexture.pglCompressedTextureSubImage3D)
					glTexture.pglCompressedTextureSubImage3D(texture, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
			}
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glTexture.pglCompressedTextureSubImage3DEXT)
					glTexture.pglCompressedTextureSubImage3DEXT(texture, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
			}
			else if (glTexture.pglCompressedTexSubImage3D)
			{
				GLint bound;
				switch (target)
				{
				case GL_TEXTURE_2D_ARRAY:
					glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY, &bound);
					break;
				case GL_TEXTURE_3D:
					glGetIntegerv(GL_TEXTURE_BINDING_3D, &bound);
					break;
				case GL_TEXTURE_CUBE_MAP:
					glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &bound);
					break;
				case GL_TEXTURE_CUBE_MAP_ARRAY:
					glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP_ARRAY, &bound);
					break;
				default:
					os::Printer::log("DevSH would like to ask you what are you doing!!??\n", ELL_ERROR);
					return;
				}
				glBindTexture(target, texture);
				glTexture.pglCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
				glBindTexture(target, bound);
			}
		}
		inline void COpenGLFunctionTable::extGlGenerateTextureMipmap(GLuint texture, GLenum target)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glTexture.pglGenerateTextureMipmap)
					glTexture.pglGenerateTextureMipmap(texture);
			}
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glTexture.pglGenerateTextureMipmapEXT)
					glTexture.pglGenerateTextureMipmapEXT(texture, target);
			}
			else
			{
				GLint bound;
				switch (target)
				{
				case GL_TEXTURE_1D:
					glGetIntegerv(GL_TEXTURE_BINDING_1D, &bound);
					break;
				case GL_TEXTURE_1D_ARRAY:
					glGetIntegerv(GL_TEXTURE_BINDING_1D_ARRAY, &bound);
					break;
				case GL_TEXTURE_2D:
					glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);
					break;
				case GL_TEXTURE_2D_ARRAY:
					glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY, &bound);
					break;
				case GL_TEXTURE_2D_MULTISAMPLE:
					glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE, &bound);
					break;
				case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
					glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY, &bound);
					break;
				case GL_TEXTURE_3D:
					glGetIntegerv(GL_TEXTURE_BINDING_3D, &bound);
					break;
				case GL_TEXTURE_BUFFER:
					glGetIntegerv(GL_TEXTURE_BINDING_BUFFER, &bound);
					break;
				case GL_TEXTURE_CUBE_MAP:
					glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &bound);
					break;
				case GL_TEXTURE_CUBE_MAP_ARRAY:
					glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP_ARRAY, &bound);
					break;
				case GL_TEXTURE_RECTANGLE:
					glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE, &bound);
					break;
				default:
					os::Printer::log("DevSH would like to ask you what are you doing!!??\n", ELL_ERROR);
					return;
				}
				glBindTexture(target, texture);
				glTexture.pglGenerateMipmap(target);
				glBindTexture(target, bound);
			}
		}
		inline void COpenGLFunctionTable::extGlTextureParameterIuiv(GLuint texture, GLenum target, GLenum pname, const GLuint* params)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
				glTexture.pglTextureParameterIuiv(texture, pname, params);
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
				glTexture.pglTextureParameterIuivEXT(texture, target, pname, params);
			else
			{
				GLint bound;
				switch (target)
				{
				case GL_TEXTURE_1D:
					glGetIntegerv(GL_TEXTURE_BINDING_1D, &bound);
					break;
				case GL_TEXTURE_1D_ARRAY:
					glGetIntegerv(GL_TEXTURE_BINDING_1D_ARRAY, &bound);
					break;
				case GL_TEXTURE_2D:
					glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);
					break;
				case GL_TEXTURE_2D_ARRAY:
					glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY, &bound);
					break;
				case GL_TEXTURE_2D_MULTISAMPLE:
					glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE, &bound);
					break;
				case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
					glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY, &bound);
					break;
				case GL_TEXTURE_3D:
					glGetIntegerv(GL_TEXTURE_BINDING_3D, &bound);
					break;
				case GL_TEXTURE_BUFFER:
					glGetIntegerv(GL_TEXTURE_BINDING_BUFFER, &bound);
					break;
				case GL_TEXTURE_CUBE_MAP:
					glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &bound);
					break;
				case GL_TEXTURE_CUBE_MAP_ARRAY:
					glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP_ARRAY, &bound);
					break;
				case GL_TEXTURE_RECTANGLE:
					glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE, &bound);
					break;
				default:
					os::Printer::log("DevSH would like to ask you what are you doing!!??\n", ELL_ERROR);
					return;
				}
				glBindTexture(target, texture);
				glTexture.pglTexParameterIuiv(target, pname, params);
				glBindTexture(target, bound);
			}
		}
		inline void COpenGLFunctionTable::extGlCreateSamplers(GLsizei n, GLuint* samplers)
		{
			if (glTexture.pglCreateSamplers)
				glTexture.pglCreateSamplers(n, samplers);
			else if (glTexture.pglGenSamplers)
				glTexture.pglGenSamplers(n, samplers);
			else memset(samplers, 0, 4 * n);
		}
		inline void COpenGLFunctionTable::extGlBindSamplers(const GLuint& first, const GLsizei& count, const GLuint* samplers)
		{
			if (COpenGLDriver::Version >= 440 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_multi_bind])
			{
				if (glTexture.pglBindSamplers)
					glTexture.pglBindSamplers(first, count, samplers);
			}
			else
			{
				for (GLsizei i = 0; i < count; i++)
				{
					GLuint unit = first + i;
					if (samplers)
					{
						if (glTexture.pglBindSampler)
							glTexture.pglBindSampler(unit, samplers[i]);
					}
					else
					{
						if (glTexture.pglBindSampler)
							glTexture.pglBindSampler(unit, 0);
					}
				}
			}
		}
		//inline void COpenGLFunctionTable::extGlBindImageTextures(GLuint first, GLsizei count, const GLuint* textures, const GLenum* formats)
		//{
		//	if (glTexture.pglBindImageTextures)
		//		glTexture.pglBindImageTextures(first, count, textures);
		//	else
		//	{
		//		for (GLsizei i = 0; i < count; i++)
		//		{
		//			if (!textures || textures[i] == 0u)
		//				extGlBindImageTexture(first + i, 0u, 0u, GL_FALSE, 0, GL_READ_WRITE, GL_R8);
		//			else
		//				extGlBindImageTexture(first + i, textures[i], 0, GL_TRUE, 0, GL_READ_WRITE, formats[i]);
		//		}
		//	}
		//}
		inline GLuint64 COpenGLFunctionTable::extGlGetTextureHandle(GLuint texture)
		{
			if (glTexture.pglGetTextureHandleARB)
				return glTexture.pglGetTextureHandleARB(texture);
			else if (glTexture.pglGetTextureHandleNV)
				return glTexture.pglGetTextureHandleNV(texture);
			return 0ull;
		}
		inline GLuint64 COpenGLFunctionTable::extGlGetTextureSamplerHandle(GLuint texture, GLuint sampler)
		{
			if (glTexture.pglGetTextureSamplerHandleARB)
				return glTexture.pglGetTextureSamplerHandleARB(texture, sampler);
			else if (glTexture.pglGetTextureSamplerHandleNV)
				return glTexture.pglGetTextureSamplerHandleNV(texture, sampler);
			return 0ull;
		}
		inline void COpenGLFunctionTable::extGlMakeTextureHandleResident(GLuint64 handle)
		{
			if (glTexture.pglMakeTextureHandleResidentARB)
				return glTexture.pglMakeTextureHandleResidentARB(handle);
			else if (glTexture.pglMakeTextureHandleResidentNV)
				return glTexture.pglMakeTextureHandleResidentNV(handle);
		}
		inline void COpenGLFunctionTable::extGlMakeTextureHandleNonResident(GLuint64 handle)
		{
			if (glTexture.pglMakeTextureHandleNonResidentARB)
				return glTexture.pglMakeTextureHandleNonResidentARB(handle);
			else if (glTexture.pglMakeTextureHandleNonResidentNV)
				return glTexture.pglMakeTextureHandleNonResidentNV(handle);
		}
		inline GLuint64 COpenGLFunctionTable::extGlGetImageHandle(GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum format)
		{
			if (glTexture.pglGetImageHandleARB)
				return glTexture.pglGetImageHandleARB(texture, level, layered, layer, format);
			else if (glTexture.pglGetImageHandleNV)
				return glTexture.pglGetImageHandleNV(texture, level, layered, layer, format);
			return 0ull;
		}
		inline void COpenGLFunctionTable::extGlMakeImageHandleResident(GLuint64 handle, GLenum access)
		{
			if (glTexture.pglMakeImageHandleResidentARB)
				return glTexture.pglMakeImageHandleResidentARB(handle, access);
			else if (glTexture.pglMakeImageHandleResidentNV)
				return glTexture.pglMakeImageHandleResidentNV(handle, access);
		}
		inline void COpenGLFunctionTable::extGlMakeImageHandleNonResident(GLuint64 handle)
		{
			if (glTexture.pglMakeImageHandleNonResidentARB)
				return glTexture.pglMakeImageHandleNonResidentARB(handle);
			else if (glTexture.pglMakeImageHandleNonResidentNV)
				return glTexture.pglMakeImageHandleNonResidentNV(handle);
		}
		inline GLboolean COpenGLFunctionTable::extGlIsTextureHandleResident(GLuint64 handle)
		{
			if (glTexture.pglIsTextureHandleResidentARB)
				return glTexture.pglIsTextureHandleResidentARB(handle);
			else if (glTexture.pglIsTextureHandleResidentNV)
				return glTexture.pglIsTextureHandleResidentNV(handle);
			return false;
		}
		inline GLboolean COpenGLFunctionTable::extGlIsImageHandleResident(GLuint64 handle)
		{
			if (glTexture.pglIsTextureHandleResidentARB)
				return glTexture.pglIsTextureHandleResidentARB(handle);
			else if (glTexture.pglIsTextureHandleResidentNV)
				return glTexture.pglIsTextureHandleResidentNV(handle);
			return false;
		}
		inline void COpenGLFunctionTable::extGlCreateFramebuffers(GLsizei n, GLuint* framebuffers)
		{
			if (!COpenGLDriver::needsDSAFramebufferHack)
			{
				if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
				{
					glFrameBuffer.pglCreateFramebuffers(n, framebuffers);
					return;
				}
			}

			glFrameBuffer.pglGenFramebuffers(n, framebuffers);
		}
		inline GLenum COpenGLFunctionTable::extGlCheckNamedFramebufferStatus(GLuint framebuffer, GLenum target)
		{
			if (!COpenGLDriver::needsDSAFramebufferHack)
			{
				if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
					return glFrameBuffer.pglCheckNamedFramebufferStatus(framebuffer, target);
				else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
					return glFrameBuffer.pglCheckNamedFramebufferStatusEXT(framebuffer, target);
			}

			GLenum retval;
			GLuint bound;
			glGetIntegerv(target == GL_READ_FRAMEBUFFER ? GL_READ_FRAMEBUFFER_BINDING : GL_DRAW_FRAMEBUFFER_BINDING, reinterpret_cast<GLint*>(&bound));

			if (bound != framebuffer)
				glFrameBuffer.pglBindFramebuffer(target, framebuffer);
			retval = glFrameBuffer.pglCheckFramebufferStatus(target);
			if (bound != framebuffer)
				glFrameBuffer.pglBindFramebuffer(target, bound);

			return retval;
		}
		inline void COpenGLFunctionTable::extGlNamedFramebufferTexture(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level)
		{
			if (!COpenGLDriver::needsDSAFramebufferHack)
			{
				if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
				{
					glFrameBuffer.pglNamedFramebufferTexture(framebuffer, attachment, texture, level);
					return;
				}
				else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
				{
					glFrameBuffer.pglNamedFramebufferTextureEXT(framebuffer, attachment, texture, level);
					return;
				}
			}

			GLuint bound;
			glGetIntegerv(GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint*>(&bound));

			if (bound != framebuffer)
				glFrameBuffer.pglBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			glFrameBuffer.pglFramebufferTexture(GL_FRAMEBUFFER, attachment, texture, level);
			if (bound != framebuffer)
				glFrameBuffer.pglBindFramebuffer(GL_FRAMEBUFFER, bound);
		}
		inline void COpenGLFunctionTable::extGlNamedFramebufferTextureLayer(GLuint framebuffer, GLenum attachment, GLuint texture, GLenum textureType, GLint level, GLint layer)
		{
			if (!COpenGLDriver::needsDSAFramebufferHack)
			{
				if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
				{
					glFrameBuffer.pglNamedFramebufferTextureLayer(framebuffer, attachment, texture, level, layer);
					return;
				}
			}

			if (textureType != GL_TEXTURE_CUBE_MAP)
			{
				if (!COpenGLDriver::needsDSAFramebufferHack && COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
				{
					glFrameBuffer.pglNamedFramebufferTextureLayerEXT(framebuffer, attachment, texture, level, layer);
				}
				else
				{
					GLuint bound;
					glGetIntegerv(GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint*>(&bound));

					if (bound != framebuffer)
						glFrameBuffer.pglBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
					glFrameBuffer.pglFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, texture, level, layer);
					if (bound != framebuffer)
						glFrameBuffer.pglBindFramebuffer(GL_FRAMEBUFFER, bound);
				}
			}
			else
			{
				constexpr GLenum CubeMapFaceToCubeMapFaceGLenum[IGPUImageView::ECMF_COUNT] = {
					GL_TEXTURE_CUBE_MAP_POSITIVE_X,GL_TEXTURE_CUBE_MAP_NEGATIVE_X,GL_TEXTURE_CUBE_MAP_POSITIVE_Y,GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,GL_TEXTURE_CUBE_MAP_POSITIVE_Z,GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
				};

				if (!COpenGLDriver::needsDSAFramebufferHack && COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
				{
					glFrameBuffer.pglNamedFramebufferTexture2DEXT(framebuffer, attachment, CubeMapFaceToCubeMapFaceGLenum[layer], texture, level);
				}
				else
				{
					GLuint bound;
					glGetIntegerv(GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint*>(&bound));

					if (bound != framebuffer)
						glFrameBuffer.pglBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
					glFrameBuffer.pglFramebufferTexture2D(GL_FRAMEBUFFER, attachment, CubeMapFaceToCubeMapFaceGLenum[layer], texture, level);
					if (bound != framebuffer)
						glFrameBuffer.pglBindFramebuffer(GL_FRAMEBUFFER, bound);
				}
			}
		}
		inline void COpenGLFunctionTable::extGlBlitNamedFramebuffer(GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
		{
			if (!COpenGLDriver::needsDSAFramebufferHack)
			{
				if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
				{
					glFrameBuffer.pglBlitNamedFramebuffer(readFramebuffer, drawFramebuffer, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
					return;
				}
			}

			GLint boundReadFBO = -1;
			GLint boundDrawFBO = -1;
			glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &boundReadFBO);
			glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &boundDrawFBO);

			if (static_cast<GLint>(readFramebuffer) != boundReadFBO)
				glFrameBuffer.pglBindFramebuffer(GL_READ_FRAMEBUFFER, readFramebuffer);
			if (static_cast<GLint>(drawFramebuffer) != boundDrawFBO)
				glFrameBuffer.pglBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFramebuffer);

			glFrameBuffer.pglBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);

			if (static_cast<GLint>(readFramebuffer) != boundReadFBO)
				glFrameBuffer.pglBindFramebuffer(GL_READ_FRAMEBUFFER, boundReadFBO);
			if (static_cast<GLint>(drawFramebuffer) != boundDrawFBO)
				glFrameBuffer.pglBindFramebuffer(GL_DRAW_FRAMEBUFFER, boundDrawFBO);
		}
		inline void COpenGLFunctionTable::extGlNamedFramebufferReadBuffer(GLuint framebuffer, GLenum mode)
		{
			if (!COpenGLDriver::needsDSAFramebufferHack)
			{
				if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
				{
					glFrameBuffer.pglNamedFramebufferReadBuffer(framebuffer, mode);
					return;
				}
				else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
				{
					glFrameBuffer.pglFramebufferReadBufferEXT(framebuffer, mode);
					return;
				}
			}

			GLint boundFBO;
			glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &boundFBO);

			if (static_cast<GLuint>(boundFBO) != framebuffer)
				glFrameBuffer.pglBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
			glReadBuffer(mode);
			if (static_cast<GLuint>(boundFBO) != framebuffer)
				glFrameBuffer.pglBindFramebuffer(GL_READ_FRAMEBUFFER, boundFBO);
		}
		inline void COpenGLFunctionTable::extGlNamedFramebufferDrawBuffer(GLuint framebuffer, GLenum buf)
		{
			if (!COpenGLDriver::needsDSAFramebufferHack)
			{
				if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
				{
					glFrameBuffer.pglNamedFramebufferDrawBuffer(framebuffer, buf);
					return;
				}
				else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
				{
					glFrameBuffer.pglFramebufferDrawBufferEXT(framebuffer, buf);
					return;
				}
			}

			GLint boundFBO;
			glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &boundFBO);

			if (static_cast<GLuint>(boundFBO) != framebuffer)
				glFrameBuffer.pglBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
			glDrawBuffer(buf);
			if (static_cast<GLuint>(boundFBO) != framebuffer)
				glFrameBuffer.pglBindFramebuffer(GL_DRAW_FRAMEBUFFER, boundFBO);
		}
		inline void COpenGLFunctionTable::extGlNamedFramebufferDrawBuffers(GLuint framebuffer, GLsizei n, const GLenum* bufs)
		{
			if (!COpenGLDriver::needsDSAFramebufferHack)
			{
				if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
				{
					glFrameBuffer.pglNamedFramebufferDrawBuffers(framebuffer, n, bufs);
					return;
				}
				else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
				{
					glFrameBuffer.pglFramebufferDrawBuffersEXT(framebuffer, n, bufs);
					return;
				}
			}

			GLint boundFBO;
			glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &boundFBO);

			if (static_cast<GLuint>(boundFBO) != framebuffer)
				glFrameBuffer.pglBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
			glFrameBuffer.pglDrawBuffers(n, bufs);
			if (static_cast<GLuint>(boundFBO) != framebuffer)
				glFrameBuffer.pglBindFramebuffer(GL_DRAW_FRAMEBUFFER, boundFBO);
		}
		inline void COpenGLFunctionTable::extGlClearNamedFramebufferiv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint* value)
		{
			if (!COpenGLDriver::needsDSAFramebufferHack)
			{
				if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
				{
					glFrameBuffer.pglClearNamedFramebufferiv(framebuffer, buffer, drawbuffer, value);
					return;
				}
			}

			GLint boundFBO = -1;
			glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &boundFBO);
			if (boundFBO < 0)
				return;

			if (static_cast<GLuint>(boundFBO) != framebuffer)
				glFrameBuffer.pglBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			glFrameBuffer.pglClearBufferiv(buffer, drawbuffer, value);
			if (static_cast<GLuint>(boundFBO) != framebuffer)
				glFrameBuffer.pglBindFramebuffer(GL_FRAMEBUFFER, boundFBO);
		}
		inline void COpenGLFunctionTable::extGlClearNamedFramebufferuiv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLuint* value)
		{
			if (!COpenGLDriver::needsDSAFramebufferHack)
			{
				if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
				{
					glFrameBuffer.pglClearNamedFramebufferuiv(framebuffer, buffer, drawbuffer, value);
					return;
				}
			}

			GLint boundFBO = -1;
			glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &boundFBO);
			if (boundFBO < 0)
				return;

			if (static_cast<GLuint>(boundFBO) != framebuffer)
				glFrameBuffer.pglBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			glFrameBuffer.pglClearBufferuiv(buffer, drawbuffer, value);
			if (static_cast<GLuint>(boundFBO) != framebuffer)
				glFrameBuffer.pglBindFramebuffer(GL_FRAMEBUFFER, boundFBO);
		}
		inline void COpenGLFunctionTable::extGlClearNamedFramebufferfv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat* value)
		{
			if (!COpenGLDriver::needsDSAFramebufferHack)
			{
				if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
				{
					glFrameBuffer.pglClearNamedFramebufferfv(framebuffer, buffer, drawbuffer, value);
					return;
				}
			}

			GLint boundFBO = -1;
			glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &boundFBO);
			if (boundFBO < 0)
				return;

			if (static_cast<GLuint>(boundFBO) != framebuffer)
				glFrameBuffer.pglBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			glFrameBuffer.pglClearBufferfv(buffer, drawbuffer, value);
			if (static_cast<GLuint>(boundFBO) != framebuffer)
				glFrameBuffer.pglBindFramebuffer(GL_FRAMEBUFFER, boundFBO);
		}
		inline void COpenGLFunctionTable::extGlClearNamedFramebufferfi(GLuint framebuffer, GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)
		{
			if (!COpenGLDriver::needsDSAFramebufferHack)
			{
				if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
				{
					glFrameBuffer.pglClearNamedFramebufferfi(framebuffer, buffer, drawbuffer, depth, stencil);
					return;
				}
			}

			GLint boundFBO = -1;
			glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &boundFBO);
			if (boundFBO < 0)
				return;
			glFrameBuffer.pglBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			glFrameBuffer.pglClearBufferfi(buffer, drawbuffer, depth, stencil);
			glFrameBuffer.pglBindFramebuffer(GL_FRAMEBUFFER, boundFBO);
		}
		inline void COpenGLFunctionTable::extGlCreateBuffers(GLsizei n, GLuint* buffers)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glBuffer.pglCreateBuffers)
					glBuffer.pglCreateBuffers(n, buffers);
				else if (buffers)
					memset(buffers, 0, n * sizeof(GLuint));
			}
			else
			{
				if (glBuffer.pglGenBuffers)
					glBuffer.pglGenBuffers(n, buffers);
				else if (buffers)
					memset(buffers, 0, n * sizeof(GLuint));
			}
		}

		inline void COpenGLFunctionTable::extGlBindBuffersBase(const GLenum& target, const GLuint& first, const GLsizei& count, const GLuint* buffers)
		{
			if (COpenGLDriver::Version >= 440 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_multi_bind])
			{
				if (glBuffer.pglBindBuffersBase)
					glBuffer.pglBindBuffersBase(target, first, count, buffers);
			}
			else
			{
				for (GLsizei i = 0; i < count; i++)
				{
					if (glBuffer.pglBindBufferBase)
						glBuffer.pglBindBufferBase(target, first + i, buffers ? buffers[i] : 0);
				}
			}
		}

		inline void COpenGLFunctionTable::extGlBindBuffersRange(const GLenum& target, const GLuint& first, const GLsizei& count, const GLuint* buffers, const GLintptr* offsets, const GLsizeiptr* sizes)
		{
			if (COpenGLDriver::Version >= 440 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_multi_bind])
			{
				if (glBuffer.pglBindBuffersRange)
					glBuffer.pglBindBuffersRange(target, first, count, buffers, offsets, sizes);
			}
			else
			{
				for (GLsizei i = 0; i < count; i++)
				{
					if (buffers)
					{
						if (glBuffer.pglBindBufferRange)
							glBuffer.pglBindBufferRange(target, first + i, buffers[i], offsets[i], sizes[i]);
					}
					else
					{
						if (glBuffer.pglBindBufferBase)
							glBuffer.pglBindBufferBase(target, first + i, 0);
					}
				}
			}
		}

		inline void COpenGLFunctionTable::extGlNamedBufferStorage(GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glBuffer.pglNamedBufferStorage)
					glBuffer.pglNamedBufferStorage(buffer, size, data, flags);
			}
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glBuffer.pglNamedBufferStorageEXT)
					glBuffer.pglNamedBufferStorageEXT(buffer, size, data, flags);
			}
			else if (glBuffer.pglBufferStorage && glBuffer.pglBindBuffer)
			{
				GLint bound;
				glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &bound);
				glBuffer.pglBindBuffer(GL_ARRAY_BUFFER, buffer);
				glBuffer.pglBufferStorage(GL_ARRAY_BUFFER, size, data, flags);
				glBuffer.pglBindBuffer(GL_ARRAY_BUFFER, bound);
			}
		}

		inline void COpenGLFunctionTable::extGlNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glBuffer.pglNamedBufferSubData)
					glBuffer.pglNamedBufferSubData(buffer, offset, size, data);
			}
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glBuffer.pglNamedBufferSubDataEXT)
					glBuffer.pglNamedBufferSubDataEXT(buffer, offset, size, data);
			}
			else if (glBuffer.pglBufferSubData && glBuffer.pglBindBuffer)
			{
				GLint bound;
				glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &bound);
				glBuffer.pglBindBuffer(GL_ARRAY_BUFFER, buffer);
				glBuffer.pglBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
				glBuffer.pglBindBuffer(GL_ARRAY_BUFFER, bound);
			}
		}

		inline void COpenGLFunctionTable::extGlGetNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, void* data)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glBuffer.pglGetNamedBufferSubData)
					glBuffer.pglGetNamedBufferSubData(buffer, offset, size, data);
			}
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glBuffer.pglGetNamedBufferSubDataEXT)
					glBuffer.pglGetNamedBufferSubDataEXT(buffer, offset, size, data);
			}
			else if (glBuffer.pglGetBufferSubData && glBuffer.pglBindBuffer)
			{
				GLint bound;
				glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &bound);
				glBuffer.pglBindBuffer(GL_ARRAY_BUFFER, buffer);
				glBuffer.pglGetBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
				glBuffer.pglBindBuffer(GL_ARRAY_BUFFER, bound);
			}
		}

		inline void* COpenGLFunctionTable::extGlMapNamedBuffer(GLuint buffer, GLbitfield access)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glBuffer.pglMapNamedBuffer)
					return glBuffer.pglMapNamedBuffer(buffer, access);
			}
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glBuffer.pglMapNamedBufferEXT)
					return glBuffer.pglMapNamedBufferEXT(buffer, access);
			}
			else if (glBuffer.pglMapBuffer && glBuffer.pglBindBuffer)
			{
				GLvoid* retval;
				GLint bound;
				glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &bound);
				glBuffer.pglBindBuffer(GL_ARRAY_BUFFER, buffer);
				retval = glBuffer.pglMapBuffer(GL_ARRAY_BUFFER, access);
				glBuffer.pglBindBuffer(GL_ARRAY_BUFFER, bound);
				return retval;
			}
			return NULL;
		}

		inline void* COpenGLFunctionTable::extGlMapNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glBuffer.pglMapNamedBufferRange)
					return glBuffer.pglMapNamedBufferRange(buffer, offset, length, access);
			}
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glBuffer.pglMapNamedBufferRangeEXT)
					return glBuffer.pglMapNamedBufferRangeEXT(buffer, offset, length, access);
			}
			else if (glBuffer.pglMapBufferRange && glBuffer.pglBindBuffer)
			{
				GLvoid* retval;
				GLint bound;
				glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &bound);
				glBuffer.pglBindBuffer(GL_ARRAY_BUFFER, buffer);
				retval = glBuffer.pglMapBufferRange(GL_ARRAY_BUFFER, offset, length, access);
				glBuffer.pglBindBuffer(GL_ARRAY_BUFFER, bound);
				return retval;
			}
			return NULL;
		}

		inline void COpenGLFunctionTable::extGlFlushMappedNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glBuffer.pglFlushMappedNamedBufferRange)
					glBuffer.pglFlushMappedNamedBufferRange(buffer, offset, length);
			}
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glBuffer.pglFlushMappedNamedBufferRangeEXT)
					glBuffer.pglFlushMappedNamedBufferRangeEXT(buffer, offset, length);
			}
			else if (glBuffer.pglFlushMappedBufferRange && glBuffer.pglBindBuffer)
			{
				GLint bound;
				glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &bound);
				glBuffer.pglBindBuffer(GL_ARRAY_BUFFER, buffer);
				glBuffer.pglFlushMappedBufferRange(GL_ARRAY_BUFFER, offset, length);
				glBuffer.pglBindBuffer(GL_ARRAY_BUFFER, bound);
			}
		}

		inline GLboolean COpenGLFunctionTable::extGlUnmapNamedBuffer(GLuint buffer)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glBuffer.pglUnmapNamedBuffer)
					return glBuffer.pglUnmapNamedBuffer(buffer);
			}
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glBuffer.pglUnmapNamedBufferEXT)
					return glBuffer.pglUnmapNamedBufferEXT(buffer);
			}
			else if (glBuffer.pglUnmapBuffer && glBuffer.pglBindBuffer)
			{
				GLboolean retval;
				GLint bound;
				glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &bound);
				glBuffer.pglBindBuffer(GL_ARRAY_BUFFER, buffer);
				retval = glBuffer.pglUnmapBuffer(GL_ARRAY_BUFFER);
				glBuffer.pglBindBuffer(GL_ARRAY_BUFFER, bound);
				return retval;
			}
			return false;
		}

		inline void COpenGLFunctionTable::extGlClearNamedBufferData(GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const void* data)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glBuffer.pglClearNamedBufferData)
					glBuffer.pglClearNamedBufferData(buffer, internalformat, format, type, data);
			}
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glBuffer.pglClearNamedBufferDataEXT)
					glBuffer.pglClearNamedBufferDataEXT(buffer, internalformat, format, type, data);
			}
			else if (glBuffer.pglClearBufferData && glBuffer.pglBindBuffer)
			{
				GLint bound;
				glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &bound);
				glBuffer.pglBindBuffer(GL_ARRAY_BUFFER, buffer);
				glBuffer.pglClearBufferData(GL_ARRAY_BUFFER, internalformat, format, type, data);
				glBuffer.pglBindBuffer(GL_ARRAY_BUFFER, bound);
			}
		}

		inline void COpenGLFunctionTable::extGlClearNamedBufferSubData(GLuint buffer, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void* data)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glBuffer.pglClearNamedBufferSubData)
					glBuffer.pglClearNamedBufferSubData(buffer, internalformat, offset, size, format, type, data);
			}
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glBuffer.pglClearNamedBufferSubDataEXT)
					glBuffer.pglClearNamedBufferSubDataEXT(buffer, internalformat, offset, size, format, type, data);
			}
			else if (glBuffer.pglClearBufferSubData && glBuffer.pglBindBuffer)
			{
				GLint bound;
				glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &bound);
				glBuffer.pglBindBuffer(GL_ARRAY_BUFFER, buffer);
				glBuffer.pglClearBufferSubData(GL_ARRAY_BUFFER, internalformat, offset, size, format, type, data);
				glBuffer.pglBindBuffer(GL_ARRAY_BUFFER, bound);
			}
		}

		inline void COpenGLFunctionTable::extGlCopyNamedBufferSubData(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glBuffer.pglCopyNamedBufferSubData)
					glBuffer.pglCopyNamedBufferSubData(readBuffer, writeBuffer, readOffset, writeOffset, size);
			}
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glBuffer.pglNamedCopyBufferSubDataEXT)
					glBuffer.pglNamedCopyBufferSubDataEXT(readBuffer, writeBuffer, readOffset, writeOffset, size);
			}
			else if (glBuffer.pglCopyBufferSubData && glBuffer.pglBindBuffer)
			{
				GLint boundRead, boundWrite;
				glGetIntegerv(GL_COPY_READ_BUFFER_BINDING, &boundRead);
				glGetIntegerv(GL_COPY_WRITE_BUFFER_BINDING, &boundWrite);
				glBuffer.pglBindBuffer(GL_COPY_READ_BUFFER, readBuffer);
				glBuffer.pglBindBuffer(GL_COPY_WRITE_BUFFER, writeBuffer);
				glBuffer.pglCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, readOffset, writeOffset, size);
				glBuffer.pglBindBuffer(GL_COPY_READ_BUFFER, boundRead);
				glBuffer.pglBindBuffer(GL_COPY_WRITE_BUFFER, boundWrite);
			}
		}

		inline void COpenGLFunctionTable::extGlGetNamedBufferParameteriv(const GLuint& buffer, const GLenum& value, GLint* data)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glBuffer.pglGetNamedBufferParameteriv)
					glBuffer.pglGetNamedBufferParameteriv(buffer, value, data);
			}
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glBuffer.pglGetNamedBufferParameterivEXT)
					glBuffer.pglGetNamedBufferParameterivEXT(buffer, value, data);
			}
			else if (glBuffer.pglGetBufferParameteriv && glBuffer.pglBindBuffer)
			{
				GLint bound;
				glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &bound);
				glBuffer.pglBindBuffer(GL_ARRAY_BUFFER, buffer);
				glBuffer.pglGetBufferParameteriv(GL_ARRAY_BUFFER, value, data);
				glBuffer.pglBindBuffer(GL_ARRAY_BUFFER, bound);
			}
		}

		inline void COpenGLFunctionTable::extGlGetNamedBufferParameteri64v(const GLuint& buffer, const GLenum& value, GLint64* data)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glBuffer.pglGetNamedBufferParameteri64v)
					glBuffer.pglGetNamedBufferParameteri64v(buffer, value, data);
			}
			else if (glBuffer.pglGetBufferParameteri64v && glBuffer.pglBindBuffer)
			{
				GLint bound;
				glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &bound);
				glBuffer.pglBindBuffer(GL_ARRAY_BUFFER, buffer);
				glBuffer.pglGetBufferParameteri64v(GL_ARRAY_BUFFER, value, data);
				glBuffer.pglBindBuffer(GL_ARRAY_BUFFER, bound);
			}
		}


		inline void COpenGLFunctionTable::extGlCreateVertexArrays(GLsizei n, GLuint* arrays)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glVertex.pglCreateVertexArrays)
					glVertex.pglCreateVertexArrays(n, arrays);
			}
			else
			{
				if (glVertex.pglGenVertexArrays)
					glVertex.pglGenVertexArrays(n, arrays);
				else
					memset(arrays, 0, sizeof(GLuint) * n);
			}
		}

		inline void COpenGLFunctionTable::extGlVertexArrayElementBuffer(GLuint vaobj, GLuint buffer)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glVertex.pglVertexArrayElementBuffer)
					glVertex.pglVertexArrayElementBuffer(vaobj, buffer);
			}
			else if (glBuffer.pglBindBuffer && glVertex.pglBindVertexArray)
			{
				// Save the previous bound vertex array
				GLint restoreVertexArray;
				glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &restoreVertexArray);
				glVertex.pglBindVertexArray(vaobj);
				glBuffer.pglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
				glVertex.pglBindVertexArray(restoreVertexArray);
			}
		}

		inline void COpenGLFunctionTable::extGlVertexArrayVertexBuffer(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glVertex.pglVertexArrayVertexBuffer)
					glVertex.pglVertexArrayVertexBuffer(vaobj, bindingindex, buffer, offset, stride);
			}
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glVertex.pglVertexArrayBindVertexBufferEXT)
					glVertex.pglVertexArrayBindVertexBufferEXT(vaobj, bindingindex, buffer, offset, stride);
			}
			else if (glVertex.pglBindVertexBuffer && glVertex.pglBindVertexArray)
			{
				// Save the previous bound vertex array
				GLint restoreVertexArray;
				glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &restoreVertexArray);
				glVertex.pglBindVertexArray(vaobj);
				glVertex.pglBindVertexBuffer(bindingindex, buffer, offset, stride);
				glVertex.pglBindVertexArray(restoreVertexArray);
			}
		}

		inline void COpenGLFunctionTable::extGlVertexArrayAttribBinding(GLuint vaobj, GLuint attribindex, GLuint bindingindex)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glVertex.pglVertexArrayAttribBinding)
					glVertex.pglVertexArrayAttribBinding(vaobj, attribindex, bindingindex);
			}
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glVertex.pglVertexArrayVertexAttribBindingEXT)
					glVertex.pglVertexArrayVertexAttribBindingEXT(vaobj, attribindex, bindingindex);
			}
			else if (glVertex.pglVertexAttribBinding && glVertex.pglBindVertexArray)
			{
				// Save the previous bound vertex array
				GLint restoreVertexArray;
				glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &restoreVertexArray);
				glVertex.pglBindVertexArray(vaobj);
				glVertex.pglVertexAttribBinding(attribindex, bindingindex);
				glVertex.pglBindVertexArray(restoreVertexArray);
			}
		}

		inline void COpenGLFunctionTable::extGlEnableVertexArrayAttrib(GLuint vaobj, GLuint index)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glVertex.pglEnableVertexArrayAttrib)
					glVertex.pglEnableVertexArrayAttrib(vaobj, index);
			}
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glVertex.pglEnableVertexArrayAttribEXT)
					glVertex.pglEnableVertexArrayAttribEXT(vaobj, index);
			}
			else if (glVertex.pglEnableVertexAttribArray && glVertex.pglBindVertexArray)
			{
				// Save the previous bound vertex array
				GLint restoreVertexArray;
				glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &restoreVertexArray);
				glVertex.pglBindVertexArray(vaobj);
				glVertex.pglEnableVertexAttribArray(index);
				glVertex.pglBindVertexArray(restoreVertexArray);
			}
		}

		inline void COpenGLFunctionTable::extGlDisableVertexArrayAttrib(GLuint vaobj, GLuint index)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glVertex.pglDisableVertexArrayAttrib)
					glVertex.pglDisableVertexArrayAttrib(vaobj, index);
			}
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glVertex.pglDisableVertexArrayAttribEXT)
					glVertex.pglDisableVertexArrayAttribEXT(vaobj, index);
			}
			else if (glVertex.pglDisableVertexAttribArray && glVertex.pglBindVertexArray)
			{
				// Save the previous bound vertex array
				GLint restoreVertexArray;
				glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &restoreVertexArray);
				glVertex.pglBindVertexArray(vaobj);
				glVertex.pglDisableVertexAttribArray(index);
				glVertex.pglBindVertexArray(restoreVertexArray);
			}
		}

		inline void COpenGLFunctionTable::extGlVertexArrayAttribFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glVertex.pglVertexArrayAttribFormat)
					glVertex.pglVertexArrayAttribFormat(vaobj, attribindex, size, type, normalized, relativeoffset);
			}
			else if (!COpenGLDriver::IsIntelGPU && COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glVertex.pglVertexArrayVertexAttribFormatEXT)
					glVertex.pglVertexArrayVertexAttribFormatEXT(vaobj, attribindex, size, type, normalized, relativeoffset);
			}
			else if (glVertex.pglVertexAttribFormat && glVertex.pglBindVertexArray)
			{
				// Save the previous bound vertex array
				GLint restoreVertexArray;
				glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &restoreVertexArray);
				glVertex.pglBindVertexArray(vaobj);
				glVertex.pglVertexAttribFormat(attribindex, size, type, normalized, relativeoffset);
				glVertex.pglBindVertexArray(restoreVertexArray);
			}
		}

		inline void COpenGLFunctionTable::extGlVertexArrayAttribIFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glVertex.pglVertexArrayAttribIFormat)
					glVertex.pglVertexArrayAttribIFormat(vaobj, attribindex, size, type, relativeoffset);
			}
			else if (!COpenGLDriver::IsIntelGPU && COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glVertex.pglVertexArrayVertexAttribIFormatEXT)
					glVertex.pglVertexArrayVertexAttribIFormatEXT(vaobj, attribindex, size, type, relativeoffset);
			}
			else if (glVertex.pglVertexAttribIFormat && glVertex.pglBindVertexArray)
			{
				// Save the previous bound vertex array
				GLint restoreVertexArray;
				glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &restoreVertexArray);
				glVertex.pglBindVertexArray(vaobj);
				glVertex.pglVertexAttribIFormat(attribindex, size, type, relativeoffset);
				glVertex.pglBindVertexArray(restoreVertexArray);
			}
		}

		inline void COpenGLFunctionTable::extGlVertexArrayAttribLFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glVertex.pglVertexArrayAttribLFormat)
					glVertex.pglVertexArrayAttribLFormat(vaobj, attribindex, size, type, relativeoffset);
			}
			else if (!COpenGLDriver::IsIntelGPU && COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glVertex.pglVertexArrayVertexAttribLFormatEXT)
					glVertex.pglVertexArrayVertexAttribLFormatEXT(vaobj, attribindex, size, type, relativeoffset);
			}
			else if (glVertex.pglVertexAttribLFormat && glVertex.pglBindVertexArray)
			{
				// Save the previous bound vertex array
				GLint restoreVertexArray;
				glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &restoreVertexArray);
				glVertex.pglBindVertexArray(vaobj);
				glVertex.pglVertexAttribLFormat(attribindex, size, type, relativeoffset);
				glVertex.pglBindVertexArray(restoreVertexArray);
			}
		}

		inline void COpenGLFunctionTable::extGlVertexArrayBindingDivisor(GLuint vaobj, GLuint bindingindex, GLuint divisor)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glVertex.pglVertexArrayBindingDivisor)
					glVertex.pglVertexArrayBindingDivisor(vaobj, bindingindex, divisor);
			}
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_EXT_direct_state_access])
			{
				if (glVertex.pglVertexArrayVertexBindingDivisorEXT)
					glVertex.pglVertexArrayVertexBindingDivisorEXT(vaobj, bindingindex, divisor);
			}
			else if (glVertex.pglVertexBindingDivisor && glVertex.pglBindVertexArray)
			{
				// Save the previous bound vertex array
				GLint restoreVertexArray;
				glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &restoreVertexArray);
				glVertex.pglBindVertexArray(vaobj);
				glVertex.pglVertexBindingDivisor(bindingindex, divisor);
				glVertex.pglBindVertexArray(restoreVertexArray);
			}
		}



		inline void COpenGLFunctionTable::extGlCreateTransformFeedbacks(GLsizei n, GLuint* ids)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glTransformFeedback.pglCreateTransformFeedbacks)
					glTransformFeedback.pglCreateTransformFeedbacks(n, ids);
			}
			else
			{
				if (glTransformFeedback.pglGenTransformFeedbacks)
					glTransformFeedback.pglGenTransformFeedbacks(n, ids);
			}
		}
		inline void COpenGLFunctionTable::extGlTransformFeedbackBufferBase(GLuint xfb, GLuint index, GLuint buffer)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glTransformFeedback.pglTransformFeedbackBufferBase)
					glTransformFeedback.pglTransformFeedbackBufferBase(xfb, index, buffer);
			}
			else
			{
				GLint restoreXFormFeedback;
				glGetIntegerv(GL_TRANSFORM_FEEDBACK_BINDING, &restoreXFormFeedback);
				//	extGlBindTransformFeedback(GL_TRANSFORM_FEEDBACK, xfb);
				extGlBindBuffersBase(GL_TRANSFORM_FEEDBACK_BUFFER, index, 1, &buffer);
				//	extGlBindTransformFeedback(GL_TRANSFORM_FEEDBACK, restoreXFormFeedback);
			}
		}
		inline void COpenGLFunctionTable::extGlTransformFeedbackBufferRange(GLuint xfb, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glTransformFeedback.pglTransformFeedbackBufferRange)
					glTransformFeedback.pglTransformFeedbackBufferRange(xfb, index, buffer, offset, size);
			}
			else
			{
				GLint restoreXFormFeedback;
				glGetIntegerv(GL_TRANSFORM_FEEDBACK_BINDING, &restoreXFormFeedback);
				//	extGlBindTransformFeedback(GL_TRANSFORM_FEEDBACK, xfb);
				extGlBindBuffersRange(GL_TRANSFORM_FEEDBACK_BUFFER, index, 1, &buffer, &offset, &size);
				//	extGlBindTransformFeedback(GL_TRANSFORM_FEEDBACK, restoreXFormFeedback);
			}
		}

		inline void COpenGLFunctionTable::extGlCreateQueries(GLenum target, GLsizei n, GLuint* ids)
		{
			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glQuery.pglCreateQueries)
					glQuery.pglCreateQueries(target, n, ids);
			}
			else
			{
				if (glQuery.pglGenQueries)
					glQuery.pglGenQueries(n, ids);
			}
		}
		inline void COpenGLFunctionTable::extGlGetQueryBufferObjectuiv(GLuint id, GLuint buffer, GLenum pname, GLintptr offset)
		{
			if (COpenGLDriver::Version < 440 && !COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_query_buffer_object])
			{
#ifdef _DEBuG
				os::Printer::log("GL_ARB_query_buffer_object unsupported!\n
#endif // _DEBuG
					return;
			}

			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glQuery.pglGetQueryBufferObjectuiv)
					glQuery.pglGetQueryBufferObjectuiv(id, buffer, pname, offset);
			}
			else
			{
				GLint restoreQueryBuffer;
				glGetIntegerv(GL_QUERY_BUFFER_BINDING, &restoreQueryBuffer);
				glBuffer.pglBindBuffer(GL_QUERY_BUFFER, id);
				if (glQuery.pglGetQueryObjectuiv)
					glQuery.pglGetQueryObjectuiv(id, pname, reinterpret_cast<GLuint*>(offset));
				glBuffer.pglBindBuffer(GL_QUERY_BUFFER, restoreQueryBuffer);
			}
		}
		inline void COpenGLFunctionTable::extGlGetQueryBufferObjectui64v(GLuint id, GLuint buffer, GLenum pname, GLintptr offset)
		{
			if (COpenGLDriver::Version < 440 && !COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_query_buffer_object])
			{
#ifdef _DEBuG
				os::Printer::log("GL_ARB_query_buffer_object unsupported!\n
#endif // _DEBuG
					return;
			}

			if (COpenGLDriver::Version >= 450 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_direct_state_access])
			{
				if (glQuery.pglGetQueryBufferObjectui64v)
					glQuery.pglGetQueryBufferObjectui64v(id, buffer, pname, offset);
			}
			else
			{
				GLint restoreQueryBuffer;
				glGetIntegerv(GL_QUERY_BUFFER_BINDING, &restoreQueryBuffer);
				glBuffer.pglBindBuffer(GL_QUERY_BUFFER, id);
				if (glQuery.pglGetQueryObjectui64v)
					glQuery.pglGetQueryObjectui64v(id, pname, reinterpret_cast<GLuint64*>(offset));
				glBuffer.pglBindBuffer(GL_QUERY_BUFFER, restoreQueryBuffer);
			}
		}



		inline void COpenGLFunctionTable::extGlTextureBarrier()
		{
			if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_texture_barrier])
				glSync.pglTextureBarrier();
			else if (COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_NV_texture_barrier])
				glSync.pglTextureBarrierNV();
#ifdef _IRR_DEBUG
			else
				os::Printer::log("EDF_TEXTURE_BARRIER Not Available!\n", ELL_ERROR);
#endif // _IRR_DEBUG
		}


		inline void COpenGLFunctionTable::extGlSwapInterval(int interval)
		{
			// we have wglext, so try to use that
#if defined(_IRR_WINDOWS_API_) && defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
#ifdef WGL_EXT_swap_control
			if (glOS.pwglSwapIntervalEXT)
				glOS.pwglSwapIntervalEXT(interval);
#endif
#endif
#ifdef _IRR_COMPILE_WITH_X11_DEVICE_
			//TODO: Check GLX_EXT_swap_control and GLX_MESA_swap_control
#ifdef GLX_SGI_swap_control
	// does not work with interval==0
			if (interval && glOS.pglxSwapIntervalSGI)
				glOS.pglxSwapIntervalSGI(interval);
#elif defined(GLX_EXT_swap_control)
			Display* dpy = glXGetCurrentDisplay();
			GLXDrawable drawable = glXGetCurrentDrawable();

			if (glOS.pglxSwapIntervalEXT)
				glOS.pglxSwapIntervalEXT(dpy, drawable, interval);
#elif defined(GLX_MESA_swap_control)
			if (glOS.pglxSwapIntervalMESA)
				glOS.pglxSwapIntervalMESA(interval);
#endif
#endif
		}

		inline void COpenGLFunctionTable::extGlGetInternalformativ(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint* params)
		{
			if (COpenGLDriver::Version >= 460 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_internalformat_query])
			{
				if (glGeneral.pglGetInternalformativ)
					glGeneral.pglGetInternalformativ(target, internalformat, pname, bufSize, params);
			}
		}

		inline void COpenGLFunctionTable::extGlGetInternalformati64v(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint64* params)
		{
			if (COpenGLDriver::Version >= 460 || COpenGLDriver::FeatureAvailable[COpenGLDriver::EOpenGLFeatures::IRR_ARB_internalformat_query])
			{
				if (glGeneral.pglGetInternalformati64v)
					glGeneral.pglGetInternalformati64v(target, internalformat, pname, bufSize, params);
			}
		}

		}		//namespace video
	}		//namespace irr
#undef GL_GLEXT_PROTOTYPES
#endif
