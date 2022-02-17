/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 */

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/dsdv-helper.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/netanim-module.h"
#include<string>
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor-module.h"
 
// Default Network Topology
//
//   Wifi 10.1.3.0
//                 AP
//  *    *    *    *
//  |    |    |    |    10.1.1.0
// n5   n6   n7   n0 -------------- n1  
//                   point-to-point  |   
//                                   *    
//                                     

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TaskA_1");

int 
main (int argc, char *argv[])
{
  bool verbose = true;
 
  uint32_t nWifi = 10;
  int nSinks = 5;
  uint32_t pktpersec = 100;
  uint32_t coverageArea = 50;
  bool tracing = false;

  CommandLine cmd (__FILE__);
  cmd.AddValue ("nWifi", "The number of nodes in the topology", nWifi);
  cmd.AddValue ("nSinks", "The number of flows",nSinks);
  cmd.AddValue ("pktpersec", "Packets per second ",pktpersec);
  cmd.AddValue ("coverageArea", "Coverage area ",coverageArea);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

  cmd.Parse (argc,argv);

  // The underlying restriction of 18 is due to the grid position
  // allocator's configuration; the grid layout will exceed the
  // bounding box if more than 18 nodes are provided.
 
  nWifi-=2;
  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }
  int pktSize=64;
  int rate=pktSize*pktpersec;
  //Config::SetDefault("ns3::RangePropagationLossModel::MaxRange",DoubleValue(5));

  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue (std::to_string(rate)+"bps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);


  

  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (nWifi);
  NodeContainer wifiApNode = p2pNodes.Get (0);

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  channel.AddPropagationLoss("ns3::RangePropagationLossModel","MaxRange",DoubleValue(coverageArea));
  YansWifiPhyHelper phy;
  phy.SetChannel (channel.Create ());

  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper mac;
  Ssid ssid = Ssid ("ns-3-ssid");
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices;
  
  staDevices = wifi.Install (phy, mac, wifiStaNodes);

  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

  NetDeviceContainer apDevices;
  apDevices = wifi.Install (phy, mac, wifiApNode);

  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));


  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
   mobility.Install (wifiStaNodes);
  mobility.Install (p2pNodes);

  DsdvHelper dsdv;
  dsdv.Set ("PeriodicUpdateInterval", TimeValue (Seconds (15)));
  dsdv.Set ("SettlingTime", TimeValue (Seconds (6)));
  
  InternetStackHelper stack;
  stack.SetRoutingHelper (dsdv);
 
  stack.Install (p2pNodes);
  stack.Install (wifiStaNodes);

  Ipv4AddressHelper address;

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  address.Assign (staDevices);
  address.Assign (apDevices);



  for(int i=0;i<nSinks;i++)
  {
  UdpEchoServerHelper echoServer (i);
  ApplicationContainer serverApps = echoServer.Install (p2pNodes.Get (i%2));
  serverApps.Start (Seconds (1.0+i));
  serverApps.Stop (Seconds (10.0+i));

  UdpEchoClientHelper echoClient (p2pInterfaces.GetAddress (i%2), i);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (pktSize));
  //echoClient.SetAttribute("DataRate",StringValue (std::to_string(rate)+"bps"));

  ApplicationContainer clientApps = 
    echoClient.Install (wifiStaNodes.Get (nWifi - 1-i));
  clientApps.Start (Seconds (2.0+i));
  clientApps.Stop (Seconds (10.0+i));
  }

  

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Simulator::Stop (Seconds (10.0));

 /* if (tracing)
    {
      phy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);
      pointToPoint.EnablePcapAll ("third");
      phy.EnablePcap ("third", apDevices.Get (0));
      csma.EnablePcap ("third", csmaDevices.Get (0), true);
    }
    */
  AnimationInterface anim ("taskA_1.xml");
  Ptr<FlowMonitor> flowMonitor;
  FlowMonitorHelper flowHelper;
  flowMonitor=flowHelper.InstallAll();
  Simulator::Run ();
  flowMonitor->SerializeToXmlFile("taskA_1.flowmonitor",false,false);
  Simulator::Destroy ();
  return 0;
}
