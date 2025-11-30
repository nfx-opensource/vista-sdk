/**
 * @file LocalIdBuilder.cpp
 * @brief Implementation of the LocalIdBuilder class
 */

#include <nfx/Hashing.h>
#include <nfx/string/StringBuilder.h>
#include <nfx/string/Utils.h>

#include "dnv/vista/sdk/LocalIdBuilder.h"

#include "internal/core/LocalId.h"
#include "internal/parsing/LocalIdParsingErrorBuilder.h"
#include "dnv/vista/sdk/config/config.h"
#include "dnv/vista/sdk/mqtt/LocalId.h"
#include "dnv/vista/sdk/CodebookName.h"
#include "dnv/vista/sdk/Codebooks.h"
#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/LocalId.h"
#include "dnv/vista/sdk/LocalIdBuilder.h"
#include "dnv/vista/sdk/ParsingErrors.h"
#include "dnv/vista/sdk/VIS.h"

namespace dnv::vista::sdk
{
	namespace internal::localId
	{
		//----------------------------------------------
		// Hashing
		//----------------------------------------------

		/**
		 * @brief Computes hash value for LocalIdBuilder combining all components
		 * @details Combines hash codes of LocalIdItems and all MetadataTag components using
		 *          NFX optimized FNV-1a hash combination for consistent results
		 */
		static size_t computeHash(
			// const std::optional<VisVersion>& visVersion,
			const LocalIdItems& items,
			const std::optional<MetadataTag>& quantityTag,
			const std::optional<MetadataTag>& calculationTag,
			const std::optional<MetadataTag>& contentTag,
			const std::optional<MetadataTag>& positionTag,
			const std::optional<MetadataTag>& stateTag,
			const std::optional<MetadataTag>& commandTag,
			const std::optional<MetadataTag>& typeTag,
			const std::optional<MetadataTag>& detailTag ) noexcept
		{
			uint32_t hashCode = VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS;

			// VIS Version
			// if ( visVersion.has_value() )
			// {
			// 	hashCode = nfx::hashing::combine( hashCode, static_cast<uint32_t>( *visVersion ) );
			// }
			// else
			// {
			// 	hashCode = nfx::hashing::combine( hashCode, VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS );
			// }

			// PrimaryItem
			const auto& primaryItem = items.primaryItem();
			if ( primaryItem.has_value() )
			{
				hashCode = nfx::hashing::combine( hashCode, static_cast<uint32_t>( primaryItem->hashCode() ) );
			}
			else
			{
				hashCode = nfx::hashing::combine( hashCode, VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS );
			}

			// SecondaryItem
			const auto& secondaryItem = items.secondaryItem();
			if ( secondaryItem.has_value() )
			{
				hashCode = nfx::hashing::combine( hashCode, static_cast<uint32_t>( secondaryItem->hashCode() ) );
			}
			else
			{
				hashCode = nfx::hashing::combine( hashCode, VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS );
			}

			// Quantity
			if ( quantityTag.has_value() )
			{
				hashCode = nfx::hashing::combine( hashCode, static_cast<uint32_t>( quantityTag->hashCode() ) );
			}
			else
			{
				hashCode = nfx::hashing::combine( hashCode, VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS );
			}

			// Calculation
			if ( calculationTag.has_value() )
			{
				hashCode = nfx::hashing::combine( hashCode, static_cast<uint32_t>( calculationTag->hashCode() ) );
			}
			else
			{
				hashCode = nfx::hashing::combine( hashCode, VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS );
			}

			// Content
			if ( contentTag.has_value() )
			{
				hashCode = nfx::hashing::combine( hashCode, static_cast<uint32_t>( contentTag->hashCode() ) );
			}
			else
			{
				hashCode = nfx::hashing::combine( hashCode, VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS );
			}

			// Position
			if ( positionTag.has_value() )
			{
				hashCode = nfx::hashing::combine( hashCode, static_cast<uint32_t>( positionTag->hashCode() ) );
			}
			else
			{
				hashCode = nfx::hashing::combine( hashCode, VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS );
			}

			// State
			if ( stateTag.has_value() )
			{
				hashCode = nfx::hashing::combine( hashCode, static_cast<uint32_t>( stateTag->hashCode() ) );
			}
			else
			{
				hashCode = nfx::hashing::combine( hashCode, VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS );
			}

			// Command
			if ( commandTag.has_value() )
			{
				hashCode = nfx::hashing::combine( hashCode, static_cast<uint32_t>( commandTag->hashCode() ) );
			}
			else
			{
				hashCode = nfx::hashing::combine( hashCode, VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS );
			}

			// Type
			if ( typeTag.has_value() )
			{
				hashCode = nfx::hashing::combine( hashCode, static_cast<uint32_t>( typeTag->hashCode() ) );
			}
			else
			{
				hashCode = nfx::hashing::combine( hashCode, VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS );
			}

			// Detail
			if ( detailTag.has_value() )
			{
				hashCode = nfx::hashing::combine( hashCode, static_cast<uint32_t>( detailTag->hashCode() ) );
			}
			else
			{
				hashCode = nfx::hashing::combine( hashCode, VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS );
			}

			return static_cast<size_t>( hashCode );
		}

		//=====================================================================
		// Static lookup tables
		//=====================================================================

		static const std::unordered_map<std::string_view, LocalIdParsingState> s_prefixMap{
			{ META_QTY_SHORT, LocalIdParsingState::MetaQuantity },
			{ META_QTY_LONG, LocalIdParsingState::MetaQuantity },
			{ META_CNT_SHORT, LocalIdParsingState::MetaContent },
			{ META_CNT_LONG, LocalIdParsingState::MetaContent },
			{ META_CALC_SHORT, LocalIdParsingState::MetaCalculation },
			{ META_CALC_LONG, LocalIdParsingState::MetaCalculation },
			{ META_STATE_SHORT, LocalIdParsingState::MetaState },
			{ META_STATE_LONG, LocalIdParsingState::MetaState },
			{ META_CMD_SHORT, LocalIdParsingState::MetaCommand },
			{ META_CMD_LONG, LocalIdParsingState::MetaCommand },
			{ META_TYPE_SHORT, LocalIdParsingState::MetaType },
			{ META_TYPE_LONG, LocalIdParsingState::MetaType },
			{ META_POS_SHORT, LocalIdParsingState::MetaPosition },
			{ META_POS_LONG, LocalIdParsingState::MetaPosition },
			{ META_DETAIL_SHORT, LocalIdParsingState::MetaDetail },
			{ META_DETAIL_LONG, LocalIdParsingState::MetaDetail } };

		//=====================================================================
		// Static helper functions
		//=====================================================================

		/**
		 * @brief Finds the next parsing state indexes for LocalId parsing
		 * @param span The string view representing the current parsing span
		 * @param state The current parsing state
		 * @return A pair containing the custom index and end of custom index positions
		 */
		std::pair<size_t, size_t> nextStateIndexes( std::string_view span, LocalIdParsingState state )
		{
			size_t customIndex = span.find( PREFIX_TILDE );
			size_t endOfCustomIndex = ( customIndex != std::string_view::npos ) ? ( customIndex + PREFIX_TILDE.length() + 1 )
																				: std::string_view::npos;

			size_t metaIndex = span.find( PATH_META );
			size_t endOfMetaIndex = ( metaIndex != std::string_view::npos ) ? ( metaIndex + PATH_META.length() + 1 )
																			: std::string_view::npos;

			bool isVerbose = ( customIndex != std::string_view::npos ) &&
							 ( metaIndex != std::string_view::npos ) &&
							 ( customIndex < metaIndex );

			switch ( state )
			{
				case LocalIdParsingState::PrimaryItem:
				{
					size_t secIndex = span.find( PATH_SEC );
					size_t endOfSecIndex =
						( secIndex != std::string_view::npos ) ? ( secIndex + PATH_SEC.length() + 1 )
															   : std::string_view::npos;

					if ( secIndex != std::string_view::npos )
					{
						return { secIndex, endOfSecIndex };
					}

					if ( isVerbose && customIndex != std::string_view::npos )
					{
						return { customIndex, endOfCustomIndex };
					}

					return { metaIndex, endOfMetaIndex };
				}
				case LocalIdParsingState::SecondaryItem:
				{
					if ( isVerbose && customIndex != std::string_view::npos )
					{
						return { customIndex, endOfCustomIndex };
					}

					return { metaIndex, endOfMetaIndex };
				}
				case LocalIdParsingState::NamingRule:
				case LocalIdParsingState::VisVersion:
				case LocalIdParsingState::ItemDescription:
				case LocalIdParsingState::MetaQuantity:
				case LocalIdParsingState::MetaContent:
				case LocalIdParsingState::MetaCalculation:
				case LocalIdParsingState::MetaState:
				case LocalIdParsingState::MetaCommand:
				case LocalIdParsingState::MetaType:
				case LocalIdParsingState::MetaPosition:
				case LocalIdParsingState::MetaDetail:
				case LocalIdParsingState::EmptyState:
				case LocalIdParsingState::Formatting:
				case LocalIdParsingState::Completeness:
				case LocalIdParsingState::NamingEntity:
				case LocalIdParsingState::IMONumber:
				default:
				{
					return { metaIndex, endOfMetaIndex };
				}
			}
		}

		//=====================================================================
		// Internal parsing implementation
		//=====================================================================

		/**
		 * @brief Advances the parsing index `i` past the current `segment` and advances the parsing state to the next sequential state.
		 * @param[in,out] i The current parsing index within the input string.
		 * @param[in] segment The string view representing the segment just processed.
		 * @param[in,out] state The current parsing state (will be incremented to the next state).
		 */
		static void advanceParser( size_t& i, std::string_view segment, LocalIdParsingState& state )
		{
			state = static_cast<LocalIdParsingState>( static_cast<int>( state ) + 1 );
			i += segment.length() + 1;
		}

		/**
		 * @brief Advances the parsing index `i` past the current `segment` and the following separator '/'.
		 * @param[in,out] i The current parsing index.
		 * @param[in] segment The segment just processed.
		 */
		static void advanceParser( size_t& i, std::string_view segment )
		{
			i += segment.length() + 1;
		}

		/**
		 * @brief Explicitly sets the parsing `state` to the target state `to`.
		 * @param[in,out] state The current parsing state (will be set to `to`).
		 * @param[in] to The target `LocalIdParsingState` to transition to.
		 */
		static void advanceParser( LocalIdParsingState& state, LocalIdParsingState to )
		{
			state = to;
		}

		/**
		 * @brief Advances the parsing index `i` and explicitly sets the parsing `state` to `to`.
		 * @param[in,out] i The current parsing index.
		 * @param[in] segment The segment just processed.
		 * @param[in,out] state The current parsing state (will be set to `to`).
		 * @param[in] to The target `LocalIdParsingState` to transition to.
		 */
		static void advanceParser( size_t& i, std::string_view segment, LocalIdParsingState& state, LocalIdParsingState to )
		{
			i += segment.length() + 1;
			state = to;
		}

		/**
		 * @brief Converts a metadata prefix string (e.g., "q", "qty") to its corresponding `LocalIdParsingState`.
		 * @param[in] prefix The string view representing the metadata prefix.
		 * @return An `std::optional<LocalIdParsingState>` containing the state if the prefix is recognized,
		 *         or `std::nullopt` otherwise.
		 */
		static std::optional<LocalIdParsingState> metaPrefixToState( std::string_view prefix )
		{
			auto it = s_prefixMap.find( prefix );
			return ( it != s_prefixMap.end() )
					   ? std::make_optional( it->second )
					   : std::nullopt;
		}

		/**
		 * @brief Determines the expected next parsing state in the standard metadata sequence.
		 * @param[in] prev The previous `LocalIdParsingState` (must be a metadata state).
		 * @return An `std::optional<LocalIdParsingState>` containing the next expected state,
		 *         or `std::nullopt` if `prev` is the last state in the sequence (e.g., MetaDetail).
		 */
		static std::optional<LocalIdParsingState> nextParsingState( LocalIdParsingState prev )
		{
			switch ( prev )
			{
				case LocalIdParsingState::MetaQuantity:
				{
					return LocalIdParsingState::MetaContent;
				}
				case LocalIdParsingState::MetaContent:
				{
					return LocalIdParsingState::MetaCalculation;
				}
				case LocalIdParsingState::MetaCalculation:
				{
					return LocalIdParsingState::MetaState;
				}
				case LocalIdParsingState::MetaState:
				{
					return LocalIdParsingState::MetaCommand;
				}
				case LocalIdParsingState::MetaCommand:
				{
					return LocalIdParsingState::MetaType;
				}
				case LocalIdParsingState::MetaType:
				{
					return LocalIdParsingState::MetaPosition;
				}
				case LocalIdParsingState::MetaPosition:
				{
					return LocalIdParsingState::MetaDetail;
				}
				case LocalIdParsingState::MetaDetail:
				{
					return std::nullopt;
				}
				case LocalIdParsingState::NamingRule:
				case LocalIdParsingState::VisVersion:
				case LocalIdParsingState::PrimaryItem:
				case LocalIdParsingState::SecondaryItem:
				case LocalIdParsingState::ItemDescription:
				case LocalIdParsingState::EmptyState:
				case LocalIdParsingState::Formatting:
				case LocalIdParsingState::Completeness:
				case LocalIdParsingState::NamingEntity:
				case LocalIdParsingState::IMONumber:
				default:
				{
					return std::nullopt;
				}
			}
		}

		/**
		 * @brief Parses a single metadata tag segment (e.g., "q-value" or "qty-value").
		 * @param[in] codebookName The expected `CodebookName` for this tag.
		 * @param[in,out] state The current parsing state (updated on success).
		 * @param[in,out] i The current parsing index (updated on success).
		 * @param[in] segment The string view representing the full metadata segment (e.g., "q-value").
		 * @param[out] tag Output parameter where the successfully parsed `MetadataTag` is placed.
		 * @param[in] codebooks A pointer to the loaded codebooks, used for validation.
		 * @param[in,out] errorBuilder Used to record errors encountered during parsing.
		 * @return True if the segment was successfully parsed as the expected tag, false otherwise.
		 */
		static bool parseMetaTag(
			CodebookName codebookName, LocalIdParsingState& state, size_t& i, std::string_view segment,
			std::optional<MetadataTag>& tag, const Codebooks* codebooks, LocalIdParsingErrorBuilder& errorBuilder )
		{
			if ( !codebooks )
			{
				return false;
			}

			auto dashIndex = segment.find( localId::PREFIX_DASH );
			auto tildeIndex = segment.find( localId::PREFIX_TILDE );
			auto prefixIndex = ( dashIndex == std::string_view::npos )
								   ? tildeIndex
								   : dashIndex;

			if ( prefixIndex == std::string_view::npos )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.create();

				builder.append( "Invalid metadata tag: missing prefix '" );
				builder.append( localId::PREFIX_DASH );
				builder.append( "' or '" );
				builder.append( localId::PREFIX_TILDE );
				builder.append( "' in " );
				builder.append( segment );

				errorBuilder.addError( state, lease.toString() );
				advanceParser( i, segment, state );

				return true;
			}

			auto actualPrefix = segment.substr( 0, prefixIndex );

			auto actualState = metaPrefixToState( actualPrefix );
			if ( !actualState.has_value() || actualState.value() < state )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.create();
				builder.append( "Invalid metadata tag: unknown prefix " );
				builder.append( actualPrefix );
				errorBuilder.addError( state, lease.toString() );

				return false;
			}

			if ( actualState.value() > state )
			{
				advanceParser( state, actualState.value() );

				return true;
			}

			auto nextState = nextParsingState( actualState.value() );

			auto value = segment.substr( prefixIndex + 1 );
			if ( nfx::string::isEmpty( value ) )
			{
				auto codebookStr = CodebookNames::toString( codebookName );
				errorBuilder.addError( state, "Invalid " + codebookStr + " metadata tag: missing value" );

				return false;
			}

			tag = codebooks->tryCreateTag( codebookName, value );
			if ( !tag.has_value() )
			{
				auto codebookStr = CodebookNames::toString( codebookName );
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.create();

				if ( prefixIndex == tildeIndex )
				{
					builder.append( "Invalid custom " );
					builder.append( codebookStr );
					builder.append( " metadata tag: failed to create " );
					builder.append( value );
					errorBuilder.addError( state, lease.toString() );
				}
				else
				{
					builder.append( "Invalid " );
					builder.append( codebookStr );
					builder.append( " metadata tag: failed to create " );
					builder.append( value );
					errorBuilder.addError( state, lease.toString() );
				}

				advanceParser( i, segment, state );

				return true;
			}

			if ( prefixIndex == dashIndex && tag.value().prefix() == localId::CHAR_TILDE )
			{
				auto codebookStr = CodebookNames::toString( codebookName );
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.create();
				builder.append( "Invalid " );
				builder.append( codebookStr );
				builder.append( " metadata tag: '" );
				builder.append( value );
				builder.append( "'. Use prefix '" );
				builder.append( localId::PREFIX_TILDE );
				builder.append( "' for custom values" );
				errorBuilder.addError( state, lease.toString() );
			}

			if ( !nextState.has_value() )
			{
				advanceParser( i, segment, state );
			}
			else
			{
				advanceParser( i, segment, state, nextState.value() );
			}

			return true;
		}

		/**
		 * @brief Internal core parsing logic used by public `tryParse` methods.
		 * @param[in] localIdStr The complete Local ID string to parse.
		 * @param[in,out] errorBuilder A helper object to accumulate parsing errors.
		 * @param[out] localIdBuilder Output parameter where the successfully parsed builder is placed.
		 * @return True if parsing succeeded (potentially with non-critical errors recorded), false if a critical error occurred.
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		static bool tryParse(
			std::string_view localIdStr,
			LocalIdParsingErrorBuilder& errorBuilder,
			std::optional<LocalIdBuilder>& localIdBuilder )
		{
			localIdBuilder = std::nullopt;

			if ( nfx::string::isEmpty( localIdStr ) )
			{
				return false;
			}

			if ( localIdStr[0] != '/' )
			{
				errorBuilder.addError( LocalIdParsingState::Formatting, "Invalid format: missing '/' as first character" );

				return false;
			}

			std::string_view span = localIdStr;

			std::optional<GmodPath> primaryItem = std::nullopt;
			std::optional<GmodPath> secondaryItem = std::nullopt;
			std::optional<MetadataTag> qty = std::nullopt;
			std::optional<MetadataTag> cnt = std::nullopt;
			std::optional<MetadataTag> calc = std::nullopt;
			std::optional<MetadataTag> stateTag = std::nullopt;
			std::optional<MetadataTag> cmd = std::nullopt;
			std::optional<MetadataTag> type = std::nullopt;
			std::optional<MetadataTag> pos = std::nullopt;
			std::optional<MetadataTag> detail = std::nullopt;
			bool verbose = false;
			std::string predefinedMessage;
			bool invalidSecondaryItem = false;

			size_t primaryItemStart = std::numeric_limits<size_t>::max();
			size_t secondaryItemStart = std::numeric_limits<size_t>::max();

			auto state = LocalIdParsingState::NamingRule;
			size_t i = 1;

			const auto& vis = VIS::instance();
			auto visVersion = VisVersion::Unknown;
			const Gmod* gmod = nullptr;
			const Codebooks* codebooks = nullptr;

			while ( state <= LocalIdParsingState::MetaDetail )
			{
				size_t nextStart = std::min( span.length(), i );
				size_t nextSlashPos = span.substr( nextStart ).find( '/' );
				std::string_view segment = ( nextSlashPos == std::string_view::npos ) ? span.substr( nextStart )
																					  : span.substr( nextStart, nextSlashPos );

				switch ( state )
				{
					case LocalIdParsingState::NamingRule:
					{
						if ( nfx::string::isEmpty( segment ) )
						{
							errorBuilder.addError( LocalIdParsingState::NamingRule, predefinedMessage );
							state = static_cast<LocalIdParsingState>( static_cast<int>( state ) + 1 );
							break;
						}

						if ( !nfx::string::equals( segment, transport::ISO19848_ANNEX_C_NAMING_RULE ) )
						{
							errorBuilder.addError( LocalIdParsingState::NamingRule, predefinedMessage );

							return false;
						}

						advanceParser( i, segment, state );
						break;
					}
					case LocalIdParsingState::VisVersion:
					{
						if ( nfx::string::isEmpty( segment ) )
						{
							errorBuilder.addError( LocalIdParsingState::VisVersion, predefinedMessage );
							state = static_cast<LocalIdParsingState>( static_cast<int>( state ) + 1 );
							break;
						}

						if ( !nfx::string::startsWith( segment, localId::PREFIX_VIS ) )
						{
							errorBuilder.addError( LocalIdParsingState::VisVersion, predefinedMessage );

							return false;
						}

						std::string_view versionStr = segment.substr( localId::PREFIX_VIS.length() );
						if ( !VisVersionExtensions::tryParse( versionStr, visVersion ) )
						{
							errorBuilder.addError( LocalIdParsingState::VisVersion, predefinedMessage );

							return false;
						}

						gmod = &vis.gmod( visVersion );
						codebooks = &vis.codebooks( visVersion );

						if ( !gmod || !codebooks )
						{
							return false;
						}

						advanceParser( i, segment, state );
						break;
					}
					case LocalIdParsingState::PrimaryItem:
					{
						if ( nfx::string::isEmpty( segment ) )
						{
							if ( primaryItemStart != std::numeric_limits<size_t>::max() )
							{
								std::string_view path = span.substr( primaryItemStart, i - 1 - primaryItemStart );
								std::optional<GmodPath> parsedPath;
								if ( !gmod->tryParsePath( path, parsedPath ) )
								{
									parsedPath = std::nullopt;
								}
								if ( !parsedPath )
								{
									auto lease = nfx::string::StringBuilderPool::lease();
									auto builder = lease.create();
									builder.append( "Invalid GmodPath in Primary item: " );
									builder.append( path );
									errorBuilder.addError( LocalIdParsingState::PrimaryItem, lease.toString() );
								}
								else
								{
									primaryItem = std::move( *parsedPath );
								}
							}
							else
							{
								errorBuilder.addError( LocalIdParsingState::PrimaryItem );
							}

							errorBuilder.addError(
								LocalIdParsingState::PrimaryItem,
								"Invalid or missing '/meta' prefix after Primary item" );

							state = static_cast<LocalIdParsingState>( static_cast<int>( state ) + 1 );
							break;
						}

						size_t dashIndex = segment.find( localId::PREFIX_DASH );
						std::string_view code = ( dashIndex == std::string_view::npos )
													? segment
													: segment.substr( 0, dashIndex );

						if ( primaryItemStart == std::numeric_limits<size_t>::max() )
						{
							GmodNode* nodePtr = nullptr;
							if ( !gmod->tryGetNode( code, nodePtr ) )
							{
								auto lease = nfx::string::StringBuilderPool::lease();
								auto builder = lease.create();
								builder.append( "Invalid start GmodNode in Primary item: " );
								builder.append( code );
								errorBuilder.addError( LocalIdParsingState::PrimaryItem, lease.toString() );
							}

							primaryItemStart = i;
							advanceParser( i, segment );
						}
						else
						{
							LocalIdParsingState nextState = state;

							if ( nfx::string::startsWith( segment, localId::PREFIX_SEC ) )
							{
								nextState = LocalIdParsingState::SecondaryItem;
							}
							else if ( nfx::string::startsWith( segment, localId::PREFIX_META ) )
							{
								nextState = LocalIdParsingState::MetaQuantity;
							}
							else if ( !nfx::string::isEmpty( segment ) && segment[0] == localId::CHAR_TILDE )
							{
								nextState = LocalIdParsingState::ItemDescription;
							}

							if ( nextState != state )
							{
								std::string_view path = span.substr( primaryItemStart, i - 1 - primaryItemStart );
								std::optional<GmodPath> parsedPath;
								if ( !gmod->tryParsePath( path, parsedPath ) )
								{
									parsedPath = std::nullopt;
								}
								if ( !parsedPath )
								{
									auto lease = nfx::string::StringBuilderPool::lease();
									auto builder = lease.create();
									builder.append( "Invalid GmodPath in Primary item: " );
									builder.append( path );
									errorBuilder.addError( LocalIdParsingState::PrimaryItem, lease.toString() );

									auto [_, endOfNextStateIndex] = nextStateIndexes( span, state );
									i = endOfNextStateIndex;
									advanceParser( state, nextState );
									break;
								}
								else
								{
									primaryItem = std::move( *parsedPath );
								}

								if ( !nfx::string::isEmpty( segment ) && segment[0] == localId::CHAR_TILDE )
								{
									advanceParser( state, nextState );
								}
								else
								{
									advanceParser( i, segment, state, nextState );
								}
								break;
							}

							GmodNode* nodePtr = nullptr;
							if ( !gmod->tryGetNode( code, nodePtr ) )
							{
								auto lease = nfx::string::StringBuilderPool::lease();
								auto builder = lease.create();
								builder.append( "Invalid GmodNode in Primary item: " );
								builder.append( code );
								errorBuilder.addError( LocalIdParsingState::PrimaryItem, lease.toString() );

								auto [nextStateIndex, endOfNextStateIndex] = nextStateIndexes( span, state );

								if ( nextStateIndex == std::numeric_limits<size_t>::max() )
								{
									errorBuilder.addError(
										LocalIdParsingState::PrimaryItem,
										"Invalid or missing '/meta' prefix after Primary item" );

									return false;
								}

								std::string_view nextSegment = span.substr( nextStateIndex + 1 );

								if ( nfx::string::startsWith( nextSegment, localId::PREFIX_SEC ) )
								{
									nextState = LocalIdParsingState::SecondaryItem;
								}
								else if ( nfx::string::startsWith( nextSegment, localId::PREFIX_META ) )
								{
									nextState = LocalIdParsingState::MetaQuantity;
								}
								else if ( !nfx::string::isEmpty( nextSegment ) && nextSegment[0] == localId::CHAR_TILDE )
								{
									nextState = LocalIdParsingState::ItemDescription;
								}

								std::string_view invalidPrimaryItemPath = span.substr( i, nextStateIndex - i );
								auto lease2 = nfx::string::StringBuilderPool::lease();
								auto builder2 = lease2.create();
								builder2.append( "Invalid GmodPath: Last part in Primary item: " );
								builder2.append( invalidPrimaryItemPath );
								errorBuilder.addError( LocalIdParsingState::PrimaryItem, lease2.toString() );

								i = endOfNextStateIndex;
								advanceParser( state, nextState );
								break;
							}

							advanceParser( i, segment );
						}
						break;
					}
					case LocalIdParsingState::SecondaryItem:
					{
						if ( nfx::string::isEmpty( segment ) )
						{
							state = static_cast<LocalIdParsingState>( static_cast<int>( state ) + 1 );
							break;
						}

						size_t dashIndex = segment.find( localId::PREFIX_DASH );
						std::string_view code = ( dashIndex == std::string_view::npos ) ? segment
																						: segment.substr( 0, dashIndex );

						if ( secondaryItemStart == std::numeric_limits<size_t>::max() )
						{
							GmodNode* nodePtr = nullptr;
							if ( !gmod->tryGetNode( code, nodePtr ) )
							{
								auto lease = nfx::string::StringBuilderPool::lease();
								auto builder = lease.create();
								builder.append( "Invalid start GmodNode in Secondary item: " );
								builder.append( code );
								errorBuilder.addError( LocalIdParsingState::SecondaryItem, lease.toString() );
							}

							secondaryItemStart = i;
							advanceParser( i, segment );
						}
						else
						{
							LocalIdParsingState nextState = state;

							if ( nfx::string::startsWith( segment, localId::PREFIX_META ) )
							{
								nextState = LocalIdParsingState::MetaQuantity;
							}
							else if ( !nfx::string::isEmpty( segment ) && segment[0] == localId::CHAR_TILDE )
							{
								nextState = LocalIdParsingState::ItemDescription;
							}

							if ( nextState != state )
							{
								std::string_view path = span.substr( secondaryItemStart, i - 1 - secondaryItemStart );
								std::optional<GmodPath> parsedPath;
								if ( !gmod->tryParsePath( path, parsedPath ) )
								{
									parsedPath = std::nullopt;
								}
								if ( !parsedPath )
								{
									invalidSecondaryItem = true;
									auto lease = nfx::string::StringBuilderPool::lease();
									auto builder = lease.create();
									builder.append( "Invalid GmodPath in Secondary item: " );
									builder.append( path );
									errorBuilder.addError( LocalIdParsingState::SecondaryItem, lease.toString() );

									auto [_, endOfNextStateIndex] = nextStateIndexes( span, state );
									i = endOfNextStateIndex;
									advanceParser( state, nextState );
									break;
								}
								else
								{
									secondaryItem = std::move( *parsedPath );
								}

								if ( !nfx::string::isEmpty( segment ) && segment[0] == localId::CHAR_TILDE )
								{
									advanceParser( state, nextState );
								}
								else
								{
									advanceParser( i, segment, state, nextState );
								}

								break;
							}

							GmodNode* nodePtr = nullptr;
							if ( !gmod->tryGetNode( code, nodePtr ) )
							{
								invalidSecondaryItem = true;
								auto lease = nfx::string::StringBuilderPool::lease();
								auto builder = lease.create();
								builder.append( "Invalid GmodNode in Secondary item: " );
								builder.append( code );
								errorBuilder.addError( LocalIdParsingState::SecondaryItem, lease.toString() );

								auto [nextStateIndex, endOfNextStateIndex] = nextStateIndexes( span, state );
								if ( nextStateIndex == std::numeric_limits<size_t>::max() )
								{
									errorBuilder.addError( LocalIdParsingState::SecondaryItem, "Invalid or missing '/meta' prefix after Secondary item" );

									return false;
								}

								std::string_view nextSegment = span.substr( nextStateIndex + 1 );

								if ( nfx::string::startsWith( nextSegment, localId::PREFIX_META ) )
								{
									nextState = LocalIdParsingState::MetaQuantity;
								}
								else if ( !nfx::string::isEmpty( nextSegment ) && nextSegment[0] == localId::CHAR_TILDE )
								{
									nextState = LocalIdParsingState::ItemDescription;
								}

								std::string_view invalidSecondaryItemPath = span.substr( i, nextStateIndex - i );
								auto lease2 = nfx::string::StringBuilderPool::lease();
								auto builder2 = lease2.create();
								builder2.append( "Invalid GmodPath: Last part in Secondary item: " );
								builder2.append( invalidSecondaryItemPath );
								errorBuilder.addError( LocalIdParsingState::SecondaryItem, lease2.toString() );

								i = endOfNextStateIndex;
								advanceParser( state, nextState );
								break;
							}

							advanceParser( i, segment );
						}
						break;
					}
					case LocalIdParsingState::ItemDescription:
					{
						if ( nfx::string::isEmpty( segment ) )
						{
							state = static_cast<LocalIdParsingState>( static_cast<int>( state ) + 1 );

							break;
						}

						verbose = true;

						size_t metaIndex = span.find( localId::PATH_META );
						if ( metaIndex == std::string_view::npos )
						{
							errorBuilder.addError( LocalIdParsingState::ItemDescription, predefinedMessage );

							return false;
						}

						segment = span.substr( i, ( metaIndex + localId::PATH_META.length() ) - i );

						advanceParser( i, segment, state );
						break;
					}
					case LocalIdParsingState::MetaQuantity:
					{
						if ( nfx::string::isEmpty( segment ) )
						{
							state = static_cast<LocalIdParsingState>( static_cast<int>( state ) + 1 );

							break;
						}

						bool result = parseMetaTag( CodebookName::Quantity, state, i, segment, qty, codebooks, errorBuilder );
						if ( !result )
						{
							return false;
						}

						break;
					}
					case LocalIdParsingState::MetaContent:
					{
						if ( nfx::string::isEmpty( segment ) )
						{
							state = static_cast<LocalIdParsingState>( static_cast<int>( state ) + 1 );

							break;
						}

						bool result = parseMetaTag( CodebookName::Content, state, i, segment, cnt, codebooks, errorBuilder );
						if ( !result )
						{
							return false;
						}

						break;
					}
					case LocalIdParsingState::MetaCalculation:
					{
						if ( nfx::string::isEmpty( segment ) )
						{
							state = static_cast<LocalIdParsingState>( static_cast<int>( state ) + 1 );

							break;
						}

						bool result = parseMetaTag( CodebookName::Calculation, state, i, segment, calc, codebooks, errorBuilder );
						if ( !result )
						{
							return false;
						}

						break;
					}
					case LocalIdParsingState::MetaState:
					{
						if ( nfx::string::isEmpty( segment ) )
						{
							state = static_cast<LocalIdParsingState>( static_cast<int>( state ) + 1 );

							break;
						}

						bool result = parseMetaTag( CodebookName::State, state, i, segment, stateTag, codebooks, errorBuilder );
						if ( !result )
						{
							return false;
						}

						break;
					}
					case LocalIdParsingState::MetaCommand:
					{
						if ( nfx::string::isEmpty( segment ) )
						{
							state = static_cast<LocalIdParsingState>( static_cast<int>( state ) + 1 );
							break;
						}

						bool result = parseMetaTag( CodebookName::Command, state, i, segment, cmd, codebooks, errorBuilder );
						if ( !result )
						{
							return false;
						}

						break;
					}
					case LocalIdParsingState::MetaType:
					{
						if ( nfx::string::isEmpty( segment ) )
						{
							state = static_cast<LocalIdParsingState>( static_cast<int>( state ) + 1 );

							break;
						}

						bool result = parseMetaTag( CodebookName::Type, state, i, segment, type, codebooks, errorBuilder );
						if ( !result )
						{
							return false;
						}

						break;
					}
					case LocalIdParsingState::MetaPosition:
					{
						if ( nfx::string::isEmpty( segment ) )
						{
							state = static_cast<LocalIdParsingState>( static_cast<int>( state ) + 1 );
							break;
						}

						bool result = parseMetaTag( CodebookName::Position, state, i, segment, pos, codebooks, errorBuilder );
						if ( !result )
						{
							return false;
						}
						break;
					}
					case LocalIdParsingState::MetaDetail:
					{
						if ( nfx::string::isEmpty( segment ) )
						{
							state = static_cast<LocalIdParsingState>( static_cast<int>( state ) + 1 );
							break;
						}

						bool result = parseMetaTag( CodebookName::Detail, state, i, segment, detail, codebooks, errorBuilder );
						if ( !result )
						{
							return false;
						}
						break;
					}
					case LocalIdParsingState::EmptyState:
					case LocalIdParsingState::Formatting:
					case LocalIdParsingState::Completeness:
					case LocalIdParsingState::NamingEntity:
					case LocalIdParsingState::IMONumber:
					default:
					{
						advanceParser( i, segment, state );
						break;
					}
				}
			}

			auto builder = LocalIdBuilder::create( visVersion );

			if ( primaryItem.has_value() )
			{
				builder = builder.tryWithPrimaryItem( primaryItem.value() );
			}
			if ( secondaryItem.has_value() )
			{
				builder = builder.tryWithSecondaryItem( secondaryItem.value() );
			}
			if ( verbose )
			{
				builder = builder.withVerboseMode( verbose );
			}
			if ( qty.has_value() )
			{
				builder = builder.tryWithMetadataTag( qty.value() );
			}
			if ( cnt.has_value() )
			{
				builder = builder.tryWithMetadataTag( cnt.value() );
			}
			if ( calc.has_value() )
			{
				builder = builder.tryWithMetadataTag( calc.value() );
			}
			if ( stateTag.has_value() )
			{
				builder = builder.tryWithMetadataTag( stateTag.value() );
			}
			if ( cmd.has_value() )
			{
				builder = builder.tryWithMetadataTag( cmd.value() );
			}
			if ( type.has_value() )
			{
				builder = builder.tryWithMetadataTag( type.value() );
			}
			if ( pos.has_value() )
			{
				builder = builder.tryWithMetadataTag( pos.value() );
			}
			if ( detail.has_value() )
			{
				builder = builder.tryWithMetadataTag( detail.value() );
			}
			if ( !qty.has_value() &&
				 !cnt.has_value() &&
				 !calc.has_value() &&
				 !stateTag.has_value() &&
				 !cmd.has_value() &&
				 !type.has_value() &&
				 !pos.has_value() &&
				 !detail.has_value() )
			{
				errorBuilder.addError( LocalIdParsingState::Completeness, "No metadata tags specified. Local IDs require atleast 1 metadata tag." );
			}

			localIdBuilder = std::move( builder );

			return ( !errorBuilder.hasError() && !invalidSecondaryItem );
		}

		//=====================================================================
		// String Formatting Helpers
		//=====================================================================

		/**
		 * @brief Appends metadata tag to builder in standard LocalId format.
		 * @details Appends the metadata tag value if present. Does not append anything for empty tags,
		 * @param[in,out] builder StringBuilder to append to.
		 * @param[in] tag Optional metadata tag to append (ignored if empty).
		 */
		static void appendMeta( nfx::string::StringBuilder& builder, const std::optional<MetadataTag>& tag )
		{
			if ( tag.has_value() )
			{
				tag.value().toString( builder );
			}
		}
	} // namespace internal::localId

	//=====================================================================
	// LocalIdBuilder class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	LocalIdBuilder::LocalIdBuilder()
		: m_hashCode{ VISTA_SDK_CPP_HASH_FNV_OFFSET_BASIS }
	{
	}

	//----------------------------------------------
	// Static factory methods
	//----------------------------------------------

	LocalIdBuilder LocalIdBuilder::create( VisVersion version )
	{
		return LocalIdBuilder().withVisVersion( version );
	}

	//----------------------------------------------
	// Build methods (Immutable fluent interface)
	//----------------------------------------------

	//----------------------------
	// Build
	//----------------------------

	LocalId LocalIdBuilder::build() const
	{
		if ( isEmpty() )
		{
			throw std::invalid_argument{ "Cannot build LocalId: builder is empty." };
		}
		if ( !isValid() )
		{
			throw std::invalid_argument{ "Cannot build LocalId: builder state is invalid." };
		}

		LocalIdBuilder result( *this );

		return LocalId( result );
	}

	mqtt::LocalId LocalIdBuilder::buildMqtt() const
	{
		return mqtt::LocalId( *this );
	}

	//----------------------------------------------
	// Verbose mode
	//----------------------------------------------

	LocalIdBuilder LocalIdBuilder::withVerboseMode( bool verboseMode )
	{
		LocalIdBuilder result( *this );
		result.m_verboseMode = verboseMode;
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	//----------------------------------------------
	// VIS version
	//----------------------------------------------

	LocalIdBuilder LocalIdBuilder::withVisVersion( const std::string& visVersionStr )
	{
		bool succeeded;
		auto localIdBuilder = tryWithVisVersion( visVersionStr, succeeded );

		if ( !succeeded )
		{
			throw std::invalid_argument{ "Failed to parse VIS version" };
		}

		return localIdBuilder;
	}

	LocalIdBuilder LocalIdBuilder::withVisVersion( VisVersion version )
	{
		bool succeeded;
		auto localIdBuilder = tryWithVisVersion( version, succeeded );

		if ( !succeeded )
		{
			throw std::invalid_argument{ "withVisVersion" };
		}

		return localIdBuilder;
	}

	LocalIdBuilder LocalIdBuilder::tryWithVisVersion( const std::optional<VisVersion>& version )
	{
		bool succeeded;

		return tryWithVisVersion( version, succeeded );
	}

	LocalIdBuilder LocalIdBuilder::tryWithVisVersion( const std::optional<VisVersion>& version, bool& succeeded )
	{
		succeeded = true;
		LocalIdBuilder result( *this );
		result.m_visVersion = version;
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	LocalIdBuilder LocalIdBuilder::tryWithVisVersion( const std::optional<std::string>& visVersionStr, bool& succeeded )
	{
		if ( visVersionStr.has_value() )
		{
			VisVersion v;
			if ( VisVersionExtensions::tryParse( *visVersionStr, v ) )
			{
				auto localIdBuilder = tryWithVisVersion( v, succeeded );

				return localIdBuilder;
			}
		}

		succeeded = false;

		return LocalIdBuilder( *this );
	}

	LocalIdBuilder LocalIdBuilder::withoutVisVersion()
	{
		LocalIdBuilder result( *this );
		result.m_visVersion = std::nullopt;
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	//----------------------------------------------
	// Primary item
	//----------------------------------------------

	LocalIdBuilder LocalIdBuilder::withPrimaryItem( GmodPath&& item )
	{
		bool succeeded;
		auto localIdBuilder = tryWithPrimaryItem( std::move( item ), succeeded );

		if ( !succeeded )
		{
			throw std::invalid_argument{ "Failed to set primary item: invalid or empty GmodPath." };
		}

		return localIdBuilder;
	}

	LocalIdBuilder LocalIdBuilder::tryWithPrimaryItem( GmodPath&& item )
	{
		bool succeeded;

		return tryWithPrimaryItem( std::move( item ), succeeded );
	}

	LocalIdBuilder LocalIdBuilder::tryWithPrimaryItem( GmodPath&& item, bool& succeeded )
	{
		if ( item.length() == 0 )
		{
			succeeded = false;

			return LocalIdBuilder( *this );
		}

		succeeded = true;
		LocalIdBuilder result( *this );
		result.m_items = LocalIdItems( std::move( result.m_items ), std::move( item ) );
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	LocalIdBuilder LocalIdBuilder::tryWithPrimaryItem( std::optional<GmodPath>&& item )
	{
		bool succeeded;

		return tryWithPrimaryItem( std::move( item ), succeeded );
	}

	LocalIdBuilder LocalIdBuilder::tryWithPrimaryItem( std::optional<GmodPath>&& item, bool& succeeded )
	{
		if ( !item.has_value() )
		{
			succeeded = false;

			return LocalIdBuilder( *this );
		}

		succeeded = true;
		LocalIdBuilder result( *this );
		result.m_items = LocalIdItems( std::move( result.m_items ), std::move( *item ) );
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	LocalIdBuilder LocalIdBuilder::withoutPrimaryItem()
	{
		LocalIdBuilder result( *this );
		result.m_items = LocalIdItems{};
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	//----------------------------------------------
	// Secondary item
	//----------------------------------------------

	LocalIdBuilder LocalIdBuilder::withSecondaryItem( GmodPath&& item )
	{
		bool succeeded;
		auto localIdBuilder = tryWithSecondaryItem( std::move( item ), succeeded );

		if ( !succeeded )
		{
			throw std::invalid_argument{ "Failed to set secondary item: invalid or empty GmodPath." };
		}

		return localIdBuilder;
	}

	LocalIdBuilder LocalIdBuilder::tryWithSecondaryItem( GmodPath&& item )
	{
		bool succeeded;

		return tryWithSecondaryItem( std::move( item ), succeeded );
	}

	LocalIdBuilder LocalIdBuilder::tryWithSecondaryItem( GmodPath&& item, bool& succeeded )
	{
		if ( item.length() == 0 )
		{
			succeeded = false;

			return LocalIdBuilder( *this );
		}

		succeeded = true;
		LocalIdBuilder result( *this );
		result.m_items = LocalIdItems( std::move( result.m_items ), std::make_optional( std::move( item ) ) );
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	LocalIdBuilder LocalIdBuilder::tryWithSecondaryItem( std::optional<GmodPath>&& item )
	{
		bool succeeded;

		return tryWithSecondaryItem( std::move( item ), succeeded );
	}

	LocalIdBuilder LocalIdBuilder::tryWithSecondaryItem( std::optional<GmodPath>&& item, bool& succeeded )
	{
		if ( !item.has_value() )
		{
			succeeded = false;

			return LocalIdBuilder( *this );
		}

		succeeded = true;
		LocalIdBuilder result( *this );
		result.m_items = LocalIdItems( std::move( result.m_items ), std::move( item ) );
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	LocalIdBuilder LocalIdBuilder::withoutSecondaryItem()
	{
		LocalIdBuilder result( *this );

		result.m_items = LocalIdItems( std::move( result.m_items ), std::nullopt );
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	//----------------------------------------------
	// Metadata tags
	//----------------------------------------------

	LocalIdBuilder LocalIdBuilder::withMetadataTag( const MetadataTag& metadataTag )
	{
		bool succeeded;
		auto localIdBuilder = tryWithMetadataTag( metadataTag, succeeded );

		if ( !succeeded )
		{
			throw std::invalid_argument{
				"Invalid metadata codebook name: " + CodebookNames::toPrefix( metadataTag.name() ) };
		}

		return localIdBuilder;
	}

	LocalIdBuilder LocalIdBuilder::tryWithMetadataTag( const std::optional<MetadataTag>& metadataTag )
	{
		bool succeeded;

		return tryWithMetadataTag( metadataTag, succeeded );
	}

	LocalIdBuilder LocalIdBuilder::tryWithMetadataTag( const std::optional<MetadataTag>& metadataTag, bool& succeeded )
	{
		if ( !metadataTag.has_value() )
		{
			succeeded = false;

			return LocalIdBuilder( *this );
		}

		switch ( metadataTag->name() )
		{
			case CodebookName::Quantity:
			{
				succeeded = true;
				return withQuantity( *metadataTag );
			}
			case CodebookName::Content:
			{
				succeeded = true;
				return withContent( *metadataTag );
			}
			case CodebookName::Calculation:
			{
				succeeded = true;
				return withCalculation( *metadataTag );
			}
			case CodebookName::State:
			{
				succeeded = true;
				return withState( *metadataTag );
			}
			case CodebookName::Command:
			{
				succeeded = true;
				return withCommand( *metadataTag );
			}
			case CodebookName::Type:
			{
				succeeded = true;
				return withType( *metadataTag );
			}
			case CodebookName::Position:
			{
				succeeded = true;
				return withPosition( *metadataTag );
			}
			case CodebookName::Detail:
			{
				succeeded = true;
				return withDetail( *metadataTag );
			}
			case CodebookName::FunctionalServices:
			case CodebookName::MaintenanceCategory:
			case CodebookName::ActivityType:
			default:
			{
				succeeded = false;
				return LocalIdBuilder( *this );
			}
		}
	}

	LocalIdBuilder LocalIdBuilder::withoutMetadataTag( CodebookName name )
	{
		switch ( name )
		{
			case CodebookName::Quantity:
			{
				return withoutQuantity();
			}
			case CodebookName::Content:
			{
				return withoutContent();
			}
			case CodebookName::Calculation:
			{
				return withoutCalculation();
			}
			case CodebookName::State:
			{
				return withoutState();
			}
			case CodebookName::Command:
			{
				return withoutCommand();
			}
			case CodebookName::Type:
			{
				return withoutType();
			}
			case CodebookName::Position:
			{
				return withoutPosition();
			}
			case CodebookName::Detail:
			{
				return withoutDetail();
			}
			case CodebookName::FunctionalServices:
			case CodebookName::MaintenanceCategory:
			case CodebookName::ActivityType:
			default:
			{
				return LocalIdBuilder( *this );
			}
		}
	}

	//----------------------------------------------
	// Specific metadata tag builder methods
	//----------------------------------------------

	//----------------------------
	// Quantity
	//----------------------------

	LocalIdBuilder LocalIdBuilder::withQuantity( const MetadataTag& quantity )
	{
		LocalIdBuilder result( *this );
		result.m_quantity = quantity;
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	LocalIdBuilder LocalIdBuilder::withoutQuantity()
	{
		LocalIdBuilder result( *this );
		result.m_quantity = std::nullopt;
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	//----------------------------
	// Content
	//----------------------------

	LocalIdBuilder LocalIdBuilder::withContent( const MetadataTag& content )
	{
		LocalIdBuilder result( *this );
		result.m_content = content;
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	LocalIdBuilder LocalIdBuilder::withoutContent()
	{
		LocalIdBuilder result( *this );
		result.m_content = std::nullopt;
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	//----------------------------
	// Calculation
	//----------------------------

	LocalIdBuilder LocalIdBuilder::withCalculation( const MetadataTag& calculation )
	{
		LocalIdBuilder result( *this );
		result.m_calculation = calculation;
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	LocalIdBuilder LocalIdBuilder::withoutCalculation()
	{
		LocalIdBuilder result( *this );
		result.m_calculation = std::nullopt;
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	//----------------------------
	// State
	//----------------------------

	LocalIdBuilder LocalIdBuilder::withState( const MetadataTag& state )
	{
		LocalIdBuilder result( *this );
		result.m_state = state;
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	LocalIdBuilder LocalIdBuilder::withoutState()
	{
		LocalIdBuilder result( *this );
		result.m_state = std::nullopt;
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	//----------------------------
	// Command
	//----------------------------

	LocalIdBuilder LocalIdBuilder::withCommand( const MetadataTag& command )
	{
		LocalIdBuilder result( *this );
		result.m_command = command;
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	LocalIdBuilder LocalIdBuilder::withoutCommand()
	{
		LocalIdBuilder result( *this );
		result.m_command = std::nullopt;
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	//----------------------------
	// Type
	//----------------------------

	LocalIdBuilder LocalIdBuilder::withType( const MetadataTag& type )
	{
		LocalIdBuilder result( *this );
		result.m_type = type;
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	LocalIdBuilder LocalIdBuilder::withoutType()
	{
		LocalIdBuilder result( *this );
		result.m_type = std::nullopt;
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	//----------------------------
	// Position
	//----------------------------

	LocalIdBuilder LocalIdBuilder::withPosition( const MetadataTag& position )
	{
		LocalIdBuilder result( *this );
		result.m_position = position;
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	LocalIdBuilder LocalIdBuilder::withoutPosition()
	{
		LocalIdBuilder result( *this );
		result.m_position = std::nullopt;
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	//----------------------------
	// Detail
	//----------------------------

	LocalIdBuilder LocalIdBuilder::withDetail( const MetadataTag& detail )
	{
		LocalIdBuilder result( *this );
		result.m_detail = detail;
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	LocalIdBuilder LocalIdBuilder::withoutDetail()
	{
		LocalIdBuilder result( *this );
		result.m_detail = std::nullopt;
		result.m_hashCode = internal::localId::computeHash(
			result.m_items, result.m_quantity, result.m_calculation, result.m_content,
			result.m_position, result.m_state, result.m_command, result.m_type, result.m_detail );

		return result;
	}

	//----------------------------------------------
	// String conversion
	//----------------------------------------------

	void LocalIdBuilder::toString( nfx::string::StringBuilder& builder ) const
	{
		if ( !m_visVersion.has_value() )
		{
			throw std::invalid_argument{ "No VisVersion configured on LocalId" };
		}

		// Naming rule prefix: "/dnv-v2"
		builder.append( "/" );
		builder.append( transport::ISO19848_ANNEX_C_NAMING_RULE );
		builder.append( "/" );

		// VIS version: "vis-{major}-{minor}{patch}"
		builder.append( VisVersionExtensions::toVersionString( *m_visVersion ) );
		builder.append( "/" );

		// Items section: primary item [+ secondary item] [+ description]
		m_items.append( builder, m_verboseMode );

		// Metadata section prefix: "/meta"
		builder.append( "meta/" );

		// Metadata tags: {prefix}{separator}{value}
		internal::localId::appendMeta( builder, m_quantity );
		internal::localId::appendMeta( builder, m_content );
		internal::localId::appendMeta( builder, m_calculation );
		internal::localId::appendMeta( builder, m_state );
		internal::localId::appendMeta( builder, m_command );
		internal::localId::appendMeta( builder, m_type );
		internal::localId::appendMeta( builder, m_position );
		internal::localId::appendMeta( builder, m_detail );

		// Cleanup trailing slash
		if ( builder.length() > 0 && builder[builder.length() - 1] == '/' )
		{
			builder.resize( builder.length() - 1 );
		}
	}

	//----------------------------------------------
	// Static parsing methods
	//----------------------------------------------

	LocalIdBuilder LocalIdBuilder::parse( std::string_view localIdStr )
	{
		std::optional<LocalIdBuilder> localId;
		ParsingErrors errors;

		if ( !tryParse( localIdStr, errors, localId ) )
		{
			auto lease = nfx::string::StringBuilderPool::lease();
			auto builder = lease.create();
			builder.append( "Couldn't parse local ID from: '" );
			builder.append( localIdStr );
			builder.append( "'.\n" );
			builder.append( errors.toString() );

			throw std::invalid_argument{ lease.toString() };
		}

		return std::move( *localId );
	}

	bool LocalIdBuilder::tryParse( std::string_view localIdStr, std::optional<LocalIdBuilder>& localId )
	{
		ParsingErrors dummyErrors;

		return tryParse( localIdStr, dummyErrors, localId );
	}

	bool LocalIdBuilder::tryParse( std::string_view localIdStr, ParsingErrors& errors, std::optional<LocalIdBuilder>& localId )
	{
		localId = std::nullopt;
		auto errorBuilder = internal::LocalIdParsingErrorBuilder::create();
		bool success = internal::localId::tryParse( localIdStr, errorBuilder, localId );

		errors = errorBuilder.build();

		return success;
	}
} // namespace dnv::vista::sdk
