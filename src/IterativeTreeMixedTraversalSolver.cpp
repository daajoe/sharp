#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include "IterativeTreeMixedTraversalSolver.hpp"

#include "NullTreeSolutionExtractor.hpp"
#include "NodeTableMap.hpp"

#include <sharp/Benchmark.hpp>
#include <htd/JoinNodeReplacementOperation.hpp>
#include <htd/TreeDecompositionFactory.hpp>
#include <htd/SemiNormalizationOperation.hpp>
#include <htd/WeakNormalizationOperation.hpp>
#include <htd/LimitChildCountOperation.hpp>
#include <htd/TreeDecompositionVerifier.hpp>
#include "ExtendedLimitChildCountOperation.hpp"
#include "HumanReadableExporter.hpp"

#include <stack>
#include <memory>
#include <cstddef>


#include <htd/main.hpp>
#include <chrono>

namespace sharp
{
	using htd::vertex_t;
	using htd::IHypergraph;
	using htd::ITreeDecomposition;
	using htd::ITreeDecompositionAlgorithm;

	using std::unique_ptr;
	using std::stack;
	using std::pair;
	using std::make_pair;
	using std::size_t;
	using std::string;
	using std::to_string;

	IterativeTreeMixedTraversalSolver::IterativeTreeMixedTraversalSolver(
			const htd::ITreeDecompositionAlgorithm &decomposer,
			std::vector<std::unique_ptr<const ITreeAlgorithm> > &&algorithms,
			bool deleteAlgorithms)
		: IterativeTreeMixedTraversalSolver(decomposer,
				std::move(algorithms),
				std::unique_ptr<const ITreeSolutionExtractor>(
					new NullTreeSolutionExtractor()),
				deleteAlgorithms,
				true)
	{ }

	IterativeTreeMixedTraversalSolver::IterativeTreeMixedTraversalSolver(
			const htd::ITreeDecompositionAlgorithm &decomposer,
			std::vector<std::unique_ptr<const ITreeAlgorithm> > &&algorithms,
			std::unique_ptr<const ITreeSolutionExtractor> extractor,
			bool deleteAlgorithms,
			bool deleteExtractor)
		: IterativeTreeSolver(decomposer, std::move(algorithms), std::move(extractor), deleteAlgorithms, deleteExtractor)
	{
	}

	IterativeTreeMixedTraversalSolver::IterativeTreeMixedTraversalSolver(
			const htd::ITreeDecompositionAlgorithm &decomposer,
			const ITreeAlgorithm &algorithm)
		: IterativeTreeMixedTraversalSolver(decomposer, { &algorithm })
	{ }

	IterativeTreeMixedTraversalSolver::IterativeTreeMixedTraversalSolver(
			const htd::ITreeDecompositionAlgorithm &decomposer,
			const ITreeAlgorithm &algorithm1,
			const ITreeAlgorithm &algorithm2)
		: IterativeTreeMixedTraversalSolver(decomposer, { &algorithm1, &algorithm2 })
	{ }

	IterativeTreeMixedTraversalSolver::IterativeTreeMixedTraversalSolver(
			const htd::ITreeDecompositionAlgorithm &decomposer,
			const TreeAlgorithmVector &algorithms)
		: IterativeTreeSolver(decomposer, algorithms) 
	{ }

	IterativeTreeMixedTraversalSolver::IterativeTreeMixedTraversalSolver(
			const htd::ITreeDecompositionAlgorithm &decomposer,
			const ITreeAlgorithm &algorithm,
			const ITreeSolutionExtractor &extractor)
		: IterativeTreeMixedTraversalSolver(decomposer, { &algorithm }, extractor)
	{ }

	IterativeTreeMixedTraversalSolver::IterativeTreeMixedTraversalSolver(
			const htd::ITreeDecompositionAlgorithm &decomposer,
			const ITreeAlgorithm &alg1,
			const ITreeAlgorithm &alg2,
			const ITreeSolutionExtractor &extractor)
		: IterativeTreeMixedTraversalSolver(decomposer, { &alg1, &alg2 }, extractor)
	{ }

	IterativeTreeMixedTraversalSolver::IterativeTreeMixedTraversalSolver(
			const htd::ITreeDecompositionAlgorithm &decomposer,
			const TreeAlgorithmVector &algorithms,
			const ITreeSolutionExtractor &extractor)
		: IterativeTreeSolver(decomposer, algorithms, extractor)
	{ }


	IterativeTreeMixedTraversalSolver::~IterativeTreeMixedTraversalSolver()
	{
	}

	bool IterativeTreeMixedTraversalSolver::evaluate(
			const ITreeDecomposition &td,
			const ITreeAlgorithm &algorithm,
			const IInstance &instance,
			INodeTableMap &tables) const
	{
	if (algorithm.isBottomUp())
	{
		return IterativeTreeSolver::evaluate(td, algorithm, instance, tables);
	}
	else {
		htd::PreOrderTreeTraversal trav;
		trav.traverse(td, [&](htd::vertex_t current, htd::vertex_t, size_t) { 
			ITable* currentTable = algorithm.evaluateNode(
											current,
											td,
											tables,
											instance);

			if(currentTable) 
				insertIntoMap(current, td, currentTable, tables, algorithm.needAllTables());
		});
		return true;
	}
	}

} // namespace sharp
