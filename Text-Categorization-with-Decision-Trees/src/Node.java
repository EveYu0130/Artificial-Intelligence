import javafx.util.Pair;

import java.util.ArrayList;

public class Node {

    private DecisionTree decisionTree;
    private ArrayList<Document> documents;

    public Node(DecisionTree decisionTree, ArrayList<Document> documents) {
        this.decisionTree = decisionTree;
        this.documents = documents;

    }

    public DecisionTree getDecisionTree() {
        return decisionTree;
    }

    public void setDecisionTree(DecisionTree decisionTree) {
        this.decisionTree = decisionTree;
    }

    public ArrayList<Document> getDocuments() {
        return documents;
    }

    public void setDocuments(ArrayList<Document> documents) {
        this.documents = documents;
    }
}
