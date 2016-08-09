#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "IterativeTreeSolver.hpp"

#include "NullTreeSolutionExtractor.hpp"
#include "NodeTableMap.hpp"

#include <sharp/Benchmark.hpp>
#include <sharp/TextOutput.hpp>

#include <htd/JoinNodeReplacementOperation.hpp>
#include <htd/TreeDecompositionFactory.hpp>
#include <htd/SemiNormalizationOperation.hpp>
#include <htd/WeakNormalizationOperation.hpp>
#include <htd/LimitChildCountOperation.hpp>
#include <htd/TreeDecompositionVerifier.hpp>

#include <stack>
#include <memory>
#include <cstddef>


#include <htd/main.hpp>
#include <chrono>

namespace sharp {
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
                                  true) {}

    IterativeTreeSolver::IterativeTreeSolver(
            const htd::ITreeDecompositionAlgorithm &decomposer,
            std::vector<std::unique_ptr<const ITreeAlgorithm> > &&algorithms,
            std::unique_ptr<const ITreeSolutionExtractor> extractor,
            bool deleteAlgorithms,
            bool deleteExtractor)
            : decomposer_(decomposer),
              extractor_(extractor.get()),
              manageAlgorithmMemory_(deleteAlgorithms),
              manageExtractorMemory_(deleteExtractor) {
        for (std::unique_ptr<const ITreeAlgorithm> &alg : algorithms)
            algorithms_.push_back(alg.get());

        for (std::unique_ptr<const ITreeAlgorithm> &alg : algorithms)
            alg.release();
        extractor.release();
    }

    IterativeTreeSolver::IterativeTreeSolver(
            const htd::ITreeDecompositionAlgorithm &decomposer,
            const ITreeAlgorithm &algorithm)
            : IterativeTreeSolver(decomposer, {&algorithm}) {}

    IterativeTreeSolver::IterativeTreeSolver(
            const htd::ITreeDecompositionAlgorithm &decomposer,
            const ITreeAlgorithm &algorithm1,
            const ITreeAlgorithm &algorithm2)
            : IterativeTreeSolver(decomposer, {&algorithm1, &algorithm2}) {}

    IterativeTreeSolver::IterativeTreeSolver(
            const htd::ITreeDecompositionAlgorithm &decomposer,
            const TreeAlgorithmVector &algorithms)
            : decomposer_(decomposer),
              algorithms_(algorithms),
              extractor_(new NullTreeSolutionExtractor()),
              manageAlgorithmMemory_(false),
              manageExtractorMemory_(true) {}

    IterativeTreeSolver::IterativeTreeSolver(
            const htd::ITreeDecompositionAlgorithm &decomposer,
            const ITreeAlgorithm &algorithm,
            const ITreeSolutionExtractor &extractor)
            : IterativeTreeSolver(decomposer, {&algorithm}, extractor) {}

    IterativeTreeSolver::IterativeTreeSolver(
            const htd::ITreeDecompositionAlgorithm &decomposer,
            const ITreeAlgorithm &alg1,
            const ITreeAlgorithm &alg2,
            const ITreeSolutionExtractor &extractor)
            : IterativeTreeSolver(decomposer, {&alg1, &alg2}, extractor) {}

    IterativeTreeSolver::IterativeTreeSolver(
            const htd::ITreeDecompositionAlgorithm &decomposer,
            const TreeAlgorithmVector &algorithms,
            const ITreeSolutionExtractor &extractor)
            : decomposer_(decomposer),
              algorithms_(algorithms),
              extractor_(&extractor),
              manageAlgorithmMemory_(false),
              manageExtractorMemory_(false) {}


    IterativeTreeSolver::~IterativeTreeSolver() {
        if (manageAlgorithmMemory_)
            for (const ITreeAlgorithm *alg : algorithms_)
                if (alg)
                    delete alg;

        if (manageExtractorMemory_ && extractor_) delete extractor_;
    }


    /**
     *  Sample fitness function which minimizes width and height of the decomposition.
     *
     *  Width is of higher priority than height, i.e., at first, the width is minimized
     *  and if two decompositions have the same width, the one of lower height is chosen.
     */
    class FitnessFunction : public htd::ITreeDecompositionFitnessFunction {
    public:
        FitnessFunction(void) {

        }

        ~FitnessFunction() {

        }

        htd::FitnessEvaluation *
        fitness(const htd::IMultiHypergraph &graph, const htd::ITreeDecomposition &decomposition) const {
            HTD_UNUSED(graph)

            /**
              * Here we specify the fitness evaluation for a given decomposition. In this case, we select the maximum bag size and the height.
              */
            return new htd::FitnessEvaluation(1, (double) (decomposition.maximumBagSize()));
        }

        FitnessFunction *clone(void) const {
            return new FitnessFunction();
        }
    };


//#define ITERATIVE_TD_IMPROVEMENT

    ITreeDecomposition *IterativeTreeSolver::decompose(
            const IInstance &instance, bool weak, unsigned int maxChilds, bool optimizedTD) const {
        htd::ITreeDecomposition *td = nullptr;
        unique_ptr<IHypergraph> hg(instance.toHypergraph());
        //TODO: include preprocessingOperations in decomposition call
        //		once libhtd supports this

        //#ifdef ITERATIVE_TD_IMPROVEMENT
        if (optimizedTD) {
            FitnessFunction fitnessFunction;

            /**
             *  This operation changes the root of a given decomposition so that the fitness function is maximized.
             *
             *  When no fitness function is provided, the optimization operation does not perform any optimization and only applies provided manipulations.
             */
            htd::TreeDecompositionOptimizationOperation *operation = new htd::TreeDecompositionOptimizationOperation(
                    fitnessFunction);

            /**
             *  Set the vertex selections strategy (default = exhaustive).
             *
             *  In this case, we want to select (at most) 10 vertices of the input decomposition randomly.
             */
            operation->setVertexSelectionStrategy(new htd::RandomVertexSelectionStrategy(10));

            /**
              * Set desired manipulations like in any other decomposition algorithm.
              */
            if (maxChilds >= 2)
                operation->addManipulationOperation(new htd::LimitChildCountOperation(maxChilds));
            if (weak)
                operation->addManipulationOperation(new htd::WeakNormalizationOperation());

            //operation->addManipulationOperation(new htd::NormalizationOperation(true, true, false, false));

            /**
              * In case one uses htd::IterativeImprovementTreeDecompositionAlgorithm,
              * I recommend to use htd::MinFillOrderingAlgorithm() as it allows for
              * more randomness.
              */
            htd::OrderingAlgorithmFactory::instance().setConstructionTemplate(new htd::MinFillOrderingAlgorithm());

            /**
             *  Get the default tree decomposition algorithm.
             */
            htd::ITreeDecompositionAlgorithm *baseAlgorithm = htd::TreeDecompositionAlgorithmFactory::instance().getTreeDecompositionAlgorithm();

            /**
             *  Set the optimization operation as manipulation operation.
             */
            baseAlgorithm->addManipulationOperation(operation);

            /**
             *  Create a new instance of htd::IterativeImprovementTreeDecompositionAlgorithm based on the base algorithm and the fitness function.
             *
             *  Note that the fitness function can be an arbiraty one and can differ from the one used in the optimization operation.
             */
            htd::IterativeImprovementTreeDecompositionAlgorithm algorithm(baseAlgorithm, fitnessFunction);

            /**
             *  Set the maximum number of iterations after which the best decomposition with respect to the fitness function shall be returned.
             *
             *  Use value 1 to make the iterative algorithm return the first decomposition found.
             */
            algorithm.setIterationCount(10);

            /**
             *  Set the maximum number of iterations without improvement after which the algorithm returns best decomposition with respect to
             *  the fitness function found so far. A limit of 0 aborts the algorithm after the first non-improving solution has been found,
             *  i.e. the algorithm will perform a simple hill-climbing approach.
             */
            algorithm.setNonImprovementLimit(25);

            std::size_t optimalBagSize = (std::size_t) -1;

            /**
             *  Compute the decomposition. Note that the additional, optional parameter for computeDecomposition() in case
             *  of htd::IterativeImprovementTreeDecompositionAlgorithm can be used to intercept every new decomposition.
             *  In this case we output some intermediate information upon perceiving a new best width.
             */
            td =
                    algorithm.computeDecomposition(*hg, [&](const htd::IMultiHypergraph &graph,
                                                            const htd::ITreeDecomposition &decomposition,
                                                            const htd::FitnessEvaluation &fitness) {
                        HTD_UNUSED(graph)
                        HTD_UNUSED(decomposition)

                        std::size_t bagSize = -fitness.at(0);

                        if (bagSize < optimalBagSize) {
                            optimalBagSize = bagSize;

                            std::chrono::milliseconds::rep msSinceEpoch =
                                    std::chrono::duration_cast<std::chrono::milliseconds>
                                            (std::chrono::system_clock::now().time_since_epoch()).count();

                            std::cerr << "c status " << optimalBagSize << " " << msSinceEpoch << std::endl;
                        }
                    });
        }
        else {

            td = decomposer_.computeDecomposition(*hg);
            Benchmark::output()->debug("before normalization", td);

            //TODO: remove joinnodereplacementoperation
            //htd::JoinNodeReplacementOperation j;
            //j.apply(htd::TreeDecompositionFactory::instance().accessMutableTreeDecomposition(*td));

            htd::WeakNormalizationOperation js;
            htd::LimitChildCountOperation js2(maxChilds);
            //htd::SemiNormalizationOperation js2;

            if (maxChilds >= 2)
                js2.apply(*hg, htd::TreeDecompositionFactory::instance().accessMutableTreeDecomposition(*td));
            if (weak)
                js.apply(*hg, htd::TreeDecompositionFactory::instance().accessMutableTreeDecomposition(*td));

            htd::TreeDecompositionVerifier v;
            assert(v.verify(*hg, *td));
//            const ISharpOutput *out_ = Benchmark::output();
            Benchmark::output()->debug("after normalization", td);
        }
        Benchmark::output()->info(td);

        Benchmark::registerTimestamp("tree decomposition time");
        assert(td->maximumBagSize() <= 15);
        //assert(td->maximumBagSize() - 1 <= 15);
        return td;
    }

    ISolution *IterativeTreeSolver::solve(
            const IInstance &instance,
            const ITreeDecomposition &td) const {
        unique_ptr<INodeTableMap> tables =
                this->initializeMap(td.vertexCount());

        vertex_t root = td.root();
        bool success = true;
        unsigned int pass = 1;
        for (const ITreeAlgorithm *alg : algorithms_) {
            /*if (alg == *algorithms_.end())	//last one
                alg->setPass(1);*/
            if (!(success = this->evaluate(td, *alg, instance, *tables)))
                break;
            std::string passDesc("PASS ");
            passDesc += ('0' + pass);
            Benchmark::registerTimestamp(passDesc.c_str());
            Benchmark::output()->info(passDesc, "finished");
            ++pass;
        }
        /*else
            alg->forceSolution();
            //cleanup solutions after every branch
            for (auto &sol : (tables.get())[root])
                sol.forceSolution(); //delete it and non extended children*/

        Benchmark::registerTimestamp("solving time");
        if (!Benchmark::isInterrupt()) {
            ISolution *sol = nullptr;

            if (success)
                sol = extractor_->extractSolution(root, td, *tables, instance);
            else
                sol = extractor_->emptySolution(instance);

            Benchmark::registerTimestamp("solution extraction time");

            return sol;
        }
        else
            return extractor_->emptySolution(instance);
    }

    ISolution *IterativeTreeSolver::solve(const IInstance &instance) const {
        unique_ptr<ITreeDecomposition> td(this->decompose(instance, true, 3, false));
        return this->solve(instance, *td);
    }

    bool IterativeTreeSolver::evaluate(
            const ITreeDecomposition &td,
            const ITreeAlgorithm &algorithm,
            const IInstance &instance,
            INodeTableMap &tables) const {
        bool needAllTables = /*
			algorithms_.size() > 1 ||*/ algorithm.needAllTables();

        bool finishedBranch = false;
        vertex_t current = td.root();
        ITable *currentTable = nullptr;
        stack<pair<vertex_t, size_t> > parents;

        while (!parents.empty() || !finishedBranch) {
            if (Benchmark::isInterrupt())
                return false;
            if (!finishedBranch) {
                if (td.childCount(current) != 0) {
                    parents.push(make_pair(current, 1));
                    current = td.childAtPosition(current, 0);
                    continue;
                }

                parents.push(make_pair(current, 0));
                finishedBranch = true;
            }

            pair<vertex_t, size_t> top = parents.top();
            parents.pop();
            if (td.childCount(top.first) > top.second) {
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

            if (currentTable)
                insertIntoMap(current, td, currentTable, tables, needAllTables);
            else
                return false;
        }

        return true;
    }

    unique_ptr<INodeTableMap> IterativeTreeSolver::initializeMap(
            size_t decompositionNodeCount) const {
        return unique_ptr<INodeTableMap>(
                new NodeTableMap(decompositionNodeCount));
    }

    void IterativeTreeSolver::insertIntoMap(
            vertex_t node,
            const ITreeDecomposition &td,
            ITable *table,
            INodeTableMap &tables,
            bool needAllTables) const {
        IMutableNodeTableMap &map =
                dynamic_cast<IMutableNodeTableMap &>(tables);

        if (!map.contains(node))
            map.insert(node, table);

        if (!needAllTables) {
            //assert(false);
            size_t childCount = td.childCount(node);
            for (size_t childIndex = 0; childIndex < childCount; ++childIndex)
                map.erase(td.childAtPosition(node, childIndex));
        }
    }

} // namespace sharp
