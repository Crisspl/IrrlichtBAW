// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "irr/core/core.h"
#include "CImageWriterPNG.h"

#ifdef _IRR_COMPILE_WITH_PNG_WRITER_

#include "CImageLoaderPNG.h"

#include "IWriteFile.h"
#include "os.h" // for logging
#include "irr/asset/ICPUImageView.h"
#include "irr/asset/format/convertColor.h"

#ifdef _IRR_COMPILE_WITH_LIBPNG_
	#include "libpng/png.h"
#endif // _IRR_COMPILE_WITH_LIBPNG_

namespace irr
{
namespace asset
{

#ifdef _IRR_COMPILE_WITH_LIBPNG_
// PNG function for error handling
static void png_cpexcept_error(png_structp png_ptr, png_const_charp msg)
{
	os::Printer::log("PNG fatal error", msg, ELL_ERROR);
	longjmp(png_jmpbuf(png_ptr), 1);
}

// PNG function for warning handling
static void png_cpexcept_warning(png_structp png_ptr, png_const_charp msg)
{
	os::Printer::log("PNG warning", msg, ELL_WARNING);
}

// PNG function for file writing
void PNGAPI user_write_data_fcn(png_structp png_ptr, png_bytep data, png_size_t length)
{
	png_size_t check;

	io::IWriteFile* file=(io::IWriteFile*)png_get_io_ptr(png_ptr);
	check=(png_size_t) file->write((const void*)data,(uint32_t)length);

	if (check != length)
		png_error(png_ptr, "Write Error");
}
#endif // _IRR_COMPILE_WITH_LIBPNG_

CImageWriterPNG::CImageWriterPNG()
{
#ifdef _IRR_DEBUG
	setDebugName("CImageWriterPNG");
#endif
}

template<asset::E_FORMAT outFormat>
core::smart_refctd_ptr<asset::ICPUImage> getPNGConvertedOutput(const asset::ICPUImage* image, uint32_t referenceChannelCount)
{
	using CONVERSION_FILTER = asset::CConvertFormatImageFilter<asset::EF_UNKNOWN, outFormat>;

	core::smart_refctd_ptr<asset::ICPUImage> newConvertedImage;
	{
		auto referenceImageParams = image->getCreationParameters();
		auto referenceBuffer = image->getBuffer();
		auto referenceRegions = image->getRegions();
		auto referenceRegion = referenceRegions.begin();
		const auto newTexelOrBlockByteSize = asset::getTexelOrBlockBytesize(outFormat);

		asset::TexelBlockInfo referenceBlockInfo(referenceImageParams.format);
		core::vector3du32_SIMD referenceTrueExtent = referenceBlockInfo.convertTexelsToBlocks(referenceRegion->getTexelStrides());

		auto newImageParams = referenceImageParams;
		auto newCpuBuffer = core::make_smart_refctd_ptr<ICPUBuffer>(referenceTrueExtent.X * referenceTrueExtent.Y * referenceTrueExtent.Z * newTexelOrBlockByteSize);
		auto newRegions = core::make_refctd_dynamic_array<core::smart_refctd_dynamic_array<ICPUImage::SBufferCopy>>(1);
		auto newRegion = newRegions->front();
		newRegion = *referenceRegion;

		if (referenceChannelCount == 1)
			newImageParams.format = asset::EF_R8_SRGB;
		else if (referenceChannelCount == 2 || referenceChannelCount == 3)
			newImageParams.format = asset::EF_R8G8B8_SRGB;
		else
			newImageParams.format = asset::EF_R8G8B8A8_SRGB;

		newConvertedImage = ICPUImage::create(std::move(newImageParams));
		newConvertedImage->setBufferAndRegions(std::move(newCpuBuffer), newRegions);

		CONVERSION_FILTER convertFilter;
		CONVERSION_FILTER::state_type state;

		auto attachedRegion = newConvertedImage->getRegions().begin();

		state.inImage = image;
		state.outImage = newConvertedImage.get();
		state.inOffset = { 0, 0, 0 };
		state.inBaseLayer = 0;
		state.outOffset = { 0, 0, 0 };
		state.outBaseLayer = 0;
		state.extent = attachedRegion->getExtent();
		state.layerCount = attachedRegion->imageSubresource.layerCount;
		state.inMipLevel = attachedRegion->imageSubresource.mipLevel;
		state.outMipLevel = attachedRegion->imageSubresource.mipLevel;

		if (!convertFilter.execute(&state))
			os::Printer::log("Something went wrong while converting!", ELL_WARNING);

		return newConvertedImage;
	}
}

bool CImageWriterPNG::writeAsset(io::IWriteFile* _file, const SAssetWriteParams& _params, IAssetWriterOverride* _override)
{
    if (!_override)
        getDefaultOverride(_override);

#if defined(_IRR_COMPILE_WITH_LIBPNG_)

	SAssetWriteContext ctx{ _params, _file };

	const asset::ICPUImageView* imageView = IAsset::castDown<ICPUImageView>(_params.rootAsset);
	const auto smartImage = IImageAssetHandlerBase::getTopImageDataForCommonWriting(imageView);
	const auto image = smartImage.get();

    io::IWriteFile* file = _override->getOutputFile(_file, ctx, {image, 0u});

	if (!file || !image)
		return false;

	// Allocate the png write struct
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
		nullptr, (png_error_ptr)png_cpexcept_error, (png_error_ptr)png_cpexcept_warning);
	if (!png_ptr)
	{
		os::Printer::log("PNGWriter: Internal PNG create write struct failure\n", file->getFileName().c_str(), ELL_ERROR);
		return false;
	}

	// Allocate the png info struct
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		os::Printer::log("PNGWriter: Internal PNG create info struct failure\n", file->getFileName().c_str(), ELL_ERROR);
		png_destroy_write_struct(&png_ptr, nullptr);
		return false;
	}

	// for proper error handling
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return false;
	}

	core::smart_refctd_ptr<ICPUImage> convertedImage;
	{
		const auto channelCount = asset::getFormatChannelCount(image->getCreationParameters().format);
		if (channelCount == 1)
			convertedImage = getPNGConvertedOutput<asset::EF_R8_SRGB>(image, channelCount);
		else if(channelCount == 2 || channelCount == 3)
			convertedImage = getPNGConvertedOutput<asset::EF_R8G8B8_SRGB>(image, channelCount);
		else
			convertedImage = getPNGConvertedOutput<asset::EF_R8G8B8A8_SRGB>(image, channelCount);
	}
	
	const auto& convertedImageParams = convertedImage->getCreationParameters();
	const auto& convertedRegion = convertedImage->getRegions().begin();
	auto convertedFormat = convertedImageParams.format;

	asset::TexelBlockInfo blockInfo(convertedFormat);
	core::vector3du32_SIMD trueExtent = blockInfo.convertTexelsToBlocks(convertedRegion->getTexelStrides());
	
	png_set_write_fn(png_ptr, file, user_write_data_fcn, nullptr);
	
	// Set info
	switch (convertedFormat)
	{
		case asset::EF_R8G8B8_SRGB:
			png_set_IHDR(png_ptr, info_ptr,
				trueExtent.X, trueExtent.Y,
				8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
				PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
			break;
		case asset::EF_R8G8B8A8_SRGB:
			png_set_IHDR(png_ptr, info_ptr,
				trueExtent.X, trueExtent.Y,
				8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
				PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
		break;
		case asset::EF_R8_SRGB:
			png_set_IHDR(png_ptr, info_ptr,
				trueExtent.X, trueExtent.Y,
				8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
				PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
		break;
		default:
			{
				os::Printer::log("Unsupported color format, operation aborted.", ELL_ERROR);
				return false;
			}
	}

	int32_t lineWidth = trueExtent.X;
	switch (convertedFormat)
	{
		case asset::EF_R8_SRGB:
			lineWidth *= 1;
			break;
		case asset::EF_R8G8B8_SRGB:
			lineWidth *= 3;
			break;
		case asset::EF_R8G8B8A8_SRGB:
			lineWidth *= 4;
			break;
		default:
			{
				os::Printer::log("Unsupported color format, operation aborted.", ELL_ERROR);
				return false;
			}
	}
	
	uint8_t* data = (uint8_t*)convertedImage->getBuffer()->getPointer();

	constexpr uint32_t maxPNGFileHeight = 16u * 1024u; // arbitrary limit
	if (trueExtent.Y>maxPNGFileHeight)
	{
		os::Printer::log("PNGWriter: Image dimensions too big!\n", file->getFileName().c_str(), ELL_ERROR);
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return false;
	}
	
	// Create array of pointers to rows in image data
	png_bytep RowPointers[maxPNGFileHeight];
	irr::core::vector3d<uint32_t> imgSize;
	imgSize.X = trueExtent.X;
	imgSize.Y = trueExtent.Y;
	imgSize.Z = trueExtent.Z;

	// Fill array of pointers to rows in image data
	for (uint32_t i = 0; i < trueExtent.Y; ++i)
	{
		switch (convertedFormat) {
			case asset::EF_R8_SRGB: _IRR_FALLTHROUGH;
			case asset::EF_R8G8B8_SRGB: _IRR_FALLTHROUGH;
			case asset::EF_R8G8B8A8_SRGB:
				RowPointers[i] = reinterpret_cast<png_bytep>(data);
				break;
			default:
			{
				os::Printer::log("Unsupported color format, operation aborted.", ELL_ERROR);
				return false;
			}
		}
		
		data += lineWidth;
	}
	
	// for proper error handling
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return false;
	}

	png_set_rows(png_ptr, info_ptr, RowPointers);
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);

	png_destroy_write_struct(&png_ptr, &info_ptr);
	return true;
#else
	_IRR_DEBUG_BREAK_IF(true);
	return false;
#endif//defined(_IRR_COMPILE_WITH_LIBPNG_)
}

} // namespace video
} // namespace irr

#endif

