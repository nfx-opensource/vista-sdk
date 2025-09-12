/**
 * @file Sample_Gmod_Tree.cpp
 * @brief GMOD Tree Printer for Vista SDK C++
 *
 * This utility prints the hierarchical structure of GMOD (Generic Product Model) trees:
 * - Print complete tree structure with configurable depth
 * - Multiple output formats (simple, detailed, with paths, JSON-like)
 * - Filter by node types (leaf nodes, function nodes, etc.)
 * - Statistics and summary information
 * - Command-line options for customization
 *
 * Usage examples:
 * - Print basic tree: gmodprint
 * - Limit depth: gmodprint --depth 3
 * - Detailed output: gmodprint --detailed
 * - Show paths: gmodprint --paths
 * - JSON format: gmodprint --json
 * - Filter leaf nodes only: gmodprint --leaf-only
 */

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <set>

#include <dnv/vista/sdk/Gmod.h>
#include <dnv/vista/sdk/GmodNode.h>
#include <dnv/vista/sdk/VIS.h>
#include <dnv/vista/sdk/VisVersion.h>

using namespace dnv::vista::sdk;

struct PrintOptions
{
	int maxDepth = 3;
	bool detailed = false;
	bool showPaths = false;
	bool jsonFormat = false;
	bool leafNodesOnly = false;
	bool functionNodesOnly = false;
	bool showStats = true;
	bool sortChildren = true;
	std::string outputFile;
	VisVersion visVersion = VisVersion::v3_9a;
};

struct TreeStats
{
	size_t totalNodes = 0;
	size_t leafNodes = 0;
	size_t functionNodes = 0;
	size_t assetNodes = 0;
	size_t maxDepth = 0;
	std::map<std::string, size_t> nodeTypes;
	std::set<std::string> categories;
};

class GmodTreePrinter
{
private:
	const Gmod& m_gmod;
	PrintOptions m_options;
	TreeStats m_stats;
	std::ostream* m_output;

public:
	GmodTreePrinter( const Gmod& gmod, const PrintOptions& options, std::ostream* output = &std::cout )
		: m_gmod( gmod ), m_options( options ), m_output( output )
	{
	}

	void printTree()
	{
		if ( m_options.jsonFormat )
		{
			printJsonTree();
		}
		else
		{
			printStandardTree();
		}

		if ( m_options.showStats )
		{
			printStatistics();
		}
	}

private:
	void printStandardTree()
	{
		*m_output << "=== GMOD Tree Structure ===" << std::endl;
		*m_output << "VIS Version: " << VisVersionExtensions::toVersionString( m_options.visVersion ) << std::endl;
		*m_output << "Max Depth: " << m_options.maxDepth << std::endl;
		*m_output << std::endl;

		// Use stateful traversal handler
		TraverseHandlerWithState<GmodTreePrinter> handler =
			[]( GmodTreePrinter& printer, const std::vector<const GmodNode*>& parents, const GmodNode& node ) -> TraversalHandlerResult {
			return printer.handleNode( parents, node );
		};

		m_gmod.traverse( *this, handler );
	}

	void printJsonTree()
	{
		*m_output << "{" << std::endl;
		*m_output << "  \"visVersion\": \"" << VisVersionExtensions::toVersionString( m_options.visVersion ) << "\"," << std::endl;
		*m_output << "  \"maxDepth\": " << m_options.maxDepth << "," << std::endl;
		*m_output << "  \"tree\": ";

		// Use stateful traversal handler
		TraverseHandlerWithState<GmodTreePrinter> handler =
			[]( GmodTreePrinter& printer, const std::vector<const GmodNode*>& parents, const GmodNode& node ) -> TraversalHandlerResult {
			return printer.handleJsonNode( parents, node );
		};

		m_gmod.traverse( *this, handler );

		*m_output << std::endl
				  << "}" << std::endl;
	}

	TraversalHandlerResult handleNode( const std::vector<const GmodNode*>& parents, const GmodNode& node )
	{
		size_t depth = parents.size();

		// Update statistics
		updateStats( node, depth );

		// Check depth limit
		if ( depth > static_cast<size_t>( m_options.maxDepth ) )
		{
			return TraversalHandlerResult::SkipSubtree;
		}

		// Apply filters
		if ( m_options.leafNodesOnly && !node.isLeafNode() )
		{
			return TraversalHandlerResult::Continue;
		}

		if ( m_options.functionNodesOnly && !node.isFunctionNode() )
		{
			return TraversalHandlerResult::Continue;
		}

		// Print the node
		printNode( parents, node, depth );

		return TraversalHandlerResult::Continue;
	}

	TraversalHandlerResult handleJsonNode( const std::vector<const GmodNode*>& parents, const GmodNode& node )
	{
		size_t depth = parents.size();

		// Update statistics
		updateStats( node, depth );

		// Check depth limit
		if ( depth > static_cast<size_t>( m_options.maxDepth ) )
		{
			return TraversalHandlerResult::SkipSubtree;
		}

		// Print JSON node (simplified for this example)
		if ( depth == 0 )
		{
			printJsonNode( node, depth, true );
		}

		return TraversalHandlerResult::Continue;
	}

	void printNode( const std::vector<const GmodNode*>& parents, const GmodNode& node, size_t depth )
	{
		// Create indentation
		std::string indent( depth * 2, ' ' );

		// Print basic node info
		*m_output << indent << "- " << node.code();

		if ( m_options.detailed )
		{
			printDetailedNodeInfo( node );
		}

		if ( m_options.showPaths )
		{
			printNodePath( parents, node );
		}

		*m_output << std::endl;
	}

	void printDetailedNodeInfo( const GmodNode& node )
	{
		*m_output << " [";

		// Node type
		if ( node.isRoot() )
		{
			*m_output << "ROOT";
		}
		else if ( node.isLeafNode() )
		{
			*m_output << "LEAF";
		}
		else if ( node.isFunctionNode() )
		{
			*m_output << "FUNC";
		}
		else if ( node.isAssetFunctionNode() )
		{
			*m_output << "ASSET-FUNC";
		}
		else
		{
			*m_output << "NODE";
		}

		*m_output << "]";

		// Metadata information
		const auto& metadata = node.metadata();
		if ( !metadata.name().empty() )
		{
			*m_output << " (" << metadata.name() << ")";
		}

		// Category and type
		if ( !metadata.category().empty() )
		{
			*m_output << " {" << metadata.category();
			if ( !metadata.type().empty() )
			{
				*m_output << "/" << metadata.type();
			}
			*m_output << "}";
		}

		// Children count
		if ( !node.children().empty() )
		{
			*m_output << " <" << node.children().size() << " children>";
		}
	}

	void printNodePath( const std::vector<const GmodNode*>& parents, const GmodNode& node )
	{
		*m_output << " (Path: ";

		for ( const auto& parent : parents )
		{
			*m_output << parent->code() << "/";
		}
		*m_output << node.code() << ")";
	}

	void printJsonNode( const GmodNode& node, size_t depth, bool isRoot = false )
	{
		std::string indent( depth * 2 + 2, ' ' );

		if ( isRoot )
		{
			*m_output << "{" << std::endl;
		}

		*m_output << indent << "\"code\": \"" << node.code() << "\"," << std::endl;
		*m_output << indent << "\"isLeaf\": " << ( node.isLeafNode() ? "true" : "false" ) << "," << std::endl;
		*m_output << indent << "\"isFunction\": " << ( node.isFunctionNode() ? "true" : "false" ) << std::endl;

		const auto& metadata = node.metadata();
		if ( !metadata.name().empty() )
		{
			*m_output << "," << std::endl
					  << indent << "\"name\": \"" << metadata.name() << "\"";
		}

		if ( !metadata.category().empty() )
		{
			*m_output << "," << std::endl
					  << indent << "\"category\": \"" << metadata.category() << "\"";
		}

		if ( isRoot )
		{
			*m_output << std::endl
					  << "}";
		}
	}

	void updateStats( const GmodNode& node, size_t depth )
	{
		m_stats.totalNodes++;

		if ( node.isLeafNode() )
			m_stats.leafNodes++;

		if ( node.isFunctionNode() )
			m_stats.functionNodes++;

		if ( node.isAssetFunctionNode() )
			m_stats.assetNodes++;

		if ( depth > m_stats.maxDepth )
			m_stats.maxDepth = depth;

		const auto& metadata = node.metadata();
		if ( !metadata.category().empty() )
		{
			m_stats.categories.insert( std::string( metadata.category() ) );
			m_stats.nodeTypes[std::string( metadata.category() )]++;
		}
	}

	void printStatistics()
	{
		*m_output << std::endl
				  << "=== Tree Statistics ===" << std::endl;
		*m_output << "Total nodes: " << m_stats.totalNodes << std::endl;
		*m_output << "Leaf nodes: " << m_stats.leafNodes << std::endl;
		*m_output << "Function nodes: " << m_stats.functionNodes << std::endl;
		*m_output << "Asset function nodes: " << m_stats.assetNodes << std::endl;
		*m_output << "Maximum depth: " << m_stats.maxDepth << std::endl;

		if ( !m_stats.categories.empty() )
		{
			*m_output << std::endl
					  << "Node categories:" << std::endl;
			for ( const auto& category : m_stats.categories )
			{
				auto count = m_stats.nodeTypes[category];
				*m_output << "  " << std::setw( 20 ) << std::left << category << ": " << count << std::endl;
			}
		}
	}
};

void printUsage( const char* programName )
{
	std::cout << "Usage: " << programName << " [options]" << std::endl;
	std::cout << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "  -d, --depth <n>          Maximum depth to traverse (default: 3)" << std::endl;
	std::cout << "  -v, --version <version>  VIS version (3-4a, 3-5a, 3-6a, 3-7a, 3-8a, 3-9a)" << std::endl;
	std::cout << "  --detailed               Show detailed node information" << std::endl;
	std::cout << "  --paths                  Show full paths to each node" << std::endl;
	std::cout << "  --json                   Output in JSON format" << std::endl;
	std::cout << "  --leaf-only              Show only leaf nodes" << std::endl;
	std::cout << "  --function-only          Show only function nodes" << std::endl;
	std::cout << "  --no-stats               Don't show statistics" << std::endl;
	std::cout << "  --no-sort                Don't sort children nodes" << std::endl;
	std::cout << "  -o, --output <file>      Output to file instead of console" << std::endl;
	std::cout << "  -h, --help               Show this help message" << std::endl;
	std::cout << std::endl;
	std::cout << "Examples:" << std::endl;
	std::cout << "  " << programName << "                          # Basic tree (depth 3)" << std::endl;
	std::cout << "  " << programName << " --depth 5 --detailed     # Detailed tree with depth 5" << std::endl;
	std::cout << "  " << programName << " --json --leaf-only       # JSON format, leaf nodes only" << std::endl;
	std::cout << "  " << programName << " --paths -o tree.txt      # With paths, output to file" << std::endl;
}

// Use the existing VisVersionExtensions::parse() from the SDK
// No need to duplicate the parsing logic

PrintOptions parseCommandLine( int argc, char* argv[] )
{
	PrintOptions options;

	for ( int i = 1; i < argc; ++i )
	{
		std::string arg = argv[i];

		if ( arg == "-h" || arg == "--help" )
		{
			printUsage( argv[0] );
			exit( 0 );
		}
		else if ( arg == "-d" || arg == "--depth" )
		{
			if ( i + 1 < argc )
			{
				options.maxDepth = std::stoi( argv[++i] );
				if ( options.maxDepth < 0 || options.maxDepth > 30 )
				{
					throw std::invalid_argument( "Depth must be between 0 and 30" );
				}
			}
			else
			{
				throw std::invalid_argument( "--depth requires a number" );
			}
		}
		else if ( arg == "-v" || arg == "--version" )
		{
			if ( i + 1 < argc )
			{
				options.visVersion = VisVersionExtensions::parse( argv[++i] );
			}
			else
			{
				throw std::invalid_argument( "--version requires a version string" );
			}
		}
		else if ( arg == "--detailed" )
		{
			options.detailed = true;
		}
		else if ( arg == "--paths" )
		{
			options.showPaths = true;
		}
		else if ( arg == "--json" )
		{
			options.jsonFormat = true;
		}
		else if ( arg == "--leaf-only" )
		{
			options.leafNodesOnly = true;
		}
		else if ( arg == "--function-only" )
		{
			options.functionNodesOnly = true;
		}
		else if ( arg == "--no-stats" )
		{
			options.showStats = false;
		}
		else if ( arg == "--no-sort" )
		{
			options.sortChildren = false;
		}
		else if ( arg == "-o" || arg == "--output" )
		{
			if ( i + 1 < argc )
			{
				options.outputFile = argv[++i];
			}
			else
			{
				throw std::invalid_argument( "--output requires a filename" );
			}
		}
		else if ( arg.substr( 0, 1 ) == "-" )
		{
			throw std::invalid_argument( "Unknown option: " + arg );
		}
		else
		{
			throw std::invalid_argument( "Unexpected argument: " + arg );
		}
	}

	return options;
}

int main( int argc, char* argv[] )
{
	try
	{
		PrintOptions options = parseCommandLine( argc, argv );

		std::cout << "Loading GMOD for VIS version "
				  << VisVersionExtensions::toVersionString( options.visVersion ) << "..." << std::endl;

		auto& vis = VIS::instance();
		auto gmod = vis.gmod( options.visVersion );

		std::unique_ptr<std::ofstream> fileOutput;
		std::ostream* output = &std::cout;

		if ( !options.outputFile.empty() )
		{
			fileOutput = std::make_unique<std::ofstream>( options.outputFile );
			if ( !fileOutput->is_open() )
			{
				throw std::runtime_error( "Could not open output file: " + options.outputFile );
			}
			output = fileOutput.get();
			std::cout << "Writing output to: " << options.outputFile << std::endl;
		}

		GmodTreePrinter printer( gmod, options, output );
		printer.printTree();

		if ( !options.outputFile.empty() )
		{
			std::cout << "Tree printed to: " << options.outputFile << std::endl;
		}

		std::cout << "\n=== GMOD Tree Printing completed! ===" << std::endl;
		return 0;
	}
	catch ( const std::exception& e )
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
}
