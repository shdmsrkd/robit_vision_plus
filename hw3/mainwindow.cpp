// 먼저 시각화 과정을 통해 데이터가 어떤식으로 분포해 있는지 파악해서 k값을 대충 어떤 식으로 잡아야 할지 감을 잡고
// k값을 조작해보면서 진행했다.
// k값을 너무 큰 값으로 잡았을 경우 너무 많은 군집이 생겨서 변별이 안되는 느낌이었고,
// k값을 너무 작은 값으로 작았을 때에는 군집이 하나로 뭉쳐져서 데이터가 일반화 된 느낌이어서



#include "mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene);
    setCentralWidget(view);

    loadAndPlotData();
}

MainWindow::~MainWindow()
{
}

double MainWindow::getDistance(const Point& p1, const Centroid& p2)
{
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

void MainWindow::performKMeansClustering(const std::vector<std::vector<double>>& points)
{
    std::vector<Point> data_points;
    for (const auto& p : points)
    {
        data_points.emplace_back(p[0] * 100, p[1] * 50);
    }

    std::vector<Centroid> centroids;
    for (int i = 0; i < K; i++)
    {
        int idx = rand() % data_points.size();
        centroids.emplace_back(data_points[idx].x, data_points[idx].y);
    }

    bool changed = true;
    int iteration = 0;

    while (changed && iteration < MAX_ITERATIONS)
    {
        changed = false;

        for (auto& point : data_points)
        {
            int closest_cluster = 0;
            double min_distance = getDistance(point, centroids[0]);

            for (int i = 1; i < K; i++)
            {
                double distance = getDistance(point, centroids[i]);
                if (distance < min_distance)
                {
                    min_distance = distance;
                    closest_cluster = i;
                }
            }

            if (point.cluster != closest_cluster)
            {
                point.cluster = closest_cluster;
                changed = true;
            }
        }

        std::vector<int> cluster_sizes(K, 0);
        std::vector<double> sum_x(K, 0.0);
        std::vector<double> sum_y(K, 0.0);

        for (const auto& point : data_points)
        {
            cluster_sizes[point.cluster]++;
            sum_x[point.cluster] += point.x;
            sum_y[point.cluster] += point.y;
        }

        for (int i = 0; i < K; i++)
        {
            if (cluster_sizes[i] > 0)
            {
                centroids[i].x = sum_x[i] / cluster_sizes[i];
                centroids[i].y = sum_y[i] / cluster_sizes[i];
            }
        }

        iteration++;
    }

    plotClusters(data_points, centroids);
}

void MainWindow::plotClusters(const std::vector<Point>& points, const std::vector<Centroid>& centroids)
{
    QColor colors[3] = {Qt::red, Qt::blue, Qt::green};

    for (const auto& point : points)
    {
        QPen pen(colors[point.cluster]);
        pen.setWidth(10);
        scene->addEllipse(point.x-5, -point.y-5, 10, 10, pen, QBrush(colors[point.cluster]));
    }

    QPen centroidPen(Qt::black);
    centroidPen.setWidth(15);
    for (const auto& centroid : centroids)
    {
        scene->addEllipse(centroid.x-7.5, -centroid.y-7.5, 15, 15, centroidPen, QBrush(Qt::black));
    }
}

void MainWindow::loadAndPlotData()
{
    string myFilePath = "/home/eungang/Downloads/TalkFile_cluster_data_.csv.csv";
    ifstream cluster_data(myFilePath);
    vector<vector<double>> change_datas;
    string line;
    int count = 0;

    scene->clear();

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

        change_datas.push_back(row);
    }

    cluster_data.close();

    performKMeansClustering(change_datas);
}
