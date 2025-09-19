/**
 * @file Sample_VIS_Gmod.cpp
 * @brief GMOD API usage examples demonstrating node access, tree traversal, and search operations
 *
 * This sample demonstrates how to:
 * - Get specific nodes by code (e.g., "411.1")
 * - Perform basic tree traversal of the entire GMOD structure
 * - Use stateful traversal with custom context and early termination
 * - Traverse from a specific starting node instead of root
 * - Find the first leaf node using search traversal
 * - Print tree hierarchy with custom sorting and depth control
 *
 * Each example shows different traversal patterns and handler functions
 * that developers can use as templates for their own GMOD operations.
 */

#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <regex>

#include <dnv/vista/sdk/Gmod.h>
#include <dnv/vista/sdk/GmodNode.h>
#include <dnv/vista/sdk/VIS.h>

namespace dnv::vista::sdk::samples
{
	class UseGmod
	{
	private:
		// Custom traversal context
		struct CustomTraversalContext
		{
			long counter = 0;
			const GmodNode* targetNode = nullptr;
		};

		// Static handlers for traverse operations
		static TraversalHandlerResult basicHandler( [[maybe_unused]] const std::vector<const GmodNode*>& parents, [[maybe_unused]] const GmodNode& node )
		{
			return TraversalHandlerResult::Continue;
		}

		static TraversalHandlerResult stateHandler( CustomTraversalContext& ctx, [[maybe_unused]] const std::vector<const GmodNode*>& parents, [[maybe_unused]] const GmodNode& node )
		{
			// Access context properties during the traversal
			ctx.counter++;
			// Manually control the traversal
			if ( ctx.counter > 100 )
				return TraversalHandlerResult::Stop;
			return TraversalHandlerResult::Continue;
		}

		static TraversalHandlerResult leafFinder( CustomTraversalContext& ctx, [[maybe_unused]] const std::vector<const GmodNode*>& parents, const GmodNode& node )
		{
			if ( node.children().empty() )
			{
				ctx.targetNode = &node;
				return TraversalHandlerResult::Stop;
			}
			return TraversalHandlerResult::Continue;
		}

		static TraversalHandlerResult fromNodeHandler( CustomTraversalContext& ctx, [[maybe_unused]] const std::vector<const GmodNode*>& parents, [[maybe_unused]] const GmodNode& node )
		{
			ctx.counter++;
			return TraversalHandlerResult::Continue;
		}

	public:
		static GmodNode getNodeByCode( VisVersion visVersion, const std::string& code )
		{
			auto& vis = VIS::instance();
			const auto& gmod = vis.gmod( visVersion );

			// Assumes it exists
			GmodNode* nodePtr = nullptr;
			if ( gmod.tryGetNode( code, nodePtr ) && nodePtr )
			{
				return *nodePtr;
			}
			throw std::runtime_error( "Node not found: " + code );
		}

		static bool traverse( VisVersion visVersion )
		{
			auto& vis = VIS::instance();
			const auto& gmod = vis.gmod( visVersion );

			// Simplest form of traversal
			auto completed = gmod.traverse( basicHandler );
			// Result is either true or false, based on whether the traversal completed or was stopped early
			return completed;
		}

		static bool traverseWithState( VisVersion visVersion )
		{
			auto& vis = VIS::instance();
			const auto& gmod = vis.gmod( visVersion );

			CustomTraversalContext context;
			// Traversal with state
			auto completed = gmod.traverse( context, stateHandler );
			// Result is either true or false, based on whether the traversal completed or was stopped early
			return completed;
		}

		static bool traverseFromNode( const GmodNode& startNode )
		{
			auto& vis = VIS::instance();
			const auto& gmod = vis.gmod( startNode.visVersion() );

			// Without context
			auto completed = gmod.traverse( startNode, basicHandler );

			// With context
			CustomTraversalContext context;
			completed = gmod.traverse( context, startNode, fromNodeHandler );

			return completed;
		}

		static const GmodNode* findFirstLeafNode( VisVersion visVersion )
		{
			auto& vis = VIS::instance();
			const auto& gmod = vis.gmod( visVersion );

			CustomTraversalContext context;

			// Utilize the context, and handle the case inside the traverse
			gmod.traverse( context, leafFinder );

			return context.targetNode;
		}

		/**
		 * Notice that we don't use traverse() here, since we want to utilize the hierarchy of the graph
		 */
		static std::string printTree( VisVersion visVersion, int maxDepth )
		{
			std::ostringstream ss;
			printNode( ss, VIS::instance().gmod( visVersion ).rootNode(), 0, maxDepth );
			return ss.str();
		}

	private:
		// Helper function for tree printing
		static void printNode( std::ostringstream& ss, const GmodNode& node, int level = 0, int maxDepth = std::numeric_limits<int>::max() )
		{
			if ( level > maxDepth )
				return;

			// Add indentation
			for ( int i = 0; i < level * 2; ++i )
				ss << ' ';
			ss << "- " << node.code() << std::endl;

			// Get children and sort them
			auto children = node.children();
			std::sort( children.begin(), children.end(),
				[]( const GmodNode* a, const GmodNode* b ) {
					// Very basic numeric comparison
					std::regex numRegex( R"(\d+)" );
					std::smatch matchA, matchB;

					if ( std::regex_search( a->code(), matchA, numRegex ) && std::regex_search( b->code(), matchB, numRegex ) )
					{
						try
						{
							int numA = std::stoi( matchA.str() );
							int numB = std::stoi( matchB.str() );
							return numA < numB;
						}
						catch ( ... )
						{
							return a->code() < b->code();
						}
					}
					return a->code() < b->code();
				} );

			for ( const auto& child : children )
			{
				printNode( ss, *child, level + 1, maxDepth );
			}
		}
	};
}

int main()
{
	std::cout << "=========== VIS sample ===========" << std::endl;

	try
	{
		auto visVersion = dnv::vista::sdk::VisVersion::v3_9a;

		// Get node by code
		auto node = dnv::vista::sdk::samples::UseGmod::getNodeByCode( visVersion, "411.1" );
		std::cout << "GetNodeByCode: " << node.code() << std::endl;

		// Basic traversal
		auto result = dnv::vista::sdk::samples::UseGmod::traverse( visVersion );
		std::cout << "Traverse completed: " << ( result ? "True" : "False" ) << " Expect='True'" << std::endl;

		// Traverse from node
		result = dnv::vista::sdk::samples::UseGmod::traverseFromNode( node );
		std::cout << "TraverseFromNode completed: " << ( result ? "True" : "False" ) << " Expect='True'" << std::endl;

		// Traverse with state
		result = dnv::vista::sdk::samples::UseGmod::traverseWithState( visVersion );
		std::cout << "TraverseWithState completed: " << ( result ? "True" : "False" ) << " Expect='False'" << std::endl;

		// Find first leaf node
		auto leafNode = dnv::vista::sdk::samples::UseGmod::findFirstLeafNode( visVersion );
		if ( leafNode )
		{
			std::cout << "FindFirstLeafNode: " << leafNode->code() << std::endl;
		}
		else
		{
			std::cout << "FindFirstLeafNode: null" << std::endl;
		}

		// Print tree
		auto treeString = dnv::vista::sdk::samples::UseGmod::printTree( visVersion, 1 );
		std::cout << "PrintTree:\n"
				  << treeString << std::endl;
	}
	catch ( const std::exception& e )
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
