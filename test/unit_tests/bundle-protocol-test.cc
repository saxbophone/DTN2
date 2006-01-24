/*
 * IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING. By
 * downloading, copying, installing or using the software you agree to
 * this license. If you do not agree to this license, do not download,
 * install, copy or use the software.
 * 
 * Intel Open Source License 
 * 
 * Copyright (c) 2004 Intel Corporation. All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 
 *   Neither the name of the Intel Corporation nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *  
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <oasys/util/UnitTest.h>

#include "bundling/Bundle.h"
#include "bundling/BundleProtocol.h"

using namespace oasys;
using namespace dtn;

bool
protocol_test(Bundle* b1)
{
    u_char buf[32768];
    int encode_len, decode_len;

    encode_len = BundleProtocol::format_headers(b1, buf, sizeof(buf));
    CHECK(encode_len != -1);
    
    Bundle* b2 = new Bundle(0, BundlePayload::NODATA);
    decode_len = BundleProtocol::parse_headers(b2, buf, encode_len);
    CHECK_EQUAL(decode_len, encode_len);

    CHECK_EQUALSTR(b1->source_.c_str(),    b2->source_.c_str());
    CHECK_EQUALSTR(b1->dest_.c_str(),      b2->dest_.c_str());
    CHECK_EQUALSTR(b1->custodian_.c_str(), b2->custodian_.c_str());
    CHECK_EQUALSTR(b1->replyto_.c_str(),   b2->replyto_.c_str());
    CHECK_EQUAL(b1->is_fragment_,          b2->is_fragment_);
    CHECK_EQUAL(b1->is_admin_,             b2->is_admin_);
    CHECK_EQUAL(b1->do_not_fragment_,      b2->do_not_fragment_);
    CHECK_EQUAL(b1->custody_requested_,    b2->custody_requested_);
    CHECK_EQUAL(b1->priority_,             b2->priority_);
    CHECK_EQUAL(b1->receive_rcpt_,         b2->receive_rcpt_);
    CHECK_EQUAL(b1->custody_rcpt_,         b2->custody_rcpt_);
    CHECK_EQUAL(b1->forward_rcpt_,         b2->forward_rcpt_);
    CHECK_EQUAL(b1->delivery_rcpt_,        b2->delivery_rcpt_);
    CHECK_EQUAL(b1->deletion_rcpt_,        b2->deletion_rcpt_);
    CHECK_EQUAL(b1->creation_ts_.tv_sec,   b2->creation_ts_.tv_sec);
    CHECK_EQUAL(b1->creation_ts_.tv_usec,  b2->creation_ts_.tv_usec);
    CHECK_EQUAL(b1->expiration_,           b2->expiration_);
    CHECK_EQUAL(b1->frag_offset_,          b2->frag_offset_);
    CHECK_EQUAL(b1->orig_length_,          b2->orig_length_);
    CHECK_EQUAL(b1->payload_.length(),     b2->payload_.length());

    return true;
}

DECLARE_TEST(Basic)
{
    Bundle* bundle = new Bundle(10, BundlePayload::NODATA);
    bundle->source_.assign("dtn://source.dtn/test");
    bundle->dest_.assign("dtn://dest.dtn/test");
    bundle->custodian_.assign("dtn:none");
    bundle->replyto_.assign("dtn:none");
    
    bundle->expiration_ = 1000;
    bundle->creation_ts_.tv_sec  = 10101010;
    bundle->creation_ts_.tv_usec = 44556677;
    bundle->payload_.set_length(1024);

    CHECK(protocol_test(bundle));

    return UNIT_TEST_PASSED;
}

DECLARE_TEST(Fragment)
{
    Bundle* bundle = new Bundle(10, BundlePayload::NODATA);
    bundle->source_.assign("dtn://frag.dtn/test");
    bundle->dest_.assign("dtn://dest.dtn/test");
    bundle->custodian_.assign("dtn:none");
    bundle->replyto_.assign("dtn:none");
    
    bundle->expiration_ = 30;
    bundle->is_fragment_ = 1;
    bundle->frag_offset_ = 123456789;
    bundle->orig_length_ = 1234567890;
    
    CHECK(protocol_test(bundle));

    return UNIT_TEST_PASSED;
}

DECLARE_TEST(AllFlags)
{
    Bundle* bundle = new Bundle(10, BundlePayload::NODATA);
    bundle->source_.assign("dtn://source.dtn/test");
    bundle->dest_.assign("dtn://dest.dtn/test");
    bundle->custodian_.assign("dtn:none");
    bundle->replyto_.assign("dtn:none");

    bundle->is_admin_ = true;
    bundle->do_not_fragment_ = true;
    bundle->custody_requested_ = true;
    bundle->priority_ = 3;
    bundle->receive_rcpt_ = true;
    bundle->custody_rcpt_ = true;
    bundle->forward_rcpt_ = true;
    bundle->delivery_rcpt_ = true;
    bundle->deletion_rcpt_ = true;
    
    bundle->expiration_ = 1000;
    bundle->creation_ts_.tv_sec  = 10101010;
    bundle->creation_ts_.tv_usec = 44556677;

    CHECK(protocol_test(bundle));

    return UNIT_TEST_PASSED;    
}

DECLARE_TESTER(BundleProtocolTest) {
    ADD_TEST(Basic);
    ADD_TEST(Fragment);
    ADD_TEST(AllFlags);
}

DECLARE_TEST_FILE(BundleProtocolTest, "bundle protocol test");
