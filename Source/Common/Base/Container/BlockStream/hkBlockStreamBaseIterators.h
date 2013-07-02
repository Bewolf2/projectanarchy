/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
//HK_REFLECTION_PARSER_EXCLUDE_FILE

#ifndef HK_BLOCKSTREAM_ITERATORS_H
#define HK_BLOCKSTREAM_ITERATORS_H

#include <Common/Base/Container/BlockStream/hkBlockStreamBaseBlock.h>
#include <Common/Base/Container/BlockStream/hkBlockStreamBaseStream.h>

namespace hkBlockStreamBase
{
	/// A reader is a read-only iterator that goes through blocks from a stream and can return the elements it stores.	
	/// This serves as the base class for the templated version hkBlockStream<TYPE>::Reader and is intended
	/// for internal use only. On SPU it fetches several blocks at a time in a cache to avoid to many DMA calls.
	class Reader
	{
		public:

			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BASE, Reader );

			HK_FORCE_INLINE	Reader();
			HK_FORCE_INLINE ~Reader();

			//
			// Getters and setters
			//

			/// Returns the number of elements left to read in the current block
			HK_FORCE_INLINE int getNumUnreadElementsInThisBlock() const;

			/// Set the reader to 
			HK_FORCE_INLINE void setEmpty(); 

			/// Sets the reader to the start of a stream (on SPU, assumes stream is an SPU pointer)
			void setToStartOfStream( const Stream* stream );

			/// Sets the reader to the start of a range. (on SPU,  assume range is a PPU pointer)
			void setToRange( const Range* range );
			
			//
			// SPU memory management functions
			//

	#if defined(HK_PLATFORM_SPU)
			/// Needs to be called before setToStartOfStream() / setToRange().
			/// \a numActiveBlocks is the number of blocks you want to have access to at the same time
		#if defined(HK_PLATFORM_SIM)
			void initSpu( int dmaGroup, int numActiveBlocks, const char* what );
		#else
			protected:	void initSpu( int dmaGroup, int numActiveBlocks );
			public:		HK_FORCE_INLINE void initSpu( int dmaGroup, int numActiveBlocks, const char* what )	{ initSpu(dmaGroup, numActiveBlocks); }
		#endif

			/// Must be the last function called before the destructor
			void exitSpu();

			/// Converts the current SPU pointer returned by our last access method to the PPU version
			template<typename T>
			HK_FORCE_INLINE T* spuToPpu(T* t) const { HK_ASSERT( 0xf03df590, unsigned(hkGetByteOffsetInt(m_currentBlock.val(), t)) < sizeof(Block)); return (T*)hkAddByteOffset( m_currentBlockPpu.val(), hkGetByteOffsetInt(m_currentBlock.val(), t) ); }

	#else
			void initSpu( int dmaGroup, int numActiveBlocks, const char* what ) {}
			void exitSpu() {}

			template<typename T>
			HK_FORCE_INLINE T* spuToPpu(T* t) const { HK_ASSERT( 0xf03df590, unsigned(hkGetByteOffsetInt(m_currentBlock, t)) < sizeof(Block)); return t; }
	#endif


		protected:

			/// Advance and gives read-only access the next element, assumes the current entry is valid
			HK_FORCE_INLINE	const void* advanceAndAccessNext( int thisElemSize );

			/// Returns the current element (read-only).
			HK_FORCE_INLINE	const void* access();

			/// Advances to the next block in the stream.
			const void* advanceToNewBlock();

		protected:

			friend class Range;

			/// Pointer to the current block 
			HK_PAD_ON_SPU(const Block*)	m_currentBlock;

			/// Pointer to the current element in the block. 
			HK_PAD_ON_SPU(const char*)  m_currentByteLocation;
			
			/// Number of elements left to read in other blocks, to know whether to prefetch next block
			HK_PAD_ON_SPU(int) m_numElementsToReadInOtherBlocks;

			/// Number of elements left to read in the block.
			HK_PAD_ON_SPU(int) m_numElementsToReadInThisBlock;

		#if defined(HK_PLATFORM_SPU)
			/// Internal Block cache on SPU
			HK_PAD_ON_SPU(Block*)	m_blockBuffer;

			/// The current block on the PPU, needed by the consumer
			HK_PAD_ON_SPU(Block*)   m_currentBlockPpu;

			/// Size of the cache
			HK_PAD_ON_SPU(int)		m_numBlocksInBuffer;

			/// The next buffer index into m_blockBuffer which is currently DMA'd, negative if not exists
			HK_PAD_ON_SPU(int)		m_nextBuffer;

			/// Group index for DMA operations.
			HK_PAD_ON_SPU(int)		m_dmaGroup;
		#endif
	};

	/// A Writer is an iterator that adds data in a stream, allocating new blocks into the stream on the fly.
	/// This serves as the base class for the templated version hkBlockStream<TYPE>::Writer and is intended
	/// for internal use only. On SPU it fetches several blocks at a time in a cache to avoid to many DMA calls.
	class Writer
	{
		public:

			friend class Range;

		#if defined(HK_PLATFORM_SPU)
			enum { MAX_NUM_ACTIVE_BLOCKS = 32 };
		#endif

		public:

			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BASE, Writer );

			/// Constructor
			HK_FORCE_INLINE Writer();

			/// Destructor
			HK_FORCE_INLINE ~Writer();

			//
			// Initializers.
			//

			/// Set to the start of an existing stream to write data in the stream from the beginning.
			void setToStartOfStream( Stream::Allocator* allocator, Stream* blockStreamPpu);

			/// Set to the end to an existing stream to append data
			void setToEndOfStream( Stream::Allocator* allocator, Stream* blockStreamPpu);
			
			//
			// Advance functions.
			//

			/// Advance by numBytes. This will move the write location in the current block.
			HK_FORCE_INLINE void advance( int numBytes );

			/// Reserve and advance a number of reservations in one go.
			/// This is faster than calling reserve and advance for large numbers of reservations, however, 
			/// no addresses are returned for writing.
			void batchReserveAndAdvance( int numReservations, int numBytesPerReservation );

			/// Call this method to put the stream back in a consistent and trimmed state.
			/// On SPU this will trigger the (final) write-back to PPU memory.
			void finalize();

			/// Returns the total number of elements in the writer's stream. This method should only be called
			/// after a call to finalize().
			HK_FORCE_INLINE int getTotalNumElems();

		#if defined(HK_DEBUG)
			HK_FORCE_INLINE void checkConsistency()
			{
				if (m_blockStream) { m_blockStream->checkConsistency(); }
			}
		#endif

			//
			// SPU memory management.
			// 
		#if defined(HK_PLATFORM_SPU)

			#if	defined(HK_PLATFORM_SIM)

			/// Needs to be called before setToStartOfStream() / setToRange().
			/// \a numActiveBlocks is the number of blocks you want to have access to at the same time
			void initSpu( int dmaGroup, int numActiveBlocks, const char* what );

			#else

		protected:

			void initSpu( int dmaGroup, int numActiveBlocks );

		public:

			/// Needs to be called before setToStartOfStream() / setToRange().
			/// \a numActiveBlocks is the number of blocks you want to have access to at the same time
			HK_FORCE_INLINE void initSpu( int dmaGroup, int numActiveBlocks, const char* what )
			{
				initSpu(dmaGroup, numActiveBlocks) ;
			}

			#endif	// HK_PLATFORM_SIM

			/// Must be the last function called before the destructor
			HK_FORCE_INLINE void exitSpu();

			/// Use this method to translate the spu address returned by the most recent call to reserveBytes() to
			/// its PPU equivalent.
			template<typename T>
			HK_FORCE_INLINE T* spuToPpu(T* t) const
			{
				HK_ASSERT( 0xf03df590, unsigned(hkGetByteOffsetInt(m_currentBlock, t)) < sizeof(Block) );
				return hkAddByteOffset( t, m_spuToPpuOffset );
			}

			/// checks if our PPU address has not written to memory yet
			HK_FORCE_INLINE void checkIsStillInSpuBuffer( void* ppuAddress );

			/// returns the block header on SPU of a given PPU block. Note that the header can
			/// either be in local buffer or on the PPU
			HK_FORCE_INLINE const Block* getBlockHeaderOnSpu( const Block* ppu, char buffer[Block::BLOCK_HEADER_SIZE] ) const;

		#endif // HK_PLATFORM_SPU

		protected:

			/// Will make sure that the next reserve will access a new block. It is used to create exclusive ranges.
			HK_FORCE_INLINE void setCurrentBlockFull();

			/// Return the address where we are allowed to write \a numBytes number of bytes to. This method will move
			/// to the next block in the stream if the free storage space in the current block is not enough.
			HK_FORCE_INLINE	void* reserveBytes( int numBytes );

			/// Allocates a new block, appends it to the end of the stream and returns the pointer to the new
			/// block's storage space.
			void *allocateAndAccessNewBlock();

			/// Advance by \a currentNumBytes in the stream and return a pointer to storage space where
			/// \a reservedNumBytes can be written to.
			HK_FORCE_INLINE	void* advanceAndReserveNext( int currentNumBytes, int reservedNumBytes );

			/// Wrap up the current block and link the next block. Make sure that \a thisBlock is actually pointing to
			/// the last block in the stream!
			HK_FORCE_INLINE void finalizeLastBlock( Block* HK_RESTRICT thisBlock, Block* HK_RESTRICT nextBlock, int thisBlockNumElements, int bytesUsed );

		public:

			/// The block stream this Writer is working on.
			HK_PAD_ON_SPU(Stream*)				m_blockStream;

		protected:

			/// The current block data is written to.
			HK_PAD_ON_SPU(Block*)				m_currentBlock;

			/// The current byte location where data is written to in the current block (m_currentBlock). 
			/// This value is an offset into the current block and NOT an absolute address.
			HK_PAD_ON_SPU(int)					m_currentByteLocation;	

			/// The number of elements stored in the current block.
			HK_PAD_ON_SPU(int)					m_currentBlockNumElems;	
			
			/// The underlying thread allocator that will allocate blocks when writing.
			HK_PAD_ON_SPU(Stream::Allocator*)	m_tlAllocator;

			/// Debugging flag
			hkBool								m_finalized;		

			/// Debugging value : the numBytes used in the last call to reserveBytes()
			int									m_accessSize;		

		#if defined(HK_PLATFORM_SPU)

			/// A contiguous memory block on SPU to hold a user-defined number of stream blocks. If more than one Block
			/// is allocated here they are used in a ring-fashion, allowing for double (or more) buffering.
			HK_PAD_ON_SPU(Block*)				m_blockBuffer;		

			/// The PPU addresses of all local SPU blocks (which are held in m_blockBuffer).
			Block*								m_blocksPpu[MAX_NUM_ACTIVE_BLOCKS];	

			/// The number of allocated Blocks in local SPU memory.
			HK_PAD_ON_SPU(int)					m_numBlocksInBuffer;

			/// The index of the Block (in m_blockBuffer) which has its DMA transfer back to PPU initiated last.
			HK_PAD_ON_SPU(int)					m_nextBuffer;

			/// For the current block, add this offset to an element's SPU address to get its PPU equivalent.
			HK_PAD_ON_SPU(int)					m_spuToPpuOffset;

			/// The DMA group used for putting the stream's data (i.e. blocks) back to PPU.
			HK_PAD_ON_SPU(int)					m_dmaGroup;
	#endif
	};

	/// A Consumer inherits from the Reader iterator and adds the additional effect of freeing the block after having read
	/// its last element.
	class Consumer : public Reader
	{
		public:

			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BASE, Consumer );

			HK_FORCE_INLINE	Consumer() {}

			/// Sets the consumer to the first block of a stream.
			void setToStartOfStream( Stream::Allocator* allocator, Stream* stream, Stream* m_blockStreamPpu );
			
			/// Set to a whole range.
			void setToRange( Stream::Allocator* allocator, Stream* stream, Stream* m_blockStreamPpu, const Range* range );

		protected:

			/// Advance and access the next element, freeing the old data. Assumes the current entry is valid.
			HK_FORCE_INLINE	const void* consumeAndAccessNext( int thisElemSize );

			/// Frees current block and advance to next one.
			const void* freeAndAdvanceToNewBlock();

			/// Non-const accessors (casts the const away).
			HK_FORCE_INLINE Block* getCurrentBlock();
			HK_FORCE_INLINE char*  getCurrentByteLocation();

		protected:

			friend class Range;

			Stream* m_blockStream;	// points to SPU version
			HK_PAD_ON_SPU(int) m_numElementsToFreeInThisBlock;
			HK_PAD_ON_SPU(Stream::Allocator*) m_allocator;
	};


	/// Modifier inherits from reader but allows to get read-write access to modify its current element.
	class Modifier : public Reader
	{
		public:

			HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_BASE, Modifier );

			HK_FORCE_INLINE	Modifier() {}

			/// Sets the modifier to the first block of a stream.
			HK_FORCE_INLINE void setToStartOfStream( Stream::Allocator* allocator, Stream* stream, Stream* m_blockStreamPpu );

			/// Set to a whole range. 
			HK_FORCE_INLINE void setToRange( const Range* range );

		protected:

			/// Advance and gives read-write access the next element, assumes the current entry is valid
			HK_FORCE_INLINE	void* advanceAndAccessNext( int thisElemSize );

			/// Read-write access to the current element.
			HK_FORCE_INLINE	void* access();

			/// Advances to the next block (when we are at the end of a block).
			void* advanceToNewBlock();

			/// Non-const accessors (casts the const away).
			Block* getCurrentBlock();
			char*  getCurrentByteLocation();

		protected:

			#if defined(HK_PLATFORM_SPU)
			HK_PAD_ON_SPU(int) m_writeBackBlockStartOffset;
			#endif
			HK_PAD_ON_SPU(Stream::Allocator*) m_allocator;
	};
}

#include <Common/Base/Container/BlockStream/hkBlockStreamBaseIterators.inl>

#endif// HK_BLOCKSTREAM_ITERATORS_H

/*
 * Havok SDK - Base file, BUILD(#20130624)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available from salesteam@havok.com.
 * 
 */
