/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef DREAM_HELPER_H
#define DREAM_HELPER_H

#include "ns3/dream.h"

namespace ns3 {

/* ... */
class DreamHelper : public Ipv4RoutingHelper
{
public:
  DreamHelper ();
  ~DreamHelper ();
  /**
   * \returns pointer to clone of this DreamHelper
   *
   * This method is mainly for internal use by the other helpers;
   * clients are expected to free the dynamic memory allocated by this method
   */
  DreamHelper* Copy (void) const;

  /**
   * \param node the node on which the routing protocol will run
   * \returns a newly-created routing protocol
   *
   * This method will be called by ns3::InternetStackHelper::Install
   *
   */
  virtual Ptr<Ipv4RoutingProtocol> Create (Ptr<Node> node) const;
  /**
   * \param name the name of the attribute to set
   * \param value the value of the attribute to set.
   *
   * This method controls the attributes of ns3::dream::RoutingProtocol
   */
  void Set (std::string name, const AttributeValue &value);


private:
  ObjectFactory m_agentFactory; //!< Object factory
  
};

}

}

#endif /* DREAM_HELPER_H */

