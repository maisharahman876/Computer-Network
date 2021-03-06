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
#include "dream-packet.h"
#include "ns3/address-utils.h"
#include "ns3/packet.h"


namespace ns3 {
namespace dream {

NS_OBJECT_ENSURE_REGISTERED (DreamHeader);

DreamHeader::DreamHeader (Ipv4Address dst, uint32_t hopCount, uint32_t dstSeqNo,uint32_t x, uint32_t y,float v)
  : m_dst (dst),
    m_hopCount (hopCount),
    m_dstSeqNo (dstSeqNo),
    m_x(x),
    m_y(y),
    m_speed(v)
{
}

DreamHeader::~DreamHeader ()
{
}

TypeId
DreamHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::dream::DreamHeader")
    .SetParent<Header> ()
    .SetGroupName ("Dream")
    .AddConstructor<DreamHeader> ();
  return tid;
}

TypeId
DreamHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

uint32_t
DreamHeader::GetSerializedSize () const
{
  return 12;
}

void
DreamHeader::Serialize (Buffer::Iterator i) const
{
  WriteTo (i, m_dst);
  WriteTo (i, m_src);
  i.WriteHtonU32 (m_hopCount);
  i.WriteHtonU32 (m_dstSeqNo);
  i.WriteHtonU32 (m_x);
  i.WriteHtonU32 (m_y);

}

uint32_t
DreamHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  ReadFrom (i, m_dst);
  ReadFrom (i, m_src);
  m_hopCount = i.ReadNtohU32 ();
  m_dstSeqNo = i.ReadNtohU32 ();
  m_x=i.ReadNtohU32 ();
  m_y=i.ReadNtohU32 ();

  uint32_t dist = i.GetDistanceFrom (start);
  NS_ASSERT (dist == GetSerializedSize ());
  return dist;
}

void
DreamHeader::Print (std::ostream &os) const
{
  os << "DestinationIpv4: " << m_dst
    << "SourceIpv4: " << m_src
     << " Hopcount: " << m_hopCount
     << " SequenceNumber: " << m_dstSeqNo
     << " PositionX: " << m_x
     << " PositionY: " << m_y;
}
}
}
