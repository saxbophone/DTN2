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

#ifndef _CIPHERSUITE_ES_H_
#define _CIPHERSUITE_ES_H_

#ifdef BSP_ENABLED

#include "gcm/gcm.h"
#include "bundling/Bundle.h"
#include "EVP_PK.h"
#include "Ciphersuite_enc.h"
#include "BP_Local_CS.h"

namespace dtn {

/**
 * Ciphersuite implementation for the extension security block.
 */
class Ciphersuite_ES : public Ciphersuite_enc {
public:
    Ciphersuite_ES();

    virtual u_int16_t cs_num() = 0;

    virtual bool validate(const Bundle*           bundle,
                          BlockInfoVec*           block_list,
                          BlockInfo*              block,
                          status_report_reason_t* reception_reason,
                          status_report_reason_t* deletion_reason);

    virtual int prepare(const Bundle*    bundle,
                        BlockInfoVec*    xmit_blocks,
                        const BlockInfo* source,
                        const LinkRef&   link,
                        list_owner_t     list);

    virtual int generate(const Bundle*  bundle,
                         BlockInfoVec*  xmit_blocks,
                         BlockInfo*     block,
                         const LinkRef& link,
                         bool           last);

    virtual int finalize(const Bundle*  bundle,
                         BlockInfoVec*  xmit_blocks,
                         BlockInfo*     block,
                         const LinkRef& link);

    virtual int encrypt(
                        std::string       security_dest,
                        const LocalBuffer&       input,
                        LocalBuffer&       result) = 0;

     virtual int decrypt(
                         std::string   security_dest,
                         const LocalBuffer&   enc_data,
                         LocalBuffer&   result) = 0;

    BSPProtectionInfo bsp;
    /**
     * Callback for encrypt/decrypt a block. This is normally
     * used for handling the payload contents.
     */
    static bool do_crypt(const Bundle*    bundle,
                         const BlockInfo* caller_block,
                         BlockInfo*       target_block,
                         void*            buf,
                         size_t           len,
                         OpaqueContext*   r);

    virtual u_int32_t get_key_len() {
    	return 128/8;
    };
    const static int nonce_len = 12;
    const static int salt_len  = 4;
    const static int iv_len    = nonce_len - salt_len;
    const static int tag_len   = 128/8;

    static bool esbable(int type);

protected:
    bool should_be_encapsulated(BlockInfo* block);



};

} // namespace dtn

#endif /* BSP_ENABLED */

#endif /* _CIPHERSUITE_ES_H_ */
