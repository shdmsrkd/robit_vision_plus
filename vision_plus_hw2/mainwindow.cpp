// a = -0.321, b = 296.489
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

void MainWindow::drawRansacLine(const vector<vector<double>>& points)
{
    double sum_x = 0;
    double sum_y = 0;
    double sum_xy = 0;
    double sum_xx = 0;
    int n = points.size();

    // 데이터 포인트들의 합 계산
    for(const auto& point : points)
    {
        double x = point[0] * 100;  // 스케일링 적용
        double y = point[1] * 1;    // 스케일링 적용

        sum_x += x;
        sum_y += y;
        sum_xy += x * y;
        sum_xx += x * x;
    }

    // 최소제곱법으로 기울기와 절편 계산
    double slope = (n * sum_xy - sum_x * sum_y) / (n * sum_xx - sum_x * sum_x);
    double intercept = (sum_y - slope * sum_x) / n;

    // 결정계수(R²) 계산
    double mean_y = sum_y / n;
    double ss_tot = 0;  // 총 제곱합
    double ss_res = 0;  // 잔차 제곱합

    for(const auto& point : points) {
        double x = point[0] * 100;
        double y = point[1] * 1;
        double predicted_y = slope * x + intercept;

        ss_tot += pow(y - mean_y, 2);
        ss_res += pow(y - predicted_y, 2);
    }

    double r_squared = 1 - (ss_res / ss_tot);

    // 결과 출력
    cout << "최소제곱법 결과:" << endl;
    cout << "직선의 방정식: y = " << slope << "x + " << intercept << endl;
    cout << "결정계수(R²): " << r_squared << endl;

    // 직선 그리기
    QPen linePen(Qt::red);
    linePen.setWidth(2);

    double x1 = -5000;
    double x2 = 5000;
    double y1 = slope * x1 + intercept;
    double y2 = slope * x2 + intercept;

    scene->addLine(x1, -y1, x2, -y2, linePen);
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

    // 여백 더 증가
    QRectF rect(minX - 200, -maxY - 200, maxX - minX + 400, maxY - minY + 400);
    view->fitInView(rect, Qt::KeepAspectRatio);
}
