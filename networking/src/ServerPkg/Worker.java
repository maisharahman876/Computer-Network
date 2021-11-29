package ServerPkg;


import resources.File_pair;
import resources.pair;
import resources.tuple;

import java.io.*;
import java.net.Socket;
import java.net.SocketException;
import java.util.*;

public class Worker extends Thread {
    Socket socket;
    String curr_id;
    pair curr_pair;
    tuple curr_tuple;
    String curr_file;
    //boolean active;
    Queue<String> q;
    FileOutputStream fos;

    public Worker(Socket socket) throws SocketException {
        q= new LinkedList<>();
        this.socket = socket;
        curr_id=null;
        curr_tuple=null;
        curr_pair=null;
        curr_file=null;
        fos=null;

    }

    public void run()
    {
        // buffers
        String cmd;
        try {
            ObjectInputStream in = new ObjectInputStream(this.socket.getInputStream());
            ObjectOutputStream out = new ObjectOutputStream(this.socket.getOutputStream());
            while (true)
            {


                cmd = (String) in.readObject();
                StringTokenizer tokenizer = new StringTokenizer(cmd, " ");
                String[] userInput = new String[1000];
                int i = 0;
                //System.out.println(cmd);
                while (tokenizer.hasMoreTokens()) {
                    userInput[i] = tokenizer.nextToken();
                    i++;
                }

                if (userInput[0].equalsIgnoreCase("login")) {
                    if(curr_id!=null)
                    {
                        out.writeObject("An account is already logged in. In order to login to another account logout first");
                        continue;
                    }
                    boolean login = false;


                    for (int k = 0; k < Server.loggedUsers.size(); k++) {

                        if (Server.loggedUsers.get(k).get_id().equals(userInput[1])&&Server.loggedUsers.get(k).get_activity())
                        {
                            out.writeObject("This User already exists");
                            login=true;

                            break;
                        }
                        else if(Server.loggedUsers.get(k).get_id().equals(userInput[1])&&!Server.loggedUsers.get(k).get_activity())
                        {
                            out.writeObject("Logged in successfully");
                            login=true;
                            curr_id=new String(userInput[1]);
                            Server.loggedUsers.get(k).active();

                            //Queue q1=new LinkedList();
                            tuple t=new tuple(socket,q,curr_id);
                            curr_tuple=t;
                            Server.activeList.add(t);
                            break;
                        }

                    }

                    if (!login) {
                        pair p=new pair( userInput[1]);
                        curr_pair=p;
                        Server.loggedUsers.add(p);
                        curr_id=new String(userInput[1]);
                        p.active();
                        //Queue q1=new LinkedList();
                        tuple t=new tuple(socket,q,curr_id);
                        curr_tuple=t;
                        Server.activeList.add(t);
                        File f=new File("src\\"+userInput[1]+"\\public");
                        File f1=new File("src\\"+userInput[1]+"\\private");
                        boolean b=f.mkdirs();
                        boolean b1=f1.mkdirs();
                        out.writeObject("Logged in successfully");
                    }



                }
                else if (userInput[0].equalsIgnoreCase("lookup")) {
                    if(userInput[1].equalsIgnoreCase("list"))
                    {
                        if(curr_id==null)
                        {
                            out.writeObject("You are not logged in ");
                            continue;
                        }
                        String s="";
                        for(int k=0;k<Server.loggedUsers.size();k++)
                        {
                            if(Server.loggedUsers.get(k).get_activity())
                            {
                                s+=Server.loggedUsers.get(k).get_id()+":active\n";
                                //notify.println();
                            }
                            else
                                s+=Server.loggedUsers.get(k).get_id()+":not active\n";
                        }
                       out.writeObject(s);
                    }
                   else if(userInput[1].equalsIgnoreCase("files"))
                    {
                        if(curr_id==null)
                        {
                            out.writeObject("You are not logged in ");
                            continue;
                        }
                        String s="";
                        File folder = new File("src\\"+curr_id+"\\private");
                        File[] listOfFiles = folder.listFiles();
                        File folder1 = new File("src\\"+curr_id+"\\public");
                        File[] listOfFiles1 = folder1.listFiles();
                        for (File file : listOfFiles) {
                            if (file.isFile()) {
                                s+=file.getName()+"(private)\n";
                            }
                        }
                        for (File file : listOfFiles1) {
                            if (file.isFile()) {
                                s+=file.getName()+"(public)\n";
                            }
                        }
                        if(s.equalsIgnoreCase(""))
                            s="You don't have any file.";
                        out.writeObject(s);

                    }
                    else if(userInput[2].equalsIgnoreCase("files"))
                    {
                        if(curr_id==null)
                        {
                            out.writeObject("You are not logged in ");
                            continue;
                        }

                        String s="";

                        File folder1 = new File("src\\"+userInput[1]+"\\public");
                        File[] listOfFiles1 = folder1.listFiles();

                        for (File file : listOfFiles1) {
                            if (file.isFile()) {
                                s+=file.getName()+"\n";
                            }
                        }
                        if(s.equalsIgnoreCase(""))
                            s="There is no file.";
                        out.writeObject(s);

                    }
                   else continue;;

                }

                else if(userInput[0].equalsIgnoreCase("logout")) {
                    if(curr_id==null)
                    {
                        out.writeObject("You are not logged in ");
                        continue;
                    }
                    String st="";
                    for (int k = 0; k < Server.loggedUsers.size(); k++) {
                        pair p=Server.loggedUsers.get(k);
                        String n=p.get_id();
                        System.out.println(curr_id+ "  "+n);
                        if (n.equals(curr_id)) {
                            p.inactive();

                            // System.out.println("paise");
                            break;

                        }
                    }


                    Server.activeList.remove(curr_tuple);
                    out.writeObject("logged out");
                    curr_id=null;
                    curr_pair.inactive();
                    curr_tuple=null;




                }
                else if(userInput[0].equalsIgnoreCase("upload")) {
                    if(curr_id==null)
                    {
                        out.writeObject("You are not logged in ");
                        continue;
                    }

                    String typ=userInput[2];
                    String name=userInput[1];
                    String req_id=userInput[4];
                    if(req_id != null)
                    {


                           for (int k=0;k<Server.requests.size();k++)
                           {
                               File_pair fp=Server.requests.get(k);
                              // System.out.println(fp.req_ID);
                               if(fp.req_ID.equals(req_id))
                               {
                                   //System.out.println("dhuke");
                                   String id= fp.requester;
                                   for(int j=0;j<Server.activeList.size();j++)
                                   {
                                       tuple t=Server.activeList.get(k);
                                       if(t.id.equals(id))
                                       {
                                           t.queue.add("Student No "+curr_id+" has accepted your request\nRequest ID: "+req_id+"\nFile ID: "+curr_id+"_"+name+"\n" );
                                       }
                                   }
                                   break;
                               }
                           }
                    }

                    //tuple t=new tuple(curr_id,userInput[1],curr_id+"_"+userInput[1],userInput[2]);
                    if(Server.current_buffer+Long.parseLong(userInput[3])<=Server.max_buffer)
                    {
                        Server.current_buffer+=Long.parseLong(userInput[3]);
                        //Server.files.add(t);
                        int max=Server.max_chunk,min=Server.min_chunk;

                        int chunk= (int)Math.floor(Math.random()*(max-min+1)+min);
                        //int chunk=1024;
                        out.writeObject(curr_id+"_"+userInput[1]+" "+chunk);
                        long size=Long.parseLong(userInput[3]);
                        byte[] contents = new byte[chunk];

                        //Initialize the FileOutputStream to the output file's full path.
                         fos = new FileOutputStream("src\\"+curr_id+"\\"+typ+"\\"+curr_id+"_"+name);
                        BufferedOutputStream bos = new BufferedOutputStream(fos);
                        curr_file="src\\"+curr_id+"\\"+typ+"\\"+curr_id+"_"+name;
                        //InputStream is = socket.getInputStream();
                        //No of bytes read in one read() call
                        //contents=in.readAllBytes();
                        int bytesRead = 0;
                        int total=0;			//how many bytes read
                        int chunk_no=1;
                        boolean check=false;

                            while((size-total)>0)	//loop is continued until received byte=totalfilesize
                            {

                                bytesRead=in.read(contents);
                                total+=bytesRead;
                                bos.write(contents, 0, bytesRead);
                                out.writeObject("Chunck No."+chunk_no+" received");
                                Thread.sleep(10000);
                                out.flush();
                                chunk_no++;
                                String m= (String) in.readObject();
                                if(m.equalsIgnoreCase("timeout"))
                                {
                                    check=true;
                                    break;
                                }

                            }
                            if(check)
                            {

                                    //System.out.println(curr_file);
                                    try {
                                        fos.close();
                                    } catch (IOException ioException) {
                                        ioException.printStackTrace();
                                    }
                                    File f=new File(curr_file);
                                    f.delete();

                            }
                            else {
                                bos.flush();
                                fos.close();
                                out.writeObject("File Sending Complete!");
                                out.flush();
                                //in.reset();
                                Server.current_buffer -= size;
                                System.out.println("File saved successfully!");
                                curr_file = null;


                            }
                        //System.out.println("ashe");

                    }
                    else
                    {
                        out.writeObject("Buffer Size Overflow");
                    }
                }
                else if(userInput[0].equalsIgnoreCase("download")) {
                    if (curr_id == null) {
                        out.writeObject("You are not logged in ");
                        continue;
                    }

                        //System.out.println("ashe");
                    StringTokenizer tokenizer3 = new StringTokenizer(userInput[1], "_");
                    String[] f_name = new String[1000];
                    int q = 0;
                    //System.out.println(msg);
                    while (tokenizer3.hasMoreTokens()) {
                        f_name[q] = tokenizer3.nextToken();
                        q++;
                    }
                    System.out.println(f_name[0]);
                        File file=new File("src\\"+f_name[0]+"\\public\\"+userInput[1]);
                        if(!file.exists()&&f_name[0].equals(curr_id))
                        {
                            file=new File("src\\"+f_name[0]+"\\private\\"+userInput[1]);
                        }
                       if(!file.exists())
                       {
                           out.writeObject("This File doesn't exists");

                       }
                        else {
                           FileInputStream fis = new FileInputStream(file);
                           BufferedInputStream bis = new BufferedInputStream(fis);
                           // OutputStream os = socket.getOutputStream();

                           byte[] contents;
                           long fileLength = file.length();
                           out.writeObject("download "+Server.max_chunk+" "+fileLength);
                           long current = 0;

                           //System.out.println("ashe");

                           // DataOutputStream do =new DataOutputStream(bis);
                           while (current != fileLength) {
                               int size = Server.max_chunk;
                               if (fileLength - current >= size)
                                   current += size;
                               else {
                                   size = (int) (fileLength - current);
                                   current = fileLength;
                               }
                               //System.out.println(fileLength);
                               contents = new byte[size];
                               bis.read(contents, 0, size);
                               out.write(contents);
                               //System.out.println(contents);

                               System.out.println("Sending file ... " + (current * 100) / fileLength + "% complete!");
                           }
                           fis.close();;



                       }
                }
                else if(userInput[0].equalsIgnoreCase("request")) {
                    if (curr_id == null) {
                        out.writeObject("You are not logged in ");
                        continue;
                    }
                    String m1= (String) in.readObject();
                    System.out.println(m1);
                    //q.add();
                    out.writeObject("Request Sent");
                    File_pair fp=new File_pair(userInput[1],curr_id);
                    Server.requests.add(fp);
                    for(int k=0;k<Server.activeList.size();k++)
                    {
                        tuple t=Server.activeList.get(k);
                        if(t.sock.equals(socket))
                            continue;
                        else
                            t.queue.add("RequestID: "+userInput[1]+"\n"+m1);
                    }
                }
                else if(userInput[0].equalsIgnoreCase("view")) {
                    if (curr_id == null) {
                        out.writeObject("You are not logged in ");
                        continue;
                    }
                    String s="";
                    for(int k=0;k<q.size();k++)
                    {
                        s+=q.peek()+"\n";
                        q.poll();
                    }
                    if(s.equalsIgnoreCase(""))
                        s="You don't have any message.";
                    out.writeObject(s);
                }
                    else
                    continue;
               out.flush();
            }
        } catch (IOException | ClassNotFoundException | InterruptedException e) {
            if(curr_pair!=null)
                curr_pair.inactive();
            if(curr_tuple!=null)
                Server.activeList.remove(curr_tuple);
                if(curr_file!=null)
                {
                    //System.out.println(curr_file);
                    try {
                        fos.close();
                    } catch (IOException ioException) {
                        ioException.printStackTrace();
                    }
                    File f=new File(curr_file);
                    f.delete();
                }
        }
    }
}
