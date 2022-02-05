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

#include <iostream>
#include <cmath>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/internet-module.h"
#include "ns3/dsdv-helper.h"
#include "ns3/yans-wifi-helper.h"

#define con1 8
using namespace ns3;

uint16_t port = 9;

NS_LOG_COMPONENT_DEFINE ("DsdvManetExample");

/**
 * \ingroup dsdv
 * \ingroup dsdv-examples
 * \ingroup examples
 *
 * \brief DSDV Manet example
 */
class DsdvManetExample
{
public:
  DsdvManetExample ();
  /**
   * Run function
   * \param nWifis The total number of nodes
   * \param nSinks The total number of receivers
   * \param totalTime The total simulation time
   * \param rate The network speed
   * \param phyMode The physical mode
   * \param nodeSpeed The node speed
   * \param periodicUpdateInterval The routing update interval
   * \param settlingTime The routing update settling time
   * \param dataStart The data transmission start time
   * \param printRoutes print the routes if true
   * \param CSVfileName The CSV file name
   */
  void CaseRun (uint32_t nWifis,
                uint32_t nCsma,
                uint32_t nSinks,
                double totalTime,
                std::string rate,
                std::string phyMode,
                uint32_t nodeSpeed,
                uint32_t periodicUpdateInterval,
                uint32_t settlingTime,
                double dataStart,
                bool printRoutes,
                std::string CSVfileName);

private:
  uint32_t m_nWifis; ///< total number of nodes
  uint32_t m_nCsma; ///< total number of csma nodes
  uint32_t m_nSinks; ///< number of receiver nodes
  double m_totalTime; ///< total simulation time (in seconds)
  std::string m_rate; ///< network bandwidth
  std::string m_phyMode; ///< remote station manager data mode
  uint32_t m_nodeSpeed; ///< mobility speed
  uint32_t m_periodicUpdateInterval; ///< routing update interval
  uint32_t m_settlingTime; ///< routing setting time
  double m_dataStart; ///< time to start data transmissions (seconds)
  uint32_t bytesTotal; ///< total bytes received by all nodes
  uint32_t packetsReceived; ///< total packets received by all nodes
  bool m_printRoutes; ///< print routing table
  std::string m_CSVfileName; ///< CSV file name

  NodeContainer nodes1,nodes2,nodes3,AP1,AP2; ///< the collection of nodes
  NetDeviceContainer devices1,devices2,devices3,APdevices1,APdevices2; ///< the collection of devices
  Ipv4InterfaceContainer interfaces1,interfaces2,interfaces3,APinterfaces1,APinterfaces2; ///< the collection of interfaces

private:
  /// Create and initialize all nodes
  void CreateNodes ();
  /**
   * Create and initialize all devices
   * \param tr_name The trace file name
   */
  void CreateDevices (std::string tr_name);
  /**
   * Create network
   * \param tr_name The trace file name
   */
  void InstallInternetStack (std::string tr_name);
  /// Create data sinks and sources
  void InstallApplications ();
  /// Setup mobility model
  void SetupMobility ();
  /**
   * Packet receive function
   * \param socket The communication socket
   */
  void ReceivePacket (Ptr <Socket> socket);
  /**
   * Setup packet receivers
   * \param addr the receiving IPv4 address
   * \param node the receiving node
   * \returns the communication socket
   */
  Ptr <Socket> SetupPacketReceive (Ipv4Address addr, Ptr <Node> node );
  /// Check network throughput
  void CheckThroughput ();

};

int main (int argc, char **argv)
{
  DsdvManetExample test;
  uint32_t nWifis = 8;
  uint32_t nCsma = 6;
  uint32_t nSinks = 4;
  double totalTime = 100.0;
  std::string rate ("8kbps");
  std::string phyMode ("DsssRate11Mbps");
  uint32_t nodeSpeed = 10; // in m/s
  std::string appl = "all";
  uint32_t periodicUpdateInterval = 15;
  uint32_t settlingTime = 6;
  double dataStart = 50.0;
  bool printRoutingTable = true;
  std::string CSVfileName = "DsdvManetExample.csv";

  CommandLine cmd (__FILE__);
  cmd.AddValue ("nWifis", "Number of wifi nodes[Default:8]", nWifis);
  cmd.AddValue ("nCsmas", "Number of wifi nodes[Default:6]", nCsma);
  cmd.AddValue ("nSinks", "Number of wifi sink nodes[Default:4]", nSinks);
  cmd.AddValue ("totalTime", "Total Simulation time[Default:100]", totalTime);
  cmd.AddValue ("phyMode", "Wifi Phy mode[Default:DsssRate11Mbps]", phyMode);
  cmd.AddValue ("rate", "CBR traffic rate[Default:8kbps]", rate);
  cmd.AddValue ("nodeSpeed", "Node speed in RandomWayPoint model[Default:10]", nodeSpeed);
  cmd.AddValue ("periodicUpdateInterval", "Periodic Interval Time[Default=15]", periodicUpdateInterval);
  cmd.AddValue ("settlingTime", "Settling Time before sending out an update for changed metric[Default=6]", settlingTime);
  cmd.AddValue ("dataStart", "Time at which nodes start to transmit data[Default=50.0]", dataStart);
  cmd.AddValue ("printRoutingTable", "print routing table for nodes[Default:1]", printRoutingTable);
  cmd.AddValue ("CSVfileName", "The name of the CSV output file name[Default:DsdvManetExample.csv]", CSVfileName);
  cmd.Parse (argc, argv);

  std::ofstream out (CSVfileName.c_str ());
  out << "SimulationSecond," <<
  "ReceiveRate," <<
  "PacketsReceived," <<
  "NumberOfSinks," <<
  std::endl;
  out.close ();

  SeedManager::SetSeed (12345);

  Config::SetDefault ("ns3::OnOffApplication::PacketSize", StringValue ("1000"));
  Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue (rate));
  Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue (phyMode));
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2000"));

  test = DsdvManetExample ();
  test.CaseRun (nWifis,nCsma, nSinks, totalTime, rate, phyMode, nodeSpeed, periodicUpdateInterval,
                settlingTime, dataStart, printRoutingTable, CSVfileName);

  return 0;
}

DsdvManetExample::DsdvManetExample ()
  : bytesTotal (0),
    packetsReceived (0)
{
}

void
DsdvManetExample::ReceivePacket (Ptr <Socket> socket)
{
  NS_LOG_UNCOND (Simulator::Now ().As (Time::S) << " Received one packet!");
  Ptr <Packet> packet;
  while ((packet = socket->Recv ()))
    {
      bytesTotal += packet->GetSize ();
      packetsReceived += 1;
    }
}

void
DsdvManetExample::CheckThroughput ()
{
  double kbs = (bytesTotal * 8.0) / 1000;
  bytesTotal = 0;

  std::ofstream out (m_CSVfileName.c_str (), std::ios::app);

  out << (Simulator::Now ()).GetSeconds () << "," << kbs << "," << packetsReceived << "," << m_nSinks << std::endl;

  out.close ();
  packetsReceived = 0;
  Simulator::Schedule (Seconds (1.0), &DsdvManetExample::CheckThroughput, this);
}

Ptr <Socket>
DsdvManetExample::SetupPacketReceive (Ipv4Address addr, Ptr <Node> node)
{

  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr <Socket> sink = Socket::CreateSocket (node, tid);
  InetSocketAddress local = InetSocketAddress (addr, port);
  sink->Bind (local);
  sink->SetRecvCallback (MakeCallback ( &DsdvManetExample::ReceivePacket, this));

  return sink;
}

void
DsdvManetExample::CaseRun (uint32_t nWifis,uint32_t nCsma, uint32_t nSinks, double totalTime, std::string rate,
                           std::string phyMode, uint32_t nodeSpeed, uint32_t periodicUpdateInterval, uint32_t settlingTime,
                           double dataStart, bool printRoutes, std::string CSVfileName)
{
  m_nWifis = nWifis;
  m_nSinks = nSinks;
  m_totalTime = totalTime;
  m_rate = rate;
  m_phyMode = phyMode;
  m_nodeSpeed = nodeSpeed;
  m_periodicUpdateInterval = periodicUpdateInterval;
  m_settlingTime = settlingTime;
  m_dataStart = dataStart;
  m_printRoutes = printRoutes;
  m_CSVfileName = CSVfileName;

  std::stringstream ss;
  ss << m_nWifis;
  std::string t_nodes = ss.str ();

  std::stringstream ss3;
  ss3 << m_totalTime;
  std::string sTotalTime = ss3.str ();

  std::string tr_name = "Dsdv_Manet_" + t_nodes + "Nodes_" + sTotalTime + "SimTime";
  std::cout << "Trace file generated is " << tr_name << ".tr\n";

  CreateNodes ();
  
  CreateDevices (tr_name);
  
  SetupMobility ();
  
  InstallInternetStack (tr_name);
  
  InstallApplications ();

  std::cout << "\nStarting simulation for " << m_totalTime << " s ...\n";

  CheckThroughput ();

  Simulator::Stop (Seconds (m_totalTime));
  Simulator::Run ();
  Simulator::Destroy ();
}

void
DsdvManetExample::CreateNodes ()
{
  std::cout << "Creating " << (unsigned) m_nWifis << " nodes.\n";
  
  nodes2.Create (m_nWifis);
  nodes3.Create(2);
  nodes1.Create (m_nCsma);
  //nodes1.Add(nodes3.Get(1));
  AP1.Add(nodes3.Get(0));
  AP2.Add(nodes3.Get(1));
 
  NS_ASSERT_MSG (m_nWifis > m_nSinks, "Sinks must be less or equal to the number of nodes in network");
}

void
DsdvManetExample::SetupMobility ()
{
  MobilityHelper mobility;
  ObjectFactory pos;
  pos.SetTypeId ("ns3::RandomRectanglePositionAllocator");
  pos.Set ("X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1000.0]"));
  pos.Set ("Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1000.0]"));

  std::ostringstream speedConstantRandomVariableStream;
  speedConstantRandomVariableStream << "ns3::ConstantRandomVariable[Constant="
                                    << m_nodeSpeed
                                    << "]";

  Ptr <PositionAllocator> taPositionAlloc = pos.Create ()->GetObject <PositionAllocator> ();
  mobility.SetMobilityModel ("ns3::RandomWaypointMobilityModel", "Speed", StringValue (speedConstantRandomVariableStream.str ()),
                             "Pause", StringValue ("ns3::ConstantRandomVariable[Constant=2.0]"), "PositionAllocator", PointerValue (taPositionAlloc));
  mobility.SetPositionAllocator (taPositionAlloc);
  
  mobility.Install (nodes2);
  mobility.Install (nodes1);
 
  
}

void
DsdvManetExample::CreateDevices (std::string tr_name)
{
  
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue (m_rate));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
  devices3 = pointToPoint.Install (nodes3);
  

 
  WifiMacHelper wifiMac;
  wifiMac.SetType ("ns3::AdhocWifiMac");
  YansWifiPhyHelper wifiPhy;
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel");
  wifiPhy.SetChannel (wifiChannel.Create ());
  WifiHelper wifi;
  wifi.SetStandard (WIFI_STANDARD_80211b);
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue (m_phyMode), "ControlMode",
                                StringValue (m_phyMode));
  devices2 = wifi.Install (wifiPhy, wifiMac, nodes2);
  devices1 = wifi.Install (wifiPhy, wifiMac, nodes1);
  APdevices1 = wifi.Install (wifiPhy, wifiMac, AP1);
  APdevices2 = wifi.Install (wifiPhy, wifiMac, AP2);
  
  /*AsciiTraceHelper ascii;
  wifiPhy.EnableAsciiAll (ascii.CreateFileStream (tr_name + ".tr"));
  wifiPhy.EnablePcapAll (tr_name);
   std::cout << "\nStarting simulation for " << m_totalTime << " s ...\n";
   */
}

void
DsdvManetExample::InstallInternetStack (std::string tr_name)
{
  DsdvHelper dsdv;
  dsdv.Set ("PeriodicUpdateInterval", TimeValue (Seconds (m_periodicUpdateInterval)));
  dsdv.Set ("SettlingTime", TimeValue (Seconds (m_settlingTime)));
  
  InternetStackHelper stack;
  stack.SetRoutingHelper (dsdv); // has effect on the next Install ()
  
  stack.Install (nodes1);
  std::cout << "\nStarting simulation for " << m_totalTime << " s ...\n";
  stack.Install (nodes2);
  stack.Install (AP1);
  stack.Install (AP2);
  
  //wifi1
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  interfaces1 = address.Assign (devices1);
  address.Assign(APdevices2);

  //wifi2
  address.SetBase ("10.2.1.0", "255.255.255.0");
  interfaces2 = address.Assign (devices2);
  address.Assign(APdevices1);

  //p2p
  address.SetBase ("10.3.1.0", "255.255.255.0");
  address.Assign(devices3);
  
  if (m_printRoutes)
    {
      Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ((tr_name + ".routes"), std::ios::out);
      dsdv.PrintRoutingTableAllAt (Seconds (m_periodicUpdateInterval), routingStream);
    }
}

void
DsdvManetExample::InstallApplications ()
{
  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (nodes1.Get (m_nCsma));
  serverApps.Start (Seconds (m_dataStart));
  serverApps.Stop (Seconds (m_totalTime));

  UdpEchoClientHelper echoClient (interfaces1.GetAddress (m_nCsma), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (m_periodicUpdateInterval)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1000));

  for (uint32_t i = 0; i <= m_nSinks - 1; i++ )
    {
      Ptr<Node> node = NodeList::GetNode (i);
      Ipv4Address nodeAddress = node->GetObject<Ipv4> ()->GetAddress (1, 0).GetLocal ();
      Ptr<Socket> sink = SetupPacketReceive (nodeAddress, node);
    }

  for (uint32_t clientNode = 0; clientNode <= m_nWifis - 1; clientNode++ )
    {
      for (uint32_t j = 0; j <= m_nSinks - 1; j++ )
        {
          OnOffHelper onoff1 ("ns3::UdpSocketFactory", Address (InetSocketAddress (interfaces2.GetAddress (j), port)));
          onoff1.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
          onoff1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));

          if (j != clientNode)
            {
              ApplicationContainer apps1 = onoff1.Install (nodes2.Get (clientNode));
              Ptr<UniformRandomVariable> var = CreateObject<UniformRandomVariable> ();
              apps1.Start (Seconds (var->GetValue (m_dataStart, m_dataStart + 1)));
              apps1.Stop (Seconds (m_totalTime));
            }
        }
    }
}


