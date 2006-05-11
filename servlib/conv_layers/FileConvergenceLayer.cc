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

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>

#include <oasys/io/IO.h>
#include <oasys/util/StringBuffer.h>
#include <oasys/util/URL.h>

#include "FileConvergenceLayer.h"
#include "bundling/Bundle.h"
#include "bundling/BundleEvent.h"
#include "bundling/BundleList.h"
#include "bundling/BundleProtocol.h"
#include "bundling/BundleDaemon.h"

namespace dtn {

/******************************************************************************
 *
 * FileConvergenceLayer
 *
 *****************************************************************************/
FileConvergenceLayer::FileConvergenceLayer()
    : ConvergenceLayer("FileConvergenceLayer", "file")
{
}

/**
 * Pull a filesystem directory out of the next hop ssp.
 */
bool
FileConvergenceLayer::extract_dir(const char* nexthop, std::string* dirp)
{

    PANIC("XXX/demmer fix this implementation");
    
    oasys::URL url(nexthop);
    
    if (! url.valid()) {
        log_err("extract_dir: next hop ssp '%s' not a valid url", nexthop);
        return false;
    }

    // the ssp part of the URL should be of the form:
    // /path1/path2

    // validate that the "host" part of the url is empty, i.e. that
    // the filesystem path is absolute
    if (url.host_.length() != 0) {
        log_err("interface eid '%s' specifies a non-absolute path",
                nexthop);
        return false;
    }

    // and make sure there wasn't a port that was parsed out
    if (url.port_ != 0) {
        log_err("interface eid '%s' specifies a port", nexthop);
        return false;
    }

    dirp->assign("/");
    dirp->append(url.path_);
    return true;
}

/**
 * Validate that a given directory exists and that the permissions
 * are correct.
 */
bool
FileConvergenceLayer::validate_dir(const std::string& dir)
{
    struct stat st;
    if (stat(dir.c_str(), &st) != 0) {
        log_err("error running stat on %s: %s", dir.c_str(), strerror(errno));
        return false;
    }

    if (!S_ISDIR(st.st_mode)) {
        log_err("error: %s not a directory", dir.c_str());
        return false;
    }

    // XXX/demmer check permissions

    return true;
}

/**
 * Bring up a new interface.
 */
bool
FileConvergenceLayer::interface_up(Interface* iface,
                                   int argc, const char* argv[])
{
    (void)iface;
    (void)argc;
    (void)argv;
    
    NOTIMPLEMENTED;
    
//     // parse out the directory from the interface
//     std::string dir;
//     if (!extract_dir(iface->eid().c_str(), &dir)) {
//         return false;
//     }
    
//     // make sure the directory exists and is readable / executable
//     if (!validate_dir(dir)) {
//         return false;
//     }
    
//     // XXX/demmer parse argv for frequency
//     int secs_per_scan = 5;

//     // create a new thread to scan for new bundle files
//     Scanner* scanner = new Scanner(secs_per_scan, dir);
//     scanner->start();

//     // store the new scanner in the cl specific part of the interface
//     iface->set_cl_info(scanner);

    
    return true;
}

/**
 * Bring down the interface.
 */
bool
FileConvergenceLayer::interface_down(Interface* iface)
{
    CLInfo *cli = iface->cl_info();
    Scanner *scanner = (Scanner *)cli;
    scanner->stop();

    // We cannot "delete scanner;" because it is still running
    // right now. oasys::Thread::thread_run deletes the Scanner object
    // when Scanner::run() returns 

    return true;
}
 
/**
 * Validate that the contact eid specifies a legit directory.
 */
bool
FileConvergenceLayer::open_contact(Link* link)
{
    (void)link;
    // XXX/demmer fixme
    
    // parse out the directory from the contact
//     std::string dir;
//     if (!extract_dir(contact->nexthop(), &dir)) {
//         return false;
//     }
    
//     // make sure the directory exists and is readable / executable
//     if (!validate_dir(dir)) {
//         return false;
//     }

    return true;
}

/**
 * Close the connnection to the contact.
 */
bool
FileConvergenceLayer::close_contact(const ContactRef& contact)
{
    (void)contact;
    // nothing to do
    return true;
}
    
/**
 * Try to send the bundles queued up for the given contact.
 */
void
FileConvergenceLayer::send_bundle(const ContactRef& contact, Bundle* bundle)
{
    (void)contact;
    (void)bundle;
    
    // XXX/demmer fix this at some point
    NOTIMPLEMENTED;

#ifdef notimplemented
    std::string dir;
    if (!extract_dir(contact->nexthop(), &dir)) {
        PANIC("contact should have already been validated");
    }

    FileHeader filehdr;
    int iovcnt = BundleProtocol::MAX_IOVCNT + 2;
    struct iovec iov[iovcnt];

    filehdr.version = CURRENT_VERSION;
    
    oasys::StringBuffer fname("%s/bundle-XXXXXX", dir.c_str());
    
    iov[0].iov_base = (char*)&filehdr;
    iov[0].iov_len  = sizeof(FileHeader);

    // fill in the bundle header portion
    u_int16_t header_len =
        BundleProtocol::format_headers(bundle, &iov[1], &iovcnt);

    // fill in the file header
    size_t payload_len = bundle->payload_.length();
    filehdr.header_length = htons(header_len);
    filehdr.bundle_length = htonl(header_len + payload_len);

    // and tack on the payload (adding one to iovcnt for the
    // FileHeader, then one for the payload)
    iovcnt++;
    PANIC("XXX/demmer fix me");
    //iov[iovcnt].iov_base = (void*)bundle->payload_.data();
    iov[iovcnt].iov_len  = payload_len;
    iovcnt++;

    // open the bundle file 
    int fd = mkstemp(fname.c_str());
    if (fd == -1) {
        log_err("error opening temp file in %s: %s",
                fname.c_str(), strerror(errno));
        // XXX/demmer report error here?
        return;
    }

    log_debug("opened temp file %s for bundle id %d "
              "fd %d header_length %zu payload_length %zu",
              fname.c_str(), bundle->bundleid_, fd,
              header_len, payload_len);

    // now write everything out
    int total = sizeof(FileHeader) + header_len + payload_len;
    int cc = oasys::IO::writevall(fd, iov, iovcnt, logpath_);
    if (cc != total) {
        log_err("error writing out bundle (wrote %d/%d): %s",
                cc, total, strerror(errno));
    }

    // free up the iovec data
    BundleProtocol::free_header_iovmem(bundle, &iov[1], iovcnt - 2);
        
    // close the file descriptor
    close(fd);

    // cons up a transmission event and pass it to the router
    bool acked = false;
    BundleDaemon::post(
        new BundleTransmittedEvent(bundle, contact, payload_len, acked));
        
    log_debug("bundle id %d successfully transmitted", bundle->bundleid_);
#endif // notimplemented
}

/******************************************************************************
 *
 * FileConvergenceLayer::Scanner
 *
 *****************************************************************************/
FileConvergenceLayer::Scanner::Scanner(int secs_per_scan, 
                                       const std::string& dir)
    : Logger("FileConvergenceLayer::Scanner",
             "/dtn/cl/file/scanner"), 
      Thread("FileConvergenceLayer::Scanner"), 
      secs_per_scan_(secs_per_scan), 
      dir_(dir), 
      run_(true)
{
    set_flag(DELETE_ON_EXIT);
}

/**
 * Main thread function.
 */
void
FileConvergenceLayer::Scanner::run()
{
    FileHeader filehdr;
    DIR* dir = opendir(dir_.c_str());
    struct dirent* dirent;
    const char* fname;
    u_char* buf;
    int fd;

    if (!dir) {
        // XXX/demmer signal cl somehow?
        log_err("error in opendir");
        return;
    }
    
    while (run_) {
        seekdir(dir, 0);

        while ((dirent = readdir(dir)) != 0) {
            fname = dirent->d_name;

            if ((fname[0] == '.') &&
                ((fname[1] == '\0') ||
                 (fname[1] == '.' && fname[2] == '\0')))
            {
                continue;
            }
            
            log_debug("scan found file %s", fname);

            // cons up the full path
            oasys::StringBuffer path("%s/%s", dir_.c_str(), fname);

            // malloc a buffer for it, open a file descriptor, and
            // read in the header
            if ((fd = open(path.c_str(), 0)) == -1) {
                log_err("error opening file %s: %s", path.c_str(), strerror(errno));
                continue;
            }

            int cc = oasys::IO::readall(fd, (char*)&filehdr, sizeof(FileHeader));
            if (cc != sizeof(FileHeader)) {
                log_warn("can't read in FileHeader (read %d/%zu): %s",
                         cc, sizeof(FileHeader), strerror(errno));
                continue;
            }

            if (filehdr.version != CURRENT_VERSION) {
                log_warn("framing protocol version mismatch: %d != current %d",
                         filehdr.version, CURRENT_VERSION);
                continue;
            }

            u_int16_t header_len = ntohs(filehdr.header_length);
            size_t bundle_len = ntohl(filehdr.bundle_length);
            
            log_debug("found bundle file %s: header_length %u bundle_length %zu",
                      path.c_str(), header_len, bundle_len);

            // read in and parse the headers
            buf = (u_char*)malloc(header_len);
            cc = oasys::IO::readall(fd, (char*)buf, header_len);
            if (cc != header_len) {
                log_err("error reading file %s header (read %d/%d): %s",
                        path.c_str(), cc, header_len, strerror(errno));
                free(buf);
                continue;
            }

            Bundle* bundle = new Bundle();
            if (! BundleProtocol::parse_headers(bundle, buf, header_len)) {
                log_err("error parsing bundle headers in file %s", path.c_str());
                free(buf);
                delete bundle;
                continue;
            }
            free(buf);

            // Now validate the lengths
            size_t payload_len = bundle->payload_.length();
            if (bundle_len != header_len + payload_len) {
                log_err("error in bundle lengths in file %s: "
                        "bundle_length %zu, header_length %u, payload_length %zu",
                        path.c_str(), bundle_len, header_len, payload_len);
                delete bundle;
                continue;
            }

            // Looks good, now read in and assign the data
            buf = (u_char*)malloc(payload_len);
            cc = oasys::IO::readall(fd, (char*)buf, payload_len);
            if (cc != (int)payload_len) {
                log_err("error reading file %s payload (read %d/%zu): %s",
                        path.c_str(), cc, payload_len, strerror(errno));
                delete bundle;
                continue;
            }
            bundle->payload_.set_data(buf, payload_len);
            free(buf);
            
            // close the file descriptor and remove the file
            if (close(fd) != 0) {
                log_err("error closing file %s: %s",
                        path.c_str(), strerror(errno));
            }
            
            if (unlink(path.c_str()) != 0) {
                log_err("error removing file %s: %s",
                        path.c_str(), strerror(errno));
            }

            // all set, notify the router
            BundleDaemon::post(
                new BundleReceivedEvent(bundle, EVENTSRC_PEER));
        }
            
        sleep(secs_per_scan_);
    }
    log_info("exiting");
}

/**
 * Set the flag to ask it to stop next loop.
 */
void FileConvergenceLayer::Scanner::stop() {
    run_ = false;
}

} // namespace dtn
