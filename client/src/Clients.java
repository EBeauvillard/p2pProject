import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;

enum requests{
    INTREST,
    GETPIECES,
    HAVE,
    NONE;
}

public class Clients implements Runnable,Comparable<Clients> {

    Socket socket;
    private int file_descriptor;
    private PrintWriter writer = null;
    private BufferedInputStream reader = null;
    private boolean connection = true;
    private Peer myself;
    InetAddress Adress;
    int Port;

    Clients(InetAddress adress, int port, Peer p){
        Port = port;
        Adress = adress;
        myself = p;
    }

    Clients(Socket s, Peer p){
        socket = s;
        myself = p;
    }

    void Connect() throws IOException {
        socket = new Socket(Adress,Port);
    }

    void Disconnect() throws IOException {
        socket.close();
    }

    public void setFile_descriptor(int file_descriptor) {
        this.file_descriptor = file_descriptor;
    }

    public Socket getSocket() {
        return socket;
    }

    public int getFile_descriptor() {
        return file_descriptor;
    }

    @Override
    public void run() {
        while (!socket.isClosed()) {
            try {
                writer = new PrintWriter(socket.getOutputStream());
                reader = new BufferedInputStream(socket.getInputStream());
                //On attend la demande du client
                String response = read();
                long start = System.currentTimeMillis();
                response = response.substring(0,response.length()-1);
                InetSocketAddress remote = (InetSocketAddress) socket.getRemoteSocketAddress();

                String toSend;
                requests type = getTypeRequest(response);
                switch (type){
                    case INTREST:
                        toSend = answer_intrest(response);
                        break;
                    case GETPIECES:
                        toSend = answer_getpieces(response);
                        break;
                    case HAVE:
                        toSend = answer_have(response);
                        break;
                    default:
                        toSend = "unknown_request";
                        break;
                }



                //On envoie la réponse au client
                int l = toSend.length();
                writer.write(toSend);
                long end = System.currentTimeMillis();
                if (end-start == 0) {
                    System.out.println("ascending rate : 99999999 bytes/sec");
                } else
                if (type == requests.GETPIECES) System.out.println("ascending rate : " + (1000*l) / (end-start) + "bytes/sec");
                //Il FAUT IMPERATIVEMENT UTILISER flush()
                //Sinon les données ne seront pas transmises au client
                //et il attendra indéfiniment
                writer.flush();


            } catch (IOException e) {
                System.err.println("LA CONNEXION A ETE INTERROMPUE ! ");
                return;
            } catch (StringIndexOutOfBoundsException e) {
                System.err.println("LA CONNEXION A ETE INTERROMPUE ! ");
                return;
            }
        }
    }

    private requests getTypeRequest(String s){
        if (s.startsWith("interested")) return requests.INTREST;
        if (s.startsWith("getpieces")) return requests.GETPIECES;
        if (s.startsWith("have")) return requests.HAVE;
        return requests.NONE;
    }

    private String read() throws IOException,StringIndexOutOfBoundsException{
        String response = "";
        int stream;
        byte[] b = new byte[4096];
        stream = reader.read(b);
        response = new String(b, 0, stream);
        return response;
    }






    private String answer_intrest(String s){
        String key = s.substring(11);
        Tracker_File f = myself.searchFile(key);
        return "have " + key + " " + f.getBuffermap() + "\n";
    }

    private String answer_getpieces(String s){
        int i = 10;
        while(!s.substring(i,i+1).startsWith("[")){
            i++;
        }
        String key = s.substring(10,i-1);
        String response = "data " + key +  " [";
        i++;

        Tracker_File f = myself.searchFile(key);

        List<Integer> piecestoGet = new ArrayList();
        while(!s.substring(i,i+1).matches("]")) {
            int j = i;
            while (!s.substring(j, j + 1).matches("]") && !s.substring(j, j + 1).matches(" ")) {
                j++;
            }
            String code = s.substring(i,j);
            i = j;
            if (s.substring(i,i+1).matches(" ")) i++;


            response += code + ":" + new String(f.getPiece(Integer.parseInt(code))) + " ";
        }



        response = response.substring(0,response.length()-1) + "]\n";
        return response;
    }

    private int b2bsum(int n){
        int res = 0;
        while (n != 0){
            res += n%2;
            n /= 2;
        }
        return res;
    }

    private String answer_have(String s) {
        int i = 5;
        while(!s.substring(i,i+1).matches(" ")){
            i++;
        }
        String key = s.substring(5,i);
        return "have " + key + " " + myself.searchFile(key).getBuffermap() + "\n";
    }


    @Override
    public int compareTo(Clients o) {
        return  b2bsum(file_descriptor) -b2bsum(o.file_descriptor);
    }
}
