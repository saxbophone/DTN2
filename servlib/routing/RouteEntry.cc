/*
 * IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING. By
 * downloading, copying, installing or using the software you agree to
 * this license. If you do not agree to this license, do not download,
 * install, copy or use the software.
 * 
 * Intel Open Source License 
 * 
 * Copyright (c) 2006 Intel Corporation. All rights reserved. 
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

#include <oasys/util/OptParser.h>
#include <oasys/util/StringBuffer.h>
#include "RouteEntry.h"
#include "contacts/Link.h"

namespace dtn {

//----------------------------------------------------------------------
RouteEntry::RouteEntry(const EndpointIDPattern& pattern, Link* link)
    : pattern_(pattern),
      priority_(0),
      next_hop_(link),
      action_(FORWARD_COPY),
      custody_timeout_(),
      info_(NULL)
{
}

//----------------------------------------------------------------------
RouteEntry::~RouteEntry()
{
    if (info_)
        delete info_;
}

//----------------------------------------------------------------------
int
RouteEntry::parse_options(int argc, const char** argv)
{
    int num = custody_timeout_.parse_options(argc, argv);
    argc -= num;
    
    oasys::OptParser p;

    p.addopt(new oasys::UIntOpt("priority", &priority_));

    oasys::EnumOpt::Case fwdopts[] = {
        {"forward_unique", FORWARD_UNIQUE},
        {"forward_copy",   FORWARD_COPY},
    };
    p.addopt(new oasys::EnumOpt("action", 2, fwdopts, (int*)&action_));

    num += p.parse_and_shift(argc, argv);
    return num;
}

//----------------------------------------------------------------------
void
RouteEntry::dump(oasys::StringBuffer* buf) const
{
    buf->appendf("%s -> %s (%s) priority %d "
                 "[custody timeout: base %u lifetime_pct %u limit %u]\n",
                 pattern_.c_str(),
                 next_hop_->name(),
                 bundle_fwd_action_toa(action_),
                 priority_,
                 custody_timeout_.base_,
                 custody_timeout_.lifetime_pct_,
                 custody_timeout_.limit_);
}

} // namespace dtn
