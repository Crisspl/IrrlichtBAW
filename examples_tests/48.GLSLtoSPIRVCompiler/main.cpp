#include <irrlicht.h>

#include "SPIRV-Tools/include/spirv-tools/optimizer.hpp" 
#include "SPIRV-Tools/include/spirv-tools/libspirv.h"
#include <shaderc\shaderc.hpp>
using namespace irr;
using namespace core;
using namespace asset;
using namespace video;

int main()
{
	irr::SIrrlichtCreationParameters params;
	params.Bits = 32;
	params.ZBufferBits = 24;
	params.DriverType = video::EDT_OPENGL;
	params.WindowSize = dimension2d<uint32_t>(1920, 1080);
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

	spvtools::Optimizer opt(SPV_ENV_UNIVERSAL_1_3);
	spvtools::SpirvTools spvTool(SPV_ENV_UNIVERSAL_1_3);

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
	opt.RegisterPass(spvtools::CreateFreezeSpecConstantValuePass())
		.RegisterPass(spvtools::CreateUnifyConstantPass())
		.RegisterPass(spvtools::CreateStripDebugInfoPass());


	std::string disassembly;
	if (spvTool.Disassemble(reinterpret_cast<const uint32_t*>(specSpirvShader->getUnspecialized()->getSPVorGLSL()->getPointer()),
		specSpirvShader->getUnspecialized()->getSPVorGLSL()->getSize() / 4u, &disassembly))
	{
		std::cout << disassembly;	//doesnt work, spirvSpecShader cannot be disassembled, it is not treated as binary code
	}
	else
	{
		assert(false);
	}


	std::vector<uint32_t> binary;
	if (!opt.Run(reinterpret_cast<const uint32_t*>(specSpirvShader->getUnspecialized()->getSPVorGLSL()->getPointer()), specSpirvShader->getUnspecialized()->getSPVorGLSL()->getSize() / 4u, &binary))
		std::cout << "Failed!";

	return 0;
}
