import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.TimeUnit;

public class main2 {

    public static void main(String args[]){
        System.out.println("---- Begin of test 2");
        Peer Alice = new Peer("../tests/files",4444);
        Alice.announce();
        Peer Carlos = null;
        Carlos = new Peer("../tests/files2");
        Carlos.announce();

        try {
            TimeUnit.SECONDS.sleep(2);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        List<String> criterias = new ArrayList<>();
        criterias.add("filename=\"Shrek2.txt\"");
        List<Tracker_File> files = Carlos.look(criterias);
        Tracker_File f = files.get(0);
        Carlos.getFile(f.getKey());
        Carlos.CloseConection();
        Alice.CloseConection();
    }
}
