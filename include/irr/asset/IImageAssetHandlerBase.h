#ifndef __IRR_I_IMAGE_ASSET_HANDLER_BASE_H_INCLUDED__
#define __IRR_I_IMAGE_ASSET_HANDLER_BASE_H_INCLUDED__

#include "irr/core/core.h"

namespace irr
{
namespace asset
{

class IImageAssetHandlerBase : public virtual core::IReferenceCounted
{
	protected:

		IImageAssetHandlerBase() = default;
		virtual ~IImageAssetHandlerBase() = 0;

	public:

		static const uint32_t MAX_PITCH_ALIGNMENT = 8u;										             

		/*
			 Returns pitch for buffer row lenght, because
			 OpenGL cannot transfer rows with arbitrary padding
		*/

		static inline uint32_t calcPitchInBlocks(uint32_t width, uint32_t blockByteSize)       
		{
			auto rowByteSize = width * blockByteSize;
			for (uint32_t _alignment = MAX_PITCH_ALIGNMENT; _alignment > 1u; _alignment >>= 1u)
			{
				auto paddedSize = core::alignUp(rowByteSize, _alignment);
				if (paddedSize % blockByteSize)
					continue;
				return paddedSize / blockByteSize;
			}
			return width;
		}

		static inline core::vector3du32_SIMD calcPitchInBlocks(uint32_t width, uint32_t height, uint32_t depth, uint32_t blockByteSize)
		{
			constexpr auto VALUE_FOR_ALIGNMENT = 1;
			core::vector3du32_SIMD retVal;
			retVal.X = calcPitchInBlocks(width, blockByteSize);
			retVal.Y = calcPitchInBlocks(height, VALUE_FOR_ALIGNMENT);
			retVal.Z = calcPitchInBlocks(depth, VALUE_FOR_ALIGNMENT);
			return retVal;
		}

		/*
			Create a new image with only one top level region, one layer and one mip-map level.

			Handling ordinary images in asset writing process is a mess since multi-regions
			are valid. To avoid ambitious, the function will handle top level data from
			image view to save only stuff a user has choosen.
		*/

		static inline core::smart_refctd_ptr<ICPUImage> getTopImageDataForCommonWriting(const ICPUImageView* imageView)
		{
			auto referenceImage = imageView->getCreationParameters().image;
			auto referenceImageParams = referenceImage->getCreationParameters();
			auto referenceRegions = referenceImage->getRegions();
			auto referenceTopRegion = referenceRegions.begin();

			core::smart_refctd_ptr<ICPUImage> newImage;
			{
				auto newImageParams = referenceImageParams;
				newImageParams.arrayLayers = 1;
				newImageParams.mipLevels = 1;
				newImageParams.type = IImage::ET_2D;

				auto newRegions = core::make_refctd_dynamic_array<core::smart_refctd_dynamic_array<ICPUImage::SBufferCopy>>(1);
				auto newTopRegion = newRegions->front();
				newTopRegion = *referenceTopRegion;
			
				const auto texelOrBlockByteSize = asset::getTexelOrBlockBytesize(referenceImageParams.format);
				const IImage::SBufferCopy::TexelBlockInfo info(referenceImageParams.format);
				auto byteStrides = newTopRegion.getByteStrides(info, texelOrBlockByteSize);
				auto texelBuffer = core::make_smart_refctd_ptr<ICPUBuffer>(byteStrides.X * byteStrides.Y * byteStrides.Z);

				newImage = ICPUImage::create(std::move(newImageParams));
				newImage->setBufferAndRegions(std::move(texelBuffer), newRegions);
			}

			using COPY_FILTER = CCopyImageFilter;
			COPY_FILTER copyFilter;
			COPY_FILTER::state_type state;

			auto newTopRegion = newImage->getRegions().begin();

			state.inImage = referenceImage.get();
			state.outImage = newImage.get();
			state.inOffset = { 0, 0, 0 };
			state.outOffset = { 0, 0, 0 };
			state.inBaseLayer = 0;
			state.outBaseLayer = 0;
			state.extent = newTopRegion->getExtent();
			state.layerCount = newTopRegion->imageSubresource.layerCount; // @devsh I wonder if it shouldn't be like *in layers* and *out layers* to let user choose how many layers have to be "touched", cause it seems to me it deals with all layers. And if it's possible with that member however, it has to be documented, unless *inBaseLayer* deals with it what means user has to launch execute per each layer as well as he does for mipmaps
			state.inMipLevel = newTopRegion->imageSubresource.mipLevel;
			state.outMipLevel = 0;

			if(!copyFilter.execute(&state))
				os::Printer::log("Something went wrong while copying top level region texel's data to the image!", ELL_WARNING);

			return newImage;
		}

		/*
			Patch for not supported by OpenGL R8_SRGB formats.
			Input image needs to have all the regions filled 
			and texel buffer attached as well.
		*/

		static inline core::smart_refctd_ptr<ICPUImage> convertR8ToR8G8B8Image(core::smart_refctd_ptr<ICPUImage> image)
		{
			constexpr auto inputFormat = EF_R8_SRGB;
			constexpr auto outputFormat = EF_R8G8B8_SRGB;

			using CONVERSION_FILTER = CConvertFormatImageFilter<inputFormat, outputFormat>;

			core::smart_refctd_ptr<ICPUImage> newConvertedImage;
			{
				auto referenceImageParams = image->getCreationParameters();
				auto referenceBuffer = image->getBuffer();
				auto referenceRegions = image->getRegions();
				auto referenceRegion = referenceRegions.begin();
				const auto newTexelOrBlockByteSize = asset::getTexelOrBlockBytesize(outputFormat);

				auto newImageParams = referenceImageParams;
				auto newCpuBuffer = core::make_smart_refctd_ptr<ICPUBuffer>(referenceBuffer->getSize() * newTexelOrBlockByteSize);
				auto newRegions = core::make_refctd_dynamic_array<core::smart_refctd_dynamic_array<ICPUImage::SBufferCopy>>(referenceRegions.size());

				for (auto newRegion = newRegions->begin(); newRegion != newRegions->end(); ++newRegion)
				{
					*newRegion = *(referenceRegion++);
					newRegion->bufferOffset = referenceRegion->bufferOffset * newTexelOrBlockByteSize;
				}

				newImageParams.format = outputFormat;
				newConvertedImage = ICPUImage::create(std::move(newImageParams));
				newConvertedImage->setBufferAndRegions(std::move(newCpuBuffer), newRegions);

				CONVERSION_FILTER convertFiler;
				CONVERSION_FILTER::state_type state;

				state.inImage = image.get();
				state.outImage = newConvertedImage.get();
				state.inOffset = { 0, 0, 0 };
				state.inBaseLayer = 0;
				state.outOffset = { 0, 0, 0 };
				state.outBaseLayer = 0;

				for (auto newAttachedRegion = newConvertedImage->getRegions().begin(); newAttachedRegion != newConvertedImage->getRegions().end(); ++newAttachedRegion)
				{
					state.extent = newAttachedRegion->getExtent();
					state.layerCount = newAttachedRegion->imageSubresource.layerCount;
					state.inMipLevel = newAttachedRegion->imageSubresource.mipLevel;
					state.outMipLevel = newAttachedRegion->imageSubresource.mipLevel;
				
					if (!convertFiler.execute(&state))
						os::Printer::log("Something went wrong while converting from R8 to R8G8B8 format!", ELL_WARNING);
				}
			}
			return newConvertedImage;
		};

	private:
};

}
}

#endif // __IRR_I_IMAGE_ASSET_HANDLER_BASE_H_INCLUDED__
