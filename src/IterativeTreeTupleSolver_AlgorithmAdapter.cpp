#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include "IterativeTreeTupleSolver.hpp"

#include "TupleSet.hpp"

namespace sharp
{
	using htd::vertex_t;
	using htd::ITreeDecomposition;
	using htd::ILabelingFunction;

	IterativeTreeTupleSolver::
	TupleToTreeAlgorithmAdapter::TupleToTreeAlgorithmAdapter(
			const ITreeTupleAlgorithm &algorithm)
		: algorithm_(algorithm)
	{ }

	IterativeTreeTupleSolver::
	TupleToTreeAlgorithmAdapter::~TupleToTreeAlgorithmAdapter() { }

	std::vector<const ILabelingFunction *>
	IterativeTreeTupleSolver::
	TupleToTreeAlgorithmAdapter::preprocessOperations() const
	{
		return algorithm_.preprocessOperations();
	}

	ITable *
	IterativeTreeTupleSolver::
	TupleToTreeAlgorithmAdapter::evaluateNode(
			vertex_t node,
			const ITreeDecomposition &decomposition,
			INodeTableMap &tables,
			const IInstance &instance) const
	{
		INodeTupleSetMap& tab = dynamic_cast<INodeTupleSetMap &>(tables);
		ITupleSet *newTable = tables.contains(node) ? &tab[node] : new TupleSet();

		algorithm_.evaluateNode(
				node,
				decomposition,
				tab,
				instance,
				*newTable);

		//TODO: sometimes required?
		/*if(newTable->size() == 0)
		{
			delete newTable;
			return nullptr;
		}*/
		return newTable;
	}

	bool
	IterativeTreeTupleSolver::
	TupleToTreeAlgorithmAdapter::needAllTables() const
	{
		return algorithm_.needAllTupleSets();
	}

} // namespace sharp
