package resources;

import java.net.Socket;
import java.util.Queue;

public class tuple {
    public Queue queue;
    public Socket sock;
    public String id;
    public tuple(Socket s, Queue q,String id)
    {
        this.sock=s;
        this.queue=q;
        this.id=id;

    }
}
