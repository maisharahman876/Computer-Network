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
#include<string>

namespace ns3 {
namespace dream {
/**
 * \ingroup dream
 * \brief Dream Update Packet Format
 * \verbatim
 |      0        |      1        |      2        |       3       |
  0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                 Destination  Address                          |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                            Type                               |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |                       Sequence no                             |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * \endverbatim
 */

class DreamHeader : public Header
{
public:
  /**
   * Constructor
   *
   * \param dst destination IP address
   * \param msgtype
   * \param dstSeqNo destination sequence number
   */
  DreamHeader (Ipv4Address dst = Ipv4Address (), std:: string type="",uint32_t dstSeqNo = 0);
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
  
  void
  SetMsgType (std::string type)
  {
    m_type = type;
  }
  
  std::string
  GetMsgType () const
  {
    return m_type;
  }
  
  
private:
  Ipv4Address m_dst; ///< Destination IP Address
  uint32_t m_type; ///< msg type
  uint32_t m_dstSeqNo; ///< msg id
};
static inline std::ostream & operator<< (std::ostream& os, const DreamHeader & packet)
{
  packet.Print (os);
  return os;
}
}
}

#endif /* DREAM_PACKET_H */
