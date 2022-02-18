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
#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
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
using namespace std;
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
  std::string csv="scratch/taskA_1.csv";

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

  InternetStackHelper stack;
  //stack.SetRoutingHelper (dsdv);
 
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
  UdpEchoServerHelper echoServer (i+1);
  ApplicationContainer serverApps = echoServer.Install (p2pNodes.Get (i%2));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (p2pInterfaces.GetAddress (i%2), i+1);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (10));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (pktSize));
  //echoClient.SetAttribute("DataRate",StringValue (std::to_string(rate)+"bps"));

  ApplicationContainer clientApps = 
  echoClient.Install (wifiStaNodes.Get (nWifi - 1-i));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));
  //Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  }
Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  

  
  uint32_t rxPacketsum = 0;
  double Delaysum = 0; 
  uint32_t txPacketsum = 0;
  uint32_t txBytessum = 0;
  uint32_t rxBytessum = 0;
  uint32_t txTimeFirst = 0;
  uint32_t rxTimeLast = 0;
  uint32_t lostPacketssum = 0;

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
 // uint16_t k = 0;
  
  flowMonitor->CheckForLostPackets ();
  
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowHelper.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = flowMonitor->GetFlowStats ();
  
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
	  //Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
	  rxPacketsum += i->second.rxPackets;
	  txPacketsum += i->second.txPackets;
	  txBytessum += i->second.txBytes;
	  rxBytessum += i->second.rxBytes;
	  Delaysum += i->second.delaySum.GetSeconds();
	  lostPacketssum += i->second.lostPackets;
	  txTimeFirst += i->second.timeFirstTxPacket.GetSeconds();
	  rxTimeLast += i->second.timeLastRxPacket.GetSeconds();
	  
     }
  
  
  uint64_t timeDiff = (rxTimeLast - txTimeFirst);
  
  //uint64_t rcvd = sermon->GetReceived();
  //uint16_t lst = sermon->GetLost();
  //std::cout << k << "    " << "            " << rxTimeLast << "         " <<txTimeFirst<< "\n\n";
  //std::cout<<"rec pkts"<<rxPacketsum<<"\n";
  flowMonitor->SerializeToXmlFile("taskA_1.flowmonitor",false,false);
  std::cout << "\n\n";

  std::cout << "  Throughput: " << ((rxBytessum * 8.0) / timeDiff)/1024<<" Kbps"<<"\n";
  std::cout << "  End to End Delay: " << Delaysum<<"s"<<"\n";
  std::cout << "  Packets Delivery Ratio: " << (((txPacketsum - lostPacketssum) * 100) /txPacketsum) << "%" << "\n";
  std::cout << "  Packets Drop Ratio: " << (((lostPacketssum) * 100) /txPacketsum) << "%" << "\n";
  Simulator::Destroy ();
  return 0;
}
