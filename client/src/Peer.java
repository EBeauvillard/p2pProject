import org.ini4j.Ini;

import java.awt.event.ActionEvent;
import java.io.*;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.stream.Stream;

public class Peer {

    int NbConnexionLeft = 5;
    private List<Tracker_File> files = new ArrayList<>();
    int Piece_Size = 2048;
    private Socket tracker_socket;
    private ServerSocket receive_socket;
    private List<Clients> clients = new ArrayList<Clients>();
    private List<Clients> AwaitingClients = new ArrayList<Clients>();
    private boolean isRunning = true;
    private Peer myself = this;
    private static String inipath = "../../server/build/config.ini";
    private int MessageSize = 16000;

    public InetAddress getAdress(){
        return receive_socket.getInetAddress();
    }

    public int getPort(){
        return receive_socket.getLocalPort();
    }

    public Peer(String filepath) {
        Ini ini = null;
        try {
            ini = new Ini(new File(inipath));
        } catch (IOException e) {
            e.printStackTrace();
        }
        int port = Integer.parseInt(ini.get("database", "portserver"));
        InetAddress adress = null;
        try {
            adress = InetAddress.getByName(ini.get("database", "server"));
        } catch (UnknownHostException e) {
            e.printStackTrace();
        }
        Piece_Size = Integer.parseInt(ini.get("database", "piecesize"));
        NbConnexionLeft = Integer.parseInt(ini.get("database", "nb_clients_max"));
        MessageSize = Integer.parseInt(ini.get("database", "messagesize"));
        try {
            this.tracker_socket = new Socket(adress, port);
        } catch (IOException e) {
            e.printStackTrace();
        }
        initialize_file(filepath);
        try {
            this.receive_socket = new ServerSocket(Integer.parseInt(ini.get("database", "portclient")));
        } catch (IOException e) {
            e.printStackTrace();
        }
        this.openConection();
        System.out.println("New user created. adress:" + tracker_socket.getLocalAddress() + " and port :" + tracker_socket.getLocalPort());
    }

    public void CloseConection(){
        isRunning = false;
        try {
            tracker_socket.close();
            receive_socket.close();
            isRunning = false;
            for (int i = 0; i < clients.size(); i++){
                clients.get(i).Disconnect();
            }
            System.out.println("Client has been successfully shut down!");

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public Peer(String filepath, int portr) {
        Ini ini = null;
        try {
            ini = new Ini(new File(inipath));
        } catch (IOException e) {
            e.printStackTrace();
        }
        int port = Integer.parseInt(ini.get("database", "portserver"));
        InetAddress adress = null;
        try {
            adress = InetAddress.getByName(ini.get("database", "server"));
        } catch (UnknownHostException e) {
            e.printStackTrace();
        }
        try {
            this.tracker_socket = new Socket(adress, port);
        } catch (IOException e) {
            e.printStackTrace();
        }
        initialize_file(filepath);
        try {
            this.receive_socket = new ServerSocket(portr);
        } catch (IOException e) {
            e.printStackTrace();
        }
        this.openConection();
        System.out.println("New user created. adress:" + tracker_socket.getLocalAddress() + " and port :" + tracker_socket.getLocalPort());
    }

    public Peer(InetAddress adress, int port, String filepath) throws IOException {
        this.tracker_socket = new Socket(adress, receive_socket.getLocalPort());
        initialize_file(filepath);
        try {
            this.receive_socket = new ServerSocket(80);
        } catch (IOException e) {
            System.err.println("Le port 80 est déjà utilisé ! ");
        }
        this.openConection();
    }

    private void initialize_file(String filepath){
        try {
            Path path = Paths.get(filepath);
            Stream<Path> walk = Files.walk(path);
            walk.forEach(x -> files.add(new Tracker_File(x.toString(),Piece_Size)));
            files.removeIf(f -> f.isDirectory());
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void openConection() {

        //Toujours dans un thread à part vu qu'il est dans une boucle infinie
        Thread t = new Thread(new Runnable() {
            public void run() {
                System.out.println("My conection is open!");
                while (isRunning == true) {

                    try {
                        //On attend une connexion d'un client
                        Socket client = receive_socket.accept();

                        //Une fois reçue, on la traite dans un thread séparé
                        System.out.println("Connexion cliente reçue.");
                        Thread t = new Thread(new Clients(client, myself));
                        t.start();

                    } catch (IOException e) {
                        System.out.println();
                    }
                }

                System.out.println("thread closed :|");
            }
        });

        t.start();
    }

    private String sendRequestToTracker(String s){
        OutputStream out = null;
        try {
            out = tracker_socket.getOutputStream();
        } catch (IOException e) {
            e.printStackTrace();
        }
        PrintWriter writer = new PrintWriter(out, true);
        writer.println(s);
        writer.flush();
        InputStream input = null;
        try {
            input = tracker_socket.getInputStream();
        } catch (IOException e) {
            e.printStackTrace();
        }
        BufferedReader reader = new BufferedReader(new InputStreamReader(input));
        String line = null;
        try {
            line = reader.readLine();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return line;
    }


    private List<String> sendRequestToAllClients(String s){
        List<String> result = new ArrayList<>();
        int i = 0;
        while (i < this.clients.size()) {
            Socket sock = clients.get(i).getSocket();
            OutputStream out = null;
            try {
                out = sock.getOutputStream();
            } catch (IOException e) {
                Clients c = AwaitingClients.get(0);
                AwaitingClients.remove(0);
                AwaitingClients.add(clients.get(i));
                clients.remove(i);
                clients.add(c);
                continue;
            }
            PrintWriter writer = new PrintWriter(out, true);
            writer.println(s);
            InputStream input = null;
            try {
                input = sock.getInputStream();
            } catch (IOException e) {
                Clients c = AwaitingClients.get(0);
                AwaitingClients.remove(0);
                AwaitingClients.add(clients.get(i));
                clients.remove(i);
                clients.add(c);
                continue;
            }
            BufferedReader reader = new BufferedReader(new InputStreamReader(input));
            String line = null;
            try {
                line = reader.readLine();
            } catch (IOException e) {
                Clients c = AwaitingClients.get(0);
                AwaitingClients.remove(0);
                AwaitingClients.add(clients.get(i));
                clients.remove(i);
                clients.add(c);
                continue;
            }
            result.add(line);
            i++;
        }
        return result;
    }

    private String sendRequestToAClient(String s, Clients c) throws IOException {
        OutputStream out = null;
        out = c.getSocket().getOutputStream();
        PrintWriter writer = new PrintWriter(out, true);
        writer.println(s);
        writer.flush();
        InputStream input = null;
        input = c.getSocket().getInputStream();
        BufferedReader reader = new BufferedReader(new InputStreamReader(input));
        String line = null;
        line = reader.readLine();
        return line;
    }

    public void announce(){
        String request = "announce listen ";
        request += (String.valueOf(receive_socket.getLocalPort()));
        request += (" seed [");
        for (int i = 0; i < files.size(); i++) {
            Tracker_File f = files.get(i);
            request += (f.getName() + " "
                    + String.valueOf(f.length()) + " "
                    + String.valueOf(f.getPieceSize()) + " "
                    + f.getKey());
            /* Ajout d'espace entre les différents fichiers demanders sans en
             * ajouter après le dernier
             */
            if (i != files.size() - 1) {
              request += " ";
            }
        }
        request += ("]");
        String answer = null;
        answer = sendRequestToTracker(request);
    }

    public List<Tracker_File> look(List<String> criterias){
        String request = "look [";
        for (int i = 0; i < criterias.size(); i++) {
            if (i != 0){
                request += " ";
            }
            request += (criterias.get(i) + "");
        }
        request += ("]");

        String result = null;
        result = sendRequestToTracker(request);

        List<Tracker_File> retfiles = new ArrayList<Tracker_File>();
        int i = 5;
        while ((i < (result.length() - 2)) && (result.substring(i, i + 1) != "]" )) {
            i++;
            int beg = i;
            while (!result.substring(i, i + 1).matches(" ")) {
                i++;
            }
            String name = result.substring(beg, i);
            Tracker_File f = new Tracker_File(name,Piece_Size);
            i++;
            beg = i;
            while (!result.substring(i, i + 1).matches(" ")) {
                i++;
            }
            int length = Integer.parseInt(result.substring(beg, i));
            f.setLength(length);

            i++;
            beg = i;
            while (!result.substring(i, i + 1).matches(" ")) {
                i++;
            }
            f.setPieceSize(Integer.parseInt(result.substring(beg, i)));

            i++;
            beg = i;
            while (!result.substring(i, i + 1).matches(" ") && !result.substring(i, i + 1).matches("]")) {
                i++;
            }
            f.setKey(result.substring(beg, i));
            f.setNbPiece(f.getLength()/f.getPieceSize() + 1);
            files.add(f);
            retfiles.add(f);
        }
        return retfiles;
    }

    public void getFile(String key){
        String request = "getfile " + key;

        String result = null;
        result = sendRequestToTracker(request);

        int i = 0;
        while (!result.substring(i, i + 1).startsWith("[")) {
            i++;
        }
        i++;
        if (!result.substring(i, i + 1).matches("]"))
            i--;
        while (!result.substring(i, i + 1).matches("]")) {
            i++;
            int beg = i;
            while (!result.substring(i, i + 1).matches(":")){
                i++;
            }
            InetAddress address = null;
            try {
                address = InetAddress.getByName(result.substring(beg, i));
            } catch (UnknownHostException e) {
                e.printStackTrace();
            }
            i++;
            beg = i;
            while (!result.substring(i, i + 1).matches(" ") && !result.substring(i, i + 1).matches("]")) {
                i++;
            }
            int port = Integer.parseInt(result.substring(beg, i));
            Clients c = new Clients(address, port, myself);
            if (NbConnexionLeft != 0){
                try {
                    c.Connect();
                    clients.add(c);
                    NbConnexionLeft -= 1;
                } catch (IOException e) {
                    e.printStackTrace();
                    AwaitingClients.add(c);
                }
            }else {
                AwaitingClients.add(c);
            }
        }
    }

    public void AddClient(InetAddress address, int port){
        clients.add(new Clients(address, port, myself));
    }
    public int getPieces(String Key, List<Integer> arguments) throws IOException {
        return getPieces(Key,arguments,clients.get(0),searchFile(Key));
    }
        //WIP : Rajouter la gestion de la section rouge du sujet.
    public int getPieces(String Key, List<Integer> arguments, Clients client, Tracker_File f) throws IOException {
        int res = 0;
        String request = "getpieces "+Key+ " [";
        for (int i = 0; i < arguments.size(); i++) {
            request += (arguments.get(i) + " ");
        }
        request = request.substring(0,request.length()-1) + "]";
        String result = sendRequestToAClient(request, client);

        List<Piece> pieces = new ArrayList<>();
        int i = 0;
        while (!result.substring(i, i + 1).startsWith("[")) {
            i++;
        }
        while (!result.substring(i, i + 1).matches("]")) {
            i++;
            int beg = i;
            while (!result.substring(i, i + 1).matches(":")) {
                i++;
            }
            int index = Integer.parseInt(result.substring(beg, i));
            i++;
            beg = i;
            while ((i-beg)<Piece_Size && !result.substring(i, i + 1).matches("]")) {
                i++;
            }
            byte[] content = (result.substring(beg, i)).getBytes();
            f.Add_Piece(index,content);
            res += (i-beg);
        }
        return res;
    }

    public void send_interest(String key){
        List<String> result = sendRequestToAllClients("interested " + key);

        for (int j = 0; j < result.size(); j++) {

            int i = 5;
            String analysed = result.get(j);
            while (!analysed.substring(i, i + 1).matches(" ")) {
                i++;
            }
            while (!analysed.substring(i, i + 1).matches(" ")) {
                i++;
            }
            i++;

            clients.get(j).setFile_descriptor(Integer.parseInt(analysed.substring(i)));
        }
    }

    private void received_interested(ActionEvent e) {
        String Answer = "have " + e;
    }

    public void update(String k){
        update(k,clients.get(0));
    }

    //WIP: Je ne sais pas si je dois stocker les clés de seed et de leech dans des listes.
    public void update(String k, Clients c){
        String request = "have " + k + " " + searchFile(k).getBuffermap();
        List<String> reponse = sendRequestToAllClients(request);
        for (int i = 0; i < reponse.size(); i++) {
            int compt = 5;
            String r = reponse.get(i);
            while (!r.substring(compt, compt + 1).matches(" ")) {
                compt++;
            }
            clients.get(i).setFile_descriptor(Integer.parseInt(r.substring(compt + 1)));
        }

    }

    public void update_tracker(List<String> seeds, List<String> leech){
        String request = "update seed [";
        if (seeds.isEmpty()) request += "] leech ["; else {
            for (int i = 0; i < seeds.size(); i++) {
                request += seeds.get(i);
            }
            request = request.substring(0, request.length() - 1) + "] leech [";
        }
        if (leech.isEmpty()) request += "]"; else {
            for (int i = 0; i < leech.size(); i++) {
                request += leech.get(i);
            }
            request = request.substring(0, request.length() - 1) + "]";
        }
        sendRequestToTracker(request);
    }

    Tracker_File searchFile(String key) {
        for (int i = 0; i < files.size(); i++) {
            if (files.get(i).matchesKey(key)) return files.get(i);
        }
        System.out.println("hoho, je renvoie un fichier error");
        return (new Tracker_File("ERROR",0));

    }







    private int getocc(int index){
        int res = 0;
        for (int i = 0; i < clients.size(); i++){
            int a = (clients.get(i).getFile_descriptor() / (int) Math.pow(2,index))%2;
            if (a == 1){
                res += (int)Math.pow(2,i);
            }
        }
        return res;
    }


    private int b2bsum(int n){
        int res = 0;
        while (n != 0){
            res += n%2;
            n /= 2;
        }
        return res;
    }




    public void Download(String Key){
        Tracker_File f = searchFile(Key);
        boolean disconection = true;
        List<Piece> Pieces = new ArrayList<>();
        Pieces.add(new Piece(0,0,0,0));
        while(!Pieces.isEmpty()) {
            long startg = System.currentTimeMillis();
            int downloaded = 0;

            if (disconection) {
                getFile(Key);
                Pieces = new ArrayList<>();
                send_interest(Key);
                for (int i = 0; i < f.getNbPiece(); i++) {
                    if ((f.getBuffermap() / (int) Math.pow(2,i))%2 == 0) {
                        int occ = getocc(i);
                        Pieces.add(new Piece(i, f.getPieceSize(), b2bsum(occ), occ));
                    }
                }

                Collections.sort(clients);
                Collections.sort(Pieces);
                disconection = false;
            }
            int nbOfPiece = (int)f.getNbPiece();
            int nbOfPiecepermess = MessageSize / Piece_Size;

            List<List<Piece>> Listrequest = new ArrayList<>();
            for (int i = 0; i <clients.size(); i++){
                List<Piece> l = new ArrayList<>();
                Listrequest.add(l);
            }


            for (int i = 0; i < Pieces.size(); i++) {
                Piece p = Pieces.get(i);
                if (p.getNbOccur() != 0){
                    boolean tostay = true;
                    int j = 0;
                    while (j < clients.size() && tostay){
                        if ((p.getBytemap()/ (int) Math.pow(2,j))%2 == 1 && Listrequest.get(j).size() < nbOfPiecepermess ){
                            Listrequest.get(j).add(p);
                            tostay = false;
                        }
                        j++;
                    }
                }
            }


            for (int  i = 0 ; i < clients.size(); i++){
                try {
                    long start = System.currentTimeMillis();
                    List<Integer> toSend = new ArrayList<>();
                    for (int j = 0; j < Listrequest.get(i).size(); j++){
                        toSend.add((Listrequest.get(i).get(j)).getIndex());
                    }
                    int deb = 0;
                    if (!Listrequest.get(i).isEmpty()) {
                        deb = getPieces(Key, toSend, clients.get(i), f);
                        downloaded += deb;
                    }
                    for (int j = 0; j < Listrequest.get(i).size(); j++){
                        Pieces.remove(Listrequest.get(i).get(j));
                    }
                    long end = System.currentTimeMillis();
                    if (deb !=0)
                        System.out.println("Exchanged with client " + clients.get(i).socket.getInetAddress() + ":" + clients.get(i).socket.getPort() + "at speed " + (1000*deb)/(end-start) + "by/s");
                } catch (IOException e) {
                    disconection = true;
                }
            }
            long endg = System.currentTimeMillis();
            System.out.println("Current progress fo download of " + f.getName() + ": " + 100*(Integer.bitCount(f.getBuffermap()))/nbOfPiece + "% :" + (Integer.bitCount(f.getBuffermap())) + " Pieces out of " + nbOfPiece);
            System.out.println("Global download rate : " + 1000*downloaded/(endg-startg) + "bytes/sec");

        }
    }


}
