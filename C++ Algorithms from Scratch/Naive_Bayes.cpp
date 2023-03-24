#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <chrono>
#include <typeinfo>

using namespace std;

double calc_age_lh(double v, double mean_v, double var_v){
    return (1 / sqrt(2 * M_PI * var_v) * exp(-(pow(v - mean_v, 2)) / (2 * var_v)));
}


vector<double> calc_raw_prob(
    int pclass, int sex, double age, double passenger_class_vect[2][3], double conditionalSex[2][2],
    vector<double> ap, vector<double> ageMean, vector<double> ageVariance)
{
    double num_s = passenger_class_vect[1][pclass - 1] * conditionalSex[1][sex - 1] * ap[1] * calc_age_lh(age, ageMean[1], ageVariance[1]);
    double num_p = passenger_class_vect[0][pclass - 1] * conditionalSex[0][sex - 1] * ap[0] * calc_age_lh(age, ageMean[0], ageVariance[0]);

    double denominator = passenger_class_vect[1][pclass - 1] * conditionalSex[1][sex - 1] * calc_age_lh(age, ageMean[1], ageVariance[1]) * ap[1] +
        passenger_class_vect[0][pclass - 1] * conditionalSex[0][sex - 1] * calc_age_lh(age, ageMean[0], ageVariance[0]) * ap[0];

    vector<double> result(2);
    result[0] = num_s / denominator;
    result[1] = num_p / denominator;
    return result;
}

vector<double> vectorSplit(vector<double> a, int l, int r)
{
    int n = a.size();
    vector<double> x;
    x.resize(r - l);
    int j = 0;
    for (int i = l; i <= r; i++){
        x[j] = a[i];
        j++;
    }
    return x;
}

void print_vector(vector<double> vect){
    for(double val : vect)
        cout << val << " ";
    cout << endl << "---------" << endl;

    return;
}

double comp_sum(vector<double> vect){
    double sum = 0;

    for (double val: vect)
        sum += val;

    return sum;
}

double comp_mean(vector<double> vect){
    double sum = comp_sum(vect);
    double mean = sum / vect.size();

    return mean;
}

//Calculate apriori for 0 based on training data
double printApriori0(vector<double> vect) {
    double num = 0;
    for (int i = 0; i < 800; i++) {
        if (vect[i] == 0)
            num++;
    }
    double prob = (num / 800);

    return prob;
}

//Calculate apriori for 1 based on training data
double printApriori1(vector<double> vect) {
    double num = 0;
    for (int i = 0; i < 800; i++) {
        if (vect[i] == 1)
            num++;
    }
    double prob = (num / 800);

    return prob;
}

int main() {
    //read in csv
    ifstream in_file("titanic_project.csv");
    string token;
    vector<double> age_vect(0);
    vector<double> sex_vect(0);
    vector<double> survived_vect(0);
    vector<double> passenger_class_vect(0);

    const int train_size = 800;
    const int test_size = 247;

//****************************************************************************************************************************************************************************************************

    if(!in_file.is_open()){
        cout << "Error opening file" << endl;
        return 1;
    }

    string first_line;
    getline(in_file, first_line);
    //cout << "Header: " << first_line << endl;

    int observation_count = 0;

    while(!in_file.eof()){
        getline(in_file, token, ',');
        //passenger_num_vect.push_back(stod(token));

        getline(in_file, token, ',');
        passenger_class_vect.push_back(stod(token));

        getline(in_file, token, ',');
        survived_vect.push_back(stod(token));

        getline(in_file, token, ',');
        sex_vect.push_back(stod(token));

        getline(in_file, token, '\n');
        age_vect.push_back(stod(token));

        observation_count++;
    }

    //print_vector(passenger_class_vect);

    sex_vect.resize(observation_count);
    survived_vect.resize(observation_count);
    age_vect.resize(observation_count);
    passenger_class_vect.resize(observation_count);
    //passenger_num_vect.resize(observation_count);

    // train vectors
    vector<double> pclass_train = vectorSplit(passenger_class_vect, 0, 800);
    vector<double> survived_train = vectorSplit(survived_vect, 0, 800);
    vector<double> sex_train = vectorSplit(sex_vect, 0, 800);
    vector<double> age_train = vectorSplit(age_vect, 0, 800);

    vector<vector<double>> train = {pclass_train, survived_train, sex_train, age_train};

    // test vectors
    vector<double> pclass_test = vectorSplit(passenger_class_vect, 801, passenger_class_vect.size() - 1);
    vector<double> survived_test = vectorSplit(survived_vect, 801, survived_vect.size() - 1);
    vector<double> sex_test = vectorSplit(sex_vect, 801, sex_vect.size() - 1);
    vector<double> age_test = vectorSplit(age_vect, 801, age_vect.size() - 1);

    vector<vector<double>> test = {pclass_test, survived_test, sex_test, age_test};



//calculate Apriori based on training data***********************************************************************************************************************************************************
    vector<double> apriori;

    double apr0 = printApriori0(survived_vect);
    double apr1 = printApriori1(survived_vect);

    //print results
    cout << "A-priori probabilities: " << endl;
    cout << "   0               1" << endl;
    cout << apr0 << "            " << apr1 << endl;

    apriori.push_back(apr0);
    apriori.push_back(apr1);



//calculate conditional probabilities for qualitative data********************************************************************************************************************************************

    //receive number of people survive and died
    double numDied, numSurvived;
    for (double i : survived_vect) {
        if (survived_vect[i] == 0)
            numDied++;
    }
    for (double i : survived_vect) {
        if (survived_vect[i] == 1)
            numSurvived++;
    }

    //cout << numDied << endl;
    //cout << numSurvived << endl;

    //3d vector for passenger class
    double conditionalPassengerClass[2][3];

    //receive probability from each survivor class based on survived or died
    double class1Dead = 0, class2Dead = 0, class3Dead = 0, class1Survived = 0, class2Survived = 0, class3Survived = 0;

    for (int i = 0; i < 1046; i++) {
        if (passenger_class_vect[i] == 1 && survived_vect[i] == 0) {
           class1Dead++;
        }
    }

    conditionalPassengerClass[0][0] = class1Dead / numDied;

    for (int i = 0; i < 1046; i++) {
        if (passenger_class_vect[i] == 2 && survived_vect[i] == 0) {
           class2Dead++;
        }
    }

    conditionalPassengerClass[0][1] = class2Dead / numDied;

    for (int i = 0; i < 1046; i++) {
        if (passenger_class_vect[i] == 3 && survived_vect[i] == 0) {
           class3Dead++;
        }
    }

    conditionalPassengerClass[0][2] = class3Dead / numDied;;

    for (int i = 0; i < 1046; i++) {
        if (passenger_class_vect[i] == 1 && survived_vect[i] == 1) {
           class1Survived++;
        }
    }

    conditionalPassengerClass[1][0] = class1Survived / numSurvived;

    for (int i = 0; i < 1046; i++) {
        if (passenger_class_vect[i] == 2 && survived_vect[i] == 1) {
           class2Survived++;
        }
    }

    conditionalPassengerClass[1][1] = class2Survived / numSurvived;

    for (int i = 0; i < 1046; i++) {
        if (passenger_class_vect[i] == 3 && survived_vect[i] == 1) {
           class3Survived++;
        }
    }

    conditionalPassengerClass[1][2] = class3Survived / numSurvived;


    //receive probability from each sex based on survived or died

    double conditionalSex[2][2];
    double sex0Dead = 0, sex1Dead = 0, sex0Survived = 0, sex1Survived = 0;


    for (int i = 0; i < 1046; i++) {
        if (sex_vect[i] == 0 && survived_vect[i] == 0) {
           sex0Dead++;
        }
    }

     conditionalSex[0][0] = (sex0Dead / numDied);

    for (int i = 0; i < 1046; i++) {
        if (sex_vect[i] == 0 && survived_vect[i] == 1) {
           sex0Survived++;
        }
    }

    conditionalSex[1][0] = (sex0Survived / numSurvived);

    for (int i = 0; i < 1046; i++) {
        if (sex_vect[i] == 1 && survived_vect[i] == 0) {
           sex1Dead++;
        }
    }

    conditionalSex[0][1] = (sex1Dead / numDied);

    for (int i = 0; i < 1046; i++) {
        if (sex_vect[i] ==1 && survived_vect[i] == 1) {
           sex1Survived++;
        }
    }

    conditionalSex[1][1] = (sex1Survived / numSurvived);

    //print results

    cout << "\nConditional Probablities:" << endl;
    cout << endl << "pclass:\n1          2       3:" << endl;

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++)
            cout << conditionalPassengerClass[i][j] << " ";
        cout << endl;
    }

    cout << endl << "Sex:\nFemale       Male:" << endl;

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++)
            cout << conditionalSex[i][j] << " ";
        cout << endl;
    }


//calculate conditional probabilities for quantitative data (age) ********************************************************************************************************************************************
    vector<double> ageMean, ageVariance;

    //calculate 0 age mean
    double sum = 0, denominator = 0;
    for (int i = 0; i < 1046; i++) {
        if(survived_vect[i] == 0){
            sum += age_vect[i];
            denominator++;
        }
    }

    ageMean.push_back(sum / denominator);

    //calculate 1 age mean
    sum = 0, denominator = 0;
    for (int i = 0; i < 1046; i++) {
        if(survived_vect[i] == 1){
            sum += age_vect[i];
            denominator++;
        }
    }
    ageMean.push_back(sum / denominator);

    //calculate 0 variance
    double sqDiff = 0;
    for (int i = 0; i < 1046; i++) {
        if(survived_vect[i] == 0) {
            sqDiff += (age_vect[i] - ageMean[0]) * (age_vect[i] - ageMean[0]);
        }
    }

    ageVariance.push_back(sqDiff / numDied);

    //calculate 1 variance
    sqDiff = 0;
    for (int i = 0; i < 1046; i++) {
        if(survived_vect[i] == 1) {
            sqDiff += (age_vect[i] - ageMean[1]) * (age_vect[i] - ageMean[1]);
        }
    }

    ageVariance.push_back(sqDiff / numSurvived);

    //print results
    cout << "\nStats for Age: " << endl;
    cout << "Mean: " << ageMean[0] << "     " << ageMean[1] << endl;
    cout << "Variance: " << ageVariance[0] << "    " << ageVariance[1] << endl;


//calculate Naive Bayes **************************************************************************************************************************************************************************************


    cout << "\nApplied to first 10 test observations:\n" << endl;

    auto start = chrono::steady_clock::now();

    vector<double> raw(2);
    for (int i = 0; i < 10; i++){
        raw = calc_raw_prob(test[0][i], test[2][i], test[3][i], conditionalPassengerClass, conditionalSex, apriori, ageMean, ageVariance);
        cout << raw[0] << " " << raw[1] << endl;
    }

    auto end = chrono::steady_clock::now();

    cout << "\nElapsed time in milliseconds: "
        << chrono::duration_cast<chrono::milliseconds>(end - start).count()
        << " ms" << endl;

}
