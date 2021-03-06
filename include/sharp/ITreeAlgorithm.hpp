#ifndef SHARP_SHARP_ITREEALGORITHM_H_
#define SHARP_SHARP_ITREEALGORITHM_H_

#include <sharp/global>

#include <sharp/ISolution.hpp>
#include <sharp/ITable.hpp>
#include <sharp/INodeTableMap.hpp>
#include <sharp/IInstance.hpp>

#include <htd/main.hpp>

#include <vector>

namespace sharp
{
	class ITreeAlgorithm;

	typedef std::vector<const ITreeAlgorithm *> TreeAlgorithmVector;

	class SHARP_API ITreeAlgorithm
	{
	protected:
		ITreeAlgorithm &operator=(ITreeAlgorithm &) { return *this; };

	public:
		virtual ~ITreeAlgorithm() = 0;

		virtual std::vector<const htd::ILabelingFunction *>
			preprocessOperations() const = 0;

		virtual ITable *evaluateNode(
				htd::vertex_t node,
				const htd::ITreeDecomposition &decomposition,
				INodeTableMap &tables,
				const IInstance &instance) const = 0;

		virtual bool needAllTables() const = 0;

	}; // class ITreeAlgorithm

	inline ITreeAlgorithm::~ITreeAlgorithm() { }
} // namespace sharp

#endif // SHARP_SHARP_ITREEALGORITHM_H_
