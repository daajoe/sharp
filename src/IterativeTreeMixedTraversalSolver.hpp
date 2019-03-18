#ifndef SHARP_ITERATIVETREEMIXEDSOLVER_H_
#define SHARP_ITERATIVETREEMIXEDSOLVER_H_

#include <sharp/global>

#include <sharp/ITreeSolver.hpp>
#include <sharp/ITreeAlgorithm.hpp>
#include <sharp/IInstance.hpp>
#include <sharp/ISolution.hpp>
#include <sharp/ITreeSolutionExtractor.hpp>
#include "src/IterativeTreeSolver.hpp"

#include <htd/main.hpp>

#include <memory>

namespace sharp
{
	class IterativeTreeTupleSolver;

	class SHARP_LOCAL IterativeTreeMixedTraversalSolver : public IterativeTreeSolver
	{
		friend class IterativeTreeTupleSolver;

	private:
		IterativeTreeMixedTraversalSolver(
				const htd::ITreeDecompositionAlgorithm &decomposer,
				std::vector<std::unique_ptr<const ITreeAlgorithm> > &&algorithms,
				bool deleteAlgorithms);


		IterativeTreeMixedTraversalSolver(
				const htd::ITreeDecompositionAlgorithm &decomposer,
				std::vector<std::unique_ptr<const ITreeAlgorithm> > &&algorithms,
				std::unique_ptr<const ITreeSolutionExtractor> extractor,
				bool deleteAlgorithms,
				bool deleteExtractor);

	public:
		IterativeTreeMixedTraversalSolver(
				const htd::ITreeDecompositionAlgorithm &decomposer,
				const ITreeAlgorithm &algorithm);

		IterativeTreeMixedTraversalSolver(
				const htd::ITreeDecompositionAlgorithm &decomposer,
				const ITreeAlgorithm &algorithm1,
				const ITreeAlgorithm &algorithm2);

		IterativeTreeMixedTraversalSolver(
				const htd::ITreeDecompositionAlgorithm &decomposer,
				const TreeAlgorithmVector &algorithms);

		IterativeTreeMixedTraversalSolver(
				const htd::ITreeDecompositionAlgorithm &decomposer,
				const ITreeAlgorithm &algorithm,
				const ITreeSolutionExtractor &extractor);

		IterativeTreeMixedTraversalSolver(
				const htd::ITreeDecompositionAlgorithm &decomposer,
				const ITreeAlgorithm &algorithm1,
				const ITreeAlgorithm &algorithm2,
				const ITreeSolutionExtractor &extractor);

		IterativeTreeMixedTraversalSolver(
				const htd::ITreeDecompositionAlgorithm &decomposer,
				const TreeAlgorithmVector &algorithms,
				const ITreeSolutionExtractor &extractor);

		virtual ~IterativeTreeMixedTraversalSolver();

	private:
		virtual bool evaluate(
				const htd::ITreeDecomposition &decomposition,
				const ITreeAlgorithm &algorithm,
				const IInstance &instance,
				INodeTableMap &tables) const override;

	}; // class IterativeTreeMixedTraversalSolver

} // namespace sharp

#endif // SHARP_ITERATIVETREESOLVER_H_
