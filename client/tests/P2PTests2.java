import java.util.ArrayList;
import java.util.List;

public class P2PTests2 {
    public static void main(String args[]){
        System.out.println("\n\n----------------------------P2PTests2----------------------\n\n");
        Peer Alice = new Peer("../tests/files",3333);
        Peer Bob = new Peer("../tests/files3",4444);
        Bob.getClass();
        Alice.getClass();
        System.out.println("clients created");
        Alice.announce();
        Bob.announce();
        List<String> criterias = new ArrayList<>();
        criterias.add("filename=\"Camus.txt\"");
        List<Tracker_File> files = Alice.look(criterias);
        Tracker_File f = files.get(0);
        String key = f.getKey();


        System.out.println("\n------begining of Download----------\n");


        Alice.Download(key);


        Alice.update(key);

        List<String> a = new ArrayList<>();
        List<String> b = new ArrayList<>();
        a.add(key);
        Alice.update_tracker(a,b);

        Alice.CloseConection();
        Bob.CloseConection();
        System.exit(0);
    }
}
