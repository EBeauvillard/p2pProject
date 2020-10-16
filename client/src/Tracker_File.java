import java.io.*;
import java.lang.reflect.Array;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Arrays;
import java.util.List;

public class Tracker_File extends File {
    private int PieceSize = 2048;
    private String Key;
    private int buffermap = 0;
    private List<Piece> content;
    private int Length;
    private long nbPiece;


    public int getLength() {
        return Length;
    }

    public void setLength(int length) {
        Length = length;
    }

    void updateBufferMap(Integer i){
        if ((buffermap/ (int) Math.pow(2,i) %2) == 0) {
            buffermap += (int) Math.pow(2,i);
        }
    }

    private static final char[] HEX_ARRAY = "0123456789ABCDEF".toCharArray();
    public static String bytesToHex(byte[] bytes) {
        char[] hexChars = new char[bytes.length * 2];
        for (int j = 0; j < bytes.length; j++) {
            int v = bytes[j] & 0xFF;
            hexChars[j * 2] = HEX_ARRAY[v >>> 4];
            hexChars[j * 2 + 1] = HEX_ARRAY[v & 0x0F];
        }
        return new String(hexChars);
    }

    Tracker_File(String n, int ps){
        super(n);
        if (!this.exists()) {
            try {
                this.createNewFile();
            } catch (IOException e) {
                e.printStackTrace();
            }
        } else {
            byte[] b = new byte[0];
            if (!this.isDirectory()) {
                try {
                    b = Files.readAllBytes(Paths.get(this.getAbsolutePath()));
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }




            ProcessBuilder processBuilder = new ProcessBuilder();
            processBuilder.command("md5sum", this.getAbsolutePath());

            try {

                Process process = processBuilder.start();

                StringBuilder output = new StringBuilder();

                BufferedReader reader = new BufferedReader(
                        new InputStreamReader(process.getInputStream()));

                String line;
                while ((line = reader.readLine()) != null) {
                    output.append(line + "");
                }

                int exitVal = process.waitFor();
                Key = String.valueOf(output);
                int i = 0;
                while(i < (Key.length()-1) && !Key.substring(i,i+1).matches(" ")){
                    i++;
                }
                Key = Key.substring(0,i);

            } catch (IOException e) {
                e.printStackTrace();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            PieceSize = ps;

            nbPiece = (this.length()/ this.PieceSize) + 1;
            buffermap = (int) Math.pow(2,nbPiece) - 1;
            if (nbPiece == 0) buffermap = 0;

        }
    }

    public long getNbPiece() {
        return nbPiece;
    }

    public void setNbPiece(int nbPiece) {
        this.nbPiece = nbPiece;
    }

    Tracker_File(String n, String k) {
        super(n);
        Key = k;
    }

    private int WhereToWrite(int n){
        int res = 0;
        for (int i=0; i< n; i++){
            res += PieceSize * ((buffermap>>i) & 1);
        }
        return res;
    }

    private byte[] concatenate(byte[] a, byte[] b) {
        int aLen = a.length;
        int bLen = b.length;

        @SuppressWarnings("unchecked")
        byte[] c = (byte[]) Array.newInstance(a.getClass().getComponentType(), aLen + bLen);
        System.arraycopy(a, 0, c, 0, aLen);
        System.arraycopy(b, 0, c, aLen, bLen);

        return c;
    }

    public void Add_Piece (int n, byte[] bytes){
        int where_to_write = WhereToWrite(n);
        try {
            byte[] filecontent = Files.readAllBytes(this.toPath());
            byte[] ToReplaceBy =
                    concatenate(Arrays.copyOfRange(filecontent,0,where_to_write),
                            concatenate(bytes,Arrays.copyOfRange(filecontent,where_to_write,filecontent.length)));
            FileOutputStream stream = new FileOutputStream(String.valueOf(this.getAbsoluteFile()));
            stream.write(ToReplaceBy);
        }catch (IOException e) {
            e.printStackTrace();
        }
        updateBufferMap(n);
    }


    public void setPieceSize(int pieceSize) {
        PieceSize = pieceSize;
    }

    public void setKey(String key) {
        Key = key;
    }

    public int getPieceSize() {
        return PieceSize;
    }

    public String getKey() {
        return Key;
    }

    public int getBuffermap() {
        return buffermap;
    }

    public void setBuffermap(int buffermap) {
        this.buffermap = buffermap;
    }

    boolean matchesKey(String k){
        return Key.matches(k);
    }

    byte[] getPiece (int i){
        byte[] filecontent = new byte[0];
        try {
            filecontent = Files.readAllBytes(this.toPath());
        }catch (IOException e) {
            e.printStackTrace();
        }
        return Arrays.copyOfRange(filecontent,i*getPieceSize(),(i+1)*getPieceSize());
    }


}
