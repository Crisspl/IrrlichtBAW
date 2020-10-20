#include <irrlicht.h>

#include "SPIRV-Tools/include/spirv-tools/optimizer.hpp" 
#include "SPIRV-Tools/include/spirv-tools/libspirv.h"
#include <shaderc\shaderc.hpp>
#include "spirv_cross/spirv_glsl.hpp"
using namespace irr;
using namespace core;
using namespace asset;
using namespace video;

std::string getGLSL(std::vector<uint32_t>&& spv)
{
	spirv_cross::CompilerGLSL spvcross(std::move(spv));
	spirv_cross::CompilerGLSL::Options options;
	options.version = 450;
	options.vulkan_semantics = false;
	options.separate_shader_objects = true;
	spvcross.set_common_options(options);
	return spvcross.compile();
}
void writeToFile(const std::string& str, const char* fname)
{
	auto f = fopen(fname, "w");
	fwrite(str.c_str(), 1, str.size(), f);
	fclose(f);
}

int main()
{
	irr::SIrrlichtCreationParameters params;
	params.Bits = 32;
	params.ZBufferBits = 24;
	params.DriverType = video::EDT_OPENGL;
	params.WindowSize = dimension2d<uint32_t>(2, 2);
	params.Fullscreen = false;
	params.Doublebuffer = true;
	params.Vsync = true;
	params.Stencilbuffer = false;

	auto device = createDeviceEx(params);
	if (!device)
		return false;

	auto driver = device->getVideoDriver();
	auto assetManager = device->getAssetManager();


	std::string pathToShader = "../litByTriangle.frag";

	auto cpuFragmentSpecializedShader = core::smart_refctd_ptr_static_cast<asset::ICPUSpecializedShader>(
		assetManager->getAsset(pathToShader, {}).getContents().begin()[0]);
	ICPUSpecializedShader::SInfo infoCpy = cpuFragmentSpecializedShader->getSpecializationInfo();
	auto begin = reinterpret_cast<const char*>(cpuFragmentSpecializedShader->getUnspecialized()->getSPVorGLSL()->getPointer());
	std::string shadercode = begin;

	spvtools::Optimizer opt(SPV_ENV_UNIVERSAL_1_5);
	spvtools::SpirvTools spvTool(SPV_ENV_UNIVERSAL_1_5);

	auto* comp = assetManager->getGLSLCompiler();

	asset::ICPUShader::insertGLSLExtensionsDefines(shadercode, driver->getSupportedGLSLExtensions().get());
	auto glslShader_woIncludes = comp->resolveIncludeDirectives(shadercode.c_str(), irr::asset::ISpecializedShader::E_SHADER_STAGE::ESS_FRAGMENT, pathToShader.c_str());
	auto spvShader = comp->createSPIRVFromGLSL(
		reinterpret_cast<const char*>(glslShader_woIncludes->getSPVorGLSL()->getPointer()),
		irr::asset::ISpecializedShader::E_SHADER_STAGE::ESS_FRAGMENT,
		"main",
		pathToShader.c_str()
	);
	ICPUSpecializedShader::SInfo optShaderInfo;
	{
		optShaderInfo.shaderStage = irr::asset::ISpecializedShader::E_SHADER_STAGE::ESS_FRAGMENT;
		optShaderInfo.entryPoint = "main";
		optShaderInfo.m_filePathHint = pathToShader.c_str();
	}
	auto specSpirvShader = core::make_smart_refctd_ptr< asset::ICPUSpecializedShader>(std::move(spvShader), std::move(optShaderInfo));
	const uint32_t* spv_ptr = reinterpret_cast<const uint32_t*>(specSpirvShader->getUnspecialized()->getSPVorGLSL()->getPointer());
	const uint32_t spv_size = specSpirvShader->getUnspecialized()->getSPVorGLSL()->getSize() / sizeof(uint32_t);

	opt.RegisterPass(spvtools::CreateFreezeSpecConstantValuePass())
		.RegisterPass(spvtools::CreateUnifyConstantPass())
		.RegisterPass(spvtools::CreateStripDebugInfoPass());


	std::string disassembly;
	if (spvTool.Disassemble(spv_ptr,
		spv_size, &disassembly))
	{
		std::cout << disassembly;	//doesnt work, spirvSpecShader cannot be disassembled, it is not treated as binary code
	}
	else
	{
		assert(false);
	}

	std::vector<uint32_t> original_binary(spv_ptr, spv_ptr + spv_size);
	auto original = getGLSL(std::move(original_binary));
	writeToFile(original, "original.glsl");

	std::vector<uint32_t> opt_binary;
	if (!opt.Run(spv_ptr, spv_size, &opt_binary))
		std::cout << "Failed!";

	auto optimized = getGLSL(std::move(opt_binary));
	writeToFile(optimized, "optimized.glsl");

	return 0;
}
