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

DreamHeader::DreamHeader (Ipv4Address dst,std::string typ, uint32_t dstSeqNo)
  : m_dst (dst),
    m_type(typ),
    m_dstSeqNo(dstSeqNo)
{
}

Dreameader::~DreamHeader ()
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
  
  i.WriteHtonU32 (m_type);
  i.WriteHtonU32 (m_dstSeqNo);

}

uint32_t
DreamHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  ReadFrom (i, m_dst);
  m_type = i.ReadNtohU32 ();
  m_dstSeqNo = i.ReadNtohU32 ();

  uint32_t dist = i.GetDistanceFrom (start);
  NS_ASSERT (dist == GetSerializedSize ());
  return dist;
}

void
DreamHeader::Print (std::ostream &os) const
{
  os << "DestinationIpv4: " << m_dst
     << " MessageType: " << m_type
     << " Sequence no: " << m_dstSeqNo;
}
}
}
