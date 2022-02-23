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

#ifndef DREAM_PACKET_H
#define DREAM_PACKET_H

#include <iostream>
#include "ns3/header.h"
#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"

namespace ns3 {
namespace dream {


class DreamHeader : public Header
{
public:
  /**
   * Constructor
   *
   * \param dst destination IP address
   * \param hopcount hop count
   * \param dstSeqNo destination sequence number
   * \param src Source IP Address
   * \param x   Position of node in x axis
   * \param y   Position of node in y axis
   * \param speed   Speed of source
   */
  DreamHeader (Ipv4Address dst = Ipv4Address (), uint32_t hopcount = 0, uint32_t dstSeqNo = 0,uint32_t x=0,uint32_t y=0,float speed=0.0);
  virtual ~DreamHeader ();
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize () const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;

  /**
   * Set destination address
   * \param destination the destination IPv4 address
   */
  void
  SetDst (Ipv4Address destination)
  {
    m_dst = destination;
  }
  /**
   * Get destination address
   * \returns the destination IPv4 address
   */
  Ipv4Address
  GetDst () const
  {
    return m_dst;
  }
  void
  SetSrc (Ipv4Address destination)
  {
    m_src = destination;
  }
  
  Ipv4Address
  GetSrc () const
  {
    return m_src;
  }
  /**
   * Set hop count
   * \param hopCount the hop count
   */
  void
  SetHopCount (uint32_t hopCount)
  {
    m_hopCount = hopCount;
  }
  /**
   * Get hop count
   * \returns the hop count
   */
  uint32_t
  GetHopCount () const
  {
    return m_hopCount;
  }
  /**
   * Set destination sequence number
   * \param sequenceNumber The sequence number
   */
  void
  SetDstSeqno (uint32_t sequenceNumber)
  {
    m_dstSeqNo = sequenceNumber;
  }
  /**
   * Get destination sequence number
   * \returns the destination sequence number
   */
  uint32_t
  GetDstSeqno () const
  {
    return m_dstSeqNo;
  }
  uint32_t
  GetX()
  {
    return m_x;
  }
  void 
  SetX(uint32_t nx)
  {
    m_x=nx;
  }
  uint32_t
  GetY()
  {
    return m_y;
  }
  void 
  SetY(uint32_t ny)
  {
    m_y=ny;
  }
  float
  GetSpeed()
  {
    return m_speed;
  }
  void 
  SetSpeed(float v)
  {
    m_speed=v;
  }
private:
  Ipv4Address m_src; ///< Source IP Address
  Ipv4Address m_dst; ///< Destination IP Address
  uint32_t m_hopCount; ///< Number of Hops
  uint32_t m_dstSeqNo; ///< Destination Sequence Number
  uint32_t m_x;
  uint32_t m_y;
  float m_speed;
};
static inline std::ostream & operator<< (std::ostream& os, const DreamHeader & packet)
{
  packet.Print (os);
  return os;
}
}
}

#endif /* DREAM_PACKET_H */
