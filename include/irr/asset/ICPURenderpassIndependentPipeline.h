#ifndef __IRR_I_CPU_RENDERPASS_INDEPENDENT_PIPELINE_H_INCLUDED__
#define __IRR_I_CPU_RENDERPASS_INDEPENDENT_PIPELINE_H_INCLUDED__

#include "irr/asset/IRenderpassIndependentPipeline.h"
#include "irr/asset/ICPUSpecializedShader.h"
#include "irr/asset/ICPUPipelineLayout.h"
#include "irr/asset/IPipelineMetadata.h"

namespace irr
{
namespace asset
{

//! CPU Version of Renderpass Independent Pipeline
/*
	@see IRenderpassIndependentPipeline
*/

class ICPURenderpassIndependentPipeline : public IRenderpassIndependentPipeline<ICPUSpecializedShader, ICPUPipelineLayout>, public IAsset
{
		using base_t = IRenderpassIndependentPipeline<ICPUSpecializedShader, ICPUPipelineLayout>;

	public:
		//(TODO) it is true however it causes DSs to not be cached when ECF_DONT_CACHE_TOP_LEVEL is set which isnt really intuitive
        _IRR_STATIC_INLINE_CONSTEXPR uint32_t DESC_SET_HIERARCHYLEVELS_BELOW = 0u;
        _IRR_STATIC_INLINE_CONSTEXPR uint32_t IMAGEVIEW_HIERARCHYLEVELS_BELOW = 1u;
        _IRR_STATIC_INLINE_CONSTEXPR uint32_t IMAGE_HIERARCHYLEVELS_BELOW = 2u;


		using base_t::base_t;

		size_t conservativeSizeEstimate() const override { return sizeof(base_t); }
		void convertToDummyObject(uint32_t referenceLevelsBelowToConvert=0u) override
		{
			if (referenceLevelsBelowToConvert)
			{
                //intentionally parent is not converted
                --referenceLevelsBelowToConvert;
				m_layout->convertToDummyObject(referenceLevelsBelowToConvert);
				for (auto i=0u; i<SHADER_STAGE_COUNT; i++)
                    if (m_shaders[i])
					    m_shaders[i]->convertToDummyObject(referenceLevelsBelowToConvert);
			}
		}

		bool canBeRestoredFrom(const IAsset* _other) const override
		{
			if (!IAsset::canBeRestoredFrom(_other))
				return false;

			auto* other = static_cast<const ICPURenderpassIndependentPipeline*>(_other);
#define MEMCMP_MEMBER(m) \
			if (memcmp(&m, &other->m, sizeof(m)) != 0) \
				return false
			MEMCMP_MEMBER(m_vertexInputParams);
			MEMCMP_MEMBER(m_blendParams);
			MEMCMP_MEMBER(m_primAsmParams);
			MEMCMP_MEMBER(m_rasterParams);
#undef MECMP_MEMBER
			if (m_disableOptimizations != other->m_disableOptimizations)
				return false;

			return true;
		}

        core::smart_refctd_ptr<IAsset> clone(uint32_t _depth = ~0u) const override
        {
            core::smart_refctd_ptr<ICPUPipelineLayout> layout = (_depth > 0u && m_layout) ? core::smart_refctd_ptr_static_cast<ICPUPipelineLayout>(m_layout->clone(_depth-1u)) : m_layout;

            std::array<core::smart_refctd_ptr<ICPUSpecializedShader>, SHADER_STAGE_COUNT> shaders;
            for (uint32_t i = 0u; i < shaders.size(); ++i)
                shaders[i] = (_depth > 0u && m_shaders[i]) ? core::smart_refctd_ptr_static_cast<ICPUSpecializedShader>(m_shaders[i]->clone(_depth-1u)) : m_shaders[i];
            std::array<ICPUSpecializedShader*, SHADER_STAGE_COUNT> shaders_raw;
            for (uint32_t i = 0u; i < shaders.size(); ++i)
                shaders_raw[i] = shaders[i].get();
            std::sort(shaders_raw.begin(), shaders_raw.end(), [](ICPUSpecializedShader* a, ICPUSpecializedShader* b) { return (a && !b); });

            auto cp = core::make_smart_refctd_ptr<ICPURenderpassIndependentPipeline>(std::move(layout),
                shaders_raw.data(), &*std::find(shaders_raw.begin(), shaders_raw.end(), nullptr),
                m_vertexInputParams, m_blendParams, m_primAsmParams, m_rasterParams
            );
            clone_common(cp.get());

            return cp;
        }

		_IRR_STATIC_INLINE_CONSTEXPR auto AssetType = ET_RENDERPASS_INDEPENDENT_PIPELINE;
		inline E_TYPE getAssetType() const override { return AssetType; }

		inline ICPUPipelineLayout* getLayout() 
		{
			return m_layout.get(); 
		}
		const inline ICPUPipelineLayout* getLayout() const { return m_layout.get(); }

		inline ICPUSpecializedShader* getShaderAtStage(ISpecializedShader::E_SHADER_STAGE _stage) 
		{ 
			return m_shaders[core::findLSB<uint32_t>(_stage)].get(); 
		}
		inline ICPUSpecializedShader* getShaderAtIndex(uint32_t _ix) 
		{
			return m_shaders[_ix].get();
		}
		inline const ICPUSpecializedShader* getShaderAtIndex(uint32_t _ix) const { return m_shaders[_ix].get(); }

		inline SBlendParams& getBlendParams() 
		{
			isImmutable_debug();
			return m_blendParams;
		}
		inline const SBlendParams& getBlendParams() const { return m_blendParams; }
		inline SPrimitiveAssemblyParams& getPrimitiveAssemblyParams() 
		{
			isImmutable_debug();
			return m_primAsmParams;
		}
		inline const SPrimitiveAssemblyParams& getPrimitiveAssemblyParams() const { return m_primAsmParams; }
		inline SRasterizationParams& getRasterizationParams() 
		{
			isImmutable_debug();
			return m_rasterParams;
		}
		inline const SRasterizationParams& getRasterizationParams() const { return m_rasterParams; }
		inline SVertexInputParams& getVertexInputParams() 
		{
			isImmutable_debug();
			return m_vertexInputParams; 
		}
		inline const SVertexInputParams& getVertexInputParams() const { return m_vertexInputParams; }

		inline void setShaderAtStage(ISpecializedShader::E_SHADER_STAGE _stage, ICPUSpecializedShader* _shdr) 
		{
			if (isImmutable_debug())
				return;
			m_shaders[core::findLSB<uint32_t>(_stage)] = core::smart_refctd_ptr<ICPUSpecializedShader>(_shdr); 
		}
		inline void setShaderAtIndex(uint32_t _ix, ICPUSpecializedShader* _shdr) 
		{
			if (isImmutable_debug())
				return;
			m_shaders[_ix] = core::smart_refctd_ptr<ICPUSpecializedShader>(_shdr);
		}

		inline void setLayout(core::smart_refctd_ptr<ICPUPipelineLayout>&& _layout) 
		{
			if (isImmutable_debug())
				return;
			m_layout = std::move(_layout);
		}

	private:
		void restoreFromDummy_impl(IAsset* _other, uint32_t _levelsBelow) override
		{
			auto* other = static_cast<ICPURenderpassIndependentPipeline*>(_other);

			if (_levelsBelow)
			{
				--_levelsBelow;

				m_layout->restoreFromDummy(other->m_layout.get(), _levelsBelow);
				for (uint32_t i = 0u; i < SHADER_STAGE_COUNT; ++i)
					m_shaders[i]->restoreFromDummy(other->m_shaders[i].get(), _levelsBelow);
			}
		}

	protected:
		virtual ~ICPURenderpassIndependentPipeline() = default;
};

}
}

#endif
