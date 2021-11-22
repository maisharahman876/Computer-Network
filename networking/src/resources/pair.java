package resources;

import java.io.PrintWriter;

public class pair {
    //PrintWriter p;
    String id;
    boolean ac;
    public pair(String idd)
    {
        //this.p=pw;
        this.id=idd;
        ac=false;
    }
   /* public PrintWriter get_PrintWriter()
    {
        return p;
    }*/
    public String get_id()
    {
        return id;
    }
    public boolean get_activity()
    {
        return ac;
    }
    public void active()
    {
        ac=true;
    }
    public void inactive()
    {
        ac=false;
    }

}
