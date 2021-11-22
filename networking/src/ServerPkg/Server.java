package ServerPkg;



import resources.File_pair;
import resources.pair;
import resources.tuple;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Vector;

public class Server {
    static Vector<pair> loggedUsers=new Vector<>();
    //static Vector<tuple> files=new Vector<>();
    static Vector<tuple> activeList=new Vector<>();
    static Vector<File_pair> requests=new Vector<>();
    static  long max_buffer= 1024L *1024*1024*1024; //1MB
    static int min_chunk=20;
    static int max_chunk=1024;
    static long current_buffer=0;
    public static void main(String[] args) throws IOException {
        ServerSocket welcomeSocket = new ServerSocket(6666);

        while(true) {
            System.out.println("Waiting for connection...");
            Socket socket = welcomeSocket.accept();
            System.out.println("Connection established");

            // open thread
            Thread worker = new Worker(socket);
            worker.start();


        }

    }
}
