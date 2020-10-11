#include "irr/video/CPropertyPoolHandler.h"
#include "irr/video/CPropertyPool.h"

using namespace irr;
using namespace video;

//
constexpr char* copyCsSource = R"(
layout(local_size_x=_IRR_BUILTIN_PROPERTY_COPY_GROUP_SIZE_) in;

layout(set=0,binding=0) readonly restrict buffer Indices
{
    uint elementCount[_IRR_BUILTIN_PROPERTY_COUNT_];
	int propertyDWORDsize_upDownFlag[_IRR_BUILTIN_PROPERTY_COUNT_];
    uint indexOffset[_IRR_BUILTIN_PROPERTY_COUNT_];
    uint indices[];
};


layout(set=0, binding=1) readonly restrict buffer InData
{
    uint data[];
} inBuff[_IRR_BUILTIN_PROPERTY_COUNT_];
layout(set=0, binding=2) writeonly restrict buffer OutData
{
    uint data[];
} outBuff[_IRR_BUILTIN_PROPERTY_COUNT_];


#if 0 // optimization
uint shared workgroupShared[_IRR_BUILTIN_PROPERTY_COPY_GROUP_SIZE_];
#endif


void main()
{
    const uint propID = gl_WorkGroupID.y;

	const int combinedFlag = propertyDWORDsize_upDownFlag[propID];
	const bool download = combinedFlag<0;

	const uint propDWORDs = uint(download ? (-combinedFlag):combinedFlag);
#if 0 // optimization
	const uint localIx = gl_LocalInvocationID.x;
	const uint MaxItemsToProcess = ;
	if (localIx<MaxItemsToProcess)
		workgroupShared[localIx] = indices[localIx+indexOffset[propID]];
	barrier();
	memoryBarrier();
#endif

    const uint index = gl_GlobalInvocationID.x/propDWORDs;
    if (index>=elementCount[propID])
        return;

	const uint redir = (
#if 0 //optimization
		workgroupShared[index]
#else 
		indices[index+indexOffset[propID]]
#endif
	// its equivalent to `indices[index]*propDWORDs+gl_GlobalInvocationID.x%propDWORDs`
    -index)*propDWORDs+gl_GlobalInvocationID.x;

    const uint inIndex = download ? redir:gl_GlobalInvocationID.x;
    const uint outIndex = download ? gl_GlobalInvocationID.x:redir;
	outBuff[propID].data[outIndex] = inBuff[propID].data[inIndex];
}
)";

//
CPropertyPoolHandler::CPropertyPoolHandler(IVideoDriver* driver, IGPUPipelineCache* pipelineCache) : m_driver(driver)
{
	assert(m_driver);
	const auto maxSSBO = m_driver->getMaxSSBOBindings(); // TODO: make sure not dynamic offset

	const uint32_t maxPropertiesPerPass = (maxSSBO-1u)/2u;
	m_perPropertyCountItems.reserve(maxPropertiesPerPass);
	m_tmpIndexRanges.reserve(maxPropertiesPerPass);

	const auto maxSteamingAllocations = maxPropertiesPerPass+1u;
	m_tmpAddresses.resize(maxSteamingAllocations);
	m_tmpSizes.resize(maxSteamingAllocations);
	m_alignments.resize(maxSteamingAllocations,alignof(uint32_t));

	for (uint32_t i=0u; i<maxPropertiesPerPass; i++)
	{
		const auto propCount = i+1u;
		m_perPropertyCountItems.emplace_back(m_driver,pipelineCache,propCount);
	}
}

//
CPropertyPoolHandler::transfer_result_t CPropertyPoolHandler::addProperties(const AllocationRequest* requestsBegin, const AllocationRequest* requestsEnd, const std::chrono::nanoseconds& maxWait)
{
	bool success = true;
	for (auto it=requestsBegin; it!=requestsEnd; it++)
	{
		assert(!reinterpret_cast<const TransferRequest*>(it)->download);
		success = it->pool->allocateProperties(it->outIndices.begin(),it->outIndices.end()) && success;
	}

	if (!success)
		return {false,nullptr};

	return transferProperties(reinterpret_cast<const TransferRequest*>(requestsBegin),reinterpret_cast<const TransferRequest*>(requestsEnd),maxWait);
}

//
CPropertyPoolHandler::transfer_result_t CPropertyPoolHandler::transferProperties(const TransferRequest* requestsBegin, const TransferRequest* requestsEnd, const std::chrono::nanoseconds& maxWait)
{
	const auto totalProps = std::distance(requestsBegin,requestsEnd);

	transfer_result_t retval = { true,nullptr };
	if (totalProps!=0u)
	{
		const uint32_t maxPropertiesPerPass = m_perPropertyCountItems.size();
		const auto fullPasses = totalProps/maxPropertiesPerPass;

		auto upBuff = m_driver->getDefaultUpStreamingBuffer();
		auto downBuff = m_driver->getDefaultDownStreamingBuffer();
		constexpr auto invalid_address = std::remove_reference_t<decltype(upBuff->getAllocator())>::invalid_address;
		uint8_t* upBuffPtr = reinterpret_cast<uint8_t*>(upBuff->getBufferPointer());
				
		auto maxWaitPoint = std::chrono::high_resolution_clock::now()+maxWait; // 50 us
		auto copyPass = [&](const TransferRequest* localRequests, uint32_t propertiesThisPass) -> void
		{
			const uint32_t headerSize = sizeof(uint32_t)*3u*propertiesThisPass;

			uint32_t upAllocations = 1u;
			uint32_t downAllocations = 0u;
			for (uint32_t i=0u; i<propertiesThisPass; i++)
			{
				if (localRequests[i].download)
					downAllocations++;
				else
					upAllocations++;
			}
			
			uint32_t* const upSizes = m_tmpSizes.data()+1u;
			uint32_t* const downAddresses = m_tmpAddresses.data()+upAllocations;
			uint32_t* const downSizes = m_tmpSizes.data()+upAllocations;

			// figure out the sizes to allocate
			{
				m_tmpSizes[0u] = 3u*propertiesThisPass;

				uint32_t* upSizesIt = upSizes;
				uint32_t* downSizesIt = downSizes;
				for (uint32_t i=0; i<propertiesThisPass; i++)
				{
					const auto& request = localRequests[i];
					const auto propSize = request.pool->getPropertySize(request.propertyID);
					const auto elementsByteSize = request.indices.size()*propSize;

					if (request.download)
						*(downSizesIt++) = elementsByteSize;
					else
						*(upSizesIt++) = elementsByteSize;

					m_tmpIndexRanges[i] = {request.indices,0u};
				}
#ifdef TODO
				// find slabs
				std::sort(m_tmpIndexRanges.begin(),m_tmpIndexRanges.end(),[](auto lhs, auto rhs)->bool{return lhs.begin()<rhs.begin()});
				uint32_t indexOffset = 0u;
				auto prev = m_tmpIndexRanges.begin();
				for (auto it=m_tmpIndexRanges.begin()+1u; it!=m_tmpIndexRanges.end(); it++)
				{

				}
				m_tmpSizes[0u] += indexOffset;
				m_tmpSizes[0u] *= sizeof(uint32_t);
#endif
			}

			// allocate indices and upload/allocate data
			uint32_t maxElements = 0u;
			{
				std::fill(m_tmpAddresses.begin(),m_tmpAddresses.begin()+propertiesThisPass+1u,invalid_address);
#if 0 // TODO
				upBuff->multi_alloc(maxWaitPoint,upAllocations,m_tmpAddresses.data(),m_tmpSizes.data(),m_alignments.data());
#endif
				uint8_t* indexBufferPtr = upBuffPtr+m_tmpAddresses[0u]/sizeof(uint32_t);
				// write `elementCount`
				for (uint32_t i=0; i<propertiesThisPass; i++)
					*(indexBufferPtr++) = localRequests[i].indices.size();
				// write `propertyDWORDsize_upDownFlag`
				for (uint32_t i=0; i<propertiesThisPass; i++)
				{
					const auto& request = localRequests[i];
					int32_t propSize = request.pool->getPropertySize(request.propertyID);
					propSize /= sizeof(uint32_t);
					if (request.download)
						propSize = -propSize;
					*reinterpret_cast<int32_t*>(indexBufferPtr++) = propSize;
				}
#ifdef TODO
				// write `indexOffset`
				for (uint32_t i=0; i<propertiesThisPass; i++)
					*(indexBufferPtr++) = m_transientPassData[i].indexOffset;
				// write the indices
				for (uint32_t i=0; i<distinctPools; i++)
				{
					const auto poolID = std::distance(poolsBegin,poolsLocalBegin)+i;
					const auto indexCount = indicesEnd[poolID]-indicesBegin[poolID];
					memcpy(indexBufferPtr,indicesBegin[poolID],sizeof(uint32_t)*indexCount);
					indexBufferPtr += indexCount;

					maxElements = core::max(indexCount,maxElements);
				}
#endif
				
				// upload
				auto upAddrIt = m_tmpAddresses.begin()+1;
				for (uint32_t i=0u; i<propertiesThisPass; i++)
				{
					const auto& request = localRequests[i];
					if (request.download)
						continue;
					
					if ((*upAddrIt)!=invalid_address)
					{
						size_t propSize = request.pool->getPropertySize(request.propertyID);
						memcpy(upBuffPtr+(*(upAddrIt++)),request.writeData,request.indices.size()*propSize);
					}
				}
#if 0 // TODO
				if (downAllocations)
					downBuff->multi_alloc(maxWaitPoint,downAllocations,downAddresses,downSizes,m_alignments.data());
#endif
			}

			const auto pipelineIndex = propertiesThisPass-1u;
			auto& items = m_perPropertyCountItems[pipelineIndex];
			auto pipeline = items.pipeline.get();
			m_driver->bindComputePipeline(pipeline);

			// update desc sets
			auto set = items.descriptorSetCache.getNextSet(m_driver,localRequests,m_tmpSizes[0],m_tmpAddresses.data(),downAddresses);
			if (!set)
			{
				retval.first = false;
				return;
			}

			// bind desc sets
			m_driver->bindDescriptorSets(EPBP_COMPUTE,pipeline->getLayout(),0u,1u,&set.get(),nullptr);
		
			// dispatch (this will need to change to a cmd buffer submission with a fence)
			m_driver->dispatch((maxElements+IdealWorkGroupSize-1u)/IdealWorkGroupSize,propertiesThisPass,1u);
			auto& fence = retval.second = m_driver->placeFence(true);

			// deferred release resources
			upBuff->multi_free(upAllocations,m_tmpAddresses.data(),m_tmpSizes.data(),core::smart_refctd_ptr(fence));
			if (downAllocations)
				downBuff->multi_free(downAllocations,downAddresses,downSizes,core::smart_refctd_ptr(fence));
			items.descriptorSetCache.releaseSet(core::smart_refctd_ptr(fence),std::move(set));
		};

		
		auto requests = requestsBegin;
		for (uint32_t i=0; i<fullPasses; i++)
		{
			copyPass(requests,maxPropertiesPerPass);
			requests += maxPropertiesPerPass;
		}

		const auto leftOverProps = totalProps-fullPasses*maxPropertiesPerPass;
		if (leftOverProps)
			copyPass(requests,leftOverProps);
	}

	return retval;
}


//
CPropertyPoolHandler::PerPropertyCountItems::PerPropertyCountItems(IVideoDriver* driver, IGPUPipelineCache* pipelineCache, uint32_t propertyCount) : descriptorSetCache(driver,propertyCount)
{
	std::string shaderSource("#version 440 core\n");
	// property count
	shaderSource += "#define _IRR_BUILTIN_PROPERTY_COUNT_ ";
	shaderSource += std::to_string(propertyCount)+"\n";
	// workgroup sizes
	shaderSource += "#define _IRR_BUILTIN_PROPERTY_COPY_GROUP_SIZE_ ";
	shaderSource += std::to_string(IdealWorkGroupSize)+"\n";
	//
	shaderSource += copyCsSource;

	auto cpushader = core::make_smart_refctd_ptr<asset::ICPUShader>(shaderSource.c_str());

	auto shader = driver->createGPUShader(std::move(cpushader));
	auto specshader = driver->createGPUSpecializedShader(shader.get(),{nullptr,nullptr,"main",asset::ISpecializedShader::ESS_COMPUTE});

	auto layout = driver->createGPUPipelineLayout(nullptr,nullptr,descriptorSetCache.getLayout());
	pipeline = driver->createGPUComputePipeline(pipelineCache,std::move(layout),std::move(specshader));
}


//
CPropertyPoolHandler::DescriptorSetCache::DescriptorSetCache(IVideoDriver* driver, uint32_t _propertyCount) : propertyCount(_propertyCount)
{
	IGPUDescriptorSetLayout::SBinding bindings[3];
	for (auto j=0; j<3; j++)
	{
		bindings[j].binding = j;
		bindings[j].type = asset::EDT_STORAGE_BUFFER;
		bindings[j].count = j ? propertyCount:1u;
		bindings[j].stageFlags = asset::ISpecializedShader::ESS_COMPUTE;
		bindings[j].samplers = nullptr;
	}
	layout = driver->createGPUDescriptorSetLayout(bindings,bindings+3);
	unusedSets.reserve(4u); // 4 frames worth at least
}

CPropertyPoolHandler::DescriptorSetCache::DeferredDescriptorSetReclaimer::single_poll_t CPropertyPoolHandler::DescriptorSetCache::DeferredDescriptorSetReclaimer::single_poll;
core::smart_refctd_ptr<IGPUDescriptorSet> CPropertyPoolHandler::DescriptorSetCache::getNextSet(
	IVideoDriver* driver, const TransferRequest* requests, uint32_t parameterBufferSize, const uint32_t* uploadAddresses, const uint32_t* downloadAddresses
)
{
	deferredReclaims.pollForReadyEvents(DeferredDescriptorSetReclaimer::single_poll);

	core::smart_refctd_ptr<IGPUDescriptorSet> retval;
	if (unusedSets.size())
	{
		retval = std::move(unusedSets.back());
		unusedSets.pop_back();
	}
	else
		retval = driver->createGPUDescriptorSet(core::smart_refctd_ptr(layout));


	constexpr auto kSyntheticMax = 64;
	assert(propertyCount<kSyntheticMax);
	IGPUDescriptorSet::SDescriptorInfo info[kSyntheticMax];

	IGPUDescriptorSet::SWriteDescriptorSet dsWrite[3u];
	{
		auto upBuff = driver->getDefaultUpStreamingBuffer()->getBuffer();
		auto downBuff = driver->getDefaultDownStreamingBuffer()->getBuffer();

		info[0].desc = core::smart_refctd_ptr<asset::IDescriptor>(upBuff);
		info[0].buffer = { *(uploadAddresses++),parameterBufferSize };
		for (uint32_t i=0u; i<propertyCount; i++)
		{
			const auto& request = requests[i];

			const bool download = request.download;
			
			const auto* pool = request.pool;
			const auto& poolMemBlock = pool->getMemoryBlock();

			const uint32_t propertySize = pool->getPropertySize(request.propertyID);
			const uint32_t transferPropertySize = request.indices.size()*propertySize;
			const uint32_t poolPropertyBlockSize = pool->getCapacity()*propertySize;

			auto& inDescInfo = info[i+1];
			auto& outDescInfo = info[2*i+1];
			if (download)
			{
				inDescInfo.desc = core::smart_refctd_ptr<asset::IDescriptor>(poolMemBlock.buffer);
				inDescInfo.buffer = { pool->getPropertyOffset(request.propertyID),poolPropertyBlockSize };

				outDescInfo.desc = core::smart_refctd_ptr<asset::IDescriptor>(downBuff);
				outDescInfo.buffer = { *(downloadAddresses++),transferPropertySize };
			}
			else
			{
				inDescInfo.desc = core::smart_refctd_ptr<asset::IDescriptor>(upBuff);
				inDescInfo.buffer = { *(uploadAddresses++),transferPropertySize };
					
				outDescInfo.desc = core::smart_refctd_ptr<asset::IDescriptor>(poolMemBlock.buffer);
				outDescInfo.buffer = { pool->getPropertyOffset(request.propertyID),poolPropertyBlockSize };
			}
		}
	}
	for (auto i=0u; i<3u; i++)
	{
		dsWrite[i].dstSet = retval.get();
		dsWrite[i].binding = i;
		dsWrite[i].arrayElement = 0u;
		dsWrite[i].descriptorType = asset::EDT_STORAGE_BUFFER;
	}
	dsWrite[0].count = 1u;
	dsWrite[0].info = info+0;
	dsWrite[1].count = propertyCount;
	dsWrite[1].info = info+1;
	dsWrite[2].count = propertyCount;
	dsWrite[2].info = info+1+propertyCount;
	driver->updateDescriptorSets(3u,dsWrite,0u,nullptr);

	return retval;
}

void CPropertyPoolHandler::DescriptorSetCache::releaseSet(core::smart_refctd_ptr<IDriverFence>&& fence, core::smart_refctd_ptr<IGPUDescriptorSet>&& set)
{
	deferredReclaims.addEvent(GPUEventWrapper(std::move(fence)),DeferredDescriptorSetReclaimer(this,std::move(set)));
}