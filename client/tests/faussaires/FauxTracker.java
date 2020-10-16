package faussaires;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;


enum REQUESTS{
    ANNOUNCE,
    LOOK,
    GETFILE,
    ERROR;
}

public class FauxTracker {
    private ServerSocket server;

    FauxTracker(int port, String host) {
        try {
            server = new ServerSocket(port, 100, InetAddress.getByName(host));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


    public void open() {

        //Toujours dans un thread à part vu qu'il est dans une boucle infinie
        Thread t = new Thread(new Runnable() {
            public void run() {
                while (true == true) {

                    try {
                        //On attend une connexion d'un client
                        Socket client = server.accept();

                        //Une fois reçue, on la traite dans un thread séparé
                        System.out.println("Connexion cliente reçue.");
                        Thread t = new Thread(new Runnable() {
                            @Override
                            public void run() {
                                todo(client);
                            }
                        });
                        t.start();

                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        });

        t.start();
    }

    private void todo(Socket sock) {
        while (!sock.isClosed()) {

            try {

                //Ici, nous n'utilisons pas les mêmes objets que précédemment
                //Je vous expliquerai pourquoi ensuite
                PrintWriter writer = new PrintWriter(sock.getOutputStream());
                BufferedInputStream reader = new BufferedInputStream(sock.getInputStream());

                //On attend la demande du client
                String response = read(reader);
                InetSocketAddress remote = (InetSocketAddress) sock.getRemoteSocketAddress();

                //On affiche quelques infos, pour le débuggage
                String debug = "";
                debug = "Thread : " + Thread.currentThread().getName() + ". ";
                debug += "Demande de l'adresse : " + remote.getAddress().getHostAddress() + ".";
                debug += " Sur le port : " + remote.getPort() + ".\n";
                debug += "\t -> Commande reçue : " + response + "\n";
                System.err.println("\n" + debug);

                //On traite la demande du client en fonction de la commande envoyée
                String toSend = "";

                switch (getrequest(response)) {
                    case ANNOUNCE:
                        System.out.println("ANNOUNCE request received");
                        toSend = "> ok";
                        break;
                    case LOOK:
                        System.out.println("LOOK request received");
                        toSend = "> list [file_a.dat 2097152 1024 8905e92afeb80fc7722ec89eb0bf0966]";
                        break;
                    case GETFILE:
                        System.out.println("LOOK request received");
                        toSend = "> peers 8905e92afeb80fc7722ec89eb0bf0966 [1.1.1.2:2222 1.1.1.3:3333]";
                        break;
                    default:
                        System.out.println("WRONG request received");
                        toSend = "Commande inconnu !";
                }
                writer.write(toSend);
                writer.flush();
            } catch (IOException e) {
                e.printStackTrace();
            }

        }

    }

    private String read(BufferedInputStream reader) throws IOException {
        String response = "";
        int stream;
        byte[] b = new byte[4096];
        stream = reader.read(b);
        response = new String(b, 0, stream);
        return response;
    }

    private REQUESTS getrequest(String s){
        if (s.matches("announce")) return REQUESTS.ANNOUNCE;
        if (s.matches("look")) return REQUESTS.LOOK;
        if (s.matches("getfile")) return REQUESTS.GETFILE;
        return REQUESTS.ERROR;
    }
}