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
#ifndef _LINK_STATE_GRAPH_H_
#define _LINK_STATE_GRAPH_H_

#include <set>
#include <oasys/debug/Logger.h>

#include "BundleRouter.h"
#include "LinkScheduleEstimator.h"


#define MAX_EID 255

namespace dtn {

/**
 *  Encapsulates link state graphs and the operations upon them.
 *  
 *  A link state graph consists of Vertices and Edges. A Vertex represents
 *  a single endpoint identifier (EID). Every pair of Vertices may have at
 *  most one Edge between them, and are associated with an edge cost. Edges 
 *  represent a connection between two EIDs, usually a physical link, 
 *  but this is by no means guaranteed.
 *
 *  The use of the names Edge and Vertex is intentional to avoid any 
 *  confusion between physical nodes, and the Vertices in the graph, as 
 *  well as physical links and edges in the graph (which may not represent
 *  actual links, but rather "virtual" links within the machine).
 *
 *  Example: A network of two machines, uid://1, uid://2. Machines are 
 *           connected via Ethernet interfaces, eth://00:01, eth://00:02.
 *
 *  Vertices = { uid://1, uid://2, eth://00:01, eth://00:02 };
 *  Edges    = { ( uid://1, eth://00:01, 0 ),
 *               ( uid://2, eth://00:02, 0 ),
 *               ( eth://00:01, eth://00:02, 1 ) }
 *
 *
 *  We use the abstraction of multiple Vertices per Node to make for cleaner
 *  routing algorithms. 
 *
 *  Note: later on, edges will also have connectivity characteristics, such
 *        as priority criteria, a "calendar", and potentially others.
 *
 *  XXX/jakob - is there a case for allowing multiple edges between vertices?
 *              is it strong enough to allow the ugliness it causes?
 */

class LinkStateGraph : public oasys::Logger {
public:
    LinkStateGraph();

    class Vertex;
    class Edge;

    typedef int cost_t;
    typedef std::set<Vertex*> VertexSet;
    typedef std::set<Edge*>  EdgeSet;

    /**
     *  Every eid is represented by its own vertice. This means a
     *  single machine could potentially be represented by a large
     *  number of vertices in the graph.
     */
    VertexSet vertices_;    

    /**
     *  There is at most one edge between any pair of vertices.
     *
     *  Note: machines may have more than one edge between them, but
     *  that's because they have several interfaces.
     */
    EdgeSet edges_;

    /**
     *  Adds an edge to the link state graph. 
     */
    Edge* addEdge(Vertex* from, Vertex* to, cost_t cost);

    /*
     *  Returns the edge between from and to, if such an edge exists.
     */
    Edge* getEdge(Vertex* from, Vertex* to);

    /**
     * Removes an edge from the link state graph.
     */
    void removeEdge(Edge *to);
    
    /**
     *  Finds the Vertex with the given eid, or creates a new one if
     *  none is found.  New Vertices are automatically added to the
     *  lsg.
     */
    Vertex* getVertex(const char* eid);

    /**
     *  Run the dijkstra algorithm, and then return the best next
     *  hop. This is very naive, but at least less bug prone.
     */
    Vertex* getMatchingVertex(const char* eid);

    /**
     *   Run the dijkstra algorithm, and then return the best next
     *   hop. This is very naive, but at least less bug prone.
     */
    Vertex* findNextHop(Vertex* from, Vertex *to);

    //! Dump a debug version of the graph to buf
    void dumpGraph(oasys::StringBuffer* buf);

    EdgeSet edges();

    /* A time-varying connection between two vertices in the graph */
    class Edge {
     public:
        Edge(Vertex *from, Vertex *to, cost_t cost);

        Vertex* from_;
        Vertex* to_;

        cost_t cost_;

        // log of all the up/down events this edge has seen
        LinkScheduleEstimator::Log      log_;
        LinkScheduleEstimator::LogEntry contact_;
    };


    /* 
       A vertex in the graph is an EID 
       XXX/jakob - is it?
     */
    class Vertex {
      public:
        Vertex(const char * eid);

        char eid_[MAX_EID];        
        int dijkstra_distance_;

        std::map<Vertex*,Edge*> incoming_edges_;
        std::map<Vertex*,Edge*> outgoing_edges_;

        int operator<(Vertex* a) {
            ASSERT(a);
            return strcmp(eid_,a->eid_);
        }
    };    
};

} // namespace dtn

#endif /* _LINK_STATE_GRAPH_H_ */
