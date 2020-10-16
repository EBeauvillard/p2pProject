import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

public class main {

    public static void main(String[] args) {

        Scanner scanner = new Scanner(System.in);

        System.out.print("Hello! Please give us your directory to upload\n");

        String command = scanner.next();
        System.out.print("What port do you want to use?\n");

        int port = scanner.nextInt();

        Peer me = new Peer("../" + command,port);
        me.announce();

        while (true) {
            // create a scanner so we can read the command-line input
            //  prompt for the user's name
            System.out.print("To search for a file: Search + *the name of the file to search\nTo download a file : Download + *Key of the file to download\n");

            // get their input as a String
            command = scanner.next();

            if (command.startsWith("Search")) {
                System.out.println("what name to search\n");
                command = scanner.next();
                String req = "filename=\"" + command + "\"";
                List<String>  l = new ArrayList<>();
                l.add(req);
                List<Tracker_File> res = me.look(l);
                for (int i = 0; i < res.size(); i++){
                    Tracker_File f =res.get(i);
                    System.out.println("File n°" + i + ": " + f.getName() + " of size " + f.getLength() + " and of Key " + f.getKey() + "\n");
                }
            }else {
                if (command.startsWith("Download")) {
                    System.out.println("what key to Download?\n");
                    command = scanner.next();
                    me.Download(command);
            }else {
                    System.out.println("wrong command\n");
                }
            }


        }

    }
}
