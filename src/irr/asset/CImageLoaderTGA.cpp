// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CImageLoaderTGA.h"

#ifdef _IRR_COMPILE_WITH_TGA_LOADER_

#include "IReadFile.h"
#include "os.h"
#include "irr/asset/format/convertColor.h"
#include "irr/asset/ICPUImage.h"

namespace irr
{
namespace asset
{

//! loads a compressed tga.
void CImageLoaderTGA::loadCompressedImage(io::IReadFile *file, const STGAHeader& header, const uint32_t wholeSizeWithPitchInBytes, core::smart_refctd_ptr<ICPUBuffer>& bufferData) const
{
	// This was written and sent in by Jon Pry, thank you very much!
	// I only changed the formatting a little bit.
	int32_t bytesPerPixel = header.PixelDepth/8;
	int32_t imageSizeInBytes =  header.ImageHeight * header.ImageWidth * bytesPerPixel;
	bufferData = core::make_smart_refctd_ptr<ICPUBuffer>(wholeSizeWithPitchInBytes);
	auto data = reinterpret_cast<uint8_t*>(bufferData->getPointer());
	int32_t currentByte = 0;

	while(currentByte < imageSizeInBytes)
	{
		uint8_t chunkheader = 0;
		file->read(&chunkheader, sizeof(uint8_t)); // Read The Chunk's Header

		if(chunkheader < 128) // If The Chunk Is A 'RAW' Chunk
		{
			chunkheader++; // Add 1 To The Value To Get Total Number Of Raw Pixels

			file->read(&data[currentByte], bytesPerPixel * chunkheader);
			currentByte += bytesPerPixel * chunkheader;
		}
		else
		{
			// thnx to neojzs for some fixes with this code

			// If It's An RLE Header
			chunkheader -= 127; // Subtract 127 To Get Rid Of The ID Bit

			int32_t dataOffset = currentByte;
			file->read(&data[dataOffset], bytesPerPixel);

			currentByte += bytesPerPixel;

			for(int32_t counter = 1; counter < chunkheader; counter++)
			{
				for(int32_t elementCounter=0; elementCounter < bytesPerPixel; elementCounter++)
					data[currentByte + elementCounter] = data[dataOffset + elementCounter];

				currentByte += bytesPerPixel;
			}
		}
	}
}

//! returns true if the file maybe is able to be loaded by this class
bool CImageLoaderTGA::isALoadableFileFormat(io::IReadFile* _file) const
{
	if (!_file)
		return false;
	
    const size_t prevPos = _file->getPos();

	STGAFooter footer;
	memset(&footer, 0, sizeof(STGAFooter));
	_file->seek(_file->getSize() - sizeof(STGAFooter));
	_file->read(&footer, sizeof(STGAFooter));
	
	// 16 bytes for "TRUEVISION-XFILE", 17th byte is '.', and the 18th byte contains '\0'.
	if (strncmp(footer.Signature, "TRUEVISION-XFILE.", 18u) != 0)
	{
		os::Printer::log("Invalid (non-TGA) file!", ELL_ERROR);
		return false;
	}

	float gamma;

	if (footer.ExtensionOffset == 0)
	{
		os::Printer::log("Gamma information is not present! Assuming 2.333333", ELL_WARNING);
		gamma = 2.333333f;
	}
	else
	{
		STGAExtensionArea extension;
		_file->seek(footer.ExtensionOffset);
		_file->read(&extension, sizeof(STGAExtensionArea));
		
		gamma = extension.Gamma;
		
		if (gamma == 0.0f)
		{
			os::Printer::log("Gamma information is not present! Assuming 2.333333", ELL_WARNING);
			gamma = 2.333333f;
		}
		
		// TODO - pass gamma to LoadAsset()?
		// Actually I think metadata will be in used here in near future
	}
	
    _file->seek(prevPos);
	
	return true;
}

/*
	Targa formats needs two y-axis flips. The first is a flip to get the Y conforms to OpenGL coords.
	The second flip is defined from within the .tga file itself (header.ImageDescriptor & 0x20).
	if flip - perform two flips (OpenGL + Targa) = no flipping. Don't flip the image at all in that case
	if not - do an OpenGL flip
*/

template <E_FORMAT srcFormat, E_FORMAT destFormat>
void convertImageData(ICPUImage::SCreationParams& imgInfo, core::smart_refctd_ptr<ICPUImage>& newConvertedImage, core::smart_refctd_ptr<ICPUBuffer>& texelBuffer, core::smart_refctd_dynamic_array<ICPUImage::SBufferCopy>& regions, bool flip)
{
	CMatchedSizeInOutImageFilterCommon::state_type state;
	state.extent = imgInfo.extent;

	imgInfo.format = srcFormat;
	newConvertedImage = ICPUImage::create(std::move(imgInfo));
	newConvertedImage->setBufferAndRegions(std::move(texelBuffer), regions);
	state.inImage = newConvertedImage.get();

	CConvertFormatImageFilter<srcFormat, destFormat> convertFiler;
	convertFiler.execute(&state);

	bool OpenGlFlip = !flip;

	if (OpenGlFlip)
	{
		auto newFormat = newConvertedImage->getCreationParameters().format;
		IImage::SBufferCopy::TexelBlockInfo blockInfo(newFormat);
		core::vector3du32_SIMD trueExtent = IImage::SBufferCopy::TexelsToBlocks(newConvertedImage->getRegions().begin()->getTexelStrides(), blockInfo);

		auto buffer = newConvertedImage->getBuffer();
		auto out = reinterpret_cast<uint8_t*>(buffer->getPointer()) + buffer->getSize();
		auto stride = trueExtent.X * getTexelOrBlockBytesize(newFormat);

		for (uint32_t y = 0; y < trueExtent.Y; ++y)
			out -= stride;
	}
};

//! creates a surface from the file
asset::SAssetBundle CImageLoaderTGA::loadAsset(io::IReadFile* _file, const asset::IAssetLoader::SAssetLoadParams& _params, asset::IAssetLoader::IAssetLoaderOverride* _override, uint32_t _hierarchyLevel)
{
	STGAHeader header;

	core::smart_refctd_ptr<ICPUBuffer> palette = nullptr;

	_file->read(&header, sizeof(STGAHeader));

	const auto bytesPerTexel = header.PixelDepth / 8;

	// skip image identification field
	if (header.IdLength)
		_file->seek(header.IdLength, true);

	if (header.ColorMapType)
	{
		// create 32 bit palette
		palette = core::make_smart_refctd_ptr<ICPUBuffer>(header.ColorMapLength * sizeof(uint32_t));

		// read color map
		uint8_t* colorMap = _IRR_NEW_ARRAY(uint8_t, header.ColorMapEntrySize / 8 * header.ColorMapLength);
		_file->read(colorMap,header.ColorMapEntrySize/8 * header.ColorMapLength);
		
		// convert to 32-bit palette
		const void *src_container[4] = {colorMap, nullptr, nullptr, nullptr};
		switch ( header.ColorMapEntrySize )
		{
			case 16:
				convertColor<EF_A1R5G5B5_UNORM_PACK16, EF_R8G8B8A8_SRGB>(src_container, palette->getPointer(), header.ColorMapLength, 0u);   /// ITS WRONG AS WELL, GOTTA FIX IT TOMORROW
				break;
			case 24:
				convertColor<EF_B8G8R8_SRGB, EF_R8G8B8A8_SRGB>(src_container, palette->getPointer(), header.ColorMapLength, 0u);			 /// ITS WRONG AS WELL, GOTTA FIX IT TOMORROW
				break;
			case 32:
				convertColor<EF_B8G8R8A8_SRGB, EF_R8G8B8A8_SRGB>(src_container, palette->getPointer(), header.ColorMapLength, 0u);			 /// ITS WRONG AS WELL, GOTTA FIX IT TOMORROW
				break;
		}
		delete [] colorMap;
	}

	ICPUImage::SCreationParams imgInfo;
	imgInfo.type = ICPUImage::ET_2D;
	imgInfo.extent.width = header.ImageWidth;
	imgInfo.extent.height = header.ImageHeight;
	imgInfo.extent.depth = 1u;
	imgInfo.mipLevels = 1u;
	imgInfo.arrayLayers = 1u;
	imgInfo.samples = ICPUImage::ESCF_1_BIT;
	imgInfo.flags = static_cast<IImage::E_CREATE_FLAGS>(0u);

	auto regions = core::make_refctd_dynamic_array<core::smart_refctd_dynamic_array<ICPUImage::SBufferCopy>>(1u);
	core::smart_refctd_ptr<ICPUBuffer> texelBuffer = nullptr;

	ICPUImage::SBufferCopy& region = regions->front();

	region.imageSubresource.mipLevel = 0u;
	region.imageSubresource.baseArrayLayer = 0u;
	region.imageSubresource.layerCount = 1u;
	region.bufferOffset = 0u;
	region.bufferImageHeight = 0u;
	region.imageOffset = { 0u, 0u, 0u };
	region.imageExtent = imgInfo.extent;

	// read image
	size_t endBufferSize = {};
	
	switch (header.ImageType)
	{
		case 1: // Uncompressed color-mapped image
		case 2: // Uncompressed RGB image
		case 3: // Uncompressed grayscale image
			{
				region.bufferRowLength = calcPitchInBlocks(region.imageExtent.width, getTexelOrBlockBytesize(EF_R8G8B8_SRGB));
				const int32_t imageSize = endBufferSize = region.imageExtent.height * region.bufferRowLength * bytesPerTexel;
				texelBuffer = core::make_smart_refctd_ptr<ICPUBuffer>(imageSize);
				_file->read(texelBuffer->getPointer(), imageSize);
			}
			break;
		
		case 10: // Run-length encoded (RLE) true color image
		{
			region.bufferRowLength = calcPitchInBlocks(region.imageExtent.width, getTexelOrBlockBytesize(EF_A1R5G5B5_UNORM_PACK16));
			const auto bufferSize = endBufferSize = region.imageExtent.height * region.bufferRowLength * bytesPerTexel;
			loadCompressedImage(_file, header, bufferSize, texelBuffer);
			break;
		}
		
		case 0:
			{
				os::Printer::log("The given TGA doesn't have image data", _file->getFileName().c_str(), ELL_ERROR);
                return {};
			}
		
		default:
			{
				os::Printer::log("Unsupported TGA file type", _file->getFileName().c_str(), ELL_ERROR);
                return {};
			}
	}

	bool flip = (header.ImageDescriptor & 0x20) == 0;

	core::smart_refctd_ptr<ICPUImage> newConvertedImage;

	switch(header.PixelDepth)
	{
		case 8:
			{
				if (header.ImageType != 3)
				{
					os::Printer::log("Loading 8-bit non-grayscale is NOT supported.", ELL_ERROR);		
                    return {};
				}

				convertImageData<asset::EF_R8_SRGB, asset::EF_R8G8B8A8_SRGB>(imgInfo, newConvertedImage, texelBuffer, regions, flip);		
			}
			break;
		case 16:
			{
				convertImageData<asset::EF_A1R5G5B5_UNORM_PACK16, asset::EF_A1R5G5B5_UNORM_PACK16>(imgInfo, newConvertedImage, texelBuffer, regions, flip);
			}
			break;
		case 24:
			{
				convertImageData<asset::EF_B8G8R8_SRGB, asset::EF_R8G8B8_SRGB>(imgInfo, newConvertedImage, texelBuffer, regions, flip);
			}
			break;
		case 32:
			{
				convertImageData<asset::EF_B8G8R8A8_SRGB, asset::EF_R8G8B8A8_SRGB>(imgInfo, newConvertedImage, texelBuffer, regions, flip);
			}
			break;
		default:
			os::Printer::log("Unsupported TGA format", _file->getFileName().c_str(), ELL_ERROR);
			break;
	}

	core::smart_refctd_ptr<ICPUImage> image = newConvertedImage;

	if (!image)
		return {};

    return SAssetBundle({std::move(image)});
}

} // end namespace video
} // end namespace irr

#endif