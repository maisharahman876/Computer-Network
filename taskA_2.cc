/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2019 Universita' di Firenze, Italy
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
 * Author: Tommaso Pecorella <tommaso.pecorella@unifi.it>
 */

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/mobility-module.h"
#include "ns3/spectrum-module.h"
#include "ns3/network-module.h"
#include "ns3/propagation-module.h"
#include "ns3/sixlowpan-module.h"
#include "ns3/lr-wpan-module.h"
#include "ns3/csma-module.h"
#include "ns3/constant-velocity-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

int main (int argc, char** argv)
{
  bool verbose = false;
  uint32_t nWifi = 10;
  int nSinks =5;
  uint32_t pktpersec = 100;
  int Speed=5;
  Packet::EnablePrinting ();

  CommandLine cmd (__FILE__);
  
  cmd.AddValue ("verbose", "turn on log components", verbose);
  cmd.AddValue ("nWifi", "The number of nodes in the topology", nWifi);
  cmd.AddValue ("nSinks", "The number of flows",nSinks);
  cmd.AddValue ("pktpersec", "Packets per second ",pktpersec);
  cmd.AddValue ("Speed", "Speed of Nodes ",Speed);
  cmd.Parse (argc, argv);

  if (verbose)
    {
      LogComponentEnable ("Ping6Application", LOG_LEVEL_ALL);
      LogComponentEnable ("LrWpanMac", LOG_LEVEL_ALL);
      LogComponentEnable ("LrWpanPhy", LOG_LEVEL_ALL);
      LogComponentEnable ("LrWpanNetDevice", LOG_LEVEL_ALL);
      LogComponentEnable ("SixLowPanNetDevice", LOG_LEVEL_ALL);
    }
  int rate=64*pktpersec;
  uint32_t nWsnNodes = nWifi-1;
  NodeContainer wsnNodes;
  wsnNodes.Create (nWsnNodes);

  NodeContainer wiredNodes;
  wiredNodes.Create (1);
  wiredNodes.Add (wsnNodes.Get (0));

  MobilityHelper mobility;
   std::stringstream ssSpeed;
  ssSpeed << "ns3::UniformRandomVariable[Min=0.0|Max=" << Speed << "]";

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5),
                                 "DeltaY", DoubleValue (10),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Speed", StringValue (ssSpeed.str ()),
                             "Bounds", RectangleValue (Rectangle (-500, 500, -500, 500)));
  mobility.Install (wsnNodes);

  LrWpanHelper lrWpanHelper;
  // Add and install the LrWpanNetDevice for each node
  
  NetDeviceContainer lrwpanDevices = lrWpanHelper.Install (wsnNodes);

  // Fake PAN association and short address assignment.
  // This is needed because the lr-wpan module does not provide (yet)
  // a full PAN association procedure.
  lrWpanHelper.AssociateToPan (lrwpanDevices, 0);

  InternetStackHelper internetv6;
  internetv6.Install (wsnNodes);
  internetv6.Install (wiredNodes.Get (0));

  SixLowPanHelper sixLowPanHelper;
  //sixLowPanHelper.SetChannelAttribute ("DataRate", StringValue (std::to_string(rate)+"bps"));
  NetDeviceContainer sixLowPanDevices = sixLowPanHelper.Install (lrwpanDevices);

  CsmaHelper csmaHelper;
   csmaHelper.SetChannelAttribute ("DataRate", StringValue (std::to_string(rate)+"bps"));
  NetDeviceContainer csmaDevices = csmaHelper.Install (wiredNodes);

  Ipv6AddressHelper ipv6;
  ipv6.SetBase (Ipv6Address ("2001:cafe::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer wiredDeviceInterfaces;
  wiredDeviceInterfaces = ipv6.Assign (csmaDevices);
  wiredDeviceInterfaces.SetForwarding (1, true);
  wiredDeviceInterfaces.SetDefaultRouteInAllNodes (1);

  ipv6.SetBase (Ipv6Address ("2001:f00d::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer wsnDeviceInterfaces;
  wsnDeviceInterfaces = ipv6.Assign (sixLowPanDevices);
  wsnDeviceInterfaces.SetForwarding (0, true);
  wsnDeviceInterfaces.SetDefaultRouteInAllNodes (0);

  for (uint32_t i = 0; i < sixLowPanDevices.GetN (); i++)
    {
      Ptr<NetDevice> dev = sixLowPanDevices.Get (i);
      dev->SetAttribute ("UseMeshUnder", BooleanValue (true));
      dev->SetAttribute ("MeshUnderRadius", UintegerValue (10));
    }

  uint32_t packetSize = 64;
  //uint32_t maxPacketCount = pktpersec;
  Time interPacketInterval = Seconds (1.);

 for(int i=0;i<nSinks;i++)
  {
  UdpEchoServerHelper echoServer (i);
  ApplicationContainer serverApps = echoServer.Install (wiredNodes.Get (i%2));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (wiredDeviceInterfaces.GetAddress (i%2,1), i);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (10));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (packetSize));

  ApplicationContainer clientApps = 
    echoClient.Install (wsnNodes.Get (nWsnNodes - 1-i));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));
  } 
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
/*
for(int i=0;i<nSinks;i++){
  Ping6Helper ping6;
  ping6.SetLocal (wsnDeviceInterfaces.GetAddress (nWsnNodes - 1-i, 1));
  ping6.SetRemote (wiredDeviceInterfaces.GetAddress (0, 1));

  ping6.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  ping6.SetAttribute ("Interval", TimeValue (interPacketInterval));
  ping6.SetAttribute ("PacketSize", UintegerValue (packetSize));
  ApplicationContainer apps = ping6.Install (wsnNodes.Get (nWsnNodes - 1-i));

  apps.Start (Seconds (1.0+i));
  apps.Stop (Seconds (10.0+i));
}
  AsciiTraceHelper ascii;
  lrWpanHelper.EnableAsciiAll (ascii.CreateFileStream ("Ping-6LoW-lr-wpan-meshunder-lr-wpan.tr"));
  lrWpanHelper.EnablePcapAll (std::string ("Ping-6LoW-lr-wpan-meshunder-lr-wpan"), true);

  csmaHelper.EnableAsciiAll (ascii.CreateFileStream ("Ping-6LoW-lr-wpan-meshunder-csma.tr"));
  csmaHelper.EnablePcapAll (std::string ("Ping-6LoW-lr-wpan-meshunder-csma"), true);
*/
  uint32_t rxPacketsum = 0;
  double Delaysum = 0; 
  uint32_t txPacketsum = 0;
  uint32_t txBytessum = 0;
  uint32_t rxBytessum = 0;
  uint32_t txTimeFirst = 0;
  uint32_t rxTimeLast = 0;
  uint32_t lostPacketssum = 0;
  Simulator::Stop (Seconds (10));
AnimationInterface anim ("taskA_2.xml");
  Ptr<FlowMonitor> flowMonitor;
  FlowMonitorHelper flowHelper;
  flowMonitor=flowHelper.InstallAll();
  Simulator::Run ();
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
  flowMonitor->SerializeToXmlFile("taskA_2.flowmonitor",false,false);
  std::cout << "\n\n";

  std::cout << "  Throughput: " << ((rxBytessum * 8.0) / timeDiff)/1024<<" Kbps"<<"\n";
  std::cout << "  End to End Delay: " << Delaysum<<"s"<<"\n";
  std::cout << "  Packets Delivery Ratio: " << (((txPacketsum - lostPacketssum) * 100) /txPacketsum) << "%" << "\n";
  std::cout << "  Packets Drop Ratio: " << (((lostPacketssum) * 100) /txPacketsum) << "%" << "\n";
  Simulator::Destroy ();

}
