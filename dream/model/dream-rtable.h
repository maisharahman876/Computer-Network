/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 Hemanth Narra
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Hemanth Narra <hemanth@ittc.ku.com>
 *
 * James P.G. Sterbenz <jpgs@ittc.ku.edu>, director
 * ResiliNets Research Group  http://wiki.ittc.ku.edu/resilinets
 * Information and Telecommunication Technology Center (ITTC)
 * and Department of Electrical Engineering and Computer Science
 * The University of Kansas Lawrence, KS USA.
 *
 * Work supported in part by NSF FIND (Future Internet Design) Program
 * under grant CNS-0626918 (Postmodern Internet Architecture),
 * NSF grant CNS-1050226 (Multilayer Network Resilience Analysis and Experimentation on GENI),
 * US Department of Defense (DoD), and ITTC at The University of Kansas.
 */

#ifndef DREAM_RTABLE_H
#define DREAM_RTABLE_H

#include <cassert>
#include <map>
#include <sys/types.h>
#include "ns3/ipv4.h"
#include "ns3/ipv4-route.h"
#include "ns3/timer.h"
#include "ns3/net-device.h"
#include "ns3/output-stream-wrapper.h"

namespace ns3 {
namespace dream {
enum RouteFlags
{
  VALID = 0,     // !< VALID
  INVALID = 1,     // !< INVALID
};

/**
 * \ingroup dream
 * \brief Routing table entry
 */
class RoutingTableEntry
{
public:
  uint32_t node_id;
  float r;
  float theta;
  float speed;

  RoutingTableEntry (uint32_t node_id=0, float r=0.0, float theta=0.0,float speed=0.0, Time lifetime = Simulator::Now ());

  ~RoutingTableEntry ();
  
  /**
   * Set lifetime
   * \param lifeTime the lifetime value
   */
  void
  SetLifeTime (Time lifeTime)
  {
    m_lifeTime = lifeTime;
  }
  /**
   * Get lifetime
   * \returns the lifetime value
   */
  Time
  GetLifeTime () const
  {
    return (Simulator::Now () - m_lifeTime);
  }
  
  SetFlag (RouteFlags flag)
  {
    m_flag = flag;
  }
  /**
   * Get route flags
   * \returns the route flags
   */
  RouteFlags
  GetFlag () const
  {
    return m_flag;
  }
 void
  SetEntriesChanged (bool entriesChanged)
  {
    m_entriesChanged = entriesChanged;
  }
  /**
   * Get entries changed
   * \returns the entries changed flag
   */
  bool
  GetEntriesChanged () const
  {
    return m_entriesChanged;
  }
  /**
   * \brief Compare destination address
   * \param destination destination node IP address
   * \return true if equal
  
  bool
  operator== (Ipv4Address const destination) const
  {
    return (address == destination);
  }
   */
  
  /**
   * Print routing table entry
   * \param stream the output stream
   */
  void
  Print (Ptr<OutputStreamWrapper> stream, Time::Unit unit = Time::S) const;

private:
 
  Time m_lifeTime;
  bool m_entriesChanged;
 
  /// Routing flags: valid, invalid or in search
  RouteFlags m_flag;

  

};

/**
 * \ingroup dream
 * \brief The Routing table used by DREAM protocol
 */
class RoutingTable
{
public:
  /// c-tor
  RoutingTable ();
  /**
   * Add routing table entry if it doesn't yet exist in routing table
   * \param r routing table entry
   * \return true in success
   */
  bool
  AddRoute (RoutingTableEntry & r, Ipv4Address address);
  /**
   * Delete routing table entry with destination address dst, if it exists.
   * \param dst destination address
   * \return true on success
   */
  bool
  DeleteRoute (uint32_t nid);
  /**
   * Lookup routing table entry with destination address dst
   * \param dst destination address
   * \param rt entry with destination address dst, if exists
   * \return true on success
   */
  bool
  LookupRoute (uint32_t nid,RoutingTableEntry & rt);
  /**
   * Lookup routing table entry with destination address dst
   * \param id destination address
   * \param rt entry with destination address dst, if exists
   * \param forRouteInput for routing input
   * \return true on success
   */
  
  bool
  Update (RoutingTableEntry & rt);
 
  /**
   * Lookup list of all addresses in the routing table
   * \param allRoutes is the list that will hold all these addresses present in the nodes routing table
   */
  void
  GetListOfAllRoutes (std::map<Ipv4Address, RoutingTableEntry> & allRoutes);
 
  /// Delete all entries from routing table
  void
  Clear ()
  {
    m_ipv4AddressEntry.clear ();
  }
  /**
   * Delete all outdated entries if Lifetime is expired
   * \param removedAddresses is the list of addresses to purge
   */
  void
  Purge (std::map<Ipv4Address, RoutingTableEntry> & removedAddresses);
  /**
   * Print routing table
   * \param stream the output stream
   */
  //void Print (Ptr<OutputStreamWrapper> stream, Time::Unit unit = Time::S) const;
  /**
   * Provides the number of routes present in that nodes routing table.
   * \returns the number of routes
   */
  uint32_t
  RoutingTableSize ();
  /**
  * Add an event for a destination address so that the update to for that destination is sent
  * after the event is completed.
  * \param address destination address for which this event is running.
  * \param id unique eventid that was generated.
  * \return true on success
  */
 
  Time Getholddowntime () const
  {
    return m_holddownTime;
  }
  /**
   * Set hold down time (time until an invalid route may be deleted)
   * \param t the hold down time
   */
  void Setholddowntime (Time t)
  {
    m_holddownTime = t;
  }

private:
  // Fields
  /// an entry in the routing table.
  
  std::map<Ipv4Address, RoutingTableEntry> m_ipv4AddressEntry;
  
  Time m_holddownTime;

};
}
}
#endif /* DREAM_RTABLE_H */
