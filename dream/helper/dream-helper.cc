/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "dream-helper.h"
#include "ns3/dream-routing-protocol.h"
#include "ns3/node-list.h"
#include "ns3/names.h"
#include "ns3/ipv4-list-routing.h"

namespace ns3 {

DreamHelper::~DreamHelper ()
{
}

DreamHelper::DreamHelper () : Ipv4RoutingHelper ()
{
  m_agentFactory.SetTypeId ("ns3::dream::RoutingProtocol");
}

DreamHelper*
DreamHelper::Copy (void) const
{
  return new DreamHelper (*this);
}


Ptr<Ipv4RoutingProtocol>
DreamHelper::Create (Ptr<Node> node) const
{
  Ptr<dream::RoutingProtocol> agent = m_agentFactory.Create<dream::RoutingProtocol> ();
  Ptr<MobilityModel> mob = node->GetObject<MobilityModel>();
  node->AggregateObject (agent);
  agent->AggregateObject(mob);
  return agent;
}

void
DreamHelper::Set (std::string name, const AttributeValue &value)
{
  m_agentFactory.Set (name, value);
}

}


}

