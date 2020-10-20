#ifndef __IRR_I_CPU_SPECIALIZED_SHADER_H_INCLUDED__
#define __IRR_I_CPU_SPECIALIZED_SHADER_H_INCLUDED__

#include "irr/asset/ICPUShader.h"
#include "irr/asset/ISpecializedShader.h"

namespace irr
{
namespace asset
{

//! CPU Version of Specialized Shader
/*
	@see ISpecializedShader
	@see IAsset
*/

class ICPUSpecializedShader : public IAsset, public ISpecializedShader
{
	protected:
		virtual ~ICPUSpecializedShader() = default;

	public:
		ICPUSpecializedShader(core::smart_refctd_ptr<ICPUShader>&& _unspecialized, SInfo&& _spc)
			: m_specInfo(std::move(_spc)), m_unspecialized(std::move(_unspecialized))
		{
		}

		_IRR_STATIC_INLINE_CONSTEXPR auto AssetType = IAsset::ET_SPECIALIZED_SHADER;
		IAsset::E_TYPE getAssetType() const override { return AssetType; }

		size_t conservativeSizeEstimate() const override
		{
			size_t estimate = m_specInfo.entryPoint.size()+sizeof(uint32_t);
			if (m_specInfo.getEntries())
				estimate += sizeof(void*)+sizeof(SInfo::SMapEntry)*m_specInfo.getEntries()->size();
			estimate += m_specInfo.m_filePathHint.size();
			return estimate;
		}

        core::smart_refctd_ptr<IAsset> clone(uint32_t _depth = ~0u) const override
        {
            auto info = m_specInfo;
			if (_depth > 0u && info.getEntries() && info.getBackingBuffer())
			{
				info.setEntries(core::smart_refctd_dynamic_array<SInfo::SMapEntry>(info.getEntries()),core::smart_refctd_ptr_static_cast<ICPUBuffer>(info.getBackingBuffer()->clone(_depth-1u)));
			}
            auto unspec = m_unspecialized;

            auto cp = core::make_smart_refctd_ptr<ICPUSpecializedShader>(
                (_depth > 0u && unspec) ? core::smart_refctd_ptr_static_cast<ICPUShader>(unspec->clone(_depth-1u)) : std::move(unspec),
                std::move(info)
            );
            clone_common(cp.get());

            return cp;
        }

		void convertToDummyObject(uint32_t referenceLevelsBelowToConvert=0u) override
		{
            convertToDummyObject_common(referenceLevelsBelowToConvert);

			if (referenceLevelsBelowToConvert)
			{
				//NEVER DO THIS: OpenGL backend needs this data
				//if (m_specInfo.getBackingBuffer())
					//m_specInfo.getBackingBuffer()->convertToDummyObject(referenceLevelsBelowToConvert-1u);
				m_unspecialized->convertToDummyObject(referenceLevelsBelowToConvert-1u);
			}
			if (canBeConvertedToDummy())
				m_specInfo.setEntries(nullptr,core::smart_refctd_ptr<ICPUBuffer>(m_specInfo.getBackingBuffer()));
		}

		bool canBeRestoredFrom(const IAsset* _other) const override
		{
			if (!IAsset::canBeRestoredFrom(_other))
				return false;

			auto* other = static_cast<const ICPUSpecializedShader*>(_other);
			if (m_specInfo.entryPoint != other->m_specInfo.entryPoint)
				return false;
			if (m_specInfo.m_filePathHint != other->m_specInfo.m_filePathHint)
				return false;
			if (m_specInfo.shaderStage != other->m_specInfo.shaderStage)
				return false;

			return true;
		}

		inline E_SHADER_STAGE getStage() const { return m_specInfo.shaderStage; }

		inline void setSpecializationInfo(SInfo&& specInfo) 
		{
			if (isImmutable_debug())
				return;
			m_specInfo = std::move(specInfo); 
		}
		inline const SInfo& getSpecializationInfo() const { return m_specInfo; }


		inline ICPUShader* getUnspecialized() 
		{
			return m_unspecialized.get();
		}
		inline const ICPUShader* getUnspecialized() const { return m_unspecialized.get(); }

	private:
		void restoreFromDummy_impl(IAsset* _other, uint32_t _levelsBelow) override
		{
			auto* other = static_cast<ICPUSpecializedShader*>(_other);

			std::swap(m_specInfo.m_entries, other->m_specInfo.m_entries);
			if (_levelsBelow--)
			{
				m_unspecialized->restoreFromDummy(other->m_unspecialized.get(), _levelsBelow);
			}
		}

	private:
		SInfo								m_specInfo;
		core::smart_refctd_ptr<ICPUShader>	m_unspecialized;
};

}}

#endif//__IRR_I_CPU_SPECIALIZED_SHADER_H_INCLUDED__
