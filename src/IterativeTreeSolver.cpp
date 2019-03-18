#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include "IterativeTreeSolver.hpp"

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
#include <sstream>
#include <iostream>
#include <fstream>


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

	IterativeTreeSolver::IterativeTreeSolver(
			const htd::ITreeDecompositionAlgorithm &decomposer,
			std::vector<std::unique_ptr<const ITreeAlgorithm> > &&algorithms,
			bool deleteAlgorithms)
		: IterativeTreeSolver(decomposer,
				std::move(algorithms),
				std::unique_ptr<const ITreeSolutionExtractor>(
					new NullTreeSolutionExtractor()),
				deleteAlgorithms,
				true)
	{ }

	IterativeTreeSolver::IterativeTreeSolver(
			const htd::ITreeDecompositionAlgorithm &decomposer,
			std::vector<std::unique_ptr<const ITreeAlgorithm> > &&algorithms,
			std::unique_ptr<const ITreeSolutionExtractor> extractor,
			bool deleteAlgorithms,
			bool deleteExtractor)
		: decomposer_(decomposer),
		  extractor_(extractor.get()),
		  manageAlgorithmMemory_(deleteAlgorithms),
		  manageExtractorMemory_(deleteExtractor)
	{
		for(std::unique_ptr<const ITreeAlgorithm> &alg : algorithms)
			algorithms_.push_back(alg.get());

		for(std::unique_ptr<const ITreeAlgorithm> &alg : algorithms)
			alg.release();
		extractor.release();
	}

	IterativeTreeSolver::IterativeTreeSolver(
			const htd::ITreeDecompositionAlgorithm &decomposer,
			const ITreeAlgorithm &algorithm)
		: IterativeTreeSolver(decomposer, { &algorithm })
	{ }

	IterativeTreeSolver::IterativeTreeSolver(
			const htd::ITreeDecompositionAlgorithm &decomposer,
			const ITreeAlgorithm &algorithm1,
			const ITreeAlgorithm &algorithm2)
		: IterativeTreeSolver(decomposer, { &algorithm1, &algorithm2 })
	{ }

	IterativeTreeSolver::IterativeTreeSolver(
			const htd::ITreeDecompositionAlgorithm &decomposer,
			const TreeAlgorithmVector &algorithms)
		: decomposer_(decomposer),
		  algorithms_(algorithms),
		  extractor_(new NullTreeSolutionExtractor()),
		  manageAlgorithmMemory_(false),
		  manageExtractorMemory_(true)
	{ }

	IterativeTreeSolver::IterativeTreeSolver(
			const htd::ITreeDecompositionAlgorithm &decomposer,
			const ITreeAlgorithm &algorithm,
			const ITreeSolutionExtractor &extractor)
		: IterativeTreeSolver(decomposer, { &algorithm }, extractor)
	{ }

	IterativeTreeSolver::IterativeTreeSolver(
			const htd::ITreeDecompositionAlgorithm &decomposer,
			const ITreeAlgorithm &alg1,
			const ITreeAlgorithm &alg2,
			const ITreeSolutionExtractor &extractor)
		: IterativeTreeSolver(decomposer, { &alg1, &alg2 }, extractor)
	{ }

	IterativeTreeSolver::IterativeTreeSolver(
			const htd::ITreeDecompositionAlgorithm &decomposer,
			const TreeAlgorithmVector &algorithms,
			const ITreeSolutionExtractor &extractor)
		: decomposer_(decomposer),
		  algorithms_(algorithms),
		  extractor_(&extractor),
		  manageAlgorithmMemory_(false),
		  manageExtractorMemory_(false)
	{ }

	void IterativeTreeSolver::tree_decomp_output(const htd::ITreeDecomposition &td, htd::vertex_t currNode, std::ofstream& f)
	{
		f << "currentNode(" << currNode << ")." << std::endl;
		if (currNode == td.root())
			f << "final." << std::endl;
		/*else if (td.childCount(currNode) == 0)
			f << "initial." << std::endl;*/
		//for (unsigned int i = 0; i < td.bagSize(currNode); ++i)
		{
			std::vector<vertex_t> content = td.bagContent(currNode);
			for (const auto &t : content)
				f << "current(" << t << ")." << std::endl;
			htd::ConstCollection<vertex_t> rcontent = td.rememberedVertices(currNode);
			/*for (const auto &t : rcontent)
				f << "-introduced(" << t << ")." << std::endl;*/
			htd::ConstCollection<vertex_t> icontent = td.introducedVertices(currNode);
			for (const auto &t : icontent)
				f << "introduced(" << t << ")." << std::endl;
			/*htd::ConstCollection<vertex_t> fcontent = td.forgottenVertices(currNode);
			for (const auto &t : fcontent)
				f << "forgotten(" << t << ")." << std::endl;*/


			//std::cout << td.bagContent(currNode) << std::endl;
		}
		
		for (unsigned int i = 0; i < td.childCount(currNode); ++i)
			f << "childNode(" //<< currNode << "," 
						    << td.childAtPosition(currNode, i) << ")." << std::endl;
		for (unsigned int i = 0; i < td.childCount(currNode); ++i)
			tree_decomp_output(td, td.childAtPosition(currNode, i), f);
	}

	IterativeTreeSolver::~IterativeTreeSolver()
	{
		std::cout << "exitSolver" << manageAlgorithmMemory_ << std::endl;
		if(manageAlgorithmMemory_)
			for(const ITreeAlgorithm *alg : algorithms_)
				if(alg)
					delete alg;

		if(manageExtractorMemory_ && extractor_) delete extractor_;
	}

	
	/**
	 *  Sample fitness function which minimizes width and height of the decomposition.
	 *
	 *  Width is of higher priority than height, i.e., at first, the width is minimized
	 *  and if two decompositions have the same width, the one of lower height is chosen.
	 */
	class FitnessFunction : public htd::ITreeDecompositionFitnessFunction
	{
		public:
			FitnessFunction(const ITreeDecompositionEvaluator * eval) : m_eval(eval)
			{

			}

			~FitnessFunction()
			{

			}

			htd::FitnessEvaluation * fitness(const htd::IMultiHypergraph & graph, const htd::ITreeDecomposition & td) const
			{
				//HTD_UNUSED(graph)
				/*int fitness = 0;
				htd::PostOrderTreeTraversal fixJoin;
				fixJoin.traverse(td, [&](htd::vertex_t v, htd::vertex_t, size_t) {
					if (v != htd::Vertex::UNKNOWN && td.childCount(v) == 2)
					{
						fitness += td.bagContent(td.childAtPosition(v,0)).size() * td.bagContent(td.childAtPosition(v,1)).size();
					}
				});

				//
				// Here we specify the fitness evaluation for a given decomposition. In this case, we select the maximum bag size and the height.
				
				return new htd::FitnessEvaluation(1, -(double)(fitness));*/
				//std::cout << "fitness " << m_eval << std::endl;
				return m_eval->evaluate(graph, td);
			}

			FitnessFunction * clone(void) const
			{
				//std::cout << "clone " << std::endl;
				//TODO: really needed?!
				//return nullptr;
				return new FitnessFunction(m_eval);
			}
		private:
			const ITreeDecompositionEvaluator* m_eval;
	};


//#define ITERATIVE_TD_IMPROVEMENT

	/*template <class T>
	void insertSorted(std::vector<T>& target, const std::vector<T>& source)
	{
		for (const auto& el : source)
		{
			auto it = std::upper_bound(target.cbegin(), target.cend(), el);
			target.insert(it, el);
		}
	}*/


	#define HEUR_OUTPUT

	ITreeDecomposition *IterativeTreeSolver::decompose(
			const IInstance &instance, bool weak, unsigned int maxChilds, unsigned optimizedTD, const ITreeDecompositionEvaluator* eval) const
	{
		htd::LibraryInstance* inst = htd::createManagementInstance(0);
        	htd::ITreeDecomposition * td = nullptr;
		unique_ptr<IHypergraph> hg(instance.toHypergraph());
		//TODO: include preprocessingOperations in decomposition call
		//		once libhtd supports this
	
		#ifdef HEUR_OUTPUT
			if (Benchmark::isOrdering() == 2)
			{
 				//htd::IOrderingAlgorithm* ordering = new htd::MinFillOrderingAlgorithm(inst);
 				htd::IOrderingAlgorithm* ordering = new htd::MaximumCardinalitySearchOrderingAlgorithm(inst);
				htd::IVertexOrdering* od = ordering->computeOrdering(*hg.get());
				std::vector<htd::vertex_t> ordr = od->sequence();
			        delete od;
			        delete ordering;
				std::ofstream outf;
				std::stringstream file;
				file << Benchmark::folder() << "/g.inp.dynasp." << Benchmark::id();
				outf.open(file.str().c_str());
				unsigned int i = ordr.size();
				for (const auto& v : ordr)
					outf << "_heuristic(" << v << "," << i-- << ")." << std::endl;
				outf.close();
			}
		#endif



		//#ifdef ITERATIVE_TD_IMPROVEMENT
		if (optimizedTD)
		{
			//FitnessFunction* fitnessFunction = new FitnessFunction(eval);
			
			/**
			 *  This operation changes the root of a given decomposition so that the fitness function is maximized.
			 *
			 *  When no fitness function is provided, the optimization operation does not perform any optimization and only applies provided manipulations.
			 */
			//htd::TreeDecompositionOptimizationOperation * operation = new htd::TreeDecompositionOptimizationOperation(inst, fitnessFunction);

			/**
			 *  Set the vertex selections strategy (default = exhaustive).
			 *
			 *  In this case, we want to select (at most) 10 vertices of the input decomposition randomly.
			 */
			//operation->setVertexSelectionStrategy(new htd::RandomVertexSelectionStrategy(10));

			/**
			  * Set desired manipulations like in any other decomposition algorithm.
			  */
			
			/**
			  * In case one uses htd::IterativeImprovementTreeDecompositionAlgorithm,
			  * I recommend to use htd::MinFillOrderingAlgorithm() as it allows for
			  * more randomness.
			  */
			//htd::OrderingAlgorithmFactory::instance().setConstructionTemplate(new htd::MinFillOrderingAlgorithm());
			htd::OrderingAlgorithmFactory(inst).setConstructionTemplate(new htd::MinFillOrderingAlgorithm(inst));


					/**
			 *  Get the default tree decomposition algorithm.
			 */
			htd::ITreeDecompositionAlgorithm * baseAlgorithm = inst->treeDecompositionAlgorithmFactory().createInstance();//getTreeDecompositionAlgorithm();
			//htd::ITreeDecompositionAlgorithm * baseAlgorithm = htd::TreeDecompositionAlgorithmFactory::instance().getTreeDecompositionAlgorithm();

			/**
			 *  Set the optimization operation as manipulation operation.
			 */
			//baseAlgorithm->addManipulationOperation(operation);

			//htd::WeakNormalizationOperation js(inst);
			//htd::LimitChildCountOperation js2(inst, maxChilds);
			//htd::ExtendedLimitChildCountOperation js3(inst, maxChilds);
			//htd::SemiNormalizationOperation js2;

			if (!weak && maxChilds == 2)
				baseAlgorithm->addManipulationOperation(new htd::ExtendedLimitChildCountOperation(inst, maxChilds));
				//js3.apply(*hg, inst->treeDecompositionFactory().accessMutableInstance(*td));
			else if (maxChilds >= 2)
				baseAlgorithm->addManipulationOperation(new htd::LimitChildCountOperation(inst, maxChilds));
				///js2.apply(*hg, inst->treeDecompositionFactory().accessMutableInstance(*td));
				//js2.apply(*hg, htd::TreeDecompositionFactory::instance().accessMutableTreeDecomposition(*td));
			if (weak)
				baseAlgorithm->addManipulationOperation(new htd::WeakNormalizationOperation(inst));
				//js.apply(*hg, inst->treeDecompositionFactory().accessMutableInstance(*td));

			/*if (maxChilds >= 2)
				operation->addManipulationOperation(new htd::LimitChildCountOperation(inst, maxChilds));
			if (weak) 
				operation->addManipulationOperation(new htd::WeakNormalizationOperation(inst));*/

			//operation->addManipulationOperation(new htd::NormalizationOperation(true, true, false, false));


			/**
			 *  Create a new instance of htd::IterativeImprovementTreeDecompositionAlgorithm based on the base algorithm and the fitness function.
			 *
			 *  Note that the fitness function can be an arbiraty one and can differ from the one used in the optimization operation.
			 */
			htd::IterativeImprovementTreeDecompositionAlgorithm algorithm(inst, baseAlgorithm, new FitnessFunction(eval));

			/**
			 *  Set the maximum number of iterations after which the best decomposition with respect to the fitness function shall be returned.
			 *
			 *  Use value 1 to make the iterative algorithm return the first decomposition found.
			 */
			algorithm.setIterationCount(250); //10);

			/**
			 *  Set the maximum number of iterations without improvement after which the algorithm returns best decomposition with respect to
			 *  the fitness function found so far. A limit of 0 aborts the algorithm after the first non-improving solution has been found,
			 *  i.e. the algorithm will perform a simple hill-climbing approach.
			 */
			algorithm.setNonImprovementLimit(250); //5);

			std::size_t optimalBagSize = (std::size_t)-1;

			/**
			 *  Compute the decomposition. Note that the additional, optional parameter for computeDecomposition() in case
			 *  of htd::IterativeImprovementTreeDecompositionAlgorithm can be used to intercept every new decomposition.
			 *  In this case we output some intermediate information upon perceiving a new best width.
			 */
			 //std::cout << "decompose"  << eval << std::endl;
			td =
			    algorithm.computeDecomposition(*hg, [&](const htd::IMultiHypergraph & graph, const htd::ITreeDecomposition & decomposition, const htd::FitnessEvaluation & fitness)
			{
			    //std::cout << "inside decompose" << std::endl;
			    HTD_UNUSED(graph)
			    HTD_UNUSED(decomposition)

			    std::size_t bagSize = -fitness.at(0);

			    if (bagSize < optimalBagSize)
			    {
				optimalBagSize = bagSize;

				std::chrono::milliseconds::rep msSinceEpoch =
				    std::chrono::duration_cast<std::chrono::milliseconds>
					    (std::chrono::system_clock::now().time_since_epoch()).count();
					
				eval->select();

				std::cout << &decomposition << " c status " << optimalBagSize << " " << msSinceEpoch << std::endl;
			    }
			});
			//std::cout << "post decompose" << std::endl;
			if (inst->isTerminated() && !algorithm.isSafelyInterruptible())
			{
				delete td;
				td = nullptr;
			}
		}	
		else {
			td = decomposer_.computeDecomposition(*hg);

			//#define PRINT_DECOMP
			#ifdef PRINT_DECOMP
			htd::PreOrderTreeTraversal traversal;
			traversal.traverse(*td, [&](htd::vertex_t v, htd::vertex_t v2, size_t s){ std::cout << v << "[" << v2 << "]" << " @" << s << ": " << td->bagContent(v) << std::endl; });
			#endif

			//TODO: remove joinnodereplacementoperation
			//htd::JoinNodeReplacementOperation j;
			//j.apply(htd::TreeDecompositionFactory::instance().accessMutableTreeDecomposition(*td));
		
			htd::WeakNormalizationOperation js(inst);
			htd::LimitChildCountOperation js2(inst, maxChilds);
			htd::ExtendedLimitChildCountOperation js3(inst, maxChilds);
			//htd::SemiNormalizationOperation js2;

			if (!weak && maxChilds == 2)
				js3.apply(*hg, inst->treeDecompositionFactory().accessMutableInstance(*td));
			else if (maxChilds >= 2)
				js2.apply(*hg, inst->treeDecompositionFactory().accessMutableInstance(*td));
				//js2.apply(*hg, htd::TreeDecompositionFactory::instance().accessMutableTreeDecomposition(*td));
			if (weak)
				js.apply(*hg, inst->treeDecompositionFactory().accessMutableInstance(*td));
				//js.apply(*hg, htd::TreeDecompositionFactory::instance().accessMutableTreeDecomposition(*td));
		
			/*if (!weak && maxChilds == 2)	//TODO: manage induced edges
			{
				traversal.traverse(*td, [&](htd::vertex_t v, htd::vertex_t v2, size_t s){ std::cout << v << "[" << v2 << "]" << " @" << s << ": " << td->bagContent(v) << std::endl; });
				auto& tdm = htd::TreeDecompositionFactory(inst).accessMutableTreeDecomposition(*td);
				htd::PostOrderTreeTraversal fixJoin;
				std::vector<htd::vertex_t> bagNotFound, bag;
				std::unordered_set<htd::vertex_t> joinSeen, done;
				htd::vertex_t rememb = htd::Vertex::UNKNOWN;
				bool root = false, init = false;
				fixJoin.traverse(*td, [&](htd::vertex_t v, htd::vertex_t v2, size_t) {
					assert(!root);
					if ((rememb != htd::Vertex::UNKNOWN) || (v2 != htd::Vertex::UNKNOWN && td->childCount(v2) == 2 && !(init = (bag == td->bagContent(v2))) && bagNotFound.size() > 0))	//new JOIN node
					{
						if (rememb == htd::Vertex::UNKNOWN)
							rememb = v2;

						std::cout << "insert node of parent " << rememb << std::endl;
						//add introduce node
						htd::vertex_t c = tdm.addChild(rememb);
						tdm.mutableBagContent(c) = bag; //tdm.bagContent(td->childAtPosition(rememb, 0));
						//TODO: set induced edges (previously fetched)
						//insert sorted
						//insertSorted(tdm.mutableBagContent(c), bagNotFound);
						tdm.setParent(td->childAtPosition(rememb, joinSeen.count(td->childAtPosition(rememb, 0)) == 1  ? 0 : 1), c);
						
						bagNotFound.clear();
						init = false;
						rememb = htd::Vertex::UNKNOWN;
					}
					if (v2 != htd::Vertex::UNKNOWN && td->childCount(v2) == 2)
					{
						assert(rememb == htd::Vertex::UNKNOWN);
						std::vector<htd::vertex_t> bagTmp = bagNotFound;
						if (!init) // && td->childAtPosition(v2, 1) == v && !init)	//first child of first join node chain
						{
							init = true;
							bagTmp = td->bagContent(v2);
							bag = bagTmp;
							//TODO: fetch induced edges
						}
						std::cout << "backTmp for " << v << " of join " << v2 << " with result " << bagTmp << std::endl;
						if (bagTmp.size() > 0)
						{
							const auto &bag2 = td->bagContent(v);
							bagNotFound.clear();
							std::set_difference(bagTmp.begin(), bagTmp.end(), bag2.begin(), bag2.end(), std::back_inserter(bagNotFound));
							std::cout << "back inserting for " << v << " of join " << v2 << " with result " << bagNotFound << std::endl;
						}
						if ((done.count(td->childAtPosition(v2, 0)) || done.count(td->childAtPosition(v2, 1))) && bagNotFound.size() > 0)	//end child
						{
							joinSeen.insert(v2);
							bagTmp.clear();
							const auto &bag = td->bagContent(v2);
							std::set_difference(bag.begin(), bag.end(), bagNotFound.begin(), bagNotFound.end(), std::back_inserter(bagTmp));
							std::cout << "set content of " << v2 << " to " << bagTmp << std::endl;
							tdm.mutableBagContent(v2) = bagTmp;
							//TODO: restrict induced edges
							done.erase(td->childAtPosition(v2, 0));
							done.erase(td->childAtPosition(v2, 1));
						}
						else
							done.insert(v);
					}
					else if (v2 != htd::Vertex::UNKNOWN && rememb == htd::Vertex::UNKNOWN && td->childCount(v2) == 1 && bagNotFound.size() > 0)
					{
						rememb = v2;
					}
					else if (v2 == htd::Vertex::UNKNOWN && td->childCount(v) == 2 && bagNotFound.size() > 0)	//ROOT case
					{
						std::cout << "insert new parent for old root " << v << std::endl;
						htd::vertex_t c = tdm.addParent(v);
						tdm.mutableBagContent(c) = bag; //td->bagContent(v);
						//insertSorted(tdm.mutableBagContent(c), bagNotFound);
						//insert sorted
						root = true;
						//bagNotFound.clear();
					}
				});
			}*/

			#ifdef PRINT_DECOMP
			std::cout << std::endl << "AFTER NORMALIZATION" << std::endl << std::endl;
			traversal.traverse(*td, [&](htd::vertex_t v, htd::vertex_t v2, size_t s){ std::cout << v << "[" << v2 << "]" << " @" << s << ": " << td->bagContent(v) << std::endl; });
			#endif
		
			htd::TreeDecompositionVerifier v;
			if (!v.verify(*hg, *td))                                              
			{
				std::cout << "vertices: " << v.violationsVertexExistence(*hg, *td) << std::endl;
				std::cout << "edges: " << v.violationsHyperedgeCoverage(*hg, *td) << std::endl;
				std::cout << "connectedness: " << v.violationsConnectednessCriterion(*hg, *td) << std::endl;

				htd_main::HumanReadableExporter exp;
				exp.write(*td, *hg, std::cout);
				assert(v.verify(*hg, *td));
			}
		}
		Benchmark::registerTimestamp("tree decomposition time");
		assert(td->maximumBagSize() <= 30);

	#ifdef HEUR_OUTPUT
		if (Benchmark::isOrdering() == 1)
		{
			vertex_t current = td->root();

			std::ofstream f;
			std::stringstream file;
			file << Benchmark::folder() << "/g.inp.dynasp." << Benchmark::id();
			f.open(file.str().c_str());
			IterativeTreeSolver::tree_decomp_output(*td, current, f);
			f.close();
		}	
	#endif
		Benchmark::registerTimestamp("decomposition output time");


		//assert(td->maximumBagSize() - 1 <= 15);
		return td;
	}

	ISolution *IterativeTreeSolver::solve(
			const IInstance &instance,
			const ITreeDecomposition &td) const
	{
		unique_ptr<INodeTableMap> tables = 
			this->initializeMap(td.vertexCount());

		vertex_t root = td.root();
		bool success = true;
		unsigned int pass = 1;
		for(const ITreeAlgorithm *alg : algorithms_)
		{
			/*if (alg == *algorithms_.end())	//last one
				alg->setPass(1);*/
			if(!(success = this->evaluate(td, *alg, instance, *tables)))
				break;
			std::string passDesc("PASS ");
			passDesc += ('0' + pass);
			Benchmark::registerTimestamp(passDesc.c_str());
			std::cout << std::endl << passDesc <<  " finished " << std::endl;
			++pass;
		}
			/*else
				alg->forceSolution();
				//cleanup solutions after every branch
				for (auto &sol : (tables.get())[root])
					sol.forceSolution(); //delete it and non extended children*/

		Benchmark::registerTimestamp("solving time");
		if (!Benchmark::isInterrupt())
		{
			ISolution *sol = nullptr;

			if(success)
				sol = extractor_->extractSolution(root, td, *tables, instance);
			else
				sol = extractor_->emptySolution(instance);

			Benchmark::registerTimestamp("solution extraction time");

			return sol;
		}
		else
			return extractor_->emptySolution(instance);
	}

	ISolution *IterativeTreeSolver::solve(const IInstance &instance) const
	{
		unique_ptr<ITreeDecomposition> td(this->decompose(instance, true, 3, 0, nullptr));
		return this->solve(instance, *td);
	}

	bool IterativeTreeSolver::evaluate(
			const ITreeDecomposition &td,
			const ITreeAlgorithm &algorithm,
			const IInstance &instance,
			INodeTableMap &tables) const
	{
		bool needAllTables = /*
			algorithms_.size() > 1 ||*/ algorithm.needAllTables();

		bool finishedBranch = false;
		vertex_t current = td.root();
		ITable *currentTable = nullptr;
		stack<pair<vertex_t, size_t> > parents;

		while(!parents.empty() || !finishedBranch)
		{
			if (Benchmark::isInterrupt())
				return false;
			if(!finishedBranch)
			{
				if(td.childCount(current) != 0)
				{
					parents.push(make_pair(current, 1));
					current = td.childAtPosition(current, 0);
					continue;
				}

				parents.push(make_pair(current, 0));
				finishedBranch = true;
			}

			pair<vertex_t, size_t> top = parents.top();
			parents.pop();
			if(td.childCount(top.first) > top.second)
			{
				current = td.childAtPosition(top.first, top.second);
				parents.push(make_pair(top.first, top.second + 1));
				finishedBranch = false;
				continue;
			}
			current = top.first;
			currentTable = algorithm.evaluateNode(
											current,
											td,
											tables,
											instance);

			if(currentTable) 
				insertIntoMap(current, td, currentTable, tables, needAllTables);
			else
				return false;
		}
		
		return true;
	}

	unique_ptr<INodeTableMap> IterativeTreeSolver::initializeMap(
			size_t decompositionNodeCount) const
	{
		return unique_ptr<INodeTableMap>(
				new NodeTableMap(decompositionNodeCount));
	}

	void IterativeTreeSolver::insertIntoMap(
			vertex_t node,
			const ITreeDecomposition &td,
			ITable *table,
			INodeTableMap &tables,
			bool needAllTables) const
	{
		IMutableNodeTableMap &map =
			dynamic_cast<IMutableNodeTableMap &>(tables);

		if (!map.contains(node))
			map.insert(node, table);
		
		if(!needAllTables)
		{
			//assert(false);
			size_t childCount = td.childCount(node);
			for(size_t childIndex = 0; childIndex < childCount; ++childIndex)
				map.erase(td.childAtPosition(node, childIndex));
		}
	}

} // namespace sharp
