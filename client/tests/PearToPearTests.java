import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class PearToPearTests {
    public static void main(String args[]) {
        Peer Alice = new Peer("../tests/files",3333);
        Peer Bob = new Peer("../tests/files2",4444);
        Bob.getClass();
        Alice.getClass();
        Alice.announce();
        Bob.announce();
        List<String> criterias = new ArrayList<>();
        criterias.add("filename=\"Arlos.txt\"");
        List<Tracker_File> files = Alice.look(criterias);
        Tracker_File f = files.get(0);
        String key = f.getKey();
        Alice.getFile(key);

        System.out.println("\n ---début du test du P2P -- \n \n");

        Alice.send_interest(key);
        List<Integer> liste = new ArrayList<Integer>();
        liste.add(0);
        try {
            Alice.getPieces(key,liste);
        } catch (IOException e) {
            e.printStackTrace();
        }

        Alice.update(key);

        List<String> a = new ArrayList<>();
        List<String> b = new ArrayList<>();
        a.add(key);
        Alice.update_tracker(a,b);

        Alice.CloseConection();
        Bob.CloseConection();
        System.out.println("\n \n -----End of test P2P1-----\n\n");
    }
}
