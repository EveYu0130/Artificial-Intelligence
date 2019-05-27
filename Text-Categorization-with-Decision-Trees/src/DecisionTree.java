import javafx.util.Pair;

import java.util.ArrayList;

public class DecisionTree {

    private int prediction;
    private Pair<Integer, Double> wordIG;
    private DecisionTree contain;
    private DecisionTree notContain;

    public DecisionTree(int prediction, Pair<Integer, Double> wordIG) {
        this.prediction = prediction;
        this.wordIG = wordIG;
    }

    public int getPrediction() {
        return prediction;
    }

    public void setPrediction(int prediction) {
        this.prediction = prediction;
    }

    public DecisionTree getContain() {
        return contain;
    }

    public void setContain(DecisionTree contain) {
        this.contain = contain;
    }

    public DecisionTree getNotContain() {
        return notContain;
    }

    public void setNotContain(DecisionTree notContain) {
        this.notContain = notContain;
    }

    public Pair<Integer, Double> getWordIG() {
        return wordIG;
    }

    public void setWordIG(Pair<Integer, Double> wordIG) {
        this.wordIG = wordIG;
    }

    public void print(ArrayList<String> words, String prefix, Boolean isLeaf) {
        System.out.println(prefix + (isLeaf ? "└── " : "├── ") + prediction + "; " + words.get(wordIG.getKey()) +  "; IG: " + wordIG.getValue());
        if (contain != null) {
            contain.print(words, prefix + (isLeaf ? "    " : "│   "), false);
        }
        if (notContain != null) {
            notContain.print(words, prefix + (isLeaf ?"    " : "│   "), true);
        }

    }
}

