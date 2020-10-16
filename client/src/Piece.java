public class Piece implements Comparable<Piece>{

    private int Index;
    private int PieceSize;
    private int nbOccur;
    private int bytemap;

    public Piece(int index, int pieceSize, int nbocc, int bm) {
        Index = index;
        PieceSize = pieceSize;
        nbOccur = nbocc;
        bytemap = bm;
    }

    public int getPieceSize() {
      return PieceSize;
    }

    public int getIndex() {
      return Index;
    }

    public void setIndex(int index) {
      Index = index;
    }

    public void setPieceSize(int pieceSize) {
      PieceSize = pieceSize;
    }

    public int getBytemap() {
        return bytemap;
    }

    public int getNbOccur() {
        return nbOccur;
    }

    public void setNbOccur(int nbOccur) {
        this.nbOccur = nbOccur;
    }

    @Override
    public int compareTo(Piece o) {
        return this.nbOccur - o.nbOccur;
    }
}
