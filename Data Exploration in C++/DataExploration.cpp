#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <math.h>
using namespace std;

double sum(vector<double> data) {
    double sum = 0;
    for (int i = 0; i < data.size(); i++) {
        sum += data[i];
    }

    return sum;
}

double mean(vector<double> data) {
    double sumForMean = sum(data);

    double denom = data.size();

    double mean = sumForMean / denom;

    return mean;
}

double median(vector<double> data) {
    sort(data.begin(), data.end());

    int medianLocation = (data.size() / 2);
    double median = data[medianLocation];

    return median;
}

double range(vector<double> data) {
    double min = 10000000000, max = -100000000000;
    for (int i = 1; i <= data.size(); i++) {
        if (data[i] < min)
            min = data[i];
        if (data[i] > max) {
            max = data[i];
        }
    }

    double range = max - min;
    return range;
}

double covar(vector<double> dataX, vector<double> dataY) {
    double meanX = mean(dataX);
    double meanY = mean(dataY);

    double denom = dataX.size() - 1;

    double num = 0;
    for (int i = 0; i <= dataX.size(); i++) {
        num += (dataX[i] - meanX) * (dataY[i] - meanY);
    }

    double covariance = num / denom;
    return covariance;

}

double cor(vector<double> dataX, vector<double> dataY) {
    double covarianceX = covar(dataX, dataX);
    double sigmaX = sqrt(covarianceX);
    double covarianceY = covar(dataY, dataY);
    double sigmaY = sqrt(covarianceY);

    double covarianceXY = covar(dataX, dataY);

    double correlation = covarianceXY / (sigmaX * sigmaY);
    return correlation;

}

void print_stats(vector<double> data) {
    double sumF, meanF, medianF, rangeF = 0;

    sumF = sum(data);
    meanF = mean(data);
    medianF = median(data);
    rangeF = range(data);

    cout << "\nSum = " << sumF;
    cout << "\nMean = " << meanF;
    cout << "\nMedian = " << medianF;
    cout << "\nRange = " << rangeF << endl;
}

int main(int argc, char** argv) {

    ifstream inFS;
    string line;
    string rm_in, medv_in;
    const int MAX_LEN = 1000;
    vector<double> rm(MAX_LEN);
    vector<double> medv(MAX_LEN);

    cout << "Opening file Boston.csv" << endl;

    inFS.open("Boston.csv");
    if (!inFS.is_open()) {
        cout << "Could not open file Boston.csv" << endl;
        return 1;
    }

    cout << "Reading line 1" << endl;
    getline (inFS, line);
    // echo heading
    cout << "heading: " << line << endl;

    int numObservations = 0;
    while (inFS.good()) {
        getline (inFS, rm_in, ',');
        getline (inFS, medv_in, '\n');

        rm.at (numObservations) = stof(rm_in);
        medv.at (numObservations) = stof (medv_in);

        numObservations++;
    }

    rm.resize(numObservations);
    medv.resize(numObservations);

    cout << "new length " << rm.size() << endl;

    cout << "Closing file Boston.csv." << endl;
    inFS.close(); // Done with file, so close it

    cout << "Number of records: " << numObservations << endl;

    cout << "\nStats for rm" << endl;
    print_stats(rm);

    cout << "\nStats for medv" << endl;
    print_stats(medv);

    cout << "\n Covariance = " << covar(rm, medv) << endl;

    cout << "\n Correlation = " << cor(rm, medv) << endl;

    cout << "\nProgram terminated.";

    return 0;

}

