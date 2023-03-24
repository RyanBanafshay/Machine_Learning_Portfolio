#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <chrono>

using namespace std;

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

double comp_median(vector<double> vect){
    double median;
    //ascending order
    sort(vect.begin(), vect.end());

    if(vect.size() % 2 == 0)
        //even size -> get average of two middle values
        median = (vect[(vect.size() / 2) - 1] + vect[vect.size() / 2] / 2);
    else
        median = vect[vect.size() / 2];

    return median;
}

double comp_range(vector<double> vect){
    double range;
    //ascending order to get first and last element
    sort(vect.begin(), vect.end());

    double min = vect[0];
    double max = vect[vect.size() - 1];

    range = max - min;
    return range;
}

double comp_covariance(vector<double> vect1, vector<double> vect2){
    //covariance is the sum of the products of each set's elements minus the set's mean, all divided by the set size minus one
    double covariance = 0;
    double mean_one = comp_mean(vect1);
    double mean_two = comp_mean(vect2);

    //both vectors are assumed to be same size
    for(int i = 0; i < vect1.size(); i++)
        covariance += (vect1[i] - mean_one) * (vect2[i] - mean_two);
    
    covariance = covariance / (vect1.size() - 1);

    return covariance;
}

double comp_correlation(vector<double> vect1, vector<double> vect2){
    //correlation is the covariance between two sets divided by the product of both standard deviations
    double correlation = 0;

    double covariance = comp_covariance(vect1, vect2);

    double std_dev_one = sqrt(comp_covariance(vect1, vect1));
    double std_dev_two = sqrt(comp_covariance(vect2, vect2));

    correlation = covariance / (std_dev_one * std_dev_two);

    return correlation;
}

double sigmoid(double val){
    val = 1 / (1 + exp(-val));
    return val;
}

void print_feature_stats(vector<double> vect){

    cout << "Sum: " << comp_sum(vect) << endl
        << "Mean: " << comp_mean(vect) << endl
        << "Median: " << comp_median(vect) << endl
        << "Range: " << comp_range(vect) << endl;

    return;
}

void print_observation_stats(vector<double> vect1, vector<double> vect2){

    cout << "Covariance: " << comp_covariance(vect1, vect2) << endl
        << "Correlation: " << comp_correlation(vect1, vect2) << endl;

    return;
}


void print_vector(vector<double> vect){
    for(double val : vect)
        cout << val << " ";
    cout << endl << "---------" << endl;
    
    return;
}

void print_metrics(vector<double> predictions, vector<double> test_label_matrix, int test_size){
    //determine the accuracy
    double mean_acc = 0;
    for (int i = 0; i < test_size; i++){
        if (predictions[i] == test_label_matrix[i])
            mean_acc++;
    }

    mean_acc = mean_acc / test_size;

    //determine the sensitivity and specificity
    double true_pos = 0;
    double true_neg = 0;
    double false_pos = 0;
    double false_neg = 0;

    for (int i = 0; i < test_size; i++){
        if (predictions[i] == 1 && test_label_matrix[i] == 1)
            true_pos++;
        else if (predictions[i] == 0 && test_label_matrix[i] == 0)
            true_neg++;
        else if (predictions[i] == 1 && test_label_matrix[i] == 0)
            false_pos++;
        else if (predictions[i] == 0 && test_label_matrix[i] == 1)
            false_neg++;
    }

    double sensitivity = true_pos / (true_pos + false_neg);
    double specificity = true_neg / (true_neg + false_pos);

    cout << "\nCorrectly Predicted Survived: " << true_pos << endl
         << "Correctly Predicted Dead: " << true_neg << endl
         << "Incorrectly Predicted Survived: " << false_pos << endl
         << "Incorrectly Predicted Dead: " << false_neg << endl;

    cout << "\nSensitivity: " << sensitivity << endl
         << "Specificity: " << specificity << endl
         << "Accuracy: " << mean_acc << endl;
}

int main(int argc, char** argv){

    //read in csv
    ifstream in_file("titanic_project.csv");
    string token;
    //vector<double> age_vect(0);
    vector<double> sex_vect(0);
    vector<double> survived_vect(0);
    //vector<double> passenger_class_vect(0);
    //vector<double> passenger_num_vect(0);

    const int train_size = 800;
    const int test_size = 247;
    const double learning_rate = 0.001;
    vector<double> data_matrix(train_size);
    vector<double> label_matrix(train_size);
    vector<double> probability_vect(train_size);
    vector<double> error_matrix(train_size);
    vector<double> weights_vect(1);



    //get predictors
    if(!in_file.is_open()){
        cout << "Error opening file" << endl;
        return 1;
    }

    string first_line;
    getline(in_file, first_line);
    //cout << "Header: " << first_line << endl;

    int observation_count = 0;

    while(!in_file.eof()){
        //only use sex as predictor for survival
        //put predictor as vector
        
        getline(in_file, token, ',');
        //passenger_num_vect.push_back(stod(token));
        
        getline(in_file, token, ',');
        //passenger_class_vect.push_back(stod(token));
        
        getline(in_file, token, ',');
        survived_vect.push_back(stod(token));
        
        getline(in_file, token, ',');
        sex_vect.push_back(stod(token));
        
        getline(in_file, token, '\n');
        //age_vect.push_back(stod(token));

        observation_count++;
    }

    //print_vector(survived_vect);

    sex_vect.resize(observation_count);
    survived_vect.resize(observation_count);
    //age_vect.resize(observation_count);
    //passenger_class_vect.resize(observation_count);
    //passenger_num_vect.resize(observation_count);

    for (int i = 0; i < train_size; i++){
        data_matrix[i] = sex_vect[i];
        label_matrix[i] = survived_vect[i];
    }

    weights_vect[0] = 1;

    //output coefficients
    print_observation_stats(survived_vect, sex_vect);

    auto start = chrono::steady_clock::now();
    for (int epoch = 0; epoch < 500; epoch++){
        //use first 800 observations for training
        vector<double> data_error_vect(train_size);
        for (int i = 0; i < train_size; i++){
            probability_vect[i] = sigmoid(weights_vect[0] * data_matrix[i]);
            error_matrix[i] = label_matrix[i] - probability_vect[i];
            data_error_vect[i] = error_matrix[i] * data_matrix[i];

        }
        weights_vect[0] += learning_rate * comp_sum(data_error_vect);
    }
    auto end = chrono::steady_clock::now();

    cout << "Time to train: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
    
    cout << "Weight: " << weights_vect[0] << endl;
    
    double men_survived, men_dead, women_survived, women_dead = 0;

    //use the rest of the obervations (247) for testing
    for (int i = 0; i < 247; i++){
        if (survived_vect[i+799] == 1 && sex_vect[i+799] == 0)
            men_survived++;
        else if (survived_vect[i+799] == 0 && sex_vect[i+799] == 0)
            men_dead++;
        else if (survived_vect[i+799] == 1 && sex_vect[i+799] == 1)
            women_survived++;
        else if (survived_vect[i+799] == 0 && sex_vect[i+799] == 1)
            women_dead++;
    }

    cout << "\nTest Data Stats:\n"
         << "Men survived: " << men_survived << endl
         << "Men dead: " << men_dead << endl
         << "Women survived: " << women_survived << endl
         << "Women dead: " << women_dead << endl;

    //test our predictions from training data
    vector<double> test_matrix(test_size);
    vector<double> test_label_matrix(test_size);
    vector<double> test_probability_vect(test_size);
    vector<double> test_predicted(test_size);
    vector<double> predictions(test_size);
    double mean_acc = 0;

    for (int i = 0; i < test_size; i++){
        test_matrix[i] = sex_vect[i+799];
        test_label_matrix[i] = survived_vect[i+799];
    }

    //determine the predictions
    for (int i = 0; i < test_size; i++){
        test_predicted[i] = test_matrix[i] * weights_vect[0];
        test_probability_vect[i] = exp(test_predicted[i]) / (1 + exp(test_predicted[i]));
        if (test_probability_vect[i] >= 0.5)
            predictions[i] = 1;
        else
            predictions[i] = 0;
    }

    print_metrics(predictions, test_label_matrix, test_size);

    return 0;
}