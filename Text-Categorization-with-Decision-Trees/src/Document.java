import java.util.ArrayList;

public class Document {

    private int newsgroup;
    private ArrayList<Integer> wordInDoc;

    public Document(int newsgroup) {
        this.newsgroup = newsgroup;
        wordInDoc = new ArrayList<>();
        for (int i = 0; i < 3566; i++) {
            wordInDoc.add(0);
        }
    }

    public int getNewsgroup() {
        return newsgroup;
    }

    public void setNewsgroup(int newsgroup) {
        this.newsgroup = newsgroup;
    }

    public ArrayList<Integer> getWordInDoc() {
        return wordInDoc;
    }

    public void setWordInDoc(ArrayList<Integer> wordInDoc) {
        this.wordInDoc = wordInDoc;
    }
}
