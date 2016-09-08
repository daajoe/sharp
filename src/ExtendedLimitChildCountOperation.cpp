/*
 * File:   ExtendedLimitChildCountOperation.cpp
 *
 * Author: ABSEHER Michael (abseher@dbai.tuwien.ac.at)
 * 
 * Copyright 2015-2016, Michael Abseher
 *    E-Mail: <abseher@dbai.tuwien.ac.at>
 * 
 * This file is part of htd.
 * 
 * htd is free software: you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free 
 * Software Foundation, either version 3 of the License, or (at your 
 * option) any later version.
 * 
 * htd is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public 
 * License for more details.

 * You should have received a copy of the GNU General Public License
 * along with htd.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HTD_HTD_EXTLIMITCHILDCOUNTOPERATION_CPP
#define	HTD_HTD_EXTLIMITCHILDCOUNTOPERATION_CPP

#include <htd/Globals.hpp>
#include <htd/Helpers.hpp>
#include "ExtendedLimitChildCountOperation.hpp"

#include <algorithm>
#include <stdexcept>
#include <iterator>

/**
 *  Private implementation details of class htd::ExtendedLimitChildCountOperation.
 */
struct htd::ExtendedLimitChildCountOperation::Implementation
{
    /**
     *  Constructor for the implementation details structure.
     *
     *  @param[in] manager   The management instance to which the current object instance belongs.
     *  @param[in] limit    The maximum number of children for a decomposition node.
     */
    Implementation(const htd::LibraryInstance * const manager, std::size_t limit) : managementInstance_(manager), limit_(limit)
    {

    }

    virtual ~Implementation()
    {

    }

    /**
     *  The management instance to which the current object instance belongs.
     */
    const htd::LibraryInstance * managementInstance_;

    /**
     *  The maximum number of children for a decomposition node.
     */
    std::size_t limit_;
};

htd::ExtendedLimitChildCountOperation::ExtendedLimitChildCountOperation(const htd::LibraryInstance * const manager, std::size_t limit) : implementation_(new Implementation(manager, limit))
{
    HTD_ASSERT(limit > 0)
}

htd::ExtendedLimitChildCountOperation::~ExtendedLimitChildCountOperation()
{

}

void htd::ExtendedLimitChildCountOperation::apply(const htd::IMultiHypergraph & graph, htd::IMutableTreeDecomposition & decomposition) const
{
    apply(graph, decomposition, std::vector<htd::ILabelingFunction *>());
}

void htd::ExtendedLimitChildCountOperation::apply(const htd::IMultiHypergraph & graph, htd::IMutableTreeDecomposition & decomposition, const std::vector<htd::vertex_t> & relevantVertices, std::vector<htd::vertex_t> & createdVertices, std::vector<htd::vertex_t> & removedVertices) const
{
    apply(graph, decomposition, relevantVertices, std::vector<htd::ILabelingFunction *>(), createdVertices, removedVertices);
}

void htd::ExtendedLimitChildCountOperation::apply(const htd::IMultiHypergraph & graph, htd::IMutableTreeDecomposition & decomposition, const std::vector<htd::ILabelingFunction *> & labelingFunctions) const
{
    HTD_UNUSED(graph)

    std::vector<htd::vertex_t> joinNodes;

    decomposition.copyJoinNodesTo(joinNodes);

    const htd::LibraryInstance & managementInstance = *(implementation_->managementInstance_);

    for (auto it = joinNodes.begin(); it != joinNodes.end() && !managementInstance.isTerminated(); ++it)
    {
        htd::vertex_t node = *it;

        std::size_t childCount = decomposition.childCount(node);
	//std::cout << "looking at " << node << " with " << childCount << " childs" << std::endl;

        const std::vector<htd::vertex_t> bag = decomposition.bagContent(node);
        
        const htd::FilteredHyperedgeCollection & inducedHyperedges = decomposition.inducedHyperedges(node);

	std::vector<htd::vertex_t> bagNotFound = bag;

	std::vector<htd::vertex_t> children;

        decomposition.copyChildrenTo(children, node);

	//if (childCount > implementation_->limit_)
        {
	    htd::index_t index = 0;

            htd::vertex_t attachmentPoint = htd::Vertex::UNKNOWN;

	    htd::vertex_t newNode = htd::Vertex::UNKNOWN;

	    unsigned int first = 0;

            while (childCount - index >= implementation_->limit_ - 1)
            {

		if (childCount > implementation_->limit_ && index != childCount - 1)
		{

			if (attachmentPoint != htd::Vertex::UNKNOWN)
				newNode = decomposition.addParent(attachmentPoint);
			else
				newNode = decomposition.addChild(node);

			for (auto & labelingFunction : labelingFunctions)
			{
			    htd::ILabelCollection * labelCollection = decomposition.labelings().exportVertexLabelCollection(newNode);

			    htd::ILabel * newLabel = labelingFunction->computeLabel(decomposition.bagContent(newNode), *labelCollection);

			    delete labelCollection;

			    decomposition.setVertexLabel(labelingFunction->name(), newNode, newLabel);
			}

			attachmentPoint = newNode;
		}
		else
			newNode = node;

                for (htd::index_t childPosition = index; childPosition < index + implementation_->limit_ - first; ++childPosition)
                {
			if (bagNotFound.size())
			{
				const auto &bag2 = decomposition.bagContent(children[childPosition]);
				std::vector<htd::vertex_t> bagTmp = bagNotFound;
				bagNotFound.clear();
				std::set_difference(bagTmp.begin(), bagTmp.end(), bag2.begin(), bag2.end(), std::back_inserter(bagNotFound));

				//std::cout << childPosition << " resulting " << children[childPosition] << " to " << bagNotFound << std::endl;
			}

			//if (childPosition >= index)
			if (childCount > implementation_->limit_ && index != childCount - 1)
                    		decomposition.setParent(children[childPosition], attachmentPoint);
                }
	
		if (bagNotFound.size())
		{
			std::vector<htd::vertex_t> bagTmp;
			std::set_difference(bag.begin(), bag.end(), bagNotFound.begin(), bagNotFound.end(), std::back_inserter(bagTmp));

			//std::cout << "setting " << newNode << " to " << bagTmp << " due to " << bagNotFound << std::endl;
			decomposition.mutableBagContent(newNode) = bagTmp;

			//TODO: induced edges
			decomposition.mutableInducedHyperedges(newNode) = inducedHyperedges;
		}
		else
		{
			//std::cout << "setting " << newNode << " to " << bag << " due to <EMPTY>" << std::endl;
			decomposition.mutableBagContent(newNode) = bag;

			//TODO: induced edges
			decomposition.mutableInducedHyperedges(newNode) = inducedHyperedges;
		}

                index += implementation_->limit_ - first;
		first = 1;
            }

            /*if (index < childCount)
            {
                htd::vertex_t newNode = decomposition.addChild(attachmentPoint);

                for (auto & labelingFunction : labelingFunctions)
                {
                    htd::ILabelCollection * labelCollection = decomposition.labelings().exportVertexLabelCollection(newNode);

                    htd::ILabel * newLabel = labelingFunction->computeLabel(decomposition.bagContent(newNode), *labelCollection);

                    delete labelCollection;

                    decomposition.setVertexLabel(labelingFunction->name(), newNode, newLabel);
                }

                attachmentPoint = newNode;

                for (htd::index_t childPosition = index - first; childPosition < childCount; ++childPosition)
                {
			if (bagNotFound.size())
			{
				const auto &bag2 = decomposition.bagContent(children[childPosition]);
				std::vector<htd::vertex_t> bagTmp = bagNotFound;
				bagNotFound.clear();
				std::set_difference(bagTmp.begin(), bagTmp.end(), bag2.begin(), bag2.end(), std::back_inserter(bagNotFound));

				std::cout << "resulting " << children[childPosition] << " to " << bagNotFound << std::endl;
			}
			if (childPosition >= index)
                    		decomposition.setParent(children[childPosition], attachmentPoint);
                }
		
		first = 0;

		if (bagNotFound.size())
		{
			std::vector<htd::vertex_t> bagTmp;
			std::set_difference(bag.begin(), bag.end(), bagNotFound.begin(), bagNotFound.end(), std::back_inserter(bagTmp));
			
			std::cout << "setting " << newNode << " to " << bagTmp << " due to " << bagNotFound << std::endl;
			decomposition.mutableBagContent(newNode) = bagTmp;
		}
		else
		{
			std::cout << "setting " << newNode << " to " << bag << " due to <EMPTY>" << std::endl;
			decomposition.mutableBagContent(newNode) = bag;
		}

		//TODO: inducedhyperedges!
                //decomposition.mutableInducedHyperedges(newNode) = inducedHyperedges;
            }

	    if (bagNotFound.size())	//introduce left!
	    {
		htd::vertex_t c = decomposition.addParent(node);
	    	std::cout << "adding " << c << " with " << bag << " because of " << bagNotFound << std::endl;
		decomposition.mutableBagContent(c) = bag;
                decomposition.mutableInducedHyperedges(c) = inducedHyperedges;
	    }*/
        }
	/*else
	{
		//TODO
		for (htd::index_t childPosition = 0; bagNotFound.size() && childPosition < implementation_->limit_ - 1; ++childPosition)
                {
			if (bagNotFound.size())
			{
				const auto &bag2 = decomposition.bagContent(children[childPosition]);
				std::vector<htd::vertex_t> bagTmp = bagNotFound;
				bagNotFound.clear();
				std::set_difference(bagTmp.begin(), bagTmp.end(), bag2.begin(), bag2.end(), std::back_inserter(bagNotFound));

				std::cout << childPosition << " resulting " << children[childPosition] << " to " << bagNotFound << std::endl;
			}
                }
	}*/
	if (bagNotFound.size())	//introduce left! last child still unchecked
	{
		/*const auto &bag2 = decomposition.bagContent(children[childCount - 1]);
		std::vector<htd::vertex_t> bagTmp = bagNotFound;
		bagNotFound.clear();
		std::set_difference(bagTmp.begin(), bagTmp.end(), bag2.begin(), bag2.end(), std::back_inserter(bagNotFound));

		std::cout << childPosition << " resulting " << children[childCount - 1] << " to " << bagNotFound << std::endl;
		
		if (bagNotFound.size())*/
		{
			htd::vertex_t c = decomposition.addParent(node);
			//std::cout << "adding " << c << " with " << bag << " because of " << bagNotFound << std::endl;
			decomposition.mutableBagContent(c) = bag;
			decomposition.mutableInducedHyperedges(c) = inducedHyperedges;
		}
	}
    }
}

void htd::ExtendedLimitChildCountOperation::apply(const htd::IMultiHypergraph & graph, htd::IMutableTreeDecomposition & decomposition, const std::vector<htd::vertex_t> & relevantVertices, const std::vector<htd::ILabelingFunction *> & labelingFunctions, std::vector<htd::vertex_t> & createdVertices, std::vector<htd::vertex_t> & removedVertices) const
{
    HTD_UNUSED(graph)
    HTD_UNUSED(removedVertices)

//TODO: implement!

	assert(false);

    const htd::LibraryInstance & managementInstance = *(implementation_->managementInstance_);

    for (auto it = relevantVertices.begin(); it != relevantVertices.end() && !managementInstance.isTerminated(); ++it)
    {
        htd::vertex_t vertex = *it;

        std::size_t childCount = decomposition.childCount(vertex);

        if (childCount > implementation_->limit_)
        {
            const std::vector<htd::vertex_t> & bag = decomposition.bagContent(vertex);

            const htd::FilteredHyperedgeCollection & inducedHyperedges = decomposition.inducedHyperedges(vertex);

            std::vector<htd::vertex_t> children;

            decomposition.copyChildrenTo(children, vertex);

            htd::index_t index = implementation_->limit_ - 1;

            htd::vertex_t attachmentPoint = vertex;

            while (childCount - index > implementation_->limit_)
            {
                htd::vertex_t newNode = decomposition.addChild(attachmentPoint);

                decomposition.mutableBagContent(newNode) = bag;

                decomposition.mutableInducedHyperedges(newNode) = inducedHyperedges;

                for (auto & labelingFunction : labelingFunctions)
                {
                    htd::ILabelCollection * labelCollection = decomposition.labelings().exportVertexLabelCollection(newNode);

                    htd::ILabel * newLabel = labelingFunction->computeLabel(decomposition.bagContent(newNode), *labelCollection);

                    delete labelCollection;

                    decomposition.setVertexLabel(labelingFunction->name(), newNode, newLabel);
                }

                attachmentPoint = newNode;

                for (htd::index_t childPosition = index; childPosition < index + implementation_->limit_ - 1; ++childPosition)
                {
                    decomposition.setParent(children[childPosition], attachmentPoint);
                }

                createdVertices.push_back(newNode);

                index += implementation_->limit_ - 1;
            }

            if (index < childCount)
            {
                htd::vertex_t newNode = decomposition.addChild(attachmentPoint);

                decomposition.mutableBagContent(newNode) = bag;

                decomposition.mutableInducedHyperedges(newNode) = inducedHyperedges;

                for (auto & labelingFunction : labelingFunctions)
                {
                    htd::ILabelCollection * labelCollection = decomposition.labelings().exportVertexLabelCollection(newNode);

                    htd::ILabel * newLabel = labelingFunction->computeLabel(decomposition.bagContent(newNode), *labelCollection);

                    delete labelCollection;

                    decomposition.setVertexLabel(labelingFunction->name(), newNode, newLabel);
                }

                attachmentPoint = newNode;

                for (htd::index_t childPosition = index; childPosition < childCount; ++childPosition)
                {
                    decomposition.setParent(children[childPosition], attachmentPoint);
                }

                createdVertices.push_back(newNode);
            }
        }
    }
}

bool htd::ExtendedLimitChildCountOperation::isLocalOperation(void) const
{
    return true;
}

bool htd::ExtendedLimitChildCountOperation::createsTreeNodes(void) const
{
    return true;
}

bool htd::ExtendedLimitChildCountOperation::removesTreeNodes(void) const
{
    return false;
}

bool htd::ExtendedLimitChildCountOperation::modifiesBagContents(void) const
{
    return false;
}

bool htd::ExtendedLimitChildCountOperation::createsSubsetMaximalBags(void) const
{
    return false;
}

bool htd::ExtendedLimitChildCountOperation::createsLocationDependendLabels(void) const
{
    return false;
}

const htd::LibraryInstance * htd::ExtendedLimitChildCountOperation::managementInstance(void) const HTD_NOEXCEPT
{
    return implementation_->managementInstance_;
}

void htd::ExtendedLimitChildCountOperation::setManagementInstance(const htd::LibraryInstance * const manager)
{
    HTD_ASSERT(manager != nullptr)

    implementation_->managementInstance_ = manager;
}

htd::ExtendedLimitChildCountOperation * htd::ExtendedLimitChildCountOperation::clone(void) const
{
    return new htd::ExtendedLimitChildCountOperation(managementInstance(), implementation_->limit_);
}

#ifdef HTD_USE_VISUAL_STUDIO_COMPATIBILITY_MODE
htd::IDecompositionManipulationOperation * htd::ExtendedLimitChildCountOperation::cloneDecompositionManipulationOperation(void) const
{
    return clone();
}

htd::ITreeDecompositionManipulationOperation * htd::ExtendedLimitChildCountOperation::cloneTreeDecompositionManipulationOperation(void) const
{
    return clone();
}
#endif

#endif /* HTD_HTD_LIMITCHILDCOUNTOPERATION_CPP */
