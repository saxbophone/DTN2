/*
 *    Copyright 2004-2006 Intel Corporation
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


#include "BundleStatusReport.h"
#include "SDNV.h"
#include <netinet/in.h>
#include <oasys/util/ScratchBuffer.h>

namespace dtn {

void
BundleStatusReport::create_status_report(Bundle*           bundle,
                                         const Bundle*     orig_bundle,
                                         const EndpointID& source,
                                         flag_t            status_flags,
                                         reason_t          reason)
{
    bundle->source_.assign(source);
    if (orig_bundle->replyto_.equals(EndpointID::NULL_EID())){
        bundle->dest_.assign(orig_bundle->source_);
    } else {
        bundle->dest_.assign(orig_bundle->replyto_);
    }
    bundle->replyto_.assign(EndpointID::NULL_EID());
    bundle->custodian_.assign(EndpointID::NULL_EID());
    
    bundle->is_admin_ = true;

    // use the expiration time from the original bundle
    // XXX/demmer maybe something more clever??
    bundle->expiration_ = orig_bundle->expiration_;

    // store the original bundle's source eid
    EndpointID orig_source(orig_bundle->source_);

    int sdnv_encoding_len = 0;  // use an int to handle -1 return values
    int report_length = 0;
    
    // we generally don't expect the Status Peport length to be > 256 bytes
    oasys::ScratchBuffer<u_char*, 256> scratch;

    //
    // Structure of bundle status reports:
    //
    // 1 byte Admin Payload type and flags
    // 1 byte Status Flags
    // 1 byte Reason Code
    // SDNV   [Fragment Offset (if present)]
    // SDNV   [Fragment Length (if present)]
    // 8 byte Time of {receipt/forwarding/delivery/deletion/custody/app-ack}
    //        of bundle X
    // 8 byte Copy of bundle X's Creation Timestamp
    // SDNV   Length of X's source endpoint ID
    // vari   Source endpoint ID of bundle X

    // note that the spec allows for all 6 of the "Time of..." fields
    // to be present in a single Status Report, but for this
    // implementation we will always have one and only one of the 6
    // timestamp fields
    // XXX/matt we may want to change to allow multiple-timestamps per SR

    // the non-optional, fixed-length fields above:
    report_length = 1 + 1 + 1 + 8 + 8;

    // the 2 SDNV fragment fields:
    if (orig_bundle->is_fragment_) {
        report_length += SDNV::encoding_len(orig_bundle->frag_offset_);
        report_length += SDNV::encoding_len(orig_bundle->orig_length_);
    }

    // the Source Endpoint ID:
    report_length += SDNV::encoding_len(orig_source.length()) + orig_source.length();

    //
    // Done calculating length, now create the report payload
    //
    
    u_char* bp = scratch.buf(report_length);
    int len = report_length;
    
    // Admin Payload Type and flags
    *bp = BundleProtocol::ADMIN_STATUS_REPORT << 4;
    if (orig_bundle->is_fragment_) {
        *bp |= BundleProtocol::ADMIN_IS_FRAGMENT;
    }
    bp++;
    len--;
    
    // Status Flags
    *bp++ = status_flags;
    len--;

    // Reason Code
    *bp++ = reason;
    len--;
    
    // The 2 Fragment Fields
    if (orig_bundle->is_fragment_) {
        sdnv_encoding_len = SDNV::encode(orig_bundle->frag_offset_, bp, len);
        ASSERT(sdnv_encoding_len > 0);
        bp  += sdnv_encoding_len;
        len -= sdnv_encoding_len;
        
        sdnv_encoding_len = SDNV::encode(orig_bundle->orig_length_, bp, len);
        ASSERT(sdnv_encoding_len > 0);
        bp  += sdnv_encoding_len;
        len -= sdnv_encoding_len;
    }   

    // Time field, set to the current time (with no sub-second
    // accuracy defined at all)
    BundleTimestamp now;
    now.seconds_ = BundleTimestamp::get_current_time();
    now.seqno_   = 0;
    BundleProtocol::set_timestamp(bp, &now);
    len -= sizeof(u_int64_t);
    bp  += sizeof(u_int64_t);

    // Copy of bundle X's Creation Timestamp
    BundleProtocol::set_timestamp(bp, &orig_bundle->creation_ts_);
    len -= sizeof(u_int64_t);
    bp  += sizeof(u_int64_t);
    
    // The 2 Endpoint ID fields:
    sdnv_encoding_len = SDNV::encode(orig_source.length(), bp, len);
    ASSERT(sdnv_encoding_len > 0);
    len -= sdnv_encoding_len;
    bp  += sdnv_encoding_len;
    
    ASSERT((u_int)len == orig_source.length());
    memcpy(bp, orig_source.c_str(), orig_source.length());
    
    // 
    // Finished generating the payload
    //
    bundle->payload_.set_data(scratch.buf(), report_length);
}

/**
 * Parse a byte stream containing a Status Report Payload and store
 * the fields in the given struct. Returns false if parsing failed.
 */
bool BundleStatusReport::parse_status_report(data_t* data,
                                             const u_char* bp, u_int len)
{
    // 1 byte Admin Payload Type + Flags:
    if (len < 1) { return false; }
    data->admin_type_  = (*bp >> 4);
    data->admin_flags_ = *bp & 0xf;
    bp++;
    len--;

    // validate the admin type
    if (data->admin_type_ != BundleProtocol::ADMIN_STATUS_REPORT) {
        return false;
    }

    // 1 byte Status Flags:
    if (len < 1) { return false; }
    data->status_flags_ = *bp++;
    len--;
    
    // 1 byte Reason Code:
    if (len < 1) { return false; }
    data->reason_code_ = *bp++;
    len--;
    
    // Fragment SDNV Fields (offset & length), if present:
    if (data->admin_flags_ & BundleProtocol::ADMIN_IS_FRAGMENT) {
        int sdnv_bytes = SDNV::decode(bp, len, &data->orig_frag_offset_);
        if (sdnv_bytes == -1) { return false; }
        bp  += sdnv_bytes;
        len -= sdnv_bytes;
        sdnv_bytes = SDNV::decode(bp, len, &data->orig_frag_length_);
        if (sdnv_bytes == -1) { return false; }
        bp  += sdnv_bytes;
        len -= sdnv_bytes;
    }

    // The 6 Optional ACK Timestamps:
    
    if (data->status_flags_ & BundleProtocol::STATUS_RECEIVED) {
        if (len < sizeof(u_int64_t)) { return false; }
        BundleProtocol::get_timestamp(&data->receipt_tv_, bp);
        bp  += sizeof(u_int64_t);
        len -= sizeof(u_int64_t);
    }

    if (data->status_flags_ & BundleProtocol::STATUS_CUSTODY_ACCEPTED) {
        if (len < sizeof(u_int64_t)) { return false; }
        BundleProtocol::get_timestamp(&data->custody_tv_, bp);
        bp  += sizeof(u_int64_t);
        len -= sizeof(u_int64_t);
    }

    if (data->status_flags_ & BundleProtocol::STATUS_FORWARDED) {
        if (len < sizeof(u_int64_t)) { return false; }
        BundleProtocol::get_timestamp(&data->forwarding_tv_, bp);
        bp  += sizeof(u_int64_t);
        len -= sizeof(u_int64_t);
    }

    if (data->status_flags_ & BundleProtocol::STATUS_DELIVERED) {
        if (len < sizeof(u_int64_t)) { return false; }
        BundleProtocol::get_timestamp(&data->delivery_tv_, bp);
        bp  += sizeof(u_int64_t);
        len -= sizeof(u_int64_t);
    }

    if (data->status_flags_ & BundleProtocol::STATUS_DELETED) {
        if (len < sizeof(u_int64_t)) { return false; }
        BundleProtocol::get_timestamp(&data->deletion_tv_, bp);
        bp  += sizeof(u_int64_t);
        len -= sizeof(u_int64_t);
    }

    if (data->status_flags_ & BundleProtocol::STATUS_ACKED_BY_APP) {
        if (len < sizeof(u_int64_t)) { return false; }
        BundleProtocol::get_timestamp(&data->acknowledgement_tv_, bp);
        bp  += sizeof(u_int64_t);
        len -= sizeof(u_int64_t);
    }

    
    // Bundle Creation Timestamp
    if (len < sizeof(u_int64_t)) { return false; }
    BundleProtocol::get_timestamp(&data->orig_creation_tv_, bp);
    bp  += sizeof(u_int64_t);
    len -= sizeof(u_int64_t);

    
    // EID of Bundle
    u_int64_t EID_len;
    int num_bytes = SDNV::decode(bp, len, &EID_len);
    if (num_bytes == -1) { return false; }
    bp  += num_bytes;
    len -= num_bytes;

    if (len != EID_len) { return false; }
    bool ok = data->orig_source_eid_.assign(std::string((const char*)bp, len));
    if (!ok) {
        return false;
    }
    
    return true;
}

//----------------------------------------------------------------------
const char*
BundleStatusReport::reason_to_str(u_int8_t reason)
{
    switch (reason) {
    case BundleProtocol::REASON_NO_ADDTL_INFO:
        return "no additional information";

    case BundleProtocol::REASON_LIFETIME_EXPIRED:
        return "lifetime expired";

    case BundleProtocol::REASON_FORWARDED_UNIDIR_LINK:
        return "forwarded over unidirectional link";

    case BundleProtocol::REASON_TRANSMISSION_CANCELLED:
        return "transmission cancelled";

    case BundleProtocol::REASON_DEPLETED_STORAGE:
        return "depleted storage";

    case BundleProtocol::REASON_ENDPOINT_ID_UNINTELLIGIBLE:
        return "endpoint id unintelligible";

    case BundleProtocol::REASON_NO_ROUTE_TO_DEST:
        return "no known route to destination";

    case BundleProtocol::REASON_NO_TIMELY_CONTACT:
        return "no timely contact";

    case BundleProtocol::REASON_BLOCK_UNINTELLIGIBLE:
        return "block unintelligible";

    default:
        return "(unknown reason)";
    }
}

} // namespace dtn
