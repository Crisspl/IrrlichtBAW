// Copyright (C) 2020- Mateusz 'DevSH' Kielan
// This file is part of the "IrrlichtBAW" engine.
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_C_MATCHED_SIZE_IN_OUT_IMAGE_FILTER_COMMON_H_INCLUDED__
#define __IRR_C_MATCHED_SIZE_IN_OUT_IMAGE_FILTER_COMMON_H_INCLUDED__

#include "irr/core/core.h"

#include "irr/asset/filters/CBasicImageFilterCommon.h"

namespace irr
{
namespace asset
{

/*
	Common filter for images where input and desired output
	image data is known. The filter can execute various
	converting actions on input image to get an output
	image that will be a converted and ready to use one.
*/

class CMatchedSizeInOutImageFilterCommon : public CBasicImageFilterCommon
{
	public:

		/*
			To make use of the filter, it's \bstate\b must be provided.
			State contains information about input image and an image
			that will be converted as an output, but it's reference
			is a single choosen region data. In that case you are
			able to convert image with various layers and faces, but
			keep in mind that for certain mipmaps you will have to change 
			appropriate state fields to make it work.
		*/

		class CState : public IImageFilter::IState
		{
			public:
				CState()
				{
					extentLayerCount = core::vectorSIMDu32();
					inOffsetBaseLayer = core::vectorSIMDu32();
					outOffsetBaseLayer = core::vectorSIMDu32();
				}
				virtual ~CState() {}

				/*
					Wrapped extent and layer count to an union.
					You can fill image extent and layer count
					separately or fill it at once by \bextentLayerCount\b,
					where in it \bx, y, z\b is treated as extent, and a \bw\b 
					is treated as layerCount. You can use it interchangeably.

					Pay attention output image must be prepared to conversion
					process. It means it's a user resposibility to take care
					of attached regions and texel buffer with new adjusted size
					for executing the process for output image.
				*/
	
				union
				{
					core::vectorSIMDu32 extentLayerCount;
					struct
					{
						VkExtent3D		extent;
						uint32_t		layerCount;
					};
				};

				/*
					Wrapped inOffset and inBaseLayer to an union.
					You can fill in offset and in base layer
					separately or fill it at once by \binOffsetBaseLayer\b,
					where in it \bx, y, z\b is treated as inOffset, and a \bw\b
					is treated as inBaseLayer. You can use it interchangeably.
				*/

				union
				{
					core::vectorSIMDu32 inOffsetBaseLayer;
					struct
					{
						VkOffset3D		inOffset;
						uint32_t		inBaseLayer;
					};
				};

				/*
					Wrapped outOffset and outBaseLayer to an union.
					You can fill out offset and out base layer
					separately or fill it at once by \boutOffsetBaseLayer\b,
					where in it \bx, y, z\b is treated as outOffset, and a \bw\b
					is treated as outBaseLayer. You can use it interchangeably.
				*/

				union
				{
					core::vectorSIMDu32 outOffsetBaseLayer;
					struct
					{
						VkOffset3D		outOffset;
						uint32_t		outBaseLayer;
					};
				};

				uint32_t				inMipLevel = 0u;		//!<< Current handled mipmap level in reference to \binput\b image
				uint32_t				outMipLevel = 0u;		//!<< Current handled mipmap level in reference to \boutput\b image
				const ICPUImage*		inImage = nullptr;		//!<< \bInput\b image being a reference for state management
				ICPUImage*				outImage = nullptr;		//!<< \bOutput\b converted image which memory will be converted according to state data after execute call
		};
		using state_type = CState;
		
		static inline bool validate(state_type* state)
		{
			if (!state)
				return false;

			IImage::SSubresourceLayers subresource = {static_cast<IImage::E_ASPECT_FLAGS>(0u),state->inMipLevel,state->inBaseLayer,state->layerCount};
			state_type::TexelRange range = {state->inOffset,state->extent};
			if (!CBasicImageFilterCommon::validateSubresourceAndRange(subresource,range,state->inImage))
				return false;
			subresource.mipLevel = state->outMipLevel;
			subresource.baseArrayLayer = state->outBaseLayer;
			range.offset = state->outOffset;
			if (!CBasicImageFilterCommon::validateSubresourceAndRange(subresource,range,state->outImage))
				return false;

			// TODO: remove this later when we can actually write/encode to block formats
			if (isBlockCompressionFormat(state->outImage->getCreationParameters().format))
				return false;

			return true;
		}

	protected:
		virtual ~CMatchedSizeInOutImageFilterCommon() = 0;
		
		struct CommonExecuteData
		{
			const ICPUImage* const inImg;
			ICPUImage* const outImg;
			const ICPUImage::SCreationParams& inParams;
			const ICPUImage::SCreationParams& outParams;
			const E_FORMAT inFormat;
			const E_FORMAT outFormat;
			const uint32_t inBlockByteSize;
			const uint32_t outBlockByteSize;
			const uint8_t* const inData;
			uint8_t* const outData;
			const core::SRange<const IImage::SBufferCopy> inRegions;
			const core::SRange<const IImage::SBufferCopy> outRegions;
			const IImage::SBufferCopy* oit;
			core::vectorSIMDu32 offsetDifference, outByteStrides;
		};
		template<typename PerOutputFunctor>
		static inline bool commonExecute(state_type* state, PerOutputFunctor& perOutput)
		{
			if (!validate(state))
				return false;

			const auto* const inImg = state->inImage;
			auto* const outImg = state->outImage;
			const ICPUImage::SCreationParams& inParams = inImg->getCreationParameters();
			const ICPUImage::SCreationParams& outParams = outImg->getCreationParameters();
			const core::SRange<const IImage::SBufferCopy> outRegions = outImg->getRegions(state->outMipLevel);
			CommonExecuteData commonExecuteData =
			{
				inImg,
				outImg,
				inParams,
				outParams,
				inParams.format,
				outParams.format,
				getTexelOrBlockBytesize(inParams.format),
				getTexelOrBlockBytesize(outParams.format),
				reinterpret_cast<const uint8_t*>(inImg->getBuffer()->getPointer()),
				reinterpret_cast<uint8_t*>(outImg->getBuffer()->getPointer()),
				inImg->getRegions(state->inMipLevel),
				outRegions,
				outRegions.begin(), {}, {}
			};

			// iterate over output regions, then input cause read cache miss is faster
			for (; commonExecuteData.oit!=commonExecuteData.outRegions.end(); commonExecuteData.oit++)
			{
				IImage::SSubresourceLayers subresource = {static_cast<IImage::E_ASPECT_FLAGS>(0u),state->inMipLevel,state->inBaseLayer,state->layerCount};
				state_type::TexelRange range = {state->inOffset,state->extent};
				CBasicImageFilterCommon::clip_region_functor_t clip(subresource,range,commonExecuteData.outFormat);
				// setup convert state
				// I know my two's complement wraparound well enough to make this work
				const auto& outRegionOffset = commonExecuteData.oit->imageOffset;
				commonExecuteData.offsetDifference = state->outOffsetBaseLayer-(core::vectorSIMDu32(outRegionOffset.x,outRegionOffset.y,outRegionOffset.z,commonExecuteData.oit->imageSubresource.baseArrayLayer)+state->inOffsetBaseLayer);
				commonExecuteData.outByteStrides = commonExecuteData.oit->getByteStrides(IImage::SBufferCopy::TexelBlockInfo(commonExecuteData.outFormat),getTexelOrBlockBytesize(commonExecuteData.outFormat));
				if (!perOutput(commonExecuteData,clip))
					return false;
			}

			return true;
		}
};

} // end namespace asset
} // end namespace irr

#endif