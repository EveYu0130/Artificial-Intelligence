import javafx.util.Pair;
import java.util.ArrayList;
import java.util.Collections;
import java.util.PriorityQueue;

public class DTL {

    private int treeDepthLimit;

    public DTL(int treeDepthLimit) { this.treeDepthLimit = treeDepthLimit;}

    public DecisionTree decisionTreeLearner(ArrayList<Document> documents) {
        PriorityQueue<Node> pq = new PriorityQueue<>(treeDepthLimit, new NodeComparator());
        int prediction = pointEstimate(documents);
        Pair<Integer, Double> wordIG = selectFeature(documents);
        DecisionTree decisionTree = new DecisionTree(prediction, wordIG);
        pq.add(new Node(decisionTree, documents));

        for (int i = 0; i < treeDepthLimit; i++) {

            Node bestLeaf = pq.poll();
            DecisionTree dt = bestLeaf.getDecisionTree();

            if (!bestLeaf.getDocuments().isEmpty()) {

                ArrayList<Document> wordsInDoc = new ArrayList<>();
                ArrayList<Document> wordsNotInDoc = new ArrayList<>();
                for (Document document : bestLeaf.getDocuments()) {
                    if (document.getWordInDoc().get(dt.getWordIG().getKey()) == 1) {
                        wordsInDoc.add(document);
                    } else {
                        wordsNotInDoc.add(document);
                    }
                }
                if (!stopping(wordsInDoc)) {
                    int predictionIn = pointEstimate(wordsInDoc);
                    Pair<Integer, Double> wordIGIn = selectFeature(wordsInDoc);
                    DecisionTree childDecisionTreeIn = new DecisionTree(predictionIn, wordIGIn);
                    dt.setContain(childDecisionTreeIn);
                    pq.add(new Node(childDecisionTreeIn, wordsInDoc));

                }

                if (!stopping(wordsNotInDoc)) {

                    int predictionNotIn = pointEstimate(wordsNotInDoc);
                    Pair<Integer, Double> wordIGNotIn = selectFeature(wordsNotInDoc);
                    DecisionTree childDecisionTreeNotIn = new DecisionTree(predictionNotIn, wordIGNotIn);
                    dt.setNotContain(childDecisionTreeNotIn);
                    pq.add(new Node(childDecisionTreeNotIn, wordsNotInDoc));
                }


            }
        }
        return decisionTree;

    }

    private int pointEstimate(ArrayList<Document> documents) {

        int length = documents.size();

        if (length == 0) {
            return 1;
        }

        ArrayList<Integer> newsgroups = new ArrayList<>();

        for (Document document : documents) {
            newsgroups.add(document.getNewsgroup());
        }

        Collections.sort(newsgroups);
        return newsgroups.get(length/2);
    }

    private boolean stopping(ArrayList<Document> documents) {
        int N1 = 0;
        int N2 = 0;

        for (Document document : documents) {
            if (document.getNewsgroup() == 1) {
                N1++;
            } else {
                N2++;
            }
        }

        return (N1 == 0 || N2 == 0);
    }

    private double informationContent(ArrayList<Document> documents) {
        int N1 = 0;
        int N2 = 0;
        double logN1 = 0.0;
        double logN2 = 0.0;

        if (documents.isEmpty()) {
            return 1d;
        }

        for (Document document : documents) {
            if (document.getNewsgroup() == 1) {
                N1+=1;
            } else {
                N2+=1;
            }
        }


        double P1 = (double) N1 / (double) documents.size();
        double P2 = (double) N2 / (double) documents.size();

        if (N1 != 0) {
            logN1 = Math.log(P1);
        }

        if (N2 != 0) {
            logN2 = Math.log(P2);
        }

        return 0.0 - P1 * logN1 - P2 * logN2;
    }

    private double informationContentSplit(ArrayList<Document> documents, int wordIdx, int method) {
        if (documents.isEmpty()) {
            return 0.0;
        }

        ArrayList<Document> E1 = new ArrayList<>();
        ArrayList<Document> E2 = new ArrayList<>();

        for (Document document : documents) {
            if (document.getWordInDoc().get(wordIdx) == 1) {
                E1.add(document);
            } else {
                E2.add(document);
            }
        }

        double IE1 = informationContent(E1);
        double IE2 = informationContent(E2);
        double N1 = E1.size();
        double N2 = E2.size();
        double N = N1 + N2;

        if (method == 1) {
            return 0.5 * IE1 + 0.5 * IE2;
        } else {
            return (N1/N) * IE1 + (N2/N) * IE2;
        }
    }

    private double informationGain(ArrayList<Document> documents, int wordIdx, int method) {

        return informationContent(documents) - informationContentSplit(documents,wordIdx,method);
    }

    private Pair<Integer, Double> selectFeature(ArrayList<Document> documents) {
        double maxVal = 0.0;
        int wordId = 0;
        for( int i = 0; i < 3566; i++ ) {
            double fValue = informationGain(documents, i, 2);
            if ( fValue > maxVal ) {
                maxVal = fValue;
                wordId = i;
            }
        }
        return new Pair<>(wordId, maxVal);
    }
}
