/**
 * @file LocationSetsVisitor.h
 * @brief LocationSetsVisitor for Gmod path individualization processing
 * @details Visitor pattern implementation for traversing Gmod paths and managing individualization sets during version
 * conversion
 */

#pragma once

#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/GmodNode.h"
#include "dnv/vista/sdk/Locations.h"

#include <limits>
#include <optional>
#include <stdexcept>
#include <tuple>

namespace dnv::vista::sdk::internal
{
    // Sentinel value for uninitialized parent start index
    static constexpr size_t INVALID_INDEX = std::numeric_limits<size_t>::max();

    /**
     * @brief Visitor for traversing Gmod paths and managing individualization sets during version conversion.
     *
     * @details
     * The LocationSetsVisitor class implements the visitor pattern for traversing nodes in a Gmod path,
     * identifying and managing sets of nodes that can be individualized (assigned unique locations) during
     * version conversion or path processing. It provides logic to detect contiguous and valid individualizable
     * node sets, handles error conditions for invalid groupings, and returns tuple information for each set found.
     * This class is used internally by the VISTA SDK for advanced path and location management workflows.
     */
    class LocationSetsVisitor
    {
    public:
        LocationSetsVisitor()
            : m_currentParentStart{ INVALID_INDEX }
        {
        }

        template <typename Container>
        std::optional<std::tuple<size_t, size_t, std::optional<Location>>> visit(
            const GmodNode& node, size_t i, const Container& pathParents, const GmodNode& pathTargetNode )
        {
            const size_t pathParentsSize = pathParents.size();
            const bool isParent = Gmod::isPotentialParent( node.metadata().type() );
            const bool isTargetNode = ( i == pathParentsSize );
            if( m_currentParentStart == INVALID_INDEX )
            {
                if( isParent )
                {
                    m_currentParentStart = i;
                }
                if( node.isIndividualizable( isTargetNode ) )
                {
                    return std::make_tuple( i, i, node.location() );
                }
            }
            else
            {
                if( isParent || isTargetNode )
                {
                    std::optional<std::tuple<size_t, size_t, std::optional<Location>>> nodes = std::nullopt;

                    if( m_currentParentStart + 1 == i )
                    {
                        if( node.isIndividualizable( isTargetNode ) )
                            nodes = std::make_tuple( i, i, node.location() );
                    }
                    else
                    {
                        size_t skippedOne = INVALID_INDEX;
                        bool hasComposition = false;

                        for( size_t j = m_currentParentStart + 1; j <= i; ++j )
                        {
                            const GmodNode& setNode = ( j < pathParentsSize ) ? pathParents[j] : pathTargetNode;

                            const bool isTargetInSet = ( j == pathParentsSize );
                            if( !setNode.isIndividualizable( isTargetInSet, true ) )
                            {
                                if( nodes.has_value() )
                                {
                                    skippedOne = j;
                                }

                                continue;
                            }

                            if( nodes.has_value() && std::get<2>( nodes.value() ).has_value() &&
                                setNode.location().has_value() && std::get<2>( nodes.value() ) != setNode.location() )
                            {
                                throw std::runtime_error{ "Mapping error: different locations in the same nodeset" };
                            }

                            if( skippedOne != INVALID_INDEX )
                            {
                                throw std::runtime_error{ "Can't skip in the middle of individualizable set" };
                            }

                            if( setNode.isFunctionComposition() )
                            {
                                hasComposition = true;
                            }

                            auto location = nodes.has_value() && std::get<2>( nodes.value() ).has_value()
                                                ? std::get<2>( nodes.value() )
                                                : setNode.location();
                            size_t start = nodes.has_value() ? std::get<0>( nodes.value() ) : j;
                            size_t end = j;
                            nodes = std::make_tuple( start, end, location );
                        }

                        if( nodes.has_value() && std::get<0>( nodes.value() ) == std::get<1>( nodes.value() ) &&
                            hasComposition )
                        {
                            nodes = std::nullopt;
                        }
                    }

                    m_currentParentStart = i;
                    if( nodes.has_value() )
                    {
                        bool hasLeafNode = false;
                        const size_t startIdx = std::get<0>( nodes.value() );
                        const size_t endIdx = std::get<1>( nodes.value() );

                        for( size_t j = startIdx; j <= endIdx; ++j )
                        {
                            const GmodNode& setNode = ( j < pathParentsSize ) ? pathParents[j] : pathTargetNode;

                            if( setNode.isLeafNode() || j == pathParentsSize )
                            {
                                hasLeafNode = true;
                                break;
                            }
                        }
                        if( hasLeafNode )
                        {
                            return nodes;
                        }
                    }
                }

                if( isTargetNode && node.isIndividualizable( isTargetNode ) )
                {
                    return std::make_tuple( i, i, node.location() );
                }
            }

            return std::nullopt;
        }

    private:
        size_t m_currentParentStart;
    };
} // namespace dnv::vista::sdk::internal
