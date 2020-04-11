// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CImageWriterTGA.h"

#ifdef _IRR_COMPILE_WITH_TGA_WRITER_

#include "CImageLoaderTGA.h"
#include "IWriteFile.h"
#include "irr/asset/format/convertColor.h"
#include "irr/asset/ICPUImageView.h"
#include "os.h"

#include "os.h"

namespace irr
{
namespace asset
{

CImageWriterTGA::CImageWriterTGA()
{
#ifdef _IRR_DEBUG
	setDebugName("CImageWriterTGA");
#endif
}

bool CImageWriterTGA::writeAsset(io::IWriteFile* _file, const SAssetWriteParams& _params, IAssetWriterOverride* _override)
{
    if (!_override)
        getDefaultOverride(_override);

	SAssetWriteContext ctx{ _params, _file };

	const asset::ICPUImageView* imageView = IAsset::castDown<ICPUImageView>(_params.rootAsset);
	const auto smartImage = IImageAssetHandlerBase::getTopImageDataForCommonWriting(imageView);
	const auto image = smartImage.get();

	io::IWriteFile* file = _override->getOutputFile(_file, ctx, { image, 0u });
	
	const auto& imageParams = image->getCreationParameters();
	const auto& region = image->getRegions().begin();
	auto format = imageParams.format;

	IImage::SBufferCopy::TexelBlockInfo blockInfo(format);
	core::vector3du32_SIMD trueExtent = IImage::SBufferCopy::TexelsToBlocks(region->getTexelStrides(), blockInfo);

	core::vector3d<uint32_t> dim;
	dim.X = trueExtent.X;
	dim.Y = trueExtent.Y;
	dim.Z = trueExtent.Z;

	STGAHeader imageHeader;
	imageHeader.IdLength = 0;
	imageHeader.ColorMapType = 0;
	imageHeader.ImageType = ((format == EF_R8_SRGB) || (format == EF_R8_UNORM)) ? 3 : 2;
	imageHeader.FirstEntryIndex[0] = 0;
	imageHeader.FirstEntryIndex[1] = 0;
	imageHeader.ColorMapLength = 0;
	imageHeader.ColorMapEntrySize = 0;
	imageHeader.XOrigin[0] = 0;
	imageHeader.XOrigin[1] = 0;
	imageHeader.YOrigin[0] = 0;
	imageHeader.YOrigin[1] = 0;
	imageHeader.ImageWidth = trueExtent.X;
	imageHeader.ImageHeight = trueExtent.Y;

	// top left of image is the top. the image loader needs to
	// be fixed to only swap/flip
	imageHeader.ImageDescriptor = 1;
	
	switch (format)
	{
		case asset::EF_R8G8B8A8_SRGB:
			{
				imageHeader.PixelDepth = 32;
				imageHeader.ImageDescriptor |= 8;
			}
			break;
		case asset::EF_R8G8B8_SRGB:
			{
				imageHeader.PixelDepth = 24;
				imageHeader.ImageDescriptor |= 0;
			}
			break;
		case asset::EF_A1R5G5B5_UNORM_PACK16:
			{
				imageHeader.PixelDepth = 16;
				imageHeader.ImageDescriptor |= 1;
			}
			break;
		case asset::EF_R8_SRGB:
		case asset::EF_R8_UNORM:
			{
				imageHeader.PixelDepth = 8;
				imageHeader.ImageDescriptor |= 0;
			}
			break;
		default:
			{
				os::Printer::log("Unsupported color format, operation aborted.", ELL_ERROR);
				return false;
			}
	}

	if (file->write(&imageHeader, sizeof(imageHeader)) != sizeof(imageHeader))
		return false;

	core::smart_refctd_ptr<ICPUImage> newConvertedImage;
	{
		auto copyImageForConverting = core::smart_refctd_ptr_static_cast<ICPUImage>(image->clone());
		auto copyImageParams = copyImageForConverting->getCreationParameters();
		auto copyBuffer = copyImageForConverting->getBuffer();
		auto copyRegion = copyImageForConverting->getRegions().begin();

		auto newCpuBuffer = core::make_smart_refctd_ptr<ICPUBuffer>(copyBuffer->getSize());
		memcpy(newCpuBuffer->getPointer(), copyBuffer->getPointer(), newCpuBuffer->getSize());

		auto newRegions = core::make_refctd_dynamic_array<core::smart_refctd_dynamic_array<ICPUImage::SBufferCopy>>(1u);
		ICPUImage::SBufferCopy& region = newRegions->front();
		region.imageSubresource.mipLevel = copyRegion->imageSubresource.mipLevel;
		region.imageSubresource.baseArrayLayer = copyRegion->imageSubresource.baseArrayLayer;
		region.imageSubresource.layerCount = copyRegion->imageSubresource.layerCount;
		region.bufferOffset = copyRegion->bufferOffset;
		region.bufferRowLength = copyRegion->bufferRowLength;
		region.bufferImageHeight = copyRegion->bufferImageHeight;
		region.imageOffset = copyRegion->imageOffset;
		region.imageExtent = copyRegion->imageExtent;

		CMatchedSizeInOutImageFilterCommon::state_type state;
		state.extent = imageParams.extent;

		switch (imageParams.format)
		{
			case asset::EF_R8_UNORM:
			{
				copyImageParams.format = EF_R8_SRGB;
				newConvertedImage = ICPUImage::create(std::move(copyImageParams));
				newConvertedImage->setBufferAndRegions(std::move(newCpuBuffer), newRegions);
				state.inImage = newConvertedImage.get();

				CConvertFormatImageFilter<EF_R8_UNORM, EF_R8_SRGB> convertFiler;
				convertFiler.execute(&state);
			}
			break;

			// EF_R8G8B8(A8)_SRGB would need swizzling to EF_B8G8R8(A8)_SRGB.
			case asset::EF_R8G8B8_SRGB:
			{
				copyImageParams.format = EF_B8G8R8_SRGB;
				newConvertedImage = ICPUImage::create(std::move(copyImageParams));
				newConvertedImage->setBufferAndRegions(std::move(newCpuBuffer), newRegions);
				state.inImage = newConvertedImage.get();

				CConvertFormatImageFilter<EF_R8G8B8_SRGB, EF_B8G8R8_SRGB> convertFiler;
				convertFiler.execute(&state);
			}
			break;

			case asset::EF_R8G8B8A8_SRGB:
			{
				copyImageParams.format = EF_B8G8R8A8_SRGB;
				newConvertedImage = ICPUImage::create(std::move(copyImageParams));
				newConvertedImage->setBufferAndRegions(std::move(newCpuBuffer), newRegions);
				state.inImage = newConvertedImage.get();

				CConvertFormatImageFilter<EF_R8G8B8A8_SRGB, EF_B8G8R8A8_SRGB> convertFiler;
				convertFiler.execute(&state);
			}
			break;

			default:
			{
				newConvertedImage = std::move(copyImageForConverting);
			}
			break;
		}
	}

	//// TODO AFTER converting image

	uint8_t* scan_lines = (uint8_t*)newConvertedImage->getBuffer()->getPointer();
	if (!scan_lines)
		return false;

	// size of one pixel in bits
	uint32_t pixel_size_bits = newConvertedImage->getBytesPerPixel().getIntegerApprox();

	// length of one row of the source image in bytes
	uint32_t row_stride = (pixel_size_bits * imageHeader.ImageWidth);

	// length of one output row in bytes
	int32_t row_size = ((imageHeader.PixelDepth / 8) * imageHeader.ImageWidth);

	// allocate a row do translate data into
	uint8_t* row_pointer = new uint8_t[row_size];

	uint32_t y;
	for (y = 0; y < imageHeader.ImageHeight; ++y)
	{
		switch (newConvertedImage->getCreationParameters().format) {
			case asset::EF_R8_SRGB:
			case asset::EF_B8G8R8_SRGB:
			case asset::EF_B8G8R8A8_SRGB:
			case asset::EF_A1R5G5B5_UNORM_PACK16:
			{
				memcpy(row_pointer, &scan_lines[y * row_stride], row_size);
			}
			break;
			
			default:
			{
				os::Printer::log("Unsupported color format, operation aborted.", ELL_ERROR);
				if (row_pointer) delete [] row_pointer;
				return false;
			}
		}
		
		if (file->write(row_pointer, row_size) != row_size)
			break;
	}
	
	delete [] row_pointer;
	
	STGAExtensionArea extension;
	extension.ExtensionSize = sizeof(extension);
	extension.Gamma = isSRGBFormat(format) ? ((100.0f / 30.0f) - 1.1f) : 1.0f;
	
	STGAFooter imageFooter;
	imageFooter.ExtensionOffset = _file->getPos();
	imageFooter.DeveloperOffset = 0;
	strncpy(imageFooter.Signature, "TRUEVISION-XFILE.", 18);
	
	if (file->write(&extension, sizeof(extension)) < (int32_t)sizeof(extension))
		return false;

	if (file->write(&imageFooter, sizeof(imageFooter)) < (int32_t)sizeof(imageFooter))
		return false;

	return imageHeader.ImageHeight <= y;
}

} // namespace video
} // namespace irr

#endif

