// a = -0.48326 b = 435.036
// 반복횟수가 너무 적으면 이상한 선이 잡히는 경우가 많고, 반복횟수가 너무 많으면 연산 속도가 느려지기 때문에
// 여러번 디버깅 해도 이상한 선이 잡히지 않을 때까지의 수를 반복횟수로 정했고,

// inlier 허용 거리는 의도적으로 있는 노이즈 값을 피하는 것을 기준으로 값을 잡았다.

// mainwindow.cpp
#include "mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QMessageBox>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Data Plot");
    setGeometry(100, 100, 800, 600);

    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene);
    setCentralWidget(view);

    loadAndPlotData();
}

MainWindow::~MainWindow()
{
}

MainWindow::LineModel MainWindow::fitLine(const vector<double>& x, const vector<double>& y)
{
    double x1 = x[0], x2 = x[1];
    double y1 = y[0], y2 = y[1];

    double slope = (y2 - y1) / (x2 - x1);
    double intercept = y1 - slope * x1;

    return LineModel{slope, intercept};
}

double MainWindow::getDistance(double x, double y, const LineModel& model)
{
    return abs(y - (model.slope * x + model.intercept)) /
           sqrt(1 + model.slope * model.slope);
}

void MainWindow::drawRansacLine(const std::vector<std::vector<double>>& points) {
    int best_inliers = 0;
    double best_slope = 0;
    double best_intercept = 0;

    for(int i = 0; i < 1000; i++) {
        int idx1 = rand() % points.size();
        int idx2 = rand() % points.size();

        if(idx1 == idx2) continue;

        double x1 = points[idx1][0] * 100;
        double y1 = points[idx1][1] * 1;
        double x2 = points[idx2][0] * 100;
        double y2 = points[idx2][1] * 1;

        double slope = (y2 - y1) / (x2 - x1);
        double intercept = y1 - (slope * x1);

        int inliers = 0;
        for(const auto& point : points)
        {
            double x = point[0] * 100;
            double y = point[1] * 1;

            double distance = std::abs(y - (slope * x + intercept));

            if(distance < 100.0)
            {
                inliers++;
            }
        }
        if(inliers > best_inliers) {
            best_inliers = inliers;
            best_slope = slope;
            best_intercept = intercept;
        }
    }

    QPen linePen(Qt::red);
    linePen.setWidth(2);

    double x1 = -5000;
    double x2 = 5000;
    double y1 = best_slope * x1 + best_intercept;
    double y2 = best_slope * x2 + best_intercept;

    scene->addLine(x1, -y1, x2, -y2, linePen);

    std::cout << "직선의 방정식: y = " << best_slope << "x + " << best_intercept << std::endl;
    std::cout << "직선과 가까운 점의 개수: " << best_inliers << std::endl;
}

void MainWindow::loadAndPlotData()
{
    string myFilePath = "/home/eungang/Downloads/TalkFile_First-order function.csv.csv";
    ifstream cluster_data(myFilePath);
    vector<vector<double>> change_datas;
    string line;

    scene->clear();

    QPen pen(Qt::blue);
    pen.setWidth(10);

    QPen axisPen(Qt::black);
    axisPen.setWidth(2);
    scene->addLine(-10000, 0, 10000, 0, axisPen);
    scene->addLine(0, -10000, 0, 10000, axisPen);

    double minX = 0, maxX = 0, minY = 0, maxY = 0;
    bool firstPoint = true;
    int count = 0;

    while (getline(cluster_data, line))
    {
        if (count == 0)
        {
            count = 1;
            continue;
        }

        stringstream ss(line);
        string record;
        vector<double> row;

        while (getline(ss, record, ','))
        {
            double value = stod(record);
            row.push_back(value);
        }

        if (row.size() >= 2)
        {

            double x = row[0] * 100;
            double y = row[1] * 1;

            scene->addEllipse(x-5, -y-5, 10, 10, pen, QBrush(Qt::blue));

            if (firstPoint)
            {
                minX = maxX = x;
                minY = maxY = y;
                firstPoint = false;
            }
            else
            {
                minX = min(minX, x);
                maxX = max(maxX, x);
                minY = min(minY, y);
                maxY = max(maxY, y);
            }
        }
        change_datas.push_back(row);
    }

    cluster_data.close();

    drawRansacLine(change_datas);

    view->setRenderHint(QPainter::Antialiasing);

    QRectF rect(minX - 200, -maxY - 200, maxX - minX + 400, maxY - minY + 400);
    view->fitInView(rect, Qt::KeepAspectRatio);
}
