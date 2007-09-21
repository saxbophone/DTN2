/*
 *    Copyright 2006 SPARTA Inc
 * 
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 * 
 *        http://www.apache.org/licenses/LICENSE-2.0
 * 
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef _CIPHERSUITE_PS2_H_
#define _CIPHERSUITE_PS2_H_

#ifdef BSP_ENABLED

#include "bundling/BlockProcessor.h"
#include "PS_BlockProcessor.h"
#include <oasys/util/ScratchBuffer.h>

typedef oasys::ScratchBuffer<u_char*, 64> DataBuffer;

namespace dtn {
struct PrimaryBlock_ex;

/**
 * Block processor implementation for the bundle authentication block.
 */
class Ciphersuite_PS2 : public Ciphersuite {
public:
    /// Constructor
    Ciphersuite_PS2();
    
    virtual u_int16_t   cs_num();
    
    /// @{ Virtual from BlockProcessor
    /**
     * First callback for parsing blocks that is expected to append a
     * chunk of the given data to the given block. When the block is
     * completely received, this should also parse the block into any
     * fields in the bundle class.
     *
     * The base class implementation parses the block preamble fields
     * to find the length of the block and copies the preamble and the
     * data in the block's contents buffer.
     *
     * This and all derived implementations must be able to handle a
     * block that is received in chunks, including cases where the
     * preamble is split into multiple chunks.
     *
     * @return the amount of data consumed or -1 on error
     */
    virtual int consume(Bundle* bundle, BlockInfo* block,
                        u_char* buf, size_t len);

    /**
     * Validate the block. This is called after all blocks in the
     * bundle have been fully received.
     *
     * @return true if the block passes validation
     */
    virtual bool validate(const Bundle* bundle, BlockInfoVec*  block_list, BlockInfo* block,
                     BundleProtocol::status_report_reason_t* reception_reason,
                     BundleProtocol::status_report_reason_t* deletion_reason);

    /**
     * First callback to generate blocks for the output pass. The
     * function is expected to initialize an appropriate BlockInfo
     * structure in the given BlockInfoVec.
     *
     * The base class simply initializes an empty BlockInfo with the
     * appropriate owner_ pointer.
     */
    virtual int prepare(const Bundle*    bundle,
                         BlockInfoVec*    xmit_blocks,
                         const BlockInfo* source,
                         const LinkRef&   link,
                         BlockInfo::list_owner_t list);
    
    /**
     * Second callback for transmitting a bundle. This pass should
     * generate any data for the block that does not depend on other
     * blocks' contents.
     */
    virtual int generate(const Bundle* 	bundle,
                          BlockInfoVec*     xmit_blocks,
                          BlockInfo*    	block,
                          const LinkRef&    link,
                          bool          	last);
    
    /**
     * Third callback for transmitting a bundle. This pass should
     * generate any data (such as security signatures) for the block
     * that may depend on other blocks' contents.
     *
     * The base class implementation does nothing. 
     */
    virtual int finalize(const Bundle*  bundle, 
                          BlockInfoVec*  xmit_blocks, 
                          BlockInfo*     block, 
                          const LinkRef& link);

    /**
     * Callback for digesting data. 
     */
    static void digest(const Bundle* bundle, 
                             const BlockInfo* caller_block,
                             const BlockInfo* target_block,
                             const void* buf, 
                             size_t len,
                             OpaqueContext* r);

    /**
     * Internal call for digesting data. 
     */
    static void create_digest(const Bundle* bundle, 
                            BlockInfoVec*  block_list,
                          	BlockInfo*     block,
                          	DataBuffer&    db);

    /**
     * Internal call to interpret primary block fields
     */
	static int read_primary(const Bundle*		bundle, 
								BlockInfo* 		block,
								PrimaryBlock_ex&	primary,
								char**          dict);
    
    /**
     * General hook to set up a block with the given contents. Used
     * for testing generic extension blocks.
     */
    void init_block(BlockInfo* block, u_int8_t type, u_int8_t flags,
                    u_char* bp, size_t len);
    
    /**
     * Ciphersuite number
     */
    enum { CSNUM_PS2 		= 2 };      
    
    /// @}
};

} // namespace dtn

#endif /* BSP_ENABLED */

#endif /* _CIPHERSUITE_PS2_H_ */