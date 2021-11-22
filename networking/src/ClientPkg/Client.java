package ClientPkg;

import ServerPkg.Server;

import java.io.*;
import java.net.Socket;
import java.util.Scanner;
import java.util.StringTokenizer;

public class Client {
    public static void main(String[] args) throws IOException, ClassNotFoundException {
        Socket socket = new Socket("localhost", 6666);
        System.out.println("Connection established");
        System.out.println("Remote port: " + socket.getPort());
        System.out.println("Local port: " + socket.getLocalPort());

        // buffers
        ObjectOutputStream out = new ObjectOutputStream(socket.getOutputStream());
        ObjectInputStream in = new ObjectInputStream(socket.getInputStream());
        Scanner sc=new Scanner(System.in);
        //
        while(true) {
            System.out.println("1.Login");
            System.out.println("2.Lookup List");
            System.out.println("3.Lookup Self Files");
            System.out.println("4.Lookup Others' Files");
            System.out.println("5.Upload File");
            System.out.println("6.Download File");
            System.out.println("7.Request for a File");
            System.out.println("8.View Inbox");
            System.out.println("9.Logout");
           // System.out.println("10.Upload File in response of request");

           int input=sc.nextInt();
            if(input==1)
            {
                System.out.println("Enter Student No. : ");
                out.writeObject("login "+sc.next());
                String msg = (String) in.readObject();
                System.out.println(msg);
            }
            else if(input==2)
            {
                //System.out.println("Enter Student No. : ");
                out.writeObject("lookup list");
                String msg = (String) in.readObject();
                System.out.println(msg);
            }
            else if(input==3)
            {
                //System.out.println("Enter Student No. : ");
                out.writeObject("lookup files");
                String msg = (String) in.readObject();
                System.out.println(msg);
            }
            else if(input==4)
            {
                System.out.println("Enter Student No. of that student: ");
                out.writeObject("lookup "+sc.next()+" files");
                String msg = (String) in.readObject();
                System.out.println(msg);
            }
            else if(input==9)
            {
                //System.out.println("Enter Student No. of that student: ");
                out.writeObject("logout");
                String msg = (String) in.readObject();
                System.out.println(msg);
            }

           else if(input ==5)
            {
                //System.out.println("ashe");
                System.out.println("Enter file directory: ");
                String dir=sc.next();
                File file=new File(dir);
                FileInputStream fis = new FileInputStream(file);
                BufferedInputStream bis = new BufferedInputStream(fis);
               // OutputStream os = socket.getOutputStream();
                StringTokenizer tokenizer2 = new StringTokenizer(dir, "\\");
                String[] f_name = new String[1000];
                int q = 0;
                //System.out.println(msg);
                while (tokenizer2.hasMoreTokens()) {
                    f_name[q] = tokenizer2.nextToken();
                    q++;
                }
                byte[] contents;
                long fileLength = file.length();
                System.out.println("Are you uploading in response of a request?\n1.yes\n2.No");
                int choice=sc.nextInt();
                if(choice==2) {
                    System.out.println("Enter file type(private/public): ");
                    out.writeObject("upload " + f_name[q - 1] + " " + sc.next() + " " + fileLength);
                }
                else if(choice==1)
                {
                    System.out.println("Enter Request ID:");
                    out.writeObject("upload " + f_name[q - 1] + " public " + fileLength+ " "+sc.next());
                }
                String msg = (String) in.readObject();
                System.out.println(msg);
                long current = 0;
                //long start = System.nanoTime();

                //String cmd = m;
                StringTokenizer tokenizer1 = new StringTokenizer(msg, " ");
                String[] serverInput = new String[1000];

                int i = 0;
                //System.out.println(msg);
                while (tokenizer1.hasMoreTokens()) {
                    serverInput[i] = tokenizer1.nextToken();
                    i++;
                }


                if(!serverInput[0].equalsIgnoreCase("buffer")) {
                    // DataOutputStream do =new DataOutputStream(bis);
                   socket.setSoTimeout(1000);
                    try {
                        while (current != fileLength) {
                            int size = Integer.parseInt(serverInput[1]);
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
                            out.flush();
                            String msg1 = (String) in.readObject();
                            System.out.println(msg1);
                            out.writeObject("meo");
                            //System.out.println(contents);

                            // System.out.println("Sending file ... " + (current * 100) / fileLength + "% complete!");
                        }
                        String msg1 = (String) in.readObject();
                        System.out.println(msg1);
                    }
                    catch (java.net.SocketTimeoutException e) {
                        out.writeObject("Timeout");
                        out.flush();
                        System.out.println("Timeout");
                    }

                }
                //fis.close();;
                //out.flush();


            }
            else if(input==6) {
                //System.out.println("ashe");
                System.out.println("Enter file name: ");
                String dir=sc.next();
                out.writeObject("download "+dir);
                String msg = (String) in.readObject();
                System.out.println(msg);
                //out.writeObject("hello");

                //out.writeObject(curr_id+"_"+userInput[1]+" "+chunk);
                StringTokenizer tokenizer1 = new StringTokenizer(msg, " ");
                String[] serverInput = new String[1000];

                int i = 0;
                //System.out.println(msg);
                while (tokenizer1.hasMoreTokens()) {
                    serverInput[i] = tokenizer1.nextToken();
                    i++;
                }
                if (serverInput[0].equals("download")) {
                    long sz=Long.parseLong(serverInput[1]);
                    long len=Long.parseLong(serverInput[2]);
                    byte[] contents;
                    contents = new byte[(int) sz];

                    //Initialize the FileOutputStream to the output file's full path.
                    FileOutputStream fos = new FileOutputStream("downloads\\"+dir);
                    BufferedOutputStream bos = new BufferedOutputStream(fos);
                    //InputStream is = socket.getInputStream();
                    //No of bytes read in one read() call
                    //contents=in.readAllBytes();
                    int bytesRead = 0;
                    int total = 0;            //how many bytes read
                    System.out.println("ashe");
                    while ((len-total) > 0)    //loop is continued until received byte=totalfilesize
                    {
                        bytesRead=in.read(contents);
                        total += bytesRead;
                        bos.write(contents, 0, bytesRead);

                    }

                    System.out.println("ashe");
                    bos.flush();
                    fos.close();

                    //in.reset();

                    System.out.println("File saved successfully!");
                }
            }
            else if(input==7)
            {
                System.out.println("Enter Request ID: ");

                out.writeObject("request "+sc.next());
                out.flush();
                System.out.println("Enter File description: ");
                BufferedReader br=new BufferedReader(new InputStreamReader(System.in));
                String n="";
                out.writeObject(br.readLine());
                String msg = (String) in.readObject();
                System.out.println(msg);
            }
            else if(input==8)
            {
                out.writeObject("view");
                String msg = (String) in.readObject();
                System.out.println(msg);
            }

            else
            {
                out.writeObject(input);
                String msg = (String) in.readObject();
                System.out.println(msg);
            }

            out.flush();
        }
    }
}
