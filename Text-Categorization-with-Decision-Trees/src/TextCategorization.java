import java.io.*;
import java.util.ArrayList;

public class TextCategorization {

    public static void main(String[] args) {

        ArrayList<Document> trainDocuments = new ArrayList<>();

        // Reads train label inputs
        InputStreamReader trainLabelInput = new InputStreamReader(TextCategorization.class.getResourceAsStream("trainLabel.txt"));
        try {

            BufferedReader trainLabelReader = new BufferedReader(trainLabelInput);
            String line ;
            while ((line = trainLabelReader.readLine()) != null) {
                trainDocuments.add(new Document(Integer.parseInt(line)));
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        // Reads train data inputs
        try {
            InputStreamReader trainDataInput = new InputStreamReader(TextCategorization.class.getResourceAsStream("trainData.txt"));
            BufferedReader trainLabelReader = new BufferedReader(trainDataInput);
            String line;
            while ((line = trainLabelReader.readLine()) != null) {
                String[] words = line.split("\t");
                int docId = Integer.parseInt(words[0]);
                int wordId = Integer.parseInt(words[1]);
                trainDocuments.get(docId-1).getWordInDoc().set(wordId-1, 1);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        // Reads words inputs
        ArrayList<String> words = new ArrayList<>();
        try {
            InputStreamReader wordsInput = new InputStreamReader(TextCategorization.class.getResourceAsStream("words.txt"));
            BufferedReader wordsReader = new BufferedReader(wordsInput);
            String line;
            while ((line = wordsReader.readLine()) != null) {
                words.add(line);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        ArrayList<Document> testDocuments = new ArrayList<>();

        // Reads test label inputs
        try {
            InputStreamReader testLabelInput = new InputStreamReader(TextCategorization.class.getResourceAsStream("testLabel.txt"));
            BufferedReader testLabelReader = new BufferedReader(testLabelInput);
            String line;
            while ((line = testLabelReader.readLine()) != null) {
                testDocuments.add(new Document(Integer.parseInt(line)));
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        // Reads test data inputs
        try {
            InputStreamReader testDataInput = new InputStreamReader(TextCategorization.class.getResourceAsStream("testData.txt"));
            BufferedReader testDataReader = new BufferedReader(testDataInput);
            String line;
            while ((line = testDataReader.readLine()) != null) {
                String[] values = line.split("\t");
                int docId = Integer.parseInt(values[0]);
                int wordId = Integer.parseInt(values[1]);
                testDocuments.get(docId-1).getWordInDoc().set(wordId-1, 1);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        DTL dtl = new DTL(10);
        DecisionTree decisionTree = dtl.decisionTreeLearner(trainDocuments);
        decisionTree.print(words," ", true);

        return;
    }
}
