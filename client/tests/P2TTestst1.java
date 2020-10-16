import java.util.ArrayList;
import java.util.List;

public class P2TTestst1 {

    public static void main(String args[]){
        System.out.println("---- Begin of test 1");
        Peer Joey = null;
        Joey = new Peer("../tests/files",3333);
        Joey.announce();
        List<String> criterias = new ArrayList<>();
        criterias.add("filename=\"Shrek2.txt\"");
        Joey.look(criterias);
        Joey.CloseConection();
        return;
    }
}
