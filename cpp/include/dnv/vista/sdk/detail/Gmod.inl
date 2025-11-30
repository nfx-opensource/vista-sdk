/**
 * @file Gmod.inl
 * @brief Inline implementations for performance-critical Gmod operations
 */

#include <stdexcept>

namespace dnv::vista::sdk
{
	//=====================================================================
	// Gmod class
	//=====================================================================

	//----------------------------------------------
	// Lookup operators
	//----------------------------------------------

	inline const GmodNode& Gmod::operator[]( std::string_view key ) const
	{
		const auto* nodePtr = m_nodeMap.find( key );
		if ( nodePtr != nullptr )
		{
			return *nodePtr;
		}

		throw std::out_of_range{ "Key not found in Gmod node map: " + std::string{ key } };
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline VisVersion Gmod::visVersion() const
	{
		return m_visVersion;
	}

	inline const GmodNode& Gmod::rootNode() const
	{
		if ( !m_rootNode )
		{
			throw std::runtime_error{ "Root node is not initialized or 'VE' was not found." };
		}

		return *m_rootNode;
	}

	//----------------------------------------------
	// Node query methods
	//----------------------------------------------

	inline bool Gmod::tryGetNode( std::string_view code, GmodNode*& node ) const noexcept
	{
		const auto* ptr = m_nodeMap.find( code );
		if ( ptr != nullptr )
		{
			node = const_cast<GmodNode*>( ptr );
			return true;
		}
		return false;
	}

	//----------------------------------------------
	// Enumeration
	//----------------------------------------------

	inline Gmod::Enumerator Gmod::enumerator() const
	{
		return Enumerator{ &m_nodeMap };
	}

	//----------------------------------------------
	// Gmod::Enumerator class
	//----------------------------------------------

	//-----------------------------
	// Construction
	//-----------------------------

	inline Gmod::Enumerator::Enumerator( const nfx::containers::PerfectHashMap<std::string, GmodNode, uint32_t, VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS>* map ) noexcept
		: m_sourceMapPtr{ map },
		  m_currentMapIterator{ map->begin() },
		  m_isInitialState{ true }
	{
	}

	//-----------------------------
	// Iteration interface
	//-----------------------------

	inline const GmodNode& Gmod::Enumerator::current() const
	{
		if ( !m_sourceMapPtr || m_isInitialState || m_currentMapIterator == m_sourceMapPtr->end() )
		{
			throw std::out_of_range{ "Gmod::Enumerator::getCurrent() called in an invalid state or past the end." };
		}

		return m_currentMapIterator->second;
	}

	inline bool Gmod::Enumerator::next() noexcept
	{
		if ( !m_sourceMapPtr || m_sourceMapPtr->isEmpty() )
		{
			m_isInitialState = false;

			return false;
		}

		if ( m_isInitialState )
		{
			m_isInitialState = false;

			return m_currentMapIterator != m_sourceMapPtr->end();
		}

		if ( m_currentMapIterator != m_sourceMapPtr->end() )
		{
			++m_currentMapIterator;

			return m_currentMapIterator != m_sourceMapPtr->end();
		}

		return false;
	}

	inline void Gmod::Enumerator::reset() noexcept
	{
		m_isInitialState = true;

		if ( m_sourceMapPtr )
		{
			m_currentMapIterator = m_sourceMapPtr->begin();
		}
	}

	//=====================================================================
	// Traversal implementation
	//=====================================================================

	//----------------------------------------------
	// Parents class implementation
	//----------------------------------------------

	inline Gmod::Parents::Parents()
	{
		m_parents.reserve( 64 );
		m_occurrences.reserve( 4 );
	}

	inline void Gmod::Parents::push( const GmodNode* parent )
	{
		m_parents.push_back( parent );
		const size_t* countPtr = m_occurrences.find( parent->code() );
		if ( countPtr != nullptr )
		{
			m_occurrences.insertOrAssign( std::string{ parent->code() }, *countPtr + 1 );
		}
		else
		{
			m_occurrences.insertOrAssign( std::string{ parent->code() }, 1 );
		}
	}

	inline void Gmod::Parents::pop()
	{
		if ( m_parents.empty() )
		{
			return;
		}

		const GmodNode* parent = m_parents.back();
		m_parents.pop_back();

		const size_t* countPtr = m_occurrences.find( parent->code() );
		if ( countPtr != nullptr )
		{
			if ( *countPtr == 1 )
			{
				m_occurrences.erase( parent->code() );
			}
			else
			{
				m_occurrences.insertOrAssign( std::string{ parent->code() }, *countPtr - 1 );
			}
		}
	}

	inline size_t Gmod::Parents::occurrences( const GmodNode& node ) const noexcept
	{
		const size_t* countPtr = m_occurrences.find( node.code() );
		if ( countPtr != nullptr )
		{
			return *countPtr;
		}
		else
		{
			return 0;
		}
	}

	inline const GmodNode* Gmod::Parents::lastOrDefault() const noexcept
	{
		return m_parents.empty()
				   ? nullptr
				   : m_parents.back();
	}

	inline const std::vector<const GmodNode*>& Gmod::Parents::asList() const noexcept
	{
		return m_parents;
	}

	//----------------------------------------------
	// Core traversal implementation
	//----------------------------------------------

	template <typename TState>
	inline TraversalHandlerResult Gmod::traverseNode( TraversalContext<TState>& context, const GmodNode& node ) const
	{
		TraversalHandlerResult result = context.handler( context.state, context.parents.asList(), node );
		if ( result == TraversalHandlerResult::Stop || result == TraversalHandlerResult::SkipSubtree )
		{
			return result;
		}

		bool skipOccurrenceCheck = isProductSelectionAssignment( context.parents.lastOrDefault(), &node );
		if ( !skipOccurrenceCheck )
		{
			size_t occ = context.parents.occurrences( node );
			if ( occ == context.maxTraversalOccurrence )
			{
				return TraversalHandlerResult::SkipSubtree;
			}
			if ( occ > context.maxTraversalOccurrence )
			{
				throw std::runtime_error{ "Invalid state - node occurred more than expected" };
			}
		}

		context.parents.push( &node );

		const auto& children = node.children();
		for ( size_t i = 0; i < children.size(); ++i )
		{
			const GmodNode& child = *children[i];
			result = traverseNode( context, child );
			if ( result == TraversalHandlerResult::Stop )
			{
				return TraversalHandlerResult::Stop;
			}
			else if ( result == TraversalHandlerResult::SkipSubtree )
			{
				continue;
			}
		}

		context.parents.pop();

		return TraversalHandlerResult::Continue;
	}

	//----------------------------------------------
	// Public traverse method implementations
	//----------------------------------------------

	inline bool Gmod::traverse( TraverseHandler handler, const TraversalOptions& options ) const
	{
		TraverseHandler capturedHandler = handler;
		TraverseHandlerWithState<TraverseHandler> wrapperHandler =
			[]( TraverseHandler& h, const std::vector<const GmodNode*>& parents,
				const GmodNode& node ) -> TraversalHandlerResult { return h( parents, node ); };

		return traverse( capturedHandler, wrapperHandler, options );
	}

	template <typename TState>
	inline bool Gmod::traverse(
		TState& state,
		TraverseHandlerWithState<TState> handler,
		const TraversalOptions& options ) const
	{
		Parents parentsStack;
		TraversalContext<TState> context( parentsStack, handler, state, options.maxTraversalOccurrence );

		return traverseNode( context, *m_rootNode ) == TraversalHandlerResult::Continue;
	}

	inline bool Gmod::traverse( const GmodNode& rootNode, TraverseHandler handler, const TraversalOptions& options ) const
	{
		TraverseHandler capturedHandler = handler;
		TraverseHandlerWithState<TraverseHandler> wrapperHandler =
			[]( TraverseHandler& h, const std::vector<const GmodNode*>& parents,
				const GmodNode& node ) -> TraversalHandlerResult { return h( parents, node ); };

		Parents parentsStack;
		TraversalContext<TraverseHandler> context( parentsStack, wrapperHandler, capturedHandler, options.maxTraversalOccurrence );

		return traverseNode( context, rootNode ) == TraversalHandlerResult::Continue;
	}

	template <typename TState>
	inline bool Gmod::traverse(
		TState& state, const GmodNode& rootNode, TraverseHandlerWithState<TState> handler, const TraversalOptions& options ) const
	{
		Parents parentsStack;
		TraversalContext<TState> context( parentsStack, handler, state, options.maxTraversalOccurrence );

		return traverseNode( context, rootNode ) == TraversalHandlerResult::Continue;
	}
} // namespace dnv::vista::sdk
