#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

using namespace std;


struct Patient {
    int Dunetts;
    int TRIMONO;
    int Sloepnea;
    int Foriennditis;
    int Degar;
};

vector<Patient> trainData;
vector<Patient> testData;

pair<Patient, double> probTable[48];

double normalizedProbs[48];

double weightSum[48];

pair<string, double> Dunetts[3];
pair<string, double> TRIMONO[2];
pair<string, double> Sloepnea[6];
pair<string, double> Foriennditis[3];
pair<string, double> Degar[3];


double init_guess[20][20];
double EM_result[20][20];


void readData(string filename) {
    string line;
    ifstream fs;

    fs.open(filename);
    if (fs.is_open()) {
        while (getline(fs, line)) {
            istringstream iss(line);
            int n;

            Patient p;

            iss >> n;
            p.Sloepnea = n;
            iss >> n;
            p.Foriennditis = n;
            iss >> n;
            p.Degar = n;
            iss >> n;
            p.TRIMONO = n;
            iss >> n;
            p.Dunetts = n;

            if (filename == "traindata.txt") {
                trainData.push_back(p);
            } else {
                testData.push_back(p);
            }
        }
    }
}

void initCPTs() {
    Dunetts[0] = make_pair("none", 0.5);
    Dunetts[1] = make_pair("mile", 0.25);
    Dunetts[2] = make_pair("severe", 0.25);

    TRIMONO[0] = make_pair("t", 0.1);
    TRIMONO[1] = make_pair("f", 0.9);

    Degar[0] = make_pair("t,dun_none", 0.1);
    Degar[1] = make_pair("t,dun_mild", 0.2);
    Degar[2] = make_pair("t,dun_severe", 0.8);

    Foriennditis[0] = make_pair("t,dun_none", 0.1);
    Foriennditis[1] = make_pair("t,dun_mild", 0.8);
    Foriennditis[2] = make_pair("t,dun_severe", 0.2);

    Sloepnea[0] = make_pair("t,TRI_t, dun_none", 0.1);
    Sloepnea[1] = make_pair("t,TRI_t, dun_mild", 0.1);
    Sloepnea[2] = make_pair("t,TRI_t, dun_severe", 0.05);
    Sloepnea[3] = make_pair("t,TRI_f, dun_none", 0.1);
    Sloepnea[4] = make_pair("t,TRI_f, dun_mild", 0.8);
    Sloepnea[5] = make_pair("t,TRI_f, dun_severe", 0.9);
}

double getRandom(double noise) {
    return noise * ((double) rand() / (double) RAND_MAX);
}

void addNoise(double noise) {

    double delta1 = getRandom(noise);
    double delta2 = getRandom(noise);
    double delta3 = getRandom(noise);
    Dunetts[0].second = (Dunetts[0].second+delta1) / (1+delta1+delta2+delta3);
    Dunetts[1].second = (Dunetts[1].second+delta2) / (1+delta1+delta2+delta3);
    Dunetts[2].second = (Dunetts[2].second+delta3) / (1+delta1+delta2+delta3);


    delta1 = getRandom(noise);
    delta2 = getRandom(noise);
    TRIMONO[0].second = (TRIMONO[0].second+delta1) / (1+delta1+delta2);
    TRIMONO[1].second = (TRIMONO[1].second+delta2) / (1+delta1+delta2);

    for (int i = 0; i < 3; i++) {
        delta1 = getRandom(noise);
        delta2 = getRandom(noise);
        Foriennditis[i].second = (Foriennditis[i].second+delta1) / (1+delta1+delta2);
    }

    for (int i = 0; i < 3; i++) {
        delta1 = getRandom(noise);
        delta2 = getRandom(noise);
        Degar[i].second = (Degar[i].second+delta1) / (1+delta1+delta2);
    }

    for (int i = 0; i < 6; i++) {
        delta1 = getRandom(noise);
        delta2 = getRandom(noise);
        Sloepnea[i].second = (Degar[i].second+delta1) / (1+delta1+delta2);
    }
}


void updateProbability() {
    double pr;
    double pr_TRIMONO, pr_Sloepnea, pr_Foriennditis, pr_Degar;
    int index;

    for (int tri = 0; tri < 2; tri++) {
        for (int slo = 0; slo < 2; slo++) {
            for (int fori = 0; fori < 2; fori ++) {
                for (int deg = 0; deg < 2; deg++) {
                    index = tri*24 +  slo*12 + fori*6 + deg*3;
                    for (int dun = 0; dun < 3; dun++) {
                        Patient p;

                        if (tri == 1) {
                            pr_TRIMONO = TRIMONO[0].second;
                            if (slo == 1) {
                                pr_Sloepnea = Sloepnea[dun].second;
                            } else {
                                pr_Sloepnea = 1 - Sloepnea[dun].second;
                            }
                        } else {
                            pr_TRIMONO = TRIMONO[1].second;
                            if (slo == 1) {
                                pr_Sloepnea = Sloepnea[dun+3].second;
                            } else {
                                pr_Sloepnea = 1- Sloepnea[dun+3].second;
                            }
                        }

                        if (fori == 1) {
                            pr_Foriennditis = Foriennditis[dun].second;
                        } else {
                            pr_Foriennditis = 1 - Foriennditis[dun].second;
                        }

                        if (deg == 1) {
                            pr_Degar = Degar[dun].second;
                        } else {
                            pr_Degar = 1- Degar[dun].second;
                        }

                        pr = pr_TRIMONO * pr_Sloepnea * pr_Foriennditis * pr_Degar * Dunetts[dun].second;

                        p.TRIMONO = tri;
                        p.Sloepnea = slo;
                        p.Foriennditis = fori;
                        p.Degar = deg;
                        p.Dunetts = dun;
                        pair<Patient, double> entry = make_pair(p, pr);
                        probTable[index] = entry;

                        index++;
                    }

                    double sum = probTable[index-3].second + probTable[index-2].second + probTable[index-1].second;
                    normalizedProbs[index-3] = (sum == 0) ? 0 : probTable[index-3].second/sum;
                    normalizedProbs[index-2] = (sum == 0) ? 0 : probTable[index-2].second/sum;
                    normalizedProbs[index-1] = (sum == 0) ? 0 : probTable[index-1].second/sum;
                }
            }
        }
    }
}

double testAccuracy() {
    double miss = 0.0;
    double hit = 0.0;

    int table[48] = {0};

    int i = 0;
    while (i < 48) {
        double maximum = max(max(normalizedProbs[i], normalizedProbs[i+1]), normalizedProbs[i+2]);
        if (normalizedProbs[i] == maximum) {
            table[i] = 1;
        } else if (normalizedProbs[i+1] == maximum) {
            table[i+1] = 1;
        } else {
            table[i+2] = 1;
        }

        i += 3;
    }

    for (vector<Patient>::iterator it = testData.begin(); it != testData.end(); it++) {
        int j = 0;
        while (j < 48) {
            if (it->TRIMONO != probTable[j].first.TRIMONO) {
                j += 24;
            } else if (it->Sloepnea != probTable[j].first.Sloepnea) {
                j += 12;
            } else if (it->Foriennditis != probTable[j].first.Foriennditis) {
                j += 6;
            } else if (it->Degar != probTable[j].first.Degar) {
                j += 3;
            } else if (it->Dunetts == probTable[j].first.Dunetts) {
                if (table[j] == 1) {
                    hit++;
                } else {
                    miss++;
                }
                break;
            } else {
                j++;
            }
        }
    }

    return hit/(hit+miss);
}

double EStep() {
    double totalWeight = 0.0;
    for (vector<Patient>::iterator it = trainData.begin(); it != trainData.end(); it++) {
        int i = 0;
        double weight = 0.0;
        while (i < 48) {
            if (it->TRIMONO != probTable[i].first.TRIMONO) {
                i += 24;
            } else if (it->Sloepnea != probTable[i].first.Sloepnea) {
                i += 12;
            } else if (it->Foriennditis != probTable[i].first.Foriennditis) {
                i += 6;
            } else if (it->Degar != probTable[i].first.Degar) {
                i += 3;
            } else if (it->Dunetts == probTable[i].first.Dunetts) {
                weightSum[i] = weightSum[i] + 1;
                weight = probTable[i].second;
                break;
            } else {
                if (it->Dunetts == -1) {
                    for (int j = 0; j < 3; j++) {
                        weightSum[i+j] += normalizedProbs[i+j];
                    }
                    weight = max(max(probTable[i].second, probTable[i+1].second), probTable[i+2].second);
                    break;
                } else {
                    i++;
                }
            }
        }
        totalWeight += weight;
    }
    return totalWeight;
}

void MStep(double totalSum) {
    double sum_tri[2] = {0.0};
    double sum_slo[6] = {0.0};
    double sum_for[3] = {0.0};
    double sum_deg[3] = {0.0};
    double sum_dun[3] = {0.0};
    double sum_tri_dun[6] = {0.0};

    for (int i = 0; i < 48; i++) {
        Patient p = probTable[i].first;

        if (p.TRIMONO == 1) {
            sum_tri[0] += weightSum[i];
        } else {
            sum_tri[1] += weightSum[i];
        }

        if (p.Foriennditis == 1) {
            if (p.Dunetts == 0) {
                sum_for[0] += weightSum[i];
            } else if (p.Dunetts == 1) {
                sum_for[1] += weightSum[i];
            } else {
                sum_for[2] += weightSum[i];
            }
        }

        if (p.Degar == 1) {
            if (p.Dunetts == 0) {
                sum_deg[0] += weightSum[i];
            } else if (p.Dunetts == 1) {
                sum_deg[1] += weightSum[i];
            } else {
                sum_deg[2] += weightSum[i];
            }
        }

        if (p.Dunetts == 0) {
            sum_dun[0] += weightSum[i];
        } else if (p.Dunetts == 1) {
            sum_dun[1] += weightSum[i];
        } else {
            sum_dun[2] += weightSum[i];
        }

        if (p.Sloepnea == 1) {
            if (p.TRIMONO == 1) {
                if (p.Dunetts == 0) {
                    sum_slo[0] += weightSum[i];
                } else if (p.Dunetts == 1) {
                    sum_slo[1] += weightSum[i];
                } else {
                    sum_slo[2] += weightSum[i];
                }
            } else {
                if (p.Dunetts == 0) {
                    sum_slo[3] += weightSum[i];
                } else if (p.Dunetts == 1) {
                    sum_slo[4] += weightSum[i];
                } else {
                    sum_slo[5] += weightSum[i];
                }
            }
        }

        if (p.TRIMONO == 1) {
            if (p.Dunetts == 0) {
                sum_tri_dun[0] += weightSum[i];
            } else if (p.Dunetts == 1) {
                sum_tri_dun[1] += weightSum[i];
            } else {
                sum_tri_dun[2] += weightSum[i];
            }
        } else {
            if (p.Dunetts == 0) {
                sum_tri_dun[3] += weightSum[i];
            } else if (p.Dunetts == 1) {
                sum_tri_dun[4] += weightSum[i];
            } else {
                sum_tri_dun[5] += weightSum[i];
            }
        }
    }

    TRIMONO[0].second = sum_tri[0] / totalSum;
    TRIMONO[1].second = sum_tri[1] / totalSum;

    Dunetts[0].second = sum_dun[0] / totalSum;
    Dunetts[1].second = sum_dun[1] / totalSum;
    Dunetts[2].second = sum_dun[2] / totalSum;

    Foriennditis[0].second = sum_for[0] / sum_dun[0];
    Foriennditis[1].second = sum_for[1] / sum_dun[1];
    Foriennditis[2].second = sum_for[2] / sum_dun[2];

    Degar[0].second = sum_deg[0] / sum_dun[0];
    Degar[1].second = sum_deg[1] / sum_dun[1];
    Degar[2].second = sum_deg[2] / sum_dun[2];

    Sloepnea[0].second = sum_slo[0] / sum_tri_dun[0];
    Sloepnea[1].second = sum_slo[1] / sum_tri_dun[1];
    Sloepnea[2].second = sum_slo[2] / sum_tri_dun[2];
    Sloepnea[3].second = sum_slo[3] / sum_tri_dun[3];
    Sloepnea[4].second = sum_slo[4] / sum_tri_dun[4];
    Sloepnea[5].second = sum_slo[5] / sum_tri_dun[5];
}

void printCPTs() {
    cout << "Sloepnea" << endl;
    for (int i = 0;  i< 6; i++) {
        cout << Sloepnea[i].second << endl;
    }
    cout << "*************************" << endl;

    cout << "TRIMONO" << endl;
    for (int i = 0; i < 2; i++) {
        cout << TRIMONO[i].second << endl;
    }
    cout << "*************************" << endl;

    cout << "Dunetts" << endl;
    for (int i = 0; i < 3; i++) {
        cout << Dunetts[i].second << endl;
    }
    cout << "*************************" << endl;

    cout << "Foriennditis" << endl;
    for (int i = 0; i < 3; i++) {
        cout << Foriennditis[i].second << endl;
    }
    cout << "*************************" << endl;

    cout << "Degar" << endl;
    for (int i = 0; i < 3; i++) {
        cout << Degar[i].second << endl;
    }
    cout << "*************************" << endl;
}

int main(int argc, const char *argv[]) {
    double initTotalWeight = 0.0;
    double totalWeight = 0.0;
    double initAccuracy, accuracy;

    srand((unsigned)time(NULL));

    readData("traindata.txt");
    readData("testdata.txt");

    for (int i = 0; i < 20; i++) {

        double noise = 0.2 * (i+1);

        for (int j = 0; j < 20; j++) {

            int idx = 0;

            initCPTs();
            addNoise(noise);

            while (true) {
                initTotalWeight = totalWeight;
                totalWeight = 0.0;

                for (int k = 0; k < 48; k++) {
                    weightSum[k] = 0.0;
                }

                updateProbability();

                if (idx == 0) {
                    initAccuracy = testAccuracy();
                }

                totalWeight = EStep();

                MStep(2000);

                if (abs(totalWeight - initTotalWeight) < 0.01) {
                    break;
                }
                idx++;
            }

            accuracy = testAccuracy();
            cout << "Before EM: " << initAccuracy << endl;
            cout << "After EM: " << accuracy << endl;
            init_guess[i][j] = initAccuracy;
            EM_result[i][j] = accuracy;

        }
    }

    cout << "Final CPTs: " << endl;
    printCPTs();

    ofstream dataFile;
    dataFile.open("./data.csv");
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {

            if (j == 19) {
                dataFile << to_string(init_guess[i][j]) << endl;
            } else {
                dataFile << to_string(init_guess[i][j]) + ",";
            }
        }

    }

    dataFile << endl;
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {

            if (j == 19) {
                dataFile << to_string(EM_result[i][j]) << endl;
            } else {
                dataFile << to_string(EM_result[i][j]) + ",";
            }
        }
    }

    dataFile.close();

    double init_guess_mean[20];
    for (int i = 0; i < 20; i++) {
        double sum = 0.0;
        for (int j = 0; j < 20; j++) {
            sum += init_guess[i][j];
        }
        double mean=sum/20.0;
        init_guess_mean[i] = mean;
    }

    double EM_result_mean[20];
    for (int i = 0; i < 20; i++) {
        double sum = 0.0;
        for (int j = 0; j < 20; j++) {
            sum += EM_result[i][j];
        }
        double mean=sum/20.0;
        EM_result_mean[i] = mean;
    }

    ofstream resultFile;
    resultFile.open("./result.csv");
    for (int i = 0; i < 20; i++) {
        double variance = 0.0;
        double stdDeviation = 0.0;
        for (int j = 0; j < 20; j++) {
            variance += pow(init_guess[i][j] - init_guess_mean[i], 2);
        }
        variance = variance/20.0;
        stdDeviation = sqrt(variance);
        resultFile << to_string(init_guess_mean[i]) + "," << to_string(stdDeviation) + "," << to_string(0.2*(i+1)) << endl;
    }

    resultFile << endl;

    for (int i = 0; i < 20; i++) {
        double variance = 0.0;
        double stdDeviation = 0.0;
        for (int j = 0; j < 20; j++) {
            variance += pow(EM_result[i][j] - EM_result_mean[i], 2);
        }
        variance = variance/20.0;
        stdDeviation = sqrt(variance);
        resultFile << to_string(init_guess_mean[i]) + "," << to_string(stdDeviation) + "," << to_string(0.2*(i+1)) << endl;
    }

    resultFile.close();

    return 0;

}
